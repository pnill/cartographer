#include "stdafx.h"
#include "game_time.h"

#include "game.h"
#include "main/main_time.h"
#include "H2MOD.h"
#include "cutscene/cinematics.h"
#include "math/math.h"
#include "main/interpolator.h"
#include "main/main_game_time.h"
#include "shell/shell.h"
#include "shell/shell_windows.h"

time_globals* time_globals::get()
{
	return *Memory::GetAddress<time_globals**>(0x4C06E4, 0x4CF0EC);
}

uint32 game_time_get(void)
{
	const time_globals* game_time_globals = time_globals::get();
	ASSERT(game_time_globals && game_time_globals->initialized);

	return game_time_globals->passed_ticks_count;
}

int time_globals::get_tickrate()
{
	return get()->ticks_per_second;
}

real32 game_tick_length(void)
{
	const time_globals* game_time_globals = time_globals::get();
	ASSERT(game_time_globals);
	ASSERT(game_time_globals->initialized);

	return game_time_globals->tick_length;
}

real32 time_globals::seconds_to_ticks_real(real32 s)
{
	return (real32)get()->ticks_per_second * s;
}

int32 time_globals::seconds_to_ticks_round(real32 s)
{
	return blam_ticks_real_to_integer((real32)get()->ticks_per_second * s);
}

int32 time_globals::get_ticks_difference()
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

real32 game_ticks_to_seconds(real32 ticks)
{
	return time_globals::get()->tick_length * ticks;
}

bool game_time_get_paused(void)
{
	time_globals* game_time_globals = time_globals::get();
	return game_time_globals->initialized && game_time_globals->paused;
}

void game_time_set_paused(bool pause)
{
	time_globals* game_time_globals = time_globals::get();
	ASSERT(game_time_globals);
	ASSERT(game_time_globals->initialized);

	game_time_globals->paused = pause;
	return;
}

// Disables broken/experimental main loop patches in the vanilla game 
// that are also disabled when playing cinematics
// This also fixes the built in frame limiter while the game is minimized, as well as the speeding up issue
bool __cdecl cinematic_is_running_hook()
{
	bool result;
	if (halo_frame_interpolator_enabled())
	{
		result = true; // these two options disable the hacks that hired gun added to the main loop
	}
	else
	{
		result = cinematic_is_running() || xbox_tickrate_is_enabled() || main_time_is_throttled() || g_main_game_time_frame_limiter_enabled;
	}
	
	return result;
}

void game_time_discard(int32 desired_ticks, int32 actual_ticks, real32* elapsed_game_dt)
{
	if (actual_ticks > 0)
	{
		real32 result = *elapsed_game_dt - ((desired_ticks - actual_ticks) * time_globals::get()->tick_length);
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

void __cdecl game_time_update(real32 dt, real32* out_time_delta, int32* out_target_tick_count)
{
	INVOKE(0x7C1BF, 0x0, game_time_update, dt, out_time_delta, out_target_tick_count);
	return;
}

int32 game_seconds_integer_to_ticks(int32 seconds)
{
	const time_globals* game_time_globals = time_globals::get();

	ASSERT(game_time_globals);
	ASSERT(game_time_globals->initialized);

	return seconds * game_time_globals->ticks_per_second;
}

void game_time_advance(void)
{
	time_globals* game_time_globals = time_globals::get();
	ASSERT(game_time_globals);
	ASSERT(game_time_globals->initialized);
	++game_time_globals->passed_ticks_count;
	return;
}


void __cdecl game_time_reset()
{
	time_globals* game_time_globals = time_globals::get();

	game_time_globals->initialized = false;
	game_time_globals->tick_length = 0;
	game_time_globals->passed_ticks_count = 0;
	game_time_globals->game_speed = 0.f;
	game_time_globals->game_ticks_leftover = 0.f;
	game_time_globals->field_14 = 0.f;
	game_time_globals->field_18 = 0.f;
	game_time_globals->field_1C = 0.f;
	game_time_globals->field_20 = 0.f;
}

void __cdecl game_time_setup_scenario()
{
	game_time_reset();

	time_globals* game_time_globals = time_globals::get();
	s_game_options* game_options = game_options_get();

	game_time_globals->ticks_per_second = game_options->game_tick_rate;
	game_time_globals->tick_length = 1.f / (real32)game_time_globals->ticks_per_second;

	s_game_variant* variant = get_game_variant();

	if (game_is_multiplayer() && variant)
	{
		switch (variant->cartographer_settings.game_speed)
		{
			case _game_speed_modifier_half:
				game_time_globals->game_speed = 0.5f;
				break;
			case _game_speed_modifier_hundred_fifty:
				game_time_globals->game_speed = 1.5f;
				break;
			case _game_speed_modifier_double:
				game_time_globals->game_speed = 2.f;
				break;
			case _game_speed_modifier_ludicrous:
				game_time_globals->game_speed = 5.f;
				break;
			case _game_speed_modifier_none:
			default:
				game_time_globals->game_speed = k_game_time_default_game_speed;
				break;
		}
	}
	else
		game_time_globals->game_speed = k_game_time_default_game_speed;

	game_time_globals->initialized = true;
}

void game_time_apply_patches()
{
	if (!shell_is_dedicated_server())
	{
		// apply framerate throttle patches for when the game is minimized
		PatchCall(Memory::GetAddress(0x39A2A), cinematic_is_running_hook);
	}

	WritePointer(Memory::GetAddress(0x3A06F8, 0x35D428), game_time_setup_scenario);
}
