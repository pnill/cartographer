#include "stdafx.h"
#include "rasterizer_dx9_targets.h"

#include "rasterizer_dx9_main.h"

#include "camera/camera.h"
#include "main/main_screenshot.h"
#include "rasterizer/rasterizer_globals.h"

/* constants */

// This is no longer used
// #define USE_CUBEMAP_TARGETS

/* globals */

real32 g_sun_size = 0.2f;

IDirect3DTexture9* g_backbuffer_texture = NULL;
// IDirect3DSurface9* g_backbuffer_surface = NULL;

// IDirect3DSurface9* global_d3d_surface_render_primary = NULL;
// IDirect3DSurface9* global_d3d_surface_render_primary_z = NULL;

// IDirect3DSurface9* global_d3d_surface_render_z_as_target_z = NULL;

IDirect3DTexture9* global_d3d_texture_render_resolved = NULL;
// IDirect3DSurface9* global_d3d_surface_render_resolved = NULL;

IDirect3DTexture9* global_d3d_texture_sun_glow_primary = NULL;
// IDirect3DSurface9* global_d3d_surface_sun_glow_primary = NULL;
IDirect3DTexture9* global_d3d_texture_sun_glow_secondary = NULL;
IDirect3DSurface9* global_d3d_surface_sun_glow_secondary = NULL;

IDirect3DTexture9* global_d3d_texture_motion_sensor = NULL;
IDirect3DSurface9* global_d3d_surface_motion_sensor = NULL;

// IDirect3DPixelShader9* global_d3d_shader_white_source = NULL;

bool global_d3d_target_use_depth = false;

/* prototypes */

D3DMULTISAMPLE_TYPE* d3d_multisample_type_get(void);
uint32* d3d_multisample_quality_get(void);

IDirect3DSurface9** global_d3d_surface_render_z_as_target_z_get(void);
IDirect3DSurface9** g_backbuffer_surface_get(void);
IDirect3DPixelShader9** global_d3d_shader_white_source_get(void);

uint32* cubemap_target_size_get(void);
uint32* motion_sensor_texture_size_get(void);
IDirect3DTexture9* rasterizer_dx9_get_target_texture(e_rasterizer_target rasterizer_target);
IDirect3DSurface9* __cdecl rasterizer_dx9_get_target_mip_surface(e_rasterizer_target target, int16 mip);
void __cdecl rasterizer_set_render_target_internal_hook_set_main_render_surface(IDirect3DSurface9* target, IDirect3DSurface9* z_stencil, bool a3);

bool __cdecl rasterizer_dx9_primary_targets_initialize(void);
void __cdecl rasterizer_dx9_primary_targets_dispose(void);
bool __cdecl rasterizer_dx9_secondary_targets_initialize(void);
void __cdecl rasterizer_dx9_secondary_targets_dispose(void);


/* public code */

void rasterizer_dx9_targets_apply_patches(void)
{
    WriteJmpTo(Memory::GetAddress(0x25FDC0), rasterizer_dx9_set_target);
    WriteJmpTo(Memory::GetAddress(0x260256), rasterizer_dx9_set_target_as_texture);
    WriteJmpTo(Memory::GetAddress(0x26EBF8), rasterizer_dx9_set_render_target_internal);

    // fix the missing motion sensor blip
    PatchCall(Memory::GetAddress(0x284BF3), rasterizer_set_render_target_internal_hook_set_main_render_surface);

    WriteJmpTo(Memory::GetAddress(0x25F902), rasterizer_dx9_primary_targets_initialize);
    WriteJmpTo(Memory::GetAddress(0x2628EB), rasterizer_dx9_secondary_targets_initialize);

    WriteJmpTo(Memory::GetAddress(0x25FAC1), rasterizer_dx9_primary_targets_dispose);
    WriteJmpTo(Memory::GetAddress(0x262B00), rasterizer_dx9_secondary_targets_dispose);

    WriteJmpTo(Memory::GetAddress(0x25FB67), rasterizer_dx9_get_render_target_surface);
    return;
}

e_rasterizer_target* rasterizer_dx9_main_render_target_get(void)
{
	return Memory::GetAddress<e_rasterizer_target*>(0xA3E228);
}

s_rasterizer_target* rasterizer_dx9_texture_target_get(e_rasterizer_target texture_target)
{
	ASSERT(VALID_INDEX(texture_target, k_rasterizer_target_count));
	return &Memory::GetAddress<s_rasterizer_target*>(0xA8D658)[texture_target];
}

void rasterizer_dx9_copy_target(e_rasterizer_target src_target, e_rasterizer_target dst_target)
{
    IDirect3DDevice9Ex* global_d3d_device = rasterizer_dx9_device_get_interface();

    IDirect3DSurface9* dst_surface = rasterizer_dx9_target_get_main_mip_surface(dst_target);
    IDirect3DSurface9* src_surface = rasterizer_dx9_target_get_main_mip_surface(src_target);
    global_d3d_device->StretchRect(src_surface, NULL, dst_surface, NULL, D3DTEXF_POINT);
    return;
}

void __cdecl rasterizer_dx9_texture_target_surface_size(e_rasterizer_target target, int32* out_width, int32* out_height)
{
	return INVOKE(0x280413, 0x0, rasterizer_dx9_texture_target_surface_size, target, out_width, out_height);
}

IDirect3DSurface9* rasterizer_dx9_target_get_main_mip_surface(e_rasterizer_target rasterizer_target)
{
    s_rasterizer_target* target = rasterizer_dx9_texture_target_get(rasterizer_target);
    return ((target->d3d_texture_storage != NULL && !target->unk_bool_20 && target->mip_count > 0) ? target->d3d_surface_storage[0] : NULL);
}

bool __cdecl rasterizer_dx9_set_render_target_internal(IDirect3DSurface9* target, IDirect3DSurface9* z_stencil, bool use_depth)
{
    //return p_rasterizer_dx9_set_render_target_internal(target, z_stencil, use_depth);

    IDirect3DDevice9Ex* global_d3d_device = rasterizer_dx9_device_get_interface();

    IDirect3DSurface9** last_target = rasterizer_dx9_last_target_get();
    IDirect3DSurface9** last_z_target = rasterizer_dx9_last_z_target_get();
    IDirect3DSurface9** last_z_stencil = rasterizer_dx9_last_z_stencil_get();

    bool valid = true;
    if (target != (IDirect3DSurface9*)NONE && *last_target != target)
    {
        valid = SUCCEEDED(global_d3d_device->SetRenderTarget(0, target));
        *last_target = target;
    }


    bool target_set = false;
    if (*global_rasterizer_stage_get() == 2)
    {
        IDirect3DSurface9** global_d3d_surface_render_z_as_target_z = global_d3d_surface_render_z_as_target_z_get();
        if (target == *global_d3d_surface_render_primary_get() && *last_z_stencil != *global_d3d_surface_render_z_as_target_z)
        {
            *last_z_target = *global_d3d_surface_render_z_as_target_z;
            HRESULT hr = global_d3d_device->SetRenderTarget(1, *last_z_stencil);
            valid = valid && SUCCEEDED(hr);
            target_set = true;
        }
    }

    if (!target_set && *last_z_target != NULL)
    {
        *last_z_target = NULL;
        HRESULT hr = global_d3d_device->SetRenderTarget(1, NULL);
        valid = valid && SUCCEEDED(hr);
    }

    bool result;
    global_d3d_target_use_depth = use_depth;
    if (z_stencil == (IDirect3DSurface9*)NONE || *last_z_stencil == z_stencil)
    {
        result = valid;
    }
    else
    {
        HRESULT hr = global_d3d_device->SetDepthStencilSurface(z_stencil);
        *last_z_stencil = z_stencil;

        result = valid && SUCCEEDED(hr); 
    }

    return result;
}

void __cdecl rasterizer_set_render_target_internal_hook_set_viewport(IDirect3DSurface9* target, IDirect3DSurface9* z_stencil, bool a3)
{
	IDirect3DDevice9Ex* global_d3d_device = rasterizer_dx9_device_get_interface();

	const s_camera* global_camera = get_global_camera();

	rasterizer_dx9_set_render_target_internal(target, z_stencil, a3);
	D3DVIEWPORT9 vp = {
		global_camera->viewport_bounds.left,
		global_camera->viewport_bounds.top,
		rectangle2d_width(&global_camera->viewport_bounds),
		rectangle2d_height(&global_camera->viewport_bounds)
	};
	// set the viewport, after setting the main render target
	// to note that the viewport will always gets reset when a new render target is set
	global_d3d_device->SetViewport(&vp);
	return;
}

e_rasterizer_target __cdecl rasterizer_dx9_get_overlay_destination_target(void)
{
	return INVOKE(0x19DAFB, 0x0, rasterizer_dx9_get_overlay_destination_target);
}

IDirect3DSurface9* __cdecl rasterizer_dx9_get_render_target_surface(e_rasterizer_target rasterizer_target, int16 mipmap_index)
{
    IDirect3DSurface9* result;

    switch (rasterizer_target)
    {
    case _rasterizer_target_render_primary:
    case _rasterizer_target_frontbuffer:
        result = *global_d3d_surface_render_primary_get();
        break;
    case _rasterizer_target_sun_glow_primary:
        result = *global_d3d_surface_sun_glow_primary_get();
        break;
    case _rasterizer_target_sun_glow_secondary:
        result = global_d3d_surface_sun_glow_secondary;
        break;
    case _rasterizer_target_motion_sensor:
        result = global_d3d_surface_motion_sensor;
        break;
#ifdef USE_CUBEMAP_TARGETS
    case _rasterizer_target_cubemap_pos_x:
        result = global_d3d_surface_cubemap_pos_x;
        break;
    case _rasterizer_target_cubemap_neg_x:
        result = global_d3d_surface_cubemap_neg_x;
        break;
    case _rasterizer_target_cubemap_pos_y:
        result = global_d3d_surface_cubemap_pos_y;
        break;
    case _rasterizer_target_cubemap_neg_y:
        result = global_d3d_surface_cubemap_neg_y;
        break;
    case _rasterizer_target_cubemap_pos_z:
        result = global_d3d_surface_cubemap_pos_z;
        break;
    case _rasterizer_target_cubemap_neg_z:
        result = global_d3d_surface_cubemap_neg_z;
        break;
#endif
    case _rasterizer_target_resolved:
        result = *global_d3d_surface_render_resolved_get();
        break;
    default:
        result = rasterizer_dx9_get_target_mip_surface(rasterizer_target, mipmap_index);
        break;
    }
    return result;
}

bool __cdecl rasterizer_dx9_set_target_as_texture(int16 stage, e_rasterizer_target rasterizer_target)
{   
    IDirect3DTexture9* d3d_texture = NULL;
    uint32 resolution_x;
    uint32 resolution_y;

    ASSERT(VALID_INDEX(stage, 4));
    rasterizer_target_get_resolution(rasterizer_target, &resolution_x, &resolution_y);

    s_rasterizer_globals* rasterizer_globals = rasterizer_globals_get();
    rasterizer_globals->bitmaps.textures_staged_width[stage] = resolution_x;
    rasterizer_globals->bitmaps.textures_staged_height[stage] = resolution_y;

    switch (rasterizer_target)
    {
    case _rasterizer_target_render_primary:
    case _rasterizer_target_resolved:
        d3d_texture = global_d3d_texture_render_resolved;
        break;
    case _rasterizer_target_1:
        d3d_texture = rasterizer_dx9_get_target_texture(_rasterizer_target_1);
        break;
    case _rasterizer_target_backbuffer:
        d3d_texture = g_backbuffer_texture;
        break;
    case _rasterizer_target_frontbuffer:
        DISPLAY_ASSERT("###ERROR### attempt to set front buffer as a texture");
        break;
    case _rasterizer_target_bloom_primary:
    case _rasterizer_target_bloom_secondary:
    case _rasterizer_target_shadow_buffer:
    case _rasterizer_target_shadow:
    case _rasterizer_target_shadow_scratch1:
    case _rasterizer_target_shadow_scratch2:
    case _rasterizer_target_gamma_remap_lut:
    case _rasterizer_target_water_reflection:
    case _rasterizer_target_water_refraction:
    case _rasterizer_target_convolution_scratch1:
    case _rasterizer_target_convolution_scratch2:
    case _rasterizer_target_scratch:
        d3d_texture = rasterizer_dx9_get_target_texture(rasterizer_target);
        break;
    case _rasterizer_target_shadow_alias_swizzled:
        d3d_texture = rasterizer_dx9_get_target_texture(_rasterizer_target_shadow_alias_swizzled);
        break;
    case _rasterizer_target_dynamic_gamma:
        DISPLAY_ASSERT("_rasterizer_target_dynamic_gamma is no longer available");
        break;
    case _rasterizer_target_sun_glow_primary:
        d3d_texture = global_d3d_texture_sun_glow_primary;
        break;
    case _rasterizer_target_sun_glow_secondary:
        d3d_texture = global_d3d_texture_sun_glow_secondary;
        break;
    case _rasterizer_target_cinematic:
        d3d_texture = rasterizer_dx9_get_target_texture(_rasterizer_target_cinematic);
        break;
    case _rasterizer_target_active_camo:
        d3d_texture = rasterizer_dx9_get_target_texture(_rasterizer_target_active_camo);
        break;
    case _rasterizer_target_texture_camera:
        d3d_texture = rasterizer_dx9_get_target_texture(_rasterizer_target_texture_camera);
        break;
    case _rasterizer_target_particle_distortion:
        d3d_texture = rasterizer_dx9_get_target_texture(_rasterizer_target_particle_distortion);
        break;
    case _rasterizer_target_motion_sensor:
        d3d_texture = global_d3d_texture_motion_sensor;
        break;
#ifdef USE_CUBEMAP_TARGETS
    case _rasterizer_target_cubemap_pos_x:
    case _rasterizer_target_cubemap_neg_x:
    case _rasterizer_target_cubemap_pos_y:
    case _rasterizer_target_cubemap_neg_y:
    case _rasterizer_target_cubemap_pos_z:
    case _rasterizer_target_cubemap_neg_z:
        target_texture = global_d3d_texture_cubemap;
        break;
#endif
    default:
        DISPLAY_ASSERT("unreachable");
        break;
    }

    return rasterizer_dx9_set_texture(stage, d3d_texture);
}

bool rasterizer_dx9_set_target_as_texture_internal(int16 stage, e_rasterizer_target rasterizer_target, IDirect3DTexture9* d3d_texture)
{
    int32 resolution_x;
    int32 resolution_y;
    
    rasterizer_dx9_texture_target_surface_size(rasterizer_target, &resolution_x, &resolution_y);
    
    s_rasterizer_globals * rasterizer_globals = rasterizer_globals_get();
    rasterizer_globals->bitmaps.textures_staged_width[stage] = resolution_x;
    rasterizer_globals->bitmaps.textures_staged_height[stage] = resolution_y;
    
    return rasterizer_dx9_set_texture(stage, d3d_texture);
}

void __cdecl rasterizer_dx9_set_target(e_rasterizer_target rasterizer_target, int32 mipmap_index, bool use_depth)
{
    const s_rasterizer_globals* rasterizer_globals = rasterizer_globals_get();
    IDirect3DDevice9Ex* global_d3d_device = rasterizer_dx9_device_get_interface();

    IDirect3DSurface9* d3d_surface = NULL;
    IDirect3DSurface9* d3d9_depth_buffer_stencil = NULL;
    bool valid_target = true;
    bool depth_provided = false;

    e_rasterizer_target target = (*global_rasterizer_stage_get() != _rasterizer_target_1 ? rasterizer_target : _rasterizer_target_backbuffer);
    switch (target)
    {
    case _rasterizer_target_render_primary:
        depth_provided = true;
        if (rasterizer_globals->rasterizer_draw_on_main_back_buffer)
        {
            if (screenshot_in_progress())
            {
                d3d_surface = *global_d3d_surface_screenshot_get();
            }
            else
            {
                global_d3d_device->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &d3d_surface);
                d3d_surface->Release();
            }

            d3d9_depth_buffer_stencil = NULL;
        }
        else
        {
            d3d_surface = *global_d3d_surface_render_primary_get();
            d3d9_depth_buffer_stencil = *global_d3d_surface_render_primary_z_get();
        }
        break;
    case _rasterizer_target_1:
    case _rasterizer_target_bloom_primary:
    case _rasterizer_target_bloom_secondary:
    case _rasterizer_target_gamma_remap_lut:
    case _rasterizer_target_scratch:
        d3d_surface = rasterizer_dx9_target_get_main_mip_surface(target);
        d3d9_depth_buffer_stencil = *global_d3d_surface_render_primary_z_get();
        break;
    case _rasterizer_target_frontbuffer:
        DISPLAY_ASSERT("###ERROR### attempt to set front buffer as the render target");
        valid_target = false;
        break;
    case _rasterizer_target_backbuffer:
        d3d_surface = *g_backbuffer_surface_get();
        d3d9_depth_buffer_stencil = *global_d3d_surface_render_z_as_target_z_get();
        depth_provided = true;
        break;
    case _rasterizer_target_shadow_buffer:
        d3d_surface = rasterizer_dx9_target_get_main_mip_surface(_rasterizer_target_shadow_buffer);
        d3d9_depth_buffer_stencil = rasterizer_dx9_get_target_mip_surface(_rasterizer_target_shadow_buffer, 1);
        break;
    case _rasterizer_target_shadow:
    case _rasterizer_target_shadow_scratch1:
    case _rasterizer_target_shadow_scratch2:
    case _rasterizer_target_convolution_scratch1:
    case _rasterizer_target_convolution_scratch2:
        d3d_surface = rasterizer_dx9_target_get_main_mip_surface(target);
        break;
    case _rasterizer_target_shadow_alias_swizzled:
        d3d_surface = rasterizer_dx9_get_target_mip_surface(_rasterizer_target_shadow_alias_swizzled, mipmap_index);
        break;
    case _rasterizer_target_dynamic_gamma:
        DISPLAY_ASSERT("_rasterizer_target_dynamic_gamma is no longer available");
        valid_target = false;
        break;
    case _rasterizer_target_sun_glow_primary:
        d3d_surface = *global_d3d_surface_sun_glow_primary_get();
        break;
    case _rasterizer_target_sun_glow_secondary:
        d3d_surface = global_d3d_surface_sun_glow_secondary;
        break;
    case _rasterizer_target_cinematic:
        d3d_surface = rasterizer_dx9_target_get_main_mip_surface(_rasterizer_target_cinematic);
        d3d9_depth_buffer_stencil = *global_d3d_surface_render_primary_z_get();
        break;
    case _rasterizer_target_active_camo:
        d3d_surface = rasterizer_dx9_target_get_main_mip_surface(_rasterizer_target_active_camo);
        break;
    case _rasterizer_target_texture_camera:
        d3d_surface = rasterizer_dx9_target_get_main_mip_surface(_rasterizer_target_texture_camera);
        break;
    case _rasterizer_target_water_reflection:
        d3d_surface = rasterizer_dx9_target_get_main_mip_surface(_rasterizer_target_water_reflection);
        break;
    case _rasterizer_target_water_refraction:
        d3d_surface = rasterizer_dx9_target_get_main_mip_surface(_rasterizer_target_water_refraction);
        break;
    case _rasterizer_target_particle_distortion:
        d3d_surface = rasterizer_dx9_target_get_main_mip_surface(_rasterizer_target_particle_distortion);
        break;
    case _rasterizer_target_motion_sensor:
        d3d_surface = global_d3d_surface_motion_sensor;
        break;
#ifdef USE_CUBEMAP_TARGETS
    case _rasterizer_target_cubemap_pos_x:
        d3d9_depth_buffer_stencil = g_rasterizer_surface_cubemap_depth;
        d3d_surface = global_d3d_surface_cubemap_pos_x;
        break;
    case _rasterizer_target_cubemap_neg_x:
        d3d9_depth_buffer_stencil = g_rasterizer_surface_cubemap_depth;
        d3d_surface = global_d3d_surface_cubemap_neg_x;
        break;
    case _rasterizer_target_cubemap_pos_y:
        d3d_surface = global_d3d_surface_cubemap_pos_y;
        d3d9_depth_buffer_stencil = g_rasterizer_surface_cubemap_depth;
        break;
    case _rasterizer_target_cubemap_neg_y:
        d3d9_depth_buffer_stencil = g_rasterizer_surface_cubemap_depth;
        d3d_surface = global_d3d_surface_cubemap_neg_y;
        break;
    case _rasterizer_target_cubemap_pos_z:
        d3d9_depth_buffer_stencil = g_rasterizer_surface_cubemap_depth;
        d3d_surface = global_d3d_surface_cubemap_pos_z;
        break;
    case _rasterizer_target_cubemap_neg_z:
        d3d_surface = global_d3d_surface_cubemap_neg_z;
        d3d9_depth_buffer_stencil = g_rasterizer_surface_cubemap_depth;
        break;
#endif
    case _rasterizer_target_resolved:
        d3d_surface = *global_d3d_surface_render_resolved_get();
        d3d9_depth_buffer_stencil = *global_d3d_surface_render_primary_z_get();
        depth_provided = true;
        break;
    default:
        DISPLAY_ASSERT("unreachable");
        valid_target = false;
    }

    if (valid_target && d3d_surface)
    {
        const s_frame* global_window_parameters = global_window_parameters_get();
        real32 viewport_scale;

        D3DVIEWPORT9 viewport;
        // These are always the same
        viewport.MinZ = 0.f;
        viewport.MaxZ = 1.f;

        rasterizer_dx9_set_render_target_internal(d3d_surface, (use_depth ? d3d9_depth_buffer_stencil : NULL), depth_provided);

        // Change viewport parameters based on circumstances

        // If we're rendering a cubemap
        if (global_window_parameters->render_type == 2)
        {
            uint32 g_cubemap_resolution = *cubemap_target_size_get();
            viewport.X = 0;
            viewport.Y = 0;
            viewport.Width = g_cubemap_resolution;
            viewport.Height = g_cubemap_resolution;
        }
        else if (
            target == _rasterizer_target_render_primary ||
            target == _rasterizer_target_resolved ||
            target == _rasterizer_target_2 ||
            target == _rasterizer_target_1 ||
            target == _rasterizer_target_cinematic ||
            target == rasterizer_dx9_get_overlay_destination_target())
        {
            const int16 viewport_width = rectangle2d_width(&global_window_parameters->camera.viewport_bounds);
            const int16 viewport_height = rectangle2d_height(&global_window_parameters->camera.viewport_bounds);
            if (target == rasterizer_dx9_get_overlay_destination_target())
            {
                if (global_window_parameters->is_texture_camera)
                {
                    viewport.X = 0;
                    viewport.Y = 0;
                    viewport.Width = viewport_width;
                    viewport.Height = viewport_height;
                }
            }
            else if (global_window_parameters->is_texture_camera && !*hs_texture_camera_view_get())
            {
                viewport_scale = PIN(*hs_texture_camera_scale_get(), 0.0625f, 1.f);
                viewport.X = global_window_parameters->camera.viewport_bounds.left * viewport_scale;
                viewport.Y = global_window_parameters->camera.viewport_bounds.top * viewport_scale;
                viewport.Width = viewport_width * viewport_scale;
                viewport.Height = viewport_height * viewport_scale;
            }
            else
            {
                viewport_scale = 1.0;
                viewport.X = global_window_parameters->camera.viewport_bounds.left * viewport_scale;
                viewport.Y = global_window_parameters->camera.viewport_bounds.top * viewport_scale;
                viewport.Width = viewport_width * viewport_scale;
                viewport.Height = viewport_height * viewport_scale;
            }
        }
        // By default set the viewport as the desc params
        else 
        {
            D3DSURFACE_DESC desc;
            d3d_surface->GetDesc(&desc);
            viewport.X = 0;
            viewport.Y = 0;
            viewport.Width = desc.Width;
            viewport.Height = desc.Height;
        }
       
        global_d3d_device->SetViewport(&viewport);
    }
    else
    {
        error(2, "### ERROR %s failed", __FUNCTION__);
    }

    return;
}

bool __cdecl rasterizer_dx9_set_texture(uint32 stage, IDirect3DTexture9* texture)
{
    return INVOKE(0x26EBC7, 0x0, rasterizer_dx9_set_texture, stage, texture);
}

bool __cdecl rasterizer_target_get_resolution(e_rasterizer_target rasterizer_target, uint32* resolution_x, uint32* resolution_y)
{
    return INVOKE(0x280413, 0x0, rasterizer_target_get_resolution, rasterizer_target, resolution_x, resolution_y);
}

IDirect3DSurface9** global_d3d_surface_render_primary_get(void)
{
    return Memory::GetAddress<IDirect3DSurface9**>(0xA3C64C);
}

IDirect3DSurface9** global_d3d_surface_render_primary_z_get(void)
{
    return Memory::GetAddress<IDirect3DSurface9**>(0xA3C650);
}

IDirect3DSurface9** global_d3d_surface_render_resolved_get(void)
{
    return Memory::GetAddress<IDirect3DSurface9**>(0xA3C65C);
}

IDirect3DSurface9** global_d3d_surface_sun_glow_primary_get(void)
{
    return Memory::GetAddress<IDirect3DSurface9**>(0xA3C67C);
}

IDirect3DSurface9** rasterizer_dx9_last_target_get(void)
{
    return Memory::GetAddress<IDirect3DSurface9**>(0xA4B18C);
}

IDirect3DSurface9** rasterizer_dx9_last_z_target_get(void)
{
    return Memory::GetAddress<IDirect3DSurface9**>(0xA4B190);
}

IDirect3DSurface9** rasterizer_dx9_last_z_stencil_get(void)
{
    return Memory::GetAddress<IDirect3DSurface9**>(0xA4B194);
}

uint32* sun_glow_target_height_get(void)
{
    return Memory::GetAddress<uint32*>(0xA3C670);
}

uint32* sun_glow_target_width_get(void)
{
    return Memory::GetAddress<uint32*>(0xA3C674);
}

/* private code */

D3DMULTISAMPLE_TYPE* d3d_multisample_type_get(void)
{
    return Memory::GetAddress<D3DMULTISAMPLE_TYPE*>(0xA3C644);
}

uint32* d3d_multisample_quality_get(void)
{
    return Memory::GetAddress<uint32*>(0xA3C648);
}

IDirect3DSurface9** global_d3d_surface_render_z_as_target_z_get(void)
{
    return Memory::GetAddress<IDirect3DSurface9**>(0xA3C654);
}

IDirect3DSurface9** g_backbuffer_surface_get(void)
{
    return Memory::GetAddress<IDirect3DSurface9**>(0xA3C66C);
}

IDirect3DPixelShader9** global_d3d_shader_white_source_get(void)
{
    return Memory::GetAddress<IDirect3DPixelShader9**>(0x9DAAB0);
}

uint32* cubemap_target_size_get(void)
{
    return Memory::GetAddress<uint32*>(0x41F674);
}

uint32* motion_sensor_texture_size_get(void)
{
    return Memory::GetAddress<uint32*>(0x41F694);
}

IDirect3DTexture9* rasterizer_dx9_get_target_texture(e_rasterizer_target rasterizer_target)
{
    IDirect3DTexture9* result = NULL;
    if (rasterizer_target != _rasterizer_target_none)
    {
        const s_rasterizer_target* target = rasterizer_dx9_texture_target_get(rasterizer_target);

        // If the bool is false return the texture storage
        // Otherwise, return NULL
        result = (!target->unk_bool_20 ? target->d3d_texture_storage : result);             
    }

    return result;
}

IDirect3DSurface9* __cdecl rasterizer_dx9_get_target_mip_surface(e_rasterizer_target rasterizer_target, int16 mipmap_index)
{
    IDirect3DSurface9* result = NULL;
    const s_rasterizer_target* target = rasterizer_dx9_texture_target_get(rasterizer_target);
    if (target->d3d_texture_storage && !target->unk_bool_20 && VALID_INDEX(mipmap_index, target->mip_count))
    {
        ASSERT(mipmap_index < NUMBEROF(target->d3d_surface_storage));
        result = target->d3d_surface_storage[mipmap_index];
    }

    return result;
}

void __cdecl rasterizer_set_render_target_internal_hook_set_main_render_surface(IDirect3DSurface9* target, IDirect3DSurface9* z_stencil, bool a3)
{
    rasterizer_dx9_set_target(*rasterizer_dx9_main_render_target_get(), 0, true);
    return;
}

bool __cdecl rasterizer_dx9_primary_targets_initialize(void)
{
    const s_rasterizer_globals* rasterizer_globals = rasterizer_globals_get();
    IDirect3DDevice9Ex* global_d3d_device = rasterizer_dx9_device_get_interface();

    const int16 screen_bounds_width = rectangle2d_width(&rasterizer_globals->screen_bounds);
    const int16 screen_bounds_height = rectangle2d_height(&rasterizer_globals->screen_bounds);

    bool success = false;
    const D3DFORMAT backbuffer_fmt = rasterizer_globals->display_parameters.backbuffer_format;
    if (SUCCEEDED(global_d3d_device->CreateTexture(
        screen_bounds_width,
        screen_bounds_height,
        1,
        D3DUSAGE_RENDERTARGET,
        backbuffer_fmt,
        D3DPOOL_DEFAULT,
        &global_d3d_texture_render_resolved,
        NULL)))
    {
        const D3DMULTISAMPLE_TYPE type = *d3d_multisample_type_get();
        const uint32 quality = *d3d_multisample_quality_get();

        const D3DFORMAT depthstencil_fmt = rasterizer_globals->display_parameters.depthstencil_format;
        
        success = 
            SUCCEEDED(global_d3d_texture_render_resolved->GetSurfaceLevel(0, global_d3d_surface_render_resolved_get())) &&
            SUCCEEDED(global_d3d_device->CreateRenderTarget(screen_bounds_width,
                screen_bounds_height,
                backbuffer_fmt,
                type,
                quality,
                false,
                global_d3d_surface_render_primary_get(),
                NULL)) &&
            SUCCEEDED(global_d3d_device->CreateDepthStencilSurface(screen_bounds_width,
                screen_bounds_height,
                depthstencil_fmt,
                type,
                quality,
                false,
                global_d3d_surface_render_primary_z_get(),
                NULL));
    }

    const char whiteSource[] = "float4 main() : COLOR { return float4(1.0f, 1.0f, 0.0f, 1.0f); }        ";
    LPD3DXBUFFER pBuffer;
    const HRESULT hr = D3DXCompileShader(whiteSource, strlen(whiteSource), NULL, NULL, "main", "ps_2_0", 0, &pBuffer, NULL, NULL);

    bool result = false;
    if (pBuffer)
    {
        const HRESULT ps_hr = global_d3d_device->CreatePixelShader((const DWORD*)pBuffer->GetBufferPointer(), global_d3d_shader_white_source_get());
        pBuffer->Release();

        result = success && SUCCEEDED(hr) && SUCCEEDED(ps_hr);
    }

    return result;
}

void __cdecl rasterizer_dx9_primary_targets_dispose(void)
{
    IDirect3DPixelShader9** global_d3d_shader_white_source = global_d3d_shader_white_source_get();
    if (*global_d3d_shader_white_source)
    {
        (*global_d3d_shader_white_source)->Release();
        *global_d3d_shader_white_source = NULL;
    }
    IDirect3DSurface9** global_d3d_surface_screenshot = global_d3d_surface_screenshot_get();
    if (*global_d3d_surface_screenshot)
    {
        (*global_d3d_surface_screenshot)->Release();
        *global_d3d_surface_screenshot = NULL;
    }

    if (global_d3d_texture_render_resolved)
    {
        global_d3d_texture_render_resolved->Release();
        global_d3d_texture_render_resolved = NULL;
    }
    IDirect3DSurface9** global_d3d_surface_render_resolved = global_d3d_surface_render_resolved_get();
    if (*global_d3d_surface_render_resolved)
    {
        (*global_d3d_surface_render_resolved)->Release();
        *global_d3d_surface_render_resolved = NULL;
    }
    
    IDirect3DSurface9** global_d3d_surface_render_primary = global_d3d_surface_render_primary_get();
    if (*global_d3d_surface_render_primary)
    {
        (*global_d3d_surface_render_primary)->Release();
        *global_d3d_surface_render_primary = NULL;
    }
    IDirect3DSurface9** global_d3d_surface_render_primary_z = global_d3d_surface_render_primary_z_get();
    if (*global_d3d_surface_render_primary_z)
    {
        (*global_d3d_surface_render_primary_z)->Release();
        *global_d3d_surface_render_primary_z = NULL;
    }
    return;
}

bool __cdecl rasterizer_dx9_secondary_targets_initialize(void)
{
    const s_rasterizer_globals* rasterizer_globals = rasterizer_globals_get();
    IDirect3DDevice9Ex* global_d3d_device = rasterizer_dx9_device_get_interface();

    const int16 screen_bounds_width = rectangle2d_width(&rasterizer_globals->screen_bounds);
    const int16 screen_bounds_height = rectangle2d_height(&rasterizer_globals->screen_bounds);

    bool succeeded = SUCCEEDED(global_d3d_device->CreateTexture(
            screen_bounds_width,
            screen_bounds_height,
            1,
            1,
            D3DFMT_A8R8G8B8,
            D3DPOOL_DEFAULT,
            &g_backbuffer_texture,
            NULL));

    HRESULT hr;
    if (succeeded)
    {
        if (g_backbuffer_texture->GetSurfaceLevel(0, g_backbuffer_surface_get()) >= 0)
        {
            const bool render_depth = rasterizer_globals->render_depth_backbuffer;

            const D3DFORMAT format = (render_depth ? rasterizer_globals->display_parameters.backbuffer_format : rasterizer_globals->display_parameters.depthstencil_format);
            const D3DMULTISAMPLE_TYPE type = (render_depth ? *d3d_multisample_type_get() : D3DMULTISAMPLE_NONE);
            const uint32 quality = (render_depth ? *d3d_multisample_quality_get() : 0);

            IDirect3DSurface9** global_d3d_surface_render_z_as_target_z = global_d3d_surface_render_z_as_target_z_get();
            if (render_depth)
            {
                hr = global_d3d_device->CreateRenderTarget(screen_bounds_width, screen_bounds_height, format, type, quality, false, global_d3d_surface_render_z_as_target_z, NULL);
            }
            else
            {
                hr = global_d3d_device->CreateDepthStencilSurface(screen_bounds_width, screen_bounds_height, format, type, quality, false, global_d3d_surface_render_z_as_target_z, NULL);
            }
            succeeded = SUCCEEDED(hr);
        }
        else
        {
            succeeded = false;
        }
    }

    uint32 g_motion_sensor_texture_size = *motion_sensor_texture_size_get();
    rasterizer_dx9_create_texture(g_motion_sensor_texture_size, g_motion_sensor_texture_size, 0, 1, bitmap_data_format_a8r8g8b8, false, &global_d3d_texture_motion_sensor);

    bool result = false;
    if (global_d3d_texture_motion_sensor)
    {
        hr = global_d3d_texture_motion_sensor->GetSurfaceLevel(0, &global_d3d_surface_motion_sensor);
        result = succeeded && SUCCEEDED(hr) && global_d3d_surface_motion_sensor != NULL;
    }

    // We calculate the sun glow target width and height based on the viewport resolution
    const s_frame* global_window_parameters = global_window_parameters_get();
    const int16 viewport_width = rectangle2d_width(&global_window_parameters->camera.viewport_bounds);
    const int16 viewport_height = rectangle2d_height(&global_window_parameters->camera.viewport_bounds);

    D3DVIEWPORT9 d3d_viewport;
    rasterizer_dx9_device_get_interface()->GetViewport(&d3d_viewport);

    uint32* g_sun_glow_target_width = sun_glow_target_width_get();
    uint32* g_sun_glow_target_height = sun_glow_target_height_get();

    *g_sun_glow_target_width = (d3d_viewport.Height * g_sun_size) * rasterizer_globals->sun_width_scale;
    *g_sun_glow_target_height = (d3d_viewport.Height * g_sun_size);

    IDirect3DSurface9** global_d3d_surface_sun_glow_primary = global_d3d_surface_sun_glow_primary_get();
    result &=
        rasterizer_dx9_create_texture(*g_sun_glow_target_width, *g_sun_glow_target_height, 0, 1, bitmap_data_format_a8r8g8b8, 0, &global_d3d_texture_sun_glow_primary) &&
        SUCCEEDED(global_d3d_texture_sun_glow_primary->GetSurfaceLevel(0, global_d3d_surface_sun_glow_primary)) &&
        rasterizer_dx9_create_texture(*g_sun_glow_target_width, *g_sun_glow_target_height, 0, 1, bitmap_data_format_a8r8g8b8, 0, &global_d3d_texture_sun_glow_secondary) &&
        SUCCEEDED(global_d3d_texture_sun_glow_secondary->GetSurfaceLevel(0, &global_d3d_surface_sun_glow_secondary));

    // Fail if any of the textures or surfaces are null
    if (!global_d3d_texture_sun_glow_primary || !*global_d3d_surface_sun_glow_primary || !global_d3d_texture_sun_glow_secondary || !global_d3d_surface_sun_glow_secondary)
    {
        result = false;
        error(2, "### ERROR rasterizer_secondary_targets_initialize() failed to create offscreen surface(s)");
    }

    return result;
}

void __cdecl rasterizer_dx9_secondary_targets_dispose(void)
{
    if (global_d3d_surface_motion_sensor)
    {
        global_d3d_surface_motion_sensor->Release();
        global_d3d_surface_motion_sensor = NULL;
    }
    if (global_d3d_texture_motion_sensor)
    {
        global_d3d_texture_motion_sensor->Release();
        global_d3d_texture_motion_sensor = NULL;
    }

    IDirect3DSurface9** global_d3d_surface_sun_glow_primary = global_d3d_surface_sun_glow_primary_get();
    if (*global_d3d_surface_sun_glow_primary)
    {
        (*global_d3d_surface_sun_glow_primary)->Release();
        global_d3d_surface_sun_glow_primary = NULL;
    }
    if (global_d3d_surface_sun_glow_secondary)
    {
        global_d3d_surface_sun_glow_secondary->Release();
        global_d3d_surface_sun_glow_secondary = NULL;
    }
    if (global_d3d_texture_sun_glow_primary)
    {
        global_d3d_texture_sun_glow_primary->Release();
        global_d3d_texture_sun_glow_primary = NULL;
    }
    if (global_d3d_texture_sun_glow_secondary)
    {
        global_d3d_texture_sun_glow_secondary->Release();
        global_d3d_texture_sun_glow_secondary = NULL;
    }

    IDirect3DSurface9** g_backbuffer_surface = g_backbuffer_surface_get();
    if (*g_backbuffer_surface)
    {
        (*g_backbuffer_surface)->Release();
        *g_backbuffer_surface = NULL;
    }
    if (g_backbuffer_texture)
    {
        g_backbuffer_texture->Release();
        g_backbuffer_texture = NULL;
    }

    IDirect3DSurface9** global_d3d_surface_render_z_as_target_z = global_d3d_surface_render_z_as_target_z_get();
    if (*global_d3d_surface_render_z_as_target_z)
    {
        (*global_d3d_surface_render_z_as_target_z)->Release();
        global_d3d_surface_render_z_as_target_z = NULL;
    }
    return;
}
