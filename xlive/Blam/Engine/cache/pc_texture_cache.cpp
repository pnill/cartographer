#include "stdafx.h"
#include "pc_texture_cache.h"

/* public code */

void __cdecl texture_cache_new(void)
{
	INVOKE(0x2658C2, 0x0, texture_cache_new);
	return;
}

IDirect3DTexture9* __cdecl texture_cache_bitmap_get_hardware_format_add_to_texture_cache(bitmap_data* data, real32 a2, int32 a3)
{
	return INVOKE(0x265C51, 0x0, texture_cache_bitmap_get_hardware_format_add_to_texture_cache, data, a2, a3);
}

IDirect3DTexture9* __cdecl pc_texture_cache_preload_bitmap(bitmap_data* bitmap_data, uint8 flags, real32 unk, bool* unused)
{
	return INVOKE(0x265986, 0, pc_texture_cache_preload_bitmap, bitmap_data, flags, unk, unused);
}

void __cdecl texture_cache_block_for_one_frame(int32 a1)
{
	INVOKE(0x265948, 0x0, texture_cache_block_for_one_frame, a1);
	return;
}
