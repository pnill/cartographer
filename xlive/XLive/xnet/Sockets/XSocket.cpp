#include "Globals.h"

#include "XLive\xnet\upnp.h"
#include "XLive\xnet\Sockets\XSocket.h"
#include "XLive\xnet\IpManagement\XnIp.h"
#include "H2MOD\Modules\Config\Config.h"
#include "H2MOD\Modules\Networking\NetworkStats\NetworkStats.h"

#include "..\IpManagement\XnIp.h"

ModuleUPnP* upnp;

void ForwardPorts()
{
	if (h2mod->Server)
		return;

	if (upnp == nullptr)
		upnp = new ModuleUPnP;

	upnp->UPnPForwardPort(false, H2Config_base_port, H2Config_base_port, "Halo2");
	upnp->UPnPForwardPort(false, (H2Config_base_port + 1), (H2Config_base_port + 1), "Halo2_1");
	upnp->UPnPForwardPort(true, (H2Config_base_port + 10), (H2Config_base_port + 10), "Halo2_QoS");

	LOG_TRACE_NETWORK("ForwardPorts() - Finished forwarding ports.");
}

// #5310: XOnlineStartup
int WINAPI XOnlineStartup()
{
	LOG_TRACE_NETWORK("XOnlineStartup()");
	std::thread(ForwardPorts).detach();
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

	if (protocol == IPPROTO_TCP)
		return SOCKET_ERROR; // we dont support TCP yet

	// TODO: support TCP
	XSocket* newXSocket = new XSocket;
	SecureZeroMemory(newXSocket, sizeof(XSocket));
	if (protocol == IPPROTO_UDP)
	{
		newXSocket->protocol = IPPROTO_UDP;
	}
	else if (protocol == IPPROTO_VDP)
	{
		protocol = IPPROTO_UDP; // We can't support VDP (Voice / Data Protocol) it's some encrypted crap which isn't standard.
		newXSocket->protocol = IPPROTO_UDP;
		newXSocket->isVoiceSocket = true;
	}

	SOCKET ret = socket(af, type, protocol);

	if (ret == INVALID_SOCKET)
	{
		LOG_ERROR_NETWORK("XSocketCreate() - Invalid socket, last error: {}", WSAGetLastError());
		delete newXSocket;
		return ret;
	}

	newXSocket->winSockHandle = ret;

	if (newXSocket->isVoiceSocket)
	{
		LOG_TRACE_NETWORK("XSocketCreate() - Socket: {} is VDP", ret);
	}

	ipManager.SocketPtrArray.push_back(newXSocket);

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
int WINAPI XSocketIOCTLSocket(SOCKET s, __int32 cmd, u_long *argp)
{
	//LOG_TRACE_NETWORK("XSocketIOCTLSocket");
	XSocket* xsocket = (XSocket*)s;
	return ioctlsocket(xsocket->winSockHandle, cmd, argp);
}

// #7: XSocketSetSockOpt
int WINAPI XSocketSetSockOpt(SOCKET s, int level, int optname, const char *optval, int optlen)
{
	XSocket* xsocket = (XSocket*)s;

	LOG_TRACE_NETWORK("XSocketSetSockOpt  (socket = {0:x}, level = {1}, optname = {2}, optval = {3}, optlen = {4})",
		xsocket->winSockHandle, level, optname, optval ? optval : "", optlen);

	if ((level & SO_BROADCAST) > 0)
	{
		LOG_TRACE_NETWORK("XSocketSetSockOpt - SO_BROADCAST");
	}
	int ret = setsockopt(xsocket->winSockHandle, level, optname, optval, optlen);
	if (ret == SOCKET_ERROR)
	{
		LOG_TRACE_NETWORK("XSocketSetSockOpt - SOCKET_ERROR");
	}

	LOG_TRACE_NETWORK("- ret = {:x}", ret);
	return ret;
}

// #8: XSocketGetSockOpt
int WINAPI XSocketGetSockOpt(SOCKET s, int level, int optname, char *optval, int *optlen)
{
	//LOG_TRACE_NETWORK("XSocketGetSockOpt()");
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

	if (xsocket->isTCP())
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

	if (xsocket->isTCP())
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

	static int print = 0;
	if (print < 25)
	{
		LOG_TRACE_NETWORK("XSocketAccept  (socket = {0:x}, addr = {1:p}, addrlen = {2})",
			xsocket->winSockHandle, (void*)addr, *addrlen);

		print++;
	}


	return accept(xsocket->winSockHandle, addr, addrlen);
}


// #15: XSocketSelect
int WINAPI XSocketSelect(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, const struct timeval *timeout)
{
	static int print = 0;
	if (print < 15)
	{
		LOG_TRACE_NETWORK("XSocketSelect()");

		print++;
	}

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

// #21
int WINAPI XSocketWSARecvFrom(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesRecvd, LPDWORD lpFlags, struct sockaddr *lpFrom, LPINT lpFromlen, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
	XSocket* xsocket = (XSocket*)s;

	if (xsocket->isTCP() || lpFrom == NULL)
	{
		return WSARecv(
			xsocket->winSockHandle,
			lpBuffers,
			dwBufferCount,
			lpNumberOfBytesRecvd,
			lpFlags,
			lpOverlapped,
			lpCompletionRoutine);
	}
	
#if COMPILE_WITH_STD_SOCK_FUNC
	int result = recvfrom(xsocket->winSockHandle, lpBuffers->buf, lpBuffers->len, *lpFlags, lpFrom, lpFromlen);
	*lpNumberOfBytesRecvd = result;
#else
	int result = WSARecvFrom(xsocket->winSockHandle, lpBuffers, dwBufferCount, lpNumberOfBytesRecvd, lpFlags, lpFrom, lpFromlen, lpOverlapped, lpCompletionRoutine);
#endif
	if (result == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSAEWOULDBLOCK)
			LOG_ERROR_NETWORK("XSocketWSARecvFrom() - Socket Error: {}", WSAGetLastError());

		return SOCKET_ERROR;
	}

	return ipManager.handleRecvdPacket(xsocket, (sockaddr_in*)lpFrom, lpBuffers, *lpNumberOfBytesRecvd);;
}

// #25
int WINAPI XSocketWSASendTo(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesSent, DWORD dwFlags, sockaddr *lpTo, int iTolen, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
	XSocket* xsocket = (XSocket*)s;
	sockaddr_in* inTo = (sockaddr_in*)lpTo;

	if (xsocket->isTCP() || inTo == NULL)
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

	if (inTo->sin_addr.s_addr == INADDR_BROADCAST) // handle broadcast
	{
		inTo->sin_addr.s_addr = H2Config_master_ip;
		inTo->sin_port = ntohs(H2Config_master_port_relay);

		int result = sendto(xsocket->winSockHandle, lpBuffers->buf, lpBuffers->len, dwFlags, (sockaddr*)lpTo, iTolen);
		return result;
	}

	/*
		Create new SOCKADDR_IN structure,
		If we overwrite the original the game's security functions know it's not a secure address any longer.
		Worst case if this is found to cause performance issues we can handle the send and re-update to secure before return.
	*/

	XnIp* xnIp = &ipManager.XnIPs[ipManager.getConnectionIndex(inTo->sin_addr)];
	if (xnIp->isValid(inTo->sin_addr))
	{
		sockaddr_in sendToAddr;
		sendToAddr.sin_family = AF_INET;
		sendToAddr.sin_addr = xnIp->xnaddr.ina;

		//LOG_TRACE_NETWORK("SendStruct.sin_addr.s_addr == %08X", sendAddress.sin_addr.s_addr);

		if (sendToAddr.sin_addr.s_addr == H2Config_ip_wan)
		{
			sendToAddr.sin_addr.s_addr = H2Config_ip_lan;
			//LOG_TRACE_NETWORK("Replaced send struct s_addr with g_lLANIP: %08X", H2Config_ip_lan);
		}

		/* TODO: handle this dynamically */
		u_short hPort = 0;
		switch (xsocket->getNetworkSocketPort())
		{
		case 1000:
			hPort = xnIp->NatAddrSocket1000.sin_port;

			if (hPort != 0)
			{
				//LOG_TRACE_NETWORK("XSocketSendTo() port 1000 nPort: {} secure: %08X", htons(nPort), iplong);
				sendToAddr.sin_port = hPort;
			}
			else
			{
				sendToAddr.sin_port = xnIp->xnaddr.wPortOnline;
			}

			break;

		case 1001:
			hPort = xnIp->NatAddrSocket1001.sin_port;

			if (hPort != 0)
			{
				//LOG_TRACE_NETWORK("XSocketSendTo() port 1001 nPort: %i secure: %08X", htons(nPort), iplong);
				sendToAddr.sin_port = hPort;
			}
			else
			{
				sendToAddr.sin_port = ntohs(htons(xnIp->xnaddr.wPortOnline) + 1);
			}

			break;

		default:
			//LOG_TRACE_NETWORK("XSocketSendTo() port: %i not matched!", htons(port));
			break;
		}

#if COMPILE_WITH_STD_SOCK_FUNC
		int result = sendto(xsocket->winSockHandle, lpBuffers->buf, lpBuffers->len, dwFlags, (const sockaddr*)&sendToAddr, sizeof(sendToAddr));
		*lpNumberOfBytesSent = result;
#else
		int result = WSASendTo(xsocket->winSockHandle, lpBuffers, dwBufferCount, lpNumberOfBytesSent, dwFlags, (const sockaddr*)&sendToAddr, sizeof(sendToAddr), lpOverlapped, lpCompletionRoutine);
#endif

		if (result == SOCKET_ERROR)
		{
			if (WSAGetLastError() != WSAEWOULDBLOCK)
				LOG_ERROR_NETWORK("XSocketSendTo() - Socket Error: {}", WSAGetLastError());
			return SOCKET_ERROR;
		}
		else
		{
			updateSendToStatistics(*lpNumberOfBytesSent);
		}

		return ERROR_SUCCESS;
	}
	else
	{
		LOG_TRACE_NETWORK("XSocketSendTo() - Tried to send packet to unknown connection, connection index: {}, connection identifier: {}", ipManager.getConnectionIndex(inTo->sin_addr), inTo->sin_addr.s_addr);
		WSASetLastError(WSAEHOSTUNREACH);
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
VOID WINAPI XSocketWSASetLastError(int iError)
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

	for (auto i = ipManager.SocketPtrArray.begin(); i != ipManager.SocketPtrArray.end(); ++i)
	{
		if (*i == xsocket)
		{
			ipManager.SocketPtrArray.erase(i);
			break;
		}
	}

	delete xsocket;

	return ret;
}

// #52: XNetCleanup
INT WINAPI XNetCleanup()
{
	LOG_TRACE_NETWORK("XNetCleanup()");
	for (auto xsocket : ipManager.SocketPtrArray)
	{
		XSocketClose((SOCKET)xsocket);
		delete xsocket;
	}
	ipManager.SocketPtrArray.clear();

	return 0;
}

// #11: XSocketBind
SOCKET WINAPI XSocketBind(SOCKET s, const struct sockaddr *name, int namelen)
{
	if (name->sa_family == AF_INET6)
		return WSAEADDRNOTAVAIL;

	XSocket* xsocket = (XSocket*)s;

	u_short port = (((struct sockaddr_in*)name)->sin_port);

	memcpy(&xsocket->name, name, sizeof(sockaddr_in));

	if (htons(port) == 1000) {
		(((struct sockaddr_in*)name)->sin_port) = ntohs(H2Config_base_port);
		LOG_TRACE_NETWORK("XSocketBind() - replaced port {} with {}", htons(port), H2Config_base_port);
	}

	if (htons(port) == 1001) {
		(((struct sockaddr_in*)name)->sin_port) = ntohs(H2Config_base_port + 1);
		LOG_TRACE_NETWORK("XSocketBind() - replaced port {} with {}", htons(port), H2Config_base_port + 1);
	}

	if (htons(port) == 1005)
		(((struct sockaddr_in*)name)->sin_port) = ntohs(H2Config_base_port + 5);
	if (htons(port) == 1006)
		(((struct sockaddr_in*)name)->sin_port) = ntohs(H2Config_base_port + 6);

	int ret = bind(xsocket->winSockHandle, name, namelen);

	if (ret == SOCKET_ERROR)
		LOG_TRACE_NETWORK("XSocketBind() - SOCKET_ERROR");

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
	wsaBuf.buf = (CHAR*)buf;
	wsaBuf.len = len;
	DWORD numberOfBytesSent = 0;

	if (XSocketWSASendTo(s, &wsaBuf, 1, &numberOfBytesSent, flags, NULL, NULL, NULL, NULL) != ERROR_SUCCESS)
		return SOCKET_ERROR;
	else
		return numberOfBytesSent;
}

// #24: XSocketSendTo
int WINAPI XSocketSendTo(SOCKET s, const char* buf, int len, int flags, sockaddr *to, int tolen)
{
	WSABUF wsaBuf;
	wsaBuf.buf = (CHAR*)buf;
	wsaBuf.len = len;
	DWORD numberOfBytesSent = 0;

	if ( XSocketWSASendTo(s, &wsaBuf, 1, &numberOfBytesSent, flags, to, tolen, NULL, NULL) != ERROR_SUCCESS)
		return SOCKET_ERROR;
	else
		return numberOfBytesSent;
}

// #18: XSocketRecv
int WINAPI XSocketRecv(SOCKET s, char* buf, int len, int flags)
{
	WSABUF wsaBuf;
	wsaBuf.buf = buf;
	wsaBuf.len = len;
	DWORD numberOfbytesRecvd;

	if (XSocketWSARecvFrom(s, &wsaBuf, 1, &numberOfbytesRecvd, (LPDWORD)&flags, NULL, NULL, NULL, NULL) != ERROR_SUCCESS)
		return SOCKET_ERROR;
	else
		return numberOfbytesRecvd;
}

// #20
int WINAPI XSocketRecvFrom(SOCKET s, char* buf, int len, int flags, sockaddr *from, int *fromlen)
{
	WSABUF wsaBuf;
	wsaBuf.buf = buf;
	wsaBuf.len = len;
	DWORD numberOfbytesRecvd;

	if (XSocketWSARecvFrom(s, &wsaBuf, 1, &numberOfbytesRecvd, (LPDWORD)&flags, from, fromlen, NULL, NULL) != ERROR_SUCCESS)
		return SOCKET_ERROR;
	else
		return numberOfbytesRecvd;
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