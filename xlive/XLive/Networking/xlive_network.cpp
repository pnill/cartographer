#include "stdafx.h"
#include "XLive\UserManagement\CUser.h"
#include "Globals.h"
#include "H2MOD\Modules\Config\Config.h"
#include "XLive\Networking\upnp.h"

int MasterState = 0;
ModuleUPnP *upnp = nullptr;

void ForwardPorts()
{
	if (upnp == nullptr)
		upnp = new ModuleUPnP;

	upnp->UPnPForwardPort(false, H2Config_base_port, H2Config_base_port, "Halo2");
	upnp->UPnPForwardPort(false, (H2Config_base_port + 1), (H2Config_base_port + 1), "Halo2_1");
	upnp->UPnPForwardPort(false, (H2Config_base_port + 5), (H2Config_base_port + 5), "Halo2_2");
	upnp->UPnPForwardPort(false, (H2Config_base_port + 6), (H2Config_base_port + 6), "Halo2_3");
	upnp->UPnPForwardPort(true, (H2Config_base_port + 10), (H2Config_base_port + 10), "Halo2_QoS");

	TRACE_GAME_NETWORK_N("[UPNP] Finished forwarding ports.");
}

// #5310: XOnlineStartup
int WINAPI XOnlineStartup()
{
	TRACE_GAME_NETWORK_N("XOnlineStartup()");
	memset(&userManager.game_host_xn, NULL, sizeof(XNADDR));
	std::thread(ForwardPorts).detach();

	return ERROR_SUCCESS;
}

SOCKET voice_sock = NULL;
// #3: XCreateSocket
SOCKET WINAPI XCreateSocket(int af, int type, int protocol)
{
	TRACE_GAME_NETWORK_N("XCreateSocket() af = %i, type = %i, protocol = %i", af, type, protocol);

	bool vdp = false;
	if (protocol == 254)
	{
		vdp = true;
		protocol = IPPROTO_UDP; // We can't support VDP (Voice / Data Protocol) it's some encrypted crap which isn't standard.
	}

	SOCKET ret = socket(af, type, protocol);

	if (vdp)
	{
		TRACE_GAME_NETWORK_N("Socket: %08X was VDP", ret);
		voice_sock = ret;
	}

	if (ret == INVALID_SOCKET)
	{
		TRACE_GAME_NETWORK_N("XCreateSocket - INVALID_SOCKET");
	}

	return ret;
}

// #5332: XSessionEnd
int WINAPI XSessionEnd(DWORD, DWORD)
{
	mapManager->cleanup();
	TRACE_GAME_NETWORK_N("XSessionEnd()");
	return 0;
}

// #52: XNetCleanup
INT WINAPI XNetCleanup()
{
	TRACE_GAME_NETWORK_N("XNetCleanup()");
	return 0;
}

// #11: XSocketBind
SOCKET WINAPI XSocketBind(SOCKET s, const struct sockaddr *name, int namelen)
{
	int port = (((struct sockaddr_in*)name)->sin_port);

	if (s == voice_sock)
		TRACE_GAME_NETWORK_N("[h2mod-voice] Game bound potential voice socket to : %i", ntohs(port));
	
	if (ntohs(port) == 1000)
	{
		((struct sockaddr_in*)name)->sin_port = htons(H2Config_base_port);
		int nport = (((struct sockaddr_in*)name)->sin_port);

		TRACE_GAME_NETWORK_N("game_sock set for port %i", ntohs(port));
		TRACE_GAME_NETWORK_N("base_port set to %i instead of %i", ntohs(nport), ntohs(port));
		TRACE_GAME_NETWORK_N("g_port was set to %i", H2Config_base_port);
	}

	if (ntohs(port) == 1001)
	{
		((struct sockaddr_in*)name)->sin_port = htons(H2Config_base_port + 1);
		int nport = (((struct sockaddr_in*)name)->sin_port);

		TRACE_GAME_NETWORK_N("game_sock set for port %i", ntohs(port));
		TRACE_GAME_NETWORK_N("connect_port set to %i instead of %i", ntohs(nport), ntohs(port));
		TRACE_GAME_NETWORK_N("g_port was set to %i", ntohs(H2Config_base_port + 1));
	}

	if (ntohs(port) == 1005)
		((struct sockaddr_in*)name)->sin_port = htons(H2Config_base_port + 5);
	if(ntohs(port) == 1006)
		((struct sockaddr_in*)name)->sin_port = htons(H2Config_base_port + 6);
	if (ntohs(port) == 1007) // used for map downloading, TCP
		((struct sockaddr_in*)name)->sin_port = htons(H2Config_base_port + 7);

	SOCKET ret = bind(s, name, namelen);
	
	if (ret == SOCKET_ERROR)
		TRACE_GAME_NETWORK_N("XSocketBind() - SOCKET_ERROR");
	
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
	}

	return 0;
}

// #73: XNetGetTitleXnAddr
DWORD WINAPI XNetGetTitleXnAddr(XNADDR * pAddr)
{
	if (pAddr && userManager.GetLocalXNAddr(pAddr))
		return XNET_GET_XNADDR_STATIC | XNET_GET_XNADDR_ETHERNET;

	return XNET_GET_XNADDR_PENDING;
}

// #24: XSocketSendTo
int WINAPI XSocketSendTo(SOCKET s, const char *buf, int len, int flags, sockaddr *to, int tolen)
{
	u_long iplong = (((struct sockaddr_in*)to)->sin_addr.s_addr);
	u_short send_port =    (((struct sockaddr_in*)to)->sin_port);

	if (iplong == INADDR_BROADCAST || iplong == 0x00)
	{
		(((struct sockaddr_in*)to)->sin_addr.s_addr) = H2Config_master_ip;
		((struct sockaddr_in*)to)->sin_port = ntohs(H2Config_master_port_relay);
		//TRACE_GAME_NETWORK_N("XSocketSendTo - Broadcast");

		return sendto(s, buf, len, flags, to, tolen);
	}

	SOCKADDR_IN SendStruct;
	SendStruct.sin_port = send_port;
	SendStruct.sin_family = AF_INET;
	SendStruct.sin_addr.s_addr = iplong;

	// get the port of the user we have to send data
	CUser* sendto_user = userManager.iplong_to_user[iplong];
	if (sendto_user == nullptr)
		TRACE_GAME_NETWORK_N("XSocketSendTo() Failed to get the user with ip address: %08X", iplong);

	if (sendto_user != nullptr) 
	{
		switch (htons(send_port))
		{
		case 1000:
			SendStruct.sin_port = sendto_user->xnaddr.wPortOnline;
			break;

		case 1001:
			SendStruct.sin_port = ntohs(htons(sendto_user->xnaddr.wPortOnline) + 1);
			break;

		case 1005:
			SendStruct.sin_port = ntohs(htons(sendto_user->xnaddr.wPortOnline) + 5);
			break;

		case 1006:
			SendStruct.sin_port = ntohs(htons(sendto_user->xnaddr.wPortOnline) + 6);
			break;

		default:
			TRACE_GAME_NETWORK_N("XSocketSendTo() port: %i not matched!", htons(send_port));
			break;
		}
	}

	//TRACE_GAME_NETWORK_N("SendStruct.sin_addr.s_addr == %08X", SendStruct.sin_addr.s_addr);

	if (SendStruct.sin_addr.s_addr == H2Config_ip_wan)
	{
		//TRACE_GAME_NETWORK_N("Matched g_lWANIP:%08X", H2Config_ip_wan);
		SendStruct.sin_addr.s_addr = H2Config_ip_lan;
		//TRACE_GAME_NETWORK_N("Replaced send struct s_addr with g_lLANIP: %08X", H2Config_ip_lan);
	}

	int ret = sendto(s, buf, len, flags, (SOCKADDR *)&SendStruct, sizeof(SendStruct));

	if (ret == SOCKET_ERROR)
	{
		TRACE_GAME_NETWORK_N("XSocketSendTo() - Socket Error True");
		TRACE_GAME_NETWORK_N("XSocketSendTo() - WSAGetLastError(): %08X", WSAGetLastError());
	}

	return ret;
}

// #20
int WINAPI XSocketRecvFrom(SOCKET s, char *buf, int len, int flags, sockaddr *from, int *fromlen)
{
	int ret = recvfrom(s, buf, len, flags, from, fromlen);
	u_long iplong = (((struct sockaddr_in*)from)->sin_addr.s_addr);

	if (iplong == H2Config_master_ip)
	{
		((struct sockaddr_in*)from)->sin_addr.s_addr = INADDR_BROADCAST;
	}

	/*if (ret > 0)
	{
		TRACE_GAME_NETWORK_N("[h2mod-network] received socket data, total: %i", ret);
	}*/

	return ret;
}

// #55: XNetRegisterKey //need #51
int WINAPI XNetRegisterKey(XNKID *pxnkid, XNKEY *pxnkey)
{
	TRACE_GAME_NETWORK_N("XNetRegisterKey()");
	return 0;
}


// #56: XNetUnregisterKey // need #51
int WINAPI XNetUnregisterKey(const XNKID* pxnkid)
{
	TRACE_GAME_NETWORK_N("XNetUnregisterKey()");
	return 0;
}

