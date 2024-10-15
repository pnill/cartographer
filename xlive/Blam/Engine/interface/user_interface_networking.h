#pragma once

/* enums */

enum e_session_protocol
{
	_session_protocol_splitscreen_coop = 0x0,
	_session_protocol_splitscreen_custom = 0x1,
	_session_protocol_system_link_coop = 0x2,
	_session_protocol_system_link_custom = 0x3,
	_session_protocol_xbox_live_coop = 0x4,
	_session_protocol_xbox_live_custom = 0x5,
	_session_protocol_xbox_live_optimatch = 0x6,
};

/* forward declations*/

struct s_game_variant;

/* public methods */

bool session_protocol_has_coop(e_session_protocol protocol);
bool __cdecl user_interface_create_new_squad(bool a1, bool online);
bool __cdecl user_interface_squad_local_peer_is_leader();
bool __cdecl user_interface_session_get_map(uint32* campaign_id, uint32* map_id, uint32* custom_map_id);
bool __cdecl user_interface_squad_session_is_xbox_live();
int16 __cdecl user_interface_session_get_campaign_difficulty(void);
int16 __cdecl user_interface_squad_get_player_count();
e_session_protocol __cdecl user_interface_squad_get_active_protocol();
s_game_variant* __cdecl user_interface_session_get_game_variant(void);

void __cdecl user_interface_networking_reset_player_counts(void);
void __cdecl user_interface_squad_clear_match_playlist(void);
void __cdecl user_interface_squad_clear_game_settings();
void __cdecl user_interface_squad_set_campaign_difficulty(int32 difficulty);
void __cdecl user_interface_set_desired_multiplayer_mode(int32 desired_mode);

void user_interface_networking_join_game(XSESSION_INFO* session, int32 a2, bool from_game_invite);
