#pragma once
#include "game_sound_definitions.h"

/* enums */

enum e_sound_mixbin
{
	_platform_sound_mixbin_front_left = 0,
	_platform_sound_mixbin_front_right,
	_platform_sound_mixbin_back_left,
	_platform_sound_mixbin_back_right,
	_platform_sound_mixbin_center,
	_platform_sound_mixbin_low_frequency,
	_platform_sound_mixbin_reverb,
	_platform_sound_mixbin_3d_front_left,
	_platform_sound_mixbin_3d_front_right,
	_platform_sound_mixbin_3d_back_left,
	_platform_sound_mixbin_3d_back_right,
	_platform_sound_mixbin_default_left_speakers,
	_platform_sound_mixbin_default_right_speakers,
	k_platform_sound_mixbin_type_count
};

enum e_sound_filter_type
{
	_sound_filter_type_parametric_eq = 0,
	_sound_filter_type_dls2 = 1,
	_sound_filter_type_both_only_valid_for_mono = 2,
	k_sound_filter_type_count
};

enum e_platform_sound_playback_definition_flags
{
	_platform_sound_playback_radio_hack_bit = 0,
	k_number_of_platform_sound_playback_bits
};

/* structures */

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

	e_sound_filter_type type;
	int32 filter_width;

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
	e_sound_mixbin type;
	real32 gain_db;
};
ASSERT_STRUCT_SIZE(s_platform_sound_mixbin, 8);

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

// max: SHORT_MAX
struct s_platform_sound_playback_definition
{
	s_tag_block mixbins;		// s_platform_sound_mixbin

	c_flags_no_init<e_platform_sound_playback_definition_flags, uint32, k_number_of_platform_sound_playback_bits> flags;

	int32 pad_1;
	int32 pad_2;

	s_tag_block filter;			// s_platform_sound_source_filter
	s_tag_block pitch_lfo;		// s_platform_sound_source_pitch_lfo
	s_tag_block filter_lfo;		// s_platform_sound_source_filter_lfo
	s_tag_block sound_effects;	// s_sound_effect_definition
};
ASSERT_STRUCT_SIZE(s_platform_sound_playback_definition, 52);

