#pragma once

#include "H2MOD/Modules/Networking/NetworkSession/NetworkSession.h"

#define player_identifier_size_bits CHAR_BIT * sizeof(XUID)

enum e_network_message_types : int
{
	ping,
	pong,
	broadcast_search,
	broadcast_reply,
	connect_request,
	connect_refuse,
	connect_establish,
	connect_closed,
	join_request,
	join_abort,
	join_refuse,
	leave_session,
	leave_acknowledge,
	session_disband,
	session_boot,
	host_handoff,
	peer_handoff,
	host_transition,
	host_reestablish,
	host_decline,
	peer_reestablish,
	peer_establish,
	election,
	election_refuse,
	time_synchronize,
	membership_update,
	peer_properties,
	delegate_leadership,
	boot_machine,
	player_add,
	player_refuse,
	player_remove,
	player_properties,
	parameters_update,
	parameters_request,
	countdown_timer,
	mode_acknowledge,
	virtual_couch_update,
	virtual_couch_request,
	vote_update,
	view_establishment,
	player_acknowledge,
	synchronous_update,
	synchronous_actions,
	synchronous_join,
	synchronous_gamestate,
	game_results,
	text_chat,
	test,

	//custom packets bellow
	request_map_filename,
	custom_map_filename,
	team_change,
	rank_change,
	anti_cheat,

	end
};

static const char* network_message_name[] = {
	"ping",
	"pong",
	"broadcast_search",
	"broadcast_reply",
	"connect_request",
	"connect_refuse",
	"connect_establish",
	"connect_closed",
	"join_request",
	"join_abort",
	"join_refuse",
	"leave_session",
	"leave_acknowledge",
	"session_disband",
	"session_boot",
	"host_handoff",
	"peer_handoff",
	"host_transition",
	"host_reestablish",
	"host_decline",
	"peer_reestablish",
	"peer_establish",
	"election",
	"election_refuse",
	"time_synchronize",
	"membership_update",
	"peer_properties",
	"delegate_leadership",
	"boot_machine",
	"player_add",
	"player_refuse",
	"player_remove",
	"player_properties",
	"parameters_update",
	"parameters_request",
	"countdown_timer",
	"mode_acknowledge",
	"virtual_couch_update",
	"virtual_couch_request",
	"vote_update",
	"view_establishment",
	"player_acknowledge",
	"synchronous_update",
	"synchronous_actions",
	"synchronous_join",
	"synchronous_gamestate",
	"game_results",
	"text_chat",
	"test"

	//custom packets bellow
	"request_map_filename"
	"map_file_name"
	"team_change"
	"rank_change"
	"anti_cheat"
};

struct __declspec(align(8)) s_custom_map_filename
{
	wchar_t file_name[32];
	int map_download_id;
};

struct s_request_map_filename
{
	XUID user_identifier;
	int map_download_id;
};

struct s_team_change
{
	DWORD team_index;
};

struct s_rank_change
{
	BYTE rank;
};

struct s_anti_cheat
{
	bool enabled;
};

#pragma pack(push, 1)
struct s_text_chat
{
	XNKID session_id;
	unsigned int routed_players_mask;
	unsigned int metadata;
	bool source_is_server;
	XUID source_player_id;
	XUID destination_players_ids[16];
	BYTE gap_99[3];
	int destination_player_count;
	wchar_t text[122];
};
#pragma pack(pop)

const char* getNetworkMessageName(int enumVal);
void register_packet_impl(void *, int, char*, int, int, int, void*, void*, void*);

namespace CustomPackets
{
	void ApplyGamePatches();
	void sendRequestMapFilename(int mapDownloadId);
	void sendTeamChange(int peerIndex, int teamIndex);
	void sendRankChange(int peerIndex, byte rank);
	void sendAntiCheat(int peerIndex);
}