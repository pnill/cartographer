#pragma once

/* constants */

enum
{
	k_multiplayer_team_count = 8,
};

/* prototypes */

void network_session_interface_patches(void);

bool __cdecl network_session_interface_initialize(class c_network_session_manager* session_manager);
bool __cdecl network_squad_session_set_game_variant(struct s_game_variant* variant);
const wchar_t* network_session_interface_get_session_name(void);
bool network_group_session_get_membership(
	int32* update_number,
	int32* local_peer_index,
	int32* host_peer_index,
	int32* leader_peer_index,
	int32* peer_count,
	struct s_network_session_peer const** peers,
	int32* player_count,
	uint32* player_valid_flags,
	struct s_network_session_player const** players);
bool __cdecl network_session_interface_get_local_user_properties(
	int32 user_index,
	enum e_controller_index* controller_index,
	struct s_player_configuration* player_data,
	uint32* player_voice_settings,
	int32* out_player_text_chat);
bool network_session_interface_set_local_user_character_type(int32 user_index, enum e_character_type character_type);
bool network_session_interface_get_local_user_identifier(int32 user_index, struct s_player_identifier* out_identifier);
int32 network_session_interface_get_team_index(int32 user_index);
void network_session_interface_set_local_user_rank(int32 user_index, int8 rank);
int32 __cdecl network_session_interface_add_local_user(struct s_player_identifier const* user_identifier);
void network_session_interface_set_user_identifier(int32 user_index, struct s_player_identifier const* user_identifier);
bool network_session_get_membership(
	class c_network_session *session,
	int32* update_number,
	int32* local_peer_index,
	int32* host_peer_index,
	int32* leader_peer_index,
	int32* peer_count,
	struct s_network_session_peer const **peers,
	int32* player_count,
	uint32* player_valid_flags,
	struct s_network_session_player const **players);
enum e_network_session_class network_squad_session_get_session_class(void);
bool network_session_is_online(e_network_session_class session_class);

/* globals */

extern bool debug_net_distributed_always;
extern bool debug_net_distributed_never;
