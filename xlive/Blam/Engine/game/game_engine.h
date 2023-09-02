#pragma once

#include "Blam/Engine/game/game_statborg.h"



enum e_valid_multiplayer_games : short
{
	valid_multiplayer_game_capture_the_flag = FLAG(0),
	valid_multiplayer_game_slayer = FLAG(1),
	valid_multiplayer_game_oddball = FLAG(2),
	valid_multiplayer_game_king_of_the_hill = FLAG(3),
	valid_multiplayer_game_juggernaut = FLAG(4),
	valid_multiplayer_game_territories = FLAG(5),
	valid_multiplayer_game_assault = FLAG(6),
};

enum e_relevant_multiplayer_games : int
{
	relevant_multiplayer_game_capture_the_flag = FLAG(0),
	relevant_multiplayer_game_slayer = FLAG(1),
	relevant_multiplayer_game_oddball = FLAG(2),
	relevant_multiplayer_game_king_of_the_hill = FLAG(3),
	relevant_multiplayer_game_juggernaut = FLAG(4),
	relevant_multiplayer_game_territories = FLAG(5),
	relevant_multiplayer_game_assault = FLAG(6)
};

struct s_game_engine_global_player_info
{
	bool valid;
	byte pad_1[3];
	real_point3d point;
	short field_10;
	short field_12;
	byte field_14[4];
};
CHECK_STRUCT_SIZE(s_game_engine_global_player_info, 24);

struct s_game_engine_globals
{
	DWORD flags;
	short team_flags;
	WORD field_6;
	WORD field_8;
	WORD flags_A;
	WORD field_C;
	short field_E;
	WORD field_10;
	WORD field_12[8];
	DWORD field_24;
	DWORD field_28;
	DWORD gap_3A[16];
	short field_6A;
	short field_6C;
	DWORD field_70;
	DWORD gap_74[28];
	float unk_local_player_hud_field[4];
	byte field_F4;
	byte pad_F5[4];
	byte gapF9[523];
	c_game_statborg game_statborg;
	s_game_engine_global_player_info player_info[16];
	DWORD ticks;
	BYTE gap71C[1336];
	DWORD game_engine_index;
	BYTE gapC58[132];

	static s_game_engine_globals* get();
};
CHECK_STRUCT_SIZE(s_game_engine_globals, 0xCDC);