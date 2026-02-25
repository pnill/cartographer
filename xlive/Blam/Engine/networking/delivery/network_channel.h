#pragma once
#include "networking/transport/transport_address.h"

/* enums */

enum e_network_channel_closure_reason
{
	_network_channel_reason_none = 0,
	_network_channel_reason_link_destroyed = 1,
	_network_channel_reason_link_refused_listen = 2,
	_network_channel_reason_channel_deleted = 3,
	_network_channel_reason_connect_timeout = 4,
	_network_channel_reason_connect_refused = 5,
	_network_channel_reason_connect_reinitiate = 6,
	_network_channel_reason_establish_timeout = 7,
	_network_channel_reason_address_change = 8,
	_network_channel_reason_destination_unreachable = 9,
	_network_channel_reason_remote_closure = 10,
	_network_channel_reason_connection_overflow = 11,
	_network_channel_reason_message_overflow = 12,
	_network_channel_reason_security_lost = 13,
	_network_channel_reason_observer_released = 14,
	_network_channel_reason_observer_refused = 15,
	_network_channel_reason_observer_timeout = 16,
	_network_channel_reason_observer_reset = 17,

	k_network_channel_reason_count
};

enum e_network_channel_state
{
	_network_channel_state_2 = 2,
	_network_channel_state_5 = 5,
};

/* structures */

class c_network_channel_owner
{
public:
	virtual void desire_heartbeat() = 0;
	virtual int32 channel_get_state() = 0;
	virtual void attempt_channel_reconnection() = 0;
	virtual void notify_channel_connection() = 0;
	virtual void notify_channel_died() = 0;
	virtual void vtable_function_6() = 0;
};

class c_network_channel_client
{
public:
	virtual const char* get_client_name() = 0;
	virtual bool connection_lost(e_network_channel_closure_reason* channel_closure_reason) = 0;
	virtual bool has_data_to_transmit(bool* data_requires_acknowledgement) = 0;
	virtual int32 space_required_bits(int32 packet_size_bits, int32 packet_remaining_bits) = 0;
	virtual bool write_data_to_packet(int32 a1, class c_bitstream* bitstream_1, class c_bitstream* bitstream_2, int32 a4, int32 a5) = 0;
	virtual bool read_from_packet(int32* packet_sequence_number, class c_bitstream* packet, int32 unused) = 0;
	virtual void notify_packet_acknowledged(int32 outgoing_packet_sequence_number) = 0;
	virtual void notify_packet_retired(int32 outgoing_packet_sequence_number, bool delivered) = 0;
};

struct s_network_channel
{
	void* m_network_link;
	void* m_network_message_gateway;
	void* network_message_handler;
	void* network_channel_config;
	int32 network_connection_index;
	int32 network_message_queue_index;
	void* simulation_gatekeeper;
	bool simulation_data_available;
	int8 field_1D;
	int8 gap_1E[2];
	int32 field_20;
	int32 field_24;
	int32 field_28;
	int32 field_2C;
	int32 field_30;
	int32 field_34;
	int32 field_38;
	void* simulation_interface;
	class c_network_observer* network_observer;
	int32 channel_index;
	int32 channel_flags;
	int32 remote_identifier;
	int32 identifier;
	e_network_channel_state channel_state;
	e_network_channel_closure_reason closure_reason;
	transport_address old_address;
	transport_address address;
	int8 field_84;
	int8 gap_85[3];
	int32 network_connect_start_timepoint;
	int32 field_8C;
	int32 field_90;
	int32 network_establish_start_timepoint;
	int32 field_98;
	int8 gap_9C[4];
	int64 field_A0;
	int8 gap_A8[48];
	int32 field_D8;
	int8 gap_DC[4];
	int64 field_E0;
	int8 gap[10];

	static s_network_channel* get(int32 channel_index);
	bool get_network_address(transport_address* address_out);

	bool has_simulation_interface() const { return simulation_interface != NULL; }
	bool is_simulation_authority() const { return has_simulation_interface() && *(bool*)((uint8*)simulation_interface + 48); }
	bool is_channel_state_5() const { return channel_state == _network_channel_state_5; }
};
ASSERT_STRUCT_SIZE(s_network_channel, 248);

struct s_network_channel_client_info
{
	uint32 flags;
	c_network_channel_client* client;
};
ASSERT_STRUCT_SIZE(s_network_channel_client_info, 8);

class c_network_channel_simulation_interface
{
	bool m_initialized;
	void* m_simulation_context;
	void(__cdecl* m_simulation_closure_callback)(void*);
	int32 m_simulation_client_count;
	s_network_channel_client_info m_simulation_clients[4]; // k_number_of_users?
	bool m_simulation_is_authority;
	bool m_established;
};
ASSERT_STRUCT_SIZE(c_network_channel_simulation_interface, 52);