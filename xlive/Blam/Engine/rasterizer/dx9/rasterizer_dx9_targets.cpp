#include "stdafx.h"
#include "rasterizer_dx9_targets.h"

/* prototypes */



/* public code */

e_rasterizer_target* rasterizer_dx9_main_render_target_get(void)
{
	return Memory::GetAddress<e_rasterizer_target*>(0xA3E228);
}

s_rasterizer_target* rasterizer_dx9_texture_target_get(e_rasterizer_target texture_target)
{
	ASSERT(VALID_INDEX(texture_target, k_rasterizer_target_count));
	return &Memory::GetAddress<s_rasterizer_target*>(0xA8D658)[texture_target];
}

IDirect3DSurface9* __cdecl rasterizer_dx9_get_render_target_surface(e_rasterizer_target render_target_type, uint16 mipmap_index)
{
	return INVOKE(0x25FB67, 0x0, rasterizer_dx9_get_render_target_surface, render_target_type, mipmap_index);
}

IDirect3DSurface9* rasterizer_dx9_target_get_main_mip_surface(e_rasterizer_target rasterizer_target)
{
	s_rasterizer_target* target = rasterizer_dx9_texture_target_get(rasterizer_target);
	return ((target->texture != NULL && !target->unk_bool_20 && target->mip_count > 0) ? target->mip_surfaces[0] : NULL);
}

bool __cdecl rasterizer_dx9_set_render_target(IDirect3DSurface9* target, int32 z_stencil, bool a3)
{
	return INVOKE(0x26EBF8, 0x0, rasterizer_dx9_set_render_target, target, z_stencil, a3);
}

void __cdecl rasterizer_dx9_set_target(e_rasterizer_target render_target_type, int32 mip_level, bool a3)
{
	return INVOKE(0x25FDC0, 0x0, rasterizer_dx9_set_target, render_target_type, mip_level, a3);
}

void __cdecl rasterizer_set_render_target_internal_hook_set_main_render_surface(IDirect3DSurface9* target, IDirect3DSurface9* z_stencil, bool a3)
{
	rasterizer_dx9_set_target(*rasterizer_dx9_main_render_target_get(), 0, true);
}
