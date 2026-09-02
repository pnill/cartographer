#include "stdafx.h"
#include "game_time.h"

#include "game.h"
#include "game_options.h"
#include "saved_games/game_variant.h"

/* structures */

struct game_time_globals_definition
{
	bool initialized;
	bool paused;
	int16 tick_rate;
	real32 tick_length;
	int32 time;
	real32 speed;
	real32 game_ticks_leftover;
	real32 rate_scale_timer;
	real32 rate_scale_duration;
	real32 rate_scale_initial;
	real32 rate_scale_final;
};
ASSERT_STRUCT_SIZE(game_time_globals_definition, 36);

/* prototypes */

static game_time_globals_definition* time_globals_get(void);

static void game_time_statistics_start(void);

static void game_time_set_variant_speed(void);

/* public code */

void game_time_apply_patches(void)
{
	WritePointer(Memory::GetAddress(0x3A06F8, 0x35D428), game_time_initialize_for_new_map);
	return;
}

bool game_time_initialized(void)
{
	const game_time_globals_definition* game_time_globals = time_globals_get();
	return game_time_globals && game_time_globals->initialized;
}

uint32 game_time_get(void)
{
	const game_time_globals_definition* game_time_globals = time_globals_get();
	ASSERT(game_time_globals && game_time_globals->initialized);

	return game_time_globals->time;
}

void game_time_discard(int32 desired_ticks, int32 actual_ticks, real32* elapsed_game_dt)
{
	game_time_globals_definition* game_time_globals = time_globals_get();
	ASSERT(game_time_globals);
	ASSERT(game_time_globals->initialized);
	ASSERT(desired_ticks > 0);
	ASSERT(actual_ticks < desired_ticks);
	ASSERT(elapsed_game_dt);

	if (actual_ticks > 0)
	{
		const real32 difference = (real32)(desired_ticks - actual_ticks);
		const real32 lost_dt = game_ticks_to_seconds(difference);
		ASSERT(*elapsed_game_dt - lost_dt > -(_real_epsilon + game_tick_length()));

		const real32 result = *elapsed_game_dt - lost_dt;
		*elapsed_game_dt = result > 0.f ? result : 0.f;
	}
	else
	{
		*elapsed_game_dt = 0.f;
	}

	return;
}

void game_time_advance(void)
{
	game_time_globals_definition* game_time_globals = time_globals_get();
	ASSERT(game_time_globals);
	ASSERT(game_time_globals->initialized);
	++game_time_globals->time;
	return;
}

bool game_time_get_paused(void)
{
	game_time_globals_definition* game_time_globals = time_globals_get();
	ASSERT(game_time_globals);
	return game_time_globals->initialized && game_time_globals->paused;
}

void game_time_set_paused(bool pause)
{
	game_time_globals_definition* game_time_globals = time_globals_get();
	ASSERT(game_time_globals);
	ASSERT(game_time_globals->initialized);

	game_time_globals->paused = pause;
	return;
}

real32 game_time_get_speed(void)
{
	const game_time_globals_definition* game_time_globals = time_globals_get();
	ASSERT(game_time_globals);
	ASSERT(game_time_globals->initialized);
	return game_time_globals->speed;
}

void game_time_set_speed(real32 speed)
{
	game_time_globals_definition* game_time_globals = time_globals_get();
	ASSERT(game_time_globals);
	ASSERT(game_time_globals->initialized);
	game_time_globals->speed = speed;
	return;
}

int32 game_seconds_integer_to_ticks(int32 seconds)
{
	const game_time_globals_definition* game_time_globals = time_globals_get();

	ASSERT(game_time_globals);
	ASSERT(game_time_globals->initialized);

	return seconds * game_time_globals->tick_rate;
}

bool __cdecl game_time_update(real32 world_seconds_elapsed, real32* game_seconds_elapsed, int32* game_ticks_elapsed)
{
	return INVOKE(0x7C1BF, 0x4BE6F, game_time_update, world_seconds_elapsed, game_seconds_elapsed, game_ticks_elapsed);
}

int32 game_tick_rate(void)
{
	const game_time_globals_definition* game_time_globals = time_globals_get();
	ASSERT(game_time_globals);
	ASSERT(game_time_globals->initialized);

	return game_time_globals->tick_rate;
}

real32 game_tick_length(void)
{
	const game_time_globals_definition* game_time_globals = time_globals_get();
	ASSERT(game_time_globals);
	ASSERT(game_time_globals->initialized);

	return game_time_globals->tick_length;
}

real32 game_ticks_to_seconds(real32 ticks)
{
	const game_time_globals_definition* game_time_globals = time_globals_get();
	ASSERT(game_time_globals);
	ASSERT(game_time_globals->initialized);

	return game_time_globals->tick_length * ticks;
}

real32 game_seconds_to_ticks_real(real32 seconds)
{
	const game_time_globals_definition* game_time_globals = time_globals_get();

	ASSERT(game_time_globals);
	ASSERT(game_time_globals->initialized);

	return ((real32)game_time_globals->tick_rate * seconds);
}

real32 game_tick_rate_legacy_ratio_real()
{
	const game_time_globals_definition* game_time_globals = time_globals_get();

	ASSERT(game_time_globals);
	ASSERT(game_time_globals->initialized);

	return (real32)game_time_globals->tick_rate / 30.f;
}

int32 game_seconds_to_ticks_round(real32 seconds)
{
	const game_time_globals_definition* game_time_globals = time_globals_get();

	ASSERT(game_time_globals);
	ASSERT(game_time_globals->initialized);

	return real_to_long_round((real32)game_time_globals->tick_rate * seconds);
}

real32 game_time_get_max_frame_time(void)
{
	game_time_globals_definition* game_time_globals = time_globals_get();
	ASSERT(game_time_globals);
	ASSERT(game_time_globals->initialized);

	const real32 result = game_time_globals->tick_length - (real32)(game_time_globals->game_ticks_leftover / (real32)game_time_globals->tick_rate);
	return MAX(result, 0.f);
}

real32 game_time_get_leftover(void)
{
	game_time_globals_definition* game_time_globals = time_globals_get();
	return PIN(game_time_globals->game_ticks_leftover, 0.f, 1.f);
}

void __cdecl game_time_initialize_for_new_map(void)
{
	game_time_globals_definition* game_time_globals = time_globals_get();
	s_game_options const* game_options = game_options_get();

	ASSERT(game_time_globals && !game_time_globals->initialized);

	csmemset(game_time_globals, 0, sizeof(*game_time_globals));

	game_time_globals->tick_rate = game_options->game_tick_rate;

	ASSERT(game_time_globals->tick_rate > 0);

	game_time_globals->tick_length = 1.f / (real32)game_time_globals->tick_rate;

	// Updated method for allowing variants with custom speeds
	if (true)
	{
		game_time_set_variant_speed();
	}
	// Old method of setting game speed
	else
	{
		game_time_globals->speed = k_game_time_default_game_speed;
	}

	game_time_statistics_start();

	game_time_globals->initialized = true;

	return;
}

/* private code */

static game_time_globals_definition* time_globals_get(void)
{
	return *Memory::GetAddress<game_time_globals_definition**>(0x4C06E4, 0x4CF0EC);
}

static void game_time_statistics_start(void)
{
	// TODO: reimplement
	return;
}

static void game_time_set_variant_speed(void)
{
	game_time_globals_definition* game_time_globals = time_globals_get();
	s_game_variant const* variant = get_game_variant();

	if (game_is_multiplayer() && variant)
	{
		switch (variant->cartographer_settings.game_speed)
		{
		case _game_speed_modifier_half:
			game_time_globals->speed = 0.5f;
			break;
		case _game_speed_modifier_hundred_fifty:
			game_time_globals->speed = 1.5f;
			break;
		case _game_speed_modifier_double:
			game_time_globals->speed = 2.f;
			break;
		case _game_speed_modifier_ludicrous:
			game_time_globals->speed = 5.f;
			break;
		case _game_speed_modifier_none:
		default:
			game_time_globals->speed = k_game_time_default_game_speed;
			break;
		}
	}
	else
	{
		game_time_globals->speed = k_game_time_default_game_speed;
	}

	return;
}
