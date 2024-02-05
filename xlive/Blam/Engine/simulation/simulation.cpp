#include "stdafx.h"
#include "simulation.h"

#include "game/game.h"
#include "objects/objects.h"
#include "simulation_watcher.h"

#include "Util/Hooks/Hook.h"

c_simulation_world* simulation_get_world()
{
	return *Memory::GetAddress<c_simulation_world**>(0x5178DC, 0x520B6C);
}

bool simulation_engine_initialized()
{
    return *Memory::GetAddress<bool*>(0x5178D0, 0x520B60);
}

bool simulation_is_paused()
{
    return *Memory::GetAddress<bool*>(0x5178D2, 0x520B62);
}

bool simulation_in_progress()
{
    bool result = false;

    if (simulation_engine_initialized()
        && game_in_progress()
        && game_get_active_structure_bsp_index() != NONE
        && !simulation_is_paused()
        && simulation_get_world()->is_active())
    {
        result = true;
    }

    return result;
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

typedef void(__cdecl* t_simulation_update_before_game)(s_simulation_update* sim_data);
t_simulation_update_before_game p_simulation_update_before_game;

void __cdecl simulation_update_before_game_hook(s_simulation_update* sim_data)
{
    if (sim_data->simulation_in_progress)
    {
        simulation_get_world()->apply_high_priority_queue();
        simulation_get_world()->apply_basic_queue();

        // destroy the update exactly after we applied the queues to the gamestate
        simulation_get_world()->destroy_update();
    }
    else
    {
        // purge any deletion pending object during this update
        // if simulation is not in progress
        objects_purge_deleted_objects();
    }

    p_simulation_update_before_game(sim_data);
}

typedef void (__cdecl* t_simulation_update_pregame)();
t_simulation_update_pregame p_simulation_update_pregame;

void __cdecl simulation_update_pregame()
{
    p_simulation_update_pregame();
    if (simulation_engine_initialized() && game_in_progress() && !simulation_is_paused())
    {
        if (c_simulation_watcher::get()->need_to_generate_updates())
        {
            simulation_destroy_update();
        }
    }
}

void simulation_destroy_update()
{
    // only discard these updates if they were actually applied
    // during simulation
    if (simulation_in_progress())
    {
        // remove everything from the queue
		// simulation_get_world()->destroy_update();
    }
}

void simulation_apply_patches()
{
    // ### TODO move somewhere else, network related
    network_memory_apply_patches();

    simulation_event_handler_apply_patches();
    simulation_world_apply_patches();
    DETOUR_ATTACH(p_simulation_update_before_game, Memory::GetAddress<t_simulation_update_before_game>(0x1AE902, 0x1A8B5C), simulation_update_before_game_hook);
    DETOUR_ATTACH(p_simulation_update_pregame, Memory::GetAddress<t_simulation_update_pregame>(0x1AE9D3, 0x1A8C2D), simulation_update_pregame);
}