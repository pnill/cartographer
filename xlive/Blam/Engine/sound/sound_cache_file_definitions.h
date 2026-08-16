#pragma once
#include "geometry/geometry_block.h"

/* constants */


enum
{
	SOUND_CACHE_FILE_GESTALT_TAG = 'ugh!',
	SOUND_CACHE_FILE_GESTALT_VERSION = 1,
	CACHE_FILE_SOUND_DEFINITION_TAG = '$#!+',
	CACHE_FILE_SOUND_DEFINITION_VERSION = 1,
};

/* macros */

/* enums */

enum e_sound_effect_component_definition_flags : uint32
{
	_sound_effect_component_definition_flag_dont_play_at_start = 0,
	_sound_effect_component_definition_flag_play_on_stop = 1,
	_sound_effect_component_definition_flag_play_alternate = 2,
	_sound_effect_component_definition_flag_sync_with_origin_looping_sound = 3,
	k_sound_effect_component_definition_count
};

/* structures */

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
struct s_cache_file_sound_definition_extra_info
{
	s_tag_block encoded_permutations;				// s_sound_encoded_dialogue_section
	geometry_block_info geometry_block_info;
};
ASSERT_STRUCT_SIZE(s_cache_file_sound_definition_extra_info, 44);

/* prototypes */

void initialize_runtime_sound_gestalt_definition(void);
