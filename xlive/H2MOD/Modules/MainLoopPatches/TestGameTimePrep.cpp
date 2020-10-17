#include "TestGameTimePrep.h"
#include "H2MOD/Modules/Config/Config.h"
#include "UncappedFPS/UncappedFPS.h"
#include <iostream>
#include "Util/Hooks/Hook.h"
#include "H2MOD/Modules/OnScreenDebug/OnScreenDebug.h"
#include "H2MOD/Modules/Utils/Utils.h"

#define FLODWORD(a)      (((float)(a)) & 0x00000000FFFFFFFF)
//#define LAST_IND(x,part_type)    (sizeof(x)/sizeof(part_type) - 1)
//#if defined(__BYTE_ORDER) && __BYTE_ORDER == __BIG_ENDIAN
//#  define LOW_IND(x,part_type)   LAST_IND(x,part_type)
//#  define HIGH_IND(x,part_type)  0
//#else
//#  define HIGH_IND(x,part_type)  LAST_IND(x,part_type)
//#  define LOW_IND(x,part_type)   0
//#endif
//#define BYTEn(x, n)   (*((BYTE*)&(x)+n))
//#define WORDn(x, n)   (*((WORD*)&(x)+n))
//#define DWORDn(x, n)  (*((DWORD*)&(x)+n))
//#define LOBYTE(x)  BYTEn(x,LOW_IND(x,BYTE))
//#define LOWORD(x)  WORDn(x,LOW_IND(x,WORD))
//#define LODWORD(x) DWORDn(x,LOW_IND(x,DWORD))
//#define HIBYTE(x)  BYTEn(x,HIGH_IND(x,BYTE))
//#define HIWORD(x)  WORDn(x,HIGH_IND(x,WORD))
//#define HIDWORD(x) DWORDn(x,HIGH_IND(x,DWORD))
//typedef unsigned int uint;
//typedef unsigned char uchar;
//typedef unsigned short ushort;
//typedef unsigned long ulong;
//
//typedef          char   int8;
//typedef   signed char   sint8;
//typedef unsigned char   uint8;
//typedef          short  int16;
//typedef   signed short  sint16;
//typedef unsigned short  uint16;
//typedef          int    int32;
//typedef   signed int    sint32;
//typedef unsigned int    uint32;
//typedef long long       int64;
//typedef long long       sint64;
//typedef unsigned long long uint64;
//template<class T> T __ROL__(T value, int count)
//{
//	const uint nbits = sizeof(T) * 8;
//
//	if (count > 0)
//	{
//		count %= nbits;
//		T high = value >> (nbits - count);
//		if (T(-1) < 0) // signed value
//			high &= ~((T(-1) << count));
//		value <<= count;
//		value |= high;
//	}
//	else
//	{
//		count = -count % nbits;
//		T low = value << (nbits - count);
//		value >>= count;
//		value |= low;
//	}
//	return value;
//}
//
//inline uint8  __ROL1__(uint8  value, int count) { return __ROL__((uint8)value, count); }
//inline uint16 __ROL2__(uint16 value, int count) { return __ROL__((uint16)value, count); }
//inline uint32 __ROL4__(uint32 value, int count) { return __ROL__((uint32)value, count); }
//inline uint64 __ROL8__(uint64 value, int count) { return __ROL__((uint64)value, count); }
//inline uint8  __ROR1__(uint8  value, int count) { return __ROL__((uint8)value, -count); }
//inline uint16 __ROR2__(uint16 value, int count) { return __ROL__((uint16)value, -count); }
//inline uint32 __ROR4__(uint32 value, int count) { return __ROL__((uint32)value, -count); }
//inline uint64 __ROR8__(uint64 value, int count) { return __ROL__((uint64)value, -count); }
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
int ftoi(float in)
{
	return *reinterpret_cast<int*>(&in);
}
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
		//v8 = fminf(floor(v35), 1);
		int fi = ftoi(valid_game_speed + 0.000099999997);
		v8 = ~(((unsigned __int8)(fi >> 23) - 127) >> 31) & (((fi >> 31) ^ ((signed int)(fi & 0x7FFFFF | 0x800000) >> (-106 - (fi >> 23)))) - (fi >> 31));
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
	int fi = ftoi(valid_game_speed + 0.000099999997);
	v30 = ~(((unsigned __int8)(fi >> 23) - 127) >> 31) & (((fi >> 31) ^ ((signed int)(fi & 0x7FFFFF | 0x800000) >> (-106 - (fi >> 23)))) - (fi >> 31));
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
	
	*(float *)(v14 + 16) = v6;
	addDebugText(IntToString<float>(v7, std::dec).c_str());
	addDebugText(IntToString<float>(v8, std::dec).c_str());
	addDebugText(IntToString<float>(v6, std::dec).c_str());
	if (v5)
		*v5 = v7;
	if (v4)
		*v4 = v8;
	*(float *)(v14 + 40) = (float)v8;
}

typedef void(__cdecl c_simulation_update_result)(int target_ticks, int executed_ticks, float* a3);
c_simulation_update_result* p_simulation_update_result;

void __cdecl simulation_update_result(int target_ticks, int executed_ticks, float* a3)
{
	if(executed_ticks)
	{
		auto time_globals = time_globals::get_game_time_globals();
		*a3 = fmax(*a3 - (target_ticks - executed_ticks) * time_globals->seconds_per_tick, 0.0);
	}
	else
	{
		*a3 = 0;
	}
}

typedef int(c_get_game_mode_engine)();
c_get_game_mode_engine* get_game_mode_engine;

//70FAF
//typedef char(c_sub_9E0FAF)();
//c_sub_9E0FAF* sub_9E0FAF;
////D0FF4
//typedef void(c_sub_A40FF4)();
//c_sub_A40FF4* sub_A40FF4;
////6C802
//typedef char(c_garbage_collect_multiplayer)();
//c_garbage_collect_multiplayer* garbage_collect_multiplayer;
////7069F
//typedef char(__thiscall c_sub_9E069F)(DWORD* _this);
//c_sub_9E069F* sub_9E069F;
////6EDF6
//typedef __int16*(c_sub_9DEDF6)();
//c_sub_9DEDF6* sub_9DEDF6;
////498B7
//typedef char(c_simulation_is_predicting)();
//c_simulation_is_predicting* simulation_is_predicting;
////5E17E
//typedef char*(c_sub_9CE17E)();
//c_sub_9CE17E* sub_9CE17E;
////5CA50
//typedef void(__thiscall c_initialize_game_players_iterator)(char* _this);
//c_initialize_game_players_iterator* initialize_game_players_iterator;
////5B0E2
//typedef bool(__thiscall c_player_iterator)(char* _this);
//c_player_iterator* player_iterator;
////5B11C
//typedef int(__thiscall c_player_iterator_get_current_index)(char* _this);
//c_player_iterator_get_current_index* player_iterator_get_current_index;

DWORD* game_engine_globals = h2mod->GetAddress<DWORD*>(0x4BF8F8);

//__int64 sub_1806BBE50()
//{
//    #pragma region fuck_off
//	__int64 result; // rax
//	__int64 v1; // rcx
//	signed int v2; // er15
//	unsigned int v3; // ebx
//	__int64 v4; // rdx
//	__int64 v5; // rcx
//	__int64 v6; // rdx
//	__int64 v7; // rcx
//	__int64 v8; // rcx
//	__int64 v9; // rdx
//	int v10; // er8
//	__int64 v11; // rcx
//	__int64 v12; // rdx
//	unsigned int v13; // ebx
//	__int64 v14; // rax
//	__int64 v15; // rcx
//	__int64 v16; // rdx
//	unsigned int v17; // eax
//	unsigned int v18; // eax
//	unsigned __int16 v19; // ax
//	__int64 v20; // rdx
//	__int64 v21; // rdx
//	signed int v22; // edi
//	signed __int64 v23; // rbx
//	float v24; // xmm1_4
//	float v25; // xmm1_4
//	signed int v26; // esi
//	float v27; // xmm1_4
//	float v28; // xmm1_4
//	__int16 v29; // ax
//	__int16 v30; // cx
//	int v31; // eax
//	int v32; // ecx
//	__int64 *v33; // rbx
//	__int64 v34; // rdi
//	unsigned int v35; // eax
//	unsigned __int16 v36; // ax
//	BYTE *v37; // rdi
//	unsigned __int16 v38; // r14
//	signed __int64 v39; // rbx
//	char v40; // al
//	char v41; // si
//	bool v42; // zf
//	char v43; // al
//	char v44; // al
//	char v45; // al
//	__int16 v46; // ax
//	__int16 v47; // ax
//	__int16 v48; // cx
//	__int16 v49; // ax
//	__int64 v50; // rdx
//	__int64 v51; // rcx
//	unsigned int v52; // edi
//	signed int v53; // esi
//	__int64 v54; // rdx
//	__int64 v55; // rcx
//	int v56; // ebx
//	__int64 v57; // rax
//	unsigned int i; // ebx
//	unsigned int v59; // eax
//	int v60; // er10
//	__int64 v61; // r11
//	__int64 v62; // rsi
//	__int64 v63; // rcx
//	__int64 v64; // rax
//	__int64 v65; // rax
//	int v66; // er9
//	__int64 v67; // r8
//	__int64 v68; // rdi
//	__int64 v69; // rdx
//	__int64 v70; // rcx
//	__int64 v71; // rax
//	__int64 v72; // rax
//	signed int v73; // ebx
//	signed int v74; // ebx
//	unsigned int v75; // edi
//	__int64 v76; // rsi
//	char v77; // bl
//	__int16 v78; // dx
//	__int16 v79; // dx
//	__int64 v80; // rax
//	WORD *v81; // rcx
//	bool v82; // al
//	unsigned int v83; // edi
//	unsigned int v84; // esi
//	__int64 v85; // rcx
//	__int64 v86; // rdx
//	signed __int64 v87; // rbx
//	int v88; // eax
//	BYTE *v89; // rdx
//	int v90; // ebx
//	int v91; // eax
//	signed int v92; // ebx
//	char v93; // [rsp+20h] [rbp-49h]
//	unsigned __int8 v94; // [rsp+D0h] [rbp+67h]
//	unsigned int v95; // [rsp+D8h] [rbp+6Fh]
//#pragma endregion
//	result = get_game_mode_engine();
//	if (result)
//	{
//		sub_9E0FAF();
//		sub_A40FF4();
//		garbage_collect_multiplayer();
//		sub_9E069F(game_engine_globals + 0x207);
//		sub_9DEDF6();
//		if (!simulation_is_predicting())
//			sub_9CE17E();
//		v2 = 0;
//		initialize_game_players_iterator(&v93);
//		if ((unsigned __int8)player_iterator(&v93))
//		{
//			while (1)
//			{
//				++v2;
//				v3 = player_iterator_get_current_index(&v93);
//				if (get_game_mode_engine()
//					&& v3 != -1
//					&& (*(DWORD *)(get_game_variant(v5, v4) + 68) != 7
//						|| !(unsigned __int8)sub_180763530(v3)
//						|| !(*(BYTE *)(get_game_variant(v7, v6) + 240) & 2))
//					&& *(DWORD *)(get_game_variant(v7, v6) + 136) == 1)
//				{
//					v8 = *(long long*)(players + 72);
//					v9 = v8 + players;
//					if (!v8)
//						v9 = 0i64;
//					v10 = *(DWORD *)(532i64 * (unsigned __int16)v3 + v9 + 44);
//					if (v10 != -1)
//					{
//						v11 = *(long long *)(objects + 72);
//						v12 = v11 + objects;
//						if (!v11)
//							v12 = 0i64;
//						*(DWORD *)(sub_1808DCFD0(v12 + 12i64 * (unsigned __int16)v10) + 240) = 0;
//					}
//				}
//				v13 = player_iterator_get_current_index(&v93);
//				if (get_game_mode_engine())
//				{
//					if (get_game_mode_engine())
//					{
//						v14 = get_game_mode_engine();
//						if ((*(unsigned __int8(__fastcall **)(__int64, long long, signed __int64))(*(WORD *)v14 + 280i64))(
//							v14,
//							v13,
//							1i64))
//						{
//							v15 = *(long long *)(players + 72);
//							v16 = v15 + players;
//							if (!v15)
//								v16 = 0i64;
//							if (*(DWORD *)(532i64 * (unsigned __int16)v13 + v16 + 44) != -1)
//							{
//								v17 = time_globals_get_ticks_from_seconds_impercise(0.5);
//								sub_1806C2390(v13, 0i64, v17);
//							}
//						}
//					}
//				}
//				v18 = player_iterator_get_current_index(&v93);
//				sub_1806BCDE0(v18);
//				v19 = player_iterator_get_current_index(&v93);
//				v20 = *(long long *)(players + 72);
//				if (v20)
//					v21 = players + v20;
//				else
//					v21 = 0i64;
//				v22 = -1;
//				v23 = v21 + 532i64 * v19;
//				v24 = 256.0 / sub_18071B950(0.25);
//				if (v24 < 0.0)
//					v25 = v24 - 0.5;
//				else
//					v25 = v24 + 0.5;
//				v26 = (signed int)v25;
//				v27 = 256.0 / sub_18071B950(1.0);
//				if (v27 < 0.0)
//					v28 = v27 - 0.5;
//				else
//					v28 = v27 + 0.5;
//				v29 = *(WORD *)(v23 + 40);
//				if (v29 != -1)
//					v22 = sub_18070B950((unsigned int)v29);
//				v30 = *(WORD *)(v23 + 414);
//				if (*(DWORD *)(v23 + 404) == v22)
//				{
//					v32 = v26 + v30;
//					if (v32 > 256)
//						LOWORD(v32) = 256;
//					*(WORD *)(v23 + 414) = v32;
//					goto LABEL_50;
//				}
//				if (v30 > 0 && v22 == -1)
//					break;
//				LOWORD(v31) = *(WORD *)(v23 + 414);
//				if (v30 > 0 && v22 != -1)
//				{
//					v31 = v30 - v26;
//					goto LABEL_42;
//				}
//			LABEL_45:
//				if (!(WORD)v31)
//					*(DWORD *)(v23 + 404) = v22;
//			LABEL_50:
//				v33 = (__int64 *)get_game_mode_engine();
//				v34 = *v33;
//				v35 = player_iterator_get_current_index(&v93);
//				(*(void(__fastcall **)(__int64 *, long long))(v34 + 120))(v33, v35);
//				v36 = player_iterator_get_current_index(&v93);
//				v37 = game_engine_globals;
//				v38 = v36;
//				v39 = 3i64 * v36;
//				if (!simulation_is_predicting())
//				{
//					v40 = v37[8 * v39 + 2101];
//					v41 = v37[8 * v39 + 2100];
//					if (v40)
//					{
//						v43 = v40 - 1;
//						v42 = v43 == 0;
//						v37[8 * v39 + 2101] = v43;
//						v44 = v41;
//						if (v42)
//						{
//							v45 = v37[8 * v39 + 2102];
//							if (v45)
//							{
//								v37[8 * v39 + 2100] = v45;
//								v37[8 * v39 + 2101] = time_globals_get_ticks_from_seconds_impercise(0.5);
//								v44 = v37[8 * v39 + 2100];
//								v37[8 * v39 + 2102] = 0;
//							}
//							else
//							{
//								*(WORD *)&v37[8 * v39 + 2100] = 0;
//								v44 = 0;
//							}
//						}
//						if (v41 != v44)
//							sub_18088D680(v38, 4i64);
//					}
//				}
//				if (*(WORD *)&v37[8 * v39 + 2096])
//				{
//					v46 = *(WORD *)&v37[8 * v39 + 2098] - 1;
//					if (*(signed __int16 *)&v37[8 * v39 + 2098] - 1 <= 0)
//						v46 = 0;
//					*(WORD *)&v37[8 * v39 + 2098] = v46;
//					if (!v46)
//					{
//						v37[8 * v39 + 2080] = 0;
//						*(DWORD *)&v37[8 * v39 + 2096] = 0;
//					}
//				}
//				if (!(unsigned __int8)player_iterator(&v93))
//					goto LABEL_64;
//			}
//			v31 = v30 - (signed int)v28;
//		LABEL_42:
//			if (v31 <= 0)
//				LOWORD(v31) = 0;
//			*(WORD *)(v23 + 414) = v31;
//			goto LABEL_45;
//		}
//	LABEL_64:
//		*((DWORD *)game_engine_globals + 949) = v2 > 8;
//		v47 = *((WORD *)game_engine_globals + 1900);
//		v48 = v47 | 1;
//		v49 = v47 & 0xFFFE;
//		if (v2 <= 8)
//			v48 = v49;
//		*((WORD *)game_engine_globals + 1900) = v48;
//		if (get_game_mode_engine() && *(BYTE *)(get_game_variant(v51, v50) + 72) & 1)
//		{
//			v52 = 0;
//			v53 = 1;
//			do
//			{
//				if (get_game_mode_engine()
//					&& *(BYTE *)(get_game_variant(v55, v54) + 72) & 1
//					&& v52 <= 7
//					&& *((WORD *)game_engine_globals + 6) & (unsigned __int16)v53
//					&& !simulation_is_predicting()
//					&& get_game_mode_engine()
//					&& *((WORD *)game_engine_globals + 54) == 1
//					&& (simulation_is_predicting() || *((DWORD *)game_engine_globals + 947) == 1))
//				{
//					v56 = time_globals_get_passed_tick_count();
//					if (!(v56 % (signed int)time_globals_get_ticks_per_second()))
//					{
//						initialize_game_players_iterator((__int64)&v93);
//						if ((unsigned __int8)player_iterator(&v93))
//						{
//							while (*(char *)(sub_180043360(&v93) + 184) != v52 || *DWORD *)(sub_180043360(&v93) + 44) == -1)
//							{
//								if (!(unsigned __int8)player_iterator(&v93))
//									goto LABEL_85;
//							}
//							sub_1806ADB50(0xFFFFFFFFi64, v52, 13i64);
//						}
//					}
//				}
//			LABEL_85:
//				++v52;
//				v53 = __ROL4__(v53, 1);
//			} while ((signed int)v52 < 8);
//		}
//		v57 = get_game_mode_engine();
//		(*(void(__fastcall **)(__int64))(*(long long *)v57 + 144i64))(v57);
//		if (!simulation_is_predicting())
//		{
//			sub_1806BD340();
//			v95 = -1;
//			v94 = 0;
//			if (get_game_mode_engine()
//				&& *((WORD *)game_engine_globals + 54) == 1
//				&& (simulation_is_predicting() || *((long long *)game_engine_globals + 947) == 1)
//				&& (unsigned __int8)sub_1806BB530(&v95, &v94))
//			{
//				sub_1806B1630(v95, v94);
//			}
//			sub_180758E40();
//		}
//		for (i = get_first_local_player_index(); i != -1; i = next_local_player_index(i))
//		{
//			v59 = local_player_get_index(i);
//			if (v59 != -1)
//			{
//				v60 = sub_1806B3870(v59);
//				if (v60 != *((DWORD *)&unk_180D77180 + 3 * (signed int)i + 6))
//				{
//					v61 = qword_180D77740;
//					v62 = qword_180D77750;
//					v63 = *(signed int *)(tag_instances[LOWORD(time_game_globals[10].seconds_per_tick)].data_offset
//						+ tag_data_
//						+ 12);
//					v64 = 0i64;
//					if ((DWORD)v63 != -1)
//					{
//						if ((signed int)v63 >= 0)
//						{
//							v64 = qword_180D77740 + v63;
//						}
//						else
//						{
//							v65 = (unsigned int)v63;
//							LODWORD(v65) = v63 & 0x7FFFFFFF;
//							v64 = qword_180D77750 + v65;
//						}
//					}
//					*((DWORD *)&unk_180D77180 + 3 * (signed int)i + 6) = v60;
//					v66 = 0;
//					*((DWORD *)&unk_180D77180 + 3 * (signed int)i + 7) = 0;
//					v67 = 0i64;
//					*((DWORD *)&unk_1800D77180 + 3 * (signed int)i + 8) = -1;
//					if (*(DWORD *)(v64 + 1336) > 0)
//					{
//						v68 = *(signed int *)(v64 + 1336);
//						v69 = 0i64;
//						v70 = *(signed int *)(v64 + 1340);
//						while (1)
//						{
//							v71 = 0i64;
//							if ((DWORD)v70 != -1)
//							{
//								if ((signed int)v70 >= 0)
//								{
//									v71 = v61 + v70;
//								}
//								else
//								{
//									v72 = (unsigned int)v70;
//									LODWORD(v72) = v70 & 0x7FFFFFFF;
//									v71 = v62 + v72;
//								}
//							}
//							if (*(signed __int16 *)(v71 + v69 + 4) == v60)
//								break;
//							++v66;
//							++v67;
//							v69 += 28i64;
//							if (v67 >= v68)
//								goto LABEL_111;
//						}
//						*((DWORD *)&unk_180D77180 + 3 * (signed int)i + 8) = v66;
//					}
//				}
//			}
//		LABEL_111:
//			;
//		}
//		if (!simulation_is_predicting())
//		{
//			v73 = *((DWORD *)game_engine_globals + 616) + (unsigned __int64)sub_18071B900(3i64);
//			if ((signed int)time_globals_get_passed_tick_count() >= v73)
//			{
//				v74 = 0;
//				do
//					sub_18088D680((unsigned __int16)v74++, 16i64);
//				while (v74 < 16);
//				*((DWORD *)game_engine_globals + 616) = time_globals_get_passed_tick_count();
//			}
//		}
//		if (!simulation_is_predicting())
//		{
//			initialize_game_players_iterator((__int64)&v93);
//			while ((unsigned __int8)player_iterator(&v93))
//			{
//				v75 = player_iterator_get_current_index(&v93);
//				v76 = sub_180043360(&v93);
//				v77 = 0;
//				if ((unsigned __int8)sub_1806B6D30(v75) || (unsigned __int8)sub_1806B6F70(v75))
//					v77 = 1;
//				v78 = *(WORD *)(v76 + 6);
//				if (v77 != v78 < 0)
//				{
//					v79 = v78 | 0x8000;
//					if (!v77)
//						v79 = *(WORD *)(v76 + 6) & 0x7FFF;
//					*(WORD *)(v76 + 6) = v79;
//					sub_18088D680((unsigned __int16)v75, 1024i64);
//				}
//			}
//		}
//		v80 = get_game_mode_engine();
//		v81 = game_engine_globals;
//		if (v80
//			&& *((WORD *)game_engine_globals + 54) == 1
//			&& ((v82 = simulation_is_predicting(), v81 = game_engine_globals, v82)
//				|| *((DWORD *)game_engine_globals + 947) == 1))
//		{
//			*((WORD *)game_engine_globals + 125) = 0;
//		}
//		else
//		{
//			++v81[125];
//		}
//		result = get_first_local_player_index();
//		v83 = result;
//		while ((DWORD)result != -1)
//		{
//			v84 = local_player_get_index(v83);
//			if (v84 != -1)
//			{
//				v85 = *(long long *)(players + 72);
//				v86 = v85 + players;
//				if (!v85)
//					v86 = 0i64;
//				v87 = v86 + 532i64 * (unsigned __int16)v84;
//				v88 = time_globals_get_ticks_per_second();
//				v89 = game_engine_globals;
//				if (*((signed __int16 *)game_engine_globals + 125) <= 4 * v88)
//				{
//					if (*(DWORD *)(v87 + 44) != -1 || *(DWORD *)(v87 + 336) > 0)
//						goto LABEL_144;
//					if (!game_engine_player_has_lives(v84))
//					{
//						v89 = game_engine_globals;
//					LABEL_144:
//						if (!((unsigned __int8)(1 << v83) & v89[244]) && *(DWORD *)(v87 + 44) != -1)
//							goto LABEL_156;
//						if (*(DWORD *)(v87 + 44) == -1 && !game_engine_player_has_lives(v84))
//						{
//							v90 = (unsigned __int64)time_globals_get_passed_tick_count() - *((DWORD *)game_engine_globals + 28);
//							v91 = time_globals_get_ticks_per_second();
//							if (v90 < 0)
//								v90 = 0;
//							if (v90 > v91)
//							{
//							LABEL_156:
//								v92 = time_globals_get_ticks_from_seconds_impercise(1.5);
//								if ((signed int)time_globals_get_passed_tick_count() > v92)
//									*((float *)game_engine_globals + (signed int)v83 + 57) = fmaxf(
//										0.0,
//										*((float *)game_engine_globals
//											+ (signed int)v83
//											+ 57)
//										- (float)(3.0
//											/ (float)(signed int)time_globals_get_ticks_per_second()));
//							}
//						}
//						goto LABEL_153;
//					}
//					*((float *)game_engine_globals + (signed int)v83 + 57) = fminf(
//						(float)(1.0
//							/ (float)(signed int)time_globals_get_ticks_per_second())
//						+ *((float *)game_engine_globals
//							+ (signed int)v83
//							+ 57),
//						1.0);
//				}
//				else
//				{
//					*((float *)game_engine_globals + (signed int)v83 + 57) = fminf(
//						(float)(1.0
//							/ (float)(signed int)time_globals_get_ticks_per_second())
//						+ *((float *)game_engine_globals
//							+ (signed int)v83
//							+ 57),
//						1.0);
//				}
//			}
//		LABEL_153:
//			result = next_local_player_index(v83);
//			v83 = result;
//		}
//	}
//	return result;
//}

float __cdecl a_time_globals_get_seconds_per_tick()
{
	return 1.0f/10000.0f;
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

	p_simulation_update_result = h2mod->GetAddress<c_simulation_update_result*>(0x7C47A);

	get_game_mode_engine = h2mod->GetAddress<c_get_game_mode_engine*>(0x5B15E);
	//sub_9E0FAF = h2mod->GetAddress<c_sub_9E0FAF*>(0x70FAF);
	//sub_A40FF4 = h2mod->GetAddress<c_sub_A40FF4*>(0xD0FF4);
	//garbage_collect_multiplayer = h2mod->GetAddress<c_garbage_collect_multiplayer*>(0x6C802);
	//sub_9E069F = h2mod->GetAddress<c_sub_9E069F*>(0x7069F);
	//sub_9DEDF6 = h2mod->GetAddress<c_sub_9DEDF6*>(0x6EDF6);
	//simulation_is_predicting = h2mod->GetAddress<c_simulation_is_predicting*>(0x498B7);
	//sub_9CE17E = h2mod->GetAddress<c_sub_9CE17E*>(0x5E17E);
	//initialize_game_players_iterator = h2mod->GetAddress<c_initialize_game_players_iterator*>(0x5CA50);
	//player_iterator = h2mod->GetAddress<c_player_iterator*>(0x5B0E2);
	//player_iterator_get_current_index = h2mod->GetAddress<c_player_iterator_get_current_index*>(0x5B11C);

	PatchCall(h2mod->GetAddress(0x4A60B), simulation_update_result);
	PatchCall(h2mod->GetAddress(0x39d04), game_time_globals_prep);
	PatchCall(h2mod->GetAddress(0x22A8FE), a_time_globals_get_seconds_per_tick);
	PatchCall(h2mod->GetAddress(0x22A94D), a_time_globals_get_seconds_per_tick);
	PatchCall(h2mod->GetAddress(0x22A972), a_time_globals_get_seconds_per_tick);
	PatchCall(h2mod->GetAddress(0x22A9FA), a_time_globals_get_seconds_per_tick);
	PatchCall(h2mod->GetAddress(0x22AA47), a_time_globals_get_seconds_per_tick);
	PatchCall(h2mod->GetAddress(0x22AEAF), a_time_globals_get_seconds_per_tick);
	PatchCall(h2mod->GetAddress(0x22AF28), a_time_globals_get_seconds_per_tick);
}
