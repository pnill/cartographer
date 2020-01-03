#include "stdafx.h"

#include "XnIp.h"
#include "H2MOD\Modules\Config\Config.h"
#include "H2MOD\Modules\Achievements\Achievements.h"
#include "H2MOD\Modules\Networking\NetworkSession\NetworkSession.h"

extern XUID xFakeXuid[4];
extern CHAR g_szUserName[4][16];

CXnIp ipManager;
const DWORD annoyance_factor = 0x8E0A40F1;

/*
NOTE:
	Check inside Tweaks.cpp for removeXNetSecurity
*/

/*
	Use this to get the connection index from an XLive API call
*/
int CXnIp::getConnectionIndex(IN_ADDR connectionIdentifier)
{
	return connectionIdentifier.s_addr >> 24;
}

int CXnIp::sendNatInfoUpdate(SOCKET s, short port)
{
	if (!NetworkSession::localPeerIsSessionHost())
	{
		sockaddr_in sendToAddr;
		if (ipManager.game_host_xn.ina.s_addr != H2Config_ip_wan)
			sendToAddr.sin_addr.s_addr = ipManager.game_host_xn.ina.s_addr;
		else
			sendToAddr.sin_addr.s_addr = H2Config_ip_lan;

		sendToAddr.sin_port = port;
		sendToAddr.sin_family = AF_INET;

		int ret = sendto(s, (char*)&this->securePacket, sizeof(SecurePacket), NULL, (const sockaddr*)&sendToAddr, sizeof(sendToAddr));
		LOG_TRACE_NETWORK("[H2MOD-Network] secure packet sent, return code: {}", ret);
		return ret;
	}
	return -1;
}

IN_ADDR CXnIp::GetConnectionIdentifierByNat(sockaddr* addr)
{
	for (int i = 0; i < XnIPs.max_size(); i++)
	{
		XnIp* xnip = &XnIPs[i];
		if (memcmp(addr, &xnip->NatAddrSocket1000, sizeof(sockaddr_in)) == 0
			|| memcmp(addr, &xnip->NatAddrSocket1001, sizeof(sockaddr_in)) == 0)
		{
			return xnip->connectionIdentifier;
		}
	}

	IN_ADDR addrInval;
	addrInval.s_addr = 0x7F000001;
	return addrInval;
}

/*
	In order to use this, make sure the XnIp in present in the system
*/
void CXnIp::SaveNatInfo(IN_ADDR ipIdentifier)
{
	/*
		Use this function when joining a game
	*/
	int connectionIndex = getConnectionIndex(ipIdentifier);
	XnIp* xnIp = &this->XnIPs[connectionIndex];
	LOG_TRACE_NETWORK("SaveNatInfo() - identifier: {:x}", ipIdentifier.s_addr);

	if (xnIp->bValid
		&& xnIp->connectionIdentifier.s_addr == ipIdentifier.s_addr)
	{
		/* This happens when joining a server, it's a fix to dynamic ports... */
		/* It sets up the host NAT data */

		// TODO: dinamically handle these
		xnIp->NatAddrSocket1000.sin_family = AF_INET;
		xnIp->NatAddrSocket1000.sin_addr = xnIp->xnaddr.ina;
		xnIp->NatAddrSocket1000.sin_port = xnIp->xnaddr.wPortOnline;

		xnIp->NatAddrSocket1001.sin_family = AF_INET;
		xnIp->NatAddrSocket1001.sin_addr = xnIp->xnaddr.ina;
		xnIp->NatAddrSocket1001.sin_port = htons(ntohs(xnIp->xnaddr.wPortOnline) + 1);

		LOG_TRACE_NETWORK("SaveNatInfo() - base port: {}, join port: {}", ntohs(xnIp->NatAddrSocket1000.sin_port), ntohs(xnIp->xnaddr.wPortOnline) + 1);

		// Loopback bs
		if (H2Config_ip_wan == xnIp->xnaddr.ina.s_addr)
		{
			xnIp->NatAddrSocket1000.sin_addr.s_addr = H2Config_ip_lan;
			xnIp->NatAddrSocket1001.sin_addr.s_addr = H2Config_ip_lan;
		}
	}
	
}

void CXnIp::SaveNatInfo(SOCKET s, IN_ADDR ipIdentifier, sockaddr* addr)
{
	/*
		In theory to handle multiple instance servers in the future what we can do is populate the port field of CreateUser,
		Then map the shit before we actually attempt a connection to the server here...

		That way we intercept it and don't even have to modify the game's engine.

		While we only have enough room for one port in the XNADDR struct we can just do, port+1.

		So,
		"connect" socket = 2001 if the XNADDR struct is 2000 which is the "join" socket.

		Then once TeamSpeak is in just do +6 because the game traditionally tries to map 1000-1006 so we'll just go 2007 = TS, etc.

		This should allow us to handle servers listening on any port without much effort or engine modification.
	*/

	LOG_TRACE_NETWORK("SaveNatInfo() - Socket: {}, identifier: {:x}", s, ipIdentifier.s_addr);
	int ipIndex = getConnectionIndex(ipIdentifier);
	XnIp* xnIp = &this->XnIPs[ipIndex];

	if (xnIp->bValid 
		&& xnIp->connectionIdentifier.s_addr == ipIdentifier.s_addr)
	{

		/* Store NAT data
		   First we look at our socket's intended port.
		   port 1000 is mapped to the receiving port pmap_a via the secure address.
		   port 1001 is mapped to the receiving port pmap_b via the secure address.
		*/
		switch (ipManager.sockmap[s])
		{
		case 1000:
			//LOG_TRACE_NETWORK("XSocketRecvFrom() User.sockmap mapping port 1000 - port: %i, secure: %08X", htons(port), secure);
			memcpy(&xnIp->NatAddrSocket1000, addr, sizeof(sockaddr));
			break;

		case 1001:
			//LOG_TRACE_NETWORK("XSocketRecvFrom() User.sockmap mapping port 1001 - port: %i, secure: %08X", htons(port), secure);
			memcpy(&xnIp->NatAddrSocket1001, addr, sizeof(sockaddr));
			break;

		default:
			//LOG_TRACE_NETWORK("XSocketRecvFrom() User.sockmap[s] didn't match any ports!");
			break;

		}
	}
}

void CXnIp::CreateXnIpIdentifier(const XNADDR* pxna, const XNKID* xnkid, IN_ADDR* outIpIdentifier)
{
	/*
		This only happens for servers because we have all their data from the get go :)...

		- Update 1/20/2017 -
		This should now also be called when receiving the 'SecurityPacket' because we have the data on the first attempt to either establish a connection OR on the attempt to join a game,
		That should eliminate the need to talk to the Master server in order to get the XNADDR information from the secure address.
	*/

	int firstUnusedDataIndex = 0;
	bool firstUnusedDataIndexFound = false;

	// check if the user is already in the system
	for (int i = 0; i < XnIPs.max_size(); i++)
	{
		if (XnIPs[i].bValid && memcmp(&XnIPs[i].xnaddr, pxna, sizeof(XNADDR)) == 0)
		{
			if (outIpIdentifier) {
				*outIpIdentifier = XnIPs[i].connectionIdentifier;
				LOG_TRACE_NETWORK("CreateXnIpIdentifier() already present connection, index: {}, identifier: {:x}", i, XnIPs[i].connectionIdentifier.s_addr);
			}
			return;
		}

		if (XnIPs[i].bValid == false && firstUnusedDataIndexFound == false)
		{
			firstUnusedDataIndex = i; // save the first unused data index, in case the XnIp is not in the system, so we don't have to loop again to find a free spot
			firstUnusedDataIndexFound = true;
		}
	
		// if the ip is not in the system, add it
		if (i + 1 == XnIPs.max_size())
		{	
			std::mt19937 mt_rand(rd());
			std::uniform_int_distribution<int> dist(1, 255);

			memcpy(&XnIPs[firstUnusedDataIndex].xnkid, xnkid, sizeof(XNKID));
			memcpy(&XnIPs[firstUnusedDataIndex].xnaddr, pxna, sizeof(XNADDR));
			
			int randIdentifier = dist(mt_rand);
			randIdentifier <<= 8;
			LOG_TRACE_NETWORK("CreateXnIpIdentifier() new connection index {0:x}, identifier {1:x}", firstUnusedDataIndex, htonl(firstUnusedDataIndex | randIdentifier));

			outIpIdentifier->s_addr = htonl(firstUnusedDataIndex | randIdentifier);
			XnIPs[firstUnusedDataIndex].connectionIdentifier.s_addr = htonl(firstUnusedDataIndex | randIdentifier);
			XnIPs[firstUnusedDataIndex].bValid = true;
			return;
		}
	}
}

void CXnIp::UnregisterSecureAddr(const IN_ADDR ina)
{
	int ipIndex = getConnectionIndex(ina);
	if (this->XnIPs[ipIndex].bValid 
		&& this->XnIPs[ipIndex].connectionIdentifier.s_addr == ina.s_addr)
	{
		memset(&this->XnIPs[ipIndex], 0, sizeof(XnIp));
	}
}

void CXnIp::UpdateConnectionStatus() {
	extern int MasterState;
	extern char* ServerStatus;
	if (this->LocalUserLoggedIn()) {
		MasterState = 10;
		if (!h2mod->Server)
			snprintf(ServerStatus, 250, "Status: Online");
	}
	else
	{
		MasterState = 2;
		if (!h2mod->Server)
			snprintf(ServerStatus, 250, "Status: Offline");
	}
}

BOOL CXnIp::LocalUserLoggedIn() {
	return local_user.bValid;
}

void CXnIp::UnregisterLocal()
{
	if (!local_user.bValid)
		return;

	local_user.bValid = false;
	this->UpdateConnectionStatus();
}

void CXnIp::SetKeys(XNKID* xnkid, XNKEY* xnkey)
{
	if (xnkid)
		memcpy(&this->host_xnkid, xnkid, sizeof(XNKID));

	if (xnkey)
		memcpy(&this->host_xnkey, xnkey, sizeof(XNKEY));
}

void CXnIp::EraseKeys()
{
	SecureZeroMemory(&this->host_xnkid, sizeof(XNKID));
	SecureZeroMemory(&this->host_xnkey, sizeof(XNKEY));
}

void CXnIp::GetKeys(XNKID* xnkid, XNKEY* xnkey)
{
	if (xnkid)
		memcpy(xnkid, &this->host_xnkid, sizeof(XNKID));

	if (xnkey)
		memcpy(xnkey, &this->host_xnkey, sizeof(XNKEY));
}

wchar_t ServerLobbyName[32] = { L"Cartographer" };

void SetUserUsername(char* username) {
	SecureZeroMemory(g_szUserName[0], 16);
	snprintf(g_szUserName[0], 16, username);
	if (!h2mod->Server) {

		snprintf(h2mod->GetAddress<char*>(0x971316), 16, username);
		swprintf(h2mod->GetAddress<wchar_t*>(0x96DA94), 16, L"%hs", username);
		swprintf(h2mod->GetAddress<wchar_t*>(0x51A638), 16, L"%hs", username);
		swprintf(ServerLobbyName, 16, L"%hs", username);
	}
}

void CXnIp::ConfigureLocalUser(XNADDR* pxna, XUID xuid, char* username) {
	if (local_user.bValid) {
		local_user.bValid = false;
	}

	// copy local user info
	xFakeXuid[0] = xuid;
	SetUserUsername(username);

	SecureZeroMemory(&local_user, sizeof(XnIp));
	memcpy(&local_user.xnaddr, pxna, sizeof(XNADDR));
	SecureZeroMemory(&this->securePacket, sizeof(SecurePacket));

	// secure packet preparation
	this->securePacket.annoyance_factor = annoyance_factor;
	memcpy(&this->securePacket.xnaddr, &local_user.xnaddr, sizeof(XNADDR));

	local_user.bValid = true;
	this->UpdateConnectionStatus();

	//We want achievements loaded as early as possible, but we can't do it until after we have the XUID.
	std::thread(GetAchievements, xFakeXuid[0]).detach();
}

BOOL CXnIp::GetLocalXNAddr(XNADDR* pxna)
{
	if (local_user.bValid)
	{
		memcpy(pxna, &local_user.xnaddr, sizeof(XNADDR));
		LOG_TRACE_NETWORK("GetLocalXNAddr(): XNADDR: {:x}", pxna->ina.s_addr);
		return TRUE;
	}
	//LOG_TRACE_NETWORK_N("GetLocalXNADDR(): Local user network information not populated yet.");

	return FALSE;
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

// #57: XNetXnAddrToInAddr
INT WINAPI XNetXnAddrToInAddr(const XNADDR *pxna, const XNKID *pxnkid, IN_ADDR *pina)
{
	LOG_TRACE_NETWORK("XNetXnAddrToInAddr(): local-address: {:x}", pxna->ina.s_addr); // TODO
	LOG_TRACE_NETWORK("XNetXnAddrToInAddr(): online-address: {:x}", pxna->inaOnline.s_addr);

	if (pxna == nullptr 
		|| pxnkid == nullptr)
		return ERROR_INVALID_PARAMETER;

	ipManager.CreateXnIpIdentifier(pxna, pxnkid, pina);

	return ERROR_SUCCESS;
}

// #60: XNetInAddrToXnAddr
INT WINAPI XNetInAddrToXnAddr(const IN_ADDR ina, XNADDR* pxna, XNKID* pxnkid)
{
	LOG_TRACE_NETWORK("XNetInAddrToXnAddr() connection index: {:x}, identifier {:x}", ipManager.getConnectionIndex(ina), ina.s_addr);
	
	if (pxna == nullptr
		|| pxnkid == nullptr)
		return ERROR_INVALID_PARAMETER;

	int ipIndex = ipManager.getConnectionIndex(ina);
	XnIp* xnIp = &ipManager.XnIPs[ipIndex];
	
	if (xnIp->bValid && xnIp->connectionIdentifier.s_addr == ina.s_addr)
	{
		memcpy(pxna, &xnIp->xnaddr, sizeof(XNADDR));
		memcpy(pxnkid, &xnIp->xnkid, sizeof(XNKID));

		ipManager.GetKeys(pxnkid, nullptr);

		return ERROR_SUCCESS;
	}

	return ERROR_FUNCTION_FAILED;
}

// #63: XNetUnregisterInAddr
int WINAPI XNetUnregisterInAddr(const IN_ADDR ina)
{
	LOG_TRACE_NETWORK("XNetUnregisterInAddr(): connection index {}, connection identifier: {:x}", ipManager.getConnectionIndex(ina), ina.s_addr);
	ipManager.UnregisterSecureAddr(ina);
	return ERROR_SUCCESS;
}

// #65: XNetConnect
int WINAPI XNetConnect(const IN_ADDR ina)
{
	return ERROR_SUCCESS;
}

// #66: XNetGetConnectStatus
int WINAPI XNetGetConnectStatus(const IN_ADDR ina)
{
	return XNET_CONNECT_STATUS_CONNECTED;
}

// #73: XNetGetTitleXnAddr
DWORD WINAPI XNetGetTitleXnAddr(XNADDR * pAddr)
{
	if (pAddr)
	{
		ipManager.GetLocalXNAddr(pAddr);
	}
	return XNET_GET_XNADDR_STATIC | XNET_GET_XNADDR_ETHERNET;
}