#pragma once

/* enums */

enum e_dialogue_type : int16
{
	_dialogue_type_none = 0,
	_dialogue_type_advance = 1,
	_dialogue_type_charge = 2,
	_dialogue_type_fall_back = 3,
	_dialogue_type_retreat = 4,
	_dialogue_type_moveone = 5,
	_dialogue_type_arrival = 6,
	_dialogue_type_enter_vehicle = 7,
	_dialogue_type_exit_vehicle = 8,
	_dialogue_type_follow_player = 9,
	_dialogue_type_leave_player = 10,
	_dialogue_type_support = 11
};

/* prototypes */

void __cdecl ai_dialogue_initialize_for_new_map(void);
