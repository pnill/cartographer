#include "stdafx.h"
#include "XSocket.h"
#include "XLive/xnet/upnp.h"
#include "XLive/xnet/Sockets/XSocket.h"
#include "XLive/xnet/IpManagement/XnIp.h"
#include "H2MOD/Modules/Shell/Config.h"

#include "XLive/xnet/net_utils.h"

#include "../IpManagement/XnIp.h"

#include <MSWSock.h>

std::vector<XSocket*> XSocket::Sockets;

// #5310: XOnlineStartup
int WINAPI XOnlineStartup()
{
	LOG_TRACE_NETWORK("XOnlineStartup()");
	return ERROR_SUCCESS;
}

// #1: XWSAStartup
int WINAPI XWSAStartup(WORD wVersionRequested, LPWSADATA lpWsaData)
{
	LOG_TRACE_NETWORK("XWSAStartup({:x}, {:p})", wVersionRequested, (void*)lpWsaData);
	int result = WSAStartup(wVersionRequested, lpWsaData);
	return result;
}

// #2: XWSACleanup
int WINAPI XWSACleanup()
{
	LOG_TRACE_NETWORK("XWSACleanup()");
	return WSACleanup();
}

// #3: XSocketCreate
SOCKET WINAPI XSocketCreate(int af, int type, int protocol)
{
	LOG_TRACE_NETWORK("XSocketCreate() - af = {0}, type = {1}, protocol = {2}", af, type, protocol);

	if (af != AF_INET)
		return SOCKET_ERROR;

	// TODO: add TCP support
	if (protocol != IPPROTO_UDP
		&& protocol != IPPROTO_VDP)
		return SOCKET_ERROR; 

	bool isVoice = false;
	if (protocol == IPPROTO_VDP)
	{
		isVoice = true;
		protocol = IPPROTO_UDP; // We can't support VDP (Voice / Data Protocol) it's some encrypted crap which isn't standard.
	}

	// TODO: use an array of sockets rather than allocating the memory on heap
	XSocket* newXSocket = new XSocket(protocol, isVoice);

#if COMPILE_WITH_STD_SOCK_FUNC
	SOCKET ret = socket(af, type, protocol);
#else
	SOCKET ret = WSASocket(af, type, protocol, 0, 0, 0);
#endif

	if (ret == INVALID_SOCKET)
	{
		LOG_ERROR_NETWORK("XSocketCreate() - Invalid socket, last error: {}", WSAGetLastError());
		delete newXSocket;
		return INVALID_SOCKET;
	}

	LOG_TRACE_NETWORK("XSocketCreate() - Socket created with descriptor: {}.", ret);
	newXSocket->winSockHandle = ret;

	if (newXSocket->isVoiceSocket)
	{
		LOG_TRACE_NETWORK("XSocketCreate() - Socket: {} was set to VDP", ret);
	}

	// disable SIO_UDP_CONNRESET
	// TODO re-enable this if the issue https://github.com/pnill/cartographer/issues/320 is not caused by this

	/*DWORD ioctlSetting = 0;
	DWORD cbBytesReturned;
	if (WSAIoctl(newXSocket->winSockHandle, SIO_UDP_CONNRESET, &ioctlSetting, 4u, 0, 0, &cbBytesReturned, 0, 0) == SOCKET_ERROR)
	{
		LOG_ERROR_NETWORK("XSocketCreate() - couldn't disable SIO_UDP_CONNRESET", ret);
	}
	else
	{
		LOG_TRACE_NETWORK("XSocketCreate() - disabled SIO_UDP_CONNRESET");
	}*/

	XSocket::Sockets.push_back(newXSocket);

	return (SOCKET)newXSocket;
}

// #5: XSocketShutdown
int WINAPI XSocketShutdown(SOCKET s, int how)
{
	LOG_TRACE_NETWORK("XSocketShutdown");
	XSocket* xsocket = (XSocket*)s;
	return shutdown(xsocket->winSockHandle, how);
}

// #6: XSocketIOCTLSocket
int WINAPI XSocketIOCTLSocket(SOCKET s, long cmd, u_long *argp)
{
	XSocket* xsocket = (XSocket*)s;

	LOG_TRACE_NETWORK("XSocketIOCTLSocket() - cmd: {}", IOCTLSocket_cmd_string(cmd).c_str());
	int ret = ioctlsocket(xsocket->winSockHandle, cmd, argp);

	if (ret == NO_ERROR)
	{
		if (cmd == FIONBIO
			&& *argp)
		{
			LOG_TRACE_NETWORK("XSocketIOCTLSocket() - setting default buffer size for non-blocking socket.");
			// set socket send/recv buffers size, but only if the socket isn't blocking
			xsocket->SetBufferSize(SO_SNDBUF, gXnIpMgr.GetMinSockSendBufferSizeInBytes());
			xsocket->SetBufferSize(SO_RCVBUF, gXnIpMgr.GetMinSockRecvBufferSizeInBytes());

			// remove last error even if we didn't successfuly increased the recv/send buffer size
			WSASetLastError(0);
		}
	}

	return ret;
}

// #7: XSocketSetSockOpt
int WINAPI XSocketSetSockOpt(SOCKET s, int level, int optname, const char *optval, int optlen)
{
	XSocket* xsocket = (XSocket*)s;

	LOG_TRACE_NETWORK("XSocketSetSockOpt  (socket = {:x}, level = {}, optname = {},  optlen = {})",
		xsocket->winSockHandle, level, sockOpt_string(optname), optlen);

	if (optname == SO_SNDBUF
		|| optname == SO_RCVBUF)
	{
		int bufferSize = *(int*)(optval);
		return xsocket->SetBufferSize(optname, bufferSize);
	}

	int ret = setsockopt(xsocket->winSockHandle, level, optname, optval, optlen);
	if (ret == SOCKET_ERROR)
	{
		LOG_TRACE_NETWORK("XSocketSetSockOpt() - error: {}", WSAGetLastError());
	}

	LOG_TRACE_NETWORK("- ret = {:x}", ret);
	return ret;
}

// #8: XSocketGetSockOpt
int WINAPI XSocketGetSockOpt(SOCKET s, int level, int optname, char *optval, int *optlen)
{
	LOG_TRACE_NETWORK("XSocketGetSockOpt()");
	XSocket* xsocket = (XSocket*)s;
	return getsockopt(xsocket->winSockHandle, level, optname, optval, optlen);
}

// #9: XSocketGetSockName
int WINAPI XSocketGetSockName(SOCKET s, struct sockaddr *name, int *namelen)
{
	LOG_TRACE_NETWORK("XSocketGetSockName()");
	XSocket* xsocket = (XSocket*)s;
	return getsockname(xsocket->winSockHandle, name, namelen);
}

// #10
int WINAPI XSocketGetPeerName(SOCKET s, struct sockaddr *name, int *namelen)
{
	LOG_TRACE_NETWORK("XSocketGetPeerName()");
	XSocket* xsocket = (XSocket*)s;

	if (xsocket->IsTCP())
		return getpeername(xsocket->winSockHandle, name, namelen);
	else
		return WSAENOTCONN;
}


// #12: XSocketConnect
int WINAPI XSocketConnect(SOCKET s, const struct sockaddr *name, int namelen)
{
	XSocket* xsocket = (XSocket*)s;
	LOG_TRACE_NETWORK("XSocketConnect  (socket = {0:x}, name = {1:p}, namelen = {2})",
		xsocket->winSockHandle, (void*)name, namelen);

	if (xsocket->IsTCP())
		return connect(xsocket->winSockHandle, name, namelen);
	else
		return SOCKET_ERROR;
}


// #13: XSocketListen
int WINAPI XSocketListen(SOCKET s, int backlog)
{
	XSocket* xsocket = (XSocket*)s;
	LOG_TRACE_NETWORK("XSocketListen  (socket = {0:x}, backlog = {1:x})",
		xsocket->winSockHandle, backlog);

	return listen(xsocket->winSockHandle, backlog);
}


// #14: XSocketAccept
SOCKET WINAPI XSocketAccept(SOCKET s, struct sockaddr *addr, int *addrlen)
{
	XSocket* xsocket = (XSocket*)s;
	
	LIMITED_LOG(35, LOG_TRACE_NETWORK, "XSocketAccept  (socket = {0:x}, addr = {1:p}, addrlen = {2})",
			xsocket->winSockHandle, (void*)addr, *addrlen);

	return accept(xsocket->winSockHandle, addr, addrlen);
}


// #15: XSocketSelect
int WINAPI XSocketSelect(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, const struct timeval *timeout)
{
	LIMITED_LOG(35, LOG_TRACE_NETWORK, "XSocketSelect()");

	return select(nfds, readfds, writefds, exceptfds, timeout);
}

// #16
BOOL WINAPI XSocketWSAGetOverlappedResult(SOCKET s, LPWSAOVERLAPPED lpOverlapped, LPDWORD lpcbTransfer, BOOL fWait, LPDWORD lpdwFlags)
{
	XSocket* xsocket = (XSocket*)s;
	LOG_TRACE_NETWORK("XSocketWSAGetOverlappedResult  (socket = {0:x}, lpWSAOverlapped = {1:p}, lpcbTransfer = {2:p}, fWait = {3}, lpdwFlags = {4:p})",
		xsocket->winSockHandle, (void*)lpOverlapped, (void*)lpcbTransfer, fWait, (void*)lpdwFlags);

	return WSAGetOverlappedResult(xsocket->winSockHandle, lpOverlapped, lpcbTransfer, fWait, lpdwFlags);
}

// #17
BOOL WINAPI XSocketWSACancelOverlappedIO(HANDLE hFile)
{
	LOG_TRACE_NETWORK("XSocketWSACancelOverlappedIO");
	return CancelIo(hFile);
}

int XSocket::winsock_read_socket(
	LPWSABUF lpBuffers, 
	DWORD dwBufferCount,
	LPDWORD lpNumberOfBytesRecvd,
	LPDWORD lpFlags,
	struct sockaddr* lpFrom,
	LPINT lpFromlen,
	LPWSAOVERLAPPED lpOverlapped,
	LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine,
	bool* outWinApiError)
{
	if (outWinApiError)
		*outWinApiError = false;

	if (this->IsTCP()
		|| lpFrom == NULL)
	{
		return WSARecv(
			this->winSockHandle,
			lpBuffers,
			dwBufferCount,
			lpNumberOfBytesRecvd,
			lpFlags,
			lpOverlapped,
			lpCompletionRoutine);
	}

#if COMPILE_WITH_STD_SOCK_FUNC
	int result = ::recvfrom(this->winSockHandle, lpBuffers->buf, lpBuffers->len, *lpFlags, lpFrom, lpFromlen);
	*lpNumberOfBytesRecvd = result;
#else
	int result = WSARecvFrom(this->winSockHandle, lpBuffers, dwBufferCount, lpNumberOfBytesRecvd, lpFlags, lpFrom, lpFromlen, lpOverlapped, lpCompletionRoutine);
#endif
	if (result == SOCKET_ERROR)
	{
		*lpNumberOfBytesRecvd = 0;
		if (outWinApiError)
			*outWinApiError = true;
		return SOCKET_ERROR;
	}

	result = gXnIpMgr.HandleRecvdPacket(this, (sockaddr_in*)lpFrom, lpBuffers, dwBufferCount, lpNumberOfBytesRecvd);
	return result;
}

int XSocket::sock_read(LPWSABUF lpBuffers,
	DWORD dwBufferCount,
	LPDWORD lpNumberOfBytesRecvd, 
	LPDWORD lpFlags,
	struct sockaddr* lpFrom, 
	LPINT lpFromlen, 
	LPWSAOVERLAPPED lpOverlapped, 
	LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
	// loop MAX_PACKETS_TO_READ_PER_RECV_CALL times until we get a valid packet
	// unless Winsock API returns an error
	for (int i = 0; i < MAX_PACKETS_TO_READ_PER_RECV_CALL; i++)
	{
		bool errorByRecvAPI = false;

		int result = winsock_read_socket(lpBuffers, dwBufferCount, lpNumberOfBytesRecvd, lpFlags, lpFrom, lpFromlen, lpOverlapped, lpCompletionRoutine, &errorByRecvAPI);

		if (result == SOCKET_ERROR)
		{
			if (errorByRecvAPI)
			{
				if (WSAGetLastError() != WSAEWOULDBLOCK)
					LOG_ERROR_NETWORK("{} - socket error: {}", __FUNCTION__, WSAGetLastError());

				return result;
			}
			else
			{
				// continue reading the socket, if error wasn't by winsock api
				continue;
			}
		}
		else
		{
			// if there's no error, return the packet
			return result;
		}
	}

	// if we got this far, it means there were no game packets inbound, just XNet packets
	XSocketWSASetLastError(WSAEWOULDBLOCK);
	return SOCKET_ERROR;
}

// #21
int WINAPI XSocketWSARecvFrom(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesRecvd, LPDWORD lpFlags, struct sockaddr *lpFrom, LPINT lpFromlen, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
	XSocket* socket = (XSocket*)s;
	return socket->sock_read(lpBuffers, dwBufferCount, lpNumberOfBytesRecvd, lpFlags, lpFrom, lpFromlen, lpOverlapped, lpCompletionRoutine);
}

// #25
int WINAPI XSocketWSASendTo(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesSent, DWORD dwFlags, sockaddr *lpTo, int iTolen, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
	XSocket* xsocket = (XSocket*)s;
	sockaddr_in* inTo = (sockaddr_in*)lpTo;

	if (xsocket->IsTCP() || inTo == NULL)
	{
		return WSASend(
			xsocket->winSockHandle,
			lpBuffers,
			dwBufferCount,
			lpNumberOfBytesSent,
			dwFlags,
			lpOverlapped,
			lpCompletionRoutine);
	}

	// check if we have to send a broadcast packet
	if (inTo->sin_addr.s_addr == INADDR_BROADCAST
		|| inTo->sin_addr.s_addr == INADDR_ANY)
	{
		XBroadcastPacket* packet = (XBroadcastPacket*)_alloca(sizeof(XBroadcastPacket) + lpBuffers->len);
		new (packet) XBroadcastPacket();

		memcpy((char*)packet + sizeof(XBroadcastPacket), lpBuffers->buf, lpBuffers->len);

		int portOffset = H2Config_base_port % 1000;

		// TODO: properly implement this broadcast BS
		for (int i = 2000; i <= 5000; i += 1000)
		{
			inTo->sin_port = ntohs(i + portOffset + 1);
			int result = sendto(xsocket->winSockHandle, (const char*)packet, sizeof(XBroadcastPacket) + lpBuffers->len, dwFlags, (sockaddr*)inTo, iTolen);
			if (result == SOCKET_ERROR) {
				return SOCKET_ERROR;
			}
		}
		packet->~XBroadcastPacket();
		return 0;
	}

	/*
		Create new SOCKADDR_IN structure,
		If we overwrite the original the game's security functions know it's not a secure address any longer.
		Worst case if this is found to cause performance issues we can handle the send and re-update to secure before return.
	*/

	XnIp* xnIp = gXnIpMgr.GetConnection(inTo->sin_addr);
	if (xnIp != nullptr
		&& !xnIp->ConnectStatusLost())
	{
		sockaddr_in sendToAddr;
		ZeroMemory(&sendToAddr, sizeof(sendToAddr));

		sendToAddr.sin_family = AF_INET;
		sendToAddr.sin_addr = xnIp->GetOnlineIpAddr();
		sendToAddr.sin_port = inTo->sin_port;

		// check if the online ip address is the same as the local one
		// and if the online ip address of the connection is 0, fall back to LAN address
		// to allow packets to be sent even if an account is logged in locally or online (local Xbox profile or Online profile)
		if (xnIp->GetOnlineIpAddr().s_addr == gXnIpMgr.GetLocalUserXn()->GetOnlineIpAddr().s_addr
			|| xnIp->GetOnlineIpAddr().s_addr == 0)
		{
			sendToAddr.sin_addr = xnIp->GetLanIpAddr();
		}

		switch (ntohs(inTo->sin_port))
		{
		case 1000:
			sendToAddr.sin_port = xnIp->m_xnaddr.wPortOnline;
			if (!xsocket->SockAddrInInvalid(xnIp->NatGetAddr(H2v_sockets::Sock1000)))
			{
				// if there's nat data use it
				sendToAddr = *xnIp->NatGetAddr(H2v_sockets::Sock1000);
			}
			break;

		case 1001:
			sendToAddr.sin_port = htons(ntohs(xnIp->m_xnaddr.wPortOnline) + 1);
			if (!xsocket->SockAddrInInvalid(xnIp->NatGetAddr(H2v_sockets::Sock1001)))
			{
				sendToAddr = *xnIp->NatGetAddr(H2v_sockets::Sock1001);
			}
			break;

		default:
			LOG_CRITICAL_NETWORK("XSocketSendTo() port: {} not matched!", ntohs(xnIp->m_xnaddr.wPortOnline));
			return SOCKET_ERROR;
		}

		int result = SOCKET_ERROR;
		DWORD pckSent = 0;
		DWORD dwNumberOfBytesSent = 0;

#if COMPILE_WITH_STD_SOCK_FUNC
		for (DWORD i = 0ul; i < dwBufferCount; i++)
		{
			result = sendto(xsocket->winSockHandle, lpBuffers[i].buf, lpBuffers[i].len, dwFlags, (const sockaddr*)&sendToAddr, sizeof(sendToAddr));
			if (result == SOCKET_ERROR)
				break;

			pckSent++;
			dwNumberOfBytesSent += result;
		}
#else
		result = WSASendTo(xsocket->winSockHandle, lpBuffers, dwBufferCount, &dwNumberOfBytesSent, dwFlags, (const sockaddr*)&sendToAddr, sizeof(sendToAddr), lpOverlapped, lpCompletionRoutine);
#endif // if COMPILE_WITH_STD_SOCK_FUNC

		if (result == SOCKET_ERROR)
		{
			if (WSAGetLastError() != WSAEWOULDBLOCK)
				LOG_ERROR_NETWORK("XSocketSendTo() - socket error: {}", WSAGetLastError());

			if (lpNumberOfBytesSent)
				*lpNumberOfBytesSent = 0;
			return SOCKET_ERROR;
		}
		else
		{
#if !COMPILE_WITH_STD_SOCK_FUNC
			pckSent = dwBufferCount;
#endif
			xnIp->m_pckStats.PckSendStatsUpdate(pckSent, dwNumberOfBytesSent);
			gXnIpMgr.GetLocalUserXn()->m_pckStats.PckSendStatsUpdate(pckSent, dwNumberOfBytesSent);
			if (lpNumberOfBytesSent)
				*lpNumberOfBytesSent = dwNumberOfBytesSent;
			
			return 0;
		}

		return 0;
	}
	else
	{
		LOG_TRACE_NETWORK("XSocketSendTo() - Tried to send packet to unknown connection, connection index: {}, connection identifier: {:x}", 
			XnIp::GetConnectionIndex(inTo->sin_addr), inTo->sin_addr.s_addr);
		XSocketWSASetLastError(WSAEHOSTUNREACH);
		return SOCKET_ERROR;
	}
}

// #26: XSocketInet_Addr
LONG WINAPI XSocketInet_Addr(const char *cp)
{
	LOG_TRACE_NETWORK("XSocketInet_Addr()");
	return inet_addr(cp);
}

// #27: XWSAGetLastError
INT WINAPI XSocketWSAGetLastError()
{
	static int print = 0;

	int ret = WSAGetLastError();

	if (print < 15)
	{
		//LOG_TRACE_NETWORK("XSocketWSAGetLastError() - {}", ret);

		print++;
	}

	return ret;
}

// #28
void WINAPI XSocketWSASetLastError(int iError)
{
	LOG_TRACE_NETWORK("XSocketWSASetLastError()");
	WSASetLastError(iError);
}

// #29
HANDLE WINAPI XSocketWSACreateEvent()
{
	LOG_TRACE_NETWORK("XSocketWSACreateEvent()");
	return WSACreateEvent();
}

// #30
BOOL WINAPI XSocketWSACloseEvent(HANDLE hEvent)
{
	LOG_TRACE_NETWORK("XSocketWSACloseEvent()");
	return WSACloseEvent(hEvent);
}

// #31
BOOL WINAPI XSocketWSASetEvent(HANDLE hEvent)
{
	LOG_TRACE_NETWORK("XSocketWSASetEvent()");
	return WSASetEvent(hEvent);
}

// #32
BOOL WINAPI XSocketWSAResetEvent(HANDLE hEvent)
{
	LOG_TRACE_NETWORK("XSocketWSAResetEvent()");
	return WSAResetEvent(hEvent);
}

// #33
DWORD WINAPI XSocketWSAWaitForMultipleEvents(DWORD cEvents, HANDLE *lphEvents, BOOL fWaitAll, DWORD dwTimeout, BOOL fAlertable)
{
	LOG_TRACE_NETWORK("XSocketWSAWaitForMultipleEvents()");
	return WSAWaitForMultipleEvents(cEvents, lphEvents, fWaitAll, dwTimeout, fAlertable);
}

// #34
int WINAPI XSocketWSAFDIsSet(SOCKET fd, fd_set *a2)
{
	LOG_TRACE_NETWORK("XSocketWSAFDIsSet()");
	return __WSAFDIsSet(fd, a2);
}

// #35
int WINAPI XSocketWSAEventSelect(SOCKET s, HANDLE hEventObject, __int32 lNetworkEvents)
{
	XSocket* xsocket = (XSocket*)s;
	LOG_TRACE_NETWORK("XSocketWSAEventSelect()");
	return WSAEventSelect(xsocket->winSockHandle, hEventObject, lNetworkEvents);
}

// #4
int WINAPI XSocketClose(SOCKET s)
{
	XSocket* xsocket = (XSocket*)s;
	LOG_TRACE_NETWORK("XSocketClose() - socket: {}", xsocket->winSockHandle);

	int ret = closesocket(xsocket->winSockHandle);

	for (auto i = XSocket::Sockets.begin(); i != XSocket::Sockets.end(); ++i)
	{
		if (*i == xsocket)
		{
			XSocket::Sockets.erase(i);
			break;
		}
	}

	delete xsocket;

	return ret;
}

// #11: XSocketBind
SOCKET WINAPI XSocketBind(SOCKET s, const struct sockaddr *name, int namelen)
{
	XSocket* xsocket = (XSocket*)s;

	// copy socket ip/port
	memcpy(&xsocket->name, name, sizeof(sockaddr_in));

	u_short virtual_port = (((struct sockaddr_in*)name)->sin_port);

	switch (ntohs(virtual_port))
	{
	case 1000:
		((struct sockaddr_in*)name)->sin_port = htons(H2Config_base_port);
		break;
	case 1001:
		((struct sockaddr_in*)name)->sin_port = htons(H2Config_base_port + 1);
		break;
	case 1005:
		((struct sockaddr_in*)name)->sin_port = htons(H2Config_base_port + 5);
		break;
	case 1006:
		((struct sockaddr_in*)name)->sin_port = htons(H2Config_base_port + 6);
		break;
	default:
		break;
	}

	LOG_TRACE_NETWORK("XSocketBind() - replaced virtual socket port - {} with: {}", 
		ntohs(virtual_port), ntohs(((struct sockaddr_in*)name)->sin_port));

	int ret = bind(xsocket->winSockHandle, name, namelen);

	if (ret == SOCKET_ERROR)
		LOG_TRACE_NETWORK("{} - SOCKET_ERROR", __FUNCTION__);

	return ret;
}

// #23
int WINAPI XSocketWSASend(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesSent, DWORD dwFlags, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
	return XSocketWSASendTo(s, lpBuffers, dwBufferCount, lpNumberOfBytesSent, dwFlags, NULL, 0, lpOverlapped, lpCompletionRoutine);
}

// #19
int WINAPI XSocketWSARecv(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesRecvd, LPDWORD lpFlags, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
	return XSocketWSARecvFrom(s, lpBuffers, dwBufferCount, lpNumberOfBytesRecvd, lpFlags, NULL, 0, lpOverlapped, lpCompletionRoutine);
}

// #22: XSocketSend
int WINAPI XSocketSend(SOCKET s, const char* buf, int len, int flags)
{
	WSABUF wsaBuf;
	wsaBuf.len = len;
	wsaBuf.buf = (CHAR*)buf;
	DWORD numberOfBytesSent = 0;

	if (XSocketWSASendTo(s, &wsaBuf, 1, &numberOfBytesSent, flags, NULL, NULL, NULL, NULL) == 0)
		return numberOfBytesSent;
	else
		return SOCKET_ERROR;
}

// #24: XSocketSendTo
int WINAPI XSocketSendTo(SOCKET s, const char* buf, int len, int flags, sockaddr *to, int tolen)
{
	WSABUF wsaBuf;
	wsaBuf.len = len;
	wsaBuf.buf = (CHAR*)buf;
	DWORD numberOfBytesSent = 0;

	if (XSocketWSASendTo(s, &wsaBuf, 1, &numberOfBytesSent, flags, to, tolen, NULL, NULL) == 0)
		return numberOfBytesSent;
	else
		return SOCKET_ERROR;
}

// #18: XSocketRecv
int WINAPI XSocketRecv(SOCKET s, char* buf, int len, int flags)
{
	WSABUF wsaBuf[1];
	wsaBuf[0].len = len;
	wsaBuf[0].buf = (CHAR*)buf;
	DWORD numberOfbytesRecvd;

	if (XSocketWSARecvFrom(s, wsaBuf, ARRAYSIZE(wsaBuf), &numberOfbytesRecvd, (LPDWORD)&flags, NULL, NULL, NULL, NULL) == 0)
		return numberOfbytesRecvd;
	else
		return SOCKET_ERROR;
}

// #20
int WINAPI XSocketRecvFrom(SOCKET s, char* buf, int len, int flags, sockaddr *from, int *fromlen)
{
	WSABUF wsaBuf[1];
	wsaBuf[0].len = len;
	wsaBuf[0].buf = (CHAR*)buf;
	DWORD numberOfbytesRecvd;

	if (XSocketWSARecvFrom(s, wsaBuf, ARRAYSIZE(wsaBuf), &numberOfbytesRecvd, (LPDWORD)&flags, from, fromlen, NULL, NULL) == 0)
		return numberOfbytesRecvd;
	else
		return SOCKET_ERROR;
}

// #37: XSocketHTONL
u_long WINAPI XSocketHTONL(u_long hostlong)
{
	return htonl(hostlong);
}

// #38: XSocketNTOHS
u_short WINAPI XSocketNTOHS(u_short netshort)
{
	return ntohs(netshort);
}

// #39: XSocketNTOHL
u_long WINAPI XSocketNTOHL(u_long netlong)
{
	return ntohl(netlong);
}

// #40: XSocketHTONS
u_short WINAPI XSocketHTONS(u_short hostshort)
{
	return htons(hostshort);
}

int XSocket::SetBufferSize(int optname, INT bufSize)
{
	static const std::unordered_set<int> sockOpts = { SO_SNDBUF, SO_RCVBUF };

	if (sockOpts.count(optname) == 0)
	{
		WSASetLastError(WSAEINVAL);
		return SOCKET_ERROR;
	}

	if (!this->IsUDP())
	{
		WSASetLastError(WSAEINVAL);
		return SOCKET_ERROR;
	}

	int bufOpt, bufOptSize;
	bufOptSize = sizeof(bufOpt);

	if (getsockopt(this->winSockHandle, SOL_SOCKET, optname, (char*)&bufOpt, &bufOptSize) == SOCKET_ERROR)
	{
		LOG_ERROR_NETWORK("{} - getsockopt() failed, last error : {}, cannot increase UDP nonblocking buffer size!", __FUNCTION__, WSAGetLastError());
		return SOCKET_ERROR;
	}

	LOG_TRACE_NETWORK("{} - getsockopt() - {}: {} - {}", __FUNCTION__, sockOpt_string(optname), bufOpt, bufSize);

	// this may only affect Windows 7/Server 2008 R2 and bellow, as Windows 10 uses an 64K buffer already
	if (bufOpt < bufSize)
	{
		bufOpt = bufSize; // set the recvbuf to needed size
		// increase socket recv buffer
		if (setsockopt(this->winSockHandle, SOL_SOCKET, optname, (char*)&bufOpt, sizeof(bufOpt)) == SOCKET_ERROR) // then attempt to increase the buffer
		{
			LOG_ERROR_NETWORK("{} - setsockopt() failed, last error: {}", __FUNCTION__, WSAGetLastError());
			return SOCKET_ERROR;
		}
	}

	WSASetLastError(0);
	return 0;
}

int XSocket::UdpSend(const char* buf, int len, int flags, sockaddr *to, int tolen)
{
	return XSocketSendTo((SOCKET)this, buf, len, flags, to, tolen);
}

void XSocket::SocketsDisposeAll()
{
	for (auto xsocket : Sockets)
	{
		XSocketClose((SOCKET)xsocket);
	}
	Sockets.clear();
}
