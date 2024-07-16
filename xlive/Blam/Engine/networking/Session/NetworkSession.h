#pragma once

#include "game/players.h"
#include "input/controllers.h"
#include "networking/Transport/transport.h"
#include "networking/Transport/network_observer.h"
#include "saved_games/game_variant.h"

// ### TODO Cleanup

enum e_map_status : uint32
{
	_network_session_map_status_none,
	_network_session_map_status_unable_to_precache,
	_network_session_map_status_precaching,
	_network_session_map_status_precached,
	_network_session_map_status_loaded,
	_network_session_map_status_downloading,

	k_network_session_map_status_count
}; 

enum e_network_session_state : int
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
struct s_network_session;
struct s_session_membership;
struct s_membership_player;
struct s_membership_peer;
struct s_session_observer_channel;

#define k_network_maximum_machines_per_session (16 + 1)
#define k_number_of_users 4

namespace NetworkSession
{
	// network session getters and misc
	s_network_session* GetNetworkSessions();
	s_network_session* GetActiveNetworkSession();
	bool GetActiveNetworkSession(s_network_session** outSession);
	e_network_session_state GetLocalSessionState();
	int32 GetPeerIndexFromNetworkAddress(network_address* address);
	bool GetMapFileLocation(wchar_t* buffer, size_t size);

	bool LocalPeerIsSessionHost();
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

	// player functions
	
	/* Use this to verify if a player is currently active in the network session */
	/* Otherwise you will wonder why you don't get the right data/player index etc. */
	bool PlayerIsActive(datum player_index);
	std::vector<uint64> GetActivePlayerIdList();
	std::vector<int32> GetActivePlayerIndicesList();

	int32 GetPlayerCount();
	const wchar_t* GetPlayerName(datum player_index);
	unsigned long long GetPlayerId(datum player_index);

	int GetPlayerTeam(datum player_index);
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
	int observer_index;
	int membership_update_number;
	int parameters_update_number;
	int virtual_couch_update_number;
	int vote_update_number;
	int field_18;
};
ASSERT_STRUCT_SIZE(s_session_observer_channel, 28);

struct s_session_virtual_couch
{
	DWORD incremental_update_number;
	bool exists;
	XSESSION_INFO xsession_info;
	char pad[3];
	DWORD xuid_count;
	unsigned long long player_ids[k_maximum_players];
};
ASSERT_STRUCT_SIZE(s_session_virtual_couch, 200);

struct s_session_parameters
{
	DWORD parameters_update_number;
	BYTE gap_4C64[4];
	int session_mode;
	DWORD gap_4C6C;
	DWORD system_language_id;
	bool dedicated_server;
	char pad_1[3];
	DWORD party_privacy;
	int max_peers;
	int max_party_players;
	int gap_4C84;
	int field_4C88;
	int field_4C8C;
	DWORD field_4C90;
	DWORD field_4C94;
	BYTE gap_4C98[4];
	bool current_peer_is_host;
	char pad[3];
	DWORD field_4CA0;
	DWORD field_4CA4;
	BYTE gap_4CA8[16];
	DWORD field_4CB8;
	wchar_t scenario_path[128];
	BYTE gap4C00[12];
	BYTE random_bytes[8];
	DWORD dead_beef_lol;
	DWORD field_4DD4;
	DWORD field_4DD8;
	wchar_t custom_map_name[32];
	BYTE custom_map_sha256_hash[32];
	s_game_variant game_variant;
	BYTE gap_4E40[404];
	DWORD field_5100;
	BYTE gap_5104[3508];
	DWORD field_5EB8;
	BYTE gap_5EBC[76];
	DWORD field_5F08;
	DWORD field_5F0C;
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
	BYTE gap_24[4];
	wchar_t name[16];
	wchar_t peer_session_name[32];
	e_map_status map_status;
	unsigned int map_progress_percentage;
	char field_70;
	BYTE gap_71[3];
	char field_74;
	BYTE gap_75[3];
	unsigned int nat_type;
	unsigned int connectivity_mask;
	unsigned int latency_min;
	unsigned int latency_est;
	unsigned int latency_max;
	BYTE gap_9C[68];
	DWORD field_F0;
	DWORD field_F4;
	DWORD update_number;
	datum local_players_indexes[k_number_of_users];
};
ASSERT_STRUCT_SIZE(s_membership_peer, 268);

struct s_membership_player
{
	unsigned long long identifier; // -0xA
	int peer_index; // -0x8
	int peer_user_index; // -0x4
	WORD player_flags; // 0x0
	bool properties_valid;
	char pad[1];
	unsigned int controller_index;
	s_player_properties properties[2];
	unsigned int player_voice;
	unsigned int player_text_chat;
};
ASSERT_STRUCT_OFFSET(s_membership_player, controller_index, 20);
ASSERT_STRUCT_SIZE(s_membership_player, 296);

struct s_session_membership
{
	DWORD update_number; // 0x70
	int session_leader_peer_index; // 0x74
	unsigned long long dedicated_server_xuid; // 0x78
	int xbox_session_leader_peer_index; // 0x80
	int peer_count; // 0x84
	s_membership_peer peers[k_network_maximum_machines_per_session]; // 0x88
	int player_count; // 0x1254
	uint32 players_active_mask; // 0x1258
	s_membership_player players[k_maximum_players]; // 0x125C
	DWORD unk;
};
ASSERT_STRUCT_SIZE(s_session_membership, 9328);


struct s_session_interface_user
{
	bool user_exists;
	s_player_identifier network_user_identifier;
	BYTE gap_B[3];
	e_controller_index controller_index;
	s_player_properties properties;
	DWORD player_voice_exists;
	DWORD player_text_chat_exists;
	DWORD field_9C[2];
	BYTE gap_A4[16];
	char field_b4[4];
};
ASSERT_STRUCT_SIZE(s_session_interface_user, 0xB8);

struct s_session_interface_globals
{
	char initialised;
	BYTE gap_1;
	wchar_t m_machine_name[16];
	wchar_t m_session_name[32];
	BYTE m_qos_active;
	BYTE gap_63;
	BYTE gap_64[16];
	DWORD m_upstream_bandwidth_bps;
	DWORD m_downstream_bandwidth_bps;
	BYTE gap_7C[8];
	DWORD m_nat_type;
	DWORD field_88;
	DWORD field_8C;
	DWORD field_90;
	int current_map_progress_percentage;
	s_session_interface_user users[4];
	DWORD session_connection_identifiers[6];
	s_game_variant variants[2];
	BYTE gap_5F0[68];
	DWORD m_sessions_manager;
	
	static s_session_interface_globals* get();
};
ASSERT_STRUCT_SIZE(s_session_interface_globals, 0x638);

struct s_network_session
{
	void* vtbl;
	void* p_network_message_gateway;
	c_network_observer* p_network_observer;
	void* session_manager_ptr;
	DWORD text_chat;
	int session_index;
	int field_18;
	int network_protocol; // LIVE - 2, Network - 1
	XNKID session_id;
	wchar_t field_28[16];
	char field_48;
	XNKEY xnkey;
	char pad[3];
	int xnkey_index;
	int field_60;
	int session_host_peer_index;
	int elected_host_peer_index;
	DWORD field_6C;
	s_session_membership membership[2]; // 0x70
	s_session_virtual_couch virtual_couch[2];
	s_session_vote voting_data[2]; // unused
	BYTE gap_4B84[64];
	s_session_parameters parameters[2];
	int local_peer_index;
	s_session_observer_channel observer_channels[k_network_maximum_machines_per_session];
	e_network_session_state local_session_state;
	DWORD time_unk_2;
	DWORD time_unk_3;
	DWORD time_unk;
	DWORD possible_new_peer_host_index;
	DWORD field_73B4;
	DWORD field_73B8;
	int field_73BC;
	char field_73C0[4];
	DWORD time_new_session_host_found;
	int field_73C8;
	DWORD field_73CC;
	int field_73D0;
	int field_73D4;
	int field_73D8;
	int field_73DC;
	BYTE field_73E0[32];
	int field_7400;
	int field_7404;
	BYTE gap_7408[4];
	int field_740C;
	int field_7410;
	BYTE gap_7414[100];
	DWORD field_7478;
	DWORD field_747C;
	DWORD field_7480;
	DWORD field_7484;
	BYTE gap_7488[1132];
	int field_78F4;
	int field_78F8;
	BYTE field_78FC[36];
	int field_7920;
	DWORD field_7924;
	DWORD total_join_request_attempts;
	DWORD last_time_join_request_sent;
	DWORD field_7930;
	DWORD field_7934;
	DWORD field_7938;
	int field_793C;
	int field_7940;
	int field_7944;
	int field_7948;
	int field_794C;
	int field_7950;
	int field_7954;
	int field_7958;
	int field_795C;
	DWORD field_7960;
	DWORD field_7964;
	DWORD field_7968;
	char field_796C[4];
	int field_7970;
	DWORD field_7974;
	DWORD field_7978;
	BYTE gap_797C[508];
	void* c_kablam_session_join_request_handler; // dedicated server session join handler
	char field_7B7C[12];

	const char* describe_network_protocol_type() const
	{
		static const char* network_protocols_str[] =
		{
			"<disconnected>",
			"System-Link",
			"LIVE",
			"<unknown>",
		};

		return this->network_protocol > 2 ? network_protocols_str[3] : network_protocols_str[this->network_protocol];
	}
};
ASSERT_STRUCT_SIZE(s_network_session, 31624);
ASSERT_STRUCT_OFFSET(s_network_session, membership[0], 0x70);
ASSERT_STRUCT_OFFSET(s_network_session, parameters[0], 0x4C60);
#pragma pack(pop)

s_session_interface_user* session_interface_get_local_user_properties(int user_index);

bool network_session_interface_set_local_user_character_type(int user_index, e_character_type character_type);
bool network_session_interface_get_local_user_identifier(int user_index, s_player_identifier* out_identifier);
void network_session_interface_set_local_user_rank(int32 user_index, int8 rank);
bool __cdecl network_session_interface_get_local_user_properties(int32 user_index, int32* out_controller_index, s_player_properties* out_properties, int32* out_player_voice, int32* out_player_text_chat);
void __cdecl network_session_init_session(int unk, char unk_2);