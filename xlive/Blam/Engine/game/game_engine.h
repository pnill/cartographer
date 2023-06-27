#pragma once

#include "Blam/Common/Common.h"

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

struct s_game_engine_globals
{
	byte gap0[10];
	short fieldA;
	byte gapC[216];
	float unk_local_player_hud_field[4];
	byte gapF4[2912];
	int game_engine_index;
	byte gapC58[132];

	static s_game_engine_globals* get();
};
CHECK_STRUCT_SIZE(s_game_engine_globals, 0xCDC);