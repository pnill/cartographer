#include "stdafx.h"
#include "user_interface_globals.h"

#include "scenario/scenario_definitions.h"

bool __cdecl user_interface_globals_is_beta_build()
{
	return INVOKE(0x209ED8, 0x0, user_interface_globals_is_beta_build);
}

int32 __cdecl user_interface_globals_get_game_difficulty()
{
	return INVOKE(0x209E98, 0x0, user_interface_globals_get_game_difficulty);
}

int32 __cdecl user_interface_globals_get_edit_player_profile_index()
{
	return INVOKE(0x209BA3, 0, user_interface_globals_get_edit_player_profile_index);
}

s_saved_game_player_profile* __cdecl user_interface_globals_get_edit_player_profile()
{
	return INVOKE(0x209B9D, 0, user_interface_globals_get_edit_player_profile);
}

e_scenario_type __cdecl user_interface_globals_get_map_type()
{
	return INVOKE(0x20B8BB, 0x0, user_interface_globals_get_map_type);
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

void __cdecl user_interface_globals_save_profile_changes_to_disk()
{
	INVOKE(0x209C3E, 0x0, user_interface_globals_save_profile_changes_to_disk);
}

void __cdecl user_interface_globals_finish_saving_profile_changes()
{
	INVOKE(0x209D08, 0x0, user_interface_globals_finish_saving_profile_changes);
}

void __cdecl user_interface_globals_set_edit_player_profile(e_controller_index controller_index, uint32 profile_index, s_saved_game_player_profile* profile)
{
	INVOKE(0x209B72, 0x0, user_interface_globals_set_edit_player_profile, controller_index, profile_index, profile);
}

void __cdecl user_interface_globals_save_edit_profile_to_disk()
{
	INVOKE(0x209C3E, 0, user_interface_globals_save_edit_profile_to_disk);
}

s_user_interface_globals* user_interface_globals_get(void)
{
	return Memory::GetAddress<s_user_interface_globals*>(0x9718E0);
}
