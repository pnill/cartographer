#include "stdafx.h"

#include "NetworkObserver.h"
#include "Util\Hooks\Hook.h"

network_observer_configuration* g_network_configuration;

#define k_network_preference_size 108

#if INCREASE_NETWORK_TICKRATE
#define k_online_netcode_tickrate_real 60.0f
#endif

// LIVE netcode research
void __cdecl initialize_network_observer_configuration()
{
	/*
		Configuration constants used by the game to define how LIVE netcode will behave
	*/

	g_network_configuration->field_0 = 1;
	g_network_configuration->field_2C = 500;
	g_network_configuration->field_30 = 500;
	g_network_configuration->field_4C = 500;
	g_network_configuration->field_50 = 500;
	g_network_configuration->field_78 = 500;
	g_network_configuration->field_94 = 1.0f;
	g_network_configuration->field_98 = 30.0f;
	g_network_configuration->field_80 = 4000;
	g_network_configuration->field_84 = 4000;
	g_network_configuration->field_100 = 4000;
	g_network_configuration->unk_floats_A0[0] = 1.0f;
	g_network_configuration->unk_floats_A0[1] = 0.66666669f;
	g_network_configuration->unk_floats_A0[2] = 0.5f;
	g_network_configuration->unk_floats_A0[3] = 0.33333334f;
	g_network_configuration->unk_floats_A0[4] = 0.25f;
	g_network_configuration->unk_floats_A0[5] = 0.2f;
	g_network_configuration->unk_floats_A0[6] = 0.1f;
	g_network_configuration->unk_total_flt_array_elements = 7;
	g_network_configuration->field_4 = 0;
	g_network_configuration->field_24 = 3;
	g_network_configuration->field_28 = 0;
	g_network_configuration->field_48 = 4;
	g_network_configuration->field_54 = 1000;
	g_network_configuration->field_58 = 2000;
	g_network_configuration->field_6C = 15000;
	g_network_configuration->field_70 = 1000;
	g_network_configuration->field_74 = 2000;
	g_network_configuration->field_7C = 2000;
	g_network_configuration->field_88 = 1000;
	g_network_configuration->field_8C = 5000;
	g_network_configuration->field_90 = 0.5f;
	g_network_configuration->field_E0 = 200;
	g_network_configuration->field_E4 = 0.25f;
	g_network_configuration->field_E8 = 0.5f;
	g_network_configuration->field_EC = 0.75f;
	g_network_configuration->field_F0 = 12.0f;
	g_network_configuration->field_F4 = 2000;
	g_network_configuration->field_F8 = 2000;
	g_network_configuration->field_FC = 30;
	g_network_configuration->field_104 = 0.5;
	g_network_configuration->field_108 = 64;
	g_network_configuration->field_10C = 96;
	g_network_configuration->field_110 = 0.33333334f;
	g_network_configuration->field_114 = 10240 * 4; // H2v - 10240, 60 tick = H2v * 4
	g_network_configuration->field_118 = 1000;
	g_network_configuration->field_11C = 0.5f;
	g_network_configuration->field_120 = 3;
	g_network_configuration->field_124 = 40;
	g_network_configuration->field_128 = 5000;
	g_network_configuration->field_12C = 15;
	g_network_configuration->field_130 = 100;
	g_network_configuration->field_134 = 50 * 2; // h2v - 50, 60 tick - 100
	g_network_configuration->field_138 = 50 * 4; // h2v - 50, 60 tick - 200
	g_network_configuration->field_13C = 32;     // h2v - 15, 60 tick - 32
	g_network_configuration->field_140 = 32;     // h2v - 8, 60 tick - 32
	g_network_configuration->field_144 = 1;
	g_network_configuration->field_148 = 2000;
	g_network_configuration->field_14C = 4096 * 4; // h2v - 4096, 60 tick = 4096 * 4
	g_network_configuration->field_150 = 71680 * 4; // H2v - 71680, 60 tick = H2v * 4
	g_network_configuration->field_154 = 1000;
	g_network_configuration->max_bits_per_second_single_player = 30720 * 4; // H2v = 30720, 60 tick = H2v * 4
	g_network_configuration->max_bits_per_second_full_lobby = 262144; // H2v - 122880, 60 tick = 262144
	g_network_configuration->max_bits_per_second_splitscreen_players = 512000;
	g_network_configuration->field_1C4 = 0.5f;
	g_network_configuration->field_1A0 = 1500;
	g_network_configuration->field_1A4 = 1500;
	g_network_configuration->field_178 = 0.1f;
	g_network_configuration->field_190 = 0.2f;
	g_network_configuration->field_1C8 = 20.0f;
	g_network_configuration->field_168 = 40;
	g_network_configuration->field_1D8 = 40;
	g_network_configuration->field_164 = 65536; // H2v 8192, 60 tick = 65536
	g_network_configuration->field_16C = 320;
	g_network_configuration->field_170 = 3;
	g_network_configuration->field_174 = 32;
	g_network_configuration->field_17C = 4;
	g_network_configuration->field_180 = 0.80000001f;
	g_network_configuration->field_184 = 10;
	g_network_configuration->field_188 = 21;
	g_network_configuration->field_18C = 3072 * 4; // H2v - 3072, 60 tick = H2V * 4
	g_network_configuration->field_194 = 5120 * 4; // H2v - 5120, 60 tick = H2V * 4
	g_network_configuration->field_198 = 0.30000001f;
	g_network_configuration->field_19C = 5000;
	g_network_configuration->field_1B4 = 3;
	g_network_configuration->field_1B8 = 6144 * 4; // H2v - 6144, 60 tick = H2v * 4
	g_network_configuration->field_1BC = 20000;
	g_network_configuration->field_1A8 = 30720 * 4; // H2v - 30720, 60 tick = H2v * 4;
	g_network_configuration->field_1AC = 0.75f;
	g_network_configuration->field_1B0 = 60000;
	g_network_configuration->field_1C0 = 6;
	g_network_configuration->field_1CC = 0;
	g_network_configuration->field_1D0 = 0.00039062501f;
	g_network_configuration->field_1D4 = 160;
	g_network_configuration->field_1DC = 120;
	g_network_configuration->field_1E0 = 3;
	g_network_configuration->field_1E4 = 10;
	g_network_configuration->field_1E8 = 10;
	g_network_configuration->field_1EC = 3;
	g_network_configuration->field_1F0 = 8000;
	g_network_configuration->field_1F4 = 6;
	g_network_configuration->field_1F8 = 8192 * 4; // H2v - 8192, 60 tick  = H2v * 4 = 32768
	g_network_configuration->field_1FC = 0.33333334f;
	g_network_configuration->field_200 = 4096 * 4; // H2v - 4096, 60 tick  = H2v * 4 = 16384
}

void s_network_observer::sendNetworkMessage(int session_index, int observer_index, e_network_message_send_protocol send_out_of_band, int type, int size, void* data)
{
	typedef void(__thiscall* observer_channel_send_message)(s_network_observer*, int, int, e_network_message_send_protocol, int, int, void*);
	auto p_observer_channel_send_message = Memory::GetAddress<observer_channel_send_message>(0x1BED40, 0x1B8C1A);

	p_observer_channel_send_message(this, session_index, observer_index, send_out_of_band, type, size, data);
}

bool __cdecl is_network_observer_mode_managed()
{
	// or in other terms this verifies if the network protocol is LIVE (aka managed)
	// this is used for host migration happening on game start (that causes the short delay when the game starts in a p2p session)
	// which is disabled in LAN mode
	return false;
}

void s_network_observer::ResetNetworkPreferences()
{
	// clear the network bandwidth preferences so they won't cause issues
	SecureZeroMemory(Memory::GetAddress<void*>(0x47E9D8 + 0x1DC), k_network_preference_size);
}

bool __thiscall s_network_observer::GetNetworkMeasurements(DWORD *out_throughput, float *out_satiation, DWORD *a4)
{
	// let the game know we don't have any bandwidth measurements available to save
	return false;
}

void __declspec(naked) call_GetNetworkMeasurements() { __asm jmp s_network_observer::GetNetworkMeasurements }

DWORD* dataToOverwrite1 = nullptr;
__declspec (naked) void overwrite1()
{
	__asm 
	{
		mov eax, dataToOverwrite1
		mov dword ptr[eax], 131072
		mov dword ptr[eax + 4], 131072
		ret
	}
}

// raw WinSock has a 28 bytes packet overhead for the packet header, unlike Xbox LIVE, which has 44 bytes (28 bytes + whatever LIVE packet header adds)
int __cdecl transport_get_packet_overhead_hook(int protocol_type)
{
	enum e_protocol_type : int
	{
		e_protocol_udp_loopback = 2,
		e_protocol_udp,
		e_protocol_tcp // not entirely sure if this is TCP
	};

	switch ((e_protocol_type)protocol_type)
	{

	// replace XNet UDP header overhead with WinSock overhead
	case e_protocol_udp_loopback:
		// return 44;
		return 28;

	case e_protocol_udp:
		//return 48;
		return 28 + 4;
		
	case e_protocol_tcp:
		return 56;
	
	default:
		break;
	}

	return 0;
}

void s_network_observer::ApplyPatches()
{
#if USE_LIVE_NETCODE
#if INCREASE_NETWORK_TICKRATE
	// increase the network tickrate of hosts to 60
	static float netcode_tickrate = k_online_netcode_tickrate_real;

	WritePointer(Memory::GetAddress(0x1BDE27, 0x1B7D01) + 4, &netcode_tickrate);
	WritePointer(Memory::GetAddress(0x1BE2FA, 0x1B81D4) + 4, &netcode_tickrate);
	WritePointer(Memory::GetAddress(0x1BFB3C, 0x1B9A1C) + 4, &netcode_tickrate);
	WritePointer(Memory::GetAddress(0x1C11FA, 0x1BB0DA) + 4, &netcode_tickrate);
	WritePointer(Memory::GetAddress(0x1C12BF, 0x1BB19F) + 4, &netcode_tickrate);

	g_network_configuration = Memory::GetAddress<network_observer_configuration*>(0x4F960C, 0x523B5C);
	PatchCall(Memory::GetAddress(0x1ABE23, 0x1AC328), initialize_network_observer_configuration);

	// other config patches
	WriteValue<DWORD>(Memory::GetAddress(0x1AB4A1, 0x1AB9A6) + 6, 20480 * 4); // 60 tick = H2v * 4
	WriteValue<DWORD>(Memory::GetAddress(0x1AB4AB, 0x1AB9B0) + 6, 51200 * 4); // 60 tick = H2v * 4
	WriteValue<DWORD>(Memory::GetAddress(0x1AB4C9, 0x1AB9CE) + 6, 65536 * 4); // 60 tick = H2v * 4
	WriteValue<DWORD>(Memory::GetAddress(0x1AB4D3, 0x1AB9D8) + 6, 32768 * 4); // 60 tick = H2v * 4

	WriteValue<float>(Memory::GetAddress(0x3A03CC, 0x360E54), 8192.f * 2.f);  // 60 tick = H2v * 2, H2v = 8192
	WriteValue<float>(Memory::GetAddress(0x3C60F0, 0x381BDC), 40960.f * 4.f); // 60 tick = H2v * 4, H2v = 40960
	WriteValue<float>(Memory::GetAddress(0x3C60F4, 0x381BE0), 30720.f * 4.f); // 60 tick = H2v * 4, H2v = 30720
	WriteValue<float>(Memory::GetAddress(0x3C60F8, 0x381BE4), 53248.f);		 // 60 tick = 53248, H2v = 9216

	WriteValue<DWORD>(Memory::GetAddress(0x1AB4FF, 0x1ABA04) + 1, 8192 * 2);  // h2v = 8192, 60 tick = h2v * 4
	WriteValue<DWORD>(Memory::GetAddress(0x1AB504, 0x1ABA09) + 1, 40960 * 4); // h2v = 40960, 60 tick = h2v * 4

	WriteValue<DWORD>(Memory::GetAddress(0x1AB558, 0x1ABA5D) + 1, 15360 * 4); // h2v = 15360, 60 tick = h2v * 4
	WriteValue<DWORD>(Memory::GetAddress(0x1AB55D, 0x1ABA62) + 2, 61440 * 4); // h2v = 61440, 60 tick = h2v * 4

	WriteValue<DWORD>(Memory::GetAddress(0x1AB582, 0x1ABA87) + 1, 131072 * 4); // 60 tick - 524288
	WriteValue<DWORD>(Memory::GetAddress(0x1AB587, 0x1ABA8C) + 1, 262144 * 4); // 60 tick - 1048576

	WriteValue<DWORD>(Memory::GetAddress(0x1AB5B6, 0x1ABABB) + 6, 10240 * 4); // 60 tick - 40960

	dataToOverwrite1 = Memory::GetAddress<DWORD*>(0x4F8200, 0x522750);
	Codecave(Memory::GetAddress(0x1AB5A2, 0x1ABAA7), overwrite1, 5);

	// prevent the game from setting the client's tickrate to half of host network tickrate
	NopFill(Memory::GetAddress(0x1BFBE7, 0x1B9AC7), 19);
	NopFill(Memory::GetAddress(0x1BE33A, 0x1B8214), 15);
	NopFill(Memory::GetAddress(0x1BDF1D, 0x1B7DF7), 18);
#endif
#else
	// disables LIVE netcode
	WriteValue<BYTE>(Memory::GetAddress(0x1B555B, 0x1A92B9) + 1, 0);
	// disable ping bars
	NopFill(Memory::GetAddress(0x1D4E33, 0x1C1B7D), 2);
	WriteValue<BYTE>(Memory::GetAddress(0x1D4E35, 0x1C1B7F), 0xEB); // jmp
#endif

	// increase the network heap size
	WriteValue<DWORD>(Memory::GetAddress(0x1ACCC8, 0x1ACE96) + 6, NETWORK_HEAP_SIZE);

	PatchCall(Memory::GetAddress(0x1E0FEE, 0x1B5EDE), call_GetNetworkMeasurements);

	WriteJmpTo(Memory::GetAddressRelative(0x5AC1BD, 0x5A6B76), transport_get_packet_overhead_hook);

	// don't force packet filling when game simulation is attached
	// otherwise we send packets filled with nothing
	// no idea if this was done on purpose or not...
	// but it makes no sense to send network packets filled with barely any game data (around 30 bytes on avg) and the rest of the packet size
	// filled with nothing
	// unless original XNet transport layer had packet compression but even then it's rather dumb
	// or maybe the UDP protocol has something like that, no idea
	NopFill(Memory::GetAddressRelative(0x5BF000, 0x5B8EDA), 14);

	// time patches, use the locked time at the start of netweork_send instead of the frame time delta
	BYTE instr_offset_1[] = { 0x2C };
	WriteBytes(Memory::GetAddressRelative(0x5BF145, 0x5B901F) + 0x3, instr_offset_1, sizeof(instr_offset_1));

	BYTE instr_offset_2[] = { 0x08, 0x07, 0x00, 0x00 };
	WriteBytes(Memory::GetAddressRelative(0x5BF14B, 0x5B9025) + 0x2, instr_offset_2, sizeof(instr_offset_2));

	if (!Memory::isDedicatedServer())
	{
		PatchCall(Memory::GetAddress(0x1D97DD), is_network_observer_mode_managed);
	}
}
