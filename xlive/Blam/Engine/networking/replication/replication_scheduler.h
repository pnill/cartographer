#pragma once
#include "networking/delivery/network_channel.h"
#include "simulation/simulation_view_telemetry.h"

/* structures */

class c_replication_scheduler_client
{
private:
	int32 m_client_index;

public:
	virtual bool has_data_to_transmit() = 0;
	virtual bool build_outgoing_requests(const s_simulation_view_telemetry_data* telemetry_data, int32 maximum_number_of_requests, void* requests) = 0;
	virtual int32 terminator_required_bits() = 0;
	virtual void write_to_packet(void* request_identifier, int32 request_type, void* telemetry_data, int32 packet_sequence_number, c_bitstream* packet, int32 must_leave_space_bits) = 0;
	virtual void write_terminator_to_packet(c_bitstream* packet) = 0;
	virtual int32 read_from_packet(c_bitstream* packet, int32 maximum_number_of_requests, void* requests, int32* out_number_of_requests) = 0;
	virtual void process_incoming_request(void* request) = 0;
	virtual void notify_packet_acknowledged() = 0;
	virtual void mark_packet_delivered(bool delivered) = 0;
};

class c_replication_scheduler : c_network_channel_client
{
private:
	bool m_initialized;
	int32 m_view_index;
	c_replication_scheduler_client* m_clients[3];
	int32 m_client_terminator_bits[3];
	int32 m_total_terminator_bits;
	c_simulation_view_telemetry_provider* m_telemetry_provider;

public:
	c_replication_scheduler() = default;

	const char* get_client_name() override;
	bool connection_lost(e_network_channel_closure_reason* channel_closure_reason) override;
	bool has_data_to_transmit(bool* data_requires_acknowledgement) override;
	int32 space_required_bits(int32 packet_size_bits, int32 packet_remaining_bits) override;
	bool write_data_to_packet(int32 a1, c_bitstream* bitstream_1, c_bitstream* bitstream_2, int32 a4,
		int32 a5) override;
	bool read_from_packet(int32* packet_sequence_number, c_bitstream* packet, int32 unused) override;
	void notify_packet_acknowledged(int32 outgoing_packet_sequence_number) override;
	void notify_packet_retired(int32 outgoing_packet_sequence_number, bool delivered) override;
};
ASSERT_STRUCT_SIZE(c_replication_scheduler, 44);