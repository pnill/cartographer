#pragma once
#include "game_interface/simulation_game_events.h"

class c_replication_event_manager_client
{
public:
    virtual int32 read_incoming_event(int32 a2, int32 a3, int32 a4, uint32* a5, int32 a6, int32 a7) = 0;
    virtual void process_incoming_event(e_simulation_event_type simulation_event_type, int32* entity_reference_indices, int32 block_count, struct s_replication_allocation_block* block) = 0;
    virtual void write_outgoing_event(int32 a2, int32 a3, uint32* a4, int8* a5) = 0;
    virtual void notify_outgoing_event_retired(int32 a2) = 0;
    virtual int8 calculate_requirements(int32 a2, uint32** a3, real32* a4, uint32* a5) = 0;
    virtual int32 write_description_to_string(int32 a2, int32 a3, int32 a4, int32 a5) = 0;
};

class c_simulation_event_handler : c_replication_event_manager_client
{
public:
    int32 read_incoming_event(int32 a2, int32 a3, int32 a4, uint32* a5, int32 a6, int32 a7) override;
    void process_incoming_event(e_simulation_event_type simulation_event_type, int32* entity_reference_indices, int32 block_count, struct s_replication_allocation_block* payload_block) override;
    void write_outgoing_event(int32 a2, int32 a3, uint32* a4, int8* a5) override;
    void notify_outgoing_event_retired(int32 a2) override;
    int8 calculate_requirements(int32 a2, uint32** a3, real32* a4, uint32* a5) override;
    int32 write_description_to_string(int32 a2, int32 a3, int32 a4, int32 a5) override;

	void reset()
    {
    }

private:
    bool m_initialized;
    class c_simulation_world* m_world;
    class c_replication_event_manager* m_event_manager;
    class c_simulation_type_collection* m_type_collection;
    class c_simulation_entity_database* m_entity_database;
};

/* prototypes */

void simulation_event_handler_apply_patches();