#pragma once
#include "geometry/geometry_block.h"
#include "geometry/geometry_definitions_new.h"
#include "math/color_math.h"
#include "tag_files/tag_block.h"
#include "tag_files/tag_reference.h"

// max: 1
struct s_water_definition
{
	tag_reference shader; // shad

	// max: 1
	tag_block<geometry_section> section;

	geometry_block_info geometry_block_info;

	real_rgb_color sun_spot_color;
	real_rgb_color reflection_tint;
	real_rgb_color refraction_tint;
	real_rgb_color horizon_color;
	real32 sun_specular_power;
	real32 reflection_bump_scale;
	real32 refraction_bump_scale;
	real32 frensel_scale;
	real32 sun_dir_heading;
	real32 sun_dir_pitch;
	real32 field_of_view;
	real32 aspect;
	real32 height;
	real32 farz;
	real32 rotate_offset;
	real_vector2d center;
	real_vector2d extents;
	real32 fog_near;
	real32 fog_far;
	real32 dynamic_height_bias;
};
ASSERT_STRUCT_SIZE(s_water_definition, 172);