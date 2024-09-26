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

uint32 user_interface_globals_get_edit_player_profile_index()
{
	return INVOKE(0x209BA3, 0, user_interface_globals_get_edit_player_profile_index);
}

s_saved_game_player_profile* user_interface_globals_get_edit_player_profile()
{
	return INVOKE(0x209B9D, 0, user_interface_globals_get_edit_player_profile);
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
