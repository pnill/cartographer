#include "stdafx.h"

#include "GameGlobals.h"

s_game_globals* s_game_globals::get()
{
	return *Memory::GetAddress<s_game_globals**>(0x482D3C, 0x4CB520);
}

bool s_game_globals::map_initialized()
{
	return get() && get()->map_active && get()->sbsp_index != 0xFFFF;
}

s_game_options* s_game_globals::get_game_options()
{
	return &get()->m_options;
}

s_game_variant* s_game_globals::get_game_variant()
{
	return &get_game_options()->m_game_variant;
}

bool s_game_globals::game_is_campaign()
{
	return get_game_options()->m_engine_type == e_engine_type::SinglePlayer;
}

bool s_game_globals::game_is_multiplayer()
{
	return get_game_options()->m_engine_type == e_engine_type::Multiplayer;
}

bool s_game_globals::game_is_mainmenu()
{
	return get_game_options()->m_engine_type == e_engine_type::MainMenu;
}