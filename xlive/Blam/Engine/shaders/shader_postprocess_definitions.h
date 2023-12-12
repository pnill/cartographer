#pragma once
#include "Blam/Cache/DataTypes/TagBlock.h"
#include "Blam/Engine/math/color_math.h"
#include "Blam/Engine/math/function_definitions.h"
#include "Blam/Engine/math/real_math.h"
#include "Blam/Engine/tag_files/string_id.h"

struct s_shader_postprocess_value_property
{
	real32 value;
};

struct s_shader_postprocess_color_property
{
	real_rgb_color color;
};

struct s_shader_postprocess_bitmap_property
{
	int16 bitmap_index;
	int16 animated_parameter_index;
};

struct s_shader_postprocess_animated_parameter_reference
{
	int8 pad[3];
	int8 parameter_index;
};

struct s_shader_postprocess_animated_parameter
{
	int16 overlay_references;
};

struct s_shader_postprocess_overlay_reference
{
	int16 overlay_index;
	int16 transform_index;
};

struct s_shader_postprocess_overlay_new
{
	string_id input_name;
	string_id range_name;
	real32 time_period_in_seconds;
	c_function_definition function;
};

struct s_shader_postprocess_implementation_new
{
	int16 bitmap_transforms;
	int16 render_states;
	int16 texture_states;
	int16 pixel_constants;
	int16 vertex_constants;
};

struct s_shader_postprocess_level_of_detail_new
{
	int32 available_layer_flags;
	int16 layers;
};

struct s_shader_postprocess_bitmap_new
{
	int32 bitmap_group;
	int32 bitmap_index;
	real32 log_bitmap_dimension;
};

struct s_shader_postprocess_definition_new
{
	int32 shader_template_index;
	tag_block<s_shader_postprocess_bitmap_new> bitmaps;
	tag_block<pixel32> pixel_constants;
	tag_block<real_vector4d> vertex_constants;
	tag_block<s_shader_postprocess_level_of_detail_new> levels_of_detail;
	tag_block<int16> layers;
	tag_block<int16> passes;
	tag_block<s_shader_postprocess_implementation_new> implementations;
	tag_block<s_shader_postprocess_overlay_new> overlays;
	tag_block<s_shader_postprocess_overlay_reference> overlay_references;
	tag_block<s_shader_postprocess_animated_parameter> animated_parameters;
	tag_block<s_shader_postprocess_animated_parameter_reference> animated_parameter_references;
	tag_block<s_shader_postprocess_bitmap_property> bitmap_properties;
	tag_block<s_shader_postprocess_color_property> color_properties;
	tag_block<s_shader_postprocess_value_property> value_properties;
	tag_block<s_shader_postprocess_level_of_detail_new> old_levels_of_detail;
};
