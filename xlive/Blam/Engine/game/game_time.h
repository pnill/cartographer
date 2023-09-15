#pragma once



//size: 0x24
struct time_globals
{
	bool initialized;
	bool paused;
	short ticks_per_second;
	float seconds_per_tick;
	DWORD tick_count;
	float game_speed;
	float game_ticks_leftover;
	float field_14;
	float field_18;
	float field_1C;
	float field_20;

	static bool available();
	static time_globals* get();
	static int get_game_time();
	static int get_tickrate();
	static float get_seconds_per_tick();
	static float seconds_to_ticks_real(float s);
	static int seconds_to_ticks_round(float s);
	static float ticks_to_seconds(int ticks);
	static int get_ticks_difference();
	static float get_ticks_difference_real();
	static float get_ticks_fraction_leftover();
};
CHECK_STRUCT_SIZE(time_globals, 0x24);