#pragma once

#include "Simulation/simulation_type_collection.h"
#include "Simulation/game_interface/simulation_game_entities.h"

#define k_entity_reference_indices_count_max 2

class c_simulation_event_handler;

enum e_payload_block_type
{
    _payload_block_type_none,

};

struct s_payload_block
{
    int16 block_size;
    int16 block_type;
    uint8* block_data;
};

struct c_simulation_event_handler_vtbl
{
    int(__thiscall* read_incoming_event)(c_simulation_event_handler* thisx, int a2, int a3, int a4, uint32* a5, int a6, int a7);
    void(__thiscall* process_incoming_event)(c_simulation_event_handler* thisx, e_simulation_event_type simulation_event_type, int32* entity_reference_indices, int32 block_count, s_payload_block* block);
    void(__thiscall* write_outgoing_event)(c_simulation_event_handler* thisx, int a2, int a3, uint32* a4, char* a5);
    void(__thiscall* notify_outgoing_event_retired)(c_simulation_event_handler* thisx, int a2);
    char(__thiscall* calculate_requirements)(c_simulation_event_handler* thisx, int a2, uint32** a3, float* a4, uint32* a5);
    int(__thiscall* write_description_to_string)(c_simulation_event_handler* thisx, int a2, int a3, int a4, int a5);

    static c_simulation_event_handler_vtbl* get()
    {
        // ### OFFSET
        return Memory::GetAddress<c_simulation_event_handler_vtbl*>(0x0);
    }
};
CHECK_STRUCT_SIZE(c_simulation_event_handler_vtbl, 6 * sizeof(void*));

class c_simulation_event_handler
{
    c_simulation_event_handler_vtbl* get_vtbl()
    {
        // ### TODO OFFSET
        return Memory::GetAddress<c_simulation_event_handler_vtbl*>(0x0);
    };

    c_simulation_event_handler_vtbl* vtbl;
    uint8 gap_4[4];
    c_simulation_type_collection* m_event_type_collection;

    void process_incoming_event(e_simulation_event_type simulation_event_type, int32* entity_reference_indices, int32 block_count, s_payload_block* block);
};
