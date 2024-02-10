#pragma once
#include "simulation_queue.h"
#include "simulation_players.h"

enum e_simulation_queue_player_event_type : uint32
{
	_simulation_queue_player_event_update = 0,
	k_simulation_queue_player_event_count,
	k_simulation_queue_player_event_size_bits = 2
};

struct s_simulation_queue_player_event_data
{
	bool active;
};

void simulation_queue_player_event_insert(e_simulation_queue_player_event_type event_type, datum player_index, const s_simulation_queue_player_event_data* event_data);
void simulation_queue_player_event_apply(const s_simulation_queue_element* element);

void simulation_queue_player_update_insert(const simulation_player_update* player_update);
void simulation_queue_player_update_apply(const s_simulation_queue_element* element);
