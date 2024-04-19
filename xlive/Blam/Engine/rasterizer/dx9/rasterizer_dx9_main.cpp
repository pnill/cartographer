#include "stdafx.h"
#include "rasterizer_dx9_main.h"

#include "bitmaps/bitmaps.h"
#include "bink/wmv_playback.h"

#include "rasterizer/rasterizer_loading.h"

datum last_bitmap_tag_index = 0;

typedef void(__cdecl* sub_65F600_t)(int16, datum, int16, real32);
sub_65F600_t p_sub_65F600;

void __cdecl sub_65F600(int16 stage, datum bitmap_tag_index, int16 bitmap_data_index, real32 a4);
bool __cdecl rasterizer_set_render_target(IDirect3DSurface9* target, IDirect3DSurface9* z_stencil, bool a3);
void __cdecl clear_render_target(uint32 flags, D3DCOLOR color, real32 z, bool stencil);
void __cdecl rasterizer_set_stream_source(void);
void __cdecl debug_frame_usage_draw(void);
void __cdecl rasterizer_present_backbuffer(void);
void __cdecl rasterizer_cache_bitmaps(void);

uint64* frame_presented_count_get(void)
{
    return Memory::GetAddress<uint64*>(0xA3E430);
}

bool* rasterizer_reset_screen_global_get(void)
{
    return Memory::GetAddress<bool*>(0xA3E4D4);
}

bool* rasterizer_clear_screen_global_get(void)
{
    return Memory::GetAddress<bool*>(0xA3E4D5);
}

rectangle2d* rasterizer_globals_screen_bounds_get(void)
{
    return Memory::GetAddress<rectangle2d*>(0xA3E410);
}

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

void rasterizer_dx9_main_apply_patches(void)
{
    DETOUR_ATTACH(p_sub_65F600, Memory::GetAddress<sub_65F600_t>(0x25F600, 0x0), sub_65F600);
    return;
}

bool __cdecl rasterizer_initialize(void)
{
    return INVOKE(0x0263359, 0x0, rasterizer_initialize);
}

void rasterizer_present(bitmap_data* screenshot_bitmap)
{
    bool result = true;
    if (!media_foundation_player_running())
    {
        bool* g_clear_screen = rasterizer_clear_screen_global_get();
        *g_clear_screen = false;
        if (screenshot_bitmap && screenshot_bitmap->base_address)
        {
            const rectangle2d* screen_bounds = rasterizer_globals_screen_bounds_get();
            int16 right = screen_bounds->right;
            if (screen_bounds->right > screen_bounds->left + screenshot_bitmap->width_pixels)
            {
                right = screen_bounds->left + screenshot_bitmap->width_pixels;
            }

            int16 bottom = screen_bounds->bottom;
            if (screen_bounds->bottom > screen_bounds->top + screenshot_bitmap->height_pixels)
            {
                bottom = screen_bounds->top + screenshot_bitmap->height_pixels;
            }

            int16 screenshot_height = bottom - screen_bounds->top;
            int32 width = right - screen_bounds->left;
            IDirect3DSurface9* global_d3d_surface_screenshot = global_d3d_surface_screenshot_get();

            D3DLOCKED_RECT locked_rect;
            if ((screenshot_bitmap->format == bitmap_data_format_a8r8g8b8 || screenshot_bitmap->format == bitmap_data_format_x8r8g8b8)
                && !screenshot_bitmap->mipmap_count
                && global_d3d_surface_screenshot->LockRect(&locked_rect, NULL, D3DLOCK_READONLY) >= 0)
            {
                if (locked_rect.pBits)
                {
                    int32 screenshot_pitch = width * (bitmap_format_get_bits_per_pixel(screenshot_bitmap->format) / 8);
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
            rasterizer_set_render_target(global_d3d_surface_render_primary_get(), (IDirect3DSurface9*)NONE, true);
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
            ++ * frame_presented_count_get();
            // nullsub_69();
            if (!loading_screen_in_progress())
            {
                rasterizer_cache_bitmaps();
            }
        }
        else
        {
            *rasterizer_reset_screen_global_get() = true;
        }
    }
}


void __cdecl sub_65F600(int16 stage, datum bitmap_tag_index, int16 bitmap_data_index, real32 a4)
{
    last_bitmap_tag_index = bitmap_tag_index;
    p_sub_65F600(stage, bitmap_tag_index, bitmap_data_index, a4);
    return;
}

bool __cdecl rasterizer_set_render_target(IDirect3DSurface9* target, IDirect3DSurface9* z_stencil, bool a3)
{
    return INVOKE(0x26EBF8, 0x0, rasterizer_set_render_target, target, z_stencil, a3);
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
