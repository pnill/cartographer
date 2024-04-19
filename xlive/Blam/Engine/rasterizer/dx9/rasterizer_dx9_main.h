#pragma once
#include "bitmaps/bitmap_group.h"

static D3DFORMAT g_supported_depth_stencil_formats[] = { D3DFMT_D24S8, D3DFMT_D24X4S4 };

datum last_bitmap_tag_index_get(void);

IDirect3D9* rasterizer_dx9_get_interface(void);

IDirect3DDevice9Ex* rasterizer_dx9_device_get_interface(void);

void rasterizer_dx9_main_apply_patches(void);

bool __cdecl rasterizer_initialize(void);

void __cdecl rasterizer_present(bitmap_data* screenshot_bitmap);
