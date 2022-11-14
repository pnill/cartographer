#pragma once

#include "Blam/Common/Common.h"

//size: 0x24
struct time_globals
{
	bool initialized;
	bool paused;
	short tickrate;
	float tick_length;
	long passed_ticks_count;
	float game_speed;
	float game_ticks_leftover;
	float game_rate0;
	float game_rate1;
	float game_rate2;
	float game_rate3;

	static time_globals* get();
	static bool game_time_initialized();
	static int get_game_time_ticks();
	static void game_time_advance();
	static bool game_time_get_paused();
	static void game_time_set_paused(bool is_paused);
	static float __cdecl get_game_speed();
	static void game_time_update_tickrate_with_new_speed(float new_game_speed);
	static int game_time_tick_rate();
	static float game_time_tick_length();
	static float game_time_ticks_to_seconds(float ticks);
	static int game_time_seconds_to_ticks(int seconds);
	static float game_time_seconds_to_ticks_real(float seconds);
	static int game_time_seconds_to_ticks_round(float seconds);
	static int get_ticks_difference();
	static float get_ticks_difference_real();
	static float get_ticks_fraction_leftover();
};
CHECK_STRUCT_SIZE(time_globals, 0x24);
