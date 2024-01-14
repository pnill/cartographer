#pragma once

#include "simulation/game_interface/simulation_game_engine_player.h"
#include "Blam/Engine/Networking/player_prediction.h"

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

	k_network_memory_block_count,
};

struct s_replication_allocation_block
{
	int16 block_size;
	e_network_memory_block block_type;
	uint8* block_data;
};
CHECK_STRUCT_SIZE(s_replication_allocation_block, 0x8);

struct s_replication_control_request
{
	DWORD unknown_count;
	unsigned int control_index;
	char gap_8[52];
	signed int block_count;
	s_replication_allocation_block blocks[2];
	char gap_50[48];
};

struct c_replication_control_view
{
	char gap_0[4];
	DWORD field_4;
	char gap_8[4];
	DWORD unk_vtable;
	DWORD motion_flags;
	DWORD field_14;
	char gap_18[1792];
	DWORD player_prediction_flags;
	DWORD field_71C;
	s_player_prediction player_prediction[k_maximum_multiplayer_players];
};
CHECK_STRUCT_SIZE(c_replication_control_view, 0xB20);
