#include "stdafx.h"
#include "Globals.h"
#include "XLive\Networking\upnp.h"
#include "XLive\UserManagement\CUser.h"
#include "H2MOD\Modules\Config\Config.h"
#include "H2MOD\Modules\Networking\NetworkStats\NetworkStats.h"

int MasterState = 0;
SOCKET game_network_data_gateway_socket_1000 = INVALID_SOCKET; // used for game data
SOCKET game_network_message_gateway_socket_1001 = INVALID_SOCKET; // used for messaging like connection requests

void ForwardPorts()
{
	if (h2mod->Server)
		return;

	ModuleUPnP upnp;

	upnp.UPnPForwardPort(false, H2Config_base_port, H2Config_base_port, "Halo2");
	upnp.UPnPForwardPort(false, (H2Config_base_port + 1), (H2Config_base_port + 1), "Halo2_1");
	//upnp.UPnPForwardPort(false, (H2Config_base_port + 5), (H2Config_base_port + 5), "Halo2_2");
	//upnp.UPnPForwardPort(false, (H2Config_base_port + 6), (H2Config_base_port + 6), "Halo2_3");
	upnp.UPnPForwardPort(true, (H2Config_base_port + 10), (H2Config_base_port + 10), "Halo2_QoS");

	LOG_TRACE_NETWORK("[UPNP] Finished forwarding ports.");
}

// #5310: XOnlineStartup
int WINAPI XOnlineStartup()
{
	LOG_TRACE_NETWORK("XOnlineStartup()");
	SecureZeroMemory(&userManager.game_host_xn, sizeof(XNADDR));
	std::thread(ForwardPorts).detach();

	return ERROR_SUCCESS;
}

SOCKET voice_sock = NULL;
// #3: XCreateSocket
SOCKET WINAPI XCreateSocket(int af, int type, int protocol)
{
	LOG_TRACE_NETWORK("XCreateSocket() af = {0}, type = {1}, protocol = {2}", af, type, protocol);

	bool vdp = false;
	if (protocol == 254)
	{
		vdp = true;
		protocol = IPPROTO_UDP; // We can't support VDP (Voice / Data Protocol) it's some encrypted crap which isn't standard.
	}

	SOCKET ret = socket(af, type, protocol);

	if (vdp)
	{
		LOG_TRACE_NETWORK("Socket: {:x} was VDP", ret);
		voice_sock = ret;
	}

	if (ret == INVALID_SOCKET)
	{
		LOG_TRACE_NETWORK("XCreateSocket - INVALID_SOCKET");
	}

	return ret;
}

// #5332: XSessionEnd
int WINAPI XSessionEnd(DWORD, DWORD)
{
	mapManager->cleanup();
	LOG_TRACE_NETWORK("XSessionEnd()");
	return 0;
}

// #52: XNetCleanup
INT WINAPI XNetCleanup()
{
	LOG_TRACE_NETWORK("XNetCleanup()");
	return 0;
}

// #11: XSocketBind
SOCKET WINAPI XSocketBind(SOCKET s, const struct sockaddr *name, int namelen)
{
	u_short port = (((struct sockaddr_in*)name)->sin_port);

	LOG_TRACE_NETWORK("[H2MOD-Network] game bound socket: {}", htons(port));
	if (s == voice_sock)
		LOG_TRACE_NETWORK("[H2MOD-Network] could be voice socket", htons(port));


	if (htons(port) == 1000) {
		game_network_data_gateway_socket_1000 = s;
		(((struct sockaddr_in*)name)->sin_port) = ntohs(H2Config_base_port);
		LOG_TRACE_NETWORK("[H2MOD-Network] replaced port {0} with {1}", htons(port), H2Config_base_port);
	}

	if (htons(port) == 1001) {
		game_network_message_gateway_socket_1001= s;
		(((struct sockaddr_in*)name)->sin_port) = ntohs(H2Config_base_port + 1);
		LOG_TRACE_NETWORK("[H2MOD-Network] replaced port {0} with {1}", htons(port), H2Config_base_port + 1);
	}

	if (htons(port) == 1005)
		(((struct sockaddr_in*)name)->sin_port) = ntohs(H2Config_base_port + 5);
	if (htons(port) == 1006)
		(((struct sockaddr_in*)name)->sin_port) = ntohs(H2Config_base_port + 6);

	userManager.sockmap[s] = ntohs(port);

	SOCKET ret = bind(s, name, namelen);

	if (ret == SOCKET_ERROR)
		LOG_TRACE_NETWORK("XSocketBind() - SOCKET_ERROR");

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
	LOG_TRACE_XLIVE("XNetCreateKey");
	if (pxnkid && pxnkey)
	{
		XNetRandom((BYTE*)pxnkid, sizeof(XNKID));
		XNetRandom((BYTE*)pxnkey, sizeof(XNKEY));
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
			
		//LOG_TRACE_NETWORK_N("XSocketSendTo - Broadcast");

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
			//LOG_TRACE_XLIVE("XSocketSendTo() port 1000 nPort: {} secure: %08X", htons(nPort), iplong);
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
			//LOG_TRACE_XLIVE("XSocketSendTo() port 1001 nPort: %i secure: %08X", htons(nPort), iplong);
			SendStruct.sin_port = nPort;
		}
		else {
			SendStruct.sin_port = ntohs(2001);
		}

		break;

	default:
		//LOG_TRACE_XLIVE("XSocketSendTo() port: %i not matched!", htons(port));
		break;
	}

	u_long xn = userManager.xnmap[iplong]; // at this point iplong is secure addr

	if (xn != 0)
	{
		SendStruct.sin_addr.s_addr = xn;
	}

	//LOG_TRACE_XLIVE("SendStruct.sin_addr.s_addr == %08X", SendStruct.sin_addr.s_addr);

	if (SendStruct.sin_addr.s_addr == H2Config_ip_wan)
	{
		//LOG_TRACE_XLIVE("Matched g_lWANIP:%08X", H2Config_ip_wan);
		SendStruct.sin_addr.s_addr = H2Config_ip_lan;
		//LOG_TRACE_XLIVE("Replaced send struct s_addr with g_lLANIP: %08X", H2Config_ip_lan);
	}


	int total_bytes_sent = sendto(s, buf, len, flags, (SOCKADDR *)&SendStruct, sizeof(SendStruct));

	if (total_bytes_sent == SOCKET_ERROR)
	{
		LOG_TRACE_NETWORK("XSocketSendTo - Socket Error True");
		LOG_TRACE_NETWORK("XSocketSendTo - WSAGetLastError(): {:x}", WSAGetLastError());
	}
	else
	{
		updateSendToStatistics(len);
	}

	return total_bytes_sent;
}

// #20
int WINAPI XSocketRecvFrom(SOCKET s, char *buf, int len, int flags, sockaddr *from, int *fromlen)
{
	int total_bytes_received = recvfrom(s, buf, len, flags, from, fromlen);

	if (total_bytes_received > 0)
	{
		short port = (((struct sockaddr_in*)from)->sin_port);
		u_long iplong = (((struct sockaddr_in*)from)->sin_addr.s_addr);
		std::pair <ULONG, SHORT> hostpair = std::make_pair(iplong, port);

		SecurePacket* secure_pck = reinterpret_cast<SecurePacket*>(buf);
		if (iplong != H2Config_master_ip)
		{
			if (total_bytes_received == sizeof(SecurePacket) 
				&& secure_pck->annoyance_factor == annoyance_factor)
			{
				ULONG secure = secure_pck->secure.s_addr;	

				LOG_TRACE_NETWORK("[H2MOD-Network] Received secure packet with ip addr {0:x}, port: {1}, secure: {2:x}", htonl(iplong), htons(port), secure);
				userManager.CreateUser(&secure_pck->xn, TRUE);
				userManager.secure_map[hostpair] = secure;

				total_bytes_received = 0;
			}

			ULONG secure = userManager.secure_map[hostpair];

			(((struct sockaddr_in*)from)->sin_addr.s_addr) = secure;

			if (secure == 0)
			{
                // hahaaha -- almic
				LOG_TRACE_NETWORK("[H2MOD-Network] This is probably the issue.... now the fucking recv address is 0 you numb nuts. iplong: {0:x}, port: {1}", htonl(iplong), htons(port));
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
				//LOG_TRACE_XLIVE("XSocketRecvFrom() User.sockmap mapping port 1000 - port: %i, secure: %08X", htons(port), secure);
				userManager.pmap_a[secure] = port;
				break;

			case 1001:
				//LOG_TRACE_XLIVE("XSocketRecvFrom() User.sockmap mapping port 1001 - port: %i, secure: %08X", htons(port), secure);
				userManager.pmap_b[secure] = port;
				break;

			default:
				//LOG_TRACE_XLIVE("XSocketRecvFrom() User.sockmap[s] didn't match any ports!");
				break;

			}
		}
	}

	/*if (ret > 0)
	{
		LOG_TRACE_NETWORK_N("[h2mod-network] received socket data, total: {}", ret);
	}*/

	return total_bytes_received;
}

// #55: XNetRegisterKey //need #51
int WINAPI XNetRegisterKey(XNKID *pxnkid, XNKEY *pxnkey)
{
	LOG_TRACE_NETWORK("XNetRegisterKey()");
	userManager.SetKeys(pxnkid, pxnkey);
	return 0;
}


// #56: XNetUnregisterKey // need #51
int WINAPI XNetUnregisterKey(const XNKID* pxnkid)
{
	LOG_TRACE_NETWORK("XNetUnregisterKey()");
	userManager.EraseKeys();
	return 0;
}
