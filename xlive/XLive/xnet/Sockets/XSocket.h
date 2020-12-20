#pragma once

#define IPPROTO_VDP 254

#define SOCK_K_UNIT 1024

#define SOCK_UDP_MIN_SEND_BUFFER_K_UNITS 64 // this by default XLive is 16K (16,384 bytes)
#define SOCK_UDP_MIN_RECV_BUFFER_K_UNITS 64 // this by default XLive is 16K (16,384 bytes)

#define SOCK_UDP_MIN_SEND_BUFFER SOCK_UDP_MIN_SEND_BUFFER_K_UNITS * SOCK_K_UNIT
#define SOCK_UDP_MIN_RECV_BUFFER SOCK_UDP_MIN_RECV_BUFFER_K_UNITS * SOCK_K_UNIT

#define COMPILE_WITH_STD_SOCK_FUNC 1

struct XSocket
{
	int identifier;
	int protocol;
	bool isVoiceSocket;
	SOCKET winSockHandle;
	sockaddr_in name;

	XSocket::XSocket(int _protocol, bool _isVoiceSocket)
	{
		identifier = 'XSOC';
		protocol = _protocol;
		isVoiceSocket = _isVoiceSocket;
		winSockHandle = INVALID_SOCKET;
		memset(&name, 0, sizeof(name));
	}

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

	void setBufferSize(INT sendBufsize, INT recvBufsize);
};

void ForwardPorts();

SOCKET WINAPI XSocketCreate(int af, int type, int protocol);
int WINAPI XSocketSendTo(SOCKET s, const char* buf, int len, int flags, sockaddr *to, int tolen);