#include "stdafx.h"
#include "simulation.h"

#include "game/game.h"
#include "objects/objects.h"

#include "Util/Hooks/Hook.h"

c_simulation_world* simulation_get_world()
{
	return *Memory::GetAddress<c_simulation_world**>(0x5178DC, 0x520B6C);
}

bool simulation_query_object_is_predicted(datum object_datum)
{
    return game_is_predicted() && object_get_fast_unsafe(object_datum)->simulation_entity_index != NONE;
}

void __cdecl simulation_process_input(uint32 player_action_mask, const player_action* player_actions)
{
    INVOKE(0x1ADDA9, 0x1A8160, simulation_process_input, player_action_mask, player_actions);
    return;
}

c_simulation_type_collection* simulation_get_type_collection()
{
    return c_simulation_type_collection::get();
}

typedef void(__cdecl* t_simulation_update_before_game)(int8* sim_data_out);
t_simulation_update_before_game p_simulation_update_before_game;

void __cdecl simulation_update_before_game_hook(int8* sim_data_out)
{
    p_simulation_update_before_game(sim_data_out);
    simulation_get_world()->apply_entity_update_queue();
    simulation_get_world()->apply_event_update_queue();
}


void simulation_update_discard()
{
    // remove everything from the queue
    simulation_get_world()->destroy_update();
}

void simulation_apply_patches()
{
    c_simulation_world::queues_initialize();
    simulation_event_handler_apply_patches();
    DETOUR_ATTACH(p_simulation_update_before_game, Memory::GetAddress<t_simulation_update_before_game>(0x1AE902, 0x1A8B5C), simulation_update_before_game_hook);
}