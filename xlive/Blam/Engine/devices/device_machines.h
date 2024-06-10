#pragma once
#include "devices.h"

#include "memory/static_arrays.h"

enum e_machine_data_flags : uint32
{
	_machine_does_not_operate_automatically = 0,
	_machine_does_not_close_automatically = 1,
	_machine_is_one_sided = 2,
	_machine_is_onesided_for_player = 3,
	_machine_never_appears_locked = 4,
	_machine_opened_by_melee_attack = 5,
	k_machine_data_flags_count
};

struct machine_datum
{
	device_datum device;
	c_flags_no_init<e_machine_data_flags, uint32, k_machine_data_flags_count> flags;
	uint32 flags;
	int32 door_open_time_ticks;
	int16 pathfinding_object_index;
	int8 pad[4];
	int16 game_portal;
	int16 field_1DE;
	int16 pad1;
};
CHECK_STRUCT_SIZE(machine_datum, 480);
