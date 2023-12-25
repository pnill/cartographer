#include "stdafx.h"
#include "game_time.h"

#include "Blam/Engine/cutscene/cinematics.h"
#include "Blam/Engine/math/math.h"
#include "Blam/Engine/shell/shell_windows.h"

#include "H2MOD.h"
#include "H2MOD/Modules/Shell/Config.h"

#include "Util/Hooks/Hook.h"

FLOATING_POINT_ENV_ACCESS();

extern H2Config_Experimental_Rendering_Mode g_experimental_rendering_mode;

time_globals* time_globals::get()
{
	return *Memory::GetAddress<time_globals**>(0x4C06E4, 0x4CF0EC);
}

int time_globals::get_game_time()
{
	return get()->tick_count;
}

int time_globals::get_tickrate()
{
	return get()->ticks_per_second;
}

real32 time_globals::get_seconds_per_tick()
{
	return get()->seconds_per_tick;
}

real32 time_globals::seconds_to_ticks_real(real32 s)
{
	return (real32)get()->ticks_per_second * s;
}

int time_globals::seconds_to_ticks_round(real32 s)
{
	return blam_ticks_real_to_integer((real32)get()->ticks_per_second * s);
}

real32 time_globals::ticks_to_seconds(int ticks)
{
	return get()->seconds_per_tick * (real32)ticks;
}

int time_globals::get_ticks_difference()
{
	return (get()->ticks_per_second / 30);
}

real32 time_globals::get_ticks_difference_real()
{
	return ((real32)get()->ticks_per_second / 30.0f);
}

real32 time_globals::get_ticks_fraction_leftover()
{
	return get()->game_ticks_leftover;
}

bool time_globals::available()
{
	return get() && get()->initialized;
}

bool game_time_get_paused(void)
{
	time_globals* game_time_globals = time_globals::get();
	return game_time_globals->initialized && game_time_globals->paused;
}

// We disable some broken code added by hired gun, that is also disabled while running a cinematic 
// This should fix the built in frame limiter (while minimized)
// As well as the game speeding up while minimized
bool __cdecl cinematic_is_running_hook()
{
	H2Config_Experimental_Rendering_Mode experimental_rendering_mode = g_experimental_rendering_mode;

	switch (experimental_rendering_mode)
	{
		// these two options disable the hacks that hired gun added to the main loop
	case _rendering_mode_original_game_frame_limit:
		return true;

	case _rendering_mode_none:
	default:
		return cinematic_is_running() || xbox_tickrate_is_enabled() || game_is_minimized();
	}

	return false;
}

bool __cdecl should_limit_framerate_hook()
{
	H2Config_Experimental_Rendering_Mode experimental_rendering_mode = g_experimental_rendering_mode;

	switch (experimental_rendering_mode)
	{
	case _rendering_mode_original_game_frame_limit:
		return false; // e_render_original_game_frame_limit handles frame limit in main_game_time.cpp
	case _rendering_mode_none:
	default:
		return (game_is_minimized() || xbox_tickrate_is_enabled());
	}

	return false;
}

void game_time_discard(int32 desired_ticks, int32 actual_ticks, real32* elapsed_game_dt)
{
	if (actual_ticks)
	{
		real32 result = *elapsed_game_dt - ((desired_ticks - actual_ticks) * time_globals::get()->seconds_per_tick);
		if (result <= 0.0f)
		{
			result = 0.0f;
		}
		*elapsed_game_dt = result;
	}
	else
	{
		*elapsed_game_dt = 0.0;
	}

	return;
}

void game_time_apply_patches()
{
	// apply framerate throttle patches for when the game is minimized
	PatchCall(Memory::GetAddress(0x39A2A), cinematic_is_running_hook);
	PatchCall(Memory::GetAddress(0x288B5), should_limit_framerate_hook);
}
