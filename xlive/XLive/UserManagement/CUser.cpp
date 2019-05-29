#include "stdafx.h"
#include "CUser.h"

#include <time.h>
#include <sstream>
#include "H2MOD\Modules\Config\Config.h"
#include "H2MOD\Modules\OnScreenDebug\OnScreenDebug.h"
#include "H2MOD\Modules\Startup\Startup.h"
#include "H2MOD\Modules\Achievements\Achievements.h"
#include "XLive\UserManagement\protobuf\packet.pb.h"
#include "H2MOD\Modules\Networking\NetworkSession\NetworkSession.h"

extern XUID xFakeXuid[4];
extern CHAR g_szUserName[4][16];

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

		int ret = sendto(s, userManager.secure_packet, 12 + sizeof(XNADDR), NULL, (SOCKADDR*)&sendToAddr, sizeof(sendToAddr));
		TRACE_GAME_NETWORK_N("[H2MOD-Network] secure packet sent, return code: %d", ret);
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
	TRACE_GAME_NETWORK_N("CUserManagement::CreateUser() secure address %08X", pxna->inaOnline.s_addr);

	ULONG secure = pxna->inaOnline.s_addr;
	CUser *nUser = new CUser;
	memset(&nUser->xnaddr, 0x00, sizeof(XNADDR));
	memcpy(&nUser->xnaddr, pxna, sizeof(XNADDR));
	nUser->secure.s_addr = secure;

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

		TRACE_GAME_NETWORK_N("CreateUser - nPort_base: %i", ntohs(nPort_base));
		TRACE_GAME_NETWORK_N("CreateUser - nPort_join: %i", ntohs(nPort_join));

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
		this->cusers[secure] = nUser;
		this->xnmap[secure] = pxna->ina.s_addr;

		this->pmap_a[secure] = nPort_base;
		this->pmap_b[secure] = nPort_join;
	}
	else
	{
		/*
			If the user variable is true, we update cusers "array" to map the secure address to the newly created object...
			Then map the secure address to the WAN IP (XN->ina) using xnmap, where secure address is the key to the array.
			Then map the abenet to the secure address via xntosecure[ab], where AB is the key to the array.
		*/
		this->cusers[secure] = nUser;
		this->xnmap[secure] = pxna->ina.s_addr;
	}

	nUser->bValid = true;
}

// FIXME
void CUserManagement::UnregisterSecureAddr(const IN_ADDR ina)
{
	CUser* to_remove_user = cusers[ina.s_addr];
	if (to_remove_user != nullptr)
	{
		std::pair<ULONG, short> smap, smap1;
		if (to_remove_user->xnaddr.ina.s_addr == H2Config_ip_wan)
		{
			smap = std::make_pair(H2Config_ip_lan, to_remove_user->xnaddr.wPortOnline);
			smap1 = std::make_pair(H2Config_ip_lan, ntohs(htons(to_remove_user->xnaddr.wPortOnline) + 1));
		}
		else
		{
			smap = std::make_pair(to_remove_user->xnaddr.ina.s_addr, to_remove_user->xnaddr.wPortOnline);
			smap1 = std::make_pair(to_remove_user->xnaddr.ina.s_addr, ntohs(htons(to_remove_user->xnaddr.wPortOnline) + 1));
		}
		auto sec_map = secure_map.find(smap);
		auto sec_map1 = secure_map.find(smap1);
		secure_map.erase(sec_map);
		secure_map.erase(sec_map1);
		auto it = cusers.find(ina.s_addr);
		cusers.erase(it);
		delete to_remove_user;
	}
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

	delete[] this->secure_packet;
	this->secure_packet = nullptr;
	local_user.bValid = false;
	this->UpdateConnectionStatus();
}

wchar_t ServerLobbyName[32] = { L"Cartographer" };

void SetUserUsername(char* username) {
	ZeroMemory(g_szUserName[0], 16);
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
		delete[] this->secure_packet;

		local_user.bValid = false;
	}

	// copy local user info
	xFakeXuid[0] = xuid;
	SetUserUsername(username);

	memset(&local_user, NULL, sizeof(CUser));
	memcpy(&local_user.xnaddr, pxna, sizeof(XNADDR));
	local_user.secure.s_addr = pxna->inaOnline.s_addr;
	this->secure_packet = new char[12 + sizeof(XNADDR)];
	memset(secure_packet, NULL, 12 + sizeof(XNADDR));

	// secure packet preparation
	*(DWORD*)&this->secure_packet[0] = annoyance_factor;
	*(ULONG*)&this->secure_packet[4] = local_user.secure.s_addr;
	memcpy(&this->secure_packet[8], &local_user.xnaddr, sizeof(XNADDR));

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
		TRACE_GAME_NETWORK_N("GetLocalXNAddr(): XNADDR: %08X", pxna->ina.s_addr);
		return TRUE;
	}
	//TRACE_GAME_NETWORK_N("GetLocalXNADDR(): Local user network information not populated yet.");

	return FALSE;
}

// #57: XNetXnAddrToInAddr
INT WINAPI XNetXnAddrToInAddr(const XNADDR *pxna, const XNKID *pnkid, IN_ADDR *pina)
{
	TRACE_GAME_NETWORK_N("XNetXNAddrToInAddr(): secure: %08X", pxna->inaOnline.s_addr);
	TRACE_GAME_NETWORK_N("XNetXnAddrToInAddr(): ina.s_addr: %08X", pxna->ina.s_addr);

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
	TRACE_GAME_NETWORK_N("XNetInAddrToXnAddr() const ina: %08X", ina.s_addr);
	CUser* user = userManager.cusers[ina.s_addr];
	if (user != nullptr)
	{
		if (pxna)
		{
			memset(pxna, 0x00, sizeof(XNADDR)); // Zero memory of the current buffer passed to us by the game.
			memcpy(pxna, &user->xnaddr, sizeof(XNADDR));
		}
		/*if (pxnkid)
		{
			XNetRandom((BYTE*)pxnkid, sizeof(XNKID));
		}*/
	}
	else
	{
		TRACE_GAME_NETWORK_N("XNetInAddrToXnAddr() the peer with secure/ipaddress %08X doesn't exist!", ina.s_addr);
	}

	return 0;
}

// #63: XNetUnregisterInAddr
int WINAPI XNetUnregisterInAddr(const IN_ADDR ina)
{
	TRACE_GAME_NETWORK_N("XNetUnregisterInAddr(): %08X", ina.s_addr);
	//userManager.UnregisterSecureAddr(ina);
	return 0;
}