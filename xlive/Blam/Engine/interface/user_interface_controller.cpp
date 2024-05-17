#include "stdafx.h"
#include "user_interface_controller.h"
#include "user_interface_guide.h"
#include "Networking/online/online_account_xbox.h"
#include "tag_files/global_string_ids.h"

s_user_interface_controller_globals* user_interface_controller_globals_get(void)
{
	return Memory::GetAddress<s_user_interface_controller_globals*>(0x96C858, 0x999038);
}

void __cdecl user_interface_controller_initialize()
{
	INVOKE(0x208608, 0x0, user_interface_controller_initialize);
}

void __cdecl user_interface_controller_update()
{
	INVOKE(0x208C5D, 0x0, user_interface_controller_update);
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

void __cdecl user_interface_controller_set_desired_team_index(e_controller_index controller_index, e_game_team team)
{
	INVOKE(0x2068F2, 0x0, user_interface_controller_set_desired_team_index, controller_index, team);
}

bool __cdecl user_interface_controller_get_rumble_enabled(e_controller_index controller_index)
{
	return INVOKE(0x207600, 0x0, user_interface_controller_get_rumble_enabled, controller_index);
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
	INVOKE(0x206890, 0x0, user_interface_controller_get_profile_data, controller_index, profile, profile_index);
}

void __cdecl user_interface_controller_get_user_properties(e_controller_index controller_index, s_player_identifier* controller_user_identifier, s_player_properties* properties)
{
	INVOKE(0x20696F, 0x0, user_interface_controller_get_user_properties, controller_index, controller_user_identifier, properties);
}

void __cdecl user_interface_controller_event_submit(s_event_record* event)
{
	INVOKE(0x20D0C5, 0x0, user_interface_controller_event_submit, event);
}

void __cdecl user_interface_controller_update_network_properties(e_controller_index controller_index)
{
	INVOKE(0x206A97, 0x0, user_interface_controller_update_network_properties, controller_index);
}

void guest_name_signin_fix(e_controller_index controller_index)
{
	// INVOKE(0x208312, 0x0, guest_sign_in_name_fix, controller_index);

	s_user_interface_controller* controller = &user_interface_controller_globals_get()->controllers[controller_index];
	c_user_interface_guide_state_manager* guide = user_interface_guide_state_manager_get();
	if (guide->m_sign_in_state == eXUserSigninState_SignedInToLive)
	{
		XUID* controller_xuid = (XUID*)(&controller->controller_user_identifier);
		if (online_xuid_is_guest_account(*controller_xuid))
		{
			uint8 guest_no = online_xuid_get_guest_account_number(*controller_xuid);
			c_static_wchar_string32 format;
			global_string_resolve_stringid_to_value(HS_GUEST_OF_ASCII_GAMERTAG_UNICODE_FORMAT_STRING, format.get_buffer());// %d %hs
			swprintf(controller->player_name.get_buffer(),
				controller->player_name.max_length(),
				format.get_string(),
				guest_no,
				guide->m_gamertag);

		}
		else
		{
			swprintf(controller->player_name.get_buffer(),
				controller->player_name.max_length(),
				L"%hs",
				guide->m_gamertag);
		}
	}
	else if (user_interface_controller_is_player_profile_valid(controller_index))
	{
		controller->player_name.set(controller->player_profile.player_name.get_string());
	}
	else
	{
		controller->player_name.clear();
	}
	user_interface_controller_update_network_properties(controller_index);
}

void user_inteface_controller_apply_patches()
{
	PatchCall(Memory::GetAddress(0x20887A), guest_name_signin_fix); // fixes guest-signin names in ONLINE mode
}


