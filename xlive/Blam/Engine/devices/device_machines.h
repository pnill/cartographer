#pragma once
#include "devices.h"

struct machine_datum
{
	device_datum device;
	uint32 flags;
	int32 door_open_time_ticks;
	int16 pathfinding_object_index;
	int8 pad[4];
	int16 game_portal;
	int16 field_1DE;
	int16 pad1;
};
CHECK_STRUCT_SIZE(machine_datum, 480);