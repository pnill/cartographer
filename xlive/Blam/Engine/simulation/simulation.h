#pragma once

#include "simulation_world.h"
#include "simulation_type_collection.h"
#include "simulation_watcher.h"
#include "game/player_control.h"

// TODO structure?
struct s_simulation_update
{
	int gap_0;
	bool simulation_in_progress;
};

struct s_simulation_globals
{
	bool engine_initialized;
	bool field_1;
	bool engine_paused;
	int32 field_4;
	bool simulation_invalidate;
	bool simulation_reset;
	bool simulation_reset_in_progress;
	bool field_B;
	c_simulation_world* simulation_world;
	c_simulation_watcher* simulation_watcher;
	c_simulation_type_collection* simulation_type_collection;
};
CHECK_STRUCT_SIZE(s_simulation_globals, 24);

void simulation_reset();
bool simulation_in_progress();
void simulation_destroy_update();
bool simulation_query_object_is_predicted(datum object_datum);
c_simulation_type_collection* simulation_get_type_collection();

void __cdecl simulation_process_input(uint32 player_action_mask, const player_action* player_actions);

void simulation_apply_patches();

c_simulation_world* simulation_get_world();