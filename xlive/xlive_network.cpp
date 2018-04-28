#include "stdafx.h"
#include "CUser.h"
#include "resource.h"
#include <iostream>
#include <sstream>
#include <time.h>
#include <thread>
#include "h2mod.h"
#include <mutex>
#include "Globals.h"
#include "H2Config.h"

bool Connected = false;
bool ThreadCreated = false;
bool NetworkActive = true;

HANDLE H2MOD_Network = NULL;
SOCKET boundsock = INVALID_SOCKET;
SOCKET game_sock = INVALID_SOCKET;
SOCKET game_sock_1000 = INVALID_SOCKET;

CUserManagement User;
clock_t begin, end;
double time_spent;
char* ServerStatus = 0;
int MasterState = 0;

static LARGE_INTEGER timerFreq;
static LARGE_INTEGER counterAtStart;

float getElapsedNetworkTime(void) {
	LARGE_INTEGER c;
	QueryPerformanceCounter(&c);
	return (float)((c.QuadPart - counterAtStart.QuadPart) * 1000.0f / (float)timerFreq.QuadPart);
}

// #5310: XOnlineStartup
int WINAPI XOnlineStartup()
{
	NetworkActive = true;
	//TRACE("XOnlineStartup");
	ServerStatus = new char[250];
	QueryPerformanceFrequency(&timerFreq);
	QueryPerformanceCounter(&counterAtStart);

	snprintf(ServerStatus, 250, "Status: Initializing....");
	
	boundsock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (boundsock == INVALID_SOCKET)
	{
		TRACE("XOnlineStartup - We couldn't intialize our socket");
	}

	return 0;
}


// #5332: XSessionEnd
int WINAPI XSessionEnd(DWORD, DWORD)
{
	NetworkActive = false;
	Connected = false;
	ThreadCreated = false;
	H2MOD_Network = 0;
	mapManager->cleanup();
	TRACE("XSessionEnd");
	return 0;
}



// #52: XNetCleanup
INT WINAPI XNetCleanup()
{
	NetworkActive = false;
	ThreadCreated = false;
	Connected = false;
	H2MOD_Network = 0;

	TRACE("XNetCleanup");
	return 0;
}


// #11: XSocketBind
SOCKET WINAPI XSocketBind(SOCKET s, const struct sockaddr *name, int namelen)
{
	int port = (((struct sockaddr_in*)name)->sin_port);

	if (ntohs(port) == 1001)
	{
		((struct sockaddr_in*)name)->sin_port = htons(H2Config_base_port + 1);
		game_sock = s;
		int nport = (((struct sockaddr_in*)name)->sin_port);
		TRACE("game_sock set for port %i", ntohs(port));
		TRACE("connect_port set to %i instead of %i", ntohs(nport), ntohs(port));
		TRACE("g_port was set to %i", H2Config_base_port);
	}

	if (ntohs(port) == 1000)
	{
		((struct sockaddr_in*)name)->sin_port = htons(H2Config_base_port);
		game_sock_1000 = s;
		int nport = (((struct sockaddr_in*)name)->sin_port);

		TRACE("game_sock set for port %i", ntohs(port));
		TRACE("base_port set to %i instead of %i", ntohs(nport), ntohs(port));
		TRACE("g_port was set to %i", H2Config_base_port);
	}

	if (ntohs(port) == 1002)
		((struct sockaddr_in*)name)->sin_port = htons(H2Config_base_port + 2);
	if (ntohs(port) == 1003)
		((struct sockaddr_in*)name)->sin_port = htons(H2Config_base_port + 3);
	if (ntohs(port) == 1004)
		((struct sockaddr_in*)name)->sin_port = htons(H2Config_base_port + 4);
	if (ntohs(port) == 1005)
		((struct sockaddr_in*)name)->sin_port = htons(H2Config_base_port + 5);
	if(ntohs(port) == 1006)
		((struct sockaddr_in*)name)->sin_port = htons(H2Config_base_port + 6);



	
	
	User.sockmap[s] = ntohs(port);


	SOCKET ret = bind(s, name, namelen);
	
	if (ret == SOCKET_ERROR)
	{
		TRACE("XSocketBind - SOCKET_ERROR");
	}
	return ret;
}

// #53: XNetRandom
INT WINAPI XNetRandom(BYTE * pb, UINT cb)
{
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

		/* These are un-necessary. */
		//pxnkid->ab[0] &= ~XNET_XNKID_MASK;
		//pxnkid->ab[0] |= XNET_XNKID_SYSTEM_LINK;

		NetworkActive = false;
	}

	return 0;
}

// #57: XNetXnAddrToInAddr
INT WINAPI XNetXnAddrToInAddr(XNADDR *pxna, XNKID *pnkid, IN_ADDR *pina)
{
	TRACE("XNetXNAddrToInAddr - ina.s_addr: %08X", pxna->ina.s_addr);
	TRACE("XNetXNAddrToInAddr - secure: %08X", pxna->inaOnline.s_addr);
	ULONG secure = pxna->inaOnline.s_addr;
	
	CUser* user = User.cusers[secure];
	if (user == 0)
	{
		User.CreateUser(pxna, TRUE);
	}

	if (secure !=0)
    {
		pina->s_addr = secure;

	/*	User.cusers_mutex.lock();
			CUser* user = User.cusers[secure];
		User.cusers_mutex.unlock();

		if (user == 0) 
		{
			CUser *nUser = new CUser;
			memset(&nUser->pxna, 0x00, sizeof(XNADDR));
			memcpy(&nUser->pxna, pxna, sizeof(XNADDR));
			std::string ab(reinterpret_cast<const char*>(pxna->abEnet), 6);

			User.cusers_mutex.lock();
			User.xnmap_mutex.lock();
			User.xntosecure_mutex.lock();

			User.cusers[secure] = nUser;
			User.xnmap[secure] = pxna->ina.s_addr;
			User.xntosecure[ab] = secure;

			User.cusers_mutex.unlock();
			User.xnmap_mutex.unlock();
			User.xntosecure_mutex.unlock();
		}*/
	}
	else
	{
		TRACE("XNetXnAddrToInAddr() - Could not find user with the AbEnet Specified calling User.GetSecureFromXN()");

		TRACE("XNetXNAddrToInAddr: pina->s_addr: %08X - User.GetSecureFromXN(pxna)", pina->s_addr);
		pina->s_addr = User.GetSecureFromXN(pxna);
	}

	return 0;
}

// #73: XNetGetTitleXnAddr
DWORD WINAPI XNetGetTitleXnAddr(XNADDR * pAddr)
{
	if (pAddr)
	{
		User.GetLocalXNAddr(pAddr);
	}
	return XNET_GET_XNADDR_STATIC | XNET_GET_XNADDR_ETHERNET;
	//return XNET_GET_XNADDR_PENDING;
}

// #24: XSocketSendTo
int WINAPI XSocketSendTo(SOCKET s, const char *buf, int len, int flags, sockaddr *to, int tolen)
{
	short port = (((struct sockaddr_in*)to)->sin_port);
	//unsigned short normport = ntohs(port);
	u_long iplong = (((struct sockaddr_in*)to)->sin_addr.s_addr);
	ADDRESS_FAMILY af = (((struct sockaddr_in*)to)->sin_family);


	if (iplong == INADDR_BROADCAST || iplong == 0x00)
	{
		(((struct sockaddr_in*)to)->sin_addr.s_addr) = H2Config_master_ip;
		((struct sockaddr_in*)to)->sin_port = ntohs(H2Config_master_port_relay);
		//TRACE("XSocketSendTo - Broadcast");

		return sendto(s, buf, len, flags, to, tolen);
	}

	//TRACE("XSocketSendTo: ( %08X:%i )", iplong, htons(port));

		
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
		int nPort = 0;

		switch (htons(port))
		{
			case 1000:
				nPort = User.pmap_a[iplong];

				if (nPort != 0)
				{
					//TRACE("XSocketSendTo() port 1000 nPort: %i secure: %08X", htons(nPort), iplong);
					SendStruct.sin_port = nPort;
				}

			break;

			case 1001:
				nPort = User.pmap_b[iplong];

				if (nPort != 0)
				{
					//TRACE("XSocketSendTo() port 1001 nPort: %i secure: %08X", htons(nPort), iplong);
					SendStruct.sin_port = nPort;
				}

			break;

			default:
				//TRACE("XSocketSendTo() port: %i not matched!", htons(port));
			break;
		}

		u_long xn = User.xnmap[iplong];

		if (xn != 0)
		{
			SendStruct.sin_addr.s_addr = xn;
		}
		else
		{
			SendStruct.sin_addr.s_addr = User.GetXNFromSecure(iplong);
		}

		//TRACE("SendStruct.sin_addr.s_addr == %08X", SendStruct.sin_addr.s_addr);
		
		if (SendStruct.sin_addr.s_addr == H2Config_ip_wan)
		{
			//TRACE("Matched g_lWANIP:%08X", H2Config_ip_wan);
			SendStruct.sin_addr.s_addr = H2Config_ip_lan;
			//TRACE("Replaced send struct s_addr with g_lLANIP: %08X", H2Config_ip_lan);
		}

	int ret = sendto(s, buf, len, flags, (SOCKADDR *) &SendStruct, sizeof(SendStruct));

	if (ret == SOCKET_ERROR)
	{
		TRACE("XSocketSendTo - Socket Error True");
		TRACE("XSocketSendTo - WSAGetLastError(): %08X", WSAGetLastError());


	}

	return ret;
}

int TimesWrittenTo=0;

// #20
int WINAPI XSocketRecvFrom(SOCKET s, char *buf, int len, int flags, sockaddr *from, int *fromlen)
{
	int ret = recvfrom(s, buf, len, flags, from, fromlen);

	if (ret > 0)
	{
		//TRACE_GAME_N("[h2mod-network] received socket data %s", buf);

		u_long iplong = (((struct sockaddr_in*)from)->sin_addr.s_addr);
		short port    =	(((struct sockaddr_in*)from)->sin_port);
		//unsigned short normport = ntohs(port);
		
		std::pair <ULONG, SHORT> hostpair = std::make_pair(iplong,port); 

		if (iplong != H2Config_master_ip)
		{
	
			if (*(ULONG*)buf == annoyance_factor)
			{
				User.smap[hostpair] = *(ULONG*)(buf + 4);
				
				User.CreateUser((XNADDR*)(buf + 8),TRUE);

				/* Hacky fix, basically if someone swaps IPs we cached their data so we should check if the cached version is still correct. */
				CUser* user = User.cusers[*(ULONG*)(buf + 4)];
				if (user)
				{
					if (user->pxna.ina.s_addr != iplong)
					{
						user->pxna.ina.s_addr = iplong;
					}
				}

				ret = 0;
			}

			ULONG secure = User.smap[hostpair];

			(((struct sockaddr_in*)from)->sin_addr.s_addr) = secure;

			if (secure == 0)
			{
				//TRACE("This is probably the issue.... now the fucking recv address is 0 you numb nuts. iplong: %08X, port: %08X",iplong,htons(port));
			}
			else
			{
				User.xnmap[secure] = iplong;
			}

			/* Store NAT data 
			   First we look at our socket's intended port.
			   port 1000 is mapped to the receiving port pmap_a via the secure address.
			   port 1001 is mapped to the receiving port pmap_b via the secure address.
			  */
			switch (User.sockmap[s])
			{
				case 1000:
					//TRACE("XSocketRecvFrom() User.sockmap mapping port 1000 - port: %i, secure: %08X", htons(port), secure);
						User.pmap_a[secure] = port;
				break;

				case 1001:
					//TRACE("XSocketRecvFrom() User.sockmap mapping port 1001 - port: %i, secure: %08X", htons(port), secure);
						User.pmap_b[secure] = port;
				break;

				default:
					//TRACE("XSocketRecvFrom() User.sockmap[s] didn't match any ports!");
				break;

			}
		}
	}
	return ret;
}

// #55: XNetRegisterKey //need #51
int WINAPI XNetRegisterKey(XNKID *pxnkid, XNKEY *pxnkey)
{
	TRACE("XNetRegisterKey( %08X, %08X )",pxnkid,pxnkey);
	return 0;
}


// #56: XNetUnregisterKey // need #51
int WINAPI XNetUnregisterKey(const XNKID* pxnkid)
{
	Connected = false;
	TRACE("XNetUnregisterKey(%08X)",pxnkid);
	return 0;
}

// #60: XNetInAddrToXnAddr
INT   WINAPI XNetInAddrToXnAddr(const IN_ADDR ina, XNADDR * pxna, XNKID * pxnkid)
{
	CUser* user = User.cusers[ina.s_addr];

	if (user != 0)
	{
		memset(pxna, 0x00, sizeof(XNADDR)); // Zero memory of the current buffer passed to us by the game.
		memcpy(pxna, &user->pxna, sizeof(XNADDR));

	}
	else
	{
		TRACE("XnetInAddrToXnAddr() - Data did not exist calling User.GetXNFromSecure()");

		ULONG xnaddr = User.GetXNFromSecure(ina.s_addr);
		user = User.cusers[ina.s_addr];

		memset(pxna, 0x00, sizeof(XNADDR)); // Zero the memory of the current buffer before doing the copy.
		memcpy(pxna, &user->pxna, sizeof(XNADDR));
	}

	return 0;
}

// #63: XNetUnregisterInAddr
int WINAPI XNetUnregisterInAddr(const IN_ADDR ina)
{
	//User.UnregisterSecureAddr(ina);
	TRACE("XNetUnregisterInAddr: %08X",ina.s_addr);
	return 0;
}