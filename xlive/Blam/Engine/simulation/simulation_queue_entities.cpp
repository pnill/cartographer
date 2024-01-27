#include "stdafx.h"
#include "simulation_queue_entities.h"

#include "simulation_gamestate_entities.h"

#include "game/game.h"
#include "simulation/simulation_entity_database.h"
#include "Networking/replication/replication_control_view.h"

// We get the absolute entity_def index from here as well as h2 needs it in order to create the game entity_def
bool encode_simulation_queue_creation_to_buffer(uint8* out_buffer, int32 out_buffer_size, datum gamestate_index, s_simulation_queue_entity_data* data, uint32 initial_update_mask, int32* out_written_size);
bool decode_simulation_queue_creation_from_buffer(int32 encoded_size, uint8* encoded_data, s_simulation_queue_decoded_creation_data* creation_data);
bool simulation_queue_entity_decode_header(c_bitstream* bitstream, e_simulation_entity_type* entity_type, datum* gamestate_index);

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

void simulation_queue_entity_encode_header(c_bitstream* stream, e_simulation_entity_type type, datum gamestate_index)
{
	stream->write_integer("entity-type", type, 5);
	//simulation_gamestate_index_encode(stream, gamestate_index);
}

bool simulation_queue_entity_decode_header(c_bitstream* stream, e_simulation_entity_type* type, datum* gamestate_index)
{
	*type = (e_simulation_entity_type)stream->read_integer("entity-type", 5);
	//simulation_gamestate_index_decode(stream, gamestate_index);
	bool successfully_decoded = IN_RANGE_INCLUSIVE(*type, _simulation_entity_type_slayer_engine_globals, k_simulation_entity_count - 1);
	return successfully_decoded;
}

bool encode_simulation_queue_creation_to_buffer(uint8* out_buffer, int32 out_buffer_size, datum gamestate_index, s_simulation_queue_entity_data* data, uint32 initial_update_mask, int32* out_written_size)
{
	c_bitstream stream(out_buffer, out_buffer_size);
	stream.begin_writing(k_bitstream_default_alignment);
	
	simulation_queue_entity_encode_header(&stream, data->entity_type, gamestate_index);
	simulation_entity_index_encode(&stream, data->entity_index);
	stream.write_integer("initial-update-mask", initial_update_mask, 32);

	// write the actual data
	c_simulation_entity_definition* entity_def = simulation_queue_entities_get_definition(data->entity_type);
	entity_def->object_creation_encode(data->creation_data_size, data->creation_data, NULL, &stream);

	if (initial_update_mask != 0)
	{
		uint32 update_mask_written = 0;
		entity_def->entity_update_encode(true, initial_update_mask, &update_mask_written, data->state_data_size, data->state_data, NULL, &stream, 0);
	}

	stream.finish_writing(NULL);

	bool result = !stream.error_occured();
	if (result)
	{
		SIM_QUEUE_DBG("#####");
		SIM_QUEUE_DBG("entity encoding, stream is fine? %d, encoded size: %d",
			!stream.error_occured(),
			stream.get_space_used_in_bytes());
		SIM_QUEUE_DBG("entity type: %d, reference count: %d", event_type, reference_count);
	}
	else
	{
		SIM_QUEUE_DBG("entity encoding failed!!!! --------");
	}

	*out_written_size = stream.get_space_used_in_bytes();
	return result;
}

bool simulation_queue_entity_creation_allocate(s_simulation_queue_entity_data* sim_queue_entity_data, uint32 initial_update_mask, s_simulation_queue_element** element, int32* gamestate_index)
{
	//int32 entity_index = simulation_gamestate_entity_create();

	bool result = false;
	int32 write_buffer_space_used;
	uint8 write_buffer[k_simulation_payload_size_max];
	s_simulation_queue_element* allocated_element = NULL;
	c_simulation_entity_definition* entity_def = simulation_queue_entities_get_definition(sim_queue_entity_data->entity_type);

	if (game_is_distributed() && !game_is_playback())
	{
		// ### TODO STUB for now
		if (gamestate_index)
		{
			*gamestate_index = NONE;
		}

		// NOTE missing pre-gameworld processing, unused in h2 so skip (even in H3 mainly unused)
		// entity_def->prepare_creation_data_for_gameworld()
		// entity_def->prepare_state_data_for_gameworld()

		if (encode_simulation_queue_creation_to_buffer(
			write_buffer,
			sizeof(write_buffer),
			DATUM_INDEX_NONE,
			sim_queue_entity_data,
			initial_update_mask,
			&write_buffer_space_used
		))
		{
			simulation_get_world()->simulation_queue_allocate(_simulation_queue_element_type_entity_creation, write_buffer_space_used, &allocated_element);
			if (allocated_element)
			{
				// copy the data to the buffer, enqueuing done later for entities
				csmemcpy(allocated_element->data, write_buffer, write_buffer_space_used);
			}
		}

		return result;
	}

	return result;
}

void simulation_queue_entity_creation_apply(const s_simulation_queue_element* element)
{
	s_simulation_queue_decoded_creation_data decoded_creation_data;

	if (game_is_distributed() && !game_is_playback())
	{
		csmemset(&decoded_creation_data, 0, sizeof(s_simulation_queue_decoded_creation_data));
		
		if (decode_simulation_queue_creation_from_buffer(element->data_size, element->data, &decoded_creation_data))
		{
			c_simulation_entity_definition* entity_def = simulation_queue_entities_get_definition(decoded_creation_data.entity_type);
			
			bool valid_gamestate_index = true;
			 
			if (valid_gamestate_index)
			{
				s_simulation_game_entity* game_entity = simulation_get_entity_database()->get_entity(decoded_creation_data.entity_index);

				game_entity->entity_index = decoded_creation_data.entity_index;
				game_entity->creation_data = decoded_creation_data.creation_data;
				game_entity->creation_data_size = decoded_creation_data.creation_data_size;
				game_entity->state_data = decoded_creation_data.state_data;
				game_entity->state_data_size = decoded_creation_data.state_data_size;

				valid_gamestate_index = entity_def->create_game_entity(
					game_entity, 
					decoded_creation_data.creation_data_size,
					decoded_creation_data.creation_data, 
					decoded_creation_data.initial_update_mask,
					decoded_creation_data.state_data_size,
					decoded_creation_data.state_data
				);

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

bool decode_simulation_queue_creation_from_buffer(int32 encoded_size, uint8* encoded_data, s_simulation_queue_decoded_creation_data* decoded_creation_data)
{
	bool successfully_decoded = false;

	c_bitstream stream(encoded_data, encoded_size);
	stream.begin_reading();

	if (simulation_queue_entity_decode_header(&stream, &decoded_creation_data->entity_type, &decoded_creation_data->gamestate_index))
	{
		simulation_entity_index_decode(&stream, &decoded_creation_data->entity_index);
		decoded_creation_data->initial_update_mask = stream.read_integer("initial-update-mask", 32);

		const c_simulation_type_collection* type = simulation_get_type_collection();
		c_simulation_entity_definition* entity = type->get_entity_definition(decoded_creation_data->entity_type);
		decoded_creation_data->creation_data_size = entity->creation_data_size();
		decoded_creation_data->state_data_size = entity->state_data_size();

		if (decoded_creation_data->creation_data_size <= k_simulation_payload_size_max)
		{
			if (decoded_creation_data->state_data_size <= k_simulation_payload_size_max)
			{
				successfully_decoded = entity->object_creation_decode(
					decoded_creation_data->creation_data_size,
					decoded_creation_data->creation_data,
					&stream);
				if (successfully_decoded)
				{
					if (decoded_creation_data->initial_update_mask)
					{
						uint32 update_mask = 0;
						// this might not be needed in h2v
						// because h2v doesn't have this extra parameter where you pass the state baseline data
						// when decoding updates
						entity->build_baseline_state_data(
							decoded_creation_data->creation_data_size,
							decoded_creation_data->creation_data,
							decoded_creation_data->state_data_size,
							(s_simulation_baseline_state_data*)decoded_creation_data->state_data
						);
						// in H3 this has some extra param which signals whether to decode
						// gamestate index or entity index
						entity->entity_update_decode(
							true,
							&update_mask,
							decoded_creation_data->state_data_size,
							decoded_creation_data->state_data,
							&stream);
					}
					successfully_decoded = !stream.error_occured();
					stream.finish_reading();
				}
			}
		}

		/*if (successfully_decoded && decoded_creation_data->gamestate_index == NONE)
		{
			successfully_decoded = false;
		}*/
	}

	return successfully_decoded;
}