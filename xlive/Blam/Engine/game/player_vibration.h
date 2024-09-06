#pragma once
#include "game/players.h"
#include "math/function_definitions.h"

#define k_count_of_effects_that_effect_vibration 8 

struct s_vibration_effect_globals
{
	datum damage_effect_index;
	int32 response_index;
	real32 field_8;
};

struct s_vibration_user_globals
{
	s_vibration_effect_globals effects[k_count_of_effects_that_effect_vibration];
	real32 duration[k_count_of_effects_that_effect_vibration];
	real32 intensity[2];	// Index 0 is left and index 1 is right
};

struct s_vibration_globals
{
	s_vibration_user_globals user_data[k_number_of_users];
	real32 max_vibration_left;
	real32 max_vibration_right;
	real32 scripted_scale;
};
ASSERT_STRUCT_SIZE(s_vibration_globals, 556);

struct s_vibration_frequency_definition
{
	real32 duration;
	c_function_definition dirty_whore;	// Bungie's name for this not mine
};
ASSERT_STRUCT_SIZE(s_vibration_frequency_definition, 12);

struct s_vibration_definition
{
	// Explaination("low frequency vibration", "EMPTY STRING")
	// Explaination("high frequency vibration", "EMPTY STRING")
	// Index 0 = low || Index 1 = high
	s_vibration_frequency_definition frequency_vibration[2];
};
ASSERT_STRUCT_SIZE(s_vibration_definition, 24);

void __cdecl rumble_player_set_scripted_scale(real32 scale);

void __cdecl rumble_player_continuous(int32 user_index, real32 rumble_intensity_left, real32 rumble_intensity_right);

void player_vibration_apply_patches(void);

void __cdecl vibration_update(real32 dt);

