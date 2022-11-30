#pragma once
#include "Blam/Engine/game/game_options.h"
#include <wtypes.h>

#pragma pack(push,1)
struct s_game_globals
{
	char initializing;
	char map_active;
	__int16 active_structure_bsp_index;
	char field_4;
	char field_5;
	char field_6;
	char field_7;
	s_game_options options;
	char game_in_progress;
	char game_is_lost;
	PAD(2);
	int ticks_to_reset_game;
	BYTE game_options_related;	//need better name
	PAD(3);
	DWORD field_119C;		//stores 7 * game_ticks_per_second at game_finish
	DWORD field_11A0;
	PAD(68);
	DWORD field_11E8;
	PAD(126);
	WORD field_126A;
	PAD(3);
	char field_0;


	static s_game_globals* get();
	static bool map_initialized();
	static s_game_options* get_game_options();
	static s_game_variant* get_game_variant();
	static void s_game_globals::set_engine_type(e_engine_type engine_type);
	static e_engine_type get_engine_type();
	static bool game_is_campaign();
	static bool game_is_multiplayer();
	static bool game_is_mainmenu();
	static bool game_is_in_progress();

	static bool game_is_predicted();
};
CHECK_STRUCT_SIZE(s_game_globals, 0x1270);
#pragma pack(pop)
