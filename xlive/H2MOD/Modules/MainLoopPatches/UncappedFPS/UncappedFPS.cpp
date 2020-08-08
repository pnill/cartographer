
#include "UncappedFPS.h"

#include "H2MOD.h"
#include "Util/Hooks/Hook.h"

extern bool b_XboxTick;
static LARGE_INTEGER frequency;
static LARGE_INTEGER timeAtStartup;

#define COMPUTE_DELTA_TIME_PATCHES 0

void __cdecl compute_target_tick_count(float a1, float* a2, int* out_target_tick_count)
{
	typedef void(__cdecl *compute_target_tick_count)(float, float*, int*);
	auto p_compute_target_tick_count = h2mod->GetAddress<compute_target_tick_count>(0x7C1BF);

	//a1 = 1.f / 60.f;

	p_compute_target_tick_count(a1, a2, out_target_tick_count);
	
	if (*out_target_tick_count == 0)
	{
		LOG_TRACE_GAME("input_float: {}, dt: {}, target_tick_count: {}", a1, *a2, *out_target_tick_count);
	}
}

/*
struct __declspec(align(4)) time_globals
{
	bool initialized;
	bool field_1;
	WORD ticks_per_sec;
	float seconds_per_tick;
	DWORD tick_count;
	float game_speed;
	float field_10;
	float field_14;
	float field_18;
	float field_1C;
	float field_20;

	time_globals* get_game_time_globals()
	{
		return *h2mod->GetAddress<time_globals**>(0x4C06E4);
	}
};
*/

DWORD dwTimeAtStartup;
DWORD WINAPI timeGetTime_hook()
{
	return timeGetTime() - dwTimeAtStartup;
}

static __int64 game_time_replacement;
__int64 get_game_time()
{
	return game_time_replacement;
}

static LARGE_INTEGER lastTime;
float __cdecl compute_time_delta(bool use_static_time_increase, float static_time_delta)
{
	typedef float(__cdecl* compute_time_delta_def)(bool, float);
	auto p_compute_time_delta_def = h2mod->GetAddress<compute_time_delta_def>(0x28814);

	LARGE_INTEGER currentTime, elapsedMilliseconds;
	QueryPerformanceCounter(&currentTime);
	elapsedMilliseconds.QuadPart = currentTime.QuadPart - timeAtStartup.QuadPart;

	// We now have the elapsed number of ticks, along with the
	// number of ticks-per-second. We use these values
	// to convert to the number of elapsed microseconds.
	// To guard against loss-of-precision, we convert
	// to milliseconds *before* dividing by ticks-per-second.

	elapsedMilliseconds.QuadPart *= 1000; // convert to milliseconds
	elapsedMilliseconds.QuadPart /= frequency.QuadPart;
	game_time_replacement = elapsedMilliseconds.QuadPart; 

	float ret = p_compute_time_delta_def(use_static_time_increase, static_time_delta);

#if COMPUTE_DELTA_TIME_PATCHES
	//LOG_TRACE_GAME("compute_time_delta() - time delta: {}", ret);

	QueryPerformanceCounter(&currentTime);
	ret = (double)(currentTime.LowPart - lastTime.LowPart) / (double)(int)frequency.LowPart;
	ret = fminf(10.f, ret);

	lastTime = currentTime;
#endif // COMPUTE_DELTA_TIME_PATCHES

	return ret;
}

// we disable some broken code added by hired gun, that is also disabled while running a cinematic 
// this should fix the built in frame limiter (while minimized)
// as well as the game speeding up while minimized
bool __cdecl cinematic_in_progress_hook()
{
	typedef bool(__cdecl* cinematic_in_progress)();
	auto p_cinematic_in_progress = h2mod->GetAddress<cinematic_in_progress>(0x3A938);

	bool unk = *h2mod->GetAddress<bool*>(0x48225B);
	if (!p_cinematic_in_progress())
		unk = false;

	*h2mod->GetAddress<bool*>(0x48225B) = unk;
#if COMPUTE_DELTA_TIME_PATCHES
	return true;
#else
	return p_cinematic_in_progress() || call_is_game_minimized() || call_get_game_tick_rate() < 60 || b_XboxTick;
#endif // COMPUTE_DELTA_TIME_PATCHES
}

bool __cdecl should_limit_framerate()
{
	return call_is_game_minimized() || b_XboxTick;
}

void UncappedFPS::ApplyPatches()
{
	if (h2mod->Server == false)
	{
		//NopFill(h2mod->GetAddress(0x2728E7), 5);

		PatchCall(h2mod->GetAddress(0x39BE3), compute_time_delta);
		PatchCall(h2mod->GetAddress(0x39C0D), compute_time_delta);

#if COMPUTE_DELTA_TIME_PATCHES
		PatchCall(h2mod->GetAddress(0x39D04), compute_target_tick_count);
#endif // COMPUTE_DELTA_TIME_PATCHES

		//NopFill(h2mod->GetAddress(0x39BDA), 2);
		//NopFill(h2mod->GetAddress(0x39DF0), 8);

		//NopFill(h2mod->GetAddress(0x39DE1), 5);

		PatchCall(h2mod->GetAddress(0x288B5), should_limit_framerate);

		PatchCall(h2mod->GetAddress(0x39A2A), cinematic_in_progress_hook);

		WriteJmpTo(h2mod->GetAddress(0x1B3C5C), get_game_time);
	}

	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&timeAtStartup);
	QueryPerformanceCounter(&lastTime);

	//dwTimeAtStartup = timeGetTime();
	//PatchWinAPICall(h2mod->GetAddress(0x37E51, 0x2B4CE), timeGetTime_hook);
}

