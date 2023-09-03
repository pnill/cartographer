#pragma once
#include "Blam/Engine/game/game_options.h"
#define k_game_maximum_ragdolls 3

struct s_main_game_globals
{
	bool initializing;
	bool map_active;
	short active_structure_bsp_index;
	int unused_0;
	s_game_options options;
	bool game_in_progress;
	bool game_is_lost;
	short pad_0;
	int ticks_to_reset_game;
	bool game_is_finished;
	bool game_sounds_disabled;
	short pad_1;
	DWORD ticks_till_end;
	DWORD game_ragdoll_count;
	int unused_1;
	byte cluster_pvs[64];
	byte cluster_pvs_local[64];
	DWORD cluster_activation[16];
	byte enable_scripted_camera_pvs;
	byte pad_2;
	WORD pvs_object_is_set;		// If it's 2 then it's set but if it's 1 or 0 then it's not?
	datum pvs_object_datum;
};
CHECK_STRUCT_SIZE(s_main_game_globals, 0x1270);

s_main_game_globals* get_main_game_globals(void);
bool map_initialized(void);
s_game_options* game_options_get(void);
s_game_variant* current_game_variant(void);
e_game_mode game_mode_get(void);
bool game_is_campaign(void);
bool game_is_multiplayer(void);
bool game_is_ui_shell(void);
bool game_in_progress(void);
bool game_is_predicted(void);
