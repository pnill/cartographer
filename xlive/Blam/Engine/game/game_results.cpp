#include "stdafx.h"
#include "game_results.h"

/* prototypes */

s_game_results_globals* game_results_globals_get(void);

/* public code */

void game_results_stop_recording(void)
{
	game_results_globals_get()->recording = false;
	return;
}

void game_results_set_recording_pause(bool pause)
{
	game_results_globals_get()->recording_pause = pause;
	return;
}

bool game_results_get_game_recording(void)
{
	return game_results_globals_get()->recording;
}

bool game_results_get_game_updating(void)
{
	return game_results_globals_get()->updating;
}

void game_results_start_updating(void)
{
	game_results_globals_get()->updating = true;
	return;
}

void game_results_stop_updating(void)
{
	game_results_globals_get()->updating = false;
	return;
}

/* private code */

s_game_results_globals* game_results_globals_get(void)
{
	return Memory::GetAddress<s_game_results_globals*>(0x4B1C80, 0x4DC3B0);
}
