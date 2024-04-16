#pragma once
#include "simulation_entity_database.h"

#include "simulation/game_interface/simulation_game_entities.h"
#include "Networking/replication/replication_control_view.h"

#define k_entity_reference_indices_count_max 2

class c_simulation_event_handler;

struct c_simulation_event_handler_vtbl
{
    int(__thiscall* read_incoming_event)(c_simulation_event_handler* thisx, int a2, int a3, int a4, uint32* a5, int a6, int a7);
    void(__thiscall* process_incoming_event)(c_simulation_event_handler* thisx, e_simulation_event_type simulation_event_type, int32* entity_reference_indices, int32 block_count, s_replication_allocation_block* block);
    void(__thiscall* write_outgoing_event)(c_simulation_event_handler* thisx, int a2, int a3, uint32* a4, char* a5);
    void(__thiscall* notify_outgoing_event_retired)(c_simulation_event_handler* thisx, int a2);
    char(__thiscall* calculate_requirements)(c_simulation_event_handler* thisx, int a2, uint32** a3, float* a4, uint32* a5);
    int(__thiscall* write_description_to_string)(c_simulation_event_handler* thisx, int a2, int a3, int a4, int a5);

    static c_simulation_event_handler_vtbl* get()
    {
        return Memory::GetAddress<c_simulation_event_handler_vtbl*>(0x3C6268, 0x381D50);
    }
};
ASSERT_STRUCT_SIZE(c_simulation_event_handler_vtbl, 6 * sizeof(void*));

class c_simulation_event_handler
{
public:
    c_simulation_event_handler_vtbl* get_vtbl()
    {
        return c_simulation_event_handler_vtbl::get();
    };

    void reset()
    {
    }

private:
    c_simulation_event_handler_vtbl* vtbl;
    bool field_4;
    int8 pad[3];
    int32 field_8;
    int32 field_C;
    c_simulation_type_collection* m_event_type_collection;
    c_simulation_entity_database* m_entity_database;

    void process_incoming_event(e_simulation_event_type simulation_event_type, int32* entity_reference_indices, int32 block_count, s_replication_allocation_block* block);
};

void simulation_event_handler_apply_patches();