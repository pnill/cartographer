#include "stdafx.h"
#include "rasterizer_dx9_main.h"

#include "rasterizer_dx9_targets.h"

#include "bitmaps/bitmaps.h"
#include "bink/wmv_playback.h"
#include "rasterizer/rasterizer_globals.h"
#include "rasterizer/rasterizer_loading.h"

/* typedefs */

typedef void(__cdecl* rasterizer_dx9_set_texture_stage_t)(int16, datum, int16, real32);

/* globals */

rasterizer_dx9_set_texture_stage_t p_rasterizer_dx9_set_texture_stage;

datum g_last_bitmap_tag_index = 0;


D3DBLEND g_blend_operation[k_shader_framebuffer_blend_function_count] =
{
  D3DBLEND_ZERO,
  D3DBLEND_ZERO,
  D3DBLEND_ZERO,
  D3DBLEND_ZERO,
  D3DBLEND_SRCCOLOR,
  D3DBLEND_INVSRCCOLOR,
  D3DBLEND_SRCALPHA,
  D3DBLEND_ZERO,
  D3DBLEND_ZERO,
  D3DBLEND_ZERO,
  (D3DBLEND)NONE,
  (D3DBLEND)NONE
};

D3DBLEND g_dst_blend[k_shader_framebuffer_blend_function_count] =
{
  D3DBLEND_INVSRCALPHA,
  D3DBLEND_ZERO,
  D3DBLEND_SRCCOLOR,
  D3DBLEND_ONE,
  D3DBLEND_ONE,
  D3DBLEND_ONE,
  D3DBLEND_ONE,
  D3DBLEND_INVSRCALPHA,
  (D3DBLEND)0,
  (D3DBLEND)0,
  (D3DBLEND)NONE,
  (D3DBLEND)NONE
};

D3DBLEND g_src_blend[k_shader_framebuffer_blend_function_count] =
{
  D3DBLEND_SRCALPHA,
  D3DBLEND_DESTCOLOR,
  D3DBLEND_DESTCOLOR,
  D3DBLEND_ONE,
  D3DBLEND_ONE,
  D3DBLEND_ONE,
  D3DBLEND_ONE,
  D3DBLEND_ONE,
  (D3DBLEND)0,
  (D3DBLEND)0,
  (D3DBLEND)NONE,
  (D3DBLEND)NONE
};

/* prototypes */

void __cdecl rasterizer_dx9_clear_target(uint32 flags, D3DCOLOR color, real32 z, bool stencil);
void __cdecl rasterizer_set_stream_source(void);
void __cdecl debug_frame_usage_draw(void);
void __cdecl rasterizer_present_backbuffer(void);
void __cdecl rasterizer_cache_bitmaps(void);
bool rasterizer_dx9_get_vertex_declaration_format(D3DVERTEXELEMENT9* vertex_elements, UINT* vertex_element_count);
bool __cdecl DrawPrimitiveUP_hook_get_vertex_decl(
    D3DPRIMITIVETYPE PrimitiveType,
    UINT PrimitiveCount,
    const void* pVertexStreamZeroData,
    UINT VertexStreamZeroStride);

/* public code */

s_rasterizer_dx9_main_globals* rasterizer_dx9_main_globals_get(void)
{
    return Memory::GetAddress<s_rasterizer_dx9_main_globals*>(0xA3C640);
}

IDirect3DDevice9Ex* rasterizer_dx9_device_get_interface(void)
{
    return rasterizer_dx9_main_globals_get()->global_d3d_device;
}

datum last_bitmap_tag_index_get(void)
{
    return g_last_bitmap_tag_index;
}

IDirect3DPixelShader9** local_pixel_shaders_get(void)
{
    return Memory::GetAddress<IDirect3DPixelShader9**>(0xA56C0C);
}

void rasterizer_dx9_main_apply_patches(void)
{
    // debugging, get vertex decls currently set
    //PatchCall(Memory::GetAddress(0x2220CA), DrawPrimitiveUP_hook_get_vertex_decl);
    //PatchCall(Memory::GetAddress(0x27D746), DrawPrimitiveUP_hook_get_vertex_decl);

    DETOUR_ATTACH(p_rasterizer_dx9_set_texture_stage, Memory::GetAddress<rasterizer_dx9_set_texture_stage_t>(0x25F600, 0x0), rasterizer_dx9_set_texture_direct);

    // Patch initialize code with our own to detect sm3 support
    PatchCall(Memory::GetAddress(0x263526), rasterizer_dx9_initialize_device);
    PatchCall(Memory::GetAddress(0x26375D), rasterizer_dx9_initialize_device);
    return;
}

bool __cdecl rasterizer_initialize(void)
{
    return INVOKE(0x0263359, 0x0, rasterizer_initialize);
}

void __cdecl rasterizer_dx9_reset_depth_buffer(void)
{
    return INVOKE(0x269835, 0x0, rasterizer_dx9_reset_depth_buffer);
}

void rasterizer_present(bitmap_data* screenshot_bitmap)
{
    bool result = true;
    if (!media_foundation_player_running())
    {
        s_rasterizer_dx9_main_globals* dx9_globals = rasterizer_dx9_main_globals_get();
        s_rasterizer_globals* rasterizer_globals = rasterizer_globals_get();
        rasterizer_globals->rasterizer_draw_on_main_back_buffer = false;
        if (screenshot_bitmap && screenshot_bitmap->base_address)
        {
            rectangle2d screen_bounds = rasterizer_globals->screen_bounds;

            const int16 calc_width = screen_bounds.left + screenshot_bitmap->width_pixels;
            const int16 calc_height = screen_bounds.top + screenshot_bitmap->height_pixels;
            screen_bounds.right = (screen_bounds.right > calc_width ? calc_width : screen_bounds.right);
            screen_bounds.bottom = (screen_bounds.bottom > calc_height ? calc_height : screen_bounds.bottom);

            int16 screenshot_height = rectangle2d_height(&screen_bounds);
            int32 screenshot_width = rectangle2d_width(&screen_bounds);

            D3DLOCKED_RECT locked_rect;
            if ((screenshot_bitmap->format == bitmap_data_format_a8r8g8b8 || screenshot_bitmap->format == bitmap_data_format_x8r8g8b8)
                && !screenshot_bitmap->mipmap_count
                && SUCCEEDED(dx9_globals->global_d3d_surface_screenshot->LockRect(&locked_rect, NULL, D3DLOCK_READONLY)))
            {
                if (locked_rect.pBits)
                {
                    int32 screenshot_pitch = screenshot_width * (bitmap_format_get_bits_per_pixel(screenshot_bitmap->format) / 8);
                    for (int16 row_index = 0; row_index < screenshot_height; ++row_index)
                    {
                        uint8* base_address = bitmap_get_base_address(screenshot_bitmap, 0, row_index, 0);
                        uint8* data = (uint8*)locked_rect.pBits;
                        csmemcpy(base_address, data + row_index * locked_rect.Pitch, screenshot_pitch);
                    }
                    result = SUCCEEDED(dx9_globals->global_d3d_surface_screenshot->UnlockRect());
                }
            }
            else
            {
                result = false;
            }
            rasterizer_dx9_set_render_target_internal(dx9_globals->global_d3d_surface_render_primary, (IDirect3DSurface9*)NONE, true);
            rasterizer_dx9_clear_target(0, global_white_pixel32.color, 0.0f, false);
        }

        rasterizer_set_stream_source();
        HRESULT present_result = dx9_globals->global_d3d_device->Present(NULL, NULL, *Memory::GetAddress<HWND*>(0x46D9C8), NULL);
        if (result && SUCCEEDED(present_result))
        {

#ifdef _DEBUG
            debug_frame_usage_draw();
            rasterizer_present_backbuffer();
#endif
            ++rasterizer_globals->display_parameters.frame_presented_count;
            // nullsub_69();
            if (!loading_screen_in_progress())
            {
                rasterizer_cache_bitmaps();
            }
        }
        else
        {
            rasterizer_globals->reset_screen = true;
        }
    }
}


void __cdecl rasterizer_dx9_set_texture_direct(int16 stage, datum bitmap_tag_index, int16 bitmap_data_index, real32 a4)
{
    g_last_bitmap_tag_index = bitmap_tag_index;
    p_rasterizer_dx9_set_texture_stage(stage, bitmap_tag_index, bitmap_data_index, a4);
    return;
}

bool __cdecl rasterizer_window_begin(s_frame* preferences)
{
    return INVOKE(0x2620CF, 0x0, rasterizer_window_begin, preferences);
}

void __cdecl rasterizer_update_cameras(void)
{
    return INVOKE(0x26268D, 0x0, rasterizer_update_cameras);
}

void __cdecl rasterizer_dx9_set_stencil_mode(int16 mode)
{
    return INVOKE(0x2603D2, 0x0, rasterizer_dx9_set_stencil_mode, mode);
}

void __cdecl rasterizer_dx9_set_render_state(D3DRENDERSTATETYPE state, DWORD value)
{
    return INVOKE(0x26F8E2, 0x0, rasterizer_dx9_set_render_state, state, value);
}

void rasterizer_dx9_set_blend_render_state(e_framebuffer_blend_function framebuffer_blend_function)
{
    bool alpha_blend_enabled = framebuffer_blend_function != _framebuffer_blend_function_none;
    rasterizer_dx9_set_render_state(D3DRS_ALPHABLENDENABLE, alpha_blend_enabled);
    if (alpha_blend_enabled)
    {
        rasterizer_dx9_set_render_state(D3DRS_SRCBLEND, g_src_blend[framebuffer_blend_function]);
        rasterizer_dx9_set_render_state(D3DRS_DESTBLEND, g_dst_blend[framebuffer_blend_function]);
        rasterizer_dx9_set_render_state(D3DRS_BLENDOP, g_blend_operation[framebuffer_blend_function]);
    }
    return;
}

void rasterizer_dx9_set_screen_effect_pixel_shader(int32 local_pixel_shader)
{
    rasterizer_dx9_device_get_interface()->SetPixelShader(local_pixel_shaders_get()[local_pixel_shader]);
    return;
}

void __cdecl rasterizer_get_bloom_brightness(real32* brightness, real32* overbright)
{
    INVOKE(0x25F17D, 0x0, rasterizer_get_bloom_brightness, brightness, overbright);
    return;
}


bool __cdecl rasterizer_set_texture_bitmap_data(int16 stage, bitmap_data* texture)
{
    return INVOKE(0x25F5AC, 0x0, rasterizer_set_texture_bitmap_data, stage, texture);
}

bool rasterizer_dx9_draw_primitive_up(
    D3DPRIMITIVETYPE PrimitiveType,
    uint32 PrimitiveCount,
    const void* pVertexStreamZeroData,
    uint32 VertexStreamZeroStride)
{
    return rasterizer_dx9_device_get_interface()->DrawPrimitiveUP(
        PrimitiveType,
        PrimitiveCount,
        pVertexStreamZeroData,
        VertexStreamZeroStride) >= 0;
}

bool __cdecl rasterizer_dx9_create_texture(uint32 width, uint32 height, int32 levels, uint32 usage, e_bitmap_data_format format, bool linear, IDirect3DTexture9** texture)
{
    return INVOKE(0x260820, 0x0, rasterizer_dx9_create_texture, width, height, levels, usage, format, linear, texture);
}

void rasterizer_dx9_texture_stage_dimensions(uint8 stage, uint32 width, uint32 height)
{
    s_rasterizer_globals* rasterizer_globals = rasterizer_globals_get();
    rasterizer_globals->bitmaps.textures_staged_width[stage] = width;
    rasterizer_globals->bitmaps.textures_staged_height[stage] = height;
    return;
}

bool __cdecl rasterizer_dx9_device_set_texture(uint32 stage, IDirect3DTexture9* texture)
{
    return INVOKE(0x26EBC7, 0x0, rasterizer_dx9_device_set_texture, stage, texture);
}

void __cdecl rasterizer_dx9_set_texture(uint16 stage, e_bitmap_type type, uint32 usage, datum tag_index, int16 bitmap_index, real32 a6)
{
    INVOKE(0x25F74C, 0x0, rasterizer_dx9_set_texture, stage, type, usage, tag_index, bitmap_index, a6);
    return;
}

bool __cdecl rasterizer_dx9_initialize_device(real_vector3d* v, bool display_blackness)
{
    bool result = INVOKE(0x25E798, 0x0, rasterizer_dx9_initialize_device, v, display_blackness);

    D3DCAPS9 caps{};
    rasterizer_dx9_device_get_interface()->GetDeviceCaps(&caps);

    // Tells the game to make use of MRT and shader model 3 if the gpu supports it
    rasterizer_globals_get()->d3d9_sm3_supported = caps.MaxVertexShader30InstructionSlots >= 512 && caps.MaxPixelShader30InstructionSlots >= 512;;
    return result;
}

void __cdecl rasterizer_dx9_initialize_camera_projection(
    bool is_texture_camera,
    const s_camera* camera,
    const render_projection* projection,
    e_rasterizer_target rasterizer_target)
{
    INVOKE(0x261B53, 0x0, rasterizer_dx9_initialize_camera_projection, is_texture_camera, camera, projection, rasterizer_target);
    return;
}

/* private code */

void __cdecl rasterizer_dx9_clear_target(uint32 flags, D3DCOLOR color, real32 z, bool stencil)
{
    INVOKE(0x25FC2A, 0x0, rasterizer_dx9_clear_target, flags, color, z, stencil);
    return;
}

void __cdecl rasterizer_set_stream_source(void)
{
    INVOKE(0x26F4D3, 0x0, rasterizer_set_stream_source);
    return;
}

void __cdecl debug_frame_usage_draw(void)
{
    INVOKE(0x25ECAA, 0x0, debug_frame_usage_draw);
    return;
}

void __cdecl rasterizer_present_backbuffer(void)
{
    INVOKE(0x25F277, 0x0, rasterizer_present_backbuffer);
    return;
}

void __cdecl rasterizer_cache_bitmaps(void)
{
    INVOKE(0x261720, 0x0, rasterizer_cache_bitmaps);
    return;
}

bool rasterizer_dx9_get_vertex_declaration_format(D3DVERTEXELEMENT9* vertex_elements, UINT* vertex_element_count)
{
    IDirect3DDevice9Ex* global_d3d_device = rasterizer_dx9_device_get_interface();

    IDirect3DVertexDeclaration9* vertex_dcl;
    global_d3d_device->GetVertexDeclaration(&vertex_dcl);
    bool res = SUCCEEDED(vertex_dcl->GetDeclaration(vertex_elements, vertex_element_count));

    vertex_dcl->Release();

    return res;
}

bool __cdecl DrawPrimitiveUP_hook_get_vertex_decl(
    D3DPRIMITIVETYPE PrimitiveType,
    UINT PrimitiveCount,
    const void* pVertexStreamZeroData,
    UINT VertexStreamZeroStride)
{
    IDirect3DDevice9Ex* global_d3d_device = rasterizer_dx9_device_get_interface();

    D3DVERTEXELEMENT9 vertex_elements[MAXD3DDECLLENGTH];
    UINT vertex_element_couunt;

    if (rasterizer_dx9_get_vertex_declaration_format(vertex_elements, &vertex_element_couunt))
    {
    }

    return SUCCEEDED(global_d3d_device->DrawPrimitiveUP(PrimitiveType, PrimitiveCount, pVertexStreamZeroData, VertexStreamZeroStride));
}
