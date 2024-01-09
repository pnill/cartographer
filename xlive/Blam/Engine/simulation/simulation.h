#pragma once

#include "simulation_world.h"
#include "simulation_type_collection.h"
#include "game/player_control.h"

void __cdecl simulation_update_discard();
bool simulation_query_object_is_predicted(datum object_datum);
c_simulation_type_collection* simulation_get_type_collection();

void __cdecl simulation_process_input(uint32 player_action_mask, const player_action* player_actions);

void simulation_apply_patches();

c_simulation_world* simulation_get_world();