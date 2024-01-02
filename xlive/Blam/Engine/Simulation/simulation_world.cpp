#include "stdafx.h"
#include "simulation_world.h"

#include "game/game.h"

// TODO verify if these buffers get saturated quickly
// if that's the case, increse the buffer size
c_simulation_queue g_entity_update_queue;
c_simulation_queue g_event_update_queue;

void c_simulation_world::simulation_queue_allocate(e_event_queue_type type, int32 size, s_simulation_queue_element** out_allocated_elem)
{
	if (TEST_FLAG(FLAG(type), _simulation_queue_element_type_1))
	{
		// player event, player update, gamestate clear
		g_event_update_queue.allocate(size, out_allocated_elem);
	}
	else
	{
		bool sim_entity_queue_full = false;

		if (TEST_FLAG(FLAG(type), _simulation_queue_element_type_2))
		{
			// entity_deletion, entity_promotion, game_global_event

			// TODO
			// c_simulation_queue::get_allocation_status()
			// check the allocation status
			// this type of elements can be skupped from being queued, in case of overflow??
			// sim_entity_queue_full = true;
		}

		// event, creation, update, entity_deletion, entity_promotion, game_global_event
		if (!sim_entity_queue_full)
			g_entity_update_queue.allocate(size, out_allocated_elem);

	}

	if (out_allocated_elem)
	{
		(*out_allocated_elem)->type = type;
	}
}

void c_simulation_world::simulation_queue_enqueue(s_simulation_queue_element* element)
{
	if (TEST_FLAG(FLAG(element->type), _simulation_queue_element_type_1))
	{
		// player event, player update, gamestate clear
		g_event_update_queue.enqueue(element);
	}
	else
	{
		// event, creation, update, entity_deletion, entity_promotion, game_global_event
		g_entity_update_queue.enqueue(element);
	}
}

void c_simulation_world::apply_simulation_queue(c_simulation_queue* queue)
{
	if (queue->queued_count() > 0)
	{
		const s_simulation_queue_element* element = queue->get_first_element();

		while (element != NULL)
		{
			switch (element->type)
			{
			case _simulation_queue_element_type_event:
				// simulation_apply_event_to_gamestate(element);
				break;
			case _simulation_queue_element_type_entity_creation:
				break;
			case _simulation_queue_element_type_entity_update:
				break;
			case _simulation_queue_element_type_entity_deletion:
				break;
			case _simulation_queue_element_type_entity_promotion:
				break;
			case _simulation_queue_element_type_game_global_event:
				break;
			case _simulation_queue_element_type_player_event:
				break;
			case _simulation_queue_element_type_player_update_event:
				break;
			case _simulation_queue_element_type_gamestates_clear:
				break;
			//case _simulation_queue_element_type_sandbox_event:
				//break;
			default:
				// DEBUG unk event type
				break;
			}

			element = queue->get_next_element(element);
		}
	}
}

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

void simulation_queue_event_apply_to_gamestate()
{

}

void simulation_world_queue_apply_hooks()
{

}

void simulation_world_apply_patches()
{

}
