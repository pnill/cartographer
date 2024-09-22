#pragma once
#include "user_interface.h"

bool __cdecl user_interface_globals_is_beta_build();
int32 __cdecl user_interface_globals_get_game_difficulty();
uint32 __cdecl user_interface_globals_get_edit_player_profile_index();
s_saved_game_player_profile* user_interface_globals_get_edit_player_profile();

void __cdecl user_interface_globals_set_game_difficulty_real(int32 difficulty);
void __cdecl user_interface_globals_set_loading_from_persistent_storage(bool a1);
void __cdecl user_interface_globals_commit_edit_profile_changes();
