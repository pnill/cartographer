#include "stdafx.h"
#include "simulation_event_handler.h"

#include "Simulation/simulation.h"
#include "Simulation/simulation_events_queue.h"
#include "simulation/game_interface/simulation_game_events.h"

#include "Networking/memory/networking_memory.h"

bool g_use_network_queue_storage = true;

void c_simulation_event_handler::process_incoming_event(e_simulation_event_type simulation_event_type, int32* entity_reference_indices, int32 block_count, s_payload_block* payload_blocks)
{
	if (g_use_network_queue_storage)
	{
		c_simulation_event_definition* event_definition = m_event_type_collection->get_event_definition(simulation_event_type);
		c_simulation_event_definition_vtbl* event_def_vtbl = event_definition->get_vtbl();
		int32 entity_reference_indices_count = event_def_vtbl->number_of_entity_references(event_definition);

		if (event_def_vtbl->payload_size(event_definition) > 0)
		{
			uint8* block = payload_blocks->block_data;
			int32 block_size = payload_blocks->block_size;

			simulation_queue_event_insert(simulation_event_type, entity_reference_indices_count, entity_reference_indices, block_size, block);
		}
		else
		{
			simulation_queue_event_insert(simulation_event_type, entity_reference_indices_count, entity_reference_indices, 0, NULL);
		}

		if (payload_blocks->block_data != NULL)
		{
			network_heap_free_block((uint8*)payload_blocks);
			payload_blocks->block_data = NULL;
		}
	}
	else
	{
		// call the original
		get_vtbl()->process_incoming_event(this, simulation_event_type, entity_reference_indices, block_count, payload_blocks);
	}
}

void simulation_event_encode_to_buffer()
{

}


