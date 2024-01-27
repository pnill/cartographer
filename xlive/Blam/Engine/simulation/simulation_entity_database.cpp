#include "stdafx.h"
#include "simulation_entity_database.h"

#include "simulation_queue_entities.h"

#include "Util/Hooks/Hook.h"

c_simulation_entity_database* simulation_get_entity_database()
{
    return (c_simulation_entity_database*)((uint8*)simulation_get_world()->m_distributed_world + 8352);
}

bool c_simulation_entity_database::process_creation(int32 entity_index, e_simulation_entity_type type, uint32 update_mask, int32 block_count, s_replication_allocation_block* blocks)
{
    bool result = false;
    c_simulation_entity_definition* entity_definition = m_type_collection->get_entity_definition(type);
    s_simulation_game_entity* game_entity = get_entity(entity_index);
    game_entity->entity_index = entity_index;
    game_entity->entity_type = type;
    game_entity->entity_update_flag = 0;
    game_entity->field_10 = 0;
    game_entity->event_reference_count = 0;
    game_entity->exists_in_gameworld = false;
    game_entity->object_index = DATUM_INDEX_NONE;

    // we could also validate here the type of the blocks
    game_entity->creation_data = blocks[_entity_creation_block_order_simulation_entity_creation].block_data;
    game_entity->creation_data_size = blocks[_entity_creation_block_order_simulation_entity_creation].block_size;
    game_entity->state_data = blocks[_entity_creation_block_order_simulation_entity_state].block_data;
    game_entity->state_data_size = blocks[_entity_creation_block_order_simulation_entity_state].block_size;

	s_simulation_queue_element* queue_element = *(s_simulation_queue_element**)blocks[_entity_creation_block_order_forward_memory_queue_element].block_data;

    csmemset(blocks, 0, sizeof(s_replication_allocation_block) * block_count);

	// TODO when we implement this
	// datum gamestate_index = *(datum*)blocks[_entity_creation_block_order_gamestate_index].block_data;

    // free these blocks
	network_heap_free_block((uint8*)blocks[_entity_creation_block_order_forward_memory_queue_element].block_data);
	// network_heap_free_block((uint8*)blocks[_entity_creation_block_order_gamestate_index].block_data);

	if (queue_element)
	{
		// insert the queue element to the buffer previously created in read_creation_from_packet()
		// and passed by a replication allocation block
        simulation_queue_entity_creation_insert(queue_element);
        result = true;
    }
    else
    {
        result = entity_definition->create_game_entity(
            game_entity, 
            game_entity->creation_data_size, 
            game_entity->creation_data, 
            update_mask, 
            game_entity->state_data_size, 
            game_entity->state_data);
    }

    game_entity->exists_in_gameworld = result;
    return result;
}

__declspec(naked) void jmp_c_simulation_entity_database__process_creation() { __asm { jmp c_simulation_entity_database::process_creation } }


uint32 c_simulation_entity_database::read_creation_from_packet(int32 entity_index, e_simulation_entity_type* simulation_entity_type, uint32* out_update_mask, int32 block_max_count, int32* block_count, s_replication_allocation_block* blocks, c_bitstream* packet)
{
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
        }

        // Allocate state data
        uint8* state_data = network_heap_allocate_block(state_data_size);

        // Allocate gamestate data
        //int32* gamestate_index = (int32*)network_heap_allocate_block(sizeof(int32));

        // Allocate queue data
        s_simulation_queue_element** queue_element = (s_simulation_queue_element**)network_heap_allocate_block(sizeof(s_simulation_queue_element*));
        result = (!creation_data || !queue_element || !state_data /*|| !gamestate_index*/ ? 2 : result);

        // check if creation size is > 0 and if network heap block have been successfully allocated
        if ((!creation_data_size || creation_data != NULL) && (state_data && queue_element /* && gamestate_index*/))
        {
            if (creation_data_size > 0)
                csmemset(creation_data, 0, creation_data_size);

            if (entity_definition->object_creation_decode(creation_data_size, creation_data, packet))
            {
                if (entity_definition->build_baseline_state_data(
                    creation_data_size,
                    creation_data,
                    state_data_size,
                    (s_simulation_baseline_state_data*)state_data))
                {
                    uint32 update_mask = entity_definition->initial_update_mask();
                    uint32 entity_out_update_mask = 0;

                    bool decode_success = true;
                    if (update_mask
                        && packet->read_bool("initial-update-exists")
                        )
                    {
                        if (entity_definition->entity_update_decode(true, &entity_out_update_mask, state_data_size, state_data, packet))
                        {
                            // check if all updates have been decoded?
                            if (TEST_FLAG(~update_mask, entity_out_update_mask))
                            {
                                // if not, the block is invalid?
                                decode_success = false;
                            }
                        }
                        else
                        {
                            decode_success = false;
                        }
                    }

                    if (decode_success)
                    {
                        /* Original code
                        *simulation_entity_type = entity_type;
                        *out_update_mask = entity_out_update_mask;
                        blocks[*block_count].block_type = _network_memory_block_simulation_entity_creation;
                        blocks[*block_count].block_size = creation_data_size;
                        blocks[*block_count].block_data = creation_data;
                        blocks[*block_count + 1].block_type = _network_memory_block_simulation_entity_state;
                        blocks[*block_count + 1].block_size = state_data_size;
                        blocks[*block_count + 1].block_data = state_data;
                        *block_count += 2;
                        */

                        s_simulation_queue_entity_data simulation_queue_entity_type;
                        simulation_queue_entity_type.entity_index = entity_index;
                        simulation_queue_entity_type.entity_type = entity_type;
                        simulation_queue_entity_type.creation_data_size = creation_data_size;
                        simulation_queue_entity_type.creation_data = creation_data;
                        simulation_queue_entity_type.state_data_size = state_data_size;
                        simulation_queue_entity_type.state_data = state_data;

                        if (!packet->read_only_for_consistency()
                            && !simulation_queue_entity_creation_allocate(&simulation_queue_entity_type, entity_out_update_mask, queue_element, NULL))
                        {
                            decode_success = false;
                        }
                    }

                    if (decode_success)
                    {
                        *simulation_entity_type = entity_type;
                        *out_update_mask = entity_out_update_mask;

                        //blocks[_entity_creation_block_order_gamestate_index].block_type = _network_memory_block_forward_gamestate_element;
                        //blocks[_entity_creation_block_order_gamestate_index].block_size = sizeof(gamestate_index);
                        //blocks[_entity_creation_block_order_gamestate_index].block_data = gamestate_index;

                        // copy the block, allow the process function to use this
                        blocks[*block_count + _entity_creation_block_order_simulation_entity_creation].block_type = _network_memory_block_simulation_entity_creation;
                        blocks[*block_count + _entity_creation_block_order_simulation_entity_creation].block_size = creation_data_size;
                        blocks[*block_count + _entity_creation_block_order_simulation_entity_creation].block_data = creation_data;

                        blocks[*block_count + _entity_creation_block_order_simulation_entity_state].block_type = _network_memory_block_simulation_entity_state;
                        blocks[*block_count + _entity_creation_block_order_simulation_entity_state].block_size = state_data_size;
                        blocks[*block_count + _entity_creation_block_order_simulation_entity_state].block_data = state_data;

                        blocks[*block_count + _entity_creation_block_order_forward_memory_queue_element].block_type = _network_memory_block_forward_simulation_queue_element;
                        blocks[*block_count + _entity_creation_block_order_forward_memory_queue_element].block_size = sizeof(queue_element);
                        blocks[*block_count + _entity_creation_block_order_forward_memory_queue_element].block_data = queue_element;

                        *block_count += k_entity_creation_block_order_count;

                        result = 0;
                    }
                }
                else
                {
                    result = 3;
                }
            }
        }
        else
        {
            if (creation_data != NULL)
            {
                network_heap_free_block((uint8*)creation_data);
            }
            if (state_data != NULL)
            {
                network_heap_free_block((uint8*)state_data);
            }

            if (queue_element != NULL)
            {
                network_heap_free_block((uint8*)queue_element);
            }

            /*if (gamestate_index != NULL)
            {
                network_heap_free_block((uint8*)gamestate_index);
            }*/
        }
    }

    return result;
}

__declspec(naked) void jmp_c_simulation_entity_database__read_creation_from_packet() { __asm { jmp c_simulation_entity_database::read_creation_from_packet } }


void simulation_entity_database_apply_patches(void)
{
	WritePointer(Memory::GetAddress(0x3C6228, 0x381D10), jmp_c_simulation_entity_database__read_creation_from_packet);
	WritePointer(Memory::GetAddress(0x3C622C, 0x381D14), jmp_c_simulation_entity_database__process_creation);
	return;
}