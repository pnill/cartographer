#pragma once

/* enums */

enum e_rasterizer_widget_vertex_shader
{
	_rasterizer_widget_vertex_shader_screen,
	_rasterizer_widget_vertex_shader_transformed,
	_rasterizer_widget_vertex_shader_screen_alternate
};

enum e_rasterizer_widget_blend_mode : int32
{
	_rasterizer_widget_blend_mode_default,
	_rasterizer_widget_blend_mode_opaque,
	_rasterizer_widget_blend_mode_opaque_modulate_2x,
	_rasterizer_widget_blend_mode_opaque_alternate,
	_rasterizer_widget_blend_mode_alpha_blend,
	_rasterizer_widget_blend_mode_alpha_blend_alternate,
	_rasterizer_widget_blend_mode_additive,
	_rasterizer_widget_blend_mode_additive_multiply,
	_rasterizer_widget_blend_mode_additive_alternate
};

enum e_rasterizer_widget_flags : uint16
{
	_rasterizer_widget_flag_no_cull_bit,
	_rasterizer_widget_flag_z_test_bit,
	_rasterizer_widget_flag_z_write_bit,
	_rasterizer_widget_flag_no_color_write,

	k_rasterizer_widget_flags_antenna = FLAG(_rasterizer_widget_flag_no_cull_bit) | FLAG(_rasterizer_widget_flag_z_test_bit)
};

enum e_rasterizer_widget_sampler_flags : uint8
{
	_rasterizer_widget_sampler_border_address_bit,
	_rasterizer_widget_sampler_point_filter_bit,

	k_rasterizer_widget_sampler_flags_antenna = FLAG(_rasterizer_widget_sampler_point_filter_bit)
};

/* prototypes */

void __cdecl rasterizer_dx9_create_widget_pixel_shaders(void);

void __cdecl rasterizer_widget_render_setup(int32 vertex_shader, int32 blend_mode, uint16 flags);

bool __cdecl rasterizer_widget_set_texture(int16 stage, datum bitmap_tag_index, int16 frame_index, uint8 sampler_flags);

void rasterizer_widget_draw_quad(const real_point3d* points, const real_point2d* texcoords, const pixel32* colors);