#include "stdafx.h"
#include "simulation_queue_events.h"

#include "game/game.h"
#include "simulation.h"
#include "Simulation/simulation_event_handler.h"
#include "Simulation/game_interface/simulation_game_entities.h"

#include "memory/bitstream.h"

static void encode_object_index_reference(bitstream* stream, datum object_index_references)
{
	stream->data_encode_integer("gamestate-index-id", DATUM_INDEX_TO_IDENTIFIER(object_index_references), 16);
	stream->data_encode_integer("gamestate-index-absolute", DATUM_INDEX_TO_ABSOLUTE_INDEX(object_index_references), 11);
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
	bitstream stream(out_buffer, out_buffer_size);
	stream.begin_writing(k_bitstream_default_alignment);
	stream.data_encode_integer("event-type", event_type, 5);
	stream.data_encode_integer("reference-count", reference_count, 5);

	for (int32 i = 0; i < reference_count; i++)
	{
		stream.data_encode_bool("object-index-exists", !DATUM_IS_NONE(object_index_references[i]));
		if (!DATUM_IS_NONE(object_index_references[i]))
		{
			encode_object_index_reference(&stream, object_index_references[i]);
		}
	}

	c_simulation_type_collection* sim_type_collection = simulation_get_type_collection();
	c_simulation_event_definition* sim_event_definition = sim_type_collection->get_event_definition(event_type);
	// write the event data to the stream
	sim_event_definition->get_vtbl()->encode(sim_event_definition, block_size, block, &stream);

	stream.finish_writing(NULL);
	*out_written_size = stream.get_space_used_in_bytes();

	return !stream.overflow();
}

void simulation_queue_event_insert(e_simulation_event_type type, int32 reference_count, int32* references, int32 block_size, uint8* block)
{
	datum object_indexes_from_entity_references[k_entity_reference_indices_count_max];
	uint8 write_buffer[k_simulation_payload_size_max];
	int32 write_buffer_space_used;
	s_simulation_queue_element* allocated_element;

	if (game_is_distributed() && !game_is_playback())
	{
		c_simulation_type_collection* simulation_type_collection = simulation_get_type_collection();
		c_simulation_event_definition* event_definition = simulation_type_collection->get_event_definition(type);

		// skip postprocess, not available in h2
		// call postprocess from event def
		// end

		simulation_entity_indices_to_object_index(references, reference_count, object_indexes_from_entity_references, k_entity_reference_indices_count_max);

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
			c_simulation_world::simulation_queue_allocate(_simulation_queue_element_type_event, write_buffer_space_used, &allocated_element);
			if (allocated_element)
			{
				// copy the data to the buffer
				csmemcpy(allocated_element->data, write_buffer, write_buffer_space_used);

				// copy it to the queue
				c_simulation_world::simulation_queue_enqueue(allocated_element);
			}
		}
	}
}

void simulation_event_queue_apply(const s_simulation_queue_element* update)
{
}
