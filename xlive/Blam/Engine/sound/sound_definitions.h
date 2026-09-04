#pragma once
#include "game_sound_definitions.h"
#include "sound_definition_globals.h"

/* constants */


/* macros */

#define sound_definition_get(index)	((struct sound_definition_v1*)tag_get(SOUND_DEFINITION_TAG, (index)))

/* enums */

enum e_sound_definition_compression : uint8
{
	_sound_definition_compression_none_big_endian = 0,
	_sound_definition_compression_xbox_adpcm = 1,
	_sound_definition_compression_ima_adpcm = 2,
	_sound_definition_compression_none_little_endian = 3,
	_sound_definition_compression_wma = 4
};

enum e_sound_definition_encoding : uint8
{
	_sound_definition_encoding_mono = 0,
	_sound_definition_encoding_stereo = 1,
	_sound_definition_encoding_codec = 2
};

enum e_sound_definition_sample_rate : uint8
{
	_sound_definition_sample_rate_22khz = 0,
	_sound_definition_sample_rate_44khz = 1,
	_sound_definition_sample_rate_32khz = 2
};

enum e_sound_definition_flags : uint16
{
	_sound_definition_flag_fit_to_adpcm_block_size = 0,
	_sound_definition_flag_split_long_sound_into_permutations = 1,
	_sound_definition_flag_always_spatialize_always_play_as_3d_sound_even_in_first_person = 2,
	_sound_definition_flag_never_obstruct_disable_occlusion_obstruction = 3,
	_sound_definition_flag_internal_dont_touch = 4,
	_sound_definition_flag_use_huge_sound_transmission = 5,
	_sound_definition_flag_link_count_to_owner_unit = 6,
	_sound_definition_flag_pitch_range_is_language = 7,
	_sound_definition_flag_dont_use_sound_class_speaker_flag = 8,
	_sound_definition_flag_dont_use_lipsync_data = 9,
	k_sound_definition_flag_count
};

enum e_sound_playback_flag
{
	_sound_playback_override_azimuth_bit = 0,
	_sound_playback_override_3d_gain_bit,
	_sound_playback_bit2,
	_sound_playback_override_speaker_gain_bit,
	k_sound_definition_playback_parameter_flag_count
};

/* structures */

// max count: 1
struct s_sound_globals_definition
{
	tag_reference sound_classes;                    // sncl
	tag_reference sound_effects;                    // sfx+
	tag_reference sound_mix;                        // snmx
	tag_reference sound_combat_dialogue_constants;  // spk!
	datum runtime_gestalt_sound_definition_index;	// ugh!
};
ASSERT_STRUCT_SIZE(s_sound_globals_definition, 36);

struct s_sound_parameter_range_shorts
{
	int16 lower_bound;
	int16 upper_bound;
};

struct s_sound_parameter_range
{
	real32 lower_bound;
	real32 upper_bound;
};

struct s_sound_parameter_range_decibels
{
	real_decibel lower_bound_db;
	real_decibel upper_bound_db;
};

struct sound_definition_v1
{
	c_flags_no_init<e_sound_definition_flags, uint16, k_sound_definition_flag_count> flags;

	int8/*e_sound_class_type*/ sound_class;
	e_sound_definition_sample_rate sample_rate;
	e_sound_definition_encoding encoding;
	e_sound_definition_compression compression;

	int16 gestalt_playback_parameter_index;
	int16 gestalt_pitch_range_index;
	int8 gestalt_unknown_index;
	int8 gestalt_scale_index;
	int8 gestalt_promotion_index;
	int8 gestalt_custom_playback_index;
	int16 gestalt_extra_info_index;
	uint32 maximum_play_time;
};
ASSERT_STRUCT_SIZE(sound_definition_v1, 20);

// max: MAXIMUM_CHUNKS_PER_PERMUTATION
struct s_sound_permutation_chunk
{
	uint32 file_offset;
	uint16 flags;
	uint16 size;
	int32 runtime_index;
};
ASSERT_STRUCT_SIZE(s_sound_permutation_chunk, 12);

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
	tag_data encoded_data;

	s_tag_block sound_permutation_dialogue_info;	// s_sound_permutation_dialogue_info
};
ASSERT_STRUCT_SIZE(s_sound_permutation_dialogue_info, 16)

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
	real_decibel gain_base_db;

	/// sound's gain will be randomly modulated within this range
	real_decibel gain_variance_db;

	/// the sound's pitch will be modulated randomly within this range.
	short_bounds random_pitch_bounds_cents;

	// Explaination("directional sounds", "these settings allow sounds to be directional, fading as they turn away from the listener")

	/// within the cone defined by this angle and the sound's direction, the sound plays with a gain of 1.0.
	real32 inner_cone_angle_degrees;

	/// outside the cone defined by this angle and the sound's direction, the sound plays with a gain of OUTER_CONE_GAIN. (0 means the sound does not attenuate with direction.)
	real32 outer_cone_angle_degrees;

	/// the gain to use when the sound is directed away from the listener
	real_decibel outer_cone_gain_db;

	// Explaination("SCRIPTED LOCATION OVERRIDE", "NOTE: this will only apply when the sound is started via script
	// azimuth:
	// 0 => front
	// 90 => left
	// 180 => back
	// 270 => right
	// ")

	c_flags_no_init<e_sound_playback_flag, uint32, k_sound_definition_playback_parameter_flag_count> flags;
	real32 azimuth;
	real_decibel positional_gain_db;
	real_decibel first_person_gain_db;
};
ASSERT_STRUCT_SIZE(s_sound_definition_playback_parameters, 56);

// max: SHORT_MAX
struct s_sound_definition_scale_modifiers
{
	// Explaination("scale modifiers", "as the sound's input scale changes from zero to one,
	// these modifiers move between the two values specified here. the sound will play using the current
	// scale modifier multiplied by the values specified above. (0 values are ignored.)")

	s_sound_parameter_range_decibels gain_bounds;
	s_sound_parameter_range_shorts pitch_bounds;
	s_sound_parameter_range skip_fraction_bounds;
};
ASSERT_STRUCT_SIZE(s_sound_definition_scale_modifiers, 20);

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
	s_tag_block rules;			// s_sound_promotion_rule
	s_tag_block runtime_timers;	// real32

	int32 runtime_active_promotion_index;
	int32 runtime_last_promotion_time;
	int32 runtime_suppression_timeout;
};
ASSERT_STRUCT_SIZE(s_sound_promotion_parameters, 28);

