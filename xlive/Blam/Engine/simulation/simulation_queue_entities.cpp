#include "stdafx.h"
#include "simulation_queue_entities.h"

#include "simulation_gamestate_entities.h"

#include "game/game.h"
#include "Networking/replication/replication_control_view.h"


// We get the absolute entity index from here as well as h2 needs it in order to create the game entity
bool simulation_queue_entity_decode_header(c_bitstream* bitstream, e_simulation_entity_type* entity_type, int32* gamestate_index, uint32* entity_abs_index);
bool decode_simulation_queue_creation_from_buffer(int32 encoded_size, uint8* encoded_data, s_simulation_queue_decoded_creation_data* creation_data, uint32* entity_abs_index);

c_simulation_entity_definition* simulation_queue_entities_get_definition(e_simulation_entity_type type)
{
	c_simulation_type_collection* sim_collection = simulation_get_type_collection();
	return sim_collection->get_entity_definition(type);
}


void simulation_queue_entity_creation_insert(s_simulation_queue_element* simulation_queue_element)
{
	simulation_get_world()->simulation_queue_enqueue(simulation_queue_element);
	return;
}

bool simulation_queue_entity_creation_allocate(s_simulation_queue_entity_data* simulation_queue_entity_data, uint32 update_mask, s_simulation_queue_element** element, int32* gamestate_index)
{
	//int32 entity_index = simulation_gamestate_entity_create();

	c_simulation_entity_definition* entity_definition = simulation_queue_entities_get_definition(simulation_queue_entity_data->entity_type);

	// ### TODO STUB for now
	if (gamestate_index)
	{
		*gamestate_index = NONE;
	}

	return false;
}

void simulation_queue_entity_creation_apply(const s_simulation_queue_element* element)
{
	s_simulation_queue_decoded_creation_data creation_data;

	if (game_is_distributed())
	{
		csmemset(&creation_data, 0, sizeof(s_simulation_queue_decoded_creation_data));
		
		uint32 entity_abs_index;
		if (decode_simulation_queue_creation_from_buffer(element->data_size, element->data, &creation_data, &entity_abs_index))
		{
			c_simulation_entity_definition* entity = simulation_queue_entities_get_definition(creation_data.entity_type);
			
			bool valid_gamestate_index = true;
			if (game_is_playback())
			{
				// TODO: asserts here
			}

			 
			if (valid_gamestate_index)
			{
				s_simulation_game_entity game_entity; 
				game_entity.entity_index = entity_abs_index;
				valid_gamestate_index = entity->create_game_entity(
					&game_entity, 
					creation_data.creation_data_size,
					creation_data.creation_data, 
					entity->initial_update_mask(),
					creation_data.state_data_size,
					creation_data.state_data);

			}
			else
			{
				// TODO: asserts here
			}
		}
		else
		{
			// TODO: asserts here
		}
	}
}



bool simulation_queue_entity_decode_header(c_bitstream* bitstream, e_simulation_entity_type* entity_type, int32* gamestate_index, uint32* entity_abs_index)
{
	*entity_type = (e_simulation_entity_type)bitstream->read_integer("entity-type", 5);
	//replication_entity_index_decode_get_abs_entity_index(bitstream, gamestate_index, entity_abs_index);
	bool successfully_decoded = IN_RANGE_INCLUSIVE(*entity_type, _simulation_entity_type_slayer_engine_globals, k_simulation_entity_count - 1);
	return successfully_decoded;
}

bool decode_simulation_queue_creation_from_buffer(int32 encoded_size, uint8* encoded_data, s_simulation_queue_decoded_creation_data* creation_data, uint32* entity_abs_index)
{
	bool successfully_decoded = false;

	c_bitstream bitstream(encoded_data, encoded_size);
	bitstream.begin_reading();

	if (simulation_queue_entity_decode_header(&bitstream, &creation_data->entity_type, &creation_data->gamestate_index, entity_abs_index))
	{
		creation_data->initial_update_mask = bitstream.read_integer("initial-update-mask", 32);

		const c_simulation_type_collection* type = simulation_get_type_collection();
		c_simulation_entity_definition* entity = type->get_entity_definition(creation_data->entity_type);
		creation_data->creation_data_size = entity->creation_data_size();
		creation_data->state_data_size = entity->state_data_size();

		if (creation_data->creation_data_size <= k_simulation_payload_size_max)
		{
			if (creation_data->state_data_size <= k_simulation_payload_size_max)
			{
				successfully_decoded = entity->object_creation_decode(
					creation_data->creation_data_size,
					creation_data->creation_data,
					&bitstream);
				if (successfully_decoded)
				{
					if (creation_data->initial_update_mask)
					{
						uint32 update_mask = 0;
						entity->build_baseline_state_data(
							creation_data->creation_data_size,
							creation_data->creation_data,
							creation_data->state_data_size,
							creation_data->state_data);
						entity->entity_update_decode(
							true,
							&update_mask,
							creation_data->state_data_size,
							creation_data->state_data,
							&bitstream);
					}
					successfully_decoded = !bitstream.error_occured();
					bitstream.finish_reading();
				}
			}
		}

		if (successfully_decoded && creation_data->gamestate_index == NONE)
		{
			successfully_decoded = false;
		}
	}

	return successfully_decoded;
}