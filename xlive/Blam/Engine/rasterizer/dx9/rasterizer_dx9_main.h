#pragma once
#include "bitmaps/bitmap_group.h"
#include "render/render.h"

bool* rasterizer_clear_screen_global_get(void);

static D3DFORMAT g_supported_depth_stencil_formats[] = { D3DFMT_D24S8, D3DFMT_D24X4S4 };

datum last_bitmap_tag_index_get(void);

IDirect3D9* rasterizer_dx9_get_interface(void);

IDirect3DSurface9* global_d3d_surface_screenshot_get(void);

bool __cdecl rasterizer_dx9_set_render_target(IDirect3DSurface9* target, int32 z_stencil, bool a3);

IDirect3DDevice9Ex* rasterizer_dx9_device_get_interface(void);

void rasterizer_dx9_main_apply_patches(void);

bool __cdecl rasterizer_initialize(void);

void __cdecl rasterizer_dx9_reset_depth_buffer(void);

void __cdecl rasterizer_present(bitmap_data* screenshot_bitmap);

bool __cdecl rasterizer_window_begin(s_frame* preferences);

void __cdecl rasterizer_update_cameras(void);

void __cdecl rasterizer_dx9_set_stencil_mode(int16 mode);

void __cdecl rasterizer_dx9_set_render_state(D3DRENDERSTATETYPE render_state, D3DBLEND blend);
