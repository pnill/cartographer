#include "stdafx.h"
#include "simulation_entity_database.h"

#include "simulation.h"
#include "simulation_queue_entities.h"

#include "networking/network_event.h"
#include "networking/network_memory.h"

/* prototypes */

CLASS_HOOK_DECLARE_LABEL(c_simulation_entity_database__read_creation_from_packet, c_simulation_entity_database::read_creation_from_packet);
static __declspec(naked) void jmp_c_simulation_entity_database__read_creation_from_packet()
{
	CLASS_HOOK_JMP(c_simulation_entity_database__read_creation_from_packet, c_simulation_entity_database::read_creation_from_packet);
}

CLASS_HOOK_DECLARE_LABEL(c_simulation_entity_database__process_creation, c_simulation_entity_database::process_creation);
static __declspec(naked) void jmp_c_simulation_entity_database__process_creation()
{
	CLASS_HOOK_JMP(c_simulation_entity_database__process_creation, c_simulation_entity_database::process_creation);
}

CLASS_HOOK_DECLARE_LABEL(c_simulation_entity_database__read_update_from_packet, c_simulation_entity_database::read_update_from_packet);
static __declspec(naked) void jmp_c_simulation_entity_database__read_update_from_packet()
{
	CLASS_HOOK_JMP(c_simulation_entity_database__read_update_from_packet, c_simulation_entity_database::read_update_from_packet);
}

CLASS_HOOK_DECLARE_LABEL(c_simulation_entity_database__process_update, c_simulation_entity_database::process_update);
static __declspec(naked) void jmp_c_simulation_entity_database__process_update()
{
	CLASS_HOOK_JMP(c_simulation_entity_database__process_update, c_simulation_entity_database::process_update);
}

CLASS_HOOK_DECLARE_LABEL(c_simulation_entity_database__notify_mark_entity_for_deletion, c_simulation_entity_database::notify_mark_entity_for_deletion);
static __declspec(naked) void jmp_c_simulation_entity_database__notify_mark_entity_for_deletion()
{
	CLASS_HOOK_JMP(c_simulation_entity_database__notify_mark_entity_for_deletion, c_simulation_entity_database::notify_mark_entity_for_deletion);
}

CLASS_HOOK_DECLARE_LABEL(c_simulation_entity_database__notify_promote_to_authority, c_simulation_entity_database::notify_promote_to_authority);
static __declspec(naked) void jmp_c_simulation_entity_database__notify_promote_to_authority()
{
	CLASS_HOOK_JMP(c_simulation_entity_database__notify_promote_to_authority, c_simulation_entity_database::notify_promote_to_authority);
}

/* public code */

void simulation_entity_database_apply_patches(void)
{
	LLVM_JMP_ERROR;
	WritePointer(Memory::GetAddress(0x3C6228, 0x381D10), jmp_c_simulation_entity_database__read_creation_from_packet);
	WritePointer(Memory::GetAddress(0x3C622C, 0x381D14), jmp_c_simulation_entity_database__process_creation);
	// allow the creation of turrets by increasing the block count, block count was hardcoded
	WriteValue<int8>(Memory::GetAddress(0x1D7081, 0x1DA3A2) + 1, (int8)k_entity_creation_block_order_count);
	WriteValue<int8>(Memory::GetAddress(0x1D7091, 0x1DA3B2) + 2, (int8)sizeof(s_replication_allocation_block) * k_entity_creation_block_order_count);

	WritePointer(Memory::GetAddress(0x3C623C, 0x381D24), jmp_c_simulation_entity_database__read_update_from_packet);
	WritePointer(Memory::GetAddress(0x3C6240, 0x381D28), jmp_c_simulation_entity_database__process_update);

	WritePointer(Memory::GetAddress(0x3C624C, 0x381D34), jmp_c_simulation_entity_database__notify_mark_entity_for_deletion);

	WritePointer(Memory::GetAddress(0x3C6258, 0x381D40), jmp_c_simulation_entity_database__notify_promote_to_authority);

	// don't update the weapon state if we didn't actually received an update
	// for some reason someone at bungie thought it was a good idea to apply an weapon ammo update
	// even if we received just the weapon definition
	NopFill(Memory::GetAddress(0x1F836A, 0x1E20D0), 4);

	return;
}

c_simulation_entity_database* simulation_get_entity_database(void)
{
	return &simulation_get_world()->get_distributed_world()->m_entity_database;
}

bool c_simulation_entity_database::process_creation(int32 entity_index, e_simulation_entity_type entity_type, uint32 update_mask, int32 block_count, s_replication_allocation_block* blocks)
{
	ASSERT(entity_index != NONE);

	const uint32 absolute_index = ENTITY_INDEX_TO_ABSOLUTE_INDEX(entity_index);
	ASSERT(VALID_INDEX(absolute_index, k_simulation_entity_database_maximum_entities));

	ASSERT(m_entity_manager);
	//ASSERT(m_entity_manager->is_entity_allocated(entity_index));
	ASSERT(entity_type != NONE);

	// TODO: implement gamestate
	//ASSERT(block_count == 4);
	
	ASSERT(blocks[0].block_type == _network_memory_block_simulation_entity_creation);
	ASSERT(blocks[1].block_type == _network_memory_block_simulation_entity_state);
	//ASSERT(blocks[2].block_type == _network_memory_block_forward_gamestate_element);
	//ASSERT(blocks[3].block_type == _network_memory_block_forward_simulation_queue_element);

	bool result = false;
	s_simulation_game_entity* entity = entity_get(entity_index);
	entity->entity_index = entity_index;
	entity->entity_type = entity_type;
	entity->entity_update_flag = 0;
	entity->field_10 = 0;
	entity->event_reference_count = 0;
	entity->exists_in_gameworld = false;
	entity->object_index = NONE;

	// we could also validate here the entity_type of the blocks
	entity->creation_data = blocks[_entity_creation_block_order_simulation_entity_creation].block_data;
	entity->creation_data_size = blocks[_entity_creation_block_order_simulation_entity_creation].block_size;
	entity->state_data = blocks[_entity_creation_block_order_simulation_entity_state].block_data;
	entity->state_data_size = blocks[_entity_creation_block_order_simulation_entity_state].block_size;

	const uint32 creation_data_size = blocks[_entity_creation_block_order_simulation_entity_creation].block_size;
	const void* creation_data = blocks[_entity_creation_block_order_simulation_entity_creation].block_data;
	const uint32 state_data_size = blocks[_entity_creation_block_order_simulation_entity_state].block_size;
	const void* state_data = blocks[_entity_creation_block_order_simulation_entity_state].block_data;

	// TODO when we implement this
	// datum gamestate_index = *(datum*)blocks[_entity_creation_block_order_gamestate_index].block_data;
	s_simulation_queue_element* simulation_queue_element = *(s_simulation_queue_element**)blocks[_entity_creation_block_order_forward_memory_queue_element].block_data;

	// free these blocks
	network_heap_free_block((uint8*)blocks[_entity_creation_block_order_forward_memory_queue_element].block_data);
	// network_heap_free_block((uint8*)blocks[_entity_creation_block_order_gamestate_index].block_data);

	csmemset(blocks, 0, sizeof(s_replication_allocation_block) * block_count);

	const c_simulation_entity_definition* entity_definition = m_type_collection->get_entity_definition(entity_type);
	
	ASSERT(entity_definition != NULL);
	ASSERT(creation_data_size == entity_definition->creation_data_size());
	ASSERT(state_data_size == entity_definition->state_data_size());
	ASSERT((creation_data == NULL) == (creation_data_size == 0));
	ASSERT((state_data == NULL) == (state_data_size == 0));
	//ASSERT(gamestate_index != NONE);
	ASSERT(simulation_queue_element != NULL);

	// ### TODO FIXME create objects during game tick
	// works-around an issue where desync could occur because the entity wouldn't get an object index allocated 
	// amd attached until very late
	simulation_queue_entity_creation_apply(simulation_queue_element);
	simulation_get_world()->simulation_queue_free(simulation_queue_element);
	//simulation_queue_entity_creation_insert(simulation_queue_element);
	result = true;
	
	/*result = entity_definition->create_game_entity(
		entity,
		entity->creation_data_size,
		entity->creation_data,
		update_mask,
		entity->state_data_size,
		entity->state_data);*/
	//entity->exists_in_gameworld = result;
	return result;
}

uint32 c_simulation_entity_database::read_creation_from_packet(int32 entity_index, 
	e_simulation_entity_type* out_entity_type, 
	uint32* out_entity_initial_update_mask, 
	int32 maximum_block_count,
	int32* block_count, 
	s_replication_allocation_block* blocks,
	c_bitstream* packet)
{
	const uint16 absolute_index = ENTITY_INDEX_TO_ABSOLUTE_INDEX(entity_index);

	ASSERT(VALID_INDEX(absolute_index, k_simulation_entity_database_maximum_entities));
	ASSERT(packet);

	uint32 result = 3;
	e_simulation_entity_type entity_type = (e_simulation_entity_type)packet->read_integer("entity-type", 5);
	c_simulation_entity_definition* entity_definition = m_type_collection->get_entity_definition(entity_type);
	
	if (entity_definition)
	{
		uint32 creation_data_size = entity_definition->creation_data_size();
		uint32 state_data_size = entity_definition->state_data_size();

		// Allocate creation data
		uint8* creation_data = NULL;
		if (creation_data_size > 0)
		{
			creation_data = network_heap_allocate_block(creation_data_size);
			if (!creation_data)
			{
#ifdef EVENTS_ENABLED
				char description[1024];
				event(
					_event_error,
					"networking:simulation:entity: OUT OF MEMORY allocating %s creation data [%d] bytes [%s]",
					creation_data_size,
					network_heap_describe(description, sizeof(description))
				);
#endif
				result = 2;
			}
		}

		// Allocate state data
		uint8* state_data = network_heap_allocate_block(state_data_size);
		if (!state_data)
		{
#ifdef EVENTS_ENABLED
			char description[1024];
			event(
				_event_error, 
				"networking:simulation:entity: OUT OF MEMORY allocating %s state data [%d] bytes, heap [%s]",
				state_data_size,
				network_heap_describe(description, sizeof(description))
			);
#endif
			result = 2;
		}

		// Allocate gamestate data
		/*
		int32* gamestate_index = (int32*)network_heap_allocate_block(sizeof(int32));
		if (!gamestate_index)
		{
#ifdef EVENTS_ENABLED
			char description[1024];
			event(_event_error, "networking:simulation:entity: OUT OF MEMORY allocating %s gamestate data [%d] bytes [%s]", sizeof(int32), network_heap_describe(description, sizeof(description)));
#endif
			result = 2;
		}
		*/

		// Allocate queue data
		uint8* simulation_queue_element = (uint8*)network_heap_allocate_block(sizeof(s_simulation_queue_element*));
		if (!simulation_queue_element)
		{
#ifdef EVENTS_ENABLED
			char description[1024];
			event(
				_event_error,
				"networking:simulation:entity: OUT OF MEMORY allocating %s simulation queue data [%d] bytes [%s]",
				sizeof(s_simulation_queue_element*),
				network_heap_describe(description, sizeof(description))
			);
#endif
			result = 2;
		}

		// check if creation size is > 0 and if network heap block have been successfully allocated
		if ((!creation_data_size || creation_data != NULL) && (state_data && simulation_queue_element /* && gamestate_index*/))
		{
			if (creation_data_size > 0)
			{
				csmemset(creation_data, 0, creation_data_size);
			}

			*simulation_queue_element = NULL;

			if (entity_definition->entity_creation_decode(creation_data_size, creation_data, packet))
			{
				if (entity_definition->build_baseline_state_data(
					creation_data_size,
					creation_data,
					state_data_size,
					(s_simulation_baseline_state_data*)state_data))
				{
					uint32 entity_initial_update_mask = 0;
					uint32 entity_allowed_initial_update_mask = entity_definition->initial_update_mask();

					bool read_success = true;
					if (entity_allowed_initial_update_mask != 0 && packet->read_bool("initial-update-exists"))
					{
						if (entity_definition->entity_update_decode(true, &entity_initial_update_mask, state_data_size, state_data, packet))
						{
							// check if the state contains updates allowed only on creation
							if (TEST_FLAG(entity_initial_update_mask, ~entity_allowed_initial_update_mask))
							{
								event(
									_event_error,
									"networking:simulation:entity:read_update_from_packet: initial creation update read [0x%08X] (expecting mask of [0x%08X]) for entity [0x%08X] type %d",
									entity_initial_update_mask,
									entity_allowed_initial_update_mask,
									entity_type,
									entity_definition->entity_type());
								read_success = false;
							}
						}
						else
						{
							event(
								_event_error,
								"networking:simulation:entity:read_update_from_packet: failed to decode initial creation update for entity %d/%s/0x%08X",
								entity_type,
								entity_definition->entity_type_name(),
								entity_index);
							read_success = false;
						}
					}

					if (read_success)
					{
						/* Original code
						*out_entity_type = entity_type;
						*out_entity_initial_update_mask = entity_initial_update_mask;
						blocks[*block_count].block_type = _network_memory_block_simulation_entity_creation;
						blocks[*block_count].block_size = creation_data_size;
						blocks[*block_count].block_data = creation_data;
						blocks[*block_count + 1].block_type = _network_memory_block_simulation_entity_state;
						blocks[*block_count + 1].block_size = state_data_size;
						blocks[*block_count + 1].block_data = state_data;
						*block_count += 2;
						*/

						s_simulation_queue_entity_data sim_queue_entity_data;
						sim_queue_entity_data.entity_index = entity_index;
						sim_queue_entity_data.entity_type = entity_type;
						sim_queue_entity_data.creation_data_size = creation_data_size;
						sim_queue_entity_data.creation_data = creation_data;
						sim_queue_entity_data.state_data_size = state_data_size;
						sim_queue_entity_data.state_data = state_data;

						if (!packet->read_only_for_consistency()
							&& !simulation_queue_entity_creation_allocate(&sim_queue_entity_data, entity_initial_update_mask, (s_simulation_queue_element**)simulation_queue_element, NULL))
						{
							read_success = false;
						}
					}

					if (read_success)
					{
						ASSERT(out_entity_type);
						ASSERT(out_entity_initial_update_mask);

						*out_entity_type = entity_type;
						*out_entity_initial_update_mask = entity_initial_update_mask;

						ASSERT(block_count);
						ASSERT(blocks);
						ASSERT(*block_count + 4 <= maximum_block_count);

						//blocks[_entity_creation_block_order_gamestate_index].block_type = _network_memory_block_forward_gamestate_element;
						//blocks[_entity_creation_block_order_gamestate_index].block_size = sizeof(gamestate_index);
						//blocks[_entity_creation_block_order_gamestate_index].block_data = gamestate_index;

						// copy the block, allow the process function to use this
						blocks[*block_count + _entity_creation_block_order_simulation_entity_creation].block_type = _network_memory_block_simulation_entity_creation;
						blocks[*block_count + _entity_creation_block_order_simulation_entity_creation].block_size = (int16)creation_data_size;
						blocks[*block_count + _entity_creation_block_order_simulation_entity_creation].block_data = creation_data;

						blocks[*block_count + _entity_creation_block_order_simulation_entity_state].block_type = _network_memory_block_simulation_entity_state;
						blocks[*block_count + _entity_creation_block_order_simulation_entity_state].block_size = (int16)state_data_size;
						blocks[*block_count + _entity_creation_block_order_simulation_entity_state].block_data = state_data;

						blocks[*block_count + _entity_creation_block_order_forward_memory_queue_element].block_type = _network_memory_block_forward_simulation_queue_element;
						blocks[*block_count + _entity_creation_block_order_forward_memory_queue_element].block_size = sizeof(s_simulation_queue_element*);
						blocks[*block_count + _entity_creation_block_order_forward_memory_queue_element].block_data = simulation_queue_element;

						*block_count += k_entity_creation_block_order_count;

						result = 0;
					}
					else
					{
						event(
							_event_error,
							"networking:simulation:entity:read_creation_from_packet: unable to read initial update for entity %d/%s/[0x%08X]",
							entity_type,
							entity_definition->entity_type_name(),
							entity_index);
						result = 3;
					}

					if (!packet->read_only_for_consistency())
					{
						// FIXME: figure out why this breaks...
						//DISPLAY_ASSERT("entity initial update decode failed consistency checking a packet");
					}
				}
			}
			else
			{
				event(
					_event_error,
					"networking:simulation:entity:read_creation_from_packet: corrupt creation data reading entity %d/%s/[0x%08X]",
					entity_type,
					entity_definition->entity_type_name(),
					entity_index);
				result = 3;

				if (!packet->read_only_for_consistency())
				{
					DISPLAY_ASSERT("entity initial update decode failed consistency checking a packet");
				}
			}
		}

		if (result == 3 || result == 2)
		{
			if (creation_data != NULL)
			{
				network_heap_free_block(creation_data);
			}

			if (state_data != NULL)
			{
				network_heap_free_block(state_data);
			}

			if (simulation_queue_element != NULL)
			{
				if (*simulation_queue_element != NULL)
				{
					simulation_get_world()->simulation_queue_free(*(s_simulation_queue_element**)simulation_queue_element);
				}

				network_heap_free_block(simulation_queue_element);
			}

			/*if (gamestate_index != NULL)
			{
				network_heap_free_block((uint8*)gamestate_index);
			}*/
		}
	}
	else
	{
		event(_event_error, "simulation:entity: read_creation_from_packet: unknown entity type %d", entity_type);
	}

	return result;
}

bool c_simulation_entity_database::process_update(int32 entity_index, uint32 update_mask, int32 block_count, s_replication_allocation_block* blocks)
{
	bool result = false;
	s_simulation_game_entity* entity = entity_get(entity_index);
	//c_simulation_entity_definition* entity_definition = m_type_collection->get_entity_definition(entity->entity_type);
	s_simulation_queue_element* simulation_queue_element = *(s_simulation_queue_element**)blocks[_entity_update_block_order_forward_memory_queue_element].block_data;
	uint8* state_data = (uint8*)blocks[_entity_update_block_order_simulation_entity_state].block_data;

	csmemcpy(entity->state_data, state_data, entity->state_data_size);
	simulation_queue_entity_update_insert(simulation_queue_element);
	network_heap_free_block((uint8*)blocks[_entity_update_block_order_forward_memory_queue_element].block_data);
	csmemset(&blocks[_entity_update_block_order_forward_memory_queue_element], 0, sizeof(blocks[_entity_update_block_order_forward_memory_queue_element]));
	result = true;
	return result;
}

int32 c_simulation_entity_database::read_update_from_packet(
	int32 entity_index, 
	uint32* out_update_mask, 
	int32 maximum_block_count, 
	int32* block_count, 
	s_replication_allocation_block* blocks,
	c_bitstream* packet
)
{
	uint32 result = 3;
	s_simulation_game_entity* entity = entity_try_and_get(entity_index);
	if (entity && entity != (void*)-20)
	{
		c_simulation_entity_definition* entity_definition = m_type_collection->get_entity_definition(entity->entity_type);
		
		ASSERT(entity_definition != NULL);
		ASSERT(entity->state_data_size == entity_definition->state_data_size());

		// Allocate state data
		uint8* state_data = network_heap_allocate_block(entity->state_data_size);
		uint8* simulation_queue_element = network_heap_allocate_block(sizeof(s_simulation_queue_element*));

		result = (!state_data || !simulation_queue_element ? 2 : result);

		if (state_data && simulation_queue_element)
		{
			uint32 update_mask = 0;
			bool read_success = false;

			if (packet->read_only_for_consistency())
			{
				read_success = entity_definition->build_baseline_state_data(
						entity->creation_data_size,
						entity->creation_data, 
						entity->state_data_size, 
						(s_simulation_baseline_state_data*)state_data);

				if (!read_success)
				{
					event(
						_event_error,
						"networking:simulation:entity:read_update_from_packet: unable to build baseline data for entity [0x%08X] type %d",
						entity_index,
						entity->entity_type);
					result = 3;
				}
			}
			else
			{
				csmemcpy(state_data, entity->state_data, entity->state_data_size);
				read_success = true;
			}

			if (read_success)
			{
				read_success = entity_definition->entity_update_decode(
					false,
					&update_mask,
					entity->state_data_size,
					state_data,
					packet
				);
			}

			if (read_success)
			{
				s_simulation_queue_entity_data sim_queue_entity_data;
				sim_queue_entity_data.entity_index = entity_index;
				sim_queue_entity_data.entity_type = entity->entity_type;
				sim_queue_entity_data.creation_data_size = entity->creation_data_size;
				sim_queue_entity_data.creation_data = (uint8*)entity->creation_data;
				sim_queue_entity_data.state_data_size = entity->state_data_size;
				sim_queue_entity_data.state_data = state_data;

				if (!packet->read_only_for_consistency()
					&& !simulation_queue_entity_update_allocate(&sim_queue_entity_data, NONE, update_mask, (s_simulation_queue_element**)simulation_queue_element))
				{
					read_success = false;
				}
			}

			if (read_success)
			{
				ASSERT(out_update_mask);
				ASSERT(block_count);
				ASSERT(blocks);

				*out_update_mask = update_mask;

				ASSERT(*block_count + 2 <= maximum_block_count);


				blocks[*block_count + _entity_update_block_order_simulation_entity_state].block_type = _network_memory_block_simulation_entity_state;
				blocks[*block_count + _entity_update_block_order_simulation_entity_state].block_size = (int16)entity->state_data_size;
				blocks[*block_count + _entity_update_block_order_simulation_entity_state].block_data = state_data;

				blocks[*block_count + _entity_update_block_order_forward_memory_queue_element].block_type = _network_memory_block_forward_simulation_queue_element;
				blocks[*block_count + _entity_update_block_order_forward_memory_queue_element].block_size = sizeof(s_simulation_queue_element*);
				blocks[*block_count + _entity_update_block_order_forward_memory_queue_element].block_data = simulation_queue_element;

				*block_count += k_entity_update_block_order_count;

				result = 0;
			}
		}

		if (result == 3 || result == 2)
		{
			if (state_data != NULL)
			{
				network_heap_free_block(state_data);
			}

			if (simulation_queue_element)
			{
				if (*simulation_queue_element != NULL)
				{
					simulation_get_world()->simulation_queue_free(*(s_simulation_queue_element**)simulation_queue_element);
				}

				network_heap_free_block(simulation_queue_element);
			}
		}
	}
	else
	{
		event(_event_warning, "networking:simulation:entity:read_update_from_packet: invalid entity 0x%08X", entity_index);
	}

	return result;
}

void c_simulation_entity_database::notify_mark_entity_for_deletion(int32 entity_index)
{
	this->entity_delete_gameworld(entity_index);
}

void c_simulation_entity_database::entity_delete_gameworld(int32 entity_index)
{
	s_simulation_game_entity* entity = entity_get(entity_index);
	if (entity->object_index != NONE)
	{
		//c_simulation_entity_definition* entity_definition = m_type_collection->get_entity_definition(entity->entity_type);
		simulation_queue_entity_deletion_insert(entity);
	}

	entity->object_index = NONE;
	entity->exists_in_gameworld = false;
	entity->entity_update_flag = 0;
	entity->field_10 = 0;
	return;
}

bool c_simulation_entity_database::notify_promote_to_authority(int32 entity_index)
{
	s_simulation_game_entity* entity = this->entity_get(entity_index);
	simulation_queue_entity_promotion_insert(entity);
	return true;
}

void c_simulation_entity_database::entity_delete_internal(int32 entity_index)
{
	s_simulation_game_entity* entity = this->entity_get(entity_index);
	// Discard creation data
	if (entity->creation_data)
	{
		network_heap_free_block((uint8*)entity->creation_data);
		entity->creation_data = NULL;
		entity->creation_data_size = 0;
	}

	// Discard state data
	if (entity->state_data)
	{
		network_heap_free_block((uint8*)entity->state_data);
		entity->state_data = NULL;
		entity->state_data_size = 0;
	}

	// Clear entity data
	entity->entity_type = _simulation_entity_type_none;
	entity->entity_index = NONE;
	return;
}

void c_simulation_entity_database::reset(void)
{
	m_field_5 = true;
	if (!m_initialized)
	{
		csmemset(m_entity_data, 0, sizeof(m_entity_data));
	}

	for (uint32 i = 0; i < k_simulation_entity_database_maximum_entities; i++)
	{
		s_simulation_game_entity* entity = &m_entity_data[i];
		if (m_initialized)
		{
			if (entity->entity_index != NONE)
			{
				this->entity_delete_internal(entity->entity_index);
			}
		}
		else
		{
			entity->entity_index = NONE;
			entity->entity_type = _simulation_entity_type_none;
		}
	}
	m_field_5 = false;
	return;
}

void c_simulation_entity_database::entity_capture_creation_data(int32 entity_index)
{
	s_simulation_game_entity* entity = this->entity_get(entity_index);
	c_simulation_entity_definition* entity_definition = m_type_collection->get_entity_definition(entity->entity_type);

	entity_definition->build_creation_data(entity, entity->creation_data_size, entity->creation_data);
	entity_definition->build_baseline_state_data(entity->creation_data_size, entity->creation_data, entity->state_data_size, entity->state_data);
	entity->exists_in_gameworld = true;
	
	int32 count = entity_definition->update_flag_count();
	entity->field_10 = 0;
	entity->entity_update_flag = FLAG(count) - 1;
	return;
}
