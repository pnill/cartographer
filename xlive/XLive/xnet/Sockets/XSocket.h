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

	bool isValid() { return identifier == 'XSoC'; }

	short getHostSocketPort() { return name.sin_port; }
	short getNetworkSocketPort() { return htons(name.sin_port); }

	bool sockAddrInEqual(sockaddr_in* a1, sockaddr_in* a2)
	{
		return (a1->sin_addr.s_addr == a2->sin_addr.s_addr && a1->sin_port == a2->sin_port);
	}

	bool sockAddrInIsNull(sockaddr_in* a1)
	{
		return a1->sin_addr.s_addr == 0 || a1->sin_port == 0;
	}
};

void ForwardPorts();