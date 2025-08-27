#pragma once

#include "tag_files/string_id.h"

#define MAXIMUM_POSTURES_PER_UNIT 20

enum e_action_state_throw_state : __int8
{
	_throw_idle = 0x0,
	_throw_wind_up = 0x1,
	_throw_in_hand = 0x2,
	_throw_ending = 0x3,
	k_throw_state_count = 0x4,
};

struct s_action_state
{
	int8 data[12];
	e_action_state_throw_state throw_state;
	bool throw_predicted;
	int8 throw_ticks;
	int8 throw_full_power_ticks;
	datum throw_grenade_object;
	int16 throw_grenade_index;
	int8 data14[46];
};
ASSERT_STRUCT_SIZE(s_action_state, 68);

// max count: MAXIMUM_POSTURES_PER_UNIT 20
struct s_posture_definition
{
	string_id name;
	real_vector3d pill_offset;
};
ASSERT_STRUCT_SIZE(s_posture_definition, 16);

void unit_action_system_apply_patches();
