#pragma once
#include "simulation_queue.h"
#include "simulation_players.h"

enum e_simulation_queue_player_event_type : uint32
{
	_simulation_queue_player_event_update = 0,
	k_simulation_queue_player_event_count,
	k_simulation_queue_player_event_size_bits = 2
};

enum e_simulation_queue_global_event_type
{
	_simulation_queue_game_global_event_type_claim_authority,
	_simulation_queue_game_global_event_type_set_simulation_to_distributed_server,
	_simulation_queue_game_global_event_type_set_simulation_to_distributed_client,
	_simulation_queue_game_global_event_type_game_won,
	_simulation_queue_game_global_event_main_revert_map,
	_simulation_queue_game_global_event_main_reset_map,
	_simulation_queue_game_global_event_main_save_and_exit_campaign,
	_simulation_queue_game_global_event_notify_reset_complete,
};

struct s_simulation_queue_player_event_data
{
	bool active;
};

void simulation_queue_player_event_insert(e_simulation_queue_player_event_type event_type, datum player_index, const s_simulation_queue_player_event_data* event_data);
void simulation_queue_player_event_apply(const s_simulation_queue_element* element);

void simulation_queue_player_update_insert(const simulation_player_update* player_update);
void simulation_queue_player_update_apply(const s_simulation_queue_element* element);

void simulation_queue_game_global_event_insert(e_simulation_queue_global_event_type global_event_type);
void simulation_queue_game_global_event_apply(const s_simulation_queue_element* element);
