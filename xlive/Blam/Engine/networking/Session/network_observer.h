#pragma once

// Network Observer
//	- manages network bandwidth based on network conditions

#include "networking/transport/transport_address.h"
#include "networking/transport/transport_security.h"
#include "networking/transport/transport_qos.h"
#include "networking/network_statistics.h"

// enables/disables LIVE netcode, so we can use the LIVE serverlist
// true  - LIVE network protocol enabled
// false - System Link network protocol enabled

#if !defined(_CARTOGRAPHER_DLL_CONF)
#define USE_LIVE_NETWORK_PROTOCOL true

#if defined(USE_LIVE_NETWORK_PROTOCOL)
#	if USE_LIVE_NETWORK_PROTOCOL == true
#		define LIVE_NETWORK_PROTOCOL_FORCE_CONSTANT_NETWORK_PARAMETERS true
#	endif
#endif
#endif

// defaults
#define k_online_netcode_client_rate_real 60.0f
#define k_online_netcode_server_rate_real 60.0f

#define k_online_netcode_rate_real 60.0f

/* constants */

enum
{
	k_network_channel_count = 16,
	k_network_channel_count_for_campaign = 4,
	k_network_preferences_size = 108,		// network heap size
	k_network_heap_size = 10485760,			// default: 1048576


	k_online_netcode_client_max_packet_size_bytes = 1264,
	k_online_netcode_server_max_packet_size_bytes = 1264,

	// size in bits
	k_online_netcode_client_max_bandwidth_per_channel = ((int32)k_online_netcode_client_rate_real * k_online_netcode_client_max_packet_size_bytes * 8),
	k_online_netcode_server_max_bandwidth_per_channel = ((int32)k_online_netcode_server_rate_real * k_online_netcode_server_max_packet_size_bytes * 8)
};

/* enums */

enum e_observer_channel_state
{
	_observer_channel_state_none = 0,
	_observer_channel_state_pending_transport_layer = 3, // waiting for xnet connection to be established
	_observer_channel_state_pending_game_layer = 4, // xnet layer connected, waiting for game to establish
	_observer_channel_state_connected = 7
};

enum e_network_observer_owner
{
	_network_observer_owner_squad_one = 0,
	_network_observer_owner_squad_two,
	_network_observer_owner_simulation,
	k_network_observer_owner_count,
	_network_observer_owner_none = NONE,
};


/* structures */

struct s_observer_channel
{
	e_observer_channel_state state;
	int32 field_4;
	uint8 observer_flags;
	uint8 owner_flags;
	uint16 field_A;
	int32 channel_index;
	int32 field_10;
	XNADDR xnaddr;
	int32 field_38;
	int32 session_index;
	int32 field_40;
	int32 field_44;
	int32 field_48;
	int32 field_4C;
	int32 field_50;
	int32 field_54;
	int32 field_58;
	transport_address address;
	datum qos_attempt_index;
	s_qos_attempt_data qos_attempt;
	int32 field_94;
	int32 field_98;
	int32 field_9C;
	uint8 gap_A0[8];
	c_network_time_statistics received_bytes_statistics;
	c_network_window_statistics packet_round_trip_statistics;
	c_network_time_statistics transmitted_bytes_statistics;
	c_network_time_statistics acknowledged_bytes_statistics;
	c_network_window_statistics packet_loss_statistics;
	c_network_window_statistics packet_warning_statistics;
	int32 field_660;
	int32 field_664;
	int32 field_668;
	int32 field_66C;
	int32 field_670;
	int32 field_674;
	int32 field_678;
	int32 field_67C;
	int32 field_680;
	int32 field_684;
	int32 field_688;
	uint8 gap_68C[4];
	int64 field_690;
	int32 unmanaged_stream_bandwidth;
	real32 net_rate_unmanaged_stream;
	int32 unmanaged_stream_window_size;
	bool active;
	bool is_load_bearing;
	bool is_simulation;
	bool is_simulation_authority;
	bool is_simulation_client;
	bool is_synchronous;
	bool is_synchronous_joining;
	bool saturate_stream;
	int32 stream_bps;
	int32 stream_window_size;
	real32 stream_packet_rate;
	bool field_6B8;
	int8 field_6B9;
	int8 field_6BA;
	int32 field_6BC;
	int32 field_6C0;
	int32 field_6C4;
	int32 net_rtt;
	int32 field_6CC;
	int32 field_6D0;
	int32 field_6D4;
	bool field_6D8;
	int32 field_6DC;
	int32 field_6E0;
	int32 field_6E4;
	int32 field_6E8;
	real32 field_6EC;
	int32 field_6F0;
	int32 field_6F4;
	int32 field_6F8;
	int32 field_6FC;
	int32 field_700;
	int32 field_704;
	int32 field_708;
	bool field_70C;
	int32 field_710;
	int32 field_714;
	real32 field_718;
	int32 field_71C;
	int32 field_720;
	uint8 field_724;
	int8 field_725;
	int8 field_726;
	int8 field_727;
	int32 field_728;
	int32 throughput_bps;
	int32 field_730;
	int32 field_734;
	LONGLONG field_738;
};
ASSERT_STRUCT_SIZE(s_observer_channel, 0x740);

class c_network_observer
{
public:
	void* vtbl;
	void* m_network_link;
	void* m_network_message_gateway;
	void* m_message_types;
	struct s_network_observer_configuration* m_configuration;
	int32* field_14;
	uint8 gap_18[8];
	s_transport_secure_identifier m_session_id;
	uint8 gap28[32];
	uint8 field_48;
	s_transport_secure_key m_session_key;
	int32 field_5C;
	uint8 gap_60[4];
	int32 field_64;
	uint8 gap_68[12];
	int32 field_74;
	uint8 gap_78[8];
	s_observer_channel m_observer_channels[k_network_channel_count];
	bool network_observer_enabled;
	int8 field_7481;
	int32 m_estimate_upstream_bps;
	int32 m_bandwidth_maximum_throughput_bps;
	int32 m_bandwidth_congestion_bps;
	int32 field_7490;
	bool field_7494;
	uint8 gap_7495[3];
	int32 m_bandwidth_satiation_count[2];
	int32 field_74A0;
	int32 field_74A4;
	int32 field_74A8;
	uint8 gap_74AC[4];
	c_network_time_statistics m_bandwidth_statistics;
	int32 field_7588;
	int32 field_758C;
	int32 field_7590;
	int32 field_7594;
	int8 field_7598;
	int32 field_759C;
	int32 m_allocated_bandwidth_bps;
	real32 m_allocated_rate_authority;
	real32 m_allocated_rate_client;
	int32 field_75AC;
	int32 field_75B0;
	int32 field_75B4;
	int32 field_75B8;
	int8 field_75BC;
	int8 field_75BD;
	int8 field_75BE;
	int32 field_75C0;
	int32 field_75C4;

	static void apply_patches();
	static void reset_network_observer_bandwidth_preferences();

	bool initialize_observer(class c_network_link* link, class c_network_message_type_collection* message_types, class c_network_message_gateway* message_gateway, struct s_network_observer_configuration* configuration);
	bool __thiscall channel_should_send_packet_hook(
		int32 network_channel_index,
		bool a4,
		bool a5,
		int32 a6,
		int32* out_send_sequenced_packet,
		bool* out_force_fill_packet,
		int32* out_packet_size,
		int32* out_voice_size,
		int32 out_voice_chat_data_buffer_size,
		uint8* out_voice_chat_data_buffer);

	bool __thiscall get_bandwidth_results(int32* bandwidth_successful_bps, real32* bandwidth_satiation_fraction, int32* bandwidth_unsafe_bps);
	int32 get_observer_channel_state(int32 observer_index) { return m_observer_channels[observer_index].state; };
	void send_message(e_network_observer_owner session_index, int32 observer_index, bool send_out_of_band, int32 type, int32 size, const void* data);

	int32 observer_channel_find_by_network_channel(
		int32 session_index,
		int32 network_channel_index)
	{
		int32 observer_index = NONE;

		for (int32 i = 0; i < k_network_channel_count; i++)
		{
			s_observer_channel* observer_channel = &m_observer_channels[i];
			if (observer_channel->state != _observer_channel_state_none
				&& observer_channel->channel_index == network_channel_index
				&& TEST_BIT(observer_channel->owner_flags, session_index))
			{
				observer_index = i;
				break;
			}
		}

		return observer_index;
	}

private:
};
ASSERT_STRUCT_SIZE(c_network_observer, 0x75C8);

struct s_network_observer_configuration
{
	int32 field_0;
	int32 field_4;
	int8 gap_8[28];
	int32 field_24;
	int32 field_28;
	int32 field_2C;
	int32 field_30;
	int8 gap_34[20];
	int32 field_48;
	int32 field_4C;
	int32 field_50;
	int32 field_54;
	int32 field_58;
	int8 gap_5C[16];
	int32 field_6C;
	int32 field_70;
	int32 field_74;
	int32 field_78;
	int32 field_7C;
	int32 field_80;
	int32 field_84;
	int32 field_88;
	int32 field_8C;
	real32 field_90;
	real32 field_94;
	real32 field_98;
	int32 stream_packet_rate_throttle_scale_stage_count;
	real32 stream_packet_rate_throttle_scale_stages[16];
	int32 stream_minimum_window_size_bytes;
	real32 field_E4;
	real32 field_E8;
	real32 field_EC;
	real32 field_F0;
	int32 field_F4;
	int32 field_F8;
	int32 field_FC;
	int32 field_100;
	real32 field_104;
	int32 avg_payload_size_outside_simulation;
	int32 avg_payload_size_during_simulation;
	real32 minimum_packet_rate_scale;
	int32 minimum_estimated_upstream_bandwidth_bps;
	int32 field_118;
	real32 field_11C;
	int32 field_120;
	int32 field_124;
	int32 field_128;
	int32 field_12C;
	int32 field_130;
	int32 field_134;
	int32 field_138;
	int32 field_13C;
	int32 field_140;
	int32 field_144;
	int32 field_148;
	int32 stream_minimum_bps;
	int32 stream_maximum_bps;
	int32 field_154;
	int32 stream_desired_bps;
	int32 upstream_bandwidth0;
	int32 upstream_bandwidth1;
	int32 stream_default_bps;
	int32 field_168;
	int32 field_16C;
	int32 field_170;
	int32 field_174;
	real32 field_178;
	int32 field_17C;
	real32 stream_packet_rate_scale_during_packet_loss;
	int32 field_184;
	int32 field_188;
	int32 stream_maximum_bps_regrowth_per_cycle;
	real32 field_190;
	int32 stream_maximum_bps_throttle_per_cycle;
	real32 field_198;
	int32 field_19C;
	int32 stream_minimum_period_before_adjust_attempt_msec;
	int32 field_1A4;
	int32 field_1A8;
	real32 field_1AC;
	int32 field_1B0;
	int32 field_1B4;
	int32 field_1B8;
	int32 field_1BC;
	int32 field_1C0;
	real32 field_1C4;
	real32 field_1C8;
	int32 field_1CC;
	real32 field_1D0;
	int32 field_1D4;
	int32 field_1D8;
	int32 field_1DC;
	int32 field_1E0;
	int32 field_1E4;
	int32 field_1E8;
	int32 field_1EC;
	int32 field_1F0;
	int32 field_1F4;
	int32 field_1F8;
	real32 field_1FC;
	int32 field_200;
};
ASSERT_STRUCT_SIZE(s_network_observer_configuration, 0x204);

/* globals */

extern s_network_observer_configuration* g_network_observer_configuration;
