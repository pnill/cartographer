#include "stdafx.h"

#include "Globals.h"

// #1: XWSAStartup
int WINAPI XWSAStartup(WORD wVersionRequested, LPWSADATA lpWsaData)
{
	// TRACE("XWSAStartup(%u, %p)", wVersionRequested, lpWsaData);
	return WSAStartup(wVersionRequested, lpWsaData);
}

// #2: XWSACleanup
int WINAPI XWSACleanup()  	// XWSACleanup
{
	TRACE("XWSACleanup");
	return WSACleanup();
}

// #4
int WINAPI XSocketClose(SOCKET s)
{
	TRACE("XSocketClose: socket: %d", s);
	return closesocket(s);
}

// #5: XSocketShutdown
int WINAPI XSocketShutdown(SOCKET s, int how)
{
	TRACE("XSocketShutdown");
	return shutdown(s, how);
}

// #6: XSocketIOCTLSocket
int WINAPI XSocketIOCTLSocket(SOCKET s, __int32 cmd, u_long *argp)
{
	//TRACE("XSocketIOCTLSocket");
	return ioctlsocket(s, cmd, argp);
}

// #7: XSocketSetSockOpt
int WINAPI XSocketSetSockOpt(SOCKET s, int level, int optname, const char *optval, int optlen)
{
	int ret;

	TRACE("XSocketSetSockOpt  (socket = %X, level = %d, optname = %d, optval = %s, optlen = %d)",
		s, level, optname, optval ? optval : "", optlen);

	if ((level & SO_BROADCAST) > 0)
	{

		TRACE("XSocketSetSockOpt - SO_BROADCAST");
	}
	ret = setsockopt(s, level, optname, optval, optlen);
	if (ret == SOCKET_ERROR)
	{
		TRACE("XSocketSetSockOpt - SOCKET_ERROR");
	}

	TRACE("- ret = %X", ret);
	return ret;
}

// #8: XSocketGetSockOpt
int WINAPI XSocketGetSockOpt(SOCKET s, int level, int optname, char *optval, int *optlen)
{
	//TRACE("XSocketGetSockOpt");
	return getsockopt(s, level, optname, optval, optlen);
}

// #9: XSocketGetSockName
int WINAPI XSocketGetSockName(SOCKET s, struct sockaddr *name, int *namelen)
{
	TRACE("XSocketGetSockName");
	return getsockname(s, name, namelen);
}

// #10
int WINAPI XSocketGetPeerName(SOCKET s, struct sockaddr *name, int *namelen)
{
	TRACE("XSocketGetPeerName");
	return getpeername(s, name, namelen);
}


// #12: XSocketConnect
int WINAPI XSocketConnect(SOCKET s, const struct sockaddr *name, int namelen)
{
	TRACE("XSocketConnect  (socket = %X, name = %X, namelen = %d)",
		s, name, namelen);

	return connect(s, name, namelen);
}


// #13: XSocketListen
int WINAPI XSocketListen(SOCKET s, int backlog)
{
	TRACE("XSocketListen  (socket = %X, backlog = %X)",
		s, backlog);

	return listen(s, backlog);
}


// #14: XSocketAccept
SOCKET WINAPI XSocketAccept(SOCKET s, struct sockaddr *addr, int *addrlen)
{
	static int print = 0;


	if (print < 25)
	{
		TRACE("XSocketAccept  (socket = %X, addr = %X, addrlen = %d)",
			s, addr, *addrlen);

		print++;
	}


	return accept(s, addr, addrlen);
}


// #15: XSocketSelect
int WINAPI XSocketSelect(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, const struct timeval *timeout)
{
	static int print = 0;


	if (print < 15)
	{
		TRACE("XSocketSelect");

		print++;
	}

	return select(nfds, readfds, writefds, exceptfds, timeout);
}

// #16
BOOL WINAPI XSocketWSAGetOverlappedResult(SOCKET s, LPWSAOVERLAPPED lpOverlapped, LPDWORD lpcbTransfer, BOOL fWait, LPDWORD lpdwFlags)
{
	TRACE("XSocketWSAGetOverlappedResult  (socket = %X, lpWSAOverlapped = %X, lpcbTransfer = %X, fWait = %d, lpdwFlags = %X)",
		s, lpOverlapped, lpcbTransfer, fWait, lpdwFlags);

	return WSAGetOverlappedResult(s, lpOverlapped, lpcbTransfer, fWait, lpdwFlags);
}

// #17
BOOL WINAPI XSocketWSACancelOverlappedIO(HANDLE hFile)
{
	TRACE("XSocketWSACancelOverlappedIO");
	return CancelIo(hFile);
}

// #18: XSocketRecv
int WINAPI XSocketRecv(SOCKET s, char * buf, int len, int flags)
{
	TRACE("XSocketRecv");
	return recv(s, buf, len, flags);
}

// #19
int WINAPI XSocketWSARecv(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesRecvd, LPDWORD lpFlags, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
	TRACE("XSocketWSARecv");
	return WSARecv(s, lpBuffers, dwBufferCount, lpNumberOfBytesRecvd, lpFlags, lpOverlapped, lpCompletionRoutine);
}


// #21
int WINAPI XSocketWSARecvFrom(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesRecvd, LPDWORD lpFlags, struct sockaddr *lpFrom, LPINT lpFromlen, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
	TRACE("XSocketWSARecvFrom");
	return WSARecvFrom(
		s,
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
	TRACE("XSocketSend");
	return send(s, buf, len, flags);
}

// #23
int WINAPI XSocketWSASend(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesSent, DWORD dwFlags, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
	TRACE("XSocketWSASend");
	return WSASend(s, lpBuffers, dwBufferCount, lpNumberOfBytesSent, dwFlags, lpOverlapped, lpCompletionRoutine);
}

// #25
int WINAPI XSocketWSASendTo(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesSent, DWORD dwFlags, sockaddr *lpTo, int iTolen, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
	TRACE("XSocketWSASendTo");
	lpTo->sa_family = AF_INET;
	return WSASendTo(
		s,
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
	TRACE("XSocketInet_Addr");
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
		//TRACE("XSocketWSAGetLastError Ret = %i", ret);

		print++;
	}


	return ret;
}

// #28
VOID WINAPI XSocketWSASetLastError(int iError)
{
	TRACE("XSocketWSASetLastError");
	WSASetLastError(iError);
}

// #29
HANDLE WINAPI XSocketWSACreateEvent()
{
	TRACE("XSocketWSACreateEvent");
	return WSACreateEvent();
}

// #30
BOOL WINAPI XSocketWSACloseEvent(HANDLE hEvent)
{
	TRACE("XSocketWSACloseEvent");
	return WSACloseEvent(hEvent);
}

// #31
BOOL WINAPI XSocketWSASetEvent(HANDLE hEvent)
{
	TRACE("XSocketWSASetEvent");
	return WSASetEvent(hEvent);
}

// #32
BOOL WINAPI XSocketWSAResetEvent(HANDLE hEvent)
{
	TRACE("XSocketWSAResetEvent");
	return WSAResetEvent(hEvent);
}

// #33
DWORD WINAPI XSocketWSAWaitForMultipleEvents(DWORD cEvents, HANDLE *lphEvents, BOOL fWaitAll, DWORD dwTimeout, BOOL fAlertable)
{
	TRACE("XSocketWSAWaitForMultipleEvents");
	return WSAWaitForMultipleEvents(cEvents, lphEvents, fWaitAll, dwTimeout, fAlertable);
}

// #34
int WINAPI XSocketWSAFDIsSet(SOCKET fd, fd_set *a2)
{
	TRACE("XSocketWSAFDIsSet");
	return __WSAFDIsSet(fd, a2);
}

// #35
int WINAPI XSocketWSAEventSelect(SOCKET s, HANDLE hEventObject, __int32 lNetworkEvents)
{
	TRACE("XSocketWSAEventSelect");
	return WSAEventSelect(s, hEventObject, lNetworkEvents);
}

// #37: XSocketHTONL
u_long WINAPI XSocketHTONL(u_long hostlong)
{
	u_long ret = htonl(hostlong);

	return ret;
}

// #38: XSocketNTOHS
u_short WINAPI XSocketNTOHS(u_short netshort)
{
	u_short ret;

	ret = ntohs(netshort);

	return ret;
}


// #39: XSocketNTOHL
u_long WINAPI XSocketNTOHL(u_long netlong)
{
	u_long ret = ntohl(netlong);

	return ret;
}


// #40: XSocketHTONS
u_short WINAPI XSocketHTONS(u_short a1)
{

	u_short ret = htons(a1);

	return ret;
}