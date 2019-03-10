#include "stdafx.h"
#include "XLive\UserManagement\CUser.h"
#include "Globals.h"
#include "H2MOD\Modules\Config\Config.h"
#include "XLive\Networking\upnp.h"

int MasterState = 0;
ModuleUPnP *upnp = nullptr;
SOCKET game_network_message_gateway_socket = INVALID_SOCKET;

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
	u_short port = (((struct sockaddr_in*)name)->sin_port);

	TRACE_GAME_NETWORK_N("[H2MOD-Network] game bound socket: %i", ntohs(port));
	if (s == voice_sock)
		TRACE_GAME_NETWORK_N("[H2MOD-Network] could be voice socket", ntohs(port));

	if (ntohs(port) == H2Config_base_port + 1)
		game_network_message_gateway_socket = s;

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
	u_short port = (((struct sockaddr_in*)to)->sin_port);
	u_long iplong = (((struct sockaddr_in*)to)->sin_addr.s_addr);

	if (iplong == INADDR_BROADCAST || iplong == 0x00)
	{
		(((struct sockaddr_in*)to)->sin_addr.s_addr) = H2Config_master_ip;
		((struct sockaddr_in*)to)->sin_port = ntohs(H2Config_master_port_relay);
		//TRACE_GAME_NETWORK_N("XSocketSendTo - Broadcast");

		return sendto(s, buf, len, flags, to, tolen);
	}

	SOCKADDR_IN SendStruct;
	SendStruct.sin_port = port;
	SendStruct.sin_family = AF_INET;
	SendStruct.sin_addr.s_addr = iplong;

	// get the port of the user we have to send data
	CUser* userToSend = userManager.address_to_user[iplong];
	if (userToSend == nullptr)
		TRACE_GAME_NETWORK_N("XSocketSendTo() Failed to get the user with ip or secure address: %08X", iplong);

	if (userToSend != nullptr) 
	{
		switch (htons(port))
		{
		case 1000:
			SendStruct.sin_port = userToSend->xnaddr.wPortOnline;
			break;

		case 1001:
			SendStruct.sin_port = ntohs(htons(userToSend->xnaddr.wPortOnline) + 1);
			break;

		case 1005:
			SendStruct.sin_port = ntohs(htons(userToSend->xnaddr.wPortOnline) + 5);
			break;

		case 1006:
			SendStruct.sin_port = ntohs(htons(userToSend->xnaddr.wPortOnline) + 6);
			break;

		default:
			TRACE_GAME_NETWORK_N("XSocketSendTo() port: %i not matched!", htons(port));
			break;
		}
		SendStruct.sin_addr.s_addr = userToSend->xnaddr.ina.s_addr;
	}

	//TRACE_GAME_NETWORK_N("SendStruct.sin_addr.s_addr == %08X", SendStruct.sin_addr.s_addr);

	if (SendStruct.sin_addr.s_addr == H2Config_ip_wan)
	{
		//TRACE_GAME_NETWORK_N("Matched g_lWANIP:%08X", H2Config_ip_wan);
		SendStruct.sin_addr.s_addr = H2Config_ip_lan;
		//TRACE_GAME_NETWORK_N("Replaced send struct s_addr with g_lLANIP: %08X", H2Config_ip_lan);
	}

	
	/*extern void addDebugText(const char*);
	static int max_dbg;
	if (max_dbg < 20)
	{
		std::string dbg = "sendto port:" + std::to_string(htons(SendStruct.sin_port));
		addDebugText(dbg.c_str());
		max_dbg++;
	}*/

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
	u_short port  = (((struct sockaddr_in*)from)->sin_port);
	u_long iplong = (((struct sockaddr_in*)from)->sin_addr.s_addr);

	if (iplong == H2Config_master_ip)
		return ret;
	
	if (ret > 0)
	{
		if (using_secure)
		{
			std::pair<ULONG, short> spair = std::make_pair(iplong, port);
			if (*(DWORD*)&buf[0] == annoyance_factor)
			{
				XNADDR* nuser = reinterpret_cast<XNADDR*>(&buf[4]);
				TRACE_GAME_NETWORK_N("[H2MOD-Network] received secure packet from user with XNADDR: %08X", nuser->ina.s_addr);
				userManager.CreateUser(nuser);
				spair = std::make_pair(iplong, nuser->wPortOnline);
				ret = 0;
			}

			ULONG secure = userManager.secure_map[spair];

			if (secure != NULL)
			{
				((struct sockaddr_in*)from)->sin_addr.s_addr = secure;
			}
			else
			{
				TRACE_GAME_NETWORK_N("[H2MOD-Network] recvfrom: this is probably the issue....now the fucking recv address is 0 you numb nuts. iplong: %08X, port: %i", iplong, htons(port));
			}
		}
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

