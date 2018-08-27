#ifndef H2MOD_NETWORK
#define H2MOD_NETWORK
#include <queue>
#include <h2mod.pb.h>

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
};
enum packet_type
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
	test
};

class CustomNetwork {
public:
	void applyNetworkHooks();
	void sendCustomPacketToAllPlayers();
	void sendCustomPacket(int peerIndex);
	int getPacketOffset();
	std::string currentMapFilename;

	//command sent from server
	char* networkCommand;

	inline void send_h2mod_packet(const H2ModPacket &packet)
	{
		char* SendBuf = new char[packet.ByteSize()];
		packet.SerializeToArray(SendBuf, packet.ByteSize());
		this->networkCommand = SendBuf;

		sendCustomPacketToAllPlayers();

		this->networkCommand = nullptr;
		delete[] SendBuf;
	}

	//command queue on the client
	std::deque<std::string> queuedNetworkCommands;
};

#endif 