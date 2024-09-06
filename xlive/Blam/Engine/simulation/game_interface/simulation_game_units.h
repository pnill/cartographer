#pragma once
#include "simulation_game_objects.h"
#include "game/players.h"
#include "units/units.h"

struct s_simulation_unit_state_data
{
	s_simulation_object_state_data object_state_data;
	int16 controlling_player_index;
	int16 controlling_actor_index;
	datum parent_vehicle_entity_index;
	int16 parent_vehicle_seat;
	int16 gap_9A;
	real_vector3d desired_aiming_vector;
	int16 desired_weapon_set_identifier;
	int8 desired_primary_weapon;
	int8 desired_secondary;
	int8 gap_AC[64];
	int8 grenade_count[k_unit_grenade_types_count];
	bool active_camo_active;
	int8 gap_EF;
	real32 active_camo;
	real32 active_camo_regrowth;
};
ASSERT_STRUCT_SIZE(s_simulation_unit_state_data, 0xF8);

struct s_simulation_unit_creation_data
{
	s_simulation_object_creation_data object;
	s_player_profile_traits profile_traits;
	e_game_team team;
	int8 pad1[2];
};
ASSERT_STRUCT_SIZE(s_simulation_unit_creation_data, 36);

void simulation_game_units_apply_patches(void);
