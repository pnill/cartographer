#pragma once

/* structures */

struct s_game_results_globals
{
	bool recording;
	bool recording_pause;
	bool updating;
};

/* prototypes */

void game_results_stop_recording(void);
void game_results_set_recording_pause(bool pause);
bool game_results_get_game_recording(void);
bool game_results_get_game_updating(void);
void game_results_start_updating(void);
void game_results_stop_updating(void);
