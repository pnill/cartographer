#pragma once
#include "user_interface.h"

bool __cdecl user_interface_globals_is_beta_build();
int32 __cdecl user_interface_globals_get_game_difficulty();
void __cdecl user_interface_globals_set_game_difficulty_real(int32 difficulty);
void __cdecl user_interface_globals_set_loading_from_persistent_storage(bool a1);

//TODO : move these to their files
void* __cdecl c_screen_bungie_news_load(s_screen_parameters* params);
void* __cdecl c_screen_network_squad_browser_load(s_screen_parameters* params);
void* __cdecl c_screen_single_player_level_select_load_lobby(s_screen_parameters* parameters);
void* __cdecl c_screen_single_player_difficulty_select_load_lobby(s_screen_parameters* parameters);
