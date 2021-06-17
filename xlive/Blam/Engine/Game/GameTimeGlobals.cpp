
#include "GameTimeGlobals.h"

#include <float.h>
#pragma fenv_access (on)

time_globals* time_globals::get()
{
	return *Memory::GetAddress<time_globals**>(0x4C06E4, 0x4CF0EC);
}

float time_globals::get_seconds_per_tick()
{
	return get()->seconds_per_tick;
}

float time_globals::seconds_to_ticks_precise(float s)
{
	return (float)(signed __int16)get()->ticks_per_second * s;
}

signed int time_globals::seconds_to_ticks_impercise(float s)
{
	return (int)(float)((float)(__int16)get()->ticks_per_second * s);
}

int time_globals::get_ticks_difference()
{
	return (get()->ticks_per_second / 30);
}
