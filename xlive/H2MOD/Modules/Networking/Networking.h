#pragma once
#include <queue>
#include "Util/hash.h"
#include <H2MOD\protobuf\h2mod.pb.h>
#include "CustomPackets\CustomPackets.h"

class CustomNetwork {
public:
	void applyNetworkHooks();
	std::string currentMapFilename;

	//command sent from server
	size_t command_size;
	unsigned char* networkCommand;

	std::mutex network_send;

	inline void send_h2mod_packet(const H2ModPacket &packet)
	{
		std::unique_lock<std::mutex> network_lock(network_send);
		encode_packet(packet);

		delete[] networkCommand;
		networkCommand = nullptr;
	}

	inline void send_h2mod_packet_player(int peerIndex, const H2ModPacket &packet)
	{
		std::unique_lock<std::mutex> network_lock(network_send);
		encode_packet(packet);

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

extern CustomNetwork *network;