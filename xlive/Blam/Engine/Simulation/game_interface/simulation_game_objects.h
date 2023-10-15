#pragma once
#include "Blam/Math/real_math.h"
#include "Blam/Engine/tag_files/string_id.h"

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
	int8 multiplayer_spawn_monitor_index;
	// unused padding
	char pad[3];

	s_emblem_info emblem_info;
	
	// Repurpose padding for variant index
	int8 model_variant_id;
};
CHECK_STRUCT_SIZE(s_simulation_object_creation_data, 16);
