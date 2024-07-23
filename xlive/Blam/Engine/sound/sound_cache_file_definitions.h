#pragma once
#include "geometry/geometry_block.h"
#include "math/function_definitions.h"
#include "memory/static_arrays.h"
#include "tag_files/data_reference.h"
#include "tag_files/string_id.h"
#include "tag_files/tag_block.h"
#include "tag_files/tag_reference.h"

#define k_maximum_promotion_rules_per_sound 9
#define MAXIMUM_CHUNKS_PER_PERMUTATION 32767
#define k_maximum_number_of_sound_effect_inputs 16
#define k_maximum_number_of_sound_effect_templates 8
#define k_maximum_number_of_sound_effect_parameters 128
#define k_maximum_number_of_sound_effect_components 16
#define k_maximum_number_of_override_mixbins 8

enum e_platform_sound_effect_function_operation : uint16
{
	_platform_sound_effect_function_zero = 0,
	_platform_sound_effect_function_time = 1,
	_platform_sound_effect_function_scale = 2,
	_platform_sound_effect_function_roll_off = 3
};

enum e_sound_effect_component_definition_flags : uint32
{
	_sound_effect_component_definition_flag_dont_play_at_start = 0,
	_sound_effect_component_definition_flag_play_on_stop = 1,
	_sound_effect_component_definition_flag_play_alternate = 2,
	_sound_effect_component_definition_flag_sync_with_origin_looping_sound = 3,
	k_sound_effect_component_definition_count
};

enum e_platform_sound_source_filter_type : uint32
{
	_platform_sound_source_filter_parametric_eq = 0,
	_platform_sound_source_filter_dls2 = 1,
	_platform_sound_source_filter_both_only_valid_for_mono = 2
};

enum e_platform_sound_mixbin_type : uint32
{
	_platform_sound_mixbin_front_left = 0,
	_platform_sound_mixbin_front_right = 1,
	_platform_sound_mixbin_back_left = 2,
	_platform_sound_mixbin_back_right = 3,
	_platform_sound_mixbin_center = 4,
	_platform_sound_mixbin_low_frequency = 5,
	_platform_sound_mixbin_reverb = 6,
	_platform_sound_mixbin_3d_front_left = 7,
	_platform_sound_mixbin_3d_front_right = 8,
	_platform_sound_mixbin_3d_back_left = 9,
	_platform_sound_mixbin_3d_back_right = 10,
	_platform_sound_mixbin_default_left_speakers = 11,
	_platform_sound_mixbin_default_right_speakers = 12
};

enum e_platform_sound_playback_definition_flags : uint32
{
	_platform_sound_playback_definition_flag_use_3d_radio_hack = 0,
	k_platform_sound_playback_definition_flag_count
};

enum e_sound_definition_playback_parameter_flags : uint32
{
	_sound_definition_playback_parameter_flag_override_azimuth = 0,
	_sound_definition_playback_parameter_flag_override_3d_gain = 1,
	_sound_definition_playback_parameter_flag_override_speaker_gain = 3,
	k_sound_definition_playback_parameter_flag_count
};

// max: 288 (MAXIMUM_PITCH_RANGES_PER_SOUND*MAXIMUM_PERMUTATIONS_PER_PITCH_RANGE)
struct s_sound_permutation_dialogue_info
{
	uint32 mouth_data_offset;
	uint32 mouth_data_length;
	uint32 lip_sync_data_offset;
	uint32 lip_sync_data_length;
};
ASSERT_STRUCT_SIZE(s_sound_permutation_dialogue_info, 16);

// max: 1
struct s_sound_encoded_dialogue_section
{
	/****************************************
	* definition_name: generic_tag_data
	* flags: 0
	* alignment_bit: 0
	****************************************/
	// DataSize(301989888)
	data_reference encoded_data;

	tag_block<s_sound_permutation_dialogue_info> sound_permutation_dialogue_info;
};
ASSERT_STRUCT_SIZE(s_sound_permutation_dialogue_info, 16)

// max: SHORT_MAX
struct s_cache_file_sound_definition_extra_info
{
	tag_block<s_sound_encoded_dialogue_section> encoded_permutations;
	geometry_block_info geometry_block_info;
};
ASSERT_STRUCT_SIZE(s_cache_file_sound_definition_extra_info, 44);

// max: k_maximum_promotion_rules_per_sound
struct s_sound_promotion_rule
{
	uint16 sound_pitch_range_index;
	uint16 maximum_playing_count;

	/// time from when first permutation plays to when another sound from an equal or lower promotion can play
	real32 suppression_time_seconds;
	int32 runtime_value_1;
	int32 runtime_value_2;
};
ASSERT_STRUCT_SIZE(s_sound_promotion_rule, 16);

// max: 1
struct s_sound_promotion_parameters
{
	tag_block<s_sound_promotion_rule> rules;
	tag_block<real32> runtime_timers;

	int32 runtime_active_promotion_index;
	int32 runtime_last_promotion_time;
	int32 runtime_suppression_timeout;
};
ASSERT_STRUCT_SIZE(s_sound_promotion_parameters, 28);

// max: MAXIMUM_CHUNKS_PER_PERMUTATION
struct s_sound_permutation_chunk
{
	uint32 file_offset;
	uint16 flags;
	uint16 size;
	int32 runtime_index;
};
ASSERT_STRUCT_SIZE(s_sound_permutation_chunk, 12);

// max: k_maximum_number_of_sound_effect_inputs
struct s_platform_sound_effect_function
{
	e_platform_sound_effect_function_operation input;
	e_platform_sound_effect_function_operation range;
	c_function_definition function;
	real32 time_period_seconds;
};
ASSERT_STRUCT_SIZE(s_platform_sound_effect_function, 16);

// max: k_maximum_number_of_sound_effect_templates
struct s_platform_sound_effect_definition
{
	tag_block<s_platform_sound_effect_function> function_inputs;

	// max: k_maximum_number_of_sound_effect_inputs
	tag_block<real32> constants;

	// max: k_maximum_number_of_sound_effect_inputs
	tag_block<int8> overrides;

	int32 input_overrides;
};
ASSERT_STRUCT_SIZE(s_platform_sound_effect_definition, 28);

// max: 1
struct s_platform_sound_effect_collection
{
	tag_block<s_platform_sound_effect_definition> sound_effects;
	tag_block<s_platform_sound_effect_function> low_frequency_input;
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
	tag_block<s_sound_effect_override_parameter> parameters;
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
	tag_reference sound_effect_template; //<fx>

	tag_block<s_sound_effect_component_definition> components;
	tag_block<s_sound_effect_override_collection> overrides;

	/****************************************
	* definition_name: sound_effect_hardware_format_data_definition
	* flags: 0
	* alignment_bit: 4
	****************************************/
	// DataSize(1024)
	data_reference hardware_format_data;

	tag_block<s_platform_sound_effect_collection> sound_effect_collection;
};
ASSERT_STRUCT_SIZE(s_sound_effect_definition, 40);

// max: 1
struct s_sound_playback_parameter
{
	real_bounds scale_bounds;
	real_bounds random_base_and_variance;
};
ASSERT_STRUCT_SIZE(s_sound_playback_parameter, 16);

// max: 1
struct s_platform_sound_source_filter_lfo
{
	// Explaination("delay", "in seconds")
	s_sound_playback_parameter delay;

	// Explaination("frequency", "in Hz[0,43.7]")
	s_sound_playback_parameter frequency;

	// Explaination("cutoff modulation", "octaves[-8,8]")
	s_sound_playback_parameter cut_off_modulation;

	// Explaination("gain modulation", "dB[-16,16]")
	s_sound_playback_parameter gain_modulation;

};
ASSERT_STRUCT_SIZE(s_platform_sound_source_filter_lfo, 64);

// max: 1
struct s_platform_sound_source_pitch_lfo
{
	// Explaination("delay", "seconds")
	s_sound_playback_parameter delay;

	// Explaination("frequency", "Hz[0,43.7]")
	s_sound_playback_parameter frequency;

	// Explaination("pitch modulation", "octaves[-1,1]")
	s_sound_playback_parameter pitch_modulation;
};
ASSERT_STRUCT_SIZE(s_platform_sound_source_pitch_lfo, 48);

// max: 1
struct s_platform_sound_source_filter
{
	// Explaination("parameter description", "DLS2 filtering:
	// resonance gain range : [0, 22.5] dB
	//	parametric EQ :
	// gain range : [-64, 14] dB
	//	for mono sounds :
	//the left filter controls the DLS 2 parameters
	//	the right filter controls the Parametric EQ parameters
	//		for stereo sounds :
	// both left and right channels must have the same filter
	//	i.e., filter type both is invalid")

	e_platform_sound_source_filter_type type;
	int32  filter_width;

	// Explaination("left filter frequency", "in Hz [0,8000]")
	s_sound_playback_parameter left_filter_frequency;

	// Explaination("left filter gain", "in dB")
	s_sound_playback_parameter left_filter_gain;

	// Explaination("right filter frequency", "in Hz [0,8000]")
	s_sound_playback_parameter right_filter_frequency;

	// Explaination("right filter gain", "in dB")
	s_sound_playback_parameter right_filter_gain;
};
ASSERT_STRUCT_SIZE(s_platform_sound_source_filter, 72);

// max: k_maximum_number_of_override_mixbins
struct s_platform_sound_mixbin
{
	e_platform_sound_mixbin_type type;
	real32 gain_db;
};
ASSERT_STRUCT_SIZE(s_platform_sound_mixbin, 8);

// max: SHORT_MAX
struct s_platform_sound_playback_definition
{
	tag_block<s_platform_sound_mixbin> mixbins;

	c_flags_no_init<e_platform_sound_playback_definition_flags, uint32, k_platform_sound_playback_definition_flag_count> flags;

	int32 pad_1;
	int32 pad_2;

	tag_block<s_platform_sound_source_filter> filter;
	tag_block<s_platform_sound_source_pitch_lfo> pitch_lfo;
	tag_block<s_platform_sound_source_filter_lfo> filter_lfo;
	tag_block<s_sound_effect_definition> sound_effects;
};
ASSERT_STRUCT_SIZE(s_platform_sound_playback_definition, 52);

// max: SHORT_MAX
struct s_cache_file_sound_permutation
{
	uint16 import_name_index;
	uint16 encoded_skip_fraction;
	int8 encoded_gain_db;
	int8 permutation_info_index;
	uint16 language_neutral_time_ms;
	int32 sample_size;
	uint16 first_permutation_chunk_index;
	uint16 permutation_chunk_count;
};
ASSERT_STRUCT_SIZE(s_cache_file_sound_permutation, 16);

// max: SHORT_MAX
struct s_cache_file_sound_pitch_range
{
	uint16 name_index;
	uint16 pitch_parameters_index;
	uint16 encoded_permutation_data;
	uint16 first_runtime_permutation_flag_index;
	uint16 first_permutation_index;
	uint16 permutation_count;
};
ASSERT_STRUCT_SIZE(s_cache_file_sound_pitch_range, 12);

// max: SHORT_MAX
struct s_cache_file_sound_pitch_range_parameters
{
	int16 natural_pitch_cents;

	/// the range of pitches that will be represented using this sample.
	short_bounds bend_bounds_cents;
	short_bounds max_gain_pitch_cents;
};
ASSERT_STRUCT_SIZE(s_cache_file_sound_pitch_range_parameters, 10);

// max: SHORT_MAX
struct s_sound_definition_scale_modifiers
{
	// Explaination("scale modifiers", "as the sound's input scale changes from zero to one,
	// these modifiers move between the two values specified here. the sound will play using the current
	// scale modifier multiplied by the values specified above. (0 values are ignored.)")

	real_bounds gain_modifier_db;
	short_bounds pitch_modifier_cents;
	real_bounds skip_fraction_modifier;
};
ASSERT_STRUCT_SIZE(s_sound_definition_scale_modifiers, 20);

// max: SHORT_MAX
struct s_sound_definition_playback_parameters
{
	/// the distance below which this sound no longer gets louder
	real32 minimum_distance_world_units;

	/// the distance beyond which this sound is no longer audible
	real32 maximum_distance_world_units;

	/// fraction of requests to play this sound that will be ignored (0 means always play.)
	real32 skip_fraction;

	real32 maximum_bend_per_second_cents;

	// Explaination("randomization", "these settings control random variation of volume and pitch.
	// the second parameter gets clipped to the first.")

	/// sound's random gain will start here
	real32 gain_base_db;

	/// sound's gain will be randomly modulated within this range
	real32 gain_variance_db;

	/// the sound's pitch will be modulated randomly within this range.
	short_bounds random_pitch_bounds_cents;

	// Explaination("directional sounds", "these settings allow sounds to be directional, fading as they turn away from the listener")

	/// within the cone defined by this angle and the sound's direction, the sound plays with a gain of 1.0.
	real32 inner_cone_angle_degrees;

	/// outside the cone defined by this angle and the sound's direction, the sound plays with a gain of OUTER_CONE_GAIN. (0 means the sound does not attenuate with direction.)
	real32 outer_cone_angle_degrees;

	/// the gain to use when the sound is directed away from the listener
	real32 outer_cone_gain_db;

	// Explaination("SCRIPTED LOCATION OVERRIDE", "NOTE: this will only apply when the sound is started via script
	// azimuth:
	// 0 => front
	// 90 => left
	// 180 => back
	// 270 => right
	// ")

	c_flags_no_init<e_sound_definition_playback_parameter_flags, uint32, k_sound_definition_playback_parameter_flag_count> flags;
	real32 azimuth;
	real32 positional_gain_db;
	real32 first_person_gain_db;
};
ASSERT_STRUCT_SIZE(s_sound_definition_playback_parameters, 56);

struct s_sound_cache_file_gestalt_definition
{
	tag_block<s_sound_definition_playback_parameters> playbacks;
	tag_block<s_sound_definition_scale_modifiers> scales;

	// max: SHORT_MAX
	tag_block<string_id> import_names;
	tag_block<s_cache_file_sound_pitch_range_parameters> pitch_range_parameters;
	tag_block<s_cache_file_sound_pitch_range> pitch_ranges;
	tag_block<s_cache_file_sound_permutation> permutations;
	tag_block<s_platform_sound_playback_definition> custom_playbacks;

	// max: SHORT_MAX
	tag_block<int8> runtime_permutation_flags;
	tag_block<s_sound_permutation_chunk> chunks;
	tag_block<s_sound_promotion_parameters> promotions;
	tag_block<s_cache_file_sound_definition_extra_info> extra_info;
};
ASSERT_STRUCT_SIZE(s_sound_cache_file_gestalt_definition, 88);

struct runtime_global_sound_gestalt
{
	bool initialized;
	s_sound_definition_playback_parameters* playbacks;
	uint32 playbacks_size;
	s_sound_definition_scale_modifiers* scales;
	uint32 scales_size;
	string_id* import_names;
	uint32 import_names_size;
	s_cache_file_sound_pitch_range_parameters* pitch_range_parameters;
	uint32 pitch_range_parameters_size;
	s_cache_file_sound_pitch_range* pitch_ranges;
	uint32 pitch_ranges_size;
	s_cache_file_sound_permutation* permutations;
	uint32 permutations_size;
	s_platform_sound_playback_definition* custom_playbacks;
	uint32 custom_playbacks_size;
	int8* runtime_permutation_flags;
	uint32 runtime_permutation_flags_size;
	s_sound_permutation_chunk* chunks;
	uint32 chunks_size;
	s_sound_promotion_parameters* promotions;
	uint32 promotions_size;
	s_cache_file_sound_definition_extra_info* extra_info;
	uint32 extra_info_size;
	int8* buffer;
};
ASSERT_STRUCT_SIZE(runtime_global_sound_gestalt, 96);

runtime_global_sound_gestalt* get_runtime_global_sound_gestalt();

void __cdecl initialize_runtime_sound_gestalt_definition();