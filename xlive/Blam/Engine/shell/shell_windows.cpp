#include "stdafx.h"
#include "shell_windows.h"
#include "shell_windows_internals.h"

#include "math/math.h"



static LARGE_INTEGER g_startup_counter;

bool __cdecl game_is_minimized(void)
{
	return INVOKE(0x28729, 0x248AB, game_is_minimized);
}

LARGE_INTEGER shell_get_startup_counter()
{
	return g_startup_counter;
}

long long shell_time_from_counter(LARGE_INTEGER counter, LARGE_INTEGER freq, long long denominator)
{
	long long _Whole, _Part;

	_Whole = (counter.QuadPart / freq.QuadPart) * denominator;
	_Part = (counter.QuadPart % freq.QuadPart) * denominator / freq.QuadPart;

	return _Whole + _Part;
}

LARGE_INTEGER shell_time_counter_now(LARGE_INTEGER* freq)
{
	LARGE_INTEGER counter;
	if (freq) 
	{
		QueryPerformanceFrequency(freq);
	}
	QueryPerformanceCounter(&counter);
	counter.QuadPart -= shell_get_startup_counter().QuadPart;
	return counter;
}

long long shell_time_diff(LARGE_INTEGER t2, long long denominator)
{
	LARGE_INTEGER counter, freq;
	counter = shell_time_counter_now(&freq);
	counter.QuadPart -= t2.QuadPart;
	return shell_time_from_counter(counter, freq, denominator);
}

long long shell_time_now(long long denominator)
{
	LARGE_INTEGER counter, freq;
	counter = shell_time_counter_now(&freq);
	return shell_time_from_counter(counter, freq, denominator) + (k_process_system_time_startup_offset_sec * denominator);
}

long long shell_time_now_sec()
{
	return shell_time_now(k_shell_time_sec_denominator);
}

long long shell_time_now_msec()
{
	return shell_time_now(k_shell_time_msec_denominator);
}

static DWORD(WINAPI* p_timeGetTime)() = timeGetTime;
DWORD WINAPI timeGetTime_hook()
{
	long long time_now_msec = shell_time_now_msec();
	return (DWORD)time_now_msec;
}
static_assert(std::is_same_v<decltype(timeGetTime), decltype(timeGetTime_hook)>, "Invalid timeGetTime_hook signature");

void shell_windows_initialize()
{
	QueryPerformanceCounter(&g_startup_counter);
}

void shell_windows_apply_patches()
{
	DETOUR_ATTACH(p_timeGetTime, timeGetTime, timeGetTime_hook);
}

static void shell_system_set_timer_resolution_max(bool enable)
{
	ULONG ulMinimumResolution, ulMaximumResolution, ulCurrentResolution;
	NtQueryTimerResolutionHelper(&ulMinimumResolution, &ulMaximumResolution, &ulCurrentResolution);
	NtSetTimerResolutionHelper(ulMaximumResolution, enable, &ulCurrentResolution);
}

void shell_windows_throttle_framerate(int desired_framerate)
{
	static LARGE_INTEGER last_counter;
	static int last_desired_framerate_setting = -1;
	static bool frame_limiter_initialized = false;

	const int threadWaitTimePercentage = 90;
	static HANDLE hFrameLimitTimer = NULL;

	if (desired_framerate <= 0)
	{
		last_desired_framerate_setting = MAX(desired_framerate, 15);
		frame_limiter_initialized = false;
		return;
	}

	if (last_desired_framerate_setting != desired_framerate)
	{
		last_desired_framerate_setting = desired_framerate;
		frame_limiter_initialized = false;
	}

	if (!frame_limiter_initialized)
	{
		last_counter = shell_time_counter_now(NULL);
		frame_limiter_initialized = true;

		if (NULL == hFrameLimitTimer)
		{
			hFrameLimitTimer = CreateWaitableTimer(NULL, FALSE, NULL);

			atexit([]() {
				if (NULL != hFrameLimitTimer)
					CloseHandle(hFrameLimitTimer);

				// reset timer resolution back to default on exit
				shell_system_set_timer_resolution_max(false);
				});
		}

		// skip the first frame after init
		return;
	}

	auto min_frametime_usec = (long long)(1000000.0 / (double)desired_framerate);
	auto dt_usec = shell_time_diff(last_counter, k_shell_time_usec_denominator);

	const int min_time_to_suspend_exec_usec = 3000;

	if (dt_usec < min_frametime_usec)
	{
		auto sleep_time_usec = min_frametime_usec - dt_usec;

		// sleep threadWaitTimePercentage out of the target render time using thread sleep or timer wait
		long long system_yield_time_usec = (threadWaitTimePercentage * sleep_time_usec) / 100;

		// sleep just the milliseconds part
		// system_yield_time_usec = system_yield_time_usec - (system_yield_time_usec % 1000);

		// skip CPU yield if time is lower than 3ms
		// because the system timer isn't precise enough for our needs
		if (system_yield_time_usec > min_time_to_suspend_exec_usec)
		{
			if (NULL != hFrameLimitTimer)
			{
				// Create an unnamed waitable timer.
				ULONG ulMinimumResolution, ulMaximumResolution, ulCurrentResolution;
				NtQueryTimerResolutionHelper(&ulMinimumResolution, &ulMaximumResolution, &ulCurrentResolution);

				if (10ll * system_yield_time_usec > ulMaximumResolution)
				{
					LARGE_INTEGER liDueTime;
					NtSetTimerResolutionHelper(ulMaximumResolution, TRUE, &ulCurrentResolution);

					liDueTime.QuadPart = -10ll * system_yield_time_usec;
					if (SetWaitableTimer(hFrameLimitTimer, &liDueTime, 0, NULL, NULL, TRUE))
					{
						// Wait for the timer.
						NtWaitForSingleObjectHelper(hFrameLimitTimer, FALSE, &liDueTime);
					}
				}
			}

			/*int sleepTimeMs = system_yield_time_usec / 1000ll;
			if (sleepTimeMs >= 0)
				Sleep(sleepTimeMs);*/
		}

		// spin-lock the remaining slice of time
		while (true)
		{
			if (shell_time_diff(last_counter, k_shell_time_usec_denominator) >= min_frametime_usec)
				break;
		}
	}

	last_counter = shell_time_counter_now(NULL);
}