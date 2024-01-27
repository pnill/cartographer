#pragma once
#include "simulation.h"

#include "Networking/replication/replication_entity_manager.h"

#define k_simulation_entity_database_maximum_entities 1024

enum e_entity_creation_block_order
{
	_entity_creation_block_order_forward_memory_queue_element,
	// _entity_creation_block_order_gamestate_index,
	_entity_creation_block_order_simulation_entity_creation,
	_entity_creation_block_order_simulation_entity_state,
	k_entity_creation_block_order_count
};

// TODO reverse parent class: c_replication_entity_manager
class c_simulation_entity_database : c_replication_entity_manager_client
{
public:
	virtual bool write_creation_to_packet(int32 entity_index, uint32 update_mask, const void* telemetry_data, c_bitstream* packet, int32 required_leave_space_bits, uint32* out_update_mask) = 0;
	virtual uint32 read_creation_from_packet(int32 entity_index, e_simulation_entity_type* simulation_entity_type, uint32* out_update_mask, int32 a5, int32* block_count, s_replication_allocation_block* a7, c_bitstream* packet);
	virtual bool process_creation(int32 entity_index, e_simulation_entity_type type, uint32 update_mask, int32 block_count, s_replication_allocation_block* blocks);
	virtual void calculate_creation_requirements(int32 entity_index, uint32 update_mask, const void* a4, real32* a5, int32* a6) = 0;
	virtual void write_creation_description_to_string(int32 entity_index, void* telemetry_data, int32 buffer_size, char* buffer) = 0;
	virtual bool write_update_to_packet(int32 entity_index, uint32 update_mask, void* telemetry_data, c_bitstream* packet, int32 required_leave_space_bits, uint32* update_mask_written) = 0;
	virtual int32 read_update_from_packet(int32 entity_index, uint32* out_update_mask, int32 maximum_block_count, int32* block_count, s_replication_allocation_block* blocks, c_bitstream* packet) = 0;
	virtual bool process_update(int32 entity_index, uint32 update_mask, int32 block_count, s_replication_allocation_block* blocks) = 0;
	virtual void calculate_update_requirements(int32 entity_index, uint32 a3, uint32 a4, void* a5, real32* priority, uint32* a7) = 0;
	virtual void calculate_deletion_requirements(int32 entity_index, int32 a3, real32* requirements) = 0;
	virtual void notify_mark_entity_for_deletion(int32 entity_index) = 0;
	virtual void notify_entity_collision(int32 old_entity_index, int32 entity_index, e_simulation_entity_type entity_type, const s_replication_allocation_block* blocks) = 0;
	virtual void notify_delete_entity(int32 entity_index) = 0;
	virtual bool notify_promote_to_authority(int32 entity_index) = 0;
	virtual void rotate_entity_seed(void) = 0;
	virtual uint32 generate_current_entity_update_mask(int32 entity_index) = 0;

private:
	c_simulation_type_collection* m_type_collection;
	s_simulation_game_entity m_game_entities[k_simulation_entity_database_maximum_entities];
};
CHECK_STRUCT_SIZE(c_simulation_entity_database, 36884);

void simulation_entity_database_apply_patches(void);
