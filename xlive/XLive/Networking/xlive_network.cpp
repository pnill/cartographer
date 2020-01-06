#include "Globals.h"

#include "..\Cryptography\Rc4.h"
#include "XLive\Networking\upnp.h"
#include "XLive\IpManagement\XnIp.h"
#include "H2MOD\Modules\Config\Config.h"
#include "H2MOD\Modules\Networking\NetworkStats\NetworkStats.h"

int MasterState = 0;
XECRYPT_RC4_STATE Rc4StateRand;
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
		LOG_TRACE_NETWORK("XCreateSocket() - INVALID_SOCKET");
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
		LOG_TRACE_NETWORK("[H2MOD-Network] replaced port {} with {}", htons(port), H2Config_base_port);
	}

	if (htons(port) == 1001) {
		game_network_message_gateway_socket_1001 = s;
		(((struct sockaddr_in*)name)->sin_port) = ntohs(H2Config_base_port + 1);
		LOG_TRACE_NETWORK("[H2MOD-Network] replaced port {} with {}", htons(port), H2Config_base_port + 1);
	}

	if (htons(port) == 1005)
		(((struct sockaddr_in*)name)->sin_port) = ntohs(H2Config_base_port + 5);
	if (htons(port) == 1006)
		(((struct sockaddr_in*)name)->sin_port) = ntohs(H2Config_base_port + 6);

	ipManager.sockmap[s] = htons(port);

	SOCKET ret = bind(s, name, namelen);

	if (ret == SOCKET_ERROR)
		LOG_TRACE_NETWORK("XSocketBind() - SOCKET_ERROR");

	return ret;
}

// #53: XNetRandom
INT WINAPI XNetRandom(BYTE * pb, UINT cb)
{
	static bool Rc4CryptInitialized = false;

	LARGE_INTEGER key;

	if (Rc4CryptInitialized == false)
	{
		QueryPerformanceCounter(&key);
		XeCryptRc4Key(&Rc4StateRand, (BYTE*)&key, sizeof(LARGE_INTEGER));
		Rc4CryptInitialized = true;
	}

	XeCryptRc4Ecb(&Rc4StateRand, pb, cb);
	return ERROR_SUCCESS;
}

// #24: XSocketSendTo
int WINAPI XSocketSendTo(SOCKET s, const char *buf, int len, int flags, sockaddr *to, int tolen)
{
	if (((struct sockaddr_in*)to)->sin_addr.s_addr == INADDR_BROADCAST) // handle broadcast
	{
		(((struct sockaddr_in*)to)->sin_addr.s_addr) = H2Config_master_ip;
		((struct sockaddr_in*)to)->sin_port = ntohs(H2Config_master_port_relay);
			
		//LOG_TRACE_NETWORK_N("XSocketSendTo - Broadcast");

		int result = sendto(s, buf, len, flags, (sockaddr*)to, sizeof(sockaddr));
		
		return result;
	}

	/*
		Create new SOCKADDR_IN structure,
		If we overwrite the original the game's security functions know it's not a secure address any longer.
		Worst case if this is found to cause performance issues we can handle the send and re-update to secure before return.
	*/

	u_long connectionIndex = ipManager.getConnectionIndex(((struct sockaddr_in*)to)->sin_addr);
	XnIp* xnIp = &ipManager.XnIPs[connectionIndex];

	sockaddr_in sendAddress;
	sendAddress.sin_family = AF_INET;
	sendAddress.sin_addr = xnIp->xnaddr.ina;
	sendAddress.sin_port = ((struct sockaddr_in*)to)->sin_port;

	//LOG_TRACE_XLIVE("SendStruct.sin_addr.s_addr == %08X", sendAddress.sin_addr.s_addr);

	if (sendAddress.sin_addr.s_addr == H2Config_ip_wan)
	{
		sendAddress.sin_addr.s_addr = H2Config_ip_lan;
		//LOG_TRACE_XLIVE("Replaced send struct s_addr with g_lLANIP: %08X", H2Config_ip_lan);
	}

	/*
	 Handle NAT map socket to port
	 Switch on port to determine which port we're intending to send to.
	 1000-> User.pmap_a[secureaddress]
	 1001-> User.pmap_b[secureaddress]
	*/
	u_short nPort = 0;
	switch (htons(((struct sockaddr_in*)to)->sin_port))
	{
	case 1000:
		nPort = xnIp->NatAddrSocket1000.sin_port;

		if (nPort != 0)
		{
			//LOG_TRACE_XLIVE("XSocketSendTo() port 1000 nPort: {} secure: %08X", htons(nPort), iplong);
			sendAddress.sin_port = nPort;
		}

		break;

	case 1001:
		nPort = xnIp->NatAddrSocket1001.sin_port;

		if (nPort != 0)
		{
			//LOG_TRACE_XLIVE("XSocketSendTo() port 1001 nPort: %i secure: %08X", htons(nPort), iplong);
			sendAddress.sin_port = nPort;
		}

		break;

	default:
		//LOG_TRACE_XLIVE("XSocketSendTo() port: %i not matched!", htons(port));
		break;
	}

	int result = sendto(s, buf, len, flags, (const sockaddr*)&sendAddress, sizeof(sendAddress));

	if (result == SOCKET_ERROR)
	{
		LOG_TRACE_NETWORK("XSocketSendTo() - Socket Error: {:x}", WSAGetLastError());
		return SOCKET_ERROR;
	}
	else
	{
		updateSendToStatistics(result);
	}

	return result;
}

// #20
int WINAPI XSocketRecvFrom(SOCKET s, char *buf, int len, int flags, sockaddr *from, int *fromlen)
{
	int result = recvfrom(s, buf, len, flags, from, fromlen);

	if (result == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSAEWOULDBLOCK)
			LOG_TRACE_NETWORK("XSocketRecvFrom() - Socket Error: {:x}", WSAGetLastError());
		return SOCKET_ERROR;
	}
	else if (result > 0)
	{
		u_long iplong = ((struct sockaddr_in*)from)->sin_addr.s_addr;

		SecurePacket* secure_pck = reinterpret_cast<SecurePacket*>(buf);
		if (iplong != H2Config_master_ip)
		{
			if (result == sizeof(SecurePacket)
				&& secure_pck->annoyance_factor == annoyance_factor)
			{
				IN_ADDR ipIdentification;

				LOG_TRACE_NETWORK("[H2MOD-Network] Received secure packet with ip address {:x}, port: {}", htonl(iplong), htons(((struct sockaddr_in*)from)->sin_port));
				XNetXnAddrToInAddr(&secure_pck->xnaddr, &secure_pck->xnkid, &ipIdentification); // create identification key for the new connection
				ipManager.SaveConnectionNatInfo(s, ipIdentification, from); // copy the nat info in the connection structure

				result = 0;
			}

			((struct sockaddr_in*)from)->sin_addr = ipManager.GetConnectionIdentifierByNat(from);		
		}
	}
	

	/*if (ret > 0)
	{
		LOG_TRACE_NETWORK("XSocketRecvFrom() received socket data, total: {}", bytesReceived);
	}*/

	return result;
}

// #55: XNetRegisterKey //need #51
int WINAPI XNetRegisterKey(XNKID *pxnkid, XNKEY *pxnkey)
{
	LOG_TRACE_NETWORK("XNetRegisterKey()");
	ipManager.SetKeys(pxnkid, pxnkey);
	return 0;
}


// #56: XNetUnregisterKey // need #51
int WINAPI XNetUnregisterKey(const XNKID* pxnkid)
{
	LOG_TRACE_NETWORK("XNetUnregisterKey()");
	ipManager.EraseKeys();
	return 0;
}
