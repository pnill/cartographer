#pragma once
#include "simulation_players.h"

#include "game/players.h"
#include "networking/network_constants.h"
#include "units/unit_control.h"

/* constants */

enum
{
	k_maximum_simulation_player_updates = 64,
	k_bits_required_for_simulation_player_updates_count = 5
};

/* structures */

struct simulation_machine_update
{
	uint32 machine_valid_mask;
	s_machine_identifier identifiers[k_network_maximum_machines_per_session];
};

struct simulation_update
{
	int32 update_number;
	bool simulation_in_progress;
	uint32 player_action_mask;
	int32 field_C;
	player_action player_actions[k_maximum_players];
	uint32 unit_control_mask;
	datum control_unit_index[k_maximum_players];
	unit_control_data unit_control[k_maximum_players];
	bool machine_update_valid;
	simulation_machine_update machine_update;
	int32 player_update_count;
	simulation_player_update player_updates[k_maximum_simulation_player_updates];
	bool flush_gamestate;
	int32 verify_game_time;
	uint32 verify_random_seed;
};
ASSERT_STRUCT_SIZE(struct simulation_update, 0x3BD8);
