#pragma once
#include "replication_scheduler.h"


/* constants */

enum
{
	k_replication_entity_manager_view_max_entities = 1024
};

/* structures */

struct s_replication_entity_view_data
{
	  int32 entity_index;
	  uint32 update_mask;
	  int16 state;
	  int16 next_index;
	  uint32 update_timestamp;
	  uint16 flags;
};
ASSERT_STRUCT_SIZE(s_replication_entity_view_data, 20);

struct s_replication_entity_manager_view_statistics
{
	uint32 creations_sent;
	uint32 creations_pending;
	uint32 updates_sent;
	uint32 updates_pending;
	uint32 deletions_sent;
	uint32 deletions_pending;
};
ASSERT_STRUCT_SIZE(s_replication_entity_manager_view_statistics, 24);

class c_replication_entity_manager_view : c_replication_scheduler_client
{
private:
	bool m_initialized;
	bool m_replicating;
	bool m_fatal_error;
	int32 m_view_index;
	uint32 m_view_mask;
	class c_replication_entity_manager* m_entity_manager;
	class c_replication_entity_packet_record* m_packet_list;
	class c_replication_entity_packet_record* m_outgoing_packet;
	int32 m_replication_start_time;
	s_replication_entity_view_data m_entity_data[k_replication_entity_manager_view_max_entities];
	int32 m_current_absolute_index_position;
	s_replication_entity_manager_view_statistics m_statistics;

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

	void initialize(int32 world_view_index, class c_replication_entity_manager* entity_manager);
	void reset(void);
};
ASSERT_STRUCT_SIZE(c_replication_entity_manager_view, 20544);
