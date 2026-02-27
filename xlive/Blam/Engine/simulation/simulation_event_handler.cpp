#include "stdafx.h"
#include "simulation_event_handler.h"

#include "simulation_queue_events.h"

#include "math/random_math.h"
#include "simulation/game_interface/simulation_game_events.h"

#include "networking/network_memory.h"
#include "networking/replication/replication_control_view.h"

/* globals */

bool g_use_network_queue_storage = true;

typedef void(__thiscall* t_process_incoming_event)(c_simulation_event_handler* thisx, e_simulation_event_type simulation_event_type, int32* entity_reference_indices, int32 block_count, s_replication_allocation_block* block);
t_process_incoming_event p_process_incoming_event;

/* prototypes */

CLASS_HOOK_DECLARE_LABEL(c_simulation_event_handler__process_incoming_event, c_simulation_event_handler::process_incoming_event);
static __declspec(naked) void jmp_c_simulation_event_handler_process_incoming_event()
{
	CLASS_HOOK_JMP(c_simulation_event_handler__process_incoming_event, c_simulation_event_handler::process_incoming_event);
}

/* public code */

void simulation_event_handler_apply_patches(void)
{
	DETOUR_ATTACH(p_process_incoming_event, Memory::GetAddress<t_process_incoming_event>(0x1D3E02, 0x1D9092), jmp_c_simulation_event_handler_process_incoming_event);
	return;
}

int32 c_simulation_event_handler::read_incoming_event(int32 a2, int32 a3, int32 a4, uint32* a5, int32 a6, int32 a7)
{
	return INVOKE_TYPE(0x1D3D45, 0x1D8FD5, int32(__thiscall*)(int32, int32, int32, uint32*, int32, int32), a2, a3, a4, a5, a6, a7);
}

void c_simulation_event_handler::process_incoming_event(e_simulation_event_type event_type, int32* entity_reference_indices, int32 block_count, s_replication_allocation_block* payload_block)
{
	ASSERT(VALID_INDEX(event_type, m_type_collection->get_event_definition_count()));
	ASSERT(entity_reference_indices);

	c_simulation_event_definition* event_definition = m_type_collection->get_event_definition(event_type);
	ASSERT(event_definition);

	if (g_use_network_queue_storage)
	{
		int32 entity_reference_indices_count = event_definition->number_of_entity_references();


		void* block_data;
		int32 block_size;
		if (event_definition->payload_size() <= 0)
		{
			ASSERT(block_count==0);
			block_data = NULL;
			block_size = 0;
		}
		else
		{
			ASSERT(block_count==1);
			ASSERT(payload_block->block_size == (int16)event_definition->payload_size());
			ASSERT(payload_block->block_type == _network_memory_block_simulation_event);
			ASSERT(payload_block->block_data != NULL);

			block_data = payload_block->block_data;
			block_size = payload_block->block_size;
			payload_block->block_data = NULL;
		}
		simulation_queue_event_insert(event_type, entity_reference_indices_count, entity_reference_indices, block_size, block_data);

		if (block_data)
		{
			network_heap_free_block(block_data);
		}
	}
	else
	{
		random_seed_allow_use();
		// call the original
		p_process_incoming_event(this, event_type, entity_reference_indices, block_count, payload_block);
		random_seed_disallow_use();
	}
	return;
}

void c_simulation_event_handler::write_outgoing_event(int32 a2, int32 a3, uint32* a4, int8* a5)
{
	INVOKE_TYPE(0x1D3E75, 0x1D9105, void(__thiscall*)(c_simulation_event_handler*, int32, int32, uint32*, int8*), this, a2, a3, a4, a5);
}

void c_simulation_event_handler::notify_outgoing_event_retired(int32 a2)
{
	INVOKE_TYPE(0x01D3FAF, 0x1D923F, void(__thiscall*)(c_simulation_event_handler*, int32), this, a2);
}

int8 c_simulation_event_handler::calculate_requirements(int32 a2, uint32** a3, real32* a4, uint32* a5)
{
	return INVOKE_TYPE(0x1D3E9D, 0x1D912D, int8(__thiscall*)(c_simulation_event_handler*, int32, uint32**, real32*, uint32*), this, a2, a3, a4, a5);
}

int32 c_simulation_event_handler::write_description_to_string(int32 a2, int32 a3, int32 a4, int32 a5)
{
	return INVOKE_TYPE(0x1D3F70, 0x1D9200, int32(__thiscall*)(c_simulation_event_handler*, int32, int32, int32, int32), this, a2, a3, a4, a5);
}
