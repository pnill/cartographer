
#include "UncappedFPS.h"

#include "H2MOD.h"
#include "Util/Hooks/Hook.h"

void __cdecl compute_target_tick_count(float a1, float* a2, int* out_target_tick_count)
{
	typedef void(__cdecl *compute_target_tick_count)(float, float*, int*);
	auto p_compute_target_tick_count = h2mod->GetAddress<compute_target_tick_count>(0x7C1BF);

	//a1 = 1.f / 60.f;
	p_compute_target_tick_count(a1, a2, out_target_tick_count);
	LOG_TRACE_GAME("input_float: {}, dt: {}, target_tick_count: {}", a1, *a2, *out_target_tick_count);
}

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
};


static LARGE_INTEGER PerformanceFrequency;
static LARGE_INTEGER PerformanceCountAtStartup;
static LARGE_INTEGER compute_time_delta_last_time;

int WINAPI timeGetTime_hook()
{
	LARGE_INTEGER PerformanceCount;
	QueryPerformanceCounter(&PerformanceCount);
	PerformanceCount.QuadPart = PerformanceCount.QuadPart - PerformanceCountAtStartup.QuadPart;
	return (unsigned int)(PerformanceCount.QuadPart / (PerformanceFrequency.QuadPart / 1000));
}

float __cdecl compute_time_delta(bool use_static_time_increase, float static_time_delta)
{
	typedef float(__cdecl* compute_time_delta_def)(bool, float);
	auto p_compute_time_delta_def = h2mod->GetAddress<compute_time_delta_def>(0x28814);

	LARGE_INTEGER frequency;
	LARGE_INTEGER current_time;
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&current_time);
	float ret = (current_time.LowPart - compute_time_delta_last_time.LowPart) / (float)frequency.LowPart;

	p_compute_time_delta_def(true, ret);

	compute_time_delta_last_time = current_time;

	ret = ret * 1.0f;
	if (ret >= 0.0f)
		ret = fmin(10.f, ret);

	return ret;
}

// we disable some broken code added by hired gun, that is also disabled while running a cinematic 
// this should fix the built in frame limiter (while minimized)
// as well as the game speeding up while minimized
bool __cdecl cinematic_in_progress_hook()
{
	typedef bool(__cdecl* game_is_minimized_def)();
	auto p_game_is_minimized = h2mod->GetAddress<game_is_minimized_def>(0x28729);

	typedef bool(__cdecl* cinematic_in_progress)();
	auto p_cinematic_in_progress = h2mod->GetAddress<cinematic_in_progress>(0x3A938);

	return p_cinematic_in_progress() || p_game_is_minimized();
	//return true;
}

void UncappedFPS::ApplyPatches()
{
	if (h2mod->Server == false)
	{
		//NopFill(h2mod->GetAddress(0x2728E7), 5);

		//PatchCall(h2mod->GetAddress(0x39BE3), compute_time_delta);
		//PatchCall(h2mod->GetAddress(0x39D04), compute_target_tick_count);

		//NopFill(h2mod->GetAddress(0x39BDA), 2);
		//NopFill(h2mod->GetAddress(0x39DF0), 8);

		//NopFill(h2mod->GetAddress(0x39DE1), 5);

		PatchCall(h2mod->GetAddress(0x39A2A), cinematic_in_progress_hook);
	}

	PatchWinAPICall(h2mod->GetAddress(0x37E51, 0x2B4CE), timeGetTime_hook);

	QueryPerformanceFrequency(&PerformanceFrequency);
	QueryPerformanceCounter(&PerformanceCountAtStartup);
}

