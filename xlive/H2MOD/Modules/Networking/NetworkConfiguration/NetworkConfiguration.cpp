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
	g_network_configuration->field_E0_200 = 200;
	g_network_configuration->field_E8 = 0.5f;
	g_network_configuration->field_EC = 0.75f;
	g_network_configuration->field_F0 = 12.0f;
	g_network_configuration->field_F4 = 2000;
	g_network_configuration->field_F8 = 2000;
	g_network_configuration->field_FC = 30;
	g_network_configuration->field_104_0_5 = 0.5;
	g_network_configuration->field_108_64 = 64;
	g_network_configuration->field_10C_96 = 96;
	g_network_configuration->field_110_0_3333333 = 0.33333334f; // this
	g_network_configuration->field_114 = 10240;
	g_network_configuration->field_118 = 1000;
	g_network_configuration->field_11C = 0.5f;
	g_network_configuration->field_120 = 3;
	g_network_configuration->field_124 = 40;
	g_network_configuration->field_128 = 5000;
	g_network_configuration->field_12C = 15;
	g_network_configuration->field_130 = 100;
	g_network_configuration->field_134 = 50;
	g_network_configuration->field_138 = 50;
	g_network_configuration->field_13C = 15;
	g_network_configuration->field_140 = 8;
	g_network_configuration->field_144 = 1;
	g_network_configuration->field_148 = 2000;
	g_network_configuration->field_14C_4096 = 4096;
	g_network_configuration->field_150 = 71680; // this
	g_network_configuration->field_154_1000 = 1000;
	g_network_configuration->max_bits_per_second_single_player = 30720 * 2;
	g_network_configuration->max_bits_per_second_full_lobby = 122880 * 2;
	g_network_configuration->max_bits_per_second_splitscreen_players = 512000 * 2;
	g_network_configuration->field_1C4 = 0.5f;
	g_network_configuration->field_1A0_1500 = 1500;
	g_network_configuration->field_1A4 = 1500;
	g_network_configuration->field_178 = 0.1f;
	g_network_configuration->field_190_0_2 = 0.2f;
	g_network_configuration->field_1C8 = 20.0f;
	g_network_configuration->field_168 = 40;
	g_network_configuration->field_1D8 = 40;
	g_network_configuration->field_164_8192 = 8192;
	g_network_configuration->field_16C_320 = 320;
	g_network_configuration->field_170 = 3;
	g_network_configuration->field_174 = 32;
	g_network_configuration->field_17C = 4;
	g_network_configuration->field_180 = 0.80000001f;
	g_network_configuration->field_184 = 10;
	g_network_configuration->field_188_21 = 21;
	g_network_configuration->field_18C_3072 = 3072;
	g_network_configuration->field_194_5120 = 5120;
	g_network_configuration->field_198_0_300000 = 0.30000001f;
	g_network_configuration->field_19C = 5000;
	g_network_configuration->field_1B4 = 3;
	g_network_configuration->field_1B8 = 6144;
	g_network_configuration->field_1BC = 20000;
	g_network_configuration->field_1A8 = 30720;
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
	g_network_configuration->field_1F8 = 8192;
	g_network_configuration->field_1FC = 0.33333334f;
	g_network_configuration->field_200 = 4096;
}

//1bfb23
typedef double(__stdcall* unk_live_netcode_func_def)(DWORD *thisx, signed int a2, bool a3, bool a4);
unk_live_netcode_func_def p_unk_live_netcode_func;

double __stdcall unk_live_netcode_func(DWORD *thisx, signed int a2, bool a3, bool a4)
{
	LOG_DEBUG_FUNC("unk: {0}, bits_per_sec: {1}, protocol_is_sys_link: {2}", a2, a3, a4);

	return p_unk_live_netcode_func(thisx, a2, a3, a4);;
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

long long int getGameTime()
{
	LARGE_INTEGER time;
	LARGE_INTEGER freq;

	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&time);

	return time.QuadPart / (freq.QuadPart / 1000);
}
 
void NetworkConfiguration::ApplyPatches()
{
	// increase the network tickrate of hosts to 60 and for the clients to 30
	static float unk_flt_ = 60.0f;

	std::vector<std::pair<int, int>> addresses = 
	{
		{ 0x1BDE27, 0x1B7D01 },
		{ 0x1BE2FA, 0x1B81D4 },
		{ 0x1BFB3C, 0x1B9A1C },
		{ 0x1C11FA, 0x1BB0DA },
		{ 0x1C12BF, 0x1BB19F }
	};

	//for (auto address : addresses)
		//WritePointer(h2mod->GetAddress(address.first, address.second) + 4, &unk_flt_);

	g_network_configuration = h2mod->GetAddress<network_configuration*>(0x4F960C, 0x523B5C);
	//PatchCall(h2mod->GetAddress(0x1ABE23, 0x1AC328), InitializeConfiguration);

	// use a constant timer rather than the game's timer, seems to improve the medal delay issue
	//WriteJmpTo(h2mod->GetAddress(0x1B3C5C, 0x1AF225), getGameTime);

	// disable network observer (broken on H2V)
	//WriteValue<BYTE>(h2mod->GetAddress() + (h2mod->Server ? 0x1A92BA : 0x1B555C), (BYTE)0);
	// also ping bars
	//WriteValue<BYTE>(h2mod->GetAddress() + (h2mod->Server ? 0x1C1B7F : 0x1D4E35), 0xEB);

	// disable LIVE netcode for now
	WriteValue<BYTE>(h2mod->GetAddress(0x1B555B, 0x1A92B9) + 1, 0);
	// disable ping bars
	NopFill(h2mod->GetAddress(0x1D4E33, 0x1C1B7D), 2);
	WriteValue<BYTE>(h2mod->GetAddress(0x1D4E35, 0x1C1B7F), 0xEB); // jmp

	// makes Live network not as laggy 
	//WriteValue<int>(h2mod->GetAddress(0x28702, 0x24896), 500);

	//p_unk_live_netcode_func = (unk_live_netcode_func_def)DetourClassFunc(h2mod->GetAddress<BYTE*>(0x1BFB23, 0x1B9A03), (BYTE*)unk_live_netcode_func, 10);
	//NopFill((DWORD)(BYTE*)(p_unk_live_netcode_func)+5 + 3, 5);

	//p_unk_live_netcode_func_2 = (unk_live_netcode_func_def_2)DetourClassFunc(h2mod->GetAddress<BYTE*>(0x1BEE8D, 0x1B9A03), (BYTE*)unk_live_netcode_func_2, 14);
}