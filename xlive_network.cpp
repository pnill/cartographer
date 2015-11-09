#include "stdafx.h"
#include "CUser.h"
#include "resource.h"
#include <iostream>
#include <sstream>
#include <time.h>

using namespace std;
extern ULONG broadcast_server;

SOCKET boundsock = INVALID_SOCKET;

CUserManagement User;
clock_t begin, end;
double time_spent;

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
	pxna->inaOnline.s_addr = 0x00000000;

	std::string ab(reinterpret_cast<const char*>(pxna->abEnet), 6);

	if (User.xntosecure[ab] != 0)
	{
		pina->s_addr = User.xntosecure[ab];
		TRACE("XNetXNAddrToInAddr: (User.xntosecure[ab] != NULL - pina->s_addr: %08X", pina->s_addr);
	}
	else
	{
		TRACE("XNetXNAddrToInAddr: pina->s_addr: %08X - User.GetSecureFromXN(pxna)", pina->s_addr);
		pina->s_addr = User.GetSecureFromXN(pxna);
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
	short port = (((struct sockaddr_in*)to)->sin_port);
	u_long iplong = (((struct sockaddr_in*)to)->sin_addr.s_addr);
	ADDRESS_FAMILY af = (((struct sockaddr_in*)to)->sin_family);


	if (iplong == INADDR_BROADCAST || iplong == 0x00)
	{
		(((struct sockaddr_in*)to)->sin_addr.s_addr) = broadcast_server;
		TRACE("XSocketSendTo - Broadcast");

		return sendto(s, buf, len, flags, to, tolen);
	}

		


		/*
			Create new SOCKADDR_IN structure,
			If we overwrite the original the game's security functions know it's not a secure address any longer.
			Worst case if this is found to cause performance issues we can handle the send and re-update to secure before return.
		*/
		SOCKADDR_IN SendStruct;
		SendStruct.sin_port = port;
		SendStruct.sin_addr.s_addr = iplong;
		SendStruct.sin_family = AF_INET;

		/* 
		 Handle NAT map socket to port
		 Switch on port to determine which port we're intending to send to.
		 1000-> User.pmap_a[secureaddress]
		 1001-> User.pmap_b[secureaddress]
		*/

		switch (htons(port))
		{
			case 1000:
				if (User.pmap_a[iplong] = 0)
				{
					SendStruct.sin_port = User.pmap_a[iplong];
				}
			break;

			case 1001:
				if (User.pmap_b[iplong] = 0)
				{
					SendStruct.sin_port = User.pmap_b[iplong];
				}
			break;
		}

		std::pair <ULONG, SHORT> hostpair = std::make_pair(iplong, SendStruct.sin_port);

		u_long xn = User.xnmap[iplong];

		if (xn != 0)
		{
			SendStruct.sin_addr.s_addr = xn;
		}
		else
		{
			SendStruct.sin_addr.s_addr = User.GetXNFromSecure(iplong);
		}


		if (User.sentmap[hostpair] == 0)
		{
			int f = sendto(s, (char*)User.SecurityPacket, 8, flags, (SOCKADDR *) &SendStruct, sizeof(SendStruct));

			User.sentmap[hostpair] = 1;

			if (f == SOCKET_ERROR)
			{
				TRACE("XSocketSendTo - Socket Error True");
				TRACE("XSocketSendTo - WSAGetLastError(): %08X", WSAGetLastError());
			}
		}


	

	int ret = sendto(s, buf, len, flags, (SOCKADDR *) &SendStruct, sizeof(SendStruct));

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
	int ret = recvfrom(s, buf, len, flags, from, fromlen);

	
	if (ret > 0)
	{

		u_long iplong = (((struct sockaddr_in*)from)->sin_addr.s_addr);
		short port    =	(((struct sockaddr_in*)from)->sin_port);
		std::pair <ULONG, SHORT> hostpair = std::make_pair(iplong,port); 

		if (iplong != broadcast_server)
		{

			if (*(ULONG*)buf == 0x11223344)
			{
				User.smap[hostpair] = *(ULONG*)(buf+4);
				ret = 0;
			}

			ULONG secure = User.smap[hostpair];
			(((struct sockaddr_in*)from)->sin_addr.s_addr) = secure;

			/* Store NAT data 
			   First we look at our socket's intended port.
			   port 1000 is mapped to the receiving port pmap_a via the secure address.
			   port 1001 is mapped to the receiving port pma_b via the secure address.
			  */
			switch (User.sockmap[s])
			{
				case 1000:
					User.pmap_a[secure] = port;
				break;

				case 1001:
					User.pmap_b[secure] = port;
				break;

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
	TRACE("XNetInAddrToXnAddr( pxna_addr: %08X, ina: %08X ) - memcpy", pxna->ina.s_addr, ina.s_addr);
	
	memcpy(pxna,&User.cusers[ina.s_addr]->pxna,sizeof(XNADDR));
	pxna->inaOnline.s_addr = 0x00000000;

	TRACE("XNetInAddrToXnAddr( pxna_addr: %08X, ina: %08X ) - Copy &User.cusers[ina.s_addr]->pxna,sizeof(XNADDR)", pxna->ina.s_addr, ina.s_addr);
	return 0;
}

// #63: XNetUnregisterInAddr
int WINAPI XNetUnregisterInAddr(const IN_ADDR ina)
{
	//User.UnregisterSecureAddr(ina);

	TRACE("XNetUnregisterInAddr: %08X",ina.s_addr);
	return 0;
}