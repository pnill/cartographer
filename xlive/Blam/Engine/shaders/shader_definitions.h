#pragma once
#include "shader_postprocess_definitions.h"

#include "cache/predicted_resources.h"
#include "math/color_math.h"
#include "math/function_definitions.h"
#include "tag_files/string_id.h"
#include "tag_files/tag_reference.h"

#define k_number_of_shader_animation_property_types 14
#define k_maximum_parameters_per_shader 64
#define k_shader_bitmap_property_count 5

enum e_shader_definition_flags : int16
{
	_shader_definition_water_bit = 0,
	_shader_definition_sort_first_bit = 1,
	_shader_definition_no_active_camo_bit = 2
};

enum e_shader_lod_bias : int16
{
	_shader_lod_bias_none = 0,
	_shader_lod_bias_4x_size = 1,
	_shader_lod_bias_2x_size = 2,
	_shader_lod_bias_half_size = 3,
	_shader_lod_bias_fourth_size = 4,
	_shader_lod_bias_never = 5,
	_shader_lod_bias_cinematic = 6
};

enum e_shader_specular_type : int16
{
	_shader_specular_type_none = 0,
	_shader_specular_type_default = 1,
	_shader_specular_type_dull = 2,
	_shader_specular_type_shiny = 3
};

enum e_shader_lightmap_type : int16
{
	_shader_lightmap_type_diffuse = 0,
	_shader_lightmap_type_default_specular = 1,
	_shader_lightmap_type_dull_specular = 2,
	_shader_lightmap_type_shiny_specular = 3
};

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

// max_count: 1
struct s_shader_properties
{
	tag_reference diffuse_map;				// bitm
	tag_reference lightmap_emissive_map;	// bitm
	real_rgb_color lightmap_emissive_color;

	real32 lightmap_emissive_power;
	real32 lightmap_resolution_scale;
	real32 lightmap_half_life;
	real32 lightmap_diffuse_scale;
	
	tag_reference alpha_test_map;	// bitm
	tag_reference translucent_map;	// bitm
	real_rgb_color lightmap_transparent_color;
	real32 lightmap_transparent_alpha;
	real32 lightmap_foliage_scale;
};
ASSERT_STRUCT_SIZE(s_shader_properties, 80);

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
ASSERT_STRUCT_SIZE(s_shader_animation_property, 24);

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
ASSERT_STRUCT_SIZE(s_shader_parameter, 40);

struct s_shader_definition
{
	tag_reference shader_template;	// stem
	string_id material_name;
	tag_block<s_shader_properties> runtime_properties;
	int16 pad;

	e_shader_definition_flags flags;
	tag_block<s_shader_parameter> parameters;
	tag_block<s_shader_postprocess_definition_new> postprocess_definition;
	int32 pad1;
	tag_block<predicted_resource> predicted_resources;
	tag_reference light_response;	// slit

	e_shader_lod_bias shader_lod_bias;
	e_shader_specular_type specular_type;
	e_shader_lightmap_type lightmap_type;
	int16 pad2;
	
	real32 lightmap_specular_brightness;
	real32 lightmap_ambient_bias;
	tag_block<datum> bitmap_group_index;	// max_count: k_shader_bitmap_property_count 5
	
	real32 added_depth_bias_offset;
	real32 added_depth_bias_slope_scale;
};
ASSERT_STRUCT_SIZE(s_shader_definition, 92);
