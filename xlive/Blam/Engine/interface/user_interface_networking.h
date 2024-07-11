#pragma once

bool __cdecl user_interface_create_new_squad(bool a1, bool online);


void __cdecl user_interface_networking_reset_player_counts(void);
void __cdecl user_interface_squad_clear_match_playlist();
void __cdecl user_interface_squad_clear_game_settings();
void __cdecl user_interface_squad_set_campaign_difficulty(int32 difficulty);
void __cdecl user_interface_set_desired_multiplayer_mode(int32 desired_mode);

void user_interface_networking_join_game(XSESSION_INFO* session, int32 a2, bool from_game_invite);
