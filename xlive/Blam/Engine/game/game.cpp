#include "stdafx.h"
#include "game.h"

s_main_game_globals* get_main_game_globals(void)
{
	return *Memory::GetAddress<s_main_game_globals**>(0x482D3C, 0x4CB520);
}

bool map_initialized(void)
{
	return get_main_game_globals() && get_main_game_globals()->map_active && get_main_game_globals()->active_structure_bsp_index != -1;
}

s_game_options* game_options_get(void)
{
	return &get_main_game_globals()->options;
}

s_game_variant* current_game_variant(void)
{
	return &game_options_get()->game_variant;
}

e_game_mode game_mode_get(void)
{
	return get_main_game_globals()->options.game_mode;
}

bool game_is_campaign(void)
{
	return game_options_get()->game_mode == _game_mode_campaign;
}

bool game_is_multiplayer(void)
{
	return game_options_get()->game_mode == _game_mode_multiplayer;
}

bool game_is_ui_shell(void)
{
	return game_options_get()->game_mode == _game_mode_ui_shell;
}

bool game_is_predicted(void)
{
	return game_options_get()->simulation_type == _game_simulation_distributed_client;
}

bool game_in_progress(void)
{
	return get_main_game_globals() && get_main_game_globals()->game_in_progress;
}

void game_options_setup_default_players(int player_count, s_game_options* game_options)
{
	typedef void(__cdecl* cache_file_map_clear_all_failures_t)(int, s_game_options*);
	auto p_game_options_setup_default_players = Memory::GetAddress<cache_file_map_clear_all_failures_t>(0x49650, 0x428ED);
	p_game_options_setup_default_players(player_count, game_options);
	return;
}
