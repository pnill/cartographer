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

	bool IsTCP() const { return protocol == IPPROTO_TCP; }
	bool IsUDP() const { return protocol == IPPROTO_UDP; }

	/* VDP uses UDP, and some encryption (done at network transport layer, not game layer) */
	bool IsVDP() const { return protocol == IPPROTO_UDP; }

	bool IsValid() const { return identifier == 'XSoC'; }

	// all fields in sockaddr_in are in network byte order
	// some helpers for conversion for each case needed
	/* get the port, in host byte order, in this case little-endian */
	short GetHostOrderSocketPort() const { return ntohs(name.sin_port); }

	/* get the port, in network byte order, in this case big-endian */
	short GetNetworkOrderSocketPort() const { return name.sin_port; }

	/* sets the socket send/recv buffer size */
	int SetBufferSize(int optName, INT bufSize);

	static bool SockAddrInEqual(const sockaddr_in* a1, const sockaddr_in* a2)
	{
		return (a1->sin_addr.s_addr == a2->sin_addr.s_addr && a1->sin_port == a2->sin_port);
	}

	static bool SockAddrInInvalid(const sockaddr_in* a1)
	{
		return a1->sin_addr.s_addr == 0 || a1->sin_port == 0;
	}

	int UdpSend(const char* buf, int len, int flags, sockaddr *to, int tolen);

	static void SocketsDisposeAll();

	static std::vector<XSocket*> Sockets;

	// dwBufferCount > 1 is unsupported/unimplemented
	int sock_read(LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesRecvd, LPDWORD lpFlags, struct sockaddr* lpFrom, LPINT lpFromlen, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);

private:
	int winsock_read_socket(LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesRecvd, LPDWORD lpFlags, struct sockaddr* lpFrom, LPINT lpFromlen, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine, bool* winApiError);
};

void WINAPI XSocketWSASetLastError(int iError);
SOCKET WINAPI XSocketCreate(int af, int type, int protocol);
int WINAPI XSocketSendTo(SOCKET s, const char* buf, int len, int flags, sockaddr *to, int tolen);