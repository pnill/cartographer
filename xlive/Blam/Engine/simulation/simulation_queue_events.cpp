#include "stdafx.h"
#include "simulation_queue_events.h"

#include "game/game.h"
#include "simulation.h"
#include "simulation/simulation_world.h"
#include "simulation/game_interface/simulation_game_entities.h"
#include "simulation/simulation_gamestate_entities.h"

#include "H2MOD/GUI/ImGui_Integration/Console/ImGui_ConsoleImpl.h"

#include "memory/bitstream.h"

void simulation_event_encode_header(
	c_bitstream* stream,
	e_simulation_event_type event_type,
	int32 reference_count,
	datum* object_references
)
{
	stream->write_integer("event-type", event_type, 5);
	stream->write_integer("reference-count", reference_count, 2);

	for (int32 i = 0; i < reference_count; i++)
	{
		stream->write_bool("object-reference-exists", object_references[i] != NONE);
		if (object_references[i] != NONE)
		{
			simulation_gamestate_index_encode(stream, object_references[i]);
		}
	}
}

void simulation_event_decode_header(
	c_bitstream* stream,
	e_simulation_event_type* event_type,
	int32* reference_count,
	datum* object_references
)
{
	*event_type = (e_simulation_event_type)stream->read_integer("event-type", 5);
	*reference_count = stream->read_integer("reference-count", 2);

	for (int32 i = 0; i < *reference_count; i++)
	{
		if (stream->read_bool("object-reference-exists"))
		{
			simulation_gamestate_index_decode(stream, &object_references[i]);
		}
		else
		{
			object_references[i] = NONE;
		}
	}
}

static bool encode_event_to_buffer(
	uint8* out_buffer,
	int32 out_buffer_size,
	int32* out_written_size,
	e_simulation_event_type event_type,
	int32 reference_count,
	datum* object_index_references,
	int32 block_size,
	uint8* block
)
{
	c_bitstream stream(out_buffer, out_buffer_size);
	stream.begin_writing(k_bitstream_default_alignment);
	simulation_event_encode_header(&stream, event_type, reference_count, object_index_references);

	c_simulation_type_collection* sim_type_collection = simulation_get_type_collection();
	c_simulation_event_definition* sim_event_definition = sim_type_collection->get_event_definition(event_type);
	// write the event data to the stream
	sim_event_definition->encode(block_size, block, &stream);

	bool result = !stream.error_occured();
	stream.finish_writing(NULL);

	if (result)
	{
		SIM_EVENT_QUEUE_DBG("#####");
		SIM_EVENT_QUEUE_DBG("event encoding, stream is fine? %d, encoded size: %d", 
			!stream.error_occured(), 
			stream.get_space_used_in_bytes());
		SIM_EVENT_QUEUE_DBG("event type: %d, reference count: %d", event_type, reference_count);
	}
	else
	{
		SIM_EVENT_QUEUE_DBG("event encoding failed!!!! --------");
	}

	*out_written_size = stream.get_space_used_in_bytes();
	return result;
}

static bool decode_event_to_buffer(int32 encoded_size, uint8* encoded_data, s_simulation_queue_events_apply* decode_out)
{
	c_bitstream stream(encoded_data, encoded_size);
	stream.begin_reading();
	simulation_event_decode_header(&stream, &decode_out->event_type, &decode_out->reference_count, decode_out->object_refereces);

	c_simulation_type_collection* sim_type_collection = simulation_get_type_collection();
	c_simulation_event_definition* sim_entity_def = sim_type_collection->get_event_definition(decode_out->event_type);

	decode_out->data_size = sim_entity_def->payload_size();
	if (decode_out->data_size < k_simulation_payload_size_max)
	{
		if (decode_out->data_size > 0)
		{
			if (sim_entity_def->decode(decode_out->data_size, decode_out->data, &stream))
			{
			}
			else
			{
				// TODO debugging macros for statements, asserts, etc
				// debug
			}
		}
	}

	bool result = !stream.error_occured();
	stream.finish_reading();

	SIM_EVENT_QUEUE_DBG("#####");
	if (result)
	{
		// everything is fine
		SIM_EVENT_QUEUE_DBG("event decoding, stream is fine? %d, decoded size: %d, size of the payload: %d", 
			!stream.error_occured(),
			stream.get_space_used_in_bytes(), 
			decode_out->data_size);
		SIM_EVENT_QUEUE_DBG("event type: %d, reference count: %d", decode_out->event_type, decode_out->reference_count);

		for (int32 i = 0; i < decode_out->reference_count; i++)
		{
			SIM_EVENT_QUEUE_DBG("reference: %08X", decode_out->object_refereces[i]);
		}
	}
	else
	{
		SIM_EVENT_QUEUE_DBG("event decoding failed!!!! --------");
	}

	return result;
}

void simulation_queue_event_insert(e_simulation_event_type type, int32 reference_count, int32* entity_references, int32 block_size, uint8* block)
{
	int32 write_buffer_space_used;
	uint8 write_buffer[k_simulation_payload_size_max];
	datum object_indexes_from_entity_references[k_entity_reference_indices_count_max];
	s_simulation_queue_element* allocated_element = NULL;

	if (game_is_distributed() && !game_is_playback())
	{
		c_simulation_type_collection* simulation_type_collection = simulation_get_type_collection();
		c_simulation_event_definition* event_definition = simulation_type_collection->get_event_definition(type);

		// skip postprocess, not available in h2
		// call postprocess from event def
		// end

		SIM_EVENT_QUEUE_DBG("#####");
		for (int32 i = 0; i < reference_count; i++)
		{
			SIM_EVENT_QUEUE_DBG("entity reference indice				 -> %08X",
				entity_references[i]
			);
		}

		simulation_entity_indices_to_object_index(entity_references, reference_count, object_indexes_from_entity_references, k_entity_reference_indices_count_max);

		for (int32 i = 0; i < k_entity_reference_indices_count_max; i++)
		{
			SIM_EVENT_QUEUE_DBG("object reference indice (datum index) -> %08X",
				object_indexes_from_entity_references[i]
			);
		}

		if (encode_event_to_buffer(
			write_buffer,
			sizeof(write_buffer),
			&write_buffer_space_used,
			type,
			k_entity_reference_indices_count_max,
			object_indexes_from_entity_references,
			block_size,
			block))
		{
			simulation_get_world()->simulation_queue_allocate(_simulation_queue_element_type_event, write_buffer_space_used, &allocated_element);
			if (allocated_element)
			{
				// copy the data to the buffer
				csmemcpy(allocated_element->data, write_buffer, write_buffer_space_used);

				// copy it to the queue
				simulation_get_world()->simulation_queue_enqueue(allocated_element);

				SIM_EVENT_QUEUE_DBG("added element 0x%08X, type: %d, size: %d to simulation queue", 
					allocated_element, 
					allocated_element->type, 
					allocated_element->data_size
				);
			}
		}
	}
}

bool simulation_queue_event_apply(const s_simulation_queue_element* update)
{
	s_simulation_queue_events_apply sim_apply_data;
	bool decode_success = decode_event_to_buffer(update->data_size, update->data, &sim_apply_data);

	bool apply_event_result = false;

	// apply the decoded event to sim
	if (decode_success)
	{
		c_simulation_type_collection* sim_type_collection = simulation_get_type_collection();
		c_simulation_event_definition* sim_entity_def = sim_type_collection->get_event_definition(sim_apply_data.event_type);

		// convert object indexes back to entity references, lol
		// ### FIXME !!! at some point, add the gamestate to entity database
		int32 entity_references[k_entity_reference_indices_count_max];

		for (int32 i = 0; i < sim_apply_data.reference_count; i++)
		{
			if (!DATUM_IS_NONE(sim_apply_data.object_refereces[i]))
			{
				// hopefully this does the job
				// TODO FIXME this crashed the game once
				// the events need to be discarded whenever simulation layer initializes
				entity_references[i] = object_get_entity_index(sim_apply_data.object_refereces[i]);
			}
			else
			{
				entity_references[i] = NONE;
			}

			SIM_EVENT_QUEUE_DBG("object reference (datum index): %08X converted -> entity reference: %08X: ", sim_apply_data.object_refereces[i], entity_references[i]);
		}

		// ### FIXME !!! this actually requires entity references, instead of object references
		apply_event_result = sim_entity_def->perform(sim_apply_data.reference_count, entity_references, sim_apply_data.data_size, sim_apply_data.data);

		SIM_EVENT_QUEUE_DBG("type %d decoded: %d, perform result: %d, element: 0x%08X, perform data size: %d",
			sim_apply_data.event_type,
			decode_success,
			apply_event_result,
			update,
			sim_apply_data.data_size);
	}
	
	return apply_event_result;
}
