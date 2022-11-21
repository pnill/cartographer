#pragma once
#include "Blam/Common/Common.h"
#include "Blam/Engine/Game/math/real_math.h"

struct s_simulation_object_state_data
{
	real_point3d position;
	real_vector3d forward;
	real_vector3d up;
	int scale;
	real_vector3d translational_velocity;
	real_vector3d angular_velocity;
	int body_vitality;
	char body_stun_ticks_is_zero;
	char gap45[3];
	int shield_vitality;
	char shield_stun_ticks_is_zero;
	char dead;
	char gap4D[2];
	char region_count;
	char region_states[16];
	char constraint_count;
	__int16 destroyed_constraints;
	__int16 loosened_constraints;
	char pad[42];
};
CHECK_STRUCT_SIZE(s_simulation_object_state_data, 0x90);

struct s_simulation_object_creation_data
{
	datum object_scenario_datum_index;
	datum object_definition_index;
	char multiplayer_spawn_monitor_index;
	char gap_9[3];
	char emblem_info_foreground_index;
	char emblem_info_background_index;
	char emblem_info_flags;
};
CHECK_STRUCT_SIZE(s_simulation_object_creation_data, 0x10);
