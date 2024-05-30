#include "stdafx.h"
#include "rasterizer_dx9_main.h"

#include "rasterizer_dx9_dof.h"
#include "rasterizer_dx9_targets.h"


#include "bitmaps/bitmaps.h"
#include "bink/wmv_playback.h"
#include "rasterizer/rasterizer_globals.h"
#include "rasterizer/rasterizer_loading.h"

/* globals */
datum last_bitmap_tag_index = 0;


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
  (D3DBLEND)NONE
};


/* typedefs */

typedef void(__cdecl* rasterizer_dx9_set_texture_stage_t)(int16, datum, int16, real32);
rasterizer_dx9_set_texture_stage_t p_rasterizer_dx9_set_texture_stage;

/* prototypes */

void __cdecl clear_render_target(uint32 flags, D3DCOLOR color, real32 z, bool stencil);
void __cdecl rasterizer_set_stream_source(void);
void __cdecl debug_frame_usage_draw(void);
void __cdecl rasterizer_present_backbuffer(void);
void __cdecl rasterizer_cache_bitmaps(void);

datum last_bitmap_tag_index_get(void)
{
    return last_bitmap_tag_index;
}

IDirect3D9* rasterizer_dx9_get_interface()
{
    return *Memory::GetAddress<IDirect3D9**>(0xA3C640);
}

IDirect3DDevice9Ex* rasterizer_dx9_device_get_interface(void)
{
    return *Memory::GetAddress<IDirect3DDevice9Ex**>(0xA3C6B4);
}

IDirect3DSurface9* global_d3d_surface_screenshot_get(void)
{
    return *Memory::GetAddress<IDirect3DSurface9**>(0xA3C660);
}

IDirect3DSurface9* global_d3d_surface_render_primary_get(void)
{
    return *Memory::GetAddress<IDirect3DSurface9**>(0xA3C64C);
}

IDirect3DSurface9* global_d3d_surface_render_primary_z_get(void)
{
    return *Memory::GetAddress<IDirect3DSurface9**>(0xA3C650);
}

IDirect3DSurface9* global_d3d_surface_render_resolved_get(void)
{
    return *Memory::GetAddress<IDirect3DSurface9**>(0xA3C65C);
}

IDirect3DPixelShader9** local_pixel_shaders_get(void)
{
    return Memory::GetAddress<IDirect3DPixelShader9**>(0xA56C0C);
}

void rasterizer_dx9_main_apply_patches(void)
{
    DETOUR_ATTACH(p_rasterizer_dx9_set_texture_stage, Memory::GetAddress<rasterizer_dx9_set_texture_stage_t>(0x25F600, 0x0), rasterizer_dx9_set_texture_stage);
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
            IDirect3DSurface9* global_d3d_surface_screenshot = global_d3d_surface_screenshot_get();

            D3DLOCKED_RECT locked_rect;
            if ((screenshot_bitmap->format == bitmap_data_format_a8r8g8b8 || screenshot_bitmap->format == bitmap_data_format_x8r8g8b8)
                && !screenshot_bitmap->mipmap_count
                && global_d3d_surface_screenshot->LockRect(&locked_rect, NULL, D3DLOCK_READONLY) >= 0)
            {
                if (locked_rect.pBits)
                {
                    int32 screenshot_pitch = screenshot_width * (bitmap_format_get_bits_per_pixel(screenshot_bitmap->format) / 8);
                    for (int16 row_index = 0; row_index < screenshot_height; ++row_index)
                    {
                        uint8* base_address = bitmap_get_base_address(screenshot_bitmap, 0, row_index, 0);
                        uint8* data = (uint8*)locked_rect.pBits;
                        csmemcpy(base_address, (void*)&data[row_index * locked_rect.Pitch], screenshot_pitch);
                    }
                    result = global_d3d_surface_screenshot->UnlockRect() >= 0;
                }
            }
            else
            {
                result = false;
            }
            rasterizer_dx9_set_render_target_internal(global_d3d_surface_render_primary_get(), NONE, true);
            clear_render_target(0, NONE, 0.0f, false);
        }

        rasterizer_set_stream_source();
        HRESULT present_result = rasterizer_dx9_device_get_interface()->Present(NULL, NULL, *Memory::GetAddress<HWND*>(0x46D9C8), NULL);
        if (result && present_result >= 0)
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


void __cdecl rasterizer_dx9_set_texture_stage(int16 stage, datum bitmap_tag_index, int16 bitmap_data_index, real32 a4)
{
    last_bitmap_tag_index = bitmap_tag_index;
    p_rasterizer_dx9_set_texture_stage(stage, bitmap_tag_index, bitmap_data_index, a4);
    return;
}

void __cdecl clear_render_target(uint32 flags, D3DCOLOR color, real32 z, bool stencil)
{
    INVOKE(0x25FC2A, 0x0, clear_render_target, flags, color, z, stencil);
    return;
}

void __cdecl rasterizer_set_stream_source(void)
{
    INVOKE(0x26F4D3, 0x0, rasterizer_set_stream_source);
    return;
}

// Draws 
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

void __cdecl rasterizer_dx9_set_target(e_rasterizer_target render_target_type, int32 mip_level, bool use_depth)
{
    INVOKE(0x25FDC0, 0x0, rasterizer_dx9_set_target, render_target_type, mip_level, use_depth);
    return;
}

bool __cdecl rasterizer_dx9_set_target_as_texture(int16 stage, int16 rasterizer_target)
{
    return INVOKE(0x260256, 0x0, rasterizer_dx9_set_target_as_texture, stage, rasterizer_target);
}

void rasterizer_dx9_set_screen_effect_pixel_shader(int32 local_pixel_shader)
{
    rasterizer_dx9_device_get_interface()->SetPixelShader(local_pixel_shaders_get()[local_pixel_shader]);
    return;
}

e_rasterizer_target __cdecl rasterizer_dx9_convolve_screen_surfaces(
    real32 a1,
    real32 a2,
    real32 a3,
    e_rasterizer_target source,
    e_rasterizer_target destination,
    e_rasterizer_target stretch_source,
    e_rasterizer_target stretch_destination,
    int32 a8,
    int32 a9,
    real32 a10,
    real32 a11)
{
    return INVOKE(0x26C2CA, 0x0, rasterizer_dx9_convolve_screen_surfaces,
        a1,
        a2,
        a3,
        source,
        destination,
        stretch_source,
        stretch_destination,
        a8,
        a9,
        a10,
        a11);
}

void __cdecl rasterizer_get_bloom_brightness(real32* brightness, real32* overbright)
{
    INVOKE(0x25F17D, 0x0, rasterizer_get_bloom_brightness, brightness, overbright);
    return;
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
