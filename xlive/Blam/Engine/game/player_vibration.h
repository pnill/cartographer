#pragma once
#include "math/function_definitions.h"
#include "Networking/Session/NetworkSession.h"

struct s_vibration_effect_globals
{
	int32 field_0;
	datum damage_effect_index;
	real32 field_8;
};

struct s_vibration_user_globals
{
	s_vibration_effect_globals effects[8];
	real32 field_60[8];
	real32 intensity_left;
	real32 intensity_right;
};

struct s_vibration_globals
{
	s_vibration_user_globals user_data[k_number_of_users];
	real32 max_vibration_left;
	real32 max_vibration_right;
	real32 scripted_scale;
};

struct s_vibration_frequency_definition
{
	real32 duration;
	c_function_definition dirty_whore;	// Bungie's name for this not mine
};

struct s_vibration_definition
{
	// Explaination("low frequency vibration", "EMPTY STRING")
	s_vibration_frequency_definition low_frequency_vibration;

	// Explaination("high frequency vibration", "EMPTY STRING")
	s_vibration_frequency_definition high_frequency_vibration;
};

void __cdecl rumble_player_set_scripted_scale(real32 scale);

void __cdecl rumble_player_continuous(int32 user_index, real32 rumble_intensity_left, real32 rumble_intensity_right);

void __cdecl vibration_update(real32 dt);
