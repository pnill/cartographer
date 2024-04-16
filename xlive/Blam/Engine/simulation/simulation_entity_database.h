#pragma once
#include "simulation_type_collection.h"
#include "Networking/replication/replication_entity_manager.h"

#define ENTITY_INDEX_TO_ABSOLUTE_INDEX(_entity_index)  ((_entity_index) & (k_simulation_entity_database_maximum_entities - 1))
#define ENTITY_INDEX_TO_SEED(_entity_index) ((_entity_index) >> 28)

enum e_entity_creation_block_order
{
	_entity_creation_block_order_simulation_entity_creation,
	_entity_creation_block_order_simulation_entity_state,
	_entity_creation_block_order_forward_memory_queue_element,
	// _entity_creation_block_order_gamestate_index,
	k_entity_creation_block_order_count
};

enum e_entity_update_block_order
{
	_entity_update_block_order_simulation_entity_state,
	_entity_update_block_order_forward_memory_queue_element,
	k_entity_update_block_order_count
};

// TODO reverse parent class: c_replication_entity_manager
class c_simulation_entity_database : public c_replication_entity_manager_client
{
public:
	virtual bool write_creation_to_packet(int32 entity_index, uint32 update_mask, const void* telemetry_data, c_bitstream* packet, int32 required_leave_space_bits, uint32* out_update_mask);
	virtual uint32 read_creation_from_packet(int32 entity_index, e_simulation_entity_type* simulation_entity_type, uint32* out_update_mask, int32 block_max_count, int32* block_count, s_replication_allocation_block* a7, c_bitstream* packet);
	virtual bool process_creation(int32 entity_index, e_simulation_entity_type type, uint32 update_mask, int32 block_count, s_replication_allocation_block* blocks);
	virtual void calculate_creation_requirements(int32 entity_index, uint32 update_mask, const void* a4, real32* a5, int32* a6);
	virtual void write_creation_description_to_string(int32 entity_index, void* telemetry_data, int32 buffer_size, char* buffer);
	virtual bool write_update_to_packet(int32 entity_index, uint32 update_mask, void* telemetry_data, c_bitstream* packet, int32 required_leave_space_bits, uint32* update_mask_written);
	virtual int32 read_update_from_packet(int32 entity_index, uint32* out_update_mask, int32 maximum_block_count, int32* block_count, s_replication_allocation_block* blocks, c_bitstream* packet);
	virtual bool process_update(int32 entity_index, uint32 update_mask, int32 block_count, s_replication_allocation_block* blocks);
	virtual void calculate_update_requirements(int32 entity_index, uint32 a3, uint32 a4, void* a5, real32* priority, uint32* a7);
	virtual void calculate_deletion_requirements(int32 entity_index, int32 a3, real32* requirements);
	virtual void notify_mark_entity_for_deletion(int32 entity_index);
	virtual void notify_entity_collision(int32 old_entity_index, int32 entity_index, e_simulation_entity_type entity_type, const s_replication_allocation_block* blocks);
	virtual void notify_delete_entity(int32 entity_index);
	virtual bool notify_promote_to_authority(int32 entity_index);
	virtual void rotate_entity_seed(void);
	virtual uint32 generate_current_entity_update_mask(int32 entity_index);


	void entity_capture_creation_data(int32 entity_index);

	void entity_delete_internal(int32 entity_index);
	void reset(void);

	s_simulation_game_entity* entity_get(int32 entity_index)
	{
		return &m_entity_data[ENTITY_INDEX_TO_ABSOLUTE_INDEX(entity_index)];
	}

	s_simulation_game_entity* entity_try_and_get(int32 entity_index)
	{
		if (entity_index != NONE)
		{
			if (entity_get(entity_index)->entity_index == entity_index)
			{
				return entity_get(entity_index);
			}
		}

		return NULL;
	}

private:
	c_simulation_type_collection* m_type_collection;
	s_simulation_game_entity m_entity_data[k_simulation_entity_database_maximum_entities];

	void entity_delete_gameworld(int32 entity_index);
};
ASSERT_STRUCT_SIZE(c_simulation_entity_database, 36884);

c_simulation_entity_database* simulation_get_entity_database();

void simulation_entity_database_apply_patches(void);
