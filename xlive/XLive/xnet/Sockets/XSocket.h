#pragma once

#define IPPROTO_VDP 254

#define SOCK_K_UNIT 1024

#define SOCK_UDP_MIN_SEND_BUFFER_K_UNITS 64 // this by default XLive is 16K
#define SOCK_UDP_MIN_RECV_BUFFER_K_UNITS 64 // this by default XLive is 16K

#define SOCK_UDP_MIN_SEND_BUFFER (SOCK_UDP_MIN_SEND_BUFFER_K_UNITS * SOCK_K_UNIT)
#define SOCK_UDP_MIN_RECV_BUFFER (SOCK_UDP_MIN_RECV_BUFFER_K_UNITS * SOCK_K_UNIT)

#define COMPILE_WITH_STD_SOCK_FUNC 1

#define MAX_PACKETS_TO_READ_PER_RECV_CALL 20

// the only needed sockets to be connected
const static std::unordered_set<int> H2v_socketsToConnect =
{
	1000,
	1001
};

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

	bool isTCP() const { return protocol == IPPROTO_TCP; }
	bool isUDP() const { return protocol == IPPROTO_UDP; }

	/* VDP uses UDP, and some encryption (done at network transport layer, not game layer) */
	bool isVDP() const { return protocol == IPPROTO_UDP; }

	bool isValid() const { return identifier == 'XSoC'; }

	// all fields in sockaddr_in are in network byte order
	// some helpers for conversion for each case needed
	/* get the port, in host byte order, in this case little-endian */
	short getHostOrderSocketPort() const { return ntohs(name.sin_port); }

	/* get the port, in network byte order, in this case big-endian */
	short getNetworkOrderSocketPort() const { return name.sin_port; }

	/* sets the socket send buffer size */
	void setBufferSize(INT sendBufsize, INT recvBufsize);

	bool sockAddrInEqual(sockaddr_in* a1, sockaddr_in* a2) const
	{
		return (a1->sin_addr.s_addr == a2->sin_addr.s_addr && a1->sin_port == a2->sin_port);
	}

	bool sockAddrInInvalid(sockaddr_in* a1) const
	{
		return a1->sin_addr.s_addr == 0 || a1->sin_port == 0;
	}

	int udpSend(const char* buf, int len, int flags, sockaddr *to, int tolen);

	static void socketsDisposeAll();

	static std::vector<XSocket*> Sockets;
	int recvfrom(LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesRecvd, LPDWORD lpFlags, struct sockaddr *lpFrom, LPINT lpFromlen, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);
};

void WINAPI XSocketWSASetLastError(int iError);
SOCKET WINAPI XSocketCreate(int af, int type, int protocol);
int WINAPI XSocketSendTo(SOCKET s, const char* buf, int len, int flags, sockaddr *to, int tolen);