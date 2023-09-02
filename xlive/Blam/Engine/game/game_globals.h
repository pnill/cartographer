#pragma once
#include "Blam/Cache/DataTypes/BlamDataTypes.h"
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

	static s_main_game_globals* get();
	static bool map_initialized();
	static s_game_options* get_game_options();
	static s_game_variant* get_game_variant();
	static bool game_is_campaign();
	static bool game_is_multiplayer();
	static bool game_is_mainmenu();
	static bool game_is_in_progress();

	static bool game_is_predicted();
};
CHECK_STRUCT_SIZE(s_main_game_globals, 0x1270);

e_game_mode get_current_engine_type();
