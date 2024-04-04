#include "stdafx.h"
#include "main_game_time.h"

#include "game/game.h"
#include "game/game_time.h"
#include "math/math.h"
#include "shell/shell_windows.h"
#include "game/player_control.h"

#include "H2MOD/Utils/Utils.h"


extern bool g_xbox_tickrate_enabled;

bool g_main_game_time_frame_limiter_enabled = false;

// if this is enabled, the tick count to be executed will be calculated the same way as in Halo 1/CE
#define USE_HALO_1_TARGET_TICK_COUNT_COMPUTE_CODE 0

float get_ticks_leftover_time()
{
	time_globals* time_globals = time_globals::get();
	float result = time_globals->tick_length - (float)(time_globals->game_ticks_leftover / (float)time_globals->ticks_per_second);
	return MAX(result, 0.0f);
}

#if USE_HALO_1_TARGET_TICK_COUNT_COMPUTE_CODE
static float accumulator = 0.0f;
#endif // USE_HALO_1_TARGET_TICK_COUNT_COMPUTE_CODE

void __cdecl compute_target_tick_count(float dt, float* out_time_delta, int* out_target_tick_count)
{
	typedef void(__cdecl* compute_target_tick_count_t)(float, float*, int*);
	auto p_compute_target_tick_count = Memory::GetAddress<compute_target_tick_count_t>(0x7C1BF);

	typedef bool(__cdecl* game_is_not_paused_t)();
	auto p_game_is_not_paused = Memory::GetAddress<game_is_not_paused_t>(0x497EA);

	p_compute_target_tick_count(dt, out_time_delta, out_target_tick_count);

#if USE_HALO_1_TARGET_TICK_COUNT_COMPUTE_CODE
	time_globals* time_globals = time_globals::get();
	if (p_game_is_not_paused() && !time_globals->game_is_paused && time_globals->game_speed > 0.f)
	{
		float game_speed_in_ticks = time_globals->game_speed * time_globals->ticks_per_second;
		double unk = accumulator + dt; // in seconds
		double unk_seconds = floor(unk * game_speed_in_ticks);

		double tick_count = 0.0;

		if (unk_seconds > 1000.0)
			tick_count = 1000.0;
		else
			tick_count = (float)unk_seconds;

		*out_time_delta = tick_count * time_globals->seconds_per_tick;
		*out_target_tick_count = (int)tick_count;

		double unk4 = ((float)unk - (float)unk_seconds / game_speed_in_ticks);
		//LOG_TRACE_GAME("compute_target_tick_count() - time_delta: {}, output_time_delta: {}, target_tick_count: {}, unk: {}, unk4: {}, field_1C: {}", dt, *a2, *out_target_tick_count, unk, unk4, field_1C);

		accumulator = unk4;
		if (unk4 < 0.0)
			accumulator = 0.0f;
	}
	else
	{
		*out_time_delta = 0.f;
		*out_target_tick_count = 0;
	}
#endif // USE_HALO_1_TARGET_TICK_COUNT_COMPUTE_CODE
}

uint64 __cdecl main_time_get_absolute_milliseconds()
{
	uint64 milliseconds = system_milliseconds();
	s_main_time_globals* main_time_globals = s_main_time_globals::get();

	// copy the high part
	milliseconds |= (main_time_globals->last_milliseconds >> 32) << 32;

	if (milliseconds < main_time_globals->last_milliseconds)
	{
		// adjust the time, a complete cycle of system_milliseconds has passed
		milliseconds += UINT_MAX + 1llu;
	}

	ASSERT(milliseconds >= main_time_globals->last_milliseconds);
	return milliseconds;
}

real32 __cdecl main_time_update_hook(bool fixed_time_step, real32 fixed_time_delta)
{
	typedef real32(__cdecl* main_time_update_t)(bool, real32);
	auto p_main_time_update = Memory::GetAddress<main_time_update_t>(0x28814);

	typedef void(__cdecl* shell_update_t)();
	auto p_shell_update = Memory::GetAddress<shell_update_t>(0x7902);

	int game_time;
	real32 dt_sec = 0.0f;
	uint64 time_now_msec;

	s_main_time_globals* main_time_globals = s_main_time_globals::get();
	game_time = game_in_progress() ? time_globals::get_game_time() : 0;

	// TranslateMessage()
	// TODO move to function and cleanup
	p_shell_update();

	if (fixed_time_step)
	{
		dt_sec = fixed_time_delta;
		if (time_globals::available())
			time_globals::get()->game_ticks_leftover = 0.0f;

		time_now_msec = main_time_globals->last_milliseconds + (long long)(fixed_time_delta * 1000.0);
	}
	else
	{
		time_now_msec = main_time_get_absolute_milliseconds();
		dt_sec = (double)(time_now_msec - main_time_globals->last_milliseconds) / 1000.;

		// don't run the frame limiter when time step is fixed, because the code doesn't support it
		// in case of fixed time step, frame limiter should be handled by the other frame limiter
		if (g_main_game_time_frame_limiter_enabled || game_is_minimized())
		{
			if (time_globals::available())
			{
				// if there's game tick leftover time (i.e the actual game tick update executed faster than the actual engine's fixed time step)
				// limit the framerate to get back in sync with the renderer to prevent ghosting and jagged movement
				while (get_ticks_leftover_time() > dt_sec)
				{
					uint32 yield_time_msec = 0;
					real32 fMsSleep = (real32)(get_ticks_leftover_time() - dt_sec) * 1000.f;

					if ((int)fMsSleep > 0)
					{
						yield_time_msec = (int)fMsSleep;
					}

					// TODO FIXME to reduce stuttering, spend some of the time to sleep by CPU spinning,
					// Sleep is not precise since Windows is not a RTOS
					Sleep(yield_time_msec);

					time_now_msec = main_time_get_absolute_milliseconds();
					dt_sec = (double)(time_now_msec - main_time_globals->last_milliseconds) / 1000.;
				}
			}
			else
			{
				Sleep(15u);

				time_now_msec = main_time_get_absolute_milliseconds();
				dt_sec = (double)(time_now_msec - main_time_globals->last_milliseconds) / 1000.;
			}
		}
	}

	dt_sec = PIN(dt_sec, 0.0f, 10.f);
	main_time_globals->last_milliseconds = time_now_msec;
	main_time_globals->previous_game_time = game_time;
	main_time_globals->field_16[0] = *Memory::GetAddress<__int64*>(0xA3E440);
	main_time_globals->field_16[1] = *Memory::GetAddress<__int64*>(0xA3E440);

	player_control_update_dt(dt_sec);

	//LOG_TRACE_GAME("main_time_update() - timeDeltaSeconds: {}", timeDeltaSeconds);
	return dt_sec;
}

void main_game_time_apply_patches()
{
	if (!Memory::IsDedicatedServer())
	{
		//NopFill(Memory::GetAddress(0x2728E7), 5);

		PatchCall(Memory::GetAddress(0x39BE3), main_time_update_hook);
		PatchCall(Memory::GetAddress(0x39C0D), main_time_update_hook);

#if USE_HALO_1_TARGET_TICK_COUNT_COMPUTE_CODE
		PatchCall(Memory::GetAddress(0x39D04), compute_target_tick_count);
#endif // USE_HALO_1_TARGET_TICK_COUNT_COMPUTE_CODE

		//NopFill(Memory::GetAddress(0x39BDA), 2);
		//NopFill(Memory::GetAddress(0x39DF0), 8);

		//NopFill(Memory::GetAddress(0x39DE1), 5);
	}
}

