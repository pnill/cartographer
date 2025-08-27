#pragma once
#include "game/game_allegiance.h"
#include "game/player_constants.h"
#include "input/controllers.h"
#include "main/map_repository.h"
#include "networking/network_game_definitions.h"
#include "networking/transport/transport.h"
#include "networking/transport/transport_security.h"
#include "saved_games/game_variant.h"

/* enums */

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

enum e_network_session_class
{
	_network_session_class_unknown = NONE,
	_network_session_class_offline = 0x0,
	_network_session_class_system_link = 0x1,
	_network_session_class_xbox_live = 0x2,

	k_network_session_class_count = 0x3,
};

enum e_network_session_mode
{
	_network_session_mode_none = 0,
	_network_session_mode_idle = 1,
	_network_session_mode_in_game = 4,
	_network_session_mode_migration_start = 6,
	_network_session_mode_migration_joining = 7,
	_network_session_mode_migration_disbanding = 9,

	k_network_session_mode_count = 10
};

enum e_network_game_privacy : int32
{
	_network_game_privacy_open,
	_network_game_privacy_gold,
	_network_game_privacy_invite,
	_network_game_privacy_closed,
	k_network_game_privacy_count
};

enum e_network_session_type : uint32
{
	_network_session_type_none = 0,
	_network_session_type_one = 1,
	k_network_session_type_count,
};

/* constants */

enum
{
	k_network_maximum_sessions = 2,
	k_network_maximum_machines_per_session = k_maximum_players + 1
};

extern const char* const k_network_protocols_text[];

// THE FOLLOWING FUNCTIONS ARE NOW DEPRECATED, AND WILL BE REMOVED EVENTUALLY
// WHEN PORTING IS FINISHED
// USE/IMPLEMENT THE HELPER FUNCTIONS IN THE c_network_session CLASS !!!!!!
namespace NetworkSession
{
	bool GetMapFileLocation(wchar_t* buffer, size_t size);

	bool LocalPeerIsSessionHost();
	bool LocalPeerIsSessionLeader();
	bool LocalPeerIsEstablished();

	// peer functions
	int32 GetPeerCount();
	int32 GetLocalPeerIndex();
	IN_ADDR GetLocalNetworkAddress();
	void KickPeer(int32 peer_index);
	void EndGame();

	// peer-player functions
	int32 GetPeerIndex(datum player_index);

	// player functions
	
	/* Use this to verify if a player is currently active in the network session */
	/* Otherwise you will wonder why you don't get the right data/player index etc. */
	bool PlayerIsActive(datum player_index);
	std::vector<int32> GetActivePlayerIndicesList();

	int32 GetPlayerCount();
	const wchar_t* GetPlayerName(datum player_index);
	s_player_identifier GetPlayerId(datum player_index);

	int8 GetPlayerTeam(datum player_index);

	wchar_t* GetGameVariantName();

	bool IsVariantTeamPlay();
	void LeaveSession();
}

/* structures */

// CARTOGRAPHER ADDITION

struct s_cartographer_peer_data
{
	uint32 cartographer_session_parameters_update_number;
};

struct s_cartographer_network_session_parameters
{
	uint32 incremental_update_number;
};

class c_network_session_cartographer
{
	s_cartographer_peer_data peers[k_network_maximum_machines_per_session];

	s_cartographer_network_session_parameters parameters;
	s_cartographer_network_session_parameters transmitted_parameters;

public:
};

// END CARTOGRAPHER ADDITION

#pragma pack(push, 1)
struct s_session_peer
{
	bool peer_valid;
	bool is_remote_peer;
	bool mode_transition_pending;
	bool peer_needs_reestablishment;
	int32 observer_channel_index;
	int32 membership_update_number;
	int32 parameters_update_number;
	int32 virtual_couch_update_number;
	int32 vote_update_number;
	int32 field_18;
};
ASSERT_STRUCT_SIZE(s_session_peer, 28);

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
	e_network_game_privacy party_privacy;
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
	s_transport_secure_address secure_address;
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
	int32 field_AC;
	wchar_t description[32];
	uint32 field_F0;
	uint32 field_F4;
	uint32 update_number;
	datum local_players_indexes[k_number_of_users];
};
ASSERT_STRUCT_SIZE(s_membership_peer, 268);

struct s_membership_player
{
	s_player_identifier identifier; // -0xA
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
	s_membership_peer membership_peers[k_network_maximum_machines_per_session]; // 0x88
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
	wchar_t machine_name[16];
	wchar_t session_name[32];
	uint8 qos_active;
	uint8 gap_63;
	uint8 gap_64[16];
	uint32 upstream_bandwidth_bps;
	uint32 downstream_bandwidth_bps;
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

/* classes */

class c_network_session
{
public:
	void* vtbl;
	void* m_network_message_gateway;
	class c_network_observer* m_network_observer;
	void* m_session_manager;
	uint32 field_10;
	int32 m_session_index;
	int32 m_session_type;
	e_network_session_class m_session_class;
	s_transport_secure_identifier m_session_id;
	wchar_t field_28[16];
	bool m_field_48;
	XNKEY m_session_key;
	char pad[3];
	int32 m_session_transport_index;
	e_transport_platform m_session_transport_platform;
	int32 m_session_host_peer_index;
	int32 m_elected_host_peer_index;
	uint32 field_6C;

	s_session_membership m_session_membership;
	s_session_membership m_session_transmitted_membership;
	s_session_virtual_couch m_session_virtual_couch;
	s_session_virtual_couch m_session_transmitted_virtual_couch;

	/* unused and unfinished leftover voting system */
	s_session_vote m_session_voting_data; 
	s_session_vote m_session_transmitted_voting_data; 
	
	uint8 gap_4B84[64];
	s_session_parameters m_session_parameters;
	s_session_parameters m_session_transmitted_parameters;

	int32 m_local_peer_index;
	s_session_peer m_session_peers[k_network_maximum_machines_per_session];
	e_network_session_state m_local_state;
	uint32 field_73A4;
	uint32 field_73A8;
	uint32 field_73AC;
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
	bool m_time_exists;
	int8 pad_7B7D[3];
	uint32 m_time;
	char field_7B7D[4];

	bool initialize_session(
		int32 session_index,
		e_network_session_type session_type,
		int32 session_transport_index,
		class c_network_message_gateway* message_gateway,
		class c_network_observer* observer,
		class c_network_session_manager* session_manager,
		class c_network_text_chat_manager* text_chat_manager);

	bool channel_is_authoritative(int32 network_channel_index) const;

	e_network_session_state current_local_state() const
	{
		ASSERT(VALID_INDEX(m_local_state, k_network_session_state_count));
		return m_local_state;
	}

	s_membership_peer* get_peer_membership(int32 peer_index)
	{
		return &m_session_membership.membership_peers[peer_index];
	}

	s_membership_player* get_player_membership(datum player_index)
	{
		return &m_session_membership.players[DATUM_INDEX_TO_ABSOLUTE_INDEX(player_index)];
	}

	s_session_peer* get_session_peer(int32 peer_index)
	{
		return &m_session_peers[peer_index];
	}

	int32 get_peer_count() const
	{
		return m_session_membership.peer_count;
	}

	int32 get_player_count() const
	{
		return m_session_membership.player_count;
	}

	s_player_identifier get_player_id(datum player_index)
	{
		return get_player_membership(player_index)->identifier;
	}

	const wchar_t* get_player_name(datum player_index)
	{
		return get_player_membership(player_index)->properties[0].player_name;
	}

	int32 get_local_peer_index() const
	{
		return m_local_peer_index;
	}

	const wchar_t* get_game_variant_name() const
	{
		return m_session_parameters.game_variant.variant_name;
	}

	int32 get_session_host_peer_index() const
	{
		return m_session_host_peer_index;
	}

	bool peer_index_local_peer(int32 peer_index) const
	{
		return get_local_peer_index() == peer_index;
	}

	bool disconnected(void) const
	{
		return current_local_state() == _network_session_state_none;
	}

	bool leaving() const
	{
		bool result = false;

		switch (current_local_state())
		{
		case _network_session_state_peer_join_abort:
		case _network_session_state_peer_leaving:
		case _network_session_state_host_disband:
			result = true;
			break;
		case _network_session_state_host_handoff:
		case _network_session_state_host_reestablish:
			result = field_73A4;
			break;
		default:
			break;
		}

		return result;
	}

	bool is_host(void) const
	{
		bool result = false;
		const e_network_session_state state = current_local_state();

		if (state == _network_session_state_host_established ||
			state == _network_session_state_host_disband ||
			state == _network_session_state_host_handoff ||
			state == _network_session_state_host_reestablish)
		{
			result = true;
			ASSERT(m_local_peer_index == m_session_host_peer_index);
		}
		else if (established())
		{
			ASSERT(m_local_peer_index != m_session_host_peer_index);
		}

		return result;
	}

	bool is_peer_local(int32 peer_index) const
	{
		return get_local_peer_index() == peer_index;
	}

	bool is_peer_session_host(int32 peer_index) const
	{
		return get_session_host_peer_index() == peer_index;
	}

	bool peer_joining() const
	{
		return current_local_state() == _network_session_state_peer_joining;
	}

	bool established() const
	{
		bool result = false;

		switch (current_local_state())
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
		
		default:
			unreachable();
			break;
		}

		return result;
	}

	bool is_local_peer_session_leader() const
	{
		return m_session_membership.session_leader_peer_index == m_local_peer_index;
	}

	bool is_session_player_active(datum player_index) const
	{
		return TEST_BIT(m_session_membership.players_active_mask, DATUM_INDEX_TO_ABSOLUTE_INDEX(player_index));
	}

	int32 get_peer_index_from_address(const struct transport_address* address)
	{
		return INVOKE_BY_TYPE(0x1C71DF, 0x19E9CF, int32(__thiscall*)(c_network_session*, const struct transport_address*), this, address);
	}

	void request_membership_update()
	{
		this->m_session_membership.update_number++;
		this->local_membership_update_number++;
	}

	int32 session_mode() const
	{
		return m_session_parameters.session_mode;
	}

	void switch_player_team(datum player_index, e_game_team team_index)
	{
		if (is_host())
		{
			get_player_membership(player_index)->properties[0].team_index = (int8)team_index;
			request_membership_update();
		}
	}

	// switch multiple players with a single membership update
	void switch_players_to_teams(datum* player_indexes, int32 player_count, e_game_team* team_indexes);

	int32 get_peer_index_by_observer_index(int32 observer_index) const
	{
		int32 peer_index = NONE;

		for (int32 i = 0; i < k_network_maximum_machines_per_session; i++)
		{
			const s_session_peer* peer = &m_session_peers[i];
			if (peer->peer_valid && peer->observer_channel_index == observer_index)
			{
				peer_index = i;
				break;
			}
		}

		return peer_index;
	}

	bool is_session_class_online() const
	{
		return m_session_class == _network_session_class_xbox_live;
	}

	bool get_secure_key(s_transport_secure_identifier* out_session_id, XNKEY* out_session_key, int32* out_session_key_index, e_transport_platform* transport_platform) const;
	bool get_transport_session_id(s_transport_secure_identifier* out_session_id) const;
	uint32 time_get(void) const;
	
	const char* describe_network_protocol_type() const
	{
		return VALID_INDEX(this->m_session_class, k_network_session_class_count) ? k_network_protocols_text[this->m_session_class] : "<unknown>";
	}
};
ASSERT_STRUCT_SIZE(c_network_session, 31624);
ASSERT_STRUCT_OFFSET(c_network_session, m_session_membership, 0x70);
ASSERT_STRUCT_OFFSET(c_network_session, m_session_parameters, 0x4C60);
#pragma pack(pop)

/* prototypes */

s_session_interface_user* session_interface_get_local_user_properties(int32 user_index);

void network_session_apply_patches();

void network_session_membership_update_local_players_teams();

e_network_session_class network_squad_session_get_session_class();
bool network_session_interface_set_local_user_character_type(int32 user_index, e_character_type character_type);
bool network_session_interface_get_local_user_identifier(int32 user_index, s_player_identifier* out_identifier);
void network_session_interface_set_local_user_rank(int32 user_index, int8 rank);
bool __cdecl network_session_get_scenario_id(uint32* out_1, uint32* out_2, uint32* out_3, s_custom_map_id** out_custom_map);
bool __cdecl network_session_interface_get_local_user_properties(int32 user_index, int32* out_controller_index, s_player_properties* out_properties, int32* out_player_voice, int32* out_player_text_chat);
void __cdecl network_globals_switch_environment(int32 a1, bool a2);
