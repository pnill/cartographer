#include "stdafx.h"
#include "simulation_event_handler.h"

#include "Simulation/simulation_world.h"
#include "simulation/game_interface/simulation_game_events.h"

#include "Networking/memory/networking_memory.h"

#include "Util/Hooks/Hook.h"

bool g_use_network_queue_storage = true;

typedef void(__thiscall* t_process_incoming_event)(c_simulation_event_handler* thisx, e_simulation_event_type simulation_event_type, int32* entity_reference_indices, int32 block_count, s_payload_block* block);
t_process_incoming_event p_process_incoming_event;

void c_simulation_event_handler::process_incoming_event(e_simulation_event_type simulation_event_type, int32* entity_reference_indices, int32 block_count, s_payload_block* payload_blocks)
{
	if (g_use_network_queue_storage)
	{
		c_simulation_event_definition* sim_event_def = m_event_type_collection->get_event_definition(simulation_event_type);
		int32 entity_reference_indices_count = sim_event_def->number_of_entity_references();

		SIM_QUEUE_DBG(
			"received simulation event type: %d, entity reference count: %d, block count: %d, payload size: %d", 
			simulation_event_type,  
			entity_reference_indices_count,
			block_count,
			sim_event_def->payload_size()
		);

		if (sim_event_def->payload_size() > 0)
		{
			uint8* block = payload_blocks->block_data;
			int32 block_size = payload_blocks->block_size;

			SIM_QUEUE_DBG(
				"event block: 0x%08X payload size from block: %d",
				block,
				block_size
			);

			for (int32 i = 0; i < entity_reference_indices_count; i++)
			{
				SIM_QUEUE_DBG("event entity reference indices: %08X", entity_reference_indices[i]);
			}

			simulation_queue_event_insert(simulation_event_type, entity_reference_indices_count, entity_reference_indices, block_size, block);
		}
		else
		{
			SIM_QUEUE_DBG("payload size : 0");

			for (int32 i = 0; i < entity_reference_indices_count; i++)
			{
				SIM_QUEUE_DBG("event entity reference indices: %d", entity_reference_indices[i]);
			}

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
		p_process_incoming_event(this, simulation_event_type, entity_reference_indices, block_count, payload_blocks);
	}
}

__declspec(naked) void jmp_c_simulation_event_handler_process_incoming_event() { __asm { jmp c_simulation_event_handler::process_incoming_event } }

void simulation_event_handler_apply_patches()
{
	DETOUR_ATTACH(p_process_incoming_event, Memory::GetAddress<t_process_incoming_event>(0x1D3E02), jmp_c_simulation_event_handler_process_incoming_event);
}