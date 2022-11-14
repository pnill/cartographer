#include "stdafx.h"

#include "game.h"
#include "game_time.h"

#include <float.h>
#if (!defined(_M_FP_FAST)) || !_M_FP_FAST
#pragma fenv_access (on)
#endif

time_globals* time_globals::get()
{
	return *Memory::GetAddress<time_globals**>(0x4C06E4, 0x4CF0EC);
}

bool time_globals::game_time_initialized()
{
	return get() && get()->initialized;
}

int time_globals::get_game_time_ticks()
{
	return get()->passed_ticks_count;
}

void time_globals::game_time_advance()
{
	++get()->passed_ticks_count;
}

bool time_globals::game_time_get_paused()
{
	return get()->initialized && get()->paused;
}

void time_globals::game_time_set_paused(bool is_paused)
{
	get()->paused = is_paused;
}

float time_globals::get_game_speed()
{
	return get()->game_speed;
}

void time_globals::game_time_update_tickrate_with_new_speed(float new_game_speed)
{
	const s_game_options* game_options; // eax
	float p_new_game_speed; // xmm0_4
	int tickrate; // edx
	time_globals* game_time_globals; // ecx

	game_options = s_game_globals::get_game_options();
	p_new_game_speed = new_game_speed;
	if (new_game_speed <= 0.0099999998f)
		p_new_game_speed = 0.0099999998f;
	tickrate = (int)(game_options->tickrate / p_new_game_speed);
	game_time_globals = get();
	game_time_globals->tick_length = (float)(1.0 / tickrate);
	game_time_globals->tickrate = (short)tickrate;
	game_time_globals->game_speed = game_options->tickrate / tickrate;
}

int time_globals::game_time_tick_rate()
{
	return get()->tickrate;
}

float time_globals::game_time_tick_length()
{
	return get()->tick_length;
}

float time_globals::game_time_ticks_to_seconds(float ticks)
{
	return get()->tick_length * ticks;
}

int time_globals::game_time_seconds_to_ticks(int seconds)
{
	return (seconds * get()->tickrate);
}

float time_globals::game_time_seconds_to_ticks_real(float seconds)
{
	return get()->tickrate * seconds;
}

int time_globals::game_time_seconds_to_ticks_round(float seconds)
{
	return (int)(get()->tickrate * seconds);
}

int time_globals::get_ticks_difference()
{
	return (get()->tickrate / 30);
}

float time_globals::get_ticks_difference_real()
{
	return (get()->tickrate / 30.0f);
}

float time_globals::get_ticks_fraction_leftover()
{
	return get()->game_ticks_leftover;
}