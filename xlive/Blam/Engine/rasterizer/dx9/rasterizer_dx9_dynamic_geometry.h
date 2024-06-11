#pragma once
#include "bitmaps/bitmap_group.h"
#include "math/color_math.h"
#include "rasterizer/dx9/rasterizer_dx9_main.h"

/* constants */

#define k_dynamic_geometry_map_max_count 3

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

struct s_dynamic_geometry_parameters
{
	void* meter_parameters;
	real_vector2d* field_4;
	bool field_8;
	bool field_9;
	bool field_10;
	int8 pad_B;
	bitmap_data* map[k_dynamic_geometry_map_max_count];
	bool wrap_uv[k_dynamic_geometry_map_max_count];
	int8 pad_1B;
	real_vector2d* field_1C;
	real_vector2d* field_20;
	real_vector2d* field_24;
	real_vector4d vs_constants_second_4;
	real_vector2d field_34;
	real32 width_ratio;
	real32 height_ratio;
	real_vector4d vs_constants_second_0;
	s_dynamic_geometry_map_color_parameters* map_color[3];
	int8 field_60[32];
	s_dynamic_geometry_map_alpha* map_alpha[3];
	int16 field_90;
	int16 field_92;
	e_framebuffer_blend_function blend_function;
	bool point_interpolation;
	int8 pad_97;
};
ASSERT_STRUCT_SIZE(s_dynamic_geometry_parameters, 152)

/* public code */

void rasterizer_dx9_primitive_builder(uint8* data, uint32 size, const void* old_data);
