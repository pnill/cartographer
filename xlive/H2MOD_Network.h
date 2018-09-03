#ifndef H2MOD_NETWORK
#define H2MOD_NETWORK
#include <queue>
#include <h2mod.pb.h>
#include "Util/hash.h"

#define run_async(expression) \
	std::thread{ [=] { expression; } }.detach();

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
	size_t command_size;
	unsigned char* networkCommand;

	std::mutex network_send;

	inline void send_h2mod_packet(const H2ModPacket &packet)
	{
		std::unique_lock<std::mutex> network_lock(network_send);
		encode_packet(packet);

		sendCustomPacketToAllPlayers();

		delete[] networkCommand;
		networkCommand = nullptr;
	}

	inline void send_h2mod_packet_player(int peerIndex, const H2ModPacket &packet)
	{
		std::unique_lock<std::mutex> network_lock(network_send);
		encode_packet(packet);

		sendCustomPacket(peerIndex);

		delete[] networkCommand;
		networkCommand = nullptr;
	}

	//command queue on the client
	std::deque<std::string> queuedNetworkCommands;
private:
	inline void encode_packet(const H2ModPacket &packet)
	{
		size_t message_len = packet.ByteSize();
		command_size = message_len;
		unsigned char* SendBuf = new unsigned char[message_len];
		packet.SerializeToArray(SendBuf, message_len);
		this->networkCommand = SendBuf;

#if _DEBUG
		TRACE_FUNC_N("data: %s", hashes::as_hex_string((BYTE*)SendBuf, packet.ByteSize()).c_str());
#endif
	}
};

#endif 