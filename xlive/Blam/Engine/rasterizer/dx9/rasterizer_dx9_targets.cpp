#include "stdafx.h"
#include "rasterizer_dx9_targets.h"
#include "rasterizer/dx9/rasterizer_dx9_main.h"
#include "camera/camera.h"

/* prototypes */



/* public code */

e_rasterizer_target* rasterizer_dx9_main_render_target_get(void)
{
	return Memory::GetAddress<e_rasterizer_target*>(0xA3E228);
}

bool* rasterizer_target_back_buffer(void)
{
	return Memory::GetAddress<bool*>(0xA3E4D5);
}

s_rasterizer_target* rasterizer_dx9_texture_target_get(e_rasterizer_target texture_target)
{
	ASSERT(VALID_INDEX(texture_target, k_rasterizer_target_count));
	return &Memory::GetAddress<s_rasterizer_target*>(0xA8D658)[texture_target];
}

void __cdecl rasterizer_get_texture_target_surface_size(e_rasterizer_target target, int32* out_width, int32* out_height)
{
	return INVOKE(0x280413, 0x0, rasterizer_get_texture_target_surface_size, target, out_width, out_height);
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

void __cdecl rasterizer_set_render_target_internal_hook_set_main_render_surface(IDirect3DSurface9* target, IDirect3DSurface9* z_stencil, bool a3)
{
	rasterizer_dx9_set_target(*rasterizer_dx9_main_render_target_get(), 0, true);
}

void __cdecl rasterizer_set_render_target_internal_hook_set_viewport(IDirect3DSurface9* target, IDirect3DSurface9* z_stencil, bool a3)
{
	IDirect3DDevice9Ex* global_d3d_device = rasterizer_dx9_device_get_interface();

	s_camera* global_camera = get_global_camera();

	rasterizer_dx9_set_render_target(target, (int32)z_stencil, a3);
	D3DVIEWPORT9 vp = {
		global_camera->viewport_bounds.left,
		global_camera->viewport_bounds.top,
		rectangle2d_width(&global_camera->viewport_bounds),
		rectangle2d_height(&global_camera->viewport_bounds)
	};
	// set the viewport, after setting the main render target
	// to note that the viewport will always gets reset when a new render target is set
	global_d3d_device->SetViewport(&vp);
}

e_rasterizer_target __cdecl rasterizer_dx9_get_overlay_destination_target(void)
{
	return INVOKE(0x19DAFB, 0x0, rasterizer_dx9_get_overlay_destination_target);
}
