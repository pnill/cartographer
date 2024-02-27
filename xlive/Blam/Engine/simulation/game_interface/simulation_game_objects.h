#pragma once

#include "Blam/Engine/objects/emblems.h"
#include "Blam/Engine/objects/object_placement.h"
#include "Blam/Engine/tag_files/string_id.h"

struct s_simulation_object_state_data
{
	real_point3d position;
	real_vector3d forward;
	real_vector3d up;
	real32 scale;
	real_vector3d translational_velocity;
	real_vector3d angular_velocity;
	real32 body_vitality;
	char body_stun_ticks_is_zero;
	char gap45[3];
	real32 shield_vitality;
	char shield_stun_ticks_is_zero;
	char dead;
	char gap4D[2];
	char region_count;
	char region_states[16];
	uint8 constraint_count;
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
	int8 model_variant_index;
};
CHECK_STRUCT_SIZE(s_simulation_object_creation_data, 16);

datum simulation_object_get_replicated_object_from_entity(int32 entity_index);

bool __stdcall c_simulation_object_entity_definition__object_setup_placement_data(void* _this,
	s_simulation_object_creation_data* object_creation_data,
	s_simulation_object_state_data* state_data,
	uint32* flags,
	object_placement_data* placement_data);

datum __fastcall c_simulation_object_entity_definition__object_create_object(void* _this,
	s_simulation_object_creation_data* object_creation,
	s_simulation_object_state_data* state_data,
	uint32* flags,
	object_placement_data* placement_data);

void simulation_game_objects_apply_patches(void);
