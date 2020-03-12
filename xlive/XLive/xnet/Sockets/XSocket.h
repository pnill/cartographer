#pragma once

#define IPPROTO_VDP 254

struct XSocket
{
	int identifier = 'XSOC';
	int protocol;
	bool isVoiceSocket;
	SOCKET winSockHandle;
	sockaddr_in name;

	bool isTCP() { return protocol == IPPROTO_TCP; }
	bool isUDP() { return protocol == IPPROTO_UDP; }
	bool isVDP() { return protocol == IPPROTO_UDP; } /* VDP uses UDP*/

	bool isValid() { return identifier == 'XSOC'; }

	short getHostSocketPort() { return name.sin_port; }
	short getNetworkSocketPort() { return htons(name.sin_port); }
};