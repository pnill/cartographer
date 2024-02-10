#include "stdafx.h"
#include "simulation.h"

#include "simulation_queue_global_events.h"
#include "simulation_watcher.h"

#include "game/game.h"
#include "objects/objects.h"
#include "simulation_watcher.h"
#include "simulation_entity_database.h"

s_simulation_globals* simulation_get_globals()
{
    return Memory::GetAddress<s_simulation_globals*>(0x5178D0, 0x520B60);
}

c_simulation_world* simulation_get_world()
{
    return simulation_get_globals()->simulation_world;
}

bool simulation_engine_initialized()
{
    return simulation_get_globals()->engine_initialized;
}

bool simulation_is_paused()
{
    return simulation_get_globals()->engine_paused;
}

void simulation_reset()
{
    s_simulation_globals* sim_globals = simulation_get_globals();
    if (sim_globals->simulation_invalidate)
    {
        sim_globals->simulation_invalidate = false;
    }
    else
    {
        sim_globals->simulation_reset = true;
    }
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

void __cdecl simulation_apply_before_game(simulation_update* update)
{
    if (update->simulation_in_progress)
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

    for (int32 i = 0; i < k_maximum_players; i++)
    {
        datum control_unit_index = update->control_unit_index[i];
        if (TEST_BIT(update->unit_control_mask, i) && 
            object_try_and_get_and_verify_type(control_unit_index, FLAG(_object_type_biped) | FLAG(_object_type_vehicle)))
        {
            unit_control(control_unit_index, &update->unit_control[i]);
        }
    }
    
    if (update->machine_update_valid)
    {
        players_set_machines(update->machine_update.machine_valid_mask, update->machine_update.identifiers);
    }

    if (update->flush_gamestate)
    {
        simulation_get_globals()->simulation_world->gamestate_flush();
    }
    return;
}

typedef void (__cdecl* t_simulation_update_pregame)();
t_simulation_update_pregame p_simulation_update_pregame;

void __cdecl simulation_update_pregame()
{
    p_simulation_update_pregame();
    if (simulation_engine_initialized() && game_in_progress() && !simulation_is_paused())
    {
        if (simulation_get_globals()->simulation_watcher->need_to_generate_updates())
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

bool __cdecl simulation_get_machine_active_in_game(s_machine_identifier* machine_identifier)
{
    return INVOKE(0x1AE0CB, 0x1A8482, simulation_get_machine_active_in_game, machine_identifier);
}

void __cdecl simulation_build_player_updates(int32* player_update_count, int32 maximum_player_update_count, simulation_player_update* player_updates)
{
    simulation_get_globals()->simulation_watcher->generate_player_updates(player_update_count, maximum_player_update_count, player_updates);
    for (int32 i = 0; i < *player_update_count; i++)
    {
        simulation_queue_player_update_insert(&player_updates[i]);
    }
    return;
}

void simulation_apply_patches(void)
{
    // ### TODO move somewhere else, network related
    network_memory_apply_patches();

    simulation_event_handler_apply_patches();
    simulation_world_apply_patches();
    simulation_entity_database_apply_patches();
    PatchCall(Memory::GetAddress(0x4A4DF, 0x4375D), simulation_apply_before_game);
    PatchCall(Memory::GetAddress(0x1AEA23, 0x1A8C7D), simulation_apply_before_game);
    
    DETOUR_ATTACH(p_simulation_update_pregame, Memory::GetAddress<t_simulation_update_pregame>(0x1AE9D3, 0x1A8C2D), simulation_update_pregame);

    PatchCall(Memory::GetAddress(0x1DD22F, 0x1C46E3), simulation_build_player_updates);
    return;
}