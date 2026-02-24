#pragma once

#define IPPROTO_VDP 254

#define SOCK_K_UNIT 1024

#define SOCK_UDP_MIN_SEND_BUFFER_K_UNITS 64 // this by default XLive is 16K
#define SOCK_UDP_MIN_RECV_BUFFER_K_UNITS 64 // this by default XLive is 16K

#define SOCK_UDP_MIN_SEND_BUFFER (SOCK_UDP_MIN_SEND_BUFFER_K_UNITS * SOCK_K_UNIT)
#define SOCK_UDP_MIN_RECV_BUFFER (SOCK_UDP_MIN_RECV_BUFFER_K_UNITS * SOCK_K_UNIT)

#define XSOCK_USING_STANDARD_APIS 1

#define SOCK_MAX_RECV_PAYLOADS_TO_READ_PER_CALL 20

enum
{
	EXSOCK_IDENTIFIER = 'XSoC'
};

// 239.255.0.1
#define XSOCK_MUTICAST_ADDR  0xefff0001
#define XSOCK_MULTICAST_PORT 56011

enum e_system_link_socket_type
{
	_system_link_socket_type_lan,
	_system_link_socket_type_localhost,
	k_system_link_socket_type_count
};

class XSocketManager
{
	bool m_initialized = false;

	class XInternalSocket
	{
		friend class XSocketManager;

		WORD m_port = 0;
		SOCKET m_systemSockHandle = INVALID_SOCKET;
		bool m_multicast;

		void Dispose();

		~XInternalSocket()
		{
			this->Dispose();
		}
	};

	XInternalSocket m_mainUdpSocket;
	XInternalSocket m_systemLinkSockets[k_system_link_socket_type_count];

public:
	void Initialize();
	void Dispose();

	bool SystemLinkSocketInitialize(WORD port);
	bool SystemLinkSocketReset(WORD port);
	void SystemLinkDispose();

	bool MainLinkSocketInitialize(WORD port);
	bool MainLinkSocketReset(WORD port);
	void MainLinkDispose();

	SOCKET GetMainUdpSocketSystemHandle() const;

	bool CreateSocketUDP(XInternalSocket* sock, unsigned long interfaceAddress, WORD port, bool multicast, bool allow_address_reuse);

	WORD SystemLinkGetPort() const
	{
		return m_systemLinkSockets[_system_link_socket_type_lan].m_port;
	}

	SOCKET SystemLinkGetLANSystemSockHandle() const
	{
		return m_systemLinkSockets[_system_link_socket_type_lan].m_systemSockHandle;
	}

	SOCKET SystemLinkGetSystemSockHandle(e_system_link_socket_type type) const
	{
		return m_systemLinkSockets[type].m_systemSockHandle;
	}

	bool SystemLinkAvailable() const
	{
		bool result = false;
		for (size_t i = 0; i < k_system_link_socket_type_count; ++i)
		{
			if (SystemLinkGetSystemSockHandle((e_system_link_socket_type)i) != INVALID_SOCKET)
			{
				result = true;
				break;
			}
		}
		return result;
	}

	void SocketsDisposeAll();
	std::vector<struct XVirtualSocket*> sockets;
};

struct XVirtualSocket
{
	SOCKET systemSocketHandle;
	int identifier;
	int protocol;
	sockaddr_in name;
	bool isBroadcast;
	bool isVoiceProtocol;

	XVirtualSocket(int _protocol, bool _isVoiceSocket)
	{
		identifier = EXSOCK_IDENTIFIER;
		protocol = _protocol;
		isVoiceProtocol = _isVoiceSocket;
		systemSocketHandle = INVALID_SOCKET;
		isBroadcast = false;
		memset(&name, 0, sizeof(name));
	}

	bool IsTCP() const { return protocol == IPPROTO_TCP; }
	bool IsUDP() const { return protocol == IPPROTO_UDP; }

	bool IsBroadcast() const { return isBroadcast; }

	/* VDP uses UDP, and some encryption (done at network transport layer, not game layer) */
	bool IsVDP() const { return protocol == IPPROTO_UDP; }

	bool IsValid() const { return identifier == EXSOCK_IDENTIFIER; }

	// all fields in sockaddr_in are in network byte order
	// some helpers for conversion for each case needed
	/* get the port, in host byte order, in this case little-endian */
	u_short GetHostOrderSocketVirtualPort() const { return ntohs(name.sin_port); }

	/* get the port, in network byte order, in this case big-endian */
	u_short GetNetworkOrderSocketVirtualPort() const { return name.sin_port; }

	/* sets the socket send/recv buffer size */
	int SetBufferSize(int optName, INT bufSize);

	static bool SockAddrInEqual(const sockaddr_in* a1, const sockaddr_in* a2);

	static bool SockAddrInInvalid(const sockaddr_in* a1);

	int UdpSend(const char* buf, int len, int flags, sockaddr *to, int tolen);

	// dwBufferCount > 1 is unsupported/unimplemented
	int sock_read(LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesRecvd, LPDWORD lpFlags, struct sockaddr* lpFrom, LPINT lpFromlen, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);

private:
	int read_socket(LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesRecvd, LPDWORD lpFlags, struct sockaddr* lpFrom, LPINT lpFromlen, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine, bool* winApiError);
};

void WINAPI XSocketWSASetLastError(int iError);

SOCKET WINAPI XSocketCreate(int af, int type, int protocol);

int WINAPI XSocketSendTo(SOCKET s, const char* buf, int len, int flags, sockaddr *to, int tolen);

u_long WINAPI XSocketNTOHL(u_long netlong);

u_short WINAPI XSocketHTONS(u_short hostshort);

extern XSocketManager g_XSockMgr;