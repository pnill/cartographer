#include "stdafx.h"
#include "simulation.h"

#include "simulation_queue_global_events.h"
#include "simulation_entity_database.h"
#include "simulation_event_handler.h"
#include "simulation_update.h"
#include "simulation_watcher.h"
#include "simulation_world.h"

#include "cartographer/discord/discord_interface.h"
#include "game/game.h"
#include "game/game_time.h"
#include "game/players.h"
#include "networking/logic/life_cycle_manager.h"
#include "networking/session/network_session.h"
#include "networking/network_event.h"
#include "objects/objects.h"
#include "shell/shell.h"
#include "units/units.h"
#include "simulation/game_interface/simulation_game_action.h"

#include "H2MOD/Modules/EventHandler/EventHandler.hpp"


/* structures */

struct s_simulation_globals
{
	bool initialized;
	bool simulation_fatal_error;
	bool engine_paused;
	int32 field_4;
	bool simulation_invalidate;
	bool simulation_reset_pending;
	bool simulation_reset_in_progress;
	bool loading_saved_game;
	class c_simulation_world* world;
	class c_simulation_watcher* watcher;
	class c_simulation_type_collection* type_collection;
};
ASSERT_STRUCT_SIZE(s_simulation_globals, 24);

/* prototypes */

static s_simulation_globals* simulation_get_globals(void);

static void simulation_player_joined_game_patch_calls(void);
static void simulation_player_left_game_patch_calls(void);

/* public code */

void simulation_apply_patches(void)
{
	simulation_event_handler_apply_patches();
	simulation_world_apply_patches();
	simulation_entity_database_apply_patches();
	simulation_game_action_apply_patches();

	PatchCall(Memory::GetAddress(0x1DD22F, 0x1C46E3), simulation_build_player_updates);	// c_simulation_world::build_update

	WriteJmpTo(Memory::GetAddress(0x1AE6D8, 0x1A8932), simulation_reset);

	simulation_player_joined_game_patch_calls();
	simulation_player_left_game_patch_calls();
	return;
}

void __cdecl simulation_player_joined_game(
	int32 player_index)
{
	s_simulation_globals* simulation_globals = simulation_get_globals();

	ASSERT(simulation_globals->world);

	if (simulation_globals->initialized && !simulation_globals->loading_saved_game)
	{
		simulation_globals->world->create_player(player_index);
		if (!shell_is_dedicated_server())
		{
			// Update discord player counts
			discord_interface_set_player_counts();
		}
	}

	// Remove this when new custom variant settings are finished
	c_network_session* session = NULL;

	if (network_life_cycle_in_squad_session(&session))
	{
		EventHandler::NetworkPlayerEventExecute(EventExecutionType::execute_after, session->get_player_membership(player_index)->peer_index, EventHandler::NetworkPlayerEventType::add);
	}

	return;
}

void __cdecl simulation_player_left_game(
	int32 player_index)
{
	s_simulation_globals* simulation_globals = simulation_get_globals();

	ASSERT(simulation_globals->world);

	if (simulation_globals->initialized && !simulation_globals->loading_saved_game)
	{
		simulation_globals->world->delete_player(player_index);
		if (!shell_is_dedicated_server())
		{
			// Update discord player counts
			discord_interface_set_player_counts();
		}
	}

	return;
}

c_simulation_world* simulation_get_world()
{
	return simulation_get_globals()->world;
}

bool simulation_engine_initialized()
{
	return simulation_get_globals()->initialized;
}

bool simulation_is_paused()
{
	return simulation_get_globals()->engine_paused;
}

bool simulation_reset_in_progress(void)
{
	return simulation_get_globals()->simulation_reset_in_progress;
}

void __cdecl simulation_update(void)
{
	INVOKE(0x1AE7C5, 0x1A8A1F, simulation_update);
	return;
}

bool simulation_starting_up(void)
{
	const s_simulation_globals* simulation_globals = simulation_get_globals();

	bool result = false;
	if (simulation_globals->initialized)
	{
		ASSERT(simulation_globals->world);
		if (!simulation_globals->engine_paused && simulation_globals->world->exists())
		{
			result = !simulation_globals->world->is_active();
		}
	}

	return result;
}

void simulation_notify_reset_complete(void)
{
	s_simulation_globals* sim_globals = simulation_get_globals();
	if (!game_is_playback())
	{
		sim_globals->world->send_player_acknowledgements(true);
	}
	sim_globals->simulation_reset_in_progress = false;
	return;
}

void simulation_notify_reset_initiate(void)
{
	s_simulation_globals* sim_globals = simulation_get_globals();
	sim_globals->simulation_reset_in_progress = true;
	return;
}

void simulation_notify_going_active(void)
{
	if (game_is_campaign() && game_is_cooperative())
	{
		players_update_for_checkpoint();
	}
	return;
}

void simulation_reset_immediate()
{
	s_simulation_globals* simulation_globals = simulation_get_globals();

	ASSERT(simulation_globals->world);
	ASSERT(simulation_globals->world->exists());
	ASSERT(!simulation_globals->world->is_authority());

	simulation_globals->simulation_reset_in_progress = true;
	simulation_globals->world->reset_world();
	simulation_queue_game_global_event_insert(_simulation_queue_game_global_event_type_reset_map);
	// ### TODO figure out these
	// simulation_gamestate_entities_build_clear_flags();
	// simulation_queue_gamestates_delete_insert();
	simulation_queue_game_global_event_insert(_simulation_queue_game_global_event_type_simulation_reset_complete);
	return;
}

// FIXME: figure out why this function is being called on clients...
void __cdecl simulation_reset(void)
{
	s_simulation_globals* simulation_globals = simulation_get_globals();
	ASSERT(simulation_globals->world);
	//ASSERT(simulation_globals->world->is_authority());

	if (simulation_globals->simulation_invalidate)
	{
		simulation_globals->simulation_invalidate = false;
	}
	else
	{
		// this will use the main game simulation reset code
		// but we don't need it
		//simulation_globals->simulation_reset_pending = true;

		// instead, call reset directly
		simulation_reset_immediate();
	}
	return;
}

bool simulation_in_progress(void)
{
	bool result = false;

	if (simulation_engine_initialized()
		&& game_in_progress()
		&& game_get_active_structure_bsp_index() != NONE
		&& !simulation_is_paused())
	{
		ASSERT(simulation_get_globals()->world);
		if (simulation_get_world()->is_active())
		{
			result = true;
		}
	}

	return result;
}

bool simulation_query_object_is_predicted(datum object_index)
{
	return game_is_predicted() && object_get(object_index)->object.simulation_entity_index != NONE;
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

void __cdecl simulation_apply_before_game(const struct simulation_update* update)
{
	ASSERT(update != NULL);
	ASSERT(simulation_get_globals()->initialized);
	ASSERT(simulation_get_globals()->world);
	ASSERT(game_in_progress());

	c_simulation_queue simulation_bookkeeping_queue, game_simulation_queue;
	c_simulation_world* sim_world = simulation_get_world();

	simulation_get_globals()->world->queues_update_statistics();

	// only during distributed system or server synchronous
	// but not client synchronous
	// transfer the elements to the 
	if (sim_world->runs_simulation())
	{
		sim_world->attach_simulation_queues_to_update(
			&simulation_bookkeeping_queue,
			&game_simulation_queue
		);
	}

	for (int32 i = 0; i < k_maximum_players; i++)
	{
		datum control_unit_index = update->control_unit_index[i];
		if (TEST_BIT(update->unit_control_mask, i) && unit_try_and_get(control_unit_index))
		{
			unit_control(control_unit_index, &update->unit_control[i]);
		}
	}
	
	if (update->machine_update_valid)
	{
		players_set_machines(update->machine_update.machine_valid_mask, update->machine_update.identifiers);
	}

	sim_world->apply_simulation_queue(&simulation_bookkeeping_queue);

	// Player activation code
	/* Moved so we can activate in the queue
	s_simulation_globals* globals = simulation_get_globals();
	if (update->player_update_count > 0)
	{
		bool fatal_error = false;
		for (int32 player_update_index = 0; simulation_players_apply_update(&update->player_updates[player_update_index]); player_update_index++)
		{
			// Get out of here if we've overflown
			if (player_update_index >= update->player_update_count) 
			{
				fatal_error = true;
				break;
			}
		}
		
		//  Set bool to true ONLY if overflown is false, don't change otherwise
		if (!fatal_error)
		{
			globals->fatal_error = true;
		}
	}*/

	// #########
	// ### FIXME 
	// IMPLEMENT simulation_get_world()->queue_get(_simulation_queue_basic)->requires_application();

	if (game_simulation_queue.queued_count() <= 0)
	{
		ASSERT(game_simulation_queue.allocated_size_in_bytes() == 0);
	}
	else
	{
		ASSERT(game_simulation_queue.allocated_size_in_bytes() > 0);

		sim_world->apply_simulation_queue(&game_simulation_queue);

		// purge any deletion pending object during this update
		// if simulation is not in progress
		if (!update->simulation_in_progress)
			objects_purge_deleted_objects();
	}

	if (update->flush_gamestate)
	{
		simulation_get_globals()->world->gamestate_flush_immediate();
	}

	// destroy the update exactly after we applied the queues to the gamestate
	simulation_bookkeeping_queue.dispose();
	game_simulation_queue.dispose();
	//simulation_destroy_update();

	return;
}

void simulation_apply_after_game(const struct simulation_update* update)
{
	// This never did anything
	return;
}

void simulation_fatal_error(void)
{
	s_simulation_globals* simulation_globals = simulation_get_globals();

	ASSERT(simulation_globals->initialized);

	event(_event_error, "simulation:global: fatal error raised at time [%d]", game_time_get());

	simulation_globals->simulation_fatal_error = true;

	return;
}

void __cdecl simulation_build_update(struct simulation_update* update)
{
	s_simulation_globals* simulation_globals = simulation_get_globals();

	ASSERT(simulation_globals->initialized);
	ASSERT(simulation_globals->world);

	if (simulation_globals->engine_paused)
	{
		DISPLAY_ASSERT("simulation aborted inside game update");
	}

	ASSERT(simulation_globals->world->exists());
	ASSERT(game_in_progress());
	ASSERT(update);


	INVOKE(0x1ADDF3, 0x1A81AA, simulation_build_update, update);
	return;
}

void __cdecl simulation_update_aftermath(const struct simulation_update* update)
{
	INVOKE(0x1ADEA9, 0x1A8260, simulation_update_aftermath, update);
	return;
}

void simulation_update_pregame(void)
{
	ASSERT(!simulation_in_progress());

	struct simulation_update update;
	s_simulation_globals* simulation_globals = simulation_get_globals();

	if (simulation_globals->initialized && game_in_progress() && !simulation_is_paused())
	{
		if (simulation_globals->watcher->need_to_generate_updates())
		{
			simulation_build_update(&update);
			simulation_apply_before_game(&update);
			simulation_update_aftermath(&update);
		}
		else
		{
			simulation_globals->world->queues_update_statistics();
		}
	}
	return;
}

void simulation_destroy_update(struct simulation_update* update)
{
	ASSERT(update);
	s_simulation_globals* simulation_globals = simulation_get_globals();
	simulation_globals->world->destroy_world();
	return;
}

bool __cdecl simulation_get_machine_active_in_game(s_machine_identifier* machine_identifier)
{
	return INVOKE(0x1AE0CB, 0x1A8482, simulation_get_machine_active_in_game, machine_identifier);
}

void __cdecl simulation_build_player_updates(int32* player_update_count, int32 maximum_player_update_count, simulation_player_update* player_updates)
{
	s_simulation_globals* simulation_globals = simulation_get_globals();

	ASSERT(simulation_globals->initialized);
	ASSERT(simulation_globals->world);
	ASSERT(simulation_globals->world->runs_simulation());
	ASSERT(simulation_globals->watcher);
	ASSERT(game_in_progress());

	simulation_globals->watcher->generate_player_updates(player_update_count, maximum_player_update_count, player_updates);
	for (int32 i = 0; i < *player_update_count; i++)
	{
		simulation_queue_player_update_insert(&player_updates[i]);
	}
	return;
}

/* private code */

static s_simulation_globals* simulation_get_globals(void)
{
	return Memory::GetAddress<s_simulation_globals*>(0x5178D0, 0x520B60);
}


static void simulation_player_joined_game_patch_calls(void)
{
	PatchCall(Memory::GetAddress(0x56447, 0x5E93F), simulation_player_joined_game);
	PatchCall(Memory::GetAddress(0x5647F, 0x5E977), simulation_player_joined_game);
	PatchCall(Memory::GetAddress(0x57E85, 0x6037D), simulation_player_joined_game);
	return;
}

static void simulation_player_left_game_patch_calls(void)
{
	PatchCall(Memory::GetAddress(0x5633A, 0x5E832), simulation_player_left_game);
	return;
}
