#pragma once

#include "..\NetworkObserver\NetworkObserver.h"

struct __declspec(align(8)) network_channel
{
	enum e_channel_closure_reason
	{
		link_destroyed = 1,
		channel_deleted = 3,
		connect_reinitiate = 6,
		channel_establishment_timeout = 7,
		destination_unreachable = 9,
		remote_closure = 10,
		connection_overflows = 11,
		observer_discard = 14,
		observer_reset = 17,
	};

	enum e_channel_state
	{
		unk_state_5 = 5,
	};

	void* network_link;
	void* network_message_gateway;
	void* network_message_handler;
	void* network_channel_config;
	int network_connection_index;
	int network_shared_globals_index;
	char gap_18[4];
	bool field_1C;
	BYTE field_1D;
	BYTE gap_1E[2];
	DWORD field_20;
	int field_24;
	BYTE gap_28[8];
	DWORD field_30;
	BYTE gap_34[4];
	BYTE gap_38[4];
	int field_3C;
	network_observer *network_observer;
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
	DWORD field_88;
	DWORD field_8C;
	DWORD field_90;
	DWORD unk_time;
	DWORD field_98;
	BYTE gap_9C[4];
	LARGE_INTEGER field_A0;
	BYTE gap_A8[48];
	DWORD field_D8;
	BYTE gap_DC[4];
	LARGE_INTEGER field_E0;
	char gap[10];

	static network_channel* getNetworkChannel(int channelIndex);
	bool getNetworkAddressFromNetworkChannel(network_address* out_addr);
};
static_assert(sizeof(network_channel) == 248, "network_channel size != 248");

