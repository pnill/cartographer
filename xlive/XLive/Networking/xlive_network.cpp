
#include "Globals.h"
#include "XLive\Networking\upnp.h"
#include "XLive\UserManagement\CUser.h"
#include "H2MOD\Modules\Config\Config.h"
#include "H2MOD\Modules\NetworkStats\NetworkStats.h"
#include <Windows.h>

int MasterState = 0;
ModuleUPnP *upnp = nullptr;
SOCKET game_network_data_gateway_socket_1000 = INVALID_SOCKET; // used for game data
SOCKET game_network_message_gateway_socket_1001 = INVALID_SOCKET; // used for messaging like connection requests

void ForwardPorts()
{
	if (upnp == nullptr)
		upnp = new ModuleUPnP;

	upnp->UPnPForwardPort(false, H2Config_base_port, H2Config_base_port, "Halo2");
	upnp->UPnPForwardPort(false, (H2Config_base_port + 1), (H2Config_base_port + 1), "Halo2_1");
	//upnp->UPnPForwardPort(false, (H2Config_base_port + 5), (H2Config_base_port + 5), "Halo2_2");
	//upnp->UPnPForwardPort(false, (H2Config_base_port + 6), (H2Config_base_port + 6), "Halo2_3");
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

	TRACE_GAME_NETWORK_N("[H2MOD-Network] game bound socket: %i", htons(port));
	if (s == voice_sock)
		TRACE_GAME_NETWORK_N("[H2MOD-Network] could be voice socket", htons(port));


	if (htons(port) == 1000) {
		game_network_data_gateway_socket_1000 = s;
		(((struct sockaddr_in*)name)->sin_port) = ntohs(H2Config_base_port);
		TRACE_GAME_NETWORK_N("[H2MOD-Network] replaced port %i with %i", htons(port), H2Config_base_port);
	}

	if (htons(port) == 1001) {
		game_network_message_gateway_socket_1001= s;
		(((struct sockaddr_in*)name)->sin_port) = ntohs(H2Config_base_port + 1);
		TRACE_GAME_NETWORK_N("[H2MOD-Network] replaced port %i with %i", htons(port), H2Config_base_port + 1);
	}

	if (htons(port) == 1005)
		(((struct sockaddr_in*)name)->sin_port) = ntohs(H2Config_base_port + 5);
	if (htons(port) == 1006)
		(((struct sockaddr_in*)name)->sin_port) = ntohs(H2Config_base_port + 6);
	
	userManager.sockmap[s] = ntohs(port);

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
		pxnkid->ab[0] &= ~XNET_XNKID_MASK;
		pxnkid->ab[0] |= XNET_XNKID_SYSTEM_LINK_XPLAT;
	}

	return 0;
}

// #73: XNetGetTitleXnAddr
DWORD WINAPI XNetGetTitleXnAddr(XNADDR * pAddr)
{
	if (pAddr)
	{
		userManager.GetLocalXNAddr(pAddr);
	}
	return XNET_GET_XNADDR_STATIC | XNET_GET_XNADDR_ETHERNET;
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
	u_short nPort = 0;
	switch (htons(port))
	{
	case 1000:
		nPort = userManager.pmap_a[iplong];

		if (nPort != 0)
		{
			//TRACE("XSocketSendTo() port 1000 nPort: %i secure: %08X", htons(nPort), iplong);
			SendStruct.sin_port = nPort;
		}
		else {
			SendStruct.sin_port = ntohs(2000);
		}

		break;

	case 1001:
		nPort = userManager.pmap_b[iplong];

		if (nPort != 0)
		{
			//TRACE("XSocketSendTo() port 1001 nPort: %i secure: %08X", htons(nPort), iplong);
			SendStruct.sin_port = nPort;
		}
		else {
			SendStruct.sin_port = ntohs(2001);
		}

		break;

	default:
		//TRACE("XSocketSendTo() port: %i not matched!", htons(port));
		break;
	}

	u_long xn = userManager.xnmap[iplong];

	if (xn != 0)
	{
		SendStruct.sin_addr.s_addr = xn;
	}

	//TRACE("SendStruct.sin_addr.s_addr == %08X", SendStruct.sin_addr.s_addr);

	if (SendStruct.sin_addr.s_addr == H2Config_ip_wan)
	{
		//TRACE("Matched g_lWANIP:%08X", H2Config_ip_wan);
		SendStruct.sin_addr.s_addr = H2Config_ip_lan;
		//TRACE("Replaced send struct s_addr with g_lLANIP: %08X", H2Config_ip_lan);
	}

	
	int ret = sendto(s, buf, len, flags, (SOCKADDR *)&SendStruct, sizeof(SendStruct));
	
	if (ret == SOCKET_ERROR)
	{
		TRACE_GAME_NETWORK_N("XSocketSendTo - Socket Error True");
		TRACE_GAME_NETWORK_N("XSocketSendTo - WSAGetLastError(): %08X", WSAGetLastError());
	}
	else 
	{
		updateSendToStatistics(len);
	}
	
	return ret;
}

// #20
int WINAPI XSocketRecvFrom(SOCKET s, char *buf, int len, int flags, sockaddr *from, int *fromlen)
{
	int ret = recvfrom(s, buf, len, flags, from, fromlen);
	
	if (ret > 0)
	{
		short port = (((struct sockaddr_in*)from)->sin_port);
		u_long iplong = (((struct sockaddr_in*)from)->sin_addr.s_addr);
		std::pair <ULONG, SHORT> hostpair = std::make_pair(iplong, port);

		if (iplong != H2Config_master_ip)
		{
			if (*(ULONG*)buf == annoyance_factor && ret == 12 + sizeof(XNADDR))
			{
				ULONG secure = *(ULONG*)(buf + 4);

				userManager.secure_map[hostpair] = secure;

				TRACE_GAME_NETWORK("[H2MOD-Network] Received secure packet with ip addr %08X, port: %i, secure: %08X", htonl(iplong), htons(port), secure);
				CUser* user = userManager.cusers[secure]; // Try to get the user by the secure address they've sent.
				if (user)
				{
					if (user->xnaddr.ina.s_addr != iplong)
					{
						user->xnaddr.ina.s_addr = iplong;
					}
				}
				else // only create the user if they're not already in the system.
					userManager.CreateUser((XNADDR*)(buf + 8), TRUE);

				ret = 0;
			}

			ULONG secure = userManager.secure_map[hostpair];

			(((struct sockaddr_in*)from)->sin_addr.s_addr) = secure;

			if (secure == 0)
			{
				TRACE_GAME_NETWORK_N("[H2MOD-Network] This is probably the issue.... now the fucking recv address is 0 you numb nuts. iplong: %08X, port: %i", htonl(iplong), htons(port));
			}
			else
			{
				userManager.xnmap[secure] = iplong;
			}

			/* Store NAT data
			   First we look at our socket's intended port.
			   port 1000 is mapped to the receiving port pmap_a via the secure address.
			   port 1001 is mapped to the receiving port pmap_b via the secure address.
			  */
			switch (userManager.sockmap[s])
			{
			case 1000:
				//TRACE("XSocketRecvFrom() User.sockmap mapping port 1000 - port: %i, secure: %08X", htons(port), secure);
				userManager.pmap_a[secure] = port;
				break;

			case 1001:
				//TRACE("XSocketRecvFrom() User.sockmap mapping port 1001 - port: %i, secure: %08X", htons(port), secure);
				userManager.pmap_b[secure] = port;
				break;

			default:
				//TRACE("XSocketRecvFrom() User.sockmap[s] didn't match any ports!");
				break;

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

