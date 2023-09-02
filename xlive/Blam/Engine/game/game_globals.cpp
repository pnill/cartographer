#include "stdafx.h"

#include "game_globals.h"

s_main_game_globals* s_main_game_globals::get()
{
	return *Memory::GetAddress<s_main_game_globals**>(0x482D3C, 0x4CB520);
}

bool s_main_game_globals::map_initialized()
{
	return get() && get()->map_active && get()->active_structure_bsp_index != 0xFFFF;
}

s_game_options* s_main_game_globals::get_game_options()
{
	return &get()->options;
}

s_game_variant* s_main_game_globals::get_game_variant()
{
	return &get_game_options()->game_variant;
}

bool s_main_game_globals::game_is_campaign()
{
	return get_game_options()->game_mode == _game_mode_campaign;
}

bool s_main_game_globals::game_is_multiplayer()
{
	return get_game_options()->game_mode == _game_mode_multiplayer;
}

bool s_main_game_globals::game_is_mainmenu()
{
	return get_game_options()->game_mode == _game_mode_ui_shell;
}

bool s_main_game_globals::game_is_predicted()
{
	return get_game_options()->simulation_type == _game_simulation_distributed_client;
}

bool s_main_game_globals::game_is_in_progress()
{
	return get() && get()->game_in_progress;
}

e_game_mode get_current_engine_type()
{
	return s_main_game_globals::get()->options.game_mode;
}