#include "TestGameTimePrep.h"
#include "H2MOD/Modules/Config/Config.h"
#include "UncappedFPS/UncappedFPS.h"
#include <iostream>
#include "Util/Hooks/Hook.h"
#include "H2MOD/Modules/OnScreenDebug/OnScreenDebug.h"
#include "H2MOD/Modules/Utils/Utils.h"

#define FLODWORD(a)      (((float)(a)) & 0x00000000FFFFFFFF)
//(((unsigned_64 *)&(a))->u._32[0])


char(*game_globals_unk1)();

typedef void __cdecl sub_9EC02C(float a1);
sub_9EC02C* _sub_9EC02C;

typedef bool __cdecl c_cinematic_in_progress();
c_cinematic_in_progress* cinimatic_in_progress;

typedef char c_cinematic_is_running();
c_cinematic_is_running* cinimatic_is_running;

typedef signed int c_get_tickrate();
c_get_tickrate* get_tickrate;

typedef char c_sub_9b9887();
c_sub_9b9887* sub_9b9887;

typedef signed int __cdecl c_sub_B1DCBB(BYTE *a1);
c_sub_B1DCBB* sub_B1DCBB;

enum t_startup_flags : int
{
	windowed,
	disable_voice_chat,
	nosound,
	allow_d3d_ex_version, // allows the d3d ex version to be used instead
	disable_hardware_vertex_processing, // force hardware vertex processing off
	novsync,
	unk6, // squad browser/xlive/ui?
	nointro, // disables intro movie
	unk8, // some tag thing?
	unk9, // some tag thing?
	unk10, // some tag thing?
	unk11, // some tag thing?
	unk12, // some tag thing?
	unk13, // some tag thing?
	unk14, // some tag thing?
	unk15, // seen near xlive init code
	unk16,
	xbox_live_silver_account, // if true, disables 'gold-only' features, like quickmatch etc
	unk18, // fuzzer/automated testing? (sapien)
	ui_fast_test_no_start, // same as below but doesn't start a game
	ui_fast_test, // auto navigates the UI selecting the default option
	unk21, // player controls related, is checked when using a vehicle
	monitor_count,
	unk23,
	unk24,
	unk25, // something to do with game time?
	unk26,
	unk27, // network? value seems unused?
	high_quality, // forced sound reverb ignoring CPU score and disable forcing low graphical settings (sapien)

	count
};
static_assert(t_startup_flags::count == 29, "Bad flags count");

int t_flag_log_count[t_startup_flags::count];

BOOL __cdecl t_is_init_flag_set(t_startup_flags id)
{
	if (t_flag_log_count[id] < 10)
	{
		LOG_TRACE_GAME("is_init_flag_set() : flag {}", id);
		t_flag_log_count[id]++;
		if (t_flag_log_count[id] == 10)
			LOG_TRACE_GAME("is_init_flag_set() : flag {} logged to many times ignoring", id);
	}
	DWORD* init_flags_array = h2mod->GetAddress<DWORD*>(0x46d820);
	return init_flags_array[id] != 0;
}


//void __cdecl game_time_globals_prep(float dt, float *out_dt, float *out_target_ticks)
//{
//	//a4 == dt
//	time_globals time_globals = *time_globals::get_game_time_globals();
//	float f_unk1 = 0.0f;	//v6
//	float f_unk2 = 0.0f;	//v7
//	float f_unk3;			//v12
//	float f_unk4;			//v14
//	float f_unk5;			//v11
//	int i_unk1 = 0;			//v8
//
//	 if(game_globals_unk1())
//	 {
//		 f_unk5 = time_globals.field_18;
//		 if (time_globals.field_18 > 0.0f)
//		 {
//			 f_unk3 = dt + time_globals.field_14;
//			 time_globals.field_14 = f_unk3;
//			 if (time_globals.field_18 <= f_unk3)
//			 {
//				 _sub_9EC02C(time_globals.field_20);
//				 time_globals.field_18 = 0;
//			 }
//			 else
//			 {
//				 _sub_9EC02C(
//					 ((1.0f - (f_unk3 / f_unk5)  * time_globals.field_1C) +
//					 (time_globals.field_20 * (f_unk3 / f_unk5)))
//				 );
//			 }
//		 }
//	 }
//	 if(game_globals_unk1() || time_globals.initialized || time_globals.game_speed <= 0.0f)
//	 {
//		 //LABEL_53
//		 time_globals.field_10 = f_unk1;
//
//	 }
//}

//void __cdecl game_time_globals_prep(float dt, float *out_dt, float *out_target_ticks)
//{
//	float f_unk1 = 0.0f;	//v17
//	float f_unk2 = 0.0f;	//v19
//	float f_unk3 = 0.0f;	//v20
//	float f_unk4 = 0.0f;	//v12
//	float f_unk5 = 0.0f;	//v13
//	float f_unk6 = 0.0f;	//v15
//	float valid_game_speed = 1.0f;
//	float time_game_speed = 0.0f;
//
//	DWORD d_valid_game_speed;
//	int i_unk1 = 0;			//v3
//	BYTE i_unk2 = 0;		//v16
//	int i_unk3 = 0;			//v7
//	signed int i_unk4 = 0;	//v10
//	int i_unk5 = 0;			//v14
//	time_globals tGlobals = *time_globals::get_game_time_globals();
//	if(game_globals_unk1())
//	{
//		auto time_18 = tGlobals.field_18;
//		if(time_18 > 0.0)
//		{
//			auto time_14_dt = time_globals::get_game_time_globals()->field_14 + dt;
//			time_globals::get_game_time_globals()->field_14 = time_14_dt;
//			if(time_18 <= time_14_dt)
//			{
//				_sub_9EC02C(time_globals::get_game_time_globals()->field_20);
//				time_globals::get_game_time_globals()->field_18 = 0.0f;
//			}
//			else
//			{
//				_sub_9EC02C(
//					(1.0f - (time_14_dt / time_18)  * time_globals::get_game_time_globals()->field_1C) +
//					(time_globals::get_game_time_globals()->field_20 * (time_14_dt / time_18))
//				);
//			}
//		}
//	}
//	if(!game_globals_unk1() || time_globals::get_game_time_globals()->game_is_paused || time_globals::get_game_time_globals()->game_speed <= 0.0)
//	{
//		if (out_dt)
//			*out_dt = f_unk1;
//		if (out_target_ticks)
//			*out_target_ticks = i_unk1;
//		return;
//	}
//	if (cinimatic_in_progress())
//		i_unk3 = 1;
//	if(sub_9b9887())
//	{
//		sub_B1DCBB(&i_unk2);
//	}
//	else
//	{
//		int ticks_per_sec_multi = 8;
//		i_unk4 = time_globals::get_game_time_globals()->ticks_per_second * ticks_per_sec_multi;
//	}
//	if(!i_unk2 && !i_unk3)
//	{
//		if(time_globals::get_game_time_globals()->game_speed >= 1.0f)
//		{
//			if (time_game_speed <= 5.0f)
//				valid_game_speed = time_globals::get_game_time_globals()->game_speed;
//			else
//				valid_game_speed = 5.0f;
//		}
//		else
//		{
//			valid_game_speed = 1.0f;
//		}
//		f_unk3 = valid_game_speed * 8;
//		if(i_unk4 > (int)f_unk3)
//		{
//			i_unk4 = f_unk3;
//		}
//	}
//	f_unk4 = time_globals::get_game_time_globals()->game_speed * dt;
//	valid_game_speed = (time_globals::get_game_time_globals()->ticks_per_second * f_unk4) + time_globals::get_game_time_globals()->field_10;
//	f_unk1 = f_unk4;
//	addDebugText(IntToString<float>(valid_game_speed, std::dec).c_str());
//	i_unk1 = floor(valid_game_speed + 0.000099999997);
//	if(cinimatic_is_running() && get_tickrate() > 30)
//	{
//		if(t_is_init_flag_set(unk21))
//		{
//			if (i_unk1 > 2)
//				i_unk1 = 2;
//			f_unk5 = valid_game_speed;
//			if (valid_game_speed > 2.0)
//				f_unk5 = 2.0f;
//		}
//		else
//		{
//			f_unk5 = valid_game_speed;
//		}
//		if(i_unk3 > 0)
//		{
//			i_unk5 = time_globals::get_game_time_globals()->tick_count;
//			if ((i_unk1 + i_unk5) & 1)
//				i_unk1--;
//			if ((i_unk4 + i_unk5) & 1)
//				i_unk4--;
//		}
//	}
//	else if(t_is_init_flag_set(unk21))
//	{
//		f_unk5 = 1.0f;
//		i_unk1 = 1;
//	}
//	else
//	{
//		f_unk5 = valid_game_speed;
//	}
//	if(i_unk2)
//	{
//		if(i_unk1 <= i_unk4 && i_unk1 + 7 >= i_unk4 && i_unk1 +1 >= i_unk4 && i_unk1 + 1 <= i_unk4)
//		{
//			f_unk2 = f_unk5 - i_unk1;
//			f_unk6 = f_unk1;
//			time_globals::get_game_time_globals()->field_10 = f_unk2;
//			if (out_dt)
//				*out_dt = f_unk6;
//			if (out_target_ticks)
//				*out_target_ticks = i_unk1;
//			return;
//		}
//	}
//	else if( i_unk1 <= i_unk4)
//	{
//		f_unk2 = f_unk5 - i_unk1;
//		f_unk6 = f_unk1;
//		time_globals::get_game_time_globals()->field_10 = f_unk2;
//		if (out_dt)
//			*out_dt = f_unk6;
//		if (out_target_ticks)
//			*out_target_ticks = i_unk1;
//		return;
//	}
//	f_unk6 = i_unk4 * time_globals::get_game_time_globals()->seconds_per_tick;
//	i_unk1 = i_unk4;
//	tGlobals.field_10 = f_unk2;
//	if (out_dt)
//		*out_dt = f_unk6;
//	if (out_target_ticks)
//		*out_target_ticks = i_unk1;
//	return;
//}
//void __cdecl game_time_globals_prep(float dt, float *out_dt, int *out_target_ticks)
//{
//	signed int v3; // esi
//	time_globals *v4; // eax
//	float v5; // xmm1_4
//	float v6; // xmm0_4
//	char v7; // bl
//	signed int v8; // edi
//	time_globals *v9; // eax
//	signed int v10; // edi
//	float v11; // xmm0_4
//	float v12; // xmm1_4
//	float v13; // xmm0_4
//	int v14; // eax
//	float v15; // xmm0_4
//	byte v16; // [esp+13h] [ebp-11h]
//	float v17; // [esp+14h] [ebp-10h]
//	float valid_game_speed; // [esp+18h] [ebp-Ch]
//	float v19; // [esp+1Ch] [ebp-8h]
//	float v20; // [esp+20h] [ebp-4h]
//	float v21;
//
//	v17 = 0.0;
//	v3 = 0;
//	v19 = 0.0;
//	if (game_globals_unk1())
//	{
//		v4 = time_globals::get_game_time_globals();
//		v5 = v4->field_18;
//		if (v5 > 0.0)
//		{
//			v6 = v4->field_14 + dt;
//			v4->field_14 = v6;
//			if (v5 <= v6)
//			{
//				_sub_9EC02C(v4->field_20);
//				v4->field_18 = 0.0;
//			}
//			else
//			{
//				_sub_9EC02C(((1.0 - (v6 / v5)) * v4->field_1C) + (v4->field_20 * (v6 / v5)));
//			}
//		}
//	}
//	if (!game_globals_unk1() || v4->game_is_paused || v4->game_speed <= 0.0)
//		goto LABEL_47;
//	v16 = 0;
//	v7 = 0;
//	if (cinimatic_in_progress())
//		v7 = 1;
//	if (sub_9b9887())
//	{
//		v10 = sub_B1DCBB(&v16);
//		v9 = time_globals::get_game_time_globals();
//	}
//	else
//	{
//		v8 = 8;
//		v9 = time_globals::get_game_time_globals();
//		v10 = v9->ticks_per_second * v8;
//	}
//	if (!v16 && !v7)
//	{
//		v11 = v9->game_speed;
//		if (v11 >= 1.0)
//		{
//			if (v11 <= 5.0)
//				valid_game_speed = v9->game_speed;
//			else
//				valid_game_speed = 5.0;
//		}
//		else
//		{
//			valid_game_speed = 1.0;
//		}
//		v20 = 8 * valid_game_speed;
//		//FLODWORD(valid_game_speed) = v20;
//		if (v10 > (int)valid_game_speed)
//			v10 = v20;
//		v9 = time_globals::get_game_time_globals();
//	}
//	
//	v12 = v9->game_speed * dt;
//	valid_game_speed = (v9->ticks_per_second * v12) + v9->field_10;
//	v17 = v12;
//	v3 = //floor(valid_game_speed + 0.000099999997);
//		(((unsigned __int8)(static_cast<int>(valid_game_speed + 0.000099999997) >> 23) - 127) >> 31) & (((static_cast<int>(valid_game_speed + 0.000099999997) >> 31) ^ ((signed int)(static_cast<int>(valid_game_speed + 0.000099999997) & 0x7FFFFF | 0x800000) >> (-106 - (static_cast<int>(valid_game_speed + 0.000099999997) >> 23))))
//	-(static_cast<int>(valid_game_speed + 0.000099999997) >> 31));
//	//v3 = ~(())
//	/*
//	 *
//	 * ~(((unsigned __int8)(COERCE_SIGNED_INT(v29 + 0.000099999997) >> 23) - 127) >> 31) & 
//	 * (((COERCE_SIGNED_INT(v29 + 0.000099999997) >> 31) ^ ((signed int)(COERCE_UNSIGNED_INT(v29 + 0.000099999997) &
//	 *  0x7FFFFF | 0x800000) >> (-106 - (COERCE_SIGNED_INT(v29 + 0.000099999997) >> 23))))
//                                                                                           - (COERCE_SIGNED_INT(v29 + 0.000099999997) >> 31));
//	 */
//	if (cinimatic_is_running() && get_tickrate() > 30)
//	{
//		if (t_is_init_flag_set(unk21))
//		{
//			v21 = fminf(2.0, valid_game_speed);
//			if (v3 > 2)
//				v3 = 2;
//			v13 = v21;
//		}
//		else
//		{
//			v13 = valid_game_speed;
//		}
//		if (v3 > 0)
//		{
//			v14 = v9->tick_count;
//			if ((v3 + v14) & 1)
//				--v3;
//			if ((v10 + v14) & 1)
//				--v10;
//		}
//	}
//	else if (t_is_init_flag_set(unk21))
//	{
//		v13 = 1.0;
//		v3 = 1;
//	}
//	else
//	{
//		v13 = valid_game_speed;
//	}
//	if (v16)
//	{
//		if (v3 <= v10 && v3 + 7 >= v10 && v3 + 1 >= v10 && v3 + 1 <= v10)// this is lockstep netcode related
//			goto LABEL_46;
//	}
//	else if (v3 <= v10)
//	{
//	LABEL_46:
//		float a = 0.89999998;
//		float b = 0.15000001;
//		float c = (v3 - 1) * b + 1;
//		if (c >= a)
//			a = fminf(1.05, c);
//		v12 = v9->game_speed * a;
//		valid_game_speed = (v9->ticks_per_second * v12) + v9->field_10;
//		v17 = v12;
//		//v3 = //floor(valid_game_speed + 0.000099999997);
//		//	(((unsigned __int8)(static_cast<int>(valid_game_speed + 0.000099999997) >> 23) - 127) >> 31) & (((static_cast<int>(valid_game_speed + 0.000099999997) >> 31) ^ ((signed int)(static_cast<int>(valid_game_speed + 0.000099999997) & 0x7FFFFF | 0x800000) >> (-106 - (static_cast<int>(valid_game_speed + 0.000099999997) >> 23))))
//		//		- (static_cast<int>(valid_game_speed + 0.000099999997) >> 31));
//
//		v19 = v13 - v3;
//	LABEL_47:
//		v15 = v17;
//		goto LABEL_48;
//	}
//
//	v15 = v10 * v9->seconds_per_tick;
//	v3 = v10;
//LABEL_48:
//
//	v9->field_10 = v19;
//	addDebugText(IntToString<float>(v15, std::dec).c_str());
//	addDebugText(IntToString<int>(v3, std::dec).c_str());
//	if (out_dt)
//		*out_dt = v15;
//	if (out_target_ticks)
//		*out_target_ticks = v3;
//}

void __cdecl game_time_globals_prep(float dt, float *out_dt, int *out_target_ticks)
{
	int *v4; // r14
	float *v5; // r15
	float v6; // xmm6_4
	float v7; // xmm7_4
	int v8; // edi
	float v9; // xmm12_4
	time_globals* v10; // rax
	float v11; // xmm2_4
	float v12; // xmm1_4
	int v13; // eax
	time_globals* v14; // rdx
	char v15; // di
	int v16; // eax
	time_globals* v17; // rcx
	int v18; // ebx
	int v19; // eax
	float v20; // xmm0_4
	float v21; // xmm7_4
	float v22; // xmm0_4
	float v23; // xmm0_4
	float valid_game_speed; // xmm9_4
	float v25; // xmm0_4
	int v26; // er8
	float v27; // xmm1_4
	float v28; // xmm0_4
	float valid_game_speed_2; // xmm6_4
	int v30; // esi
	float v31; // xmm10_4
	int v32; // ecx
	byte v34; // [rsp+B8h] [rbp+10h]
	float v35; // [rsp+C0h] [rbp+18h]

	v4 = out_target_ticks;
	v5 = out_dt;
	v6 = 0.0;
	v7 = 0.0;
	v8 = 0;
	v9 = dt;
	if (game_globals_unk1())
	{
		v10 = time_globals::get_game_time_globals();
		v11 = v10->field_18;
		if (v11 > 0.0)
		{
			v12 = v9 + v10->field_10;
			v10->field_10 = v12;
			if (v11 <= v12)
			{
				_sub_9EC02C(v10->field_20);
				v10->field_18 = 0;
			}
			else
			{
				_sub_9EC02C(
					(float)((float)(1.0 - (float)(v12 / v11)) * *(float *)(v10 + 28))
					+ (float)((float)(v12 / v11) * *(float *)(v10 + 32)));
			}
		}
	}
		v14 = time_globals::get_game_time_globals();
	if (!(game_globals_unk1()
		|| v14->initialized && v14->game_is_paused
		|| v14->game_speed <= 0.0))
	{
		goto LABEL_53;
	}
	v34 = 0;
	v15 = 0;
	if (cinimatic_in_progress())
		v15 = 1;
	if (sub_9b9887())
	{
		v19 = sub_B1DCBB(&v34);
		v17 = time_globals::get_game_time_globals();
		v18 = v19;
	}
	else
	{
		v16 = 8;
		v17 = time_globals::get_game_time_globals();
		v18 = v17->ticks_per_second * v16;
	}
	if (!v34 && !v15)
	{
		v20 = *(float *)(v17 + 12);
		v21 = v20 >= 1.0 ? fminf(5.0, v20) : 1.0;
		v22 = 8 * v21;
		v23 = v22 < 0.0 ? v22 - 0.5 : v22 + 0.5;
		if (v18 > (signed int)v23)
			v18 = (signed int)v23;
	}
	if (!sub_9b9887())
	{
		v13 = 3;
		if (v18 < 3)
			v13 = v18;
		v18 = v13;
	}
	if (!v34)
	{
		v7 = v9 * v17->game_speed;
		valid_game_speed = (float)((float)v17->ticks_per_second * v7)
			+ v17->field_10;
		v35 = valid_game_speed + 0.000099999997;
		v8 = fminf(floor(v35), 1);
			(((unsigned __int8)((signed int)(valid_game_speed + 0.000099999997f) >> 23) - 127) >> 31) & 
				((((signed int)(valid_game_speed + 0.000099999997f) >> 31) ^ ((signed int)((signed int)(valid_game_speed + 0.000099999997f) & 0x7FFFFF | 0x800000) >> (-106 - ((signed int)(valid_game_speed + 0.000099999997f) >> 23)))) - ((signed int)(valid_game_speed + 0.000099999997f) >> 31));
		if (cinimatic_is_running() && get_tickrate() > 30)
		{
			if (t_is_init_flag_set(unk21))
			{
				v25 = fminf(2.0, valid_game_speed);
				if (v8 > 2)
					v8 = 2;
				valid_game_speed = v25;
			}
			if (v8 > 0)
			{
				v14 = time_globals::get_game_time_globals();
				v26 = v14->tick_count;
				v13 = v8 - 1;
				if (!(((BYTE)v26 + (BYTE)v8) & 1))
					v13 = v8;
				v8 = v13;
				if (v18 > 0)
				{
					v13 = v26 + v18;
					if (((BYTE)v26 + (BYTE)v18) & 1)
						--v18;
				}
			LABEL_46:
				if (!v34)
				{
					if (v8 <= v18)
					{
						v6 = valid_game_speed - (float)v8;
						goto LABEL_53;
					}
					goto LABEL_52;
				}
				goto LABEL_50;
			}
		}
		else
		{
			if ((t_is_init_flag_set(unk21)))
			{
				v8 = 1;
				valid_game_speed = 1.0;
			}
		}
		v14 = time_globals::get_game_time_globals();
		goto LABEL_46;
	}
	v14 = time_globals::get_game_time_globals();
LABEL_50:
	if (v18 <= 0)
	{
		v6 = 1.0;
	LABEL_52:
		v8 = v18;
		v7 = (float)v18 * *(float *)(v14 + 4);
		goto LABEL_53;
	}
	if (v18 >= 20)
		goto LABEL_52;
	v27 = 0.89999998;
	v28 = (float)((float)(v18 - 1) * 0.15000001 + 1.0);
	if (v28 >= 0.89999998)
		v27 = fminf(1.05, v28);
	v7 = (float)(v9 * *(float *)(v14 + 12)) * v27;
	valid_game_speed_2 = (float)((float)*(signed __int16 *)(v14 + 2) * v7) + *(float *)(v14 + 16);
	v35 = valid_game_speed_2 + 0.000099999997;
	//v30 = fminf(floor(v35), 1);
		//(((unsigned __int8)(static_cast< signed int>(valid_game_speed_2 + 0.000099999997f) >> 23) - 127) >> 31) & (((static_cast<signed int>(valid_game_speed_2 + 0.000099999997f) >> 31) ^ ((signed int)(static_cast<signed int>(valid_game_speed_2 + 0.000099999997f) & 0x7FFFFF | 0x800000) >> (-106 - (static_cast<signed int>(valid_game_speed_2 + 0.000099999997f) >> 23)))) - (static_cast<signed int>(valid_game_speed_2 + 0.000099999997f) >> 31));
	auto a = (static_cast<signed int>(valid_game_speed_2 + 0.000099999997f) >> 23);
	auto b = ((unsigned __int8)a - 127);
	auto c = (static_cast<signed int>(valid_game_speed_2 + 0.000099999997f) >> 31);

	auto d = (signed int)(static_cast<signed int>(valid_game_speed_2 + 0.000099999997f) & 0x7FFFFF | 0x800000);
	auto e = (static_cast<signed int>(valid_game_speed_2 + 0.000099999997f) >> 23);
	auto f = (static_cast<signed int>(valid_game_speed_2 + 0.000099999997f) >> 31);
	v30 = (b >> 31) & ((c ^ (d >> (-106 - e))) - f);
	if (!cinimatic_is_running())
		goto LABEL_81;
	v13 = get_tickrate();
	if (v13 <= 30)
		goto LABEL_81;
	
	if (t_is_init_flag_set(unk21))
	{
		v31 = fminf(2.0, valid_game_speed_2);
		if (v30 > 2)
			v30 = 2;
		valid_game_speed_2 = v31;
	}
	if (v30 > 0)
	{
		v14 = time_globals::get_game_time_globals();
		v32 = v14->tick_count;
		v13 = v30 - 1;
		if (!(((BYTE)v32 + (BYTE)v30) & 1))
			v13 = v30;
		v30 = v13;
		v13 = v32 + v18;
		if (((BYTE)v32 + (BYTE)v18) & 1)
			--v18;
	}
	else
	{
	LABEL_81:
		v14 = time_globals::get_game_time_globals();
	}
	v6 = valid_game_speed_2 - (float)v30;
	if (v30 > v18)
		v6 = 1.0;
	v8 = v18;
	if (v30 <= v18)
		v8 = v30;
LABEL_53:
	
	*(float *)(v14 + 16) = abs(valid_game_speed - (float)v8);
	addDebugText(IntToString<float>(v7, std::dec).c_str());
	addDebugText(IntToString<float>(v8, std::dec).c_str());
	addDebugText(IntToString<float>(v6, std::dec).c_str());
	if (v5)
		*v5 = abs(v7);
	if (v4)
		*v4 = abs(v8);
	//*(float *)(v14 + 40) = (float)v8;
}


void TestGameTimePrep::Init()
{
	game_globals_unk1 = (char(*)())h2mod->GetAddress(0x497EA);
	_sub_9EC02C = h2mod->GetAddress<sub_9EC02C*>(0x7c02c);
	cinimatic_in_progress = h2mod->GetAddress<c_cinematic_in_progress*>(0x3a928);
	sub_9b9887 = h2mod->GetAddress<c_sub_9b9887*>(0x49887);
	sub_B1DCBB = h2mod->GetAddress<c_sub_B1DCBB*>(0x1adcbb);
	cinimatic_is_running = h2mod->GetAddress<c_cinematic_is_running*>(0x3a938);
	get_tickrate = h2mod->GetAddress<c_get_tickrate*>(0x28707);

	//PatchCall(h2mod->GetAddress(0x39d04), game_time_globals_prep);
}
