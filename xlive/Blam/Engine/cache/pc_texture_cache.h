#pragma once
#include "bitmaps/bitmap_group.h"
#include "memory/data.h"

#define k_max_pc_texture_cache_count 10000

/* structures */

// todo: unfinished very large struct
struct s_pc_texture_cache_globals
{
	s_data_array* predicted_textures;
};

/* prototypes */

void __cdecl texture_cache_new(void);

IDirect3DTexture9* __cdecl texture_cache_bitmap_get_hardware_format_add_to_texture_cache(bitmap_data* data, real32 a2, int32 a3);

IDirect3DTexture9* __cdecl pc_texture_cache_preload_bitmap(bitmap_data* bitmap_data, uint8 flags, real32 unk, bool* unused);

void __cdecl texture_cache_block_for_one_frame(int32 a1);
