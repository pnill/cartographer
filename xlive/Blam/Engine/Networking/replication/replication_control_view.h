#pragma once
#include "game/players.h"
#include "Networking/player_prediction.h"

enum e_network_memory_block : int16
{
	_network_memory_block_message_outgoing = 0x0,
	_network_memory_block_message_incoming = 0x1,
	_network_memory_block_replication_entity_status = 0x2,
	_network_memory_block_replication_entity_update = 0x3,
	_network_memory_block_replication_entity_packet = 0x4,
	_network_memory_block_replication_event_header = 0x5,
	_network_memory_block_replication_event_payload = 0x6,
	_network_memory_block_replication_event_record = 0x7,
	_network_memory_block_replication_event_payload2 = 0x8,
	_network_memory_block_replication_control_motion = 0x9,
	_network_memory_block_replication_control_prediction = 0xA,
	_network_memory_block_simulation_synchronous_client = 0xB,
	_network_memory_block_simulation_entity_creation = 0xC,
	_network_memory_block_simulation_entity_state = 0xD,
	_network_memory_block_simulation_event = 0xE,
	_network_memory_block_logic_session_array = 0xF,
	_network_memory_block_logic_unsuitable_session_array = 0x10,
	_network_memory_block_join_request = 0x11,

	// these are new
	_network_memory_block_forward_gamestate_element,
	_network_memory_block_forward_simulation_queue_element,

	k_network_memory_block_count //= 0x12,
};

struct s_replication_allocation_block
{
	int16 block_size;
	e_network_memory_block block_type;
	void* block_data;
};
ASSERT_STRUCT_SIZE(s_replication_allocation_block, 8);

struct s_replication_control_request
{
	int32 unknown_count;
	int32 control_index;
	int8 gap_8[52];
	int32 block_count;
	s_replication_allocation_block blocks[2];
	int8 gap_50[48];
};

class c_replication_control_view
{
	int8 gap_0[4];
	int32 field_4;
	int8 gap_8[4];
	int32 unk_vtable;
	int32 motion_flags;
	int32 field_14;
	int8 gap_18[1792];
	int32 player_prediction_flags;
	int32 field_71C;
	s_player_prediction player_prediction[k_maximum_players];
};
ASSERT_STRUCT_SIZE(c_replication_control_view, 0xB20);
