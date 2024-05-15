#include "stdafx.h"
#include "user_interface_controller.h"

s_user_interface_controller_globals* user_interface_controller_globals_get(void)
{
    return Memory::GetAddress<s_user_interface_controller_globals*>(0x96C858, 0x999038);
}

bool __cdecl user_interface_controller_is_player_profile_valid(e_controller_index controller_index)
{
	return INVOKE(0x206B50, 0x1F3F78, user_interface_controller_is_player_profile_valid, controller_index);
}

uint32 __cdecl user_interface_controller_get_next_valid_index(e_controller_index controller_index)
{
	return INVOKE(0x206B13, 0x1F3F3A, user_interface_controller_get_next_valid_index, controller_index);
}

uint32 __cdecl user_interface_controller_get_user_index(e_controller_index controller_index)
{
	return INVOKE(0x20687F, 0x1F3CE8, user_interface_controller_get_user_index, controller_index);
}

void __cdecl user_interface_controller_set_user_index(e_controller_index controller_index, uint32 user_index)
{
	INVOKE(0x207342, 0x1F43F2, user_interface_controller_set_user_index, controller_index, user_index);
}

e_controller_index __cdecl user_interface_controller_get_controller_for_user(uint32 user_index)
{
	return INVOKE(0x207365, 0x1F4415, user_interface_controller_get_controller_for_user, user_index);
}

e_game_team __cdecl user_interface_controller_get_user_active_team(e_controller_index controller_index)
{
	return INVOKE(0x206907, 0, user_interface_controller_get_user_active_team, controller_index);
}

void __cdecl user_interface_controller_set_desired_team_index(e_controller_index gamepad_index, e_game_team team)
{
	return INVOKE(0x2068F2, 0x0, user_interface_controller_set_desired_team_index, gamepad_index, team);
}

bool __cdecl user_interface_controller_get_rumble_enabled(e_controller_index gamepad_index)
{
    return INVOKE(0x207600, 0x0, user_interface_controller_get_rumble_enabled, gamepad_index);
}

bool __cdecl user_interface_controller_get_autolevel_enabled(e_controller_index controller_index)
{
	return INVOKE(0x207627, 0, user_interface_controller_get_autolevel_enabled, controller_index);
}

e_handicap __cdecl user_interface_controller_get_user_handicap_level(e_controller_index controller_index)
{
	return INVOKE(0x206938, 0, user_interface_controller_get_user_handicap_level, controller_index);
}

void __cdecl user_interface_controller_set_user_handicap_level(e_controller_index controller_index, e_handicap handicap)
{
	INVOKE(0x206923, 0, user_interface_controller_set_user_handicap_level, controller_index, handicap);
}

void __cdecl user_interface_controller_set_griefer(e_controller_index controller_index, bool griefing)
{
	INVOKE(0x206949, 0, user_interface_controller_set_griefer, controller_index, griefing);
}


wchar_t* __cdecl user_interface_controller_get_player_profile_name(e_controller_index controller_index)
{
	return INVOKE(0x206B67, 0, user_interface_controller_get_player_profile_name, controller_index);
}

uint32 __cdecl user_interface_controller_get_signed_in_controller_count()
{
	return INVOKE(0x2073AE, 0, user_interface_controller_get_signed_in_controller_count);
}

uint32 __cdecl user_interface_controller_get_signed_in_controllers_mask()
{
	return INVOKE(0x20758D, 0, user_interface_controller_get_signed_in_controllers_mask);
}

uint32 __cdecl user_interface_controller_get_last_level_played(e_controller_index controller_index)
{
	return INVOKE(0xFE106, 0, user_interface_controller_get_last_level_played, controller_index);
}

uint32 __cdecl user_interface_controller_get_highest_campaign_level_in_signed_in_controllers()
{
	return INVOKE(0x2076F7, 0, user_interface_controller_get_highest_campaign_level_in_signed_in_controllers);
}

void __cdecl user_interface_controller_sign_out(e_controller_index controller_index)
{
	INVOKE(0x208257, 0x1F491B, user_interface_controller_sign_out, controller_index);
}

void __cdecl user_interface_controller_sign_out_all_controllers()
{
	INVOKE(0x208A28, 0x1F4E9F, user_interface_controller_sign_out_all_controllers);
}

void user_interface_controller_get_profile_data(e_controller_index controller_index, s_saved_game_file_player_profile* profile, uint32* profile_index)
{
	INVOKE_TYPE(0x206890, 0x0, void(__cdecl*)(e_controller_index, s_saved_game_file_player_profile*, uint32*), controller_index, profile, profile_index);
}

void __cdecl user_interface_controller_update_user_session_data(e_controller_index gamepad_index)
{
    return INVOKE(0x206A97, 0x0, user_interface_controller_update_user_session_data, gamepad_index);
}


