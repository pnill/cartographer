#pragma once
#include "replication_scheduler.h"

/* structures */

struct s_replication_event_manager_view_statistics
{
	uint32 events_sent;
	uint32 events_pending;
	uint32 events_in_transit;
};
ASSERT_STRUCT_SIZE(s_replication_event_manager_view_statistics, 12);

class c_replication_event_manager_view : c_replication_scheduler_client
{
private:
	bool m_initialized;
	bool m_fatal_error;
	uint32 m_view_index;
	class c_replication_entity_packet_record* m_packet_list; // c_replication_entity_packet_record
	uint32 m_packet_list_length;
	void* m_event_manager;
	s_replication_event_manager_view_statistics m_statistics;

public:
	bool has_data_to_transmit() override;
	bool build_outgoing_requests(const s_simulation_view_telemetry_data* telemetry_data, int32 maximum_number_of_requests, void* requests) override;
	int32 terminator_required_bits() override;
	void write_to_packet(void* request_identifier, int32 request_type, void* telemetry_data, int32 packet_sequence_number, c_bitstream* packet, int32 must_leave_space_bits) override;
	void write_terminator_to_packet(c_bitstream* packet) override;
	int32 read_from_packet(c_bitstream* packet, int32 maximum_number_of_requests, void* requests,int32* out_number_of_requests) override;
	void process_incoming_request(void* request) override;
	void notify_packet_acknowledged() override;
	void mark_packet_delivered(bool delivered) override;
};
ASSERT_STRUCT_SIZE(c_replication_event_manager_view, 40);