#include "stdafx.h"

#include "..\IpManagement\XnIp.h"

// #1: XWSAStartup
int WINAPI XWSAStartup(WORD wVersionRequested, LPWSADATA lpWsaData)
{
	// LOG_TRACE_XLIVE("XWSAStartup(%u, %p)", wVersionRequested, lpWsaData);
	return WSAStartup(wVersionRequested, lpWsaData);
}

// #2: XWSACleanup
int WINAPI XWSACleanup()
{
	LOG_TRACE_XLIVE("XWSACleanup");
	return WSACleanup();
}

// #5: XSocketShutdown
int WINAPI XSocketShutdown(SOCKET s, int how)
{
	LOG_TRACE_NETWORK("XSocketShutdown");
	XSocket* xsocket = (XSocket*)s;
	return shutdown(xsocket->WinSockHandle, how);
}

// #6: XSocketIOCTLSocket
int WINAPI XSocketIOCTLSocket(SOCKET s, __int32 cmd, u_long *argp)
{
	//LOG_TRACE_XLIVE("XSocketIOCTLSocket");
	XSocket* xsocket = (XSocket*)s;
	return ioctlsocket(xsocket->WinSockHandle, cmd, argp);
}

// #7: XSocketSetSockOpt
int WINAPI XSocketSetSockOpt(SOCKET s, int level, int optname, const char *optval, int optlen)
{
	XSocket* xsocket = (XSocket*)s;

	LOG_TRACE_NETWORK("XSocketSetSockOpt  (socket = {0:x}, level = {1}, optname = {2}, optval = {3}, optlen = {4})",
		xsocket->WinSockHandle, level, optname, optval ? optval : "", optlen);

	if ((level & SO_BROADCAST) > 0)
	{
		LOG_TRACE_NETWORK("XSocketSetSockOpt - SO_BROADCAST");
	}
	int ret = setsockopt(xsocket->WinSockHandle, level, optname, optval, optlen);
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
	XSocket* xsocket = (XSocket*)s;
	//LOG_TRACE_XLIVE("XSocketGetSockOpt");
	return getsockopt(xsocket->WinSockHandle, level, optname, optval, optlen);
}

// #9: XSocketGetSockName
int WINAPI XSocketGetSockName(SOCKET s, struct sockaddr *name, int *namelen)
{
	LOG_TRACE_XLIVE("XSocketGetSockName");
	XSocket* xsocket = (XSocket*)s;
	return getsockname(xsocket->WinSockHandle, name, namelen);
}

// #10
int WINAPI XSocketGetPeerName(SOCKET s, struct sockaddr *name, int *namelen)
{
	LOG_TRACE_XLIVE("XSocketGetPeerName");
	XSocket* xsocket = (XSocket*)s;
	return getpeername(xsocket->WinSockHandle, name, namelen);
}


// #12: XSocketConnect
int WINAPI XSocketConnect(SOCKET s, const struct sockaddr *name, int namelen)
{
	XSocket* xsocket = (XSocket*)s;
	LOG_TRACE_XLIVE("XSocketConnect  (socket = {0:x}, name = {1:p}, namelen = {2})",
		xsocket->WinSockHandle, (void*)name, namelen);

	return connect(xsocket->WinSockHandle, name, namelen);
}


// #13: XSocketListen
int WINAPI XSocketListen(SOCKET s, int backlog)
{
	XSocket* xsocket = (XSocket*)s;
	LOG_TRACE_XLIVE("XSocketListen  (socket = {0:x}, backlog = {1:x})",
		xsocket->WinSockHandle, backlog);

	return listen(xsocket->WinSockHandle, backlog);
}


// #14: XSocketAccept
SOCKET WINAPI XSocketAccept(SOCKET s, struct sockaddr *addr, int *addrlen)
{
	XSocket* xsocket = (XSocket*)s;

	static int print = 0;
	if (print < 25)
	{
		LOG_TRACE_XLIVE("XSocketAccept  (socket = {0:x}, addr = {1:p}, addrlen = {2})",
			xsocket->WinSockHandle, (void*)addr, *addrlen);

		print++;
	}


	return accept(xsocket->WinSockHandle, addr, addrlen);
}


// #15: XSocketSelect
int WINAPI XSocketSelect(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, const struct timeval *timeout)
{
	static int print = 0;
	if (print < 15)
	{
		LOG_TRACE_XLIVE("XSocketSelect");

		print++;
	}

	return select(nfds, readfds, writefds, exceptfds, timeout);
}

// #16
BOOL WINAPI XSocketWSAGetOverlappedResult(SOCKET s, LPWSAOVERLAPPED lpOverlapped, LPDWORD lpcbTransfer, BOOL fWait, LPDWORD lpdwFlags)
{
	XSocket* xsocket = (XSocket*)s;
	LOG_TRACE_XLIVE("XSocketWSAGetOverlappedResult  (socket = {0:x}, lpWSAOverlapped = {1:p}, lpcbTransfer = {2:p}, fWait = {3}, lpdwFlags = {4:p})",
		xsocket->WinSockHandle, (void*)lpOverlapped, (void*)lpcbTransfer, fWait, (void*)lpdwFlags);

	return WSAGetOverlappedResult(xsocket->WinSockHandle, lpOverlapped, lpcbTransfer, fWait, lpdwFlags);
}

// #17
BOOL WINAPI XSocketWSACancelOverlappedIO(HANDLE hFile)
{
	LOG_TRACE_XLIVE("XSocketWSACancelOverlappedIO");
	return CancelIo(hFile);
}

// #18: XSocketRecv
int WINAPI XSocketRecv(SOCKET s, char * buf, int len, int flags)
{
	XSocket* xsocket = (XSocket*)s;
	LOG_TRACE_XLIVE("XSocketRecv");
	return recv(xsocket->WinSockHandle, buf, len, flags);
}

// #19
int WINAPI XSocketWSARecv(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesRecvd, LPDWORD lpFlags, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
	XSocket* xsocket = (XSocket*)s;
	LOG_TRACE_XLIVE("XSocketWSARecv");
	return WSARecv(xsocket->WinSockHandle, lpBuffers, dwBufferCount, lpNumberOfBytesRecvd, lpFlags, lpOverlapped, lpCompletionRoutine);
}


// #21
int WINAPI XSocketWSARecvFrom(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesRecvd, LPDWORD lpFlags, struct sockaddr *lpFrom, LPINT lpFromlen, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
	XSocket* xsocket = (XSocket*)s;
	LOG_TRACE_XLIVE("XSocketWSARecvFrom");
	return WSARecvFrom(
		xsocket->WinSockHandle,
		lpBuffers,
		dwBufferCount,
		lpNumberOfBytesRecvd,
		lpFlags,
		lpFrom,
		lpFromlen,
		lpOverlapped,
		lpCompletionRoutine);
}

// #22: XSocketSend
int WINAPI XSocketSend(SOCKET s, const char *buf, int len, int flags)
{
	XSocket* xsocket = (XSocket*)s;
	LOG_TRACE_XLIVE("XSocketSend");
	return send(xsocket->WinSockHandle, buf, len, flags);
}

// #23
int WINAPI XSocketWSASend(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesSent, DWORD dwFlags, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
	XSocket* xsocket = (XSocket*)s;
	LOG_TRACE_XLIVE("XSocketWSASend");
	return WSASend(xsocket->WinSockHandle, lpBuffers, dwBufferCount, lpNumberOfBytesSent, dwFlags, lpOverlapped, lpCompletionRoutine);
}

// #25
int WINAPI XSocketWSASendTo(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesSent, DWORD dwFlags, sockaddr *lpTo, int iTolen, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
	XSocket* xsocket = (XSocket*)s;
	LOG_TRACE_XLIVE("XSocketWSASendTo");
	lpTo->sa_family = AF_INET;
	return WSASendTo(
		xsocket->WinSockHandle,
		lpBuffers,
		dwBufferCount,
		lpNumberOfBytesSent,
		dwFlags,
		lpTo,
		iTolen,
		lpOverlapped,
		lpCompletionRoutine);
}

// #26: XSocketInet_Addr
LONG WINAPI XSocketInet_Addr(const char *cp)
{
	LOG_TRACE_XLIVE("XSocketInet_Addr");
	return inet_addr(cp);
}

// #27: XWSAGetLastError
INT WINAPI XSocketWSAGetLastError()
{
	static int print = 0;

	int ret = WSAGetLastError();

	if (print < 15)
	{
		//("XSocketWSAGetLastError");
		//LOG_TRACE_XLIVE("XSocketWSAGetLastError Ret = %i", ret);

		print++;
	}


	return ret;
}

// #28
VOID WINAPI XSocketWSASetLastError(int iError)
{
	LOG_TRACE_XLIVE("XSocketWSASetLastError");
	WSASetLastError(iError);
}

// #29
HANDLE WINAPI XSocketWSACreateEvent()
{
	LOG_TRACE_XLIVE("XSocketWSACreateEvent");
	return WSACreateEvent();
}

// #30
BOOL WINAPI XSocketWSACloseEvent(HANDLE hEvent)
{
	LOG_TRACE_XLIVE("XSocketWSACloseEvent");
	return WSACloseEvent(hEvent);
}

// #31
BOOL WINAPI XSocketWSASetEvent(HANDLE hEvent)
{
	LOG_TRACE_XLIVE("XSocketWSASetEvent");
	return WSASetEvent(hEvent);
}

// #32
BOOL WINAPI XSocketWSAResetEvent(HANDLE hEvent)
{
	LOG_TRACE_XLIVE("XSocketWSAResetEvent");
	return WSAResetEvent(hEvent);
}

// #33
DWORD WINAPI XSocketWSAWaitForMultipleEvents(DWORD cEvents, HANDLE *lphEvents, BOOL fWaitAll, DWORD dwTimeout, BOOL fAlertable)
{
	LOG_TRACE_XLIVE("XSocketWSAWaitForMultipleEvents");
	return WSAWaitForMultipleEvents(cEvents, lphEvents, fWaitAll, dwTimeout, fAlertable);
}

// #34
int WINAPI XSocketWSAFDIsSet(SOCKET fd, fd_set *a2)
{
	LOG_TRACE_XLIVE("XSocketWSAFDIsSet");
	return __WSAFDIsSet(fd, a2);
}

// #35
int WINAPI XSocketWSAEventSelect(SOCKET s, HANDLE hEventObject, __int32 lNetworkEvents)
{
	XSocket* xsocket = (XSocket*)s;
	LOG_TRACE_XLIVE("XSocketWSAEventSelect");
	return WSAEventSelect(xsocket->WinSockHandle, hEventObject, lNetworkEvents);
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
