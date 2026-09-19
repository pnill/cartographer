#include "stdafx.h"
#include "simulation_queue_global_events.h"

#include "simulation.h"
#include "simulation_encoding.h"
#include "simulation_world.h"

#include "main/main.h"
#include "game/game.h"
#include "game/game_engine.h"
#include "game/players.h"
#include "memory/bitstream.h"
#include "networking/network_event.h"
#include "simulation/game_interface/simulation_game_action.h"

/* prototypes */

static bool game_is_synchronous_networking(void);

static void simulation_queue_player_event_set_activation(int32 player_index, bool active);

/* public code */

bool simulation_queue_game_global_event_requires_cutoff(const s_simulation_queue_element* element)
{
	bool result = false;
	
	ASSERT(element);
	c_bitstream stream(element->data, element->data_size);
	stream.begin_reading();
	
	e_simulation_queue_game_global_event_type type = (e_simulation_queue_game_global_event_type)stream.read_integer("global-event-type", k_simulation_queue_game_global_event_type_bits);
	if (!stream.error_occurred())
	{
		result = type >= _simulation_queue_game_global_event_type_reset_map && type <= _simulation_queue_game_global_event_type_save_and_quit;
	}
	else
	{
		event(_event_error, "networking:simulation:queue: failed to decode global event");
	}

	stream.finish_reading();
	return result;
 }

void simulation_queue_game_global_event_insert(e_simulation_queue_game_global_event_type game_global_event_type)
{
	if (!game_is_playback() && !game_is_synchronous_networking() || game_is_server())
	{
		uint8 encoded_data[128];
		c_bitstream stream(encoded_data, sizeof(encoded_data));
		stream.begin_writing(1);
		stream.write_integer("global-event-type", game_global_event_type, k_simulation_queue_game_global_event_type_bits);
		
		const int32 size = stream.get_space_used_in_bytes();
		if (!stream.error_occurred())
		{
			s_simulation_queue_element* element = NULL;
			c_simulation_world* world = simulation_get_world();
			world->simulation_queue_allocate(_simulation_queue_element_type_game_global_event, size, &element);
			if (element)
			{
				csmemcpy(element->data, encoded_data, size);
				world->simulation_queue_enqueue(element);
			}
			else
			{
				event(_event_fatal, "networking:simulation:queue: failed to allocate element for game global event insertion %d", size);
			}
		}
		else
		{
			event(_event_error, "networking:simulation:queue: failed to insert game global event type %d", game_global_event_type);
		}
		stream.finish_writing(NULL);
	}
	else
	{
		event(_event_warning, "networking:simulation:queue: not allowed to insert global event type %d", game_global_event_type);
	}

	return;
}

// TODO: finish the rest of the logic in this function
void simulation_queue_game_global_event_apply(const s_simulation_queue_element* element)
{
	ASSERT(element);
	ASSERT(element->type == _simulation_queue_element_type_game_global_event);

	c_bitstream stream(element->data, element->data_size);
	stream.begin_reading();

	e_simulation_queue_game_global_event_type type = (e_simulation_queue_game_global_event_type)stream.read_integer("global-event-type", k_simulation_queue_game_global_event_type_bits);

	if (stream.error_occurred())
	{
		event(_event_error, "networking:simulation:queue: failed to decode game global event");
	}
	else
	{
		switch (type)
		{
		case _simulation_queue_game_global_event_type_claim_authority_gameworld:
			event(_event_message, "networking:simulation:queue:global: claiming authority");
			simulation_get_world()->claim_authority_gameworld();
			break;
		case _simulation_queue_game_global_event_type_game_simulation_server:
			event(_event_message, "networking:simulation:queue:global: setting game simulation to dist. server");
			game_simulation_set(_game_simulation_distributed_server);
			break;
		case _simulation_queue_game_global_event_type_game_simulation_client:
			event(_event_message, "networking:simulation:queue:global: setting game simulation to dist. client");
			game_simulation_set(_game_simulation_distributed_client);
			break;
		case _simulation_queue_game_global_event_type_game_finish:
			event(_event_verbose, "networking:simulation:queue:global: calling game won");
			//game_won();
			break;
		case _simulation_queue_game_global_event_type_revert_map:
			event(_event_message, "networking:simulation:queue:global: calling revert map");
			main_revert_map();
			break;
		case _simulation_queue_game_global_event_type_reset_map:
			event(_event_message, "networking:simulation:queue:global: calling reset map");
			main_reset_map();
			break;
		case _simulation_queue_game_global_event_type_save_and_quit:
			/*
			if (simulation_performed_main_save_and_exit_campaign_immediately_this_map())
			{
				event(_event_message, "networking:simulation:queue:global: redundant call to save and exit");
			}
			else
			{
				event(_event_message, "networking:simulation:queue:global: calling save and exit");
				simulation_set_performed_main_save_and_exit_campaign_immediately_this_map(1);
				main_save_and_exit_campaign_immediately();
			}
			*/
			break;
		case _simulation_queue_game_global_event_type_simulation_reset_complete:
			event(_event_message, "networking:simulation:queue:global: calling simulation_notify_reset_complete()");
			simulation_notify_reset_complete();
			break;
		case _simulation_queue_game_global_event_type_simulation_reset_initiate:
			event(_event_message, "networking:simulation:queue:global: calling simulation_notify_reset_initiate()");
			simulation_notify_reset_initiate();
			break;
		case _simulation_queue_game_global_event_type_simulation_going_active:
			event(_event_message, "networking:simulation:queue:global: calling simulation_notify_going_active()");
			simulation_notify_going_active();
			break;
		default:
			event(_event_error, "networking:simulation:queue:global_events: unknown game global event type %d", type);
			break;
		}
	}
	return;
}

void simulation_queue_player_event_insert(e_simulation_queue_player_event_type event_type, datum player_index, const s_simulation_queue_player_event_data* event_data)
{
	ASSERT(event_data);
	ASSERT(player_index != NONE);

	if (game_is_playback() || game_is_synchronous_networking() && !game_is_server())
	{
		event(_event_warning, "networking:simulation:queue: not allowed to insert simulation_queue_player_update_insert");
	}
	else
	{
		uint8 encoded_data[128];
		c_bitstream stream(encoded_data, sizeof(encoded_data));

		uint16 abs_player_index = DATUM_INDEX_TO_ABSOLUTE_INDEX(player_index);

		stream.begin_writing(1);
		stream.write_integer("player-index", abs_player_index, k_player_index_bits);
		stream.write_bool("active", event_data->active);

		const int32 size = stream.get_space_used_in_bytes();
		if (!stream.error_occurred())
		{
			s_simulation_queue_element* element = NULL;
			c_simulation_world* world = simulation_get_world();
			world->simulation_queue_allocate(_simulation_queue_element_type_player_event, size, &element);
			if (element)
			{
				csmemcpy(element->data, encoded_data, size);
				world->simulation_queue_enqueue(element);
			}
			else
			{
				event(_event_fatal, "networking:simulation:queue: failed to allocate element for player event insertion %d", size);
			}
		}
		else
		{
			event(_event_error, "networking:simulation:queue: failed to insert player event type %d", event_type);
		}
		stream.finish_writing(NULL);
	}
	return;
}

void simulation_queue_player_event_apply(const s_simulation_queue_element* element)
{
	ASSERT(element);

	c_bitstream stream(element->data, element->data_size);
	stream.begin_reading();

	const uint16 abs_player_index = (uint16)stream.read_integer("player-index", k_player_index_bits);

	const datum player_index = player_index_from_absolute_player_index(abs_player_index);
	if (!stream.error_occurred())
	{
		const bool active = stream.read_bool("active");
		simulation_queue_player_event_set_activation(player_index, active);
	}
	else
	{
		event(_event_error, "networking:simulation:queue: failed to decode player event");
	}

	stream.finish_reading();
	return;
}

void simulation_queue_player_update_insert(const simulation_player_update* player_update)
{
	ASSERT(player_update);

	if (!game_is_playback())
	{
		uint8 encoded_data[k_simulation_queue_element_data_size_max];
		c_bitstream stream(encoded_data, sizeof(encoded_data));
		stream.begin_writing(1);
		simulation_player_update_encode(&stream, player_update);

		const int32 size = stream.get_space_used_in_bytes();
		if (!stream.error_occurred())
		{
			s_simulation_queue_element* element = NULL;
			c_simulation_world* world = simulation_get_world();
			world->simulation_queue_allocate(_simulation_queue_element_type_player_update_event, size, &element);
			if (element)
			{
				csmemcpy(element->data, encoded_data, size);
				world->simulation_queue_enqueue(element);
			}
			else
			{
				event(_event_fatal, "networking:simulation:queue: failed to allocate element for player update insertion %d", size);
			}
		}
		else
		{
			event(_event_error, "networking:simulation:queue: failed to insert player update");
		}
		stream.finish_writing(NULL);
	}
	return;
}

// Function updated to return result if simulation_players_apply_update failed so we can stop applying player updates in the queue if one fails
// Originally halo 2 stopped applying player updates if one of them failed so we're mimicking the behaviour for now
// Unsure what the ramifications of this are for the moment when we added halo 3 simulation queue code
bool simulation_queue_player_update_apply(
	s_simulation_queue_element const* element)
{
	bool result = true;

	simulation_player_update update;
	csmemset(&update, 0, sizeof(simulation_player_update));

	c_bitstream stream(element->data, element->data_size);
	stream.begin_reading();

	simulation_player_update_decode(&stream, &update);

	if (stream.error_occurred())
	{
		event(_event_error, "networking:simulation:queue: failed to decode player update");
	}
	else if (!simulation_players_apply_update(&update))
	{
		event(_event_error, "networking:simulation:player_update_apply: failed to apply player update");
		result = false;
	}

	stream.finish_reading();

	return result;
}

/* private code */

static bool game_is_synchronous_networking(void)
{
	const e_game_simulation type = game_simulation_get();
	return IN_RANGE(type, _game_simulation_synchronous_client, _game_simulation_synchronous_server);
}

static void simulation_queue_player_event_set_activation(int32 player_index, bool active)
{
	player_datum* player = player_get(player_index);

	if (TEST_BIT(player->flags, 0) != active)
	{
		SET_BIT(player->flags, 0, active);
		if (active)
		{
			game_engine_player_activated(player_index);
		}
	}
	simulation_action_game_engine_player_update(player_index, FLAG(9));
	return;
}
