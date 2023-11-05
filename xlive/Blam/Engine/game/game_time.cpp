#include "stdafx.h"
#include "game_time.h"

#include "Blam/Engine/cutscene/cinematics.h"
#include "Blam/Engine/math/math.h"
#include "Blam/Engine/shell/shell_windows.h"

#include "H2MOD.h"
#include "H2MOD/Modules/Shell/Config.h"
#include "Util/Hooks/Hook.h"

FLOATING_POINT_ENV_ACCESS();

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

float time_globals::get_seconds_per_tick()
{
	return get()->seconds_per_tick;
}

float time_globals::seconds_to_ticks_real(float s)
{
	return (float)get()->ticks_per_second * s;
}

int time_globals::seconds_to_ticks_round(float s)
{
	return blam_ticks_real_to_integer((float)get()->ticks_per_second * s);
}

float time_globals::ticks_to_seconds(int ticks)
{
	return get()->seconds_per_tick * ticks;
}

int time_globals::get_ticks_difference()
{
	return (get()->ticks_per_second / 30);
}

float time_globals::get_ticks_difference_real()
{
	return ((float)get()->ticks_per_second / 30.0f);
}

float time_globals::get_ticks_fraction_leftover()
{
	return get()->game_ticks_leftover;
}

bool time_globals::available()
{
	return get() && get()->initialized;
}

// We disable some broken code added by hired gun, that is also disabled while running a cinematic 
// This should fix the built in frame limiter (while minimized)
// As well as the game speeding up while minimized
bool __cdecl cinematic_in_progress_hook()
{
	H2Config_Experimental_Rendering_Mode experimental_rendering_mode = H2Config_experimental_fps;

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
	H2Config_Experimental_Rendering_Mode experimental_rendering_mode = H2Config_experimental_fps;

	switch (experimental_rendering_mode)
	{
	case _rendering_mode_original_game_frame_limit:
		return false; // e_render_original_game_frame_limit handles frame limit in MainGameTime.cpp
	case _rendering_mode_none:
	default:
		return (game_is_minimized() || xbox_tickrate_is_enabled());
	}

	return false;
}

void game_time_apply_patches()
{
	// apply framerate throttle patches for when the game is minimized
	PatchCall(Memory::GetAddress(0x39A2A), cinematic_in_progress_hook);
	PatchCall(Memory::GetAddress(0x288B5), should_limit_framerate_hook);
}
