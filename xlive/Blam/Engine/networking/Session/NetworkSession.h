#pragma once
#include "game/players.h"
#include "input/controllers.h"
#include "networking/Transport/transport.h"
#include "networking/Transport/network_observer.h"
#include "saved_games/game_variant.h"

// ### TODO Cleanup

enum e_network_session_map_status : int32
{
	_network_session_map_status_none,
	_network_session_map_status_unable_to_precache,
	_network_session_map_status_precaching,
	_network_session_map_status_precached,
	_network_session_map_status_loaded,
	_network_session_map_status_downloading,

	k_network_session_map_status_count
}; 

enum e_network_session_state : int32
{
	_network_session_state_none,
	_network_session_state_peer_joining,
	_network_session_state_peer_join_abort,
	_network_session_state_peer_established,
	_network_session_state_peer_leaving,
	_network_session_state_host_established,
	_network_session_state_host_disband,
	_network_session_state_host_handoff,
	_network_session_state_host_reestablish,
	_network_session_state_election,
	_network_session_state_dead,

	k_network_session_state_count
};

// forward declarations
struct c_network_session;
struct s_session_membership;
struct s_membership_player;
struct s_membership_peer;
struct s_session_observer_channel;

#define k_network_maximum_machines_per_session (k_maximum_players + 1)

namespace NetworkSession
{
	// network session getters and misc
	c_network_session* GetNetworkSessions();
	c_network_session* GetActiveNetworkSession();
	bool GetActiveNetworkSession(c_network_session** outSession);
	e_network_session_state GetLocalSessionState();
	int32 GetPeerIndexFromNetworkAddress(const network_address* address);
	bool GetMapFileLocation(wchar_t* buffer, size_t size);

	bool LocalPeerIsSessionHost();
	bool LocalPeerIsEstablishedClient();
	bool LocalPeerIsSessionLeader();
	bool LocalPeerIsEstablished();

	// peer functions
	int32 GetPeerCount();
	int32 GetLocalPeerIndex();
	bool IsPeerIndexLocal(int32 peer_index);
	IN_ADDR GetLocalNetworkAddress();
	void KickPeer(int32 peer_index);
	void EndGame();
	s_session_observer_channel* GetPeerObserverChannel(int32 peer_index);

	// peer-player functions
	int32 GetPeerIndex(datum player_index);
	bool IsPlayerLocal(datum player_index);

	// player functions
	
	/* Use this to verify if a player is currently active in the network session */
	/* Otherwise you will wonder why you don't get the right data/player index etc. */
	bool PlayerIsActive(datum player_index);
	std::vector<uint64> GetActivePlayerIdList();
	std::vector<int32> GetActivePlayerIndicesList();

	int32 GetPlayerCount();
	const wchar_t* GetPlayerName(datum player_index);
	uint64 GetPlayerId(datum player_index);

	int8 GetPlayerTeam(datum player_index);
	int32 GetPeerIndexFromId(uint64 xuid);
	s_membership_player* GetPlayerInformation(datum player_index);

	wchar_t* GetGameVariantName();

	bool IsVariantTeamPlay();
	void LeaveSession();
}

s_membership_peer* network_session_get_peer(int32 peer_index);

#pragma pack(push, 1)
struct s_session_observer_channel
{
	bool field_0;
	bool field_1;
	char pad[2];
	int32 observer_index;
	int32 membership_update_number;
	int32 parameters_update_number;
	int32 virtual_couch_update_number;
	int32 vote_update_number;
	int32 field_18;
};
ASSERT_STRUCT_SIZE(s_session_observer_channel, 28);

struct s_session_virtual_couch
{
	uint32 incremental_update_number;
	bool exists;
	XSESSION_INFO xsession_info;
	char pad[3];
	uint32 xuid_count;
	uint64 player_ids[k_maximum_players];
};
ASSERT_STRUCT_SIZE(s_session_virtual_couch, 200);

struct s_session_parameters
{
	uint32 parameters_update_number;
	uint8 gap_4C64[4];
	int32 session_mode;
	uint32 gap_4C6C;
	uint32 system_language_id;
	bool dedicated_server;
	char pad_1[3];
	int32 party_privacy;
	int32 max_peers;
	int32 max_party_players;
	int32 gap_4C84;
	int32 field_4C88;
	int32 field_4C8C;
	uint32 field_4C90;
	uint32 field_4C94;
	uint8 gap_4C98[4];
	bool current_peer_is_host;
	char pad[3];
	uint32 field_4CA0;
	uint32 field_4CA4;
	uint8 gap_4CA8[16];
	uint32 field_4CB8;
	wchar_t scenario_path[128];
	uint8 gap4C00[12];
	uint8 random_bytes[8];
	uint32 dead_beef_lol;
	uint32 field_4DD4;
	uint32 field_4DD8;
	wchar_t custom_map_name[32];
	uint8 custom_map_sha256_hash[32];
	s_game_variant game_variant;
	uint8 gap_4E40[404];
	uint32 field_5100;
	uint8 gap_5104[3508];
	uint32 field_5EB8;
	uint8 gap_5EBC[76];
	uint32 field_5F08;
	uint32 field_5F0C;
};
ASSERT_STRUCT_SIZE(s_session_parameters, 4784);

struct s_session_vote
{
	char gap_0[160];
};
ASSERT_STRUCT_SIZE(s_session_vote, 160);

struct s_membership_peer
{
	XNADDR secure_address;
	uint8 gap_24[4];
	wchar_t name[16];
	wchar_t peer_session_name[32];
	e_network_session_map_status map_status;
	uint32 map_progress_percentage;
	char field_70;
	uint8 gap_71[3];
	char field_74;
	uint8 gap_75[3];
	uint32 nat_type;
	uint32 connectivity_mask;
	uint32 latency_min;
	uint32 latency_est;
	uint32 latency_max;
	uint8 gap_9C[68];
	uint32 field_F0;
	uint32 field_F4;
	uint32 update_number;
	datum local_players_indexes[k_number_of_users];
};
ASSERT_STRUCT_SIZE(s_membership_peer, 268);

struct s_membership_player
{
	uint64 identifier; // -0xA
	int32 peer_index; // -0x8
	int32 peer_user_index; // -0x4
	WORD player_flags; // 0x0
	bool properties_valid;
	char pad[1];
	int32 controller_index;
	s_player_properties properties[2];
	uint32 player_voice;
	uint32 player_text_chat;
};
ASSERT_STRUCT_OFFSET(s_membership_player, controller_index, 20);
ASSERT_STRUCT_SIZE(s_membership_player, 296);

struct s_session_membership
{
	uint32 update_number; // 0x70
	int32 session_leader_peer_index; // 0x74
	uint64 dedicated_server_xuid; // 0x78
	int32 xbox_session_leader_peer_index; // 0x80
	int32 peer_count; // 0x84
	s_membership_peer peers[k_network_maximum_machines_per_session]; // 0x88
	int32 player_count; // 0x1254
	uint32 players_active_mask; // 0x1258
	s_membership_player players[k_maximum_players]; // 0x125C
	uint32 unk;
};
ASSERT_STRUCT_SIZE(s_session_membership, 9328);


struct s_session_interface_user
{
	bool user_exists;
	s_player_identifier network_user_identifier;
	uint8 gap_B[3];
	e_controller_index controller_index;
	s_player_properties properties;
	int32 player_voice_exists;
	int32 player_text_chat_exists;
	uint32 field_A0[2];
	uint32 field_A8[2];
	uint32 field_B0[2];
	char field_B4[4];
};
ASSERT_STRUCT_SIZE(s_session_interface_user, 0xB8);

struct s_session_interface_globals
{
	bool initialised;
	uint8 gap_1;
	wchar_t m_machine_name[16];
	wchar_t m_session_name[32];
	uint8 m_qos_active;
	uint8 gap_63;
	uint8 gap_64[16];
	uint32 m_upstream_bandwidth_bps;
	uint32 m_downstream_bandwidth_bps;
	uint8 gap_7C[8];
	uint32 nat_type;
	uint32 field_88;
	uint32 field_8C;
	uint32 field_90;
	int32 current_map_progress_percentage;
	s_session_interface_user users[k_number_of_users];
	uint32 session_connection_identifiers[6];
	s_game_variant variants[2];
	uint8 gap_5F0[68];
	uint32 sessions_manager;
	
	static s_session_interface_globals* get();
};
ASSERT_STRUCT_SIZE(s_session_interface_globals, 0x638);

struct c_network_session
{
	void* vtbl;
	void* p_network_message_gateway;
	c_network_observer* p_network_observer;
	void* session_manager_ptr;
	uint32 text_chat;
	int32 session_index;
	int32 field_18;
	int32 network_protocol; // LIVE - 2, Network - 1
	XNKID session_id;
	wchar_t field_28[16];
	char field_48;
	XNKEY xnkey;
	char pad[3];
	int32 xnkey_index;
	int32 field_60;
	int32 session_host_peer_index;
	int32 elected_host_peer_index;
	uint32 field_6C;
	s_session_membership membership[2]; // 0x70
	s_session_virtual_couch virtual_couch[2];
	s_session_vote voting_data[2]; // unused
	uint8 gap_4B84[64];
	s_session_parameters parameters[2];
	int32 local_peer_index;
	s_session_observer_channel observer_channels[k_network_maximum_machines_per_session];
	e_network_session_state local_session_state;
	uint32 time_unk_2;
	uint32 time_unk_3;
	uint32 time_unk;
	uint32 possible_new_peer_host_index;
	uint32 field_73B4;
	uint32 field_73B8;
	int32 field_73BC;
	char field_73C0[4];
	uint32 time_new_session_host_found;
	int32 field_73C8;
	uint32 field_73CC;
	int32 field_73D0;
	int32 field_73D4;
	int32 field_73D8;
	int32 field_73DC;
	uint8 field_73E0[32];
	int32 field_7400;
	int32 field_7404;
	uint8 gap_7408[4];
	int32 field_740C;
	int32 field_7410;
	uint8 gap_7414[100];
	uint32 field_7478;
	uint32 field_747C;
	uint32 field_7480;
	uint32 field_7484;
	uint8 gap_7488[1132];
	int32 field_78F4;
	int32 field_78F8;
	uint8 field_78FC[36];
	int32 field_7920;
	uint32 field_7924;
	uint32 total_join_request_attempts;
	uint32 last_time_join_request_sent;
	uint32 local_membership_update_number;
	uint32 field_7934;
	uint32 field_7938;
	int32 field_793C;
	int32 field_7940;
	int32 field_7944;
	int32 field_7948;
	int32 field_794C;
	int32 field_7950;
	int32 field_7954;
	int32 field_7958;
	int32 field_795C;
	uint32 field_7960;
	uint32 field_7964;
	uint32 field_7968;
	char field_796C[4];
	int32 field_7970;
	uint32 field_7974;
	uint32 field_7978;
	uint8 gap_797C[508];
	void* c_kablam_session_join_request_handler; // dedicated server session join handler
	char field_7B7C[12];

	s_membership_peer* get_peer_membership(int32 peer_index)
	{
		return &membership[0].peers[peer_index];
	}

	s_membership_player* get_player_membership(datum player_index)
	{
		return &membership[0].players[DATUM_INDEX_TO_ABSOLUTE_INDEX(player_index)];
	}

	int32 get_local_peer_index() const
	{
		return local_peer_index;
	}

	bool peer_index_local_peer(int32 peer_index) const
	{
		return get_local_peer_index() == peer_index;
	}

	bool local_state_none() const
	{
		return local_session_state == _network_session_state_none;
	}

	bool local_state_established_client() const
	{
		return local_session_state == _network_session_state_peer_established
			|| local_session_state == _network_session_state_peer_leaving;
	}

	bool local_state_session_host() const
	{
		return local_session_state == _network_session_state_host_established
			|| local_session_state == _network_session_state_host_disband
			|| local_session_state == _network_session_state_host_handoff
			|| local_session_state == _network_session_state_host_reestablish;
	}

	bool local_state_joining_session() const
	{
		return local_session_state == _network_session_state_peer_joining;
	}

	bool local_state_established() const
	{
		bool result = false;

		switch (local_session_state)
		{
		case _network_session_state_none:
		case _network_session_state_peer_joining:
		case _network_session_state_peer_join_abort:
		case _network_session_state_election:
		case _network_session_state_dead:
			result = false;
			break;

		case _network_session_state_peer_established:
		case _network_session_state_peer_leaving:
		case _network_session_state_host_established:
		case _network_session_state_host_disband:
		case _network_session_state_host_handoff:
		case _network_session_state_host_reestablish:
			result = true;
			break;
		}

		return result;
	}

	bool local_peer_session_leader() const
	{
		return membership[0].session_leader_peer_index == local_peer_index;
	}

	bool session_player_active(datum player_index) const
	{
		return TEST_BIT(membership[0].players_active_mask, DATUM_INDEX_TO_ABSOLUTE_INDEX(player_index));
	}

	int32 get_peer_index_from_address(const network_address* address)
	{
		return INVOKE_BY_TYPE(0x1C71DF, 0x19E9CF, int32(__thiscall*)(c_network_session*, const network_address*), this, address);
	}

	void request_membership_update()
	{
		this->membership[0].update_number++;
		this->local_membership_update_number++;
	}

	void switch_player_team(datum player_index, e_game_team team_index)
	{
		if (local_state_session_host())
		{
			get_player_membership(player_index)->properties[0].team_index = team_index;
			request_membership_update();
		}
	}

	// switch multiple players with a single membership update
	void switch_players_to_teams(datum* player_indexes, int32 player_count, e_game_team* team_indexes)
	{
		if (local_state_session_host())
		{
			for (int32 i = 0; i < player_count; i++)
			{
				get_player_membership(player_indexes[i])->properties[0].team_index = team_indexes[i];
			}
			request_membership_update();
		}
	}

	const char* describe_network_protocol_type() const
	{
		static const char* network_protocols_str[] =
		{
			"<disconnected>",
			"System-Link",
			"LIVE",
		};

		return this->network_protocol <= 2 ? network_protocols_str[this->network_protocol] : "<unknown>";
	}
};
ASSERT_STRUCT_SIZE(c_network_session, 31624);
ASSERT_STRUCT_OFFSET(c_network_session, membership[0], 0x70);
ASSERT_STRUCT_OFFSET(c_network_session, parameters[0], 0x4C60);
#pragma pack(pop)

s_session_interface_user* session_interface_get_local_user_properties(int32 user_index);

void network_session_apply_patches();

bool network_life_cycle_in_squad_session(c_network_session** out_active_session);

void network_session_membership_update_local_players_teams();

bool network_session_interface_set_local_user_character_type(int32 user_index, e_character_type character_type);
bool network_session_interface_get_local_user_identifier(int32 user_index, s_player_identifier* out_identifier);
void network_session_interface_set_local_user_rank(int32 user_index, int8 rank);
bool __cdecl network_session_interface_get_local_user_properties(int32 user_index, int32* out_controller_index, s_player_properties* out_properties, int32* out_player_voice, int32* out_player_text_chat);
void __cdecl network_session_init_session(int32 a1, char a2);