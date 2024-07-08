#pragma once

/* enums */

enum e_rasterizer_target : int16
{
	_rasterizer_target_none = NONE,
	_rasterizer_target_render_primary = 0,
	_rasterizer_target_1 = 1,
	_rasterizer_target_2 = 2,
	_rasterizer_target_backbuffer = 3,
	_rasterizer_target_frontbuffer = 4,
	_rasterizer_target_bloom_primary = 5,
	_rasterizer_target_bloom_secondary = 6,
	_rasterizer_target_shadow_buffer = 7,
	_rasterizer_target_shadow = 8,
	_rasterizer_target_shadow_scratch1 = 9,
	_rasterizer_target_shadow_scratch2 = 10,
	_rasterizer_target_shadow_alias_swizzled = 11,
	_rasterizer_target_dynamic_gamma = 12,	// No longer available
	_rasterizer_target_gamma_remap_lut = 13,
	_rasterizer_target_sun_glow_primary = 14,
	_rasterizer_target_sun_glow_secondary = 15,
	_rasterizer_target_cinematic = 16,
	_rasterizer_target_active_camo = 17,
	_rasterizer_target_texture_camera = 18,
	_rasterizer_target_water_reflection = 19,
	_rasterizer_target_water_refraction = 20,
	_rasterizer_target_particle_distortion = 21,
	_rasterizer_target_z_a8b8g8r8 = 22,
	_rasterizer_target_convolution_scratch1 = 23,
	_rasterizer_target_convolution_scratch2 = 24,
	_rasterizer_target_scratch = 25,
	_rasterizer_target_unused = 26,
	_rasterizer_target_motion_sensor = 27,
	_rasterizer_target_water = 28,
	_rasterizer_target_bloom_source = 29,
	_rasterizer_target_bloom_target = 30,
	
	/* These targets go unused in the game */

	_rasterizer_target_cubemap_pos_x = 31,
	_rasterizer_target_cubemap_neg_x = 32,
	_rasterizer_target_cubemap_pos_y = 33,
	_rasterizer_target_cubemap_neg_y = 34,
	_rasterizer_target_cubemap_pos_z = 35,
	_rasterizer_target_cubemap_neg_z = 36,

	// Final target
	_rasterizer_target_resolved = 37,
	k_rasterizer_target_count
};

/* structures */

struct s_rasterizer_target
{
	IDirect3DTexture9* d3d_texture_storage;
	IDirect3DSurface9* d3d_surface_storage[4];
	int32 mip_count;
	uint32 size_x;
	uint32 size_y;
	bool unk_bool_20;
	int8 pad[3];
};

/* globals */



/* public code */

void rasterizer_dx9_targets_apply_patches(void);

e_rasterizer_target* rasterizer_dx9_main_render_target_get(void);

s_rasterizer_target* rasterizer_dx9_texture_target_get(e_rasterizer_target rasterizer_target);

void rasterizer_dx9_copy_target(e_rasterizer_target src_target, e_rasterizer_target dst_target);

void __cdecl rasterizer_dx9_texture_target_surface_size(e_rasterizer_target target, uint32* out_width, uint32* out_height);

IDirect3DSurface9* rasterizer_dx9_target_get_main_mip_surface(e_rasterizer_target rasterizer_target);

bool __cdecl rasterizer_dx9_set_render_target_internal(IDirect3DSurface9* target, IDirect3DSurface9* z_stencil, bool use_depth);

void __cdecl rasterizer_set_render_target_internal_hook_set_main_render_surface(IDirect3DSurface9* target, IDirect3DSurface9* z_stencil, bool a3);

void __cdecl rasterizer_set_render_target_internal_hook_set_viewport(IDirect3DSurface9* target, IDirect3DSurface9* z_stencil, bool a3);

e_rasterizer_target __cdecl rasterizer_dx9_get_overlay_destination_target(void);

IDirect3DSurface9* __cdecl rasterizer_dx9_get_render_target_surface(e_rasterizer_target rasterizer_target, int16 mipmap_index);

bool __cdecl rasterizer_dx9_set_target_as_texture(int16 stage, e_rasterizer_target rasterizer_target);

bool rasterizer_dx9_set_target_as_texture_internal(int16 stage, e_rasterizer_target rasterizer_target, IDirect3DTexture9* d3d_texture);

void __cdecl rasterizer_dx9_set_target(e_rasterizer_target rasterizer_target, int32 mipmap_index, bool use_depth);

bool __cdecl rasterizer_target_get_resolution(e_rasterizer_target rasterizer_target, uint32* resolution_x, uint32* resolution_y);

IDirect3DSurface9** rasterizer_dx9_last_target_get(void);

IDirect3DSurface9** rasterizer_dx9_last_z_target_get(void);

IDirect3DSurface9** rasterizer_dx9_last_z_stencil_get(void);
