#pragma once
#include "bitmaps/bitmap_group.h"

datum last_bitmap_tag_index_get(void);

IDirect3D9* rasterizer_dx9_get_interface(void);

IDirect3DDevice9Ex* rasterizer_dx9_device_get_interface(void);

void __cdecl sub_65F600(int16 stage, datum bitmap_tag_index, int16 bitmap_data_index, real32 a4);

void __cdecl rasterizer_present(bitmap_data* screenshot_bitmap);

void rasterizer_dx9_main_apply_patches(void);

static D3DFORMAT g_supported_depth_stencil_formats[] = { D3DFMT_D24S8, D3DFMT_D24X4S4 };