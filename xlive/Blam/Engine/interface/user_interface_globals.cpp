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

void __cdecl user_interface_globals_commit_edit_profile_changes()
{
	INVOKE(0x209A98, 0x0, user_interface_globals_commit_edit_profile_changes);
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

void* __cdecl c_screen_press_start_introduction_load(s_screen_parameters* parameters)
{
	return INVOKE(0x23F6B7, 0x0, c_screen_press_start_introduction_load, parameters);
}

void* __cdecl c_screen_single_player_profile_select_fancy_load(s_screen_parameters* parameters)
{
	return INVOKE(0x21EF9C, 0x0, c_screen_single_player_profile_select_fancy_load, parameters);
}

void* __cdecl c_screen_game_engine_category_load(s_screen_parameters* parameters)
{
	return INVOKE(0x21F16E, 0x0, c_screen_game_engine_category_load, parameters);
}

void* __cdecl c_screen_video_settings_load(s_screen_parameters* parameters)
{
	return INVOKE(0x21EDC7, 0x0, c_screen_video_settings_load, parameters);
}

void* __cdecl c_screen_audio_settings_load(s_screen_parameters* parameters)
{
	return INVOKE(0x21ED20, 0x0, c_screen_audio_settings_load, parameters);
}

void* __cdecl c_screen_network_adapter_menu_load(s_screen_parameters* parameters)
{
	return INVOKE(0x21F5F8, 0x0, c_screen_network_adapter_menu_load, parameters);
}

void* __cdecl c_screen_about_dialog_menu_load(s_screen_parameters* parameters)
{
	return INVOKE(0x21F681, 0x0, c_screen_about_dialog_menu_load, parameters);
}