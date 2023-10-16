#pragma once
#include "simulation_game_entities.h"
#include "simulation_game_objects.h"
#include "Blam/Engine/game/players.h"


struct s_simulation_unit_state_data
{
	s_simulation_object_state_data object_state_data;
	short controlling_player_index;
	short controlling_actor_index;
	datum parent_vehicle_entity_index;
	short parent_vehicle_seat;
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
	s_player_profile_traits profile_traits;
	e_game_team team;
	int8 pad1[2];
};
CHECK_STRUCT_SIZE(s_simulation_unit_creation_data, 36);

void simulation_game_units_apply_patches(void);
