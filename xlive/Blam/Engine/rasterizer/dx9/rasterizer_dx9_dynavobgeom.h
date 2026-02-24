#pragma once
#include "math/color_math.h"
#include "rasterizer/rasterizer_main.h"

/* constants */

enum
{
	k_dynamic_geometry_map_max_count = 3
};

/* structures */

struct s_dynamic_vertex_data
{
	real32 pos_x;
	real32 pos_y;
	real32 bounds_x;
	real32 bounds_y;
	pixel32 color;
};
ASSERT_STRUCT_SIZE(s_dynamic_vertex_data, 20)

struct s_dynamic_geometry_map_color_parameters
{
	real_rgb_color* color;
	void* pad;
};

struct s_dynamic_geometry_map_alpha
{
	real32* alpha;
};

struct rasterizer_dynamic_screen_geometry_parameters
{
	void* meter_parameters;
	real_vector2d* offset;
	bool map_anchor_screen[k_dynamic_geometry_map_max_count];
	int8 pad_B;
	struct bitmap_data* map[k_dynamic_geometry_map_max_count];
	bool map_wrapped[k_dynamic_geometry_map_max_count];
	int8 pad_1B;
	real_vector2d* map_offset[k_dynamic_geometry_map_max_count];
	real_vector4d vs_constants_second_4;
	real_vector2d field_34;
	real32 width_ratio;
	real32 height_ratio;
	real_vector4d vs_constants_second_0;
	s_dynamic_geometry_map_color_parameters* map_tint[k_dynamic_geometry_map_max_count];
	int8 field_60[32];
	s_dynamic_geometry_map_alpha* map_alpha[k_dynamic_geometry_map_max_count];
	e_framebuffer_blend_function map0_to_1_blend_function;
	e_framebuffer_blend_function map1_to_2_blend_function;
	e_framebuffer_blend_function framebuffer_blend_function;
	bool point_sampled;
	int8 pad_97;
};
ASSERT_STRUCT_SIZE(rasterizer_dynamic_screen_geometry_parameters, 152)

/* prototypes */

void __cdecl rasterizer_dx9_create_dynavobgeom_pixel_shaders(void);

void rasterizer_dx9_dynamic_screen_geometry_draw(const rasterizer_dynamic_screen_geometry_parameters* parameters, const s_dynamic_vertex_data* vertex_data);
