#include "stdafx.h"
#include "simulation_entity_database.h"

#include "simulation_queue_entities.h"

#include "Util/Hooks/Hook.h"

bool c_simulation_entity_database::process_creation(int32 entity_index, e_simulation_entity_type type, uint32 update_mask, int32 block_count, s_replication_allocation_block* blocks)
{
    bool result = false;
    c_simulation_entity_definition*  entity_definition = this->m_type_collection->get_entity_definition(type);
    s_simulation_game_entity* game_entity = &this->m_game_entities[entity_index & (k_simulation_entity_database_maximum_entities - 1)];
    game_entity->entity_index = entity_index;
    game_entity->entity_type = type;
    game_entity->entity_update_flag = 0;
    game_entity->field_10 = 0;
    game_entity->event_reference_count = 0;
    game_entity->exists_in_gameworld = 0;
    game_entity->object_index = NONE;
    game_entity->creation_data_size = blocks->block_size;
    game_entity->creation_data = blocks->block_data;
    game_entity->state_data_size = blocks[1].block_size;
    game_entity->state_data = blocks[1].block_data;
    csmemset(blocks, 0, sizeof(s_replication_allocation_block) * block_count);

    s_simulation_queue_element* simulation_queue_element = NULL;
    simulation_get_world()->simulation_queue_allocate(_simulation_queue_element_type_entity_creation, game_entity->creation_data_size, &simulation_queue_element);
    if (simulation_queue_element)
    {
        // copy the data to the buffer
        csmemcpy(simulation_queue_element->data, game_entity->creation_data, game_entity->creation_data_size);

        // copy it to the queue
        simulation_queue_entity_creation_insert(simulation_queue_element);
        result = true;
    }
    else
    {
        result = game_entity->exists_in_gameworld = entity_definition->create_game_entity(game_entity, blocks->block_size, blocks->block_data, update_mask, blocks[1].block_size, game_entity->state_data);
    }

    return result;
}

__declspec(naked) void jmp_c_simulation_entity_database__process_creation() { __asm { jmp c_simulation_entity_database::process_creation } }


uint32 c_simulation_entity_database::read_creation_from_packet(int32 entity_index, e_simulation_entity_type* simulation_entity_type, uint32* out_update_mask, int32 a5, uint32* block_count, s_replication_allocation_block* blocks, c_bitstream* packet)
{
    uint32 result = 3;
    e_simulation_entity_type entity_type = (e_simulation_entity_type)packet->read_integer("entity-type", 5);
    c_simulation_entity_definition* entity_definition = this->m_type_collection->get_entity_definition(entity_type);
    
    if (entity_definition)
    {
        uint32 creation_data_size = entity_definition->creation_data_size();
        uint32 state_data_size = entity_definition->state_data_size();


        // Allocate creation data
        uint8* creation_data = NULL;
        if (creation_data_size > 0)
        {
            creation_data = network_heap_allocate_block(creation_data_size);
            result = (!creation_data ? 2 : result);
        }

        // Allocate state data
        uint8* state_data = network_heap_allocate_block(state_data_size);
        result = (!state_data ? 2 : result);

        // Allocate queue data
        s_simulation_queue_element** queue_element = (s_simulation_queue_element**)network_heap_allocate_block(sizeof(s_simulation_queue_element*));
        result = (!queue_element ? 2 : result);

        // Allocate gamestate data
        int32* gamestate_index = (int32*)network_heap_allocate_block(sizeof(int32));
        result = (!gamestate_index ? 2 : result);

        if (!creation_data_size || (creation_data && state_data && queue_element && gamestate_index))
        {
            if (creation_data_size > 0)
            {
                csmemset(creation_data, 0, creation_data_size);
            }

            if (entity_definition->object_creation_decode(creation_data_size, creation_data, packet) 
                && entity_definition->build_baseline_state_data(
                    creation_data_size,
                    creation_data,
                    state_data_size,
                    state_data))
            {
                uint32 update_mask = entity_definition->initial_update_mask();
                uint32 entity_out_update_mask = 0;
                    
                bool encode_blocks = true;
                if (update_mask
                    && packet->read_bool("initial-update-exists")
                    && !entity_definition->entity_update_decode(true, &entity_out_update_mask, state_data_size, state_data, packet)
                    || TEST_FLAG(~update_mask, entity_out_update_mask))
                {
                    encode_blocks = false;
                }

                if (encode_blocks)
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

                    if (!packet->read_only_for_consistency() && !simulation_queue_entity_creation_allocate(&simulation_queue_entity_type, entity_out_update_mask, queue_element, gamestate_index))
                    {
                        encode_blocks = false;
                    }
                }

                if (encode_blocks)
                {
                    *simulation_entity_type = entity_type;
                    *out_update_mask = entity_out_update_mask;

                    blocks[*block_count].block_type = _network_memory_block_simulation_event;
                    blocks[*block_count].block_size = creation_data_size;
                    blocks[*block_count].block_data = creation_data;
                    blocks[*block_count + 1].block_type = _network_memory_block_logic_session_array;
                    blocks[*block_count + 1].block_size = state_data_size;
                    blocks[*block_count + 1].block_data = state_data;
                    blocks[*block_count + 2].block_type = _network_memory_block_simulation_entity_creation;
                    blocks[*block_count + 2].block_size = sizeof(gamestate_index);
                    blocks[*block_count + 2].block_data = gamestate_index;
                    blocks[*block_count + 3].block_type = _network_memory_block_simulation_synchronous_client;
                    blocks[*block_count + 3].block_size = sizeof(queue_element);
                    blocks[*block_count + 3].block_data = queue_element;
                    return 0;
                }
            }
            result = 3;
        }
        if (creation_data)
        {
            network_heap_free_block((uint8*)creation_data);
        }
        if (state_data)
        {
            network_heap_free_block((uint8*)state_data);
        }
    }

    return result;
}

__declspec(naked) void jmp_c_simulation_entity_database__read_creation_from_packet() { __asm { jmp c_simulation_entity_database::read_creation_from_packet } }


void simulation_entity_database_apply_patches(void)
{
    //WritePointer(Memory::GetAddress(0x3C6228, 0x381D10), jmp_c_simulation_entity_database__read_creation_from_packet);
    //WritePointer(Memory::GetAddress(0x3C622C, 0x381D14), jmp_c_simulation_entity_database__process_creation);
    return;
}