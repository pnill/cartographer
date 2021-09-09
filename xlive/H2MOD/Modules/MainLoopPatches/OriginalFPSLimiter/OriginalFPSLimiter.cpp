
#include "OriginalFPSLimiter.h"

#include "Util/Hooks/Hook.h"
#include "H2MOD/Modules/Config/Config.h"
#include "Blam/Engine/Game/GameTimeGlobals.h"


extern bool b_XboxTick;
static LARGE_INTEGER frequency;
static LARGE_INTEGER timeAtStartup;

// if this is enabled, the tick count to be executed will be calculated the same way as in Halo 1/CE
#define USE_HALO_1_TARGET_TICK_COUNT_COMPUTE_CODE 0

float get_remaining_time_until_next_tick_in_seconds()
{
	time_globals* timeGlobals = time_globals::get();
	float result = timeGlobals->seconds_per_tick - (float)(timeGlobals->update_time / (float)timeGlobals->ticks_per_second);
	return fmaxf(result, 0.f);
}

#if USE_HALO_1_TARGET_TICK_COUNT_COMPUTE_CODE
static float accumulator = 0.0f;
#endif // USE_HALO_1_TARGET_TICK_COUNT_COMPUTE_CODE

void __cdecl compute_target_tick_count(float dt, float* out_time_delta, int* out_target_tick_count)
{
	typedef void(__cdecl *compute_target_tick_count)(float, float*, int*);
	auto p_compute_target_tick_count = Memory::GetAddress<compute_target_tick_count>(0x7C1BF);

	typedef bool(__cdecl* game_is_not_paused)();
	auto p_unk_check = Memory::GetAddress<game_is_not_paused>(0x497EA);

	p_compute_target_tick_count(dt, out_time_delta, out_target_tick_count);

	/*if (*out_target_tick_count == 0)
	{
		LOG_TRACE_GAME("input_float: {}, dt: {}, target_tick_count: {}", dt, *a2, *out_target_tick_count);
	}*/

#if USE_HALO_1_TARGET_TICK_COUNT_COMPUTE_CODE
	time_globals* timeGlobals = time_globals::get();
	if (p_unk_check() && !timeGlobals->game_is_paused && timeGlobals->game_speed > 0.f)
	{
		float game_speed_in_ticks = timeGlobals->game_speed * timeGlobals->ticks_per_second;
		double unk = accumulator + dt; // in seconds
		double unk_seconds = floor(unk * game_speed_in_ticks);

		double tick_count = 0.0;

		if (unk_seconds > 1000.0)
			tick_count = 1000.0;
		else
			tick_count = (float)unk_seconds;

		*out_time_delta = tick_count * timeGlobals->seconds_per_tick;
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

DWORD WINAPI timeGetTime_hook()
{
	LARGE_INTEGER currentTime;
	QueryPerformanceCounter(&currentTime);
	float ret = (double)(currentTime.LowPart - timeAtStartup.LowPart) / (double)(int)frequency.LowPart;
	ret = ret * 1000.f;
	return (DWORD)ret;
}

static __int64 network_time;
__int64 get_time_delta_msec()
{
	return network_time;
}

static LARGE_INTEGER lastTime;
void __cdecl reset_time()
{
	QueryPerformanceCounter(&lastTime);
	network_time = (__int64)(((double)(lastTime.QuadPart - timeAtStartup.QuadPart) / (double)frequency.QuadPart) * 1000.0);
	*Memory::GetAddress<BYTE*>(0x479EA0) = (BYTE)1;
}

float __cdecl main_time_update(bool use_static_time_increase, float static_time_delta)
{
	typedef float(__cdecl* compute_time_delta_def)(bool, float);
	auto p_compute_time_delta_def = Memory::GetAddress<compute_time_delta_def>(0x28814);

	LARGE_INTEGER currentTime;
	float timeDeltaSeconds = 0.0f;
	if (H2Config_experimental_fps == e_render_original_game_frame_limit)
	{
		typedef void(__cdecl* translate_windows_messages)();
		auto p_translate_windows_messages = Memory::GetAddress<translate_windows_messages>(0x7902);

		p_translate_windows_messages(); // TranslateMessage()
		time_globals* timeGlobals = time_globals::get();
		QueryPerformanceCounter(&currentTime);
		timeDeltaSeconds = (double)(currentTime.LowPart - lastTime.LowPart) / (double)(int)frequency.LowPart;

		//if (Engine:IsGameMinimized())
		{
			if (timeGlobals && timeGlobals->initialized)
			{
				// what this code does is limit the FPS and makes sure every frame a tick has been executed, to prevent stale rendering (or without game state updated)
				// which causes stuttering and jagged movement due to fast execution time on newer PCs
				// TODO: in order to have real-time input and network updates, which is not a thing if we are forcing a fake time delta on each frame (basically what Hired Gun did to the game)
				// we have to add interpolation
				for (; get_remaining_time_until_next_tick_in_seconds() > timeDeltaSeconds; timeDeltaSeconds = (double)(currentTime.LowPart - lastTime.LowPart) / (double)(int)frequency.LowPart)
				{
					float temp = (float)(get_remaining_time_until_next_tick_in_seconds() - timeDeltaSeconds) * 1000.f;
					int tempInt = 0;

					if ((int)temp > 0)
						tempInt = (int)temp;

					Sleep(tempInt);
					QueryPerformanceCounter(&currentTime);
				}
			}
		}

		network_time = (__int64)(((double)(currentTime.QuadPart - timeAtStartup.QuadPart) / (double)frequency.QuadPart) * 1000.0);
		lastTime = currentTime;
		timeDeltaSeconds = fminf(10.f, timeDeltaSeconds);
	}
	else
	{
		timeDeltaSeconds = p_compute_time_delta_def(use_static_time_increase, static_time_delta);
		QueryPerformanceCounter(&currentTime);
		network_time = (__int64)(((double)(currentTime.QuadPart - timeAtStartup.QuadPart) / (double)frequency.QuadPart) * 1000.0);
	}

	//LOG_TRACE_GAME("compute_time_delta() - timeDeltaSeconds: {}", timeDeltaSeconds);
	return timeDeltaSeconds;
}

void OriginalFPSLimiter::ApplyPatches()
{
	if (Memory::isDedicatedServer() == false)
	{
		//NopFill(Memory::GetAddress(0x2728E7), 5);

		PatchCall(Memory::GetAddress(0x39BE3), main_time_update);
		PatchCall(Memory::GetAddress(0x39C0D), main_time_update);
		WriteJmpTo(Memory::GetAddress(0x1B3C5C), get_time_delta_msec);
		WriteJmpTo(Memory::GetAddress(0x286D9), reset_time);

#if USE_HALO_1_TARGET_TICK_COUNT_COMPUTE_CODE
		PatchCall(Memory::GetAddress(0x39D04), compute_target_tick_count);
#endif // USE_HALO_1_TARGET_TICK_COUNT_COMPUTE_CODE

		//NopFill(Memory::GetAddress(0x39BDA), 2);
		//NopFill(Memory::GetAddress(0x39DF0), 8);

		//NopFill(Memory::GetAddress(0x39DE1), 5);
	}

	//PatchWinAPICall(Memory::GetAddress(0x37E51), timeGetTime_hook);

	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&timeAtStartup);
}

