#include "stdafx.h"
#include "CUser.h"
#include "H2MOD\Modules\Config\Config.h"
#include "H2MOD\Modules\OnScreenDebug\OnScreenDebug.h"
#include "H2MOD\Modules\Startup\Startup.h"
#include "H2MOD\Modules\Achievements\Achievements.h"
#include "XLive\UserManagement\protobuf\packet.pb.h"
#include "H2MOD\Modules\Networking\NetworkSession\NetworkSession.h"

extern XUID xFakeXuid[4];
extern CHAR g_szUserName[4][16];

SecurePacket securePacket;
CUserManagement userManager;
const DWORD annoyance_factor = 0x11223344;

/*
NOTE:
	Check inside Tweaks.cpp for removeXNetSecurity
*/

extern SOCKET game_network_message_gateway_socket;

int CUserManagement::sendSecurePacket(SOCKET s, short to_port)
{
	if (!NetworkSession::localPeerIsSessionHost())
	{
		SOCKADDR_IN sendToAddr;
		short port = to_port == 1000 ? userManager.game_host_xn.wPortOnline : ntohs(htons(userManager.game_host_xn.wPortOnline) + 1);
		if (userManager.game_host_xn.ina.s_addr != H2Config_ip_wan)
			sendToAddr.sin_addr.s_addr = userManager.game_host_xn.ina.s_addr;
		else
			sendToAddr.sin_addr.s_addr = H2Config_ip_lan;

		sendToAddr.sin_port = port;
		sendToAddr.sin_family = AF_INET;

		int ret = sendto(s, (char*)&securePacket, sizeof(SecurePacket), NULL, (SOCKADDR*)&sendToAddr, sizeof(sendToAddr));
		LOG_TRACE_NETWORK("[H2MOD-Network] secure packet sent, return code: {}", ret);
		return ret;
	}
	return -1;
}

void CUserManagement::CreateUser(const XNADDR* pxna, BOOL user)
{
	/*
		This only happens for servers because we have all their data from the get go :)...

		- Update 1/20/2017 -
		This should now also be called when receiving the 'SecurityPacket' because we have the data on the first attempt to either establish a connection OR on the attempt to join a game,
		That should eliminate the need to talk to the Master server in order to get the XNADDR information from the secure address.
	*/

	LOG_TRACE_NETWORK("[Resources-Clear] CreateUser executed on thread {:x}", GetCurrentThreadId());
	ULONG secure = pxna->inaOnline.s_addr;
	CUser* nUser = cusers[secure];
	// check if the user is already in the system
	if (nUser == nullptr)
	{
		// allocate new mem for the new peer
		LOG_TRACE_NETWORK("CUserManagement::CreateUser() new secure address {:x}", pxna->inaOnline.s_addr);

		nUser = new CUser;
		SecureZeroMemory(nUser, sizeof(CUser));
		memcpy(&nUser->xnaddr, pxna, sizeof(XNADDR));
		this->cusers[secure] = nUser;
		nUser->secure.s_addr = secure;
		nUser->bValid = true;
	}
	else 
	{
		// if he is in system update the XNADDR
		LOG_TRACE_NETWORK("CUserManagement::CreateUser() already present secure address {:x}, updating data", secure);
		memcpy(&nUser->xnaddr, pxna, sizeof(XNADDR)); // update XNADDR 
	}

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
	if (user == FALSE)
	{
		/* This happens when joining a server, it's a fix to dynamic ports... */

		short nPort_base = pxna->wPortOnline;
		short nPort_join = htons(ntohs(pxna->wPortOnline) + 1);

		LOG_TRACE_NETWORK("CreateUser - nPort_base: {}", ntohs(nPort_base));
		LOG_TRACE_NETWORK("CreateUser - nPort_join: {}", ntohs(nPort_join));

		std::pair <ULONG, SHORT> hostpair = std::make_pair(pxna->ina.s_addr, nPort_join);
		std::pair <ULONG, SHORT> hostpair_1000 = std::make_pair(pxna->ina.s_addr, nPort_base);

		if (H2Config_ip_wan == pxna->ina.s_addr)
		{
			std::pair <ULONG, SHORT> hostpair_or = std::make_pair(H2Config_ip_lan, nPort_join);
			std::pair <ULONG, SHORT> hostpair_1000_or = std::make_pair(H2Config_ip_lan, nPort_base);

			this->secure_map[hostpair_or] = secure;
			this->secure_map[hostpair_1000_or] = secure;
		}

		this->secure_map[hostpair] = secure;
		this->secure_map[hostpair_1000] = secure;

		this->pmap_a[secure] = nPort_base;
		this->pmap_b[secure] = nPort_join;
	}
	this->xnmap[secure] = pxna->ina.s_addr;
}

void CUserManagement::UnregisterSecureAddr(const IN_ADDR ina)
{
	CUser* to_remove_user = cusers[ina.s_addr];
	if (to_remove_user != nullptr)
	{
		auto xnmap_it = xnmap.find(ina.s_addr);
		if (xnmap_it != xnmap.end())
			xnmap.erase(xnmap_it);

		auto pmap_a_it = pmap_a.find(ina.s_addr);
		if (pmap_a_it != pmap_a.end())
			pmap_a.erase(pmap_a_it);

		auto pmap_b_it = pmap_b.find(ina.s_addr);
		if (pmap_b_it != pmap_b.end())
			pmap_a.erase(pmap_b_it);

		auto cusers_it = cusers.find(ina.s_addr);
		if (cusers_it != cusers.end())
			cusers.erase(cusers_it);

		for (auto it : secure_map)
		{
			if (it.second == ina.s_addr) {
				secure_map.erase(it.first);
				break;
			}
		}

		delete to_remove_user;
	}
	LOG_TRACE_NETWORK("[Resources-Clear] UnregisterSecureAddr executed on thread {:x}", GetCurrentThreadId());
}

void CUserManagement::UpdateConnectionStatus() {
	extern int MasterState;
	extern char* ServerStatus;
	if (this->LocalUserLoggedIn()) {
		MasterState = 10;
		if (!H2IsDediServer)
			snprintf(ServerStatus, 250, "Status: Online");
	}
	else
	{
		MasterState = 2;
		if (!H2IsDediServer)
			snprintf(ServerStatus, 250, "Status: Offline");
	}
}

BOOL CUserManagement::LocalUserLoggedIn() {
	return local_user.bValid;
}

void CUserManagement::UnregisterLocal()
{
	if (!local_user.bValid)
		return;

	local_user.bValid = false;
	this->UpdateConnectionStatus();
}

wchar_t ServerLobbyName[32] = { L"Cartographer" };

void SetUserUsername(char* username) {
	SecureZeroMemory(g_szUserName[0], 16);
	snprintf(g_szUserName[0], 16, username);
	if (!H2IsDediServer) {

		snprintf((char*)((BYTE*)H2BaseAddr + 0x971316), 16, username);
		swprintf((wchar_t*)((BYTE*)H2BaseAddr + 0x96DA94), 16, L"%hs", username);
		swprintf((wchar_t*)((BYTE*)H2BaseAddr + 0x51A638), 16, L"%hs", username);
		swprintf(ServerLobbyName, 16, L"%hs", username);
	}
}

void CUserManagement::ConfigureLocalUser(XNADDR* pxna, ULONGLONG xuid, char* username) {
	if (local_user.bValid) {
		local_user.bValid = false;
	}

	// copy local user info
	xFakeXuid[0] = xuid;
	SetUserUsername(username);

	SecureZeroMemory(&local_user, sizeof(CUser));
	memcpy(&local_user.xnaddr, pxna, sizeof(XNADDR));
	local_user.secure.s_addr = pxna->inaOnline.s_addr;
	SecureZeroMemory(&securePacket, sizeof(SecurePacket));

	// secure packet preparation
	securePacket.secure.s_addr = local_user.secure.s_addr;
	securePacket.annoyance_factor = annoyance_factor;
	memcpy(&securePacket.xn, &local_user.xnaddr, sizeof(XNADDR));

	local_user.bValid = true;
	this->UpdateConnectionStatus();

	//We want achievements loaded as early as possible, but we can't do it until after we have the XUID.
	std::thread(GetAchievements).detach();
}

BOOL CUserManagement::GetLocalXNAddr(XNADDR* pxna)
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

// #57: XNetXnAddrToInAddr
INT WINAPI XNetXnAddrToInAddr(const XNADDR *pxna, const XNKID *pnkid, IN_ADDR *pina)
{
	LOG_TRACE_NETWORK("XNetXnAddrToInAddr(): secure: {:x}", pxna->inaOnline.s_addr);
	LOG_TRACE_NETWORK("XNetXnAddrToInAddr(): ip-address: {:x}", pxna->ina.s_addr);

	LOG_TRACE_NETWORK("[Resources-Clear] XNetXnAddrToInAddr executed on thread {:x}", GetCurrentThreadId());

	if (pxna->inaOnline.s_addr != NULL) {
		CUser* user = userManager.cusers[pxna->inaOnline.s_addr];

		if (user == nullptr)
			userManager.CreateUser(pxna, TRUE);

		pina->s_addr = pxna->inaOnline.s_addr;
		return ERROR_SUCCESS;
	}
	return ERROR_FUNCTION_FAILED;
}

// #60: XNetInAddrToXnAddr
INT WINAPI XNetInAddrToXnAddr(const IN_ADDR ina, XNADDR * pxna, XNKID * pxnkid)
{
	LOG_TRACE_NETWORK("XNetInAddrToXnAddr() const ina: {:x}", ina.s_addr);
	CUser* user = userManager.cusers[ina.s_addr];
	if (user != nullptr)
	{
		if (pxna)
		{
			memcpy(pxna, &user->xnaddr, sizeof(XNADDR));
		}
		/*if (pxnkid)
		{
			XNetRandom((BYTE*)pxnkid, sizeof(XNKID));
		}*/
	}
	else
	{
		LOG_TRACE_NETWORK("XNetInAddrToXnAddr() the peer with secure/ip-address {:x} doesn't exist!", ina.s_addr);
	}

	return 0;
}

// #63: XNetUnregisterInAddr
int WINAPI XNetUnregisterInAddr(const IN_ADDR ina)
{
	LOG_TRACE_NETWORK("XNetUnregisterInAddr(): {:x}", ina.s_addr);
	// this doesn't seem to be a good idea for clearing the peer NAT and other information
	// because for whatever reason the game keeps sending connection requests even in game for a odd reason, maybe voice chat
	// and it calls this constantly
	//userManager.UnregisterSecureAddr(ina);
	return 0;
}
