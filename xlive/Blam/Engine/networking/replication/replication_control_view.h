#pragma once
#include "replication_scheduler.h"
#include "game/player_constants.h"
#include "networking/player_motion.h"
#include "networking/player_prediction.h"

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

class c_replication_control_view : c_replication_scheduler_client
{
private:
	bool m_initialized;
	char _pad09[3];
	void* m_telemetry_provider;
	uint32 m_motion_available_send;
	uint32 m_motion_available_receive;
	s_player_motion m_motion[32];
	uint32 m_motion_timestamp[32];
	uint32 m_prediction_available_send;
	uint32 m_prediction_available_receive;
	s_prediction m_prediction[32];

public:
	bool has_data_to_transmit() override;
	bool build_outgoing_requests(const s_simulation_view_telemetry_data* telemetry_data, int32 maximum_number_of_requests, void* requests) override;
	int32 terminator_required_bits() override;
	void write_to_packet(void* request_identifier, int32 request_type, void* telemetry_data, int32 packet_sequence_number, c_bitstream* packet, int32 must_leave_space_bits) override;
	void write_terminator_to_packet(c_bitstream* packet) override;
	int32 read_from_packet(c_bitstream* packet, int32 maximum_number_of_requests, void* requests, int32* out_number_of_requests) override;
	void process_incoming_request(void* request) override;
	void notify_packet_acknowledged() override;
	void mark_packet_delivered(bool delivered) override;
};
ASSERT_STRUCT_SIZE(c_replication_control_view, 0xF20);
