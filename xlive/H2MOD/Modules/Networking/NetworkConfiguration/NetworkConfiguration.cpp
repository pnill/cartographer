#include "stdafx.h"

#include "NetworkConfiguration.h"

#include "H2MOD.h"
#include "Util/Hooks/Hook.h"

// LIVE netcode research

network_configuration* g_network_configuration;
void __cdecl InitializeConfiguration()
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
	g_network_configuration->field_E4 = 0.25f;
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
	g_network_configuration->field_114 = 10240 * 4; // H2v - 10240, MCC = H2v * 4
	g_network_configuration->field_118 = 1000;
	g_network_configuration->field_11C = 0.5f;
	g_network_configuration->field_120 = 3;
	g_network_configuration->field_124 = 40;
	g_network_configuration->field_128 = 5000;
	g_network_configuration->field_12C = 15;
	g_network_configuration->field_130 = 100;
	g_network_configuration->field_134 = 50 * 2; // h2v - 50, MCC - 100
	g_network_configuration->field_138 = 50 * 4; // h2v - 50, MCC - 200
	g_network_configuration->field_13C = 32; // h2v - 15, MCC - 32
	g_network_configuration->field_140 = 32; // h2v - 8, MCC - 32
	g_network_configuration->field_144 = 1;
	g_network_configuration->field_148 = 2000;
	g_network_configuration->field_14C = 4096 * 4; // h2v - 4096, MCC = 4096 * 4
	g_network_configuration->field_150 = 71680 * 4; // H2v - 71680, MCC = H2v * 4
	g_network_configuration->field_154 = 1000;
	g_network_configuration->max_bits_per_second_single_player = 30720 * 4; // H2v = 30720, MCC = H2v * 4
	g_network_configuration->max_bits_per_second_full_lobby = 262144; // H2v - 122880, MCC = 262144
	g_network_configuration->max_bits_per_second_splitscreen_players = 512000;
	g_network_configuration->field_1C4 = 0.5f;
	g_network_configuration->field_1A0 = 1500;
	g_network_configuration->field_1A4 = 1500;
	g_network_configuration->field_178 = 0.1f;
	g_network_configuration->field_190 = 0.2f;
	g_network_configuration->field_1C8 = 20.0f;
	g_network_configuration->field_168 = 40;
	g_network_configuration->field_1D8 = 40;
	g_network_configuration->field_164 = 65536; // H2v 8192, MCC = 65536
	g_network_configuration->field_16C = 320;
	g_network_configuration->field_170 = 3;
	g_network_configuration->field_174 = 32;
	g_network_configuration->field_17C = 4;
	g_network_configuration->field_180 = 0.80000001f;
	g_network_configuration->field_184 = 10;
	g_network_configuration->field_188 = 21;
	g_network_configuration->field_18C = 3072 * 4; // H2v - 3072, MCC = H2V * 4
	g_network_configuration->field_194 = 5120 * 4; // H2v - 5120, MCC = H2V * 4
	g_network_configuration->field_198 = 0.30000001f;
	g_network_configuration->field_19C = 5000;
	g_network_configuration->field_1B4 = 3;
	g_network_configuration->field_1B8 = 6144 * 4; // H2v - 6144, MCC = H2v * 4
	g_network_configuration->field_1BC = 20000;
	g_network_configuration->field_1A8 = 30720 * 4; // H2v - 30720, MCC = H2v * 4;
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
	g_network_configuration->field_1F8 = 8192 * 4; // H2v - 8192, MCC  = H2v * 4 = 32768
	g_network_configuration->field_1FC = 0.33333334f;
	g_network_configuration->field_200 = 4096 * 4; // H2v - 4096, MCC  = H2v * 4 = 16384
}

//1bfb23
typedef double(__stdcall* unk_live_netcode_func_def)(DWORD *thisx, signed int a2, bool a3, bool a4);
unk_live_netcode_func_def p_unk_live_netcode_func;

double __stdcall unk_live_netcode_func(DWORD *thisx, signed int a2, bool a3, bool a4)
{
	LOG_DEBUG_FUNC("unk: {0}, bits_per_sec: {1}, protocol_is_sys_link: {2}", a2, a3, a4);

	return p_unk_live_netcode_func(thisx, a2, a3, a4);
}

typedef bool(__stdcall* unk_live_netcode_func_def_2)(void *thisx, float a1, float packet_size, int delay_between_packets, int a4, int a5, int a6, int a7, int voice_data_buffer, int a9);
unk_live_netcode_func_def_2 p_unk_live_netcode_func_2;

static int result_print_last_time = 0;
bool __stdcall unk_live_netcode_func_2(void *thisx, float a1, float packet_size, int delay_between_packets, int a4, int a5, int a6, int a7, int voice_data_buffer, int a9)
{

	if (result_print_last_time == 0)
		result_print_last_time = timeGetTime();

	/*if (timeGetTime() - result_print_last_time >= 1000)
	{
		result_print_last_time = timeGetTime();

		LOG_DEBUG_FUNC("unk: {0} , unk2: {1} , unk3: {2} , unk4: {3} , unk5: {4} , unk6: {5} , unk7: {6} , unk8: {7} , unk9: {8}", a1, packet_size, delay_between_packets, a4, a5, a6, a7, voice_data_buffer, a9);
		LOG_DEBUG_FUNC("result: ", result);
	}*/

	bool result = p_unk_live_netcode_func_2(thisx, a1, packet_size, delay_between_packets, a4, a5, a6, a7, voice_data_buffer, a9);

	return result;
}
 
void NetworkConfiguration::ApplyPatches()
{
	// increase the network tickrate of hosts to 60 and for the clients to 30
	static float netcode_tickrate = 60.0f;

	std::vector<std::pair<int, int>> addresses = 
	{
		{ 0x1BDE27, 0x1B7D01 },
		{ 0x1BE2FA, 0x1B81D4 },
		{ 0x1BFB3C, 0x1B9A1C },
		{ 0x1C11FA, 0x1BB0DA },
		{ 0x1C12BF, 0x1BB19F }
	};

	for (auto address : addresses)
		WritePointer(h2mod->GetAddress(address.first, address.second) + 4, &netcode_tickrate);

	g_network_configuration = h2mod->GetAddress<network_configuration*>(0x4F960C, 0x523B5C);
	PatchCall(h2mod->GetAddress(0x1ABE23, 0x1AC328), InitializeConfiguration);

	NopFill(h2mod->GetAddress(0x1BFBE7, 0x1B9AC7), 19);
	NopFill(h2mod->GetAddress(0x1BE33A, 0x1B8214), 15);
	NopFill(h2mod->GetAddress(0x1BDF1D, 0x1B7DF7), 18);

	// increase the network heap size
	WriteValue<DWORD>(h2mod->GetAddress(0x1ACCC8, 0x1ACE96) + 6, 10485760); // original H2v: 1048576, MCC: 1048576

	// disables LIVE netcode
/*
	WriteValue<BYTE>(h2mod->GetAddress(0x1B555B, 0x1A92B9) + 1, 0);
	// disable ping bars
	NopFill(h2mod->GetAddress(0x1D4E33, 0x1C1B7D), 2);
	WriteValue<BYTE>(h2mod->GetAddress(0x1D4E35, 0x1C1B7F), 0xEB); // jmp
*/

	//p_unk_live_netcode_func = (unk_live_netcode_func_def)DetourClassFunc(h2mod->GetAddress<BYTE*>(0x1BFB23, 0x1B9A03), (BYTE*)unk_live_netcode_func, 10);
	//NopFill((DWORD)(BYTE*)(p_unk_live_netcode_func)+5 + 3, 5);

	//p_unk_live_netcode_func_2 = (unk_live_netcode_func_def_2)DetourClassFunc(h2mod->GetAddress<BYTE*>(0x1BEE8D, 0x1B9A03), (BYTE*)unk_live_netcode_func_2, 14);
}