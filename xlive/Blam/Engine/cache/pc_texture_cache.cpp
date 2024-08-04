#include "stdafx.h"
#include "pc_texture_cache.h"

IDirect3DTexture9* __cdecl texture_cache_bitmap_get_hardware_format_add_to_texture_cache(bitmap_data* data, real32 a2, int32 a3)
{
	return INVOKE(0x265C51, 0x0, texture_cache_bitmap_get_hardware_format_add_to_texture_cache, data, a2, a3);
}

IDirect3DTexture9* __cdecl pc_texture_cache_preload_bitmap(bitmap_data* bitmap_data, uint8 flags, real32 unk, bool* unused)
{
	return INVOKE(0x265986, 0, pc_texture_cache_preload_bitmap, bitmap_data, flags, unk, unused);
}
