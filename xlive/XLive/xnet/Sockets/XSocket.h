#pragma once

#define IPPROTO_VDP 254

struct XSocket
{
	int protocol;
	bool isVoiceSocket;
	SOCKET winSockHandle;
	sockaddr_in socketAddress;

	bool isTCP() { return protocol == IPPROTO_TCP; }
	bool isUDP() { return protocol == IPPROTO_UDP; }
	bool isVDP() { return protocol == IPPROTO_UDP; } /* VDP uses UDP*/
	short getHostSocketPort() { return socketAddress.sin_port; }
	short getNetworkSocketPort() { return htons(socketAddress.sin_port); }
};