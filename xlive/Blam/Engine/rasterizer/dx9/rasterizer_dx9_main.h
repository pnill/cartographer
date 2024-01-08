#pragma once

datum last_bitmap_tag_index_get(void);

void __cdecl sub_65F600(int16 stage, datum bitmap_tag_index, int16 bitmap_data_index, real32 a4);

void rasterizer_dx9_main_apply_patches(void);

IDirect3D9* rasterizer_dx9_get_interface();

static D3DFORMAT g_supported_depth_stencil_formats[] = { D3DFMT_D24S8, D3DFMT_D24X4S4 };