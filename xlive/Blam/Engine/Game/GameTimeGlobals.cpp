#include "stdafx.h"

#include "GameTimeGlobals.h"

#include <float.h>
#if (!defined(_M_FP_FAST)) || !_M_FP_FAST
#pragma fenv_access (on)
#endif

time_globals* time_globals::get()
{
	return *Memory::GetAddress<time_globals**>(0x4C06E4, 0x4CF0EC);
}

int time_globals::get_game_time()
{
	return get()->tick_count;
}

float time_globals::get_seconds_per_tick()
{
	return get()->seconds_per_tick;
}

float time_globals::seconds_to_ticks_real(float s)
{
	return (float)(__int16)get()->ticks_per_second * s;
}

int time_globals::seconds_to_ticks_round(float s)
{
	return (int)(float)((float)(__int16)get()->ticks_per_second * s);
}

int time_globals::get_ticks_difference()
{
	return (get()->ticks_per_second / 30);
}

float time_globals::get_ticks_difference_real()
{
	return (get()->ticks_per_second / 30.0f);
}

float time_globals::get_ticks_fraction_leftover()
{
	return get()->game_ticks_leftover;
}

bool time_globals::available()
{
	return get() && get()->initialized;
}
