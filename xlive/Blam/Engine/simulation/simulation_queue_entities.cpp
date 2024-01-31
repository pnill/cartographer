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
	*gamestate_index = NONE;
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
	entity_def->entity_creation_encode(data->creation_data_size, data->creation_data, NULL, &stream);

	if (initial_update_mask != 0)
	{
		uint32 update_mask_written = 0;
		if (entity_def->entity_update_encode(true, initial_update_mask, &update_mask_written, data->state_data_size, data->state_data, NULL, &stream, 0))
		{
			SIM_ENT_QUEUE_DBG("enity update encode success!");
		}
	}

	bool result = !stream.error_occured();
	stream.finish_writing(NULL);

	if (result)
	{
		SIM_ENT_QUEUE_DBG("#####");
		SIM_ENT_QUEUE_DBG("entity encoding, stream is fine? %d, encoded size: %d",
			!stream.error_occured(),
			stream.get_space_used_in_bytes());
		SIM_ENT_QUEUE_DBG("entity type: %d, index: %X, initial update mask: 0x%X", data->entity_type, data->entity_index, initial_update_mask);
	}
	else
	{
		SIM_ENT_QUEUE_DBG("entity encoding failed!!!! --------");
		
	}

	*out_written_size = stream.get_space_used_in_bytes();

	return result;
}

bool decode_simulation_queue_creation_from_buffer(int32 encoded_size, uint8* encoded_data, s_simulation_queue_decoded_creation_data* decoded_creation_data)
{
	bool decode_success = false;

	c_bitstream stream(encoded_data, encoded_size);
	stream.begin_reading();

	if (simulation_queue_entity_decode_header(&stream, &decoded_creation_data->entity_type, &decoded_creation_data->gamestate_index))
	{
		simulation_entity_index_decode(&stream, &decoded_creation_data->entity_index);
		decoded_creation_data->initial_update_mask = stream.read_integer("initial-update-mask", 32);

		c_simulation_entity_definition* entity_def = simulation_queue_entities_get_definition(decoded_creation_data->entity_type);
		decoded_creation_data->creation_data_size = entity_def->creation_data_size();
		decoded_creation_data->state_data_size = entity_def->state_data_size();

		if (decoded_creation_data->creation_data_size <= k_simulation_payload_size_max)
		{
			if (decoded_creation_data->state_data_size <= k_simulation_payload_size_max)
			{
				decode_success = entity_def->entity_creation_decode(
					decoded_creation_data->creation_data_size,
					decoded_creation_data->creation_data,
					&stream);

				if (decode_success)
				{
					if (decoded_creation_data->initial_update_mask)
					{
						uint32 update_mask = 0;
						if (entity_def->build_baseline_state_data(
							decoded_creation_data->creation_data_size,
							decoded_creation_data->creation_data,
							decoded_creation_data->state_data_size,
							(s_simulation_baseline_state_data*)decoded_creation_data->state_data
						))
						{
							SIM_ENT_QUEUE_DBG("entity baseline state data success when decoding!");
						}
						// in H3 this has some extra param which signals whether to decode
						// gamestate index or entity index
						if (entity_def->entity_update_decode(
							true,
							&update_mask,
							decoded_creation_data->state_data_size,
							decoded_creation_data->state_data,
							&stream))
						{
							SIM_ENT_QUEUE_DBG("entity update decode success!");
						}
					}
				}

				decode_success = !stream.error_occured();
				stream.finish_reading();
			}
		}

		/*if (decode_success && decoded_creation_data->gamestate_index == NONE)
		{
			decode_success = false;
		}*/
	}

	if (decode_success)
	{
		SIM_ENT_QUEUE_DBG("#####");
		SIM_ENT_QUEUE_DBG("entity decoding, stream is fine? %d, decoded size: %d",
			!stream.error_occured(),
			stream.get_space_used_in_bytes());
		SIM_ENT_QUEUE_DBG("entity type: %d, index: %X, initial update mask: 0x%X", decoded_creation_data->entity_type, decoded_creation_data->entity_index, decoded_creation_data->initial_update_mask);
	}
	else
	{
		SIM_ENT_QUEUE_DBG("entity decoding failed!!!! --------");
	}

	return decode_success;
}

bool simulation_queue_entity_creation_allocate(s_simulation_queue_entity_data* sim_queue_entity_data, uint32 initial_update_mask, s_simulation_queue_element** element, int32* gamestate_index)
{
	//int32 entity_index = simulation_gamestate_entity_create();

	bool result = false;
	int32 write_buffer_space_used;
	uint8 write_buffer[k_simulation_payload_size_max];
	c_simulation_entity_definition* entity_def = simulation_queue_entities_get_definition(sim_queue_entity_data->entity_type);

	// simulation_queue_allocate does set this to NULL
	// but in case encode_simulation_queue_update_to_buffer fails
	// set this to NULL
	*element = NULL;

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
			NONE,
			sim_queue_entity_data,
			initial_update_mask,
			&write_buffer_space_used
		))
		{
			simulation_get_world()->simulation_queue_allocate(_simulation_queue_element_type_entity_creation, write_buffer_space_used, element);
			if (*element)
			{
				// copy the data to the buffer, enqueuing done later for entities
				csmemcpy((*element)->data, write_buffer, write_buffer_space_used);
				result = true;
			}
		}
	}

	return result;
}

void dump_entity_unit_creation_to_file(
	e_simulation_entity_type type, 
	uint32 initial_update_mask, 
	uint8* encoded_creation, 
	uint8* decoded_creation, 
	int32 creation_size,
	uint8* encoded_state,
	uint8* decoded_state, 
	int32 state_size
)
{
	if (type == _simulation_entity_type_unit)
	{
		FILE* fp;
		c_static_wchar_string256 path;

		extern wchar_t* H2AppDataLocal;
		path.set(H2AppDataLocal);
		path.append(L"entity_update_unit_creation_state.bin");

		if (!_wfopen_s(&fp, path.get_string(), L"wb"))
		{
			fwrite(&type, sizeof(type), 1, fp);
			fwrite("creation data here:", sizeof("creation data here:"), 1, fp);
			fwrite(&creation_size, sizeof(creation_size), 1, fp);
			fwrite(encoded_creation, creation_size, 1, fp);
			fwrite("state data here:", sizeof("state data here:"), 1, fp);
			fwrite(&state_size, sizeof(state_size), 1, fp);
			fwrite(encoded_state, state_size, 1, fp);
			fwrite(&initial_update_mask, sizeof(initial_update_mask), 1, fp);

			while (ftell(fp) % 16 > 0)
				putc(0, fp);

			fwrite(&type, sizeof(type), 1, fp);
			fwrite("decoded creation data here:", sizeof("decoded creation data here:"), 1, fp);
			fwrite(&creation_size, sizeof(creation_size), 1, fp);
			fwrite(decoded_creation, creation_size, 1, fp);
			fwrite("decoded borked state data here:", sizeof("decoded borked state data here:"), 1, fp);
			fwrite(&state_size, sizeof(state_size), 1, fp);
			fwrite(decoded_state, state_size, 1, fp);
			fwrite(&initial_update_mask, sizeof(initial_update_mask), 1, fp);

			while (ftell(fp) % 16 > 0)
				putc(0, fp);

			fclose(fp);
		}

	}
}

void simulation_queue_entity_creation_apply(const s_simulation_queue_element* element)
{
	if (game_is_distributed() && !game_is_playback())
	{
		s_simulation_queue_decoded_creation_data decoded_creation_data;
		csmemset(&decoded_creation_data, 0, sizeof(decoded_creation_data));
		
		if (decode_simulation_queue_creation_from_buffer(element->data_size, element->data, &decoded_creation_data))
		{
			c_simulation_entity_definition* entity_def = simulation_queue_entities_get_definition(decoded_creation_data.entity_type);
			s_simulation_game_entity* game_entity = simulation_get_entity_database()->entity_try_and_get(decoded_creation_data.entity_index);

			if (game_entity)
			{
				game_entity->exists_in_gameworld = entity_def->create_game_entity(
					game_entity,
					decoded_creation_data.creation_data_size,
					decoded_creation_data.creation_data,
					decoded_creation_data.initial_update_mask,
					decoded_creation_data.state_data_size,
					decoded_creation_data.state_data
				);
			}
		}
	}
	else
	{
		// TODO: asserts here
	}
}

void simulation_queue_entity_update_insert(s_simulation_queue_element* simulation_queue_element)
{
	simulation_get_world()->simulation_queue_enqueue(simulation_queue_element);
	return;
}

bool encode_simulation_queue_update_to_buffer(uint8* out_buffer, int32 out_buffer_size, datum gamestate_index, s_simulation_queue_entity_data* data, uint32 update_mask, int32* out_written_size)
{
	c_bitstream stream(out_buffer, out_buffer_size);
	stream.begin_writing(k_bitstream_default_alignment);

	simulation_queue_entity_encode_header(&stream, data->entity_type, gamestate_index);
	simulation_entity_index_encode(&stream, data->entity_index);
	stream.write_integer("update-mask", update_mask, 32);

	// write the actual data
	c_simulation_entity_definition* entity_def = simulation_queue_entities_get_definition(data->entity_type);
	uint32 update_mask_written = 0;
	if (entity_def->entity_update_encode(false, update_mask, &update_mask_written, data->state_data_size, data->state_data, NULL, &stream, 0))
	{
		SIM_ENT_QUEUE_DBG("enity update encode success!");
	}

	bool result = !stream.error_occured();
	stream.finish_writing(NULL);

	if (result)
	{
		SIM_ENT_QUEUE_DBG("#####");
		SIM_ENT_QUEUE_DBG("entity encoding, stream is fine? %d, encoded size: %d",
			!stream.error_occured(),
			stream.get_space_used_in_bytes());
		SIM_ENT_QUEUE_DBG("entity type: %d, index: %X, initial update mask: 0x%X", data->entity_type, data->entity_index, initial_update_mask);
	}
	else
	{
		SIM_ENT_QUEUE_DBG("entity encoding failed!!!! --------");

	}

	*out_written_size = stream.get_space_used_in_bytes();

	return result;
}

bool decode_simulation_queue_update_from_buffer(int32 encoded_size, uint8* encoded_data, s_simulation_queue_decoded_update_data* decoded_update_data)
{
	bool decode_success = false;

	c_bitstream stream(encoded_data, encoded_size);
	stream.begin_reading();

	if (simulation_queue_entity_decode_header(&stream, &decoded_update_data->entity_type, &decoded_update_data->gamestate_index))
	{
		simulation_entity_index_decode(&stream, &decoded_update_data->entity_index);
		decoded_update_data->update_mask = stream.read_integer("update-mask", 32);

		c_simulation_entity_database* entity_database = simulation_get_entity_database();
		s_simulation_game_entity* entity = entity_database->entity_try_and_get(decoded_update_data->entity_index);

		if (entity)
		{
			c_simulation_entity_definition* entity_def = simulation_queue_entities_get_definition(decoded_update_data->entity_type);
			decoded_update_data->state_data_size = entity->state_data_size;

			if (decoded_update_data->state_data_size <= k_simulation_payload_size_max)
			{
				uint32 update_mask = 0;
				if (entity_def->build_baseline_state_data(
					entity->creation_data_size,
					entity->creation_data,
					decoded_update_data->state_data_size,
					(s_simulation_baseline_state_data*)decoded_update_data->state_data
				))
				{
					SIM_ENT_QUEUE_DBG("entity baseline state data success when decoding!");
				}

				// in H3 this has some extra param which signals whether to decode
				// gamestate index or entity index
				if (entity_def->entity_update_decode(
					false,
					&update_mask,
					decoded_update_data->state_data_size,
					decoded_update_data->state_data,
					&stream))
				{
					SIM_ENT_QUEUE_DBG("entity update decode success!");
				}

				decode_success = !stream.error_occured();
				stream.finish_reading();
			}
		}
	}

	if (decode_success)
	{
		SIM_ENT_QUEUE_DBG("#####");
		SIM_ENT_QUEUE_DBG("entity decoding, stream is fine? %d, decoded size: %d",
			!stream.error_occured(),
			stream.get_space_used_in_bytes());
		SIM_ENT_QUEUE_DBG("entity type: %d, index: %X, initial update mask: 0x%X", decoded_update_data->entity_type, decoded_update_data->entity_index, decoded_update_data->initial_update_mask);
	}
	else
	{
		SIM_ENT_QUEUE_DBG("entity decoding failed!!!! --------");
	}

	return decode_success;
}

bool simulation_queue_entity_update_allocate(s_simulation_queue_entity_data* sim_queue_entity_data, int32 gamestate_index, uint32 update_mask, s_simulation_queue_element** element)
{
	//int32 entity_index = simulation_gamestate_entity_create();

	bool result = false;
	int32 write_buffer_space_used;
	uint8 write_buffer[k_simulation_payload_size_max];
	c_simulation_entity_definition* entity_def = simulation_queue_entities_get_definition(sim_queue_entity_data->entity_type);

	// simulation_queue_allocate does set this to NULL
	// but in case encode_simulation_queue_update_to_buffer fails
	// set this to NULL
	*element = NULL;

	if (game_is_distributed() && !game_is_playback())
	{
		// NOTE missing pre-gameworld processing, unused in h2 so skip (even in H3 mainly unused)
		// entity_def->prepare_state_data_for_gameworld()

		if (encode_simulation_queue_update_to_buffer(
			write_buffer,
			sizeof(write_buffer),
			gamestate_index,
			sim_queue_entity_data,
			update_mask,
			&write_buffer_space_used
		))
		{
			simulation_get_world()->simulation_queue_allocate(_simulation_queue_element_type_entity_update, write_buffer_space_used, element);
			if (*element)
			{
				// copy the data to the buffer, enqueuing done later for entities
				csmemcpy((*element)->data, write_buffer, write_buffer_space_used);
				result = true;
			}
		}
	}

	return result;
}

void simulation_queue_entity_update_apply(const s_simulation_queue_element* element)
{
	s_simulation_queue_decoded_update_data decoded_update_data;

	if (game_is_distributed() && !game_is_playback())
	{
		csmemset(&decoded_update_data, 0, sizeof(decoded_update_data));

		if (decode_simulation_queue_update_from_buffer(element->data_size, element->data, &decoded_update_data))
		{
			c_simulation_entity_definition* entity_def = simulation_queue_entities_get_definition(decoded_update_data.entity_type);
			s_simulation_game_entity* game_entity = simulation_get_entity_database()->entity_try_and_get(decoded_update_data.entity_index);

			if (game_entity)
			{
				entity_def->update_game_entity(
					game_entity,
					decoded_update_data.update_mask,
					decoded_update_data.state_data_size,
					decoded_update_data.state_data
				);
			}
		}
	}
	else
	{
		// TODO: asserts here
	}
}

void simulation_queue_entity_deletion_insert(s_simulation_game_entity* entity)
{
	uint8 write_buffer[512];
	
	c_bitstream stream(write_buffer, sizeof(write_buffer));
	stream.begin_writing(k_bitstream_default_alignment);
	simulation_queue_entity_encode_header(&stream, entity->entity_type, NONE);
	//simulation_entity_index_encode(&stream, entity->entity_index);
	simulation_gamestate_index_encode(&stream, entity->object_index);		// Encode this as gamestate index isn't encoded
	int32 space_used = stream.get_space_used_in_bytes();

	s_simulation_queue_element* element = NULL;
	c_simulation_world* sim_world = simulation_get_world();

	if (!stream.error_occured())
	{
		sim_world->simulation_queue_allocate(_simulation_queue_element_type_entity_deletion, space_used, &element);
		if (element)
		{
			csmemcpy(element->data, write_buffer, space_used);
			sim_world->simulation_queue_enqueue(element);
		}
	}
	stream.finish_writing(NULL);
	return;
}

void simulation_queue_entity_deletion_apply(const s_simulation_queue_element* element)
{
	c_bitstream stream(element->data, element->data_size);
	stream.begin_reading();

	int32 entity_index;
	datum gamestate_index;
	e_simulation_entity_type entity_type;
	if (simulation_queue_entity_decode_header(&stream, &entity_type, &gamestate_index))
	{
		// the entity is deleted already, we need to use the object index
		// simulation_entity_index_decode(&stream, &entity_index);
		simulation_gamestate_index_decode(&stream, &gamestate_index);
		c_simulation_entity_definition* entity_def = simulation_queue_entities_get_definition(entity_type);

		// quite hacky, but should do the job
		s_simulation_game_entity game_entity;
		game_entity.entity_index = NONE;
		game_entity.entity_type = entity_type;
		game_entity.entity_update_flag = 0;
		game_entity.field_10 = 0;
		game_entity.event_reference_count = 0;
		game_entity.exists_in_gameworld = false;
		game_entity.object_index = gamestate_index;

		if (game_entity.object_index != NONE)
		{
			if (entity_def->delete_game_entity(&game_entity))
			{
				// SUCCESS
			}
		}
	}
	stream.finish_reading();
	return;
}

void simulation_queue_entity_promotion_insert(s_simulation_game_entity* entity)
{
	if (game_is_distributed() && !game_is_playback())
	{
		uint8 data[512];
		c_bitstream stream(data, sizeof(data));
		stream.begin_writing(k_bitstream_default_alignment);
		simulation_queue_entity_encode_header(&stream, entity->entity_type, NONE);
		//simulation_entity_index_encode(&stream, entity->entity_index);
		simulation_gamestate_index_encode(&stream, entity->object_index);		// Encode this as gamestate index isn't encoded
		int32 space_used = stream.get_space_used_in_bytes();
		if (!stream.error_occured())
		{
			c_simulation_world* sim_world = simulation_get_world();
			s_simulation_queue_element* element = NULL;
			sim_world->simulation_queue_allocate(_simulation_queue_element_type_entity_promotion, space_used, &element);
			if (element)
			{
				csmemcpy(element->data, data, space_used);
				sim_world->simulation_queue_enqueue(element);
			}
		}
		stream.finish_writing(NULL);
	}
	return;
}

void simulation_queue_entity_promotion_apply(const s_simulation_queue_element* element)
{
	if (game_is_distributed())
	{
		c_bitstream stream(element->data, element->data_size);
		stream.begin_reading();

		int32 entity_index;
		datum gamestate_index;
		e_simulation_entity_type entity_type;
		if (simulation_queue_entity_decode_header(&stream, &entity_type, &gamestate_index))
		{
			simulation_gamestate_index_decode(&stream, &gamestate_index);
			c_simulation_entity_definition* entity_def = simulation_queue_entities_get_definition(entity_type);

			// quite hacky, but should do the job
			s_simulation_game_entity game_entity;
			game_entity.entity_index = NONE;
			game_entity.entity_type = entity_type;
			game_entity.entity_update_flag = 0;
			game_entity.field_10 = 0;
			game_entity.event_reference_count = 0;
			game_entity.exists_in_gameworld = false;
			game_entity.object_index = gamestate_index;

			if (entity_def->promote_game_entity_to_authority(&game_entity))
			{
				// SUCCESS
			}
		}
		stream.finish_reading();
	}
	return;
}