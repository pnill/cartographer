#pragma once
#include "tag_files/tag_block.h"
#include "Blam/Engine/math/color_math.h"
#include "Blam/Engine/math/function_definitions.h"

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

// max count: k_kilo
#pragma pack(push,1)
struct s_shader_postprocess_level_of_detail_new
{
	int32 available_layer_flags;
	int16 layers;
};
TAG_BLOCK_SIZE_ASSERT(s_shader_postprocess_level_of_detail_new, 6);
#pragma pack(pop)

// max count: k_kilo
struct s_shader_postprocess_bitmap_new
{
	datum bitmap_group;		// This is the actual index in cache builds?
	datum bitmap_index;
	real32 log_bitmap_dimension;
};
TAG_BLOCK_SIZE_ASSERT(s_shader_postprocess_bitmap_new, 12);


struct s_shader_postprocess_definition_new
{
	datum shader_template_index;
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
