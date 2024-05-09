#pragma once
#include "bitmaps/bitmap_group.h"
#include "render/render.h"

#include "rasterizer_dx9_targets.h"

/* enums */

enum e_framebuffer_blend_function : int16
{
	_framebuffer_blend_function_alpha_blend = 0,
	_framebuffer_blend_function_multiply = 1,
	_framebuffer_blend_function_double_multiply = 2,
	_framebuffer_blend_function_add = 3,
	_framebuffer_blend_function_subtract = 4,
	_framebuffer_blend_function_component_min = 5,
	_framebuffer_blend_function_component_max = 6,
	_framebuffer_blend_function_alpha_multiply_add = 7,
	_framebuffer_blend_function_constant_color_blend = 8,
	_framebuffer_blend_function_inverse_constant_color_blend = 9,
	_framebuffer_blend_function_none = 10,
	k_shader_framebuffer_blend_function_count
};

static D3DFORMAT g_supported_depth_stencil_formats[] = { D3DFMT_D24S8, D3DFMT_D24X4S4 };

datum last_bitmap_tag_index_get(void);

IDirect3D9* rasterizer_dx9_get_interface(void);

IDirect3DSurface9* global_d3d_surface_screenshot_get(void);

IDirect3DDevice9Ex* rasterizer_dx9_device_get_interface(void);

IDirect3DSurface9* global_d3d_surface_render_primary_get(void);

IDirect3DSurface9* global_d3d_surface_render_primary_z_get(void);

void rasterizer_dx9_main_apply_patches(void);

bool __cdecl rasterizer_initialize(void);

void __cdecl rasterizer_dx9_reset_depth_buffer(void);

void __cdecl rasterizer_present(bitmap_data* screenshot_bitmap);

void __cdecl rasterizer_dx9_set_texture_stage(int16 stage, datum bitmap_tag_index, int16 bitmap_data_index, real32 a4);

bool __cdecl rasterizer_window_begin(s_frame* preferences);

void __cdecl rasterizer_update_cameras(void);

void __cdecl rasterizer_dx9_set_stencil_mode(int16 mode);

void __cdecl rasterizer_dx9_set_render_state(D3DRENDERSTATETYPE state, DWORD value);

void rasterizer_dx9_set_blend_render_state(e_framebuffer_blend_function framebuffer_blend_function);

void __cdecl rasterizer_dx9_set_target(e_rasterizer_target render_target_type, int32 mip_level, bool a3);

bool __cdecl rasterizer_dx9_set_target_as_texture(int16 stage, int16 rasterizer_target);

void rasterizer_dx9_set_screen_effect_pixel_shader(int32 local_pixel_shader);

e_rasterizer_target __cdecl sub_66C2CA(
	real32 a1,
	real32 a2,
	real32 a3,
	int32 a4,
	int32 a5,
	int32 a6,
	int32 a7,
	int32 a8,
	int32 a9,
	real32 a10,
	real32 a11);
