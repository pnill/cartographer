#pragma once

#include "transport.h"
#include "NetworkObserver.h"

struct alignas(8) s_network_channel
{
	enum e_channel_closure_reason
	{
		_closure_reason_no_reason_given = 0,
		_closure_reason_link_destroyed = 1,
		_closure_reason_link_refused_listen = 2,
		_closure_reason_channel_deleted = 3,
		_closure_reason_connect_timeout = 4,
		_closure_reason_connect_refused = 5,
		_closure_reason_connect_reinitiate = 6,
		_closure_reason_establish_timeout = 7,
		_closure_reason_address_change = 8,
		_closure_reason_destination_unreachable = 9,
		_closure_reason_remote_closure = 10,
		_closure_reason_connection_overflow = 11,
		_closure_reason_message_overflow = 12,
		_closure_reason_security_lost = 13,
		_closure_reason_observer_released = 14,
		_closure_reason_observer_refused = 15,
		_closure_reason_observer_timeout = 16,
		_closure_reason_observer_reset = 17,
	};

	enum e_channel_state
	{
		_channel_state_2 = 2,
		_channel_state_5 = 5,
	};

	void* network_link;
	void* network_message_gateway;
	void* network_message_handler;
	void* network_channel_config;
	int network_connection_index;
	int network_message_queue_index;
	DWORD c_network_channel_simulation_gatekeeper;
	bool simulation_data_available;
	BYTE field_1D;
	BYTE gap_1E[2];
	DWORD field_20;
	int field_24;
	DWORD field_28;
	DWORD field_2C;
	DWORD field_30;
	DWORD field_34;
	DWORD field_38;
	void* simulation_interface;
	c_network_observer* network_observer;
	int channel_index;
	DWORD channel_flags;
	DWORD remote_identifier;
	DWORD identifier;
	e_channel_state channel_state;
	e_channel_closure_reason field_58;
	network_address old_address;
	network_address address;
	char field_84;
	BYTE gap_85[3];
	DWORD network_connect_start_timepoint;
	DWORD field_8C;
	DWORD field_90;
	DWORD network_establish_start_timepoint;
	DWORD field_98;
	BYTE gap_9C[4];
	LARGE_INTEGER field_A0;
	BYTE gap_A8[48];
	DWORD field_D8;
	BYTE gap_DC[4];
	LARGE_INTEGER field_E0;
	char gap[10];

	static s_network_channel* get(int32 channel_index);
	bool get_network_address(network_address* address_out);

	bool has_simulation_interface() { return simulation_interface != NULL; }
	bool is_simulation_authority() { return has_simulation_interface() && *(bool*)((uint8*)simulation_interface + 48); }
};
ASSERT_STRUCT_SIZE(s_network_channel, 248);

