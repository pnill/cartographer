#include "stdafx.h"
#include "game.h"

void game_options_setup_default_players(int player_count, s_game_options* game_options)
{
	typedef void(__cdecl* cache_file_map_clear_all_failures_t)(int, s_game_options*);
	auto p_game_options_setup_default_players = Memory::GetAddress<cache_file_map_clear_all_failures_t>(0x49650, 0x428ED);
	p_game_options_setup_default_players(player_count, game_options);
	return;
}
