#include "stdafx.h"
#include "player_control.h"

#include "Blam/Engine/game/game_time.h"
#include "Blam/Engine/main/interpolator.h"

#include "Util/Hooks/Hook.h"

real32 g_player_control_dt = 0.0f;

void player_control_update_dt(real32 dt)
{
	g_player_control_dt = dt;
}

real32 __cdecl player_control_get_autocenter_delta()
{
	if (halo_interpolator_update_in_progress())
	{
		return time_globals::get_seconds_per_tick();
	}

	return g_player_control_dt;
}

s_player_control_globals* s_player_control_globals::get()
{
	return *Memory::GetAddress<s_player_control_globals**>(0x4CA37C, 0x4CF0E8);
}

void player_control_apply_patches()
{
	PatchCall(Memory::GetAddress(0x91149), player_control_get_autocenter_delta);
}
