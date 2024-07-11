#pragma once
#include "bitmaps/bitmap_group.h"
#include "memory/data.h"

// todo: unfinished very large struct
struct s_pc_texture_cache_globals
{
	s_data_array* predicted_textures;
};

IDirect3DTexture9* __cdecl texture_cache_bitmap_get_hardware_format_add_to_texture_cache(bitmap_data* data, real32 a2, int32 a3);

bool __cdecl pc_texture_cache_preload_bitmap(bitmap_data* bitmap_data, uint8 flags, real32 unk, bool* unused);