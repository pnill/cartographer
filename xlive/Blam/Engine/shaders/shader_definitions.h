#pragma once
#include "Blam/Cache/DataTypes/TagRef.h"
#include "Blam/Engine/math/color_math.h"
#include "Blam/Engine/math/function_definitions.h"
#include "Blam/Engine/tag_files/string_id.h"

#define k_number_of_shader_animation_property_types 14
#define k_maximum_parameters_per_shader 64

enum e_shader_parameter_type : int16
{
	_shader_parameter_bitmap,
	_shader_parameter_value,
	_shader_parameter_color,
	_shader_parameter_switch
};

enum e_shader_animation_property_type : int16
{
	_shader_animation_property_bitmap_scale_uniform,
	_shader_animation_property_bitmap_scale_x,
	_shader_animation_property_bitmap_scale_y,
	_shader_animation_property_bitmap_scale_z,
	_shader_animation_property_bitmap_translation_x,
	_shader_animation_property_bitmap_translation_y,
	_shader_animation_property_bitmap_translation_z,
	_shader_animation_property_bitmap_rotation_angle,
	_shader_animation_property_bitmap_rotation_axis_x,
	_shader_animation_property_bitmap_rotation_axis_y,
	_shader_animation_property_bitmap_rotation_axis_z,
	_shader_animation_property_value,
	_shader_animation_property_color,
	_shader_animation_property_bitmap_index
};

// max_count:    k_number_of_shader_animation_property_types = 14
struct s_shader_animation_property
{
	e_shader_animation_property_type type;
	int8 pad[2];
	string_id input_name;
	string_id range_name;
	real32 time_period_sec;

	// Explaination("FUNCTION", "EMPTY STRING")
	c_function_definition function;
};
TAG_BLOCK_SIZE_ASSERT(s_shader_animation_property, 24);

// max_count:    k_maximum_parameters_per_shader = 64
struct s_shader_parameter
{
	string_id name;
	e_shader_parameter_type type;
	int8 pad[2];
	tag_reference bitmap; // bitm
	real32 const_value;
	real_rgb_color const_color;
	tag_block<s_shader_animation_property> animation_properties;
};
TAG_BLOCK_SIZE_ASSERT(s_shader_parameter, 40);
