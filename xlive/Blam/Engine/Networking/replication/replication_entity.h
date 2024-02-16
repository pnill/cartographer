#pragma once
#include "memory/bitstream.h"

enum e_replication_entity_flags
{
	replication_entity_allocated_flag = 0x1,
	replication_entity_marked_for_deletion_flag = 0x2,
	replication_entity_local_flag = 0x4,
	replication_entity_collection_master_flag = 0x8,
	replication_entity_collection_slave_flag = 0x10,
};

enum e_replication_entity_view_code
{
	replication_entity_view_code_create_entity = 0x1,
	replication_entity_view_code_delete_entity = 0x2,
	replication_entity_view_code_create_entity_collection = 0x3,
	replication_entity_view_code_delete_entity_collection = 0x4,
	replication_entity_view_code_update_entity = 0x5,
};

enum e_replication_entity_view_state
{
	_replication_entity_view_state_none = 0x0,
	_replication_entity_view_state_ready = 0x1,
	_replication_entity_view_state_replicating = 0x2,
	_replication_entity_view_state_active = 0x3,
	_replication_entity_view_state_deleting = 0x4,
	k_replication_entity_view_state_count = 0x5,
};

// Get the replication entity index
void replication_entity_index_decode(c_bitstream* bitstream, int32* replication_entity_index);

// Get the replication entity index as well as the entity abs index
void replication_entity_index_decode_get_abs_entity_index(c_bitstream* bitstream, int32* replication_entity_index, uint32* entity_abs_index);

