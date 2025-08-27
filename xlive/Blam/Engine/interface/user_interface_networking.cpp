#include "stdafx.h"
#include "user_interface_networking.h"

#include "user_interface.h"
#include "user_interface_controller.h"

#include "game/player_constants.h"
#include "main/map_repository.h"
#include "networking/logic/life_cycle_manager.h"
#include "networking/session/network_session.h"

/* typedef */
typedef e_session_protocol(__cdecl* t_user_interface_squad_get_active_protocol)();
t_user_interface_squad_get_active_protocol p_user_interface_squad_get_active_protocol;

/* private code */

bool* byte_D6840E_get(void)
{
	return Memory::GetAddress<bool*>(0x46EE9F);
}

XSESSION_INFO* global_session_info_get(void)
{
	return Memory::GetAddress<XSESSION_INFO*>(0x46EEA4);
}

int32* dword_86EEE0_get(void)
{
	return Memory::GetAddress<int32*>(0x46EEE0);
}

bool* from_game_invite_global_get(void)
{
	return Memory::GetAddress<bool*>(0x46EEA0);
}

/* public code */

void user_interface_networking_apply_patches()
{
	DETOUR_ATTACH(p_user_interface_squad_get_active_protocol, Memory::GetAddress<t_user_interface_squad_get_active_protocol>(0x215EA9, 0x1FD528), user_interface_squad_get_active_protocol);
}

bool session_protocol_has_coop(e_session_protocol protocol)
{
	return protocol == _session_protocol_splitscreen_coop
		|| protocol == _session_protocol_system_link_coop
		|| protocol == _session_protocol_xbox_live_coop;
}

bool __cdecl user_interface_create_new_squad(bool a1, bool online)
{
	return INVOKE(0x216345, 0x0, user_interface_create_new_squad, a1, online);
}

bool __cdecl user_interface_squad_local_peer_is_leader()
{
	return INVOKE(0x2152B0, 0x0, user_interface_squad_local_peer_is_leader);
}

bool __cdecl user_interface_session_get_map(uint32* campaign_id, uint32* map_id, uint32* custom_map_id)
{
	return INVOKE(0x21564E, 0x0, user_interface_session_get_map, campaign_id, map_id, custom_map_id);
}

bool __cdecl user_interface_squad_session_is_xbox_live()
{
	return INVOKE(0x2156B9, 0x0, user_interface_squad_local_peer_is_leader);
}

int16 __cdecl user_interface_session_get_campaign_difficulty(void)
{
	return INVOKE(0x215697, 0x0, user_interface_session_get_campaign_difficulty);
}

int16 __cdecl user_interface_squad_get_player_count()
{
	return INVOKE(0x21525A, 0x0, user_interface_squad_get_player_count);
}

e_session_protocol __cdecl user_interface_squad_get_active_protocol()
{
	e_network_session_class session_class = network_squad_session_get_session_class();

	if (session_class < _network_session_class_offline)
		return _session_protocol_invalid;

	c_network_session* session = NULL;


	if (network_life_cycle_in_squad_session(&session))
	{
		s_game_variant* session_variant = &session->m_session_parameters.game_variant;
		uint32 unk_1;
		uint32 unk_2;
		uint32 unk_3;
		s_custom_map_id* custom_map_id;

		if (!network_session_get_scenario_id(&unk_1, &unk_2, &unk_3, &custom_map_id))
		{
			unk_1 = UINT_MAX;
			unk_2 = UINT_MAX;
			custom_map_id = nullptr;
		}
		if (session_class <= _network_session_class_system_link)
		{
			if (unk_1 != UINT_MAX)
				return session_class != 0 ? _session_protocol_system_link_coop : _session_protocol_splitscreen_coop;

			if (session_variant)
			{
				switch (session_variant->variant_game_engine_index)
				{
				case _game_engine_type_ctf:
				case _game_engine_type_slayer:
				case _game_engine_type_oddball:
				case _game_engine_type_koth:
				case _game_engine_type_race:
				case _game_engine_type_headhunter:
				case _game_engine_type_juggernaut:
				case _game_engine_type_territories:
				case _game_engine_type_assault:
				{
					return (e_session_protocol)(2 * (session_class != _network_session_class_offline) + 1);
				}
				default:
					return _session_protocol_invalid;
				}
			}
			return _session_protocol_invalid;
		}

		if (session_class != _network_session_class_xbox_live || session->m_session_parameters.field_4C90 != 1)
			return _session_protocol_invalid;

		if (unk_1 != UINT_MAX)
			return _session_protocol_xbox_live_coop;

		if (!session_variant)
			return _session_protocol_invalid;


		switch (session_variant->variant_game_engine_index)
		{
		case _game_engine_type_ctf:
		case _game_engine_type_slayer:
		case _game_engine_type_oddball:
		case _game_engine_type_koth:
		case _game_engine_type_race:
		case _game_engine_type_headhunter:
		case _game_engine_type_juggernaut:
		case _game_engine_type_territories:
		case _game_engine_type_assault:
		{
			return _session_protocol_xbox_live_custom;
		}
		default:
			return _session_protocol_invalid;
		}
	}

	return _session_protocol_invalid;
	//return INVOKE(0x215EA9, 0x0, user_interface_squad_get_active_protocol);
}

bool user_interface_game_settings_set_game_variant(s_game_variant* variant)
{
	return INVOKE(0x216901, 0, user_interface_game_settings_set_game_variant, variant);
}

s_game_variant* __cdecl user_interface_session_get_game_variant(void)
{
	return INVOKE(0x215692, 0x0, user_interface_session_get_game_variant);
}

void user_interface_networking_set_globals(bool a1, XSESSION_INFO* session, int32 unused, bool from_game_invite)
{
	*byte_D6840E_get() = a1;
	*global_session_info_get() = *session;
	*dword_86EEE0_get() = unused;
	*from_game_invite_global_get() = from_game_invite;
	return;
}

void __cdecl user_interface_networking_reset_player_counts(void)
{
	INVOKE(0xB8B9, 0x219B9, user_interface_networking_reset_player_counts);
	return;
}

void __cdecl user_interface_networking_open_join_screen(void)
{
	INVOKE(0x242E5B, 0x0, user_interface_networking_open_join_screen);
	return;
}

void __cdecl sub_5AD5AB(XSESSION_INFO* session, XNKEY* key, XNADDR* host_address, int32 local_player_count, s_player_identifier* user_identifiers, wchar_t** player_names, bool from_game_invite)
{
	INVOKE(0x1AD5AB, 0x0, sub_5AD5AB, session, key, host_address, local_player_count, user_identifiers, player_names, from_game_invite);
	return;
}

void __cdecl user_interface_squad_clear_match_playlist(void)
{
	INVOKE(0x209DF4, 0x0, user_interface_squad_clear_match_playlist);
	return;
}

void __cdecl user_interface_squad_clear_game_settings()
{
	INVOKE(0x2171A0, 0x0, user_interface_squad_clear_game_settings);
}

void __cdecl user_interface_squad_set_campaign_difficulty(int32 difficulty)
{
	INVOKE(0x215624, 0x0, user_interface_squad_set_campaign_difficulty, difficulty);
}

void __cdecl user_interface_set_desired_multiplayer_mode(int32 desired_mode)
{
	INVOKE(0x217138, 0x0, user_interface_set_desired_multiplayer_mode, desired_mode);
}


void user_interface_networking_join_game(XSESSION_INFO* session, int32 user_index, bool from_game_invite)
{
	//INVOKE(0x2161E1, 0x1FD827, user_interface_networking_join_game, session_info, a2, from_game_invite);
	int32 local_player_count;
	wchar_t player_names[k_number_of_users][32];
	s_player_identifier user_identifiers[k_number_of_users];
	wchar_t* p_player_names[k_number_of_users];

	if (user_interface_controller_get_signed_in_controller_count() <= 0)
	{
		user_interface_networking_set_globals(true, session, user_index, from_game_invite);
		screen_error_ok_dialog_show(_user_interface_channel_type_game_error, _ui_error_invite_requires_signin, _window_4, FLAG(user_index), user_interface_controller_pick_profile_offline, nullptr);
	}
	else
	{
		c_game_life_cycle_handler_joining::check_joining_capability();
		local_player_count = 0;
		do
		{
			s_player_properties player_properties;
			if (network_session_interface_get_local_user_identifier(local_player_count, &user_identifiers[local_player_count])
				&& network_session_interface_get_local_user_properties(local_player_count, NULL, &player_properties, NULL, NULL))
			{
				p_player_names[local_player_count] = player_names[local_player_count];
				wcsncpy_s(player_names[local_player_count], 32, player_properties.player_name, UINT_MAX);
			}
			++local_player_count;
		} while (local_player_count < k_number_of_users);

		user_interface_networking_reset_player_counts();
		sub_5AD5AB(session, &session->keyExchangeKey, &session->hostAddress, local_player_count, user_identifiers, p_player_names, from_game_invite);
		user_interface_networking_open_join_screen();

		return;
	}
}