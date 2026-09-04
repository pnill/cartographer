#pragma once
#include "math/function_definitions.h"

/* constants */

enum
{
	k_maximum_promotion_rules_per_sound = 9,
	MAXIMUM_CHUNKS_PER_PERMUTATION = 32767,
	k_maximum_number_of_sound_effect_inputs = 16,
	k_maximum_number_of_sound_effect_templates = 8,
	k_maximum_number_of_sound_effect_parameters = 128,
	k_maximum_number_of_sound_effect_components = 16,
	k_maximum_number_of_override_mixbins = 8,
};

/* enums */

enum e_platform_sound_effect_input
{
	_platform_sound_effect_input_zero = 0,
	_platform_sound_effect_function_time,
	_platform_sound_effect_function_scale,
	_platform_sound_effect_function_roll_off,
	k_number_of_platform_sound_effect_inputs
};

/* structures */

// max: k_maximum_number_of_sound_effect_inputs
struct s_platform_sound_effect_function
{
	c_enum<e_platform_sound_effect_input, uint16, _platform_sound_effect_input_zero, k_number_of_platform_sound_effect_inputs> input;
	c_enum<e_platform_sound_effect_input, uint16, _platform_sound_effect_input_zero, k_number_of_platform_sound_effect_inputs> range;
	c_function_definition function;
	real32 time_period_seconds;
};
ASSERT_STRUCT_SIZE(s_platform_sound_effect_function, 16);

// max: k_maximum_number_of_sound_effect_templates
struct s_platform_sound_effect_definition
{
	s_tag_block function_inputs;	// s_platform_sound_effect_function

	// max: k_maximum_number_of_sound_effect_inputs
	s_tag_block constants;			// real32

	// max: k_maximum_number_of_sound_effect_inputs
	s_tag_block overrides;			// int8

	int32 input_overrides;
};
ASSERT_STRUCT_SIZE(s_platform_sound_effect_definition, 28);

// max: 1
struct s_platform_sound_effect_collection
{
	s_tag_block sound_effects;			// s_platform_sound_effect_definition
	s_tag_block low_frequency_input;	// s_platform_sound_effect_function
	int32 input_overrides;
};
ASSERT_STRUCT_SIZE(s_platform_sound_effect_collection, 20);

// max: k_maximum_number_of_sound_effect_parameters
struct s_sound_effect_override_parameter
{
	string_id name;
	string_id input;
	string_id range;
	real32 time_period_seconds;
	int32 int_value;
	real32 real_value;
	c_function_definition function;
};
ASSERT_STRUCT_SIZE(s_sound_effect_override_parameter, 32);

// max: 1
struct s_sound_effect_override_collection
{
	string_id name;
	s_tag_block parameters;				// s_sound_effect_override_parameter
};
ASSERT_STRUCT_SIZE(s_sound_effect_override_collection, 12);

// max: k_maximum_number_of_sound_effect_components
struct s_sound_effect_component_definition
{
	tag_reference sound; // !snd

	// additional attenuation to sound
	real32 gain_db;

	c_flags_no_init<e_sound_effect_component_definition_flags, uint32, k_sound_effect_component_definition_count> flags;
};
ASSERT_STRUCT_SIZE(s_sound_effect_component_definition, 16);

// max: 1
struct s_sound_effect_definition
{
	tag_reference sound_effect_template;	// <fx>

	s_tag_block components;					// s_sound_effect_component_definition
	s_tag_block overrides;					// s_sound_effect_override_collection

	/****************************************
	* definition_name: sound_effect_hardware_format_data_definition
	* flags: 0
	* alignment_bit: 4
	****************************************/
	// DataSize(1024)
	tag_data hardware_format_data;

	s_tag_block sound_effect_collection;	// s_platform_sound_effect_collection
};
ASSERT_STRUCT_SIZE(s_sound_effect_definition, 40);
