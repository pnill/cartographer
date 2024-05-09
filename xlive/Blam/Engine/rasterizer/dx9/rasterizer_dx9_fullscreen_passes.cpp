#include "stdafx.h"
#include "rasterizer_dx9_fullscreen_passes.h"

#include "rasterizer_dx9.h"
#include "rasterizer_dx9_main.h"
#include "rasterizer_dx9_shader_submit.h"

#include "main/main_screenshot.h"

/* prototypes */

real32* rasterizer_dx9_gamma_get(void);
real32* rasterizer_dx9_brightness_get(void);
void __cdecl rasterizer_dx9_apply_gamma_and_brightness(e_rasterizer_target surface_index);
bool __cdecl rasterizer_fullscreen_effects_build_vertex_buffer_cb(
    e_vertex_output_type output_type,
    real32* bounds,
    real32* location,
    void* output,
    void* ctx
);

/* public code */

void rasterizer_dx9_fullscreen_passes_apply_patches(void)
{
    PatchCall(Memory::GetAddress(0x262684), rasterizer_dx9_apply_gamma_and_brightness);
    return;
}

void __cdecl rasterizer_dx9_render_fullscreen_overlay_geometry(
    real32* a1,
    unsigned __int8(__cdecl* a2)(int32),
    bool(__cdecl* a3)(
        e_vertex_output_type input_type,
        real32* bounds,
        real32* location,
        void* output,
        void* ctx),
    int32(__cdecl* a4)(int32),
    int32 a5,
    int16 a6,
    bool a7)
{
    INVOKE(0x27D4EF, 0x0, rasterizer_dx9_render_fullscreen_overlay_geometry, a1, a2, a3, a4, a5, a6, a7);
    return;
}

/* private code */

real32* rasterizer_dx9_gamma_get(void)
{
    return Memory::GetAddress<real32*>(0xA3E4C0);
}

real32* rasterizer_dx9_brightness_get(void)
{
    return Memory::GetAddress<real32*>(0xA3E4C4);
}

void __cdecl rasterizer_dx9_apply_gamma_and_brightness(e_rasterizer_target rasterizer_target) 
{
    IDirect3DSurface9* backbuffer = NULL;
    const s_frame* global_window_parameters = global_window_parameters_get();
    IDirect3DDevice9Ex* global_d3d_device = rasterizer_dx9_device_get_interface();

    if (!global_window_parameters->is_texture_camera || *hs_texture_camera_view_get())
    {
        if (screenshot_in_progress())
        {
            backbuffer = global_d3d_surface_screenshot_get();
            backbuffer->AddRef();
        }
        else
        {
            global_d3d_device->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backbuffer);
        }

        real32* gamma = rasterizer_dx9_gamma_get();
        real32* brightness = rasterizer_dx9_brightness_get();
        
        if (*brightness == 0.f && *gamma == 1.f)
        {
            IDirect3DSurface9* target_surface = rasterizer_dx9_get_render_target_surface(rasterizer_target, 0);
            const RECT rect =
            {
                global_window_parameters->camera.viewport_bounds.left,
                global_window_parameters->camera.viewport_bounds.top,
                global_window_parameters->camera.viewport_bounds.left + rectangle2d_width(&global_window_parameters->camera.viewport_bounds),
                global_window_parameters->camera.viewport_bounds.top + rectangle2d_height(&global_window_parameters->camera.viewport_bounds)
            };
            rasterizer_set_render_target_internal_hook_set_viewport(backbuffer, (IDirect3DSurface9*)NONE, true);
            global_d3d_device->StretchRect(target_surface, &rect, backbuffer, &rect, D3DTEXF_NONE);
        }
        else
        {
            rasterizer_dx9_set_sampler_state(0, D3DSAMP_ADDRESSU, 3);
            rasterizer_dx9_set_sampler_state(0, D3DSAMP_ADDRESSV, 3);
            rasterizer_dx9_set_sampler_state(0, D3DSAMP_MAGFILTER, 1);
            rasterizer_dx9_set_sampler_state(0, D3DSAMP_MINFILTER, 1);
            rasterizer_dx9_set_sampler_state(0, D3DSAMP_MIPFILTER, 1);
            rasterizer_dx9_set_sampler_state(0, D3DSAMP_MAXANISOTROPY, 1);
            rasterizer_dx9_set_sampler_state(0, D3DSAMP_MIPMAPLODBIAS, 0);
            rasterizer_dx9_set_sampler_state(0, D3DSAMP_MAXMIPLEVEL, 0);
            rasterizer_dx9_set_render_state(D3DRS_COLORWRITEENABLE, D3DBLEND_INVBLENDFACTOR);
            rasterizer_dx9_set_render_state(D3DRS_ZENABLE, 0);
            rasterizer_dx9_set_render_state(D3DRS_DEPTHBIAS, 0);
            rasterizer_dx9_set_render_state(D3DRS_ALPHATESTENABLE, 0);
            rasterizer_dx9_set_render_state(D3DRS_CULLMODE, D3DBLEND_ZERO);
            if (!*rasterizer_dx9_disable_stencil_get())
            {
                rasterizer_dx9_set_render_state(D3DRS_STENCILENABLE, 0);
            }

            rasterizer_dx9_set_blend_render_state(_framebuffer_blend_function_none);
            rasterizer_set_render_target_internal_hook_set_viewport(backbuffer, (IDirect3DSurface9*)NONE, true);
            rasterizer_dx9_set_target_as_texture(0, (int16)rasterizer_target);

            c_rasterizer_constant_4f_cache<32>* main_pixel_shader_constants = rasterizer_get_main_pixel_shader_cache();
            if (main_pixel_shader_constants->test_cache(0, (real_vector4d*)gamma, 1))
            {
                global_d3d_device->SetPixelShaderConstantF(0, gamma, 1);
            }

            rasterizer_dx9_set_screen_effect_pixel_shader(3);
            rasterizer_dx9_render_fullscreen_overlay_geometry(0, 0, rasterizer_fullscreen_effects_build_vertex_buffer_cb, 0, 0, 1, 1);
        }
        if (backbuffer)
        {
            backbuffer->Release();
        }
    }
    else
    {
        e_rasterizer_target rasterizer_target_dst = rasterizer_dx9_get_overlay_destination_target();
        if (rasterizer_target_dst != NONE)
        {
            const RECT rect =
            {
                global_window_parameters->camera.viewport_bounds.left,
                global_window_parameters->camera.viewport_bounds.top,
                rectangle2d_width(&global_window_parameters->camera.viewport_bounds),
                rectangle2d_height(&global_window_parameters->camera.viewport_bounds)
            };
            IDirect3DSurface9* dst_surface = rasterizer_dx9_get_render_target_surface(rasterizer_target_dst, 0);
            IDirect3DSurface9* src_surface = rasterizer_dx9_get_render_target_surface(rasterizer_target, 0);
            global_d3d_device->StretchRect(src_surface, &rect, dst_surface, NULL, D3DTEXF_POINT);
        }
    }
    return;
}

bool __cdecl rasterizer_fullscreen_effects_build_vertex_buffer_cb(
    e_vertex_output_type output_type,
    real32* bounds,
    real32* location,
    void* output,
    void* ctx
)
{
    real32 bounds_width = bounds[1] - bounds[0];
    real32 bounds_height = bounds[3] - bounds[2];

    if (output_type == _vertex_output_type_position)
    {
        real32* position_output = (real32*)output;

        // position normalized device coordinates
        // make sure to set the viewport, otherwise it'll represent the entire surface
        // which is not good during split-screen
        position_output[0] = location[0] * 2.0f - 1.0f; // x
        position_output[1] = -(location[1] * 2.0f - 1.0f); // y
        position_output[2] = 1.0f; // z
        position_output[3] = 1.0f; // scale

        return true;
    }
    else if (output_type == _vertex_output_type_texcoord)
    {
        real32* texcoord_output = (real32*)output;

        // texcoords normalized device coordinates (origin in 0,0 bottom left, max 1,1 top right)
        // set the tex coords based on the texture set to be drawn on the screen
        int32 main_resolved_surface_size_x, main_resolved_surface_size_y;
        // get the size of the surface
        rasterizer_get_texture_target_surface_size(_rasterizer_target_37, &main_resolved_surface_size_x, &main_resolved_surface_size_y);

        // explanation: locations are per vertex, this draw call will take 4 vertices
        // this function will get called 4 times, for each single vertex
        // the location of the first vertex is at 0, 0. and according to texcoords normalized device coordinates, the origin is in bottom left corner of the texture surface
        // then it goes counter-clockwise until it forms a rectangle, so 0,0 -> 1,0 (right down) -> 1,1 (right top) -> 0,1 (left top) -> (end) 0,0 (left bottom)
        // if the location is 1, for each component we scale it with the bounds width/lenght and offset that with the verticies located on the left
        // if it is 0, then the vertex is either at the left of the screen, or at the bottom, depending on which component is 0
        texcoord_output[0] = (0.5f / bounds_width) + (location[0] * bounds_width) / main_resolved_surface_size_x + bounds[0] / main_resolved_surface_size_x;
        texcoord_output[1] = (0.5f / bounds_height) + (location[1] * bounds_height) / main_resolved_surface_size_y + bounds[2] / main_resolved_surface_size_y;

        texcoord_output[2] = 0.0f;
        texcoord_output[3] = 1.0f;

        return true;
    }
    else if (output_type == _vertex_output_type_color)
    {
        D3DCOLOR* color = (D3DCOLOR*)output;
        *color = global_white_pixel32.color;
        return true;
    }
    else
    {
        return false;
    }
}
