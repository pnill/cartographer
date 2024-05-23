#pragma once
#include "bitmaps/bitmap_group.h"
#include "math/color_math.h"
#include "rasterizer/dx9/rasterizer_dx9_main.h"

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

struct s_dynamic_geometry_parameters
{
	void* meter_parameters;
	real32* field_4;
	real32 field_8;
	bitmap_data* map[3];
	int8 field_18[4];
	real32 field_1C;
	real32 field_20;
	real32 field_24;
	real32 field_28;
	real32 field_2C;
	int8 field_30[16];
	real32 width_ratio;
	real32 height_ratio;
	int8 field_48[76];
	e_framebuffer_blend_function blend_function;
	bool point_interpolation;
	int8 pad_97;
};
ASSERT_STRUCT_SIZE(s_dynamic_geometry_parameters, 152)
