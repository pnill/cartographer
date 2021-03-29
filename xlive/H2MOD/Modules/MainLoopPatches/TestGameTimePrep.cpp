//#include "TestGameTimePrep.h"
//#include "H2MOD/Modules/Config/Config.h"
//#include "UncappedFPS/UncappedFPS.h"
//#include <iostream>
//#include "Util/Hooks/Hook.h"
//#include "H2MOD/Modules/OnScreenDebug/OnScreenDebug.h"
//#include "H2MOD/Modules/Utils/Utils.h"
//
//#define FLODWORD(a)      (((float)(a)) & 0x00000000FFFFFFFF)
//
//char(*game_globals_unk1)();
//
//typedef void __cdecl sub_9EC02C(float a1);
//sub_9EC02C* _sub_9EC02C;
//
//typedef bool __cdecl c_cinematic_in_progress();
//c_cinematic_in_progress* cinimatic_in_progress;
//
//typedef char c_cinematic_is_running();
//c_cinematic_is_running* cinimatic_is_running;
//
//typedef signed int c_get_tickrate();
//c_get_tickrate* get_tickrate;
//
//typedef char c_sub_9b9887();
//c_sub_9b9887* sub_9b9887;
//
//typedef signed int __cdecl c_sub_B1DCBB(BYTE *a1);
//c_sub_B1DCBB* sub_B1DCBB;
//
//typedef int(c_get_game_mode_engine)();
//c_get_game_mode_engine* get_game_mode_engine;
//
//enum t_startup_flags : int
//{
//	windowed,
//	disable_voice_chat,
//	nosound,
//	allow_d3d_ex_version, // allows the d3d ex version to be used instead
//	disable_hardware_vertex_processing, // force hardware vertex processing off
//	novsync,
//	unk6, // squad browser/xlive/ui?
//	nointro, // disables intro movie
//	unk8, // some tag thing?
//	unk9, // some tag thing?
//	unk10, // some tag thing?
//	unk11, // some tag thing?
//	unk12, // some tag thing?
//	unk13, // some tag thing?
//	unk14, // some tag thing?
//	unk15, // seen near xlive init code
//	unk16,
//	xbox_live_silver_account, // if true, disables 'gold-only' features, like quickmatch etc
//	unk18, // fuzzer/automated testing? (sapien)
//	ui_fast_test_no_start, // same as below but doesn't start a game
//	ui_fast_test, // auto navigates the UI selecting the default option
//	unk21, // player controls related, is checked when using a vehicle
//	monitor_count,
//	unk23,
//	unk24,
//	unk25, // something to do with game time?
//	unk26,
//	unk27, // network? value seems unused?
//	high_quality, // forced sound reverb ignoring CPU score and disable forcing low graphical settings (sapien)
//
//	count
//};
//static_assert(t_startup_flags::count == 29, "Bad flags count");
//
//int t_flag_log_count[t_startup_flags::count];
//
//BOOL __cdecl t_is_init_flag_set(t_startup_flags id)
//{
//	if (t_flag_log_count[id] < 10)
//	{
//		LOG_TRACE_GAME("is_init_flag_set() : flag {}", id);
//		t_flag_log_count[id]++;
//		if (t_flag_log_count[id] == 10)
//			LOG_TRACE_GAME("is_init_flag_set() : flag {} logged to many times ignoring", id);
//	}
//	DWORD* init_flags_array = Memory::GetAddress<DWORD*>(0x46d820);
//	return init_flags_array[id] != 0;
//}
//int ftoi(float in)
//{
//	return *reinterpret_cast<int*>(&in);
//}
//void __cdecl game_time_globals_prep(float dt, float *out_dt, int *out_target_ticks)
//{
//	int *v4; // r14
//	float *v5; // r15
//	float v6; // xmm6_4
//	float v7; // xmm7_4
//	int v8; // edi
//	float v9; // xmm12_4
//	time_globals* v10; // rax
//	float v11; // xmm2_4
//	float v12; // xmm1_4
//	int v13; // eax
//	time_globals* v14; // rdx
//	char v15; // di
//	int v16; // eax
//	time_globals* v17; // rcx
//	int v18; // ebx
//	int v19; // eax
//	float v20; // xmm0_4
//	float v21; // xmm7_4
//	float v22; // xmm0_4
//	float v23; // xmm0_4
//	float valid_game_speed; // xmm9_4
//	float v25; // xmm0_4
//	int v26; // er8
//	float v27; // xmm1_4
//	float v28; // xmm0_4
//	float valid_game_speed_2; // xmm6_4
//	int v30; // esi
//	float v31; // xmm10_4
//	int v32; // ecx
//	byte v34; // [rsp+B8h] [rbp+10h]
//	float v35; // [rsp+C0h] [rbp+18h]
//
//	v4 = out_target_ticks;
//	v5 = out_dt;
//	v6 = 0.0;
//	v7 = 0.0;
//	v8 = 0;
//	v9 = dt;
//	if (game_globals_unk1())
//	{
//		v10 = time_globals::get_game_time_globals();
//		v11 = v10->field_18;
//		if (v11 > 0.0)
//		{
//			v12 = v9 + v10->field_10;
//			v10->field_10 = v12;
//			if (v11 <= v12)
//			{
//				_sub_9EC02C(v10->field_20);
//				v10->field_18 = 0;
//			}
//			else
//			{
//				_sub_9EC02C(
//					(float)((float)(1.0 - (float)(v12 / v11)) * *(float *)(v10 + 28))
//					+ (float)((float)(v12 / v11) * *(float *)(v10 + 32)));
//			}
//		}
//	}
//		v14 = time_globals::get_game_time_globals();
//	if (!(game_globals_unk1()
//		|| v14->initialized && v14->game_is_paused
//		|| v14->game_speed <= 0.0))
//	{
//		goto LABEL_53;
//	}
//	v34 = 0;
//	v15 = 0;
//	if (cinimatic_in_progress())
//		v15 = 1;
//	if (sub_9b9887())
//	{
//		v19 = sub_B1DCBB(&v34);
//		v17 = time_globals::get_game_time_globals();
//		v18 = v19;
//	}
//	else
//	{
//		v16 = 8;
//		v17 = time_globals::get_game_time_globals();
//		v18 = v17->ticks_per_second * v16;
//	}
//	if (!v34 && !v15)
//	{
//		v20 = *(float *)(v17 + 12);
//		v21 = v20 >= 1.0 ? fminf(5.0, v20) : 1.0;
//		v22 = 8 * v21;
//		v23 = v22 < 0.0 ? v22 - 0.5 : v22 + 0.5;
//		if (v18 > (signed int)v23)
//			v18 = (signed int)v23;
//	}
//	if (!sub_9b9887())
//	{
//		v13 = 3;
//		if (v18 < 3)
//			v13 = v18;
//		v18 = v13;
//	}
//	if (!v34)
//	{
//		v7 = v9 * v17->game_speed;
//		valid_game_speed = (float)((float)v17->ticks_per_second * v7)
//			+ v17->field_10;
//		v35 = valid_game_speed + 0.000099999997;
//		//v8 = fminf(floor(v35), 1);
//		int fi = ftoi(valid_game_speed + 0.000099999997);
//		v8 = ~(((unsigned __int8)(fi >> 23) - 127) >> 31) & (((fi >> 31) ^ ((signed int)(fi & 0x7FFFFF | 0x800000) >> (-106 - (fi >> 23)))) - (fi >> 31));
//		if (cinimatic_is_running() && get_tickrate() > 30)
//		{
//			if (t_is_init_flag_set(unk21))
//			{
//				v25 = fminf(2.0, valid_game_speed);
//				if (v8 > 2)
//					v8 = 2;
//				valid_game_speed = v25;
//			}
//			if (v8 > 0)
//			{
//				v14 = time_globals::get_game_time_globals();
//				v26 = v14->tick_count;
//				v13 = v8 - 1;
//				if (!(((BYTE)v26 + (BYTE)v8) & 1))
//					v13 = v8;
//				v8 = v13;
//				if (v18 > 0)
//				{
//					v13 = v26 + v18;
//					if (((BYTE)v26 + (BYTE)v18) & 1)
//						--v18;
//				}
//			LABEL_46:
//				if (!v34)
//				{
//					if (v8 <= v18)
//					{
//						v6 = valid_game_speed - (float)v8;
//						goto LABEL_53;
//					}
//					goto LABEL_52;
//				}
//				goto LABEL_50;
//			}
//		}
//		else
//		{
//			if ((t_is_init_flag_set(unk21)))
//			{
//				v8 = 1;
//				valid_game_speed = 1.0;
//			}
//		}
//		v14 = time_globals::get_game_time_globals();
//		goto LABEL_46;
//	}
//	v14 = time_globals::get_game_time_globals();
//LABEL_50:
//	if (v18 <= 0)
//	{
//		v6 = 1.0;
//	LABEL_52:
//		v8 = v18;
//		v7 = (float)v18 * *(float *)(v14 + 4);
//		goto LABEL_53;
//	}
//	if (v18 >= 20)
//		goto LABEL_52;
//	v27 = 0.89999998;
//	v28 = (float)((float)(v18 - 1) * 0.15000001 + 1.0);
//	if (v28 >= 0.89999998)
//		v27 = fminf(1.05, v28);
//	v7 = (float)(v9 * *(float *)(v14 + 12)) * v27;
//	valid_game_speed_2 = (float)((float)*(signed __int16 *)(v14 + 2) * v7) + *(float *)(v14 + 16);
//	v35 = valid_game_speed_2 + 0.000099999997;
//	//v30 = fminf(floor(v35), 1);
//	int fi = ftoi(valid_game_speed + 0.000099999997);
//	v30 = ~(((unsigned __int8)(fi >> 23) - 127) >> 31) & (((fi >> 31) ^ ((signed int)(fi & 0x7FFFFF | 0x800000) >> (-106 - (fi >> 23)))) - (fi >> 31));
//	if (!cinimatic_is_running())
//		goto LABEL_81;
//	v13 = get_tickrate();
//	if (v13 <= 30)
//		goto LABEL_81;
//	
//	if (t_is_init_flag_set(unk21))
//	{
//		v31 = fminf(2.0, valid_game_speed_2);
//		if (v30 > 2)
//			v30 = 2;
//		valid_game_speed_2 = v31;
//	}
//	if (v30 > 0)
//	{
//		v14 = time_globals::get_game_time_globals();
//		v32 = v14->tick_count;
//		v13 = v30 - 1;
//		if (!(((BYTE)v32 + (BYTE)v30) & 1))
//			v13 = v30;
//		v30 = v13;
//		v13 = v32 + v18;
//		if (((BYTE)v32 + (BYTE)v18) & 1)
//			--v18;
//	}
//	else
//	{
//	LABEL_81:
//		v14 = time_globals::get_game_time_globals();
//	}
//	v6 = valid_game_speed_2 - (float)v30;
//	if (v30 > v18)
//		v6 = 1.0;
//	v8 = v18;
//	if (v30 <= v18)
//		v8 = v30;
//LABEL_53:
//	
//	*(float *)(v14 + 16) = v6;
//	addDebugText(IntToString<float>(v7, std::dec).c_str());
//	addDebugText(IntToString<float>(v8, std::dec).c_str());
//	addDebugText(IntToString<float>(v6, std::dec).c_str());
//	if (v5)
//		*v5 = v7;
//	if (v4)
//		*v4 = v8;
//	*(float *)(v14 + 40) = (float)v8;
//}
//
//typedef void(__cdecl c_simulation_update_result)(int target_ticks, int executed_ticks, float* a3);
//c_simulation_update_result* p_simulation_update_result;
//
//void __cdecl simulation_update_result(int target_ticks, int executed_ticks, float* a3)
//{
//	if(executed_ticks)
//	{
//		auto time_globals = time_globals::get_game_time_globals();
//		*a3 = fmax(*a3 - (target_ticks - executed_ticks) * time_globals->seconds_per_tick, 0.0);
//	}
//	else
//	{
//		*a3 = 0;
//	}
//}
//
//
//
//
//DWORD* game_engine_globals = Memory::GetAddress<DWORD*>(0x4BF8F8);
//
//void TestGameTimePrep::Init()
//{
//	game_globals_unk1 = (char(*)())Memory::GetAddress(0x497EA);
//	_sub_9EC02C = Memory::GetAddress<sub_9EC02C*>(0x7c02c);
//	cinimatic_in_progress = Memory::GetAddress<c_cinematic_in_progress*>(0x3a928);
//	sub_9b9887 = Memory::GetAddress<c_sub_9b9887*>(0x49887);
//	sub_B1DCBB = Memory::GetAddress<c_sub_B1DCBB*>(0x1adcbb);
//	cinimatic_is_running = Memory::GetAddress<c_cinematic_is_running*>(0x3a938);
//	get_tickrate = Memory::GetAddress<c_get_tickrate*>(0x28707);
//
//	p_simulation_update_result = Memory::GetAddress<c_simulation_update_result*>(0x7C47A);
//
//	get_game_mode_engine = Memory::GetAddress<c_get_game_mode_engine*>(0x5B15E);
//
//	PatchCall(Memory::GetAddress(0x4A60B), simulation_update_result);
//	PatchCall(Memory::GetAddress(0x39d04), game_time_globals_prep);
//
//}
