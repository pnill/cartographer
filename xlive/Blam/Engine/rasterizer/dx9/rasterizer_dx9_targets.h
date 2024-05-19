#pragma once

/* enums */
enum e_rasterizer_target : uint32
{
	_rasterizer_target_0 = 0,
	_rasterizer_target_1 = 1,
	_rasterizer_target_2 = 2,
	_rasterizer_target_backbuffer = 3,
	_rasterizer_target_4 = 4,
	_rasterizer_target_5 = 5,
	_rasterizer_target_6 = 6,
	_rasterizer_target_dynamic_shadow = 7,
	_rasterizer_target_lightmap_shadows_1 = 8,
	_rasterizer_target_lightmap_shadows_2 = 9,
	_rasterizer_target_10 = 10,
	_rasterizer_target_11 = 11,
	_rasterizer_target_12 = 12,
	_rasterizer_target_13 = 13,
	_rasterizer_target_sun_glow_primary = 14,
	_rasterizer_target_sun_glow_secondary = 15,
	_rasterizer_target_16 = 16,
	_rasterizer_target_transparent_geometry = 17,
	_rasterizer_target_18 = 18,
	_rasterizer_target_water = 19,
	_rasterizer_target_water_refraction = 20,
	_rasterizer_target_21 = 21,
	_rasterizer_target_22 = 22,
	_rasterizer_target_depth_of_field = 23,
	_rasterizer_target_24 = 24,
	_rasterizer_target_25 = 25,
	_rasterizer_target_26 = 26,
	_rasterizer_target_motion_sensor = 27,
	_rasterizer_target_28 = 28,
	_rasterizer_target_29 = 29,
	_rasterizer_target_30 = 30,
	_rasterizer_target_31 = 31,
	_rasterizer_target_32 = 32,
	_rasterizer_target_33 = 33,
	_rasterizer_target_34 = 34,
	_rasterizer_target_35 = 35,
	_rasterizer_target_36 = 36,
	_rasterizer_target_render_resolved = 37,
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

/* public code */

bool* rasterizer_target_back_buffer(void);

e_rasterizer_target* rasterizer_dx9_main_render_target_get(void);

s_rasterizer_target* rasterizer_dx9_texture_target_get(e_rasterizer_target rasterizer_target);

void __cdecl rasterizer_get_texture_target_surface_size(e_rasterizer_target target, int32* out_width, int32* out_height);

bool __cdecl rasterizer_dx9_set_render_target(IDirect3DSurface9* target, int32 z_stencil, bool a3);

void __cdecl rasterizer_set_render_target_internal_hook_set_main_render_surface(IDirect3DSurface9* target, IDirect3DSurface9* z_stencil, bool a3);

void __cdecl rasterizer_set_render_target_internal_hook_set_viewport(IDirect3DSurface9* target, IDirect3DSurface9* z_stencil, bool a3);

IDirect3DSurface9* __cdecl rasterizer_dx9_get_render_target_surface(e_rasterizer_target rasterizer_target, uint16 mipmap_index);

IDirect3DSurface9* rasterizer_dx9_target_get_main_mip_surface(e_rasterizer_target rasterizer_target);

e_rasterizer_target __cdecl rasterizer_dx9_get_overlay_destination_target(void);
