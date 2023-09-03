#include "stdafx.h"

#include "MainGameTime.h"

#include "Util/Hooks/Hook.h"

#include "Blam/Engine/game/game_globals.h"
#include "Blam/Engine/game/game_time.h"

#include "H2MOD/Modules/Shell/Shell.h"
#include "H2MOD/Modules/Shell/Config.h"
#include "H2MOD/Modules/OnScreenDebug/OnscreenDebug.h"
#include "H2MOD/Utils/Utils.h"

#include "H2MOD/GUI/ImGui_Integration/Console/ImGui_ConsoleImpl.h"

extern bool xboxTickrateEnabled;

bool MainGameTime::fps_limiter_enabled = false;

// if this is enabled, the tick count to be executed will be calculated the same way as in Halo 1/CE
#define USE_HALO_1_TARGET_TICK_COUNT_COMPUTE_CODE 0

float get_ticks_leftover_time()
{
	time_globals* timeGlobals = time_globals::get();
	float result = timeGlobals->seconds_per_tick - (float)(timeGlobals->game_ticks_leftover / (float)timeGlobals->ticks_per_second);
	return blam_max(result, 0.0f);
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

	/*if (*out_target_tick_count == 0)
	{
		LOG_TRACE_GAME("input_float: {}, dt: {}, target_tick_count: {}", dt, *a2, *out_target_tick_count);
	}*/

#if USE_HALO_1_TARGET_TICK_COUNT_COMPUTE_CODE
	time_globals* timeGlobals = time_globals::get();
	if (p_game_is_not_paused() && !timeGlobals->game_is_paused && timeGlobals->game_speed > 0.f)
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

float __cdecl main_time_update_hook(bool fixed_time_step, float fixed_time_delta)
{
	// original code
	/*
	dt = p_compute_time_delta(static_time_step, static_time_delta);

	QueryPerformanceCounter(&currentCounter);
	_currentTimeMsec = _Shell::QPCToTime(std::milli::den, currentCounter, frequency);
	network_time = (_currentTimeMsec - _timeAtStartupMsec);
	*/

	//
	// TODO cleanup
	//

	typedef float(__cdecl* compute_time_delta_t)(bool, float);
	auto p_compute_time_delta = Memory::GetAddress<compute_time_delta_t>(0x28814);

	typedef void(__cdecl* translate_windows_messages_t)();
	auto p_translate_windows_messages = Memory::GetAddress<translate_windows_messages_t>(0x7902);

	int game_time;
	float dtSec = 0.0f;
	LARGE_INTEGER currentCounter, counterFrq;
	long long _currentTimeMsec, _timeAtStartupMsec;

	s_main_time_globals* main_time_globals;

	main_time_globals = s_main_time_globals::get();
	game_time = game_in_progress() ? time_globals::get_game_time() : 0;

	// QueryPerformanceFrequency(&counterFrq);
	// TODO: fixme, time offset breaks this
	// also cleanup
	// _timeAtStartupMsec = _Shell::QPCToTime(std::milli::den, _Shell::QPCGetStartupCounter(), counterFrq);

	// TranslateMessage()
	// TODO move to function and cleanup
	p_translate_windows_messages();

	if (fixed_time_step)
	{
		dtSec = fixed_time_delta;
		if (time_globals::available())
			time_globals::get()->game_ticks_leftover = 0.0f;
	}
	else
	{
		//QueryPerformanceCounter(&currentCounter);
		//_currentTimeMsec = _Shell::QPCToTime(std::milli::den, currentCounter, counterFrq) - _timeAtStartupMsec;
		_currentTimeMsec = timeGetTime();
		dtSec = (double)(_currentTimeMsec - main_time_globals->last_time_ms) / 1000.;

		// don't run the frame limiter when time step is fixed, because the code doesn't support it
		// in case of fixed time step, frame limiter should be handled by the other frame limiter
		if (MainGameTime::fps_limiter_enabled || _Shell::IsGameMinimized())
		{
			if (time_globals::available())
			{
				// if there's game tick leftover time (i.e the actual game tick update executed faster than the actual engine's fixed time step)
				// limit the framerate to get back in sync with the renderer to prevent ghosting and jagged movement
				while (get_ticks_leftover_time() > dtSec)
				{
					int iMsSleep = 0;
					float fMsSleep = (float)(get_ticks_leftover_time() - dtSec) * 1000.f;

					if ((int)fMsSleep > 0)
						iMsSleep = (int)fMsSleep;

					// TODO FIXME to reduce stuttering, spend some of the time to sleep by CPU spinning,
					// Sleep is not precise since Windows is not a RTOS
					Sleep(iMsSleep);

					// QueryPerformanceCounter(&currentCounter);
					//_currentTimeMsec = _Shell::QPCToTime(std::milli::den, currentCounter, counterFrq) - _timeAtStartupMsec;
					_currentTimeMsec = timeGetTime();
					dtSec = (double)(_currentTimeMsec - main_time_globals->last_time_ms) / 1000.;
				}
			}
			else
			{
				Sleep(15u);

				// QueryPerformanceCounter(&currentCounter);
				//_currentTimeMsec = _Shell::QPCToTime(std::milli::den, currentCounter, counterFrq) - _timeAtStartupMsec;
				_currentTimeMsec = timeGetTime();
				dtSec = (double)(_currentTimeMsec - main_time_globals->last_time_ms) / 1000.;
			}
		}
	}

	dtSec = blam_min(dtSec, 10.f);
	// QueryPerformanceCounter(&currentCounter);
	// _currentTimeMsec = _Shell::QPCToTime(std::milli::den, currentCounter, counterFrq) - _timeAtStartupMsec;
	_currentTimeMsec = timeGetTime();
	if (fixed_time_step)
		_currentTimeMsec = main_time_globals->last_time_ms + (long long)(fixed_time_delta * 1000.0f);
	main_time_globals->last_time_ms = _currentTimeMsec;
	main_time_globals->game_time_passed = game_time;
	main_time_globals->field_16[0] = *Memory::GetAddress<__int64*>(0xA3E440);
	main_time_globals->field_16[1] = *Memory::GetAddress<__int64*>(0xA3E440);

	//LOG_TRACE_GAME("main_time_update() - timeDeltaSeconds: {}", timeDeltaSeconds);
	return dtSec;
}

void MainGameTime::ApplyPatches()
{
	if (Memory::IsDedicatedServer() == false)
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

