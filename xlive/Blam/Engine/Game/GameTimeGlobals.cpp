#include "GameTimeGlobals.h"
#include "H2MOD.h"
time_globals* time_globals::get()
{
	return *h2mod->GetAddress<time_globals**>(0x4C06E4, 0x4CF0EC);
}

float time_globals::get_seconds_per_tick()
{
	return get()->seconds_per_tick;
}

float time_globals::seconds_to_ticks_precise(float s)
{
	return (float)(signed __int16)get()->ticks_per_second * s;
}