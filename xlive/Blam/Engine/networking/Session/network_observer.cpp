#include "stdafx.h"
#include "network_observer.h"

#include "networking/delivery/network_channel.h"
#include "networking/messages/network_message_gateway.h"
#include "networking/network_event.h"

#include "shell/shell.h"

s_network_observer_configuration* g_network_observer_configuration;

#if defined(LIVE_NETWORK_PROTOCOL_FORCE_CONSTANT_NETWORK_PARAMETERS) 
#	if LIVE_NETWORK_PROTOCOL_FORCE_CONSTANT_NETWORK_PARAMETERS == true

// ### TODO configurable
real32 _online_netcode_client_rate_real = k_online_netcode_client_rate_real;
real32 _online_netcode_server_rate_real = k_online_netcode_server_rate_real;

int32 _online_netcode_client_max_bandwidth_per_channel = k_online_netcode_client_max_bandwidth_per_channel;
int32 _online_netcode_server_max_bandwidth_per_channel = k_online_netcode_server_max_bandwidth_per_channel;

// ### TODO FIXME:
int32	_online_netcode_window_size = -1;

#	endif // LIVE_NETWORK_PROTOCOL_FORCE_CONSTANT_NETWORK_PARAMETERS == true
#endif // defined(LIVE_NETWORK_PROTOCOL_FORCE_CONSTANT_NETWORK_PARAMETERS)

void __cdecl initialize_network_observer_configuration()
{
	/*
		network stream configuration contains different network parameters, used to adjust the functionality and performance counters

		UPDATE:
			I don't think this is worth fixing, the H2's code itself looks unfinished/unpolished,
			things which are addressed/improved in H3
	*/

	g_network_observer_configuration->field_2C = 500;
	g_network_observer_configuration->field_30 = 500;
	g_network_observer_configuration->field_4C = 500;
	g_network_observer_configuration->field_50 = 500;
	g_network_observer_configuration->field_78 = 500;
	g_network_observer_configuration->field_94 = 1.0f;
	g_network_observer_configuration->field_98 = 30.0f;
	g_network_observer_configuration->field_80 = 4000;
	g_network_observer_configuration->field_84 = 4000;
	g_network_observer_configuration->field_100 = 4000;
	g_network_observer_configuration->field_E4 = 0.25f;
	g_network_observer_configuration->field_0 = 1;
	g_network_observer_configuration->field_4 = 0;
	g_network_observer_configuration->field_24 = 3;
	g_network_observer_configuration->field_28 = 0;
	g_network_observer_configuration->field_48 = 4;
	g_network_observer_configuration->field_54 = 1000;
	g_network_observer_configuration->field_58 = 2000;
	g_network_observer_configuration->field_6C = 15000;
	g_network_observer_configuration->field_70 = 1000;
	g_network_observer_configuration->field_74 = 2000;
	g_network_observer_configuration->field_7C = 2000;
	g_network_observer_configuration->field_88 = 1000;
	g_network_observer_configuration->field_8C = 5000;
	g_network_observer_configuration->field_90 = 0.5f;
	
	g_network_observer_configuration->stream_packet_rate_throttle_scale_stage_count = 7;
	g_network_observer_configuration->stream_packet_rate_throttle_scale_stages[0] = 1.0f;
	g_network_observer_configuration->stream_packet_rate_throttle_scale_stages[1] = 0.66666669f;
	g_network_observer_configuration->stream_packet_rate_throttle_scale_stages[2] = 0.5f;
	g_network_observer_configuration->stream_packet_rate_throttle_scale_stages[3] = 0.33333334f;
	g_network_observer_configuration->stream_packet_rate_throttle_scale_stages[4] = 0.25f;
	g_network_observer_configuration->stream_packet_rate_throttle_scale_stages[5] = 0.2f;
	g_network_observer_configuration->stream_packet_rate_throttle_scale_stages[6] = 0.1f;

	// doesn't really need to be increased, the value per stream is proportional with round time trip
	// packets take longer to arrive when round time trip is higher
	// which allows for more data on the wire at once
	g_network_observer_configuration->stream_minimum_window_size_bytes = 200;

	g_network_observer_configuration->field_E8 = 0.5f;
	g_network_observer_configuration->field_EC = 0.75f;
	g_network_observer_configuration->field_F0 = 12.0f;
	g_network_observer_configuration->field_F4 = 2000;
	g_network_observer_configuration->field_F8 = 2000;
	g_network_observer_configuration->field_FC = 30;
	g_network_observer_configuration->field_104 = 0.5f;
	g_network_observer_configuration->avg_payload_size_outside_simulation = 64;
	g_network_observer_configuration->avg_payload_size_during_simulation = 96;
	g_network_observer_configuration->minimum_packet_rate_scale = 0.33333334f;

	// this is pretty low, could be increased, but doesn't matter since bandwidth tracking is currently disabled
	g_network_observer_configuration->minimum_estimated_upstream_bandwidth_bps = 10240;

	g_network_observer_configuration->field_118 = 1000;
	g_network_observer_configuration->field_11C = 0.5f;
	g_network_observer_configuration->field_120 = 3;
	g_network_observer_configuration->field_124 = 40;
	g_network_observer_configuration->field_128 = 5000;
	g_network_observer_configuration->field_12C = 15;
	g_network_observer_configuration->field_130 = 100;
	g_network_observer_configuration->field_134 = 50;
	g_network_observer_configuration->field_138 = 50;
	g_network_observer_configuration->field_13C = 15;
	g_network_observer_configuration->field_140 = 8;
	g_network_observer_configuration->field_144 = 1;
	g_network_observer_configuration->field_148 = 2000;
	g_network_observer_configuration->stream_minimum_bps = 4096 * 4 /* 4096 */;
	g_network_observer_configuration->stream_maximum_bps = 71680;
	g_network_observer_configuration->field_154 = 1000;
	g_network_observer_configuration->stream_desired_bps = 71680 /* 30720 */;

	g_network_observer_configuration->upstream_bandwidth0 = 1146880 /* 122880 */;
	g_network_observer_configuration->upstream_bandwidth1 = 1146880 /* 512000 */;

	g_network_observer_configuration->field_1C4 = 0.5f;
	g_network_observer_configuration->stream_minimum_period_before_adjust_attempt_msec = 1500;
	g_network_observer_configuration->field_1A4 = 1500;
	g_network_observer_configuration->field_178 = 0.1f;

	// stream re-growth factor
	g_network_observer_configuration->field_190 = 0.2f /* 0.2f */;

	g_network_observer_configuration->field_1C8 = 20.0f;
	g_network_observer_configuration->field_168 = 40;
	g_network_observer_configuration->field_1D8 = 40;

	// sligtly below stream_desired_bps
	// has to be a value which is high enough for the desired packet rate to actually send
	// and to do so when the stream is actually created, for the full bandwidth at creation
	// instead of gradually increasing the stream towards stream_maximum_bps (if possible)
	// formula: packet_rate = stream_bits_per_second / (8 * (avg_payload_size_during_simulation + network_layer_overhead (48 for UDP))
	g_network_observer_configuration->stream_default_bps = 71680 /* 8192 */;

	g_network_observer_configuration->field_16C = 320;
	g_network_observer_configuration->field_170 = 3;
	g_network_observer_configuration->field_174 = 32;
	g_network_observer_configuration->field_17C = 4;
	g_network_observer_configuration->stream_packet_rate_scale_during_packet_loss = 0.80000001f;
	g_network_observer_configuration->field_184 = 10;
	g_network_observer_configuration->field_188 = 21;
	g_network_observer_configuration->stream_maximum_bps_regrowth_per_cycle = 1024 * 15 /* 3072 */;
	g_network_observer_configuration->stream_maximum_bps_throttle_per_cycle = 1024 * 20 /* 5120 */;
	g_network_observer_configuration->field_198 = 0.30000001f;
	g_network_observer_configuration->field_19C = 5000;
	g_network_observer_configuration->field_1B4 = 3;
	g_network_observer_configuration->field_1B8 = 6144;
	g_network_observer_configuration->field_1BC = 20000;
	g_network_observer_configuration->field_1A8 = 30720;
	g_network_observer_configuration->field_1AC = 0.75f;
	g_network_observer_configuration->field_1B0 = 60000;
	g_network_observer_configuration->field_1C0 = 6;
	g_network_observer_configuration->field_1CC = 0;
	g_network_observer_configuration->field_1D0 = 0.00039062501f;
	g_network_observer_configuration->field_1D4 = 160;
	g_network_observer_configuration->field_1DC = 120;
	g_network_observer_configuration->field_1E0 = 3;
	g_network_observer_configuration->field_1E4 = 10;
	g_network_observer_configuration->field_1E8 = 10;
	g_network_observer_configuration->field_1EC = 3;
	g_network_observer_configuration->field_1F0 = 8000;
	g_network_observer_configuration->field_1F4 = 6;
	g_network_observer_configuration->field_1F8 = 8192;
	g_network_observer_configuration->field_1FC = 0.33333334f;
	g_network_observer_configuration->field_200 = 4096;
}

bool c_network_observer::initialize_observer(c_network_link* link, c_network_message_type_collection* message_types, c_network_message_gateway* message_gateway, s_network_observer_configuration* configuration)
{
	return INVOKE_TYPE(0x1BEC80, 0x1B8B5A, bool(__thiscall*)(c_network_observer*, c_network_link*, c_network_message_type_collection*, c_network_message_gateway*, s_network_observer_configuration*), this, link, message_types, message_gateway, configuration);
}

void c_network_observer::send_message(
	e_network_observer_owner session_index,
	int32 observer_index,
	bool send_out_of_band,
	int32 type,
	int32 size,
	const void* data)
{
	typedef void(__thiscall* observer_channel_send_message_t)(c_network_observer*, int32, int32, bool, int32, int32, const void*);
	INVOKE_TYPE(0x1BED40, 0x1B8C1A, observer_channel_send_message_t, this, session_index, observer_index, send_out_of_band, type, size, data);
}

bool __cdecl is_network_observer_mode_managed()
{
	// or in other terms this checks if the network protocol is LIVE
	// it is used for migrating the host before starting the game (which causes the short delay when the game starts in a p2p session)
	// in LAN mode it's disabled
	return false;
}

void c_network_observer::reset_network_observer_bandwidth_preferences()
{
	// reset the network bandwidth preferences
	SecureZeroMemory(Memory::GetAddress<void*>(0x47E9D8 + 0x1DC), k_network_preferences_size);
}

CLASS_HOOK_DECLARE_LABEL(c_network_observer__get_bandwidth_results, c_network_observer::get_bandwidth_results);
bool __thiscall c_network_observer::get_bandwidth_results(int32* bandwidth_successful_bps, real32* bandwidth_satiation_fraction, int32* bandwidth_unsafe_bps)
{
	// let the game know we don't have any bandwidth measurements available to save
	ASSERT(bandwidth_successful_bps);
	ASSERT(bandwidth_satiation_fraction);
	ASSERT(bandwidth_unsafe_bps);

	bool result = false;

	int32 satiation = m_bandwidth_satiation_count[1] + m_bandwidth_satiation_count[0];

	if (m_bandwidth_maximum_throughput_bps != NONE
		&& m_bandwidth_congestion_bps != NONE
		&& satiation > 0)
	{
		*bandwidth_successful_bps = m_bandwidth_maximum_throughput_bps;
		*bandwidth_satiation_fraction = (real32)m_bandwidth_satiation_count[1] / (real32)satiation;
		*bandwidth_unsafe_bps = m_bandwidth_congestion_bps;
		result = true;
	}

	event(
		_event_message, 
		"networking:observer:bandwidth: %s bandwidth results: throughput %dbps congestion %dbps recorded satiation %d/%d",
		result ? "REPORTING" : "NOT REPORTING",
		m_bandwidth_maximum_throughput_bps,
		m_bandwidth_congestion_bps,
		m_bandwidth_satiation_count[0],
		m_bandwidth_satiation_count[1]
	);

#ifndef _DEBUG
	// TODO FIXME:
	// currently disable reporting
	result = false;
#endif
	return result;
}

void __declspec(naked) jmp_get_bandwidth_results()
{
	CLASS_HOOK_JMP(c_network_observer__get_bandwidth_results, c_network_observer::get_bandwidth_results);
}

CLASS_HOOK_DECLARE_LABEL(c_network_observer__channel_should_send_packet_hook, c_network_observer::channel_should_send_packet_hook);
bool __thiscall c_network_observer::channel_should_send_packet_hook(
	int32 network_channel_index,
	bool a3,
	bool a4,
	int32 a5,
	int32* out_send_sequenced_packet,
	bool* out_force_fill_packet,
	int32* out_packet_size,
	int32* out_voice_size,
	int32 out_voice_chat_data_buffer_size,
	uint8* out_voice_chat_data_buffer)
{
	typedef bool(__thiscall* channel_should_send_packet_t)(c_network_observer*, int32, bool, bool, int32, int32*, bool*, int32*, int32*, int32, uint8*);
	auto p_channel_should_send_packet = Memory::GetAddress<channel_should_send_packet_t>(0x1BEE8D, 0x1B8D67);

	int32 observer_index = NONE;
	for (int32 i = 0; i < 16; i++)
	{
		if (this->m_observer_channels[i].state != _observer_channel_state_none
			&& this->m_observer_channels[i].channel_index == network_channel_index)
		{
			observer_index = i;
			break;
		}
	}

	if (observer_index == NONE)
		return false;

	s_network_channel* network_channel = s_network_channel::get(network_channel_index);
	s_observer_channel* observer_channel = &this->m_observer_channels[observer_index];

	// we modify the network channel paramters to force the network tickrate
	const real32 _temp_network_rate					= observer_channel->stream_packet_rate;
	const int32	 _temp_network_bandwidth_per_stream	= observer_channel->stream_bps;
	const int32  _temp_network_window_size			= observer_channel->stream_window_size;

#if defined(LIVE_NETWORK_PROTOCOL_FORCE_CONSTANT_NETWORK_PARAMETERS) 
#	if LIVE_NETWORK_PROTOCOL_FORCE_CONSTANT_NETWORK_PARAMETERS == true

	// patches to force static network rate and bandwidth
	// because the network configuration doesn't behave well in Halo 2 Vista
	// since the configuration is the same
	// but there are some benefits in using Online's netcode
	// like limited network packet rate, otherwise people with uncapped FPS will overflow host's packet buffer

	// first we check if we are dealing with a managed network stream
	if (observer_channel->active)
	{
		// check if we're host
		if (network_channel->is_simulation_authority())
		{
			observer_channel->stream_packet_rate = _online_netcode_server_rate_real;
			observer_channel->stream_bps = _online_netcode_server_max_bandwidth_per_channel;
		}
		else
		{
			observer_channel->stream_packet_rate = _online_netcode_client_rate_real;
			observer_channel->stream_bps = _online_netcode_client_max_bandwidth_per_channel;
		}

		observer_channel->stream_window_size = _online_netcode_window_size;
	}

#	endif // LIVE_NETWORK_PROTOCOL_FORCE_CONSTANT_NETWORK_PARAMETERS == true
#endif // defined(LIVE_NETWORK_PROTOCOL_FORCE_CONSTANT_NETWORK_PARAMETERS)

	bool ret = p_channel_should_send_packet(this, network_channel_index, a3, a4, a5, out_send_sequenced_packet, out_force_fill_packet, out_packet_size, out_voice_size, out_voice_chat_data_buffer_size, out_voice_chat_data_buffer);

	// then we reset the values back to normal
	observer_channel->stream_packet_rate		= _temp_network_rate;
	observer_channel->stream_bps		= _temp_network_bandwidth_per_stream;
	observer_channel->stream_window_size	= _temp_network_window_size;

	// don't force packet filling when game simulation is attached
	// otherwise we send packets filled with nothing...
	// no idea if this was done on purpose or not...
	// but it makes no sense to send network packets filled with barely any game data (around 30 bytes on avg) and the rest of the packet size
	// filled with nothing
	// unless original XNet transport layer had packet compression but even then it's rather dumb
	// or maybe the UDP protocol has something like that, no idea
	if (out_force_fill_packet)
		*out_force_fill_packet = false;

	return ret;
}

static void __declspec(naked) jmp_network_observer_channel_should_send_packet_hook()
{
	CLASS_HOOK_JMP(c_network_observer__channel_should_send_packet_hook, c_network_observer::channel_should_send_packet_hook);
}

void c_network_observer::apply_patches()
{
#if defined(USE_LIVE_NETWORK_PROTOCOL) && USE_LIVE_NETWORK_PROTOCOL
	// increase the network tickrate of hosts to 60
	static const real32 k_network_rate_override = k_online_netcode_server_rate_real;

	WritePointer(Memory::GetAddress(0x1BDE27, 0x1B7D01) + 4, &k_network_rate_override);
	WritePointer(Memory::GetAddress(0x1BE2FA, 0x1B81D4) + 4, &k_network_rate_override);
	WritePointer(Memory::GetAddress(0x1BFB3C, 0x1B9A1C) + 4, &k_network_rate_override);
	WritePointer(Memory::GetAddress(0x1C11FA, 0x1BB0DA) + 4, &k_network_rate_override);
	WritePointer(Memory::GetAddress(0x1C12BF, 0x1BB19F) + 4, &k_network_rate_override);

	g_network_observer_configuration = Memory::GetAddress<s_network_observer_configuration*>(0x4F960C, 0x523B5C);
	PatchCall(Memory::GetAddress(0x1ABE23, 0x1AC328), initialize_network_observer_configuration);

	// increase the network heap size
	WriteValue<int32>(Memory::GetAddress(0x1ACCC8, 0x1ACE96) + 6, k_network_heap_size);
	WriteValue<int32>(Memory::GetAddress(0x1ACCE0) + 6, k_network_heap_size); //increase network heap for campaign
	WriteValue<uint32>(Memory::GetAddress(0x1ACCDB + 1), k_network_channel_count_for_campaign);	// increase network_shared_memory_globals.maximum_channel_count for campaign

	PatchCall(Memory::GetAddress(0x1E0FEE, 0x1B5EDE), jmp_get_bandwidth_results);
	// replace vtable pointer of network_observer::channel_should_send_packet
	WritePointer(Memory::GetAddress(0x3C615C, 0x381C48), jmp_network_observer_channel_should_send_packet_hook);

	// prevent the game from setting the client's tickrate to half of host network tickrate
	NopFill(Memory::GetAddress(0x1BFBE7, 0x1B9AC7), 19);
	NopFill(Memory::GetAddress(0x1BE33A, 0x1B8214), 15);
	NopFill(Memory::GetAddress(0x1BDF1D, 0x1B7DF7), 18);

	// disable upstream bandwidth estimate override
	// note it doesn't even work on dedicated servers, keeping server hosts using the lowest upstream bandwidth available
	NopFill(Memory::GetAddress(0x1C00F6, 0x1B9FD6), 2);
#else
	// disables LIVE netcode
	WriteValue<uint8>(Memory::GetAddress(0x1B555B, 0x1A92B9) + 1, 0);
	// disable ping bars
	NopFill(Memory::GetAddress(0x1D4E33, 0x1C1B7D), 2);
	WriteValue<uint8>(Memory::GetAddress(0x1D4E35, 0x1C1B7F), 0xEB); // jmp
#endif

	// disable forced host migration in P2P games
	// this is using the XNet QoS probes to select a preferred host with a possible better connection
	// which is not really available anymore with cartographer, since the QoS probes are not that accurate anymore
	if (!shell_is_dedicated_server())
	{
		PatchCall(Memory::GetAddress(0x1D97DD, 0x1BEE59), is_network_observer_mode_managed);
	}
}