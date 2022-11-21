#pragma once
#include "SimulationGameObjects.h"
#include "Blam/Engine/Game/game/players.h"

#define k_orignal_unit_creation_size  0x24
struct s_simulation_unit_state_data
{
	s_simulation_object_state_data object_state_data;
	WORD controlling_player_index;
	WORD controlling_actor_index;
	DWORD parent_vehicle_entity_index;
	WORD parent_vehicle_seat;
	WORD gap_9A;
	real_vector3d desired_aiming_vector;
	__int16 desired_weapon_set_identifier;
	char desired_primary_weapon;
	char desired_secondary;
	char gap_AC[64];
	WORD grenade_counts_mask;
	bool active_camo_active;
	char gap_EF;
	float active_camo;
	float active_camo_regrowth;
};
CHECK_STRUCT_SIZE(s_simulation_unit_state_data, 0xF8);

struct s_simulation_unit_creation_data
{
	s_simulation_object_creation_data object;
	s_player::s_player_properties::s_player_profile profile;
	PAD(8);
	WORD team;
	PAD(2);

	//New data
	string_id variant_name;
};

CHECK_STRUCT_SIZE(s_simulation_unit_creation_data, k_orignal_unit_creation_size + sizeof(string_id));
