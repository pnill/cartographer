#pragma once

#include "..\NetworkSession/NetworkSession.h"

enum e_network_message_types
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
	map_file_name,
	team_change,

	end
};

static const char* packet_type_strings[] = {
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
};

struct s_custom_map_filename
{
	bool is_custom_map;
	wchar_t file_name[32];
};

struct s_request_map_filename
{
	XUID user_identifier;
};

struct s_team_change
{
	DWORD team_index;
};

void register_packet_impl(void *, int, char*, int, int, int, void*, void*, void*);
namespace CustomPackets
{
	void ApplyGamePatches();
	void sendRequestMapFilename(network_session* session);
	void sendTeamChange(network_session* session, signed int peer_index, int team_index);
}