#include "stdafx.h"
#include "rasterizer_dx9_targets.h"

/* public code */

e_rasterizer_target* rasterizer_dx9_global_target_get(void)
{
	return Memory::GetAddress<e_rasterizer_target*>(0xA3E228);
}

s_rasterizer_target* rasterizer_dx9_target_get(e_rasterizer_target rasterizer_target)
{
	ASSERT(VALID_INDEX(rasterizer_target, k_rasterizer_target_count));
	return &Memory::GetAddress<s_rasterizer_target*>(0xA8D658)[rasterizer_target];
}

IDirect3DSurface9* __cdecl rasterizer_dx9_get_target_surface(uint16 rasterizer_target, uint16 mipmap_index)
{
	return INVOKE(0x25FB67, 0x0, rasterizer_dx9_get_target_surface, rasterizer_target, mipmap_index);
}

IDirect3DSurface9* rasterizer_dx9_target_get_surface(e_rasterizer_target rasterizer_target)
{
	s_rasterizer_target* target = rasterizer_dx9_target_get(rasterizer_target);
	return (target->texture && !target->unk_bool_20 && target->field_14 > 0 ? target->surfaces[0] : NULL);
}