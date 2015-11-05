#include "stdafx.h"
#include "CUser.h"
#include "resource.h"
#include <iostream>
#include <sstream>

using namespace std;
extern ULONG broadcast_server;

SOCKET boundsock = INVALID_SOCKET;

CUserManagement User;
extern UINT g_server;

// #5310: XOnlineStartup
int WINAPI XOnlineStartup()
{
	TRACE("XOnlineStartup");

	boundsock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	
	if (boundsock == INVALID_SOCKET)
	{
		TRACE("XOnlineStartup - We couldn't intialize our socket");
	}

	
	sockaddr_in RecvAddr;
	RecvAddr.sin_family = AF_INET;
	RecvAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	TRACE("Binding our ports g_server = %i", g_server);
	if (g_server == 0)
	{
		RecvAddr.sin_port = htons(27019);
	}
	else
	{
		RecvAddr.sin_port = htons(27018);
	}

	TRACE("Control socket bound to: %d", ntohs(RecvAddr.sin_port));

	int iResult;
	
	iResult = bind(boundsock, (SOCKADDR*)&RecvAddr, sizeof(RecvAddr));
	if (iResult != 0)
	{
		TRACE("XOnlineStartup - bind failed with error %d", WSAGetLastError());
	}
	
	return 0;
}





// #11: XSocketBind
SOCKET WINAPI XSocketBind(SOCKET s, const struct sockaddr *name, int namelen)
{
	int port = (((struct sockaddr_in*)name)->sin_port);
	
	TRACE("XSocketBind  (socket = %X, name = %X, namelen = %d, port = %d)",
		s, name, namelen, ntohs(port));
	User.sockmap[s] = ntohs(port);

	TRACE("XSocketBind(): g_server == %i", g_server);
	if (ntohs(port) == 1000 && g_server == 1)
	{
		TRACE("Setting port to 1100 g_server = 1");
		(((struct sockaddr_in*)name)->sin_port) = 1100;
	}

	if (ntohs(port) == 1001 && g_server == 1)
	{
		TRACE("Setting port to 1101 g_server = 1");
		(((struct sockaddr_in*)name)->sin_port) = 1101;
	}

	if (ntohs(port) == 1005 && g_server == 1)
	{
		TRACE("Setting port to 1105 g_server = 1");
		(((struct sockaddr_in*)name)->sin_port) = 1105;
	}

	if (ntohs(port) == 1006 && g_server == 1)
	{
		TRACE("Setting port to 1106 g_server = 1");
		(((struct sockaddr_in*)name)->sin_port) = 1106;
	}

	SOCKET ret = bind(s, name, namelen);
	
	

	TRACE("XSocketBind() - port: %d Socket: %X", ntohs(port), s);

	if (ret == SOCKET_ERROR)
	{
		TRACE("XSocketBind - SOCKET_ERROR");
	}
	return ret;
}

// #53: XNetRandom
INT WINAPI XNetRandom(BYTE * pb, UINT cb)
{
	TRACE("XNetRandom  (pb = %X, cb = %d)",
		pb, cb);


	if (cb)
		for (DWORD i = 0; i < cb; i++)
			pb[i] = static_cast<BYTE>(rand());


	return 0;
}

// #54: XNetCreateKey
INT WINAPI XNetCreateKey(XNKID * pxnkid, XNKEY * pxnkey)
{
	TRACE("XNetCreateKey");
	if (pxnkid && pxnkey)
	{

		memset(pxnkid, 0xAB, sizeof(XNKID));
		memset(pxnkey, 0XAA, sizeof(XNKEY));

		pxnkid->ab[0] &= ~XNET_XNKID_MASK;
		pxnkid->ab[0] |= XNET_XNKID_SYSTEM_LINK;


	}
	return 0;
}

// #57: XNetXnAddrToInAddr
INT WINAPI XNetXnAddrToInAddr(XNADDR *pxna, XNKID *pnkid, IN_ADDR *pina)
{
	TRACE("XNetXnAddrToInAddr");

	pina->s_addr = 0xABABABAB;

	TRACE("XnetXnAddrToInAddr: %08X", pxna->ina.s_addr);

	User.GetSecureAddr(pxna, pina);
	


	if (pina->s_addr == 0xABABABAB)
	{
		//Make request for users Address store in class so we don't have to request again later.
	}

	return 0;
}

// #73: XNetGetTitleXnAddr
DWORD WINAPI XNetGetTitleXnAddr(XNADDR * pAddr)
{
	static int print = 0;

	if (print < 15)
	{
		TRACE("XNetGetTitleXnAddr  (pAddr = %X)", pAddr);
		print++;
	}

	if (pAddr)
	{
		if( User.GetLocalXNAddr(pAddr) )
			return XNET_GET_XNADDR_STATIC | XNET_GET_XNADDR_ETHERNET;
	}



	return XNET_GET_XNADDR_PENDING;
}

// #24: XSocketSendTo
int WINAPI XSocketSendTo(SOCKET s, const char *buf, int len, int flags, sockaddr *to, int tolen)
{

	int port = (((struct sockaddr_in*)to)->sin_port);
	u_long iplong = (((struct sockaddr_in*)to)->sin_addr.s_addr);
	ADDRESS_FAMILY af = (((struct sockaddr_in*)to)->sin_family);




	char* nBuf = new char[len + 4 + sizeof(XNADDR)]; // Allocate new buffer 4 bytes larger then the original
	memcpy(nBuf + 4 + sizeof(XNADDR), buf, len); // Copy original buffer into our new buffer 4 bytes in.

	*(u_long*)nBuf = User.GetLocalSecLong(); // Store local Secure address in the first 4 bytes of the packet
	XNADDR* p = new XNADDR; // Create new XNADDR struct
	User.GetLocalXNAddr(p); // Get XNADDR data for local user

	memcpy(nBuf + 4, p, sizeof(XNADDR)); // Copy XNADDR data into packet
	
	delete[] p;

	len = len + 4 + sizeof(XNADDR); // Add 4+sizeof(XNADDR) bytes to len on send.
	
	


	TRACE("nBuf: %08X", *(u_long*)nBuf);
	TRACE("User.GetLocalSecLong(): %08X", User.GetLocalSecLong());


	if (iplong == INADDR_BROADCAST || iplong == 0x00 || iplong == broadcast_server)
	{
		(((struct sockaddr_in*)to)->sin_addr.s_addr) = broadcast_server;
		
		TRACE("XSocketSendTo - Broadcast");
	}
	else
	{
		if (User.xnmap[iplong] != 0)
		{
			(((struct sockaddr_in*)to)->sin_addr.s_addr) = User.xnmap[iplong];

			if (User.sockmap[s] == 1000)
			{
				if (User.pmap_a[iplong])
				{
					(((struct sockaddr_in*)to)->sin_port) = User.pmap_a[iplong];

					TRACE_GAME("SendTo(): Sock: %X, (1000)port: %d", s, User.pmap_a[iplong]);
				}
			}

			if (User.sockmap[s] == 1001)
			{
				if (User.pmap_b[iplong])
				{
					(((struct sockaddr_in*)to)->sin_port) = User.pmap_b[iplong];
					TRACE_GAME("SendTo(): Sock: %X, (1001)port: %d", s, User.pmap_b[iplong]);
				}
			}

			if (User.sockmap[s] == 1005)
			{
				if (User.pmap_c[iplong])
				{
					(((struct sockaddr_in*)to)->sin_port) = User.pmap_c[iplong];
				}
			}

			if (User.sockmap[s] == 1006)
			{
				if (User.pmap_d[iplong])
				{
					(((struct sockaddr_in*)to)->sin_port) = User.pmap_d[iplong];
				}
			}
		}
		else
		{
			XNADDR pxna;
			IN_ADDR pina;
			pina.s_addr = iplong;
			memset(&pxna, 0x00, sizeof(XNADDR));

			User.GetXNAddr(&pxna, pina);
			(((struct sockaddr_in*)to)->sin_addr.s_addr) = pxna.ina.s_addr;
			User.xnmap[iplong] = pxna.ina.s_addr;
			
			if (User.sockmap[s] == 1000)
			{
				if (User.pmap_a[iplong])
					(((struct sockaddr_in*)to)->sin_port) = User.pmap_a[iplong];
			}

			if (User.sockmap[s] == 1001)
			{
				if (User.pmap_b[iplong])
					(((struct sockaddr_in*)to)->sin_port) = User.pmap_b[iplong];
			}

			if (User.sockmap[s] == 1005)
			{
				if (User.pmap_c[iplong])
					(((struct sockaddr_in*)to)->sin_port) = User.pmap_c[iplong];
			}

			if (User.sockmap[s] == 1006)
			{
				if (User.pmap_d[iplong])
					(((struct sockaddr_in*)to)->sin_port) = User.pmap_d[iplong];
			}

		}
		
	}

	int ret = sendto(s, nBuf, len, flags, to, tolen); // replace the buffer with our new buffer (nBuf)	

	delete[] nBuf; // We need to cleanup our new buffer to prevent memory leaks.

	if (iplong != INADDR_BROADCAST && iplong != 0x00)
	{
		TRACE("XSocketSendTo ret = %i, port =%d", ret, ntohs(port));
	}

	if (ret == SOCKET_ERROR)
	{
		TRACE("XSocketSendTo - Socket Error True");
		TRACE("XSocketSendTo - WSAGetLastError(): %08X", WSAGetLastError());


	}

	return ret;
}




// #20
int WINAPI XSocketRecvFrom(SOCKET s, char *buf, int len, int flags, sockaddr *from, int *fromlen)
{
	static int print = 0;

	if (print < 15)
	{
		TRACE("XSocketRecvFrom");
		print++;
	}

	int ret = recvfrom(s, buf, len, flags, from, fromlen);

	
	if (ret > 0)
	{
	
		
		u_long saddr = *(u_long*)buf; // get the first 4 bytes of buf.
		u_long iplong = (((struct sockaddr_in*)from)->sin_addr.s_addr);

		XNADDR* xnrecv = new XNADDR; // Create new XNADDR struct
		memcpy(xnrecv, buf + 4, sizeof(XNADDR)); // Copy XNADDR struct from packet into our new XNADDR struct
		ret = ret - 4 - sizeof(XNADDR);  // reduce 4 from the returned length as we removed the secure addr from the packet.
		memcpy(buf, buf + 4 + sizeof(XNADDR), ret); // replace all of buf with the original data now that we have the secure addr.
		

		

		if (User.xnmap[saddr] == 0 && iplong != broadcast_server)
		{
			User.RegisterUser(xnrecv, saddr);
			User.xnmap[saddr] = xnrecv->ina.s_addr;
			delete[] xnrecv;
		}

		TRACE("XSocketRecvFrom(): %08X", saddr);
		
		if (saddr != 0)
		{
			(((struct sockaddr_in*)from)->sin_addr.s_addr) = saddr;

			TRACE_GAME("XSocketRecvFrom() SockMap: %X, Port: %d", s, User.sockmap[s]);
			TRACE_GAME("XSocketRecvFrom(): SecureAddr: %08X", saddr);
			TRACE_GAME("XSocketRecvFrom(): XAddr: %08X", iplong);

			if (User.sockmap[s] == 1000)
			{
				TRACE_GAME("XSocketRecvFrom() XSocketMap: %X");
				User.pmap_a[saddr] = (((struct sockaddr_in*)from)->sin_port);
				TRACE_GAME("XSocketRecvFrom() Recv Port: %d", (((struct sockaddr_in*)from)->sin_port));
				TRACE_GAME("XSocketRecvFrom() - (1000)New Port: %d ", User.pmap_a[saddr]);
			}

			if (User.sockmap[s] == 1001)
			{
				User.pmap_b[saddr] = (((struct sockaddr_in*)from)->sin_port);
				TRACE_GAME("XSocketRecvFrom() Recv Port: %d", htons((((struct sockaddr_in*)from)->sin_port)));
				TRACE_GAME("XSocketRecvFrom() - (1001)New Port: %d ", htons(User.pmap_b[saddr]));
			}

			if (User.sockmap[s] == 1005)
			{
				User.pmap_c[saddr] = (((struct sockaddr_in*)from)->sin_port);
			}

			if (User.sockmap[s] == 1006)
			{
				User.pmap_d[saddr] = (((struct sockaddr_in*)from)->sin_port);
			}

		}
	}


	return ret;
}

// #55: XNetRegisterKey //need #51
int WINAPI XNetRegisterKey(DWORD, DWORD)
{
	TRACE("XNetRegisterKey");
	return 0;
}


// #56: XNetUnregisterKey // need #51
int WINAPI XNetUnregisterKey(DWORD)
{
	TRACE("XNetUnregisterKey");
	return 0;
}


// #60: XNetInAddrToXnAddr
INT   WINAPI XNetInAddrToXnAddr(const IN_ADDR ina, XNADDR * pxna, XNKID * pxnkid)
{
	TRACE("XNetInAddrToXnAddr( pxna_addr: %08X, ina: %08X )", pxna->ina.s_addr, ina.s_addr);

	User.GetXNAddr(pxna, ina);

	TRACE("XNetInAddrToXnAddr( pxna_addr: %08X, ina: %08X ) - After User.GetXNADDR", pxna->ina.s_addr, ina.s_addr);
	return 0;
}

// #63: XNetUnregisterInAddr
int WINAPI XNetUnregisterInAddr(const IN_ADDR ina)
{
	User.UnregisterSecureAddr(ina);

	TRACE("XNetUnregisterInAddr: %08X",ina.s_addr);
	return 0;
}