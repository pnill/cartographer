#pragma once
#include "simulation_encoding.h"
#include "simulation_type_collection.h"
#include "simulation_watcher.h"
#include "simulation_world.h"

#include "game/player_control.h"

#define k_maximum_simulation_player_updates 64

struct alignas(8) simulation_update
{
	int32 simulation_time;
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
	int32 game_time_ticks;
	uint32 random_seed;
};
CHECK_STRUCT_SIZE(simulation_update, 0x3BD8);

struct s_simulation_globals
{
	bool engine_initialized;
	bool fatal_error;
	bool engine_paused;
	int32 field_4;
	bool simulation_invalidate;
	bool simulation_reset_pending;
	bool simulation_reset_in_progress;
	bool field_B;
	c_simulation_world* world;
	c_simulation_watcher* simulation_watcher;
	c_simulation_type_collection* simulation_type_collection;
};
CHECK_STRUCT_SIZE(s_simulation_globals, 24);

c_simulation_world* simulation_get_world();
s_simulation_globals* simulation_get_globals();

void simulation_reset();
bool simulation_reset_in_progress();
bool simulation_starting_up(void);
void simulation_notify_reset_complete();

bool simulation_in_progress();
void simulation_destroy_update(void);
bool simulation_query_object_is_predicted(datum object_datum);
c_simulation_type_collection* simulation_get_type_collection();

void __cdecl simulation_process_input(uint32 player_action_mask, const player_action* player_actions);

bool __cdecl simulation_get_machine_active_in_game(s_machine_identifier* machine_identifier);

void simulation_apply_patches(void);
