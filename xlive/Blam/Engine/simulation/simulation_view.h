#pragma once

#include "machine_id.h"
#include "networking/delivery/network_channel.h"
#include "networking/replication/replication_event_manager_view.h"
#include "networking/replication/replication_game_results.h"
#include "networking/replication/replication_scheduler.h"

/* enums */

enum e_simulation_view_type : __int16
{
	_simulation_view_type_none = 0x0,
	_simulation_view_type_synchronous_to_remote_authority = 0x1,
	_simulation_view_type_synchronous_to_remote_client = 0x2,
	_simulation_view_type_distributed_to_remote_authority = 0x3,
	_simulation_view_type_distributed_to_remote_client = 0x4,
	k_simulation_view_type_count = 0x5,
};

enum e_simulation_view_death_reason
{
	_simulation_view_death_reason_none = 0x0,
	_simulation_view_death_reason_disconnected = 0x1,
	_simulation_view_death_reason_out_of_sync = 0x2,
	_simulation_view_death_reason_failed_to_join = 0x3,
	_simulation_view_death_reason_blocking = 0x4,
	_simulation_view_death_reason_catchup_fail = 0x5,
	_simulation_view_death_reason_ended = 0x6,
	_simulation_view_death_reason_mode_error = 0x7,
	_simulation_view_death_reason_player_error = 0x8,
	_simulation_view_death_reason_replication_entity = 0x9,
	_simulation_view_death_reason_replication_event = 0xA,
	_simulation_view_death_reason_replication_game_results = 0xB,
	k_simulation_view_reason_count = 0xC,
};

/* structures */

class c_simulation_view
{
public:
	int16 m_identifier;
	e_simulation_view_type m_view_type;
	datum m_view_datum_index;
	class c_simulation_distributed_view* m_distributed_view;
	class c_simulation_world* m_world;
	uint32 m_world_view_index;
	s_machine_identifier m_remote_machine_identifier;
	int32 m_remote_machine_index;
	c_network_observer* m_observer;
	uint32 m_observer_channel_index;
	e_simulation_view_death_reason m_view_death_reason;
	int32 m_view_establishment_mode;
	int32 m_view_establishment_identifier;
	int32 m_remote_establishment_mode;
	uint32 m_remote_establishment_identifier;
	int32 m_channel_index;
	uint32 m_channel_connection_identifier;
	c_network_channel_simulation_interface m_channel_interface;
	bool m_simulation_active;
	uint32 m_simulation_player_acknowledged_mask;
	uint32 m_synchronous_received_action_number;
	uint32 m_synchronous_acknowledged_update_number;
	bool m_synchronous_client_blocked;
	uint32 m_synchronous_client_block_timestamp;
	int32 m_synchronous_catchup_attempt_count;
	void* m_synchronous_catchup_buffer;
	int32 m_synchronous_catchup_buffer_size;
	int32 m_synchronous_catchup_stream_capacity;
	int32 m_synchronous_catchup_stream_data;
	int32 m_synchronous_catchup_stream_read_offset;
	int32 m_synchronous_catchup_stream_used;
	int32 m_synchronous_catchup_buffer_offset;
	int32 m_synchronous_next_action_number;
};
ASSERT_STRUCT_SIZE(c_simulation_view, 0xB4);

class c_simulation_distributed_view
{
private:
	int16 identifier;
	c_replication_scheduler m_replication_scheduler;
	c_replication_entity_manager_view m_entity_view;
	c_replication_event_manager_view m_event_view;
	c_replication_control_view m_control_view;
	c_simulation_view_telemetry_provider m_telemetry_provider;
	c_game_results_replicator m_game_results_replicator;
};
ASSERT_STRUCT_SIZE(c_simulation_distributed_view, 0xAC3C);
