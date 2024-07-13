#include "stdafx.h"

#include "user_interface_globals.h"
#include "interface/user_interface.h"


bool __cdecl user_interface_globals_is_beta_build()
{
	return INVOKE(0x209ED8, 0x0, user_interface_globals_is_beta_build);
}

int32 __cdecl user_interface_globals_get_game_difficulty()
{
	return INVOKE(0x209E98, 0x0, user_interface_globals_get_game_difficulty);
}

void __cdecl user_interface_globals_set_game_difficulty_real(int32 difficulty)
{
	INVOKE(0x209E44, 0x0, user_interface_globals_set_game_difficulty_real, difficulty);
}

void __cdecl user_interface_globals_set_loading_from_persistent_storage(bool a1)
{
	INVOKE(0x209E6C, 0x0, user_interface_globals_set_loading_from_persistent_storage, a1);
}

void* __cdecl c_screen_bungie_news_load(s_screen_parameters* params)
{
	return INVOKE(0xB101, 0x0, c_screen_bungie_news_load, params);
}

void* __cdecl c_screen_network_squad_browser_load(s_screen_parameters* params)
{
	return INVOKE(0x21A238, 0x0, c_screen_network_squad_browser_load, params);
}

void* __cdecl c_screen_single_player_level_select_load_lobby(s_screen_parameters* parameters)
{
	return INVOKE(0x213552, 0x0, c_screen_single_player_level_select_load_lobby, parameters);
}

void* __cdecl c_screen_single_player_difficulty_select_load_lobby(s_screen_parameters* parameters)
{
	return INVOKE(0x24201D, 0x0, c_screen_single_player_difficulty_select_load_lobby, parameters);
}
