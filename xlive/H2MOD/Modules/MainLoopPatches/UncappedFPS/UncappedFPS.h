#pragma once

struct __declspec(align(4)) time_globals
{
	bool initialized;
	bool game_is_paused;
	WORD ticks_per_second;
	float seconds_per_tick;
	DWORD tick_count;
	float game_speed;
	float field_10;
	float field_14;
	float field_18;
	float field_1C;
	float field_20;
};


namespace UncappedFPS
{
	void ApplyPatches();
}