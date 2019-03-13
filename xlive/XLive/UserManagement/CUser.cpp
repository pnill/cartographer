#include "CUser.h"
#include "stdafx.h"
#include <time.h>
#include <sstream>
#include "XLive\UserManagement\protobuf\packet.pb.h"
#include "H2MOD\Modules\Config\Config.h"
#include "H2MOD\Modules\OnScreenDebug\OnScreenDebug.h"
#include "H2MOD\Modules\Startup\Startup.h"
#include "H2MOD\Modules\Achievements\Achievements.h"

extern XUID xFakeXuid[4];
extern CHAR g_szUserName[4][16];

bool using_secure = true;
CUserManagement userManager;

const DWORD annoyance_factor = 0x11223344;

/*
NOTE:
	Check inside Tweaks.cpp for removeXNetSecurity
*/

extern SOCKET game_network_message_gateway_socket;
signed int CUserManagement::sendSecurePacket()
{
	if (using_secure)
	{
		SOCKADDR_IN sendToAddr;
		short port = userManager.game_host_xn.wPortOnline;
		if (userManager.game_host_xn.ina.s_addr != H2Config_ip_wan)
			sendToAddr.sin_addr.s_addr = userManager.game_host_xn.ina.s_addr;
		else
			sendToAddr.sin_addr.s_addr = H2Config_ip_lan;

		sendToAddr.sin_port = port;
		sendToAddr.sin_family = AF_INET;

		int ret = sendto(game_network_message_gateway_socket, userManager.secure_packet, 12 + sizeof(XNADDR), NULL, (SOCKADDR*)&sendToAddr, sizeof(sendToAddr));
		TRACE_GAME_NETWORK_N("[H2MOD-Network] secure packet sent, return code: %d", ret);

		return ret;
	}
	return 0;
}

void CUserManagement::CreateUser(const XNADDR* pxna)
{
	TRACE_GAME_NETWORK_N("CUserManagement::CreateUser() ip/secure address: %08X", using_secure ? pxna->inaOnline.s_addr : pxna->ina.s_addr);
	if (address_to_user[using_secure ? pxna->inaOnline.s_addr : pxna->ina.s_addr] == nullptr)
	{
		CUser *nUser = new CUser;
		ULONG addr = using_secure ? pxna->inaOnline.s_addr : pxna->ina.s_addr;
		memset(&nUser->xnaddr, 0x00, sizeof(XNADDR));
		memcpy(&nUser->xnaddr, pxna, sizeof(XNADDR));
		this->address_to_user[addr] = nUser;
		nUser->bValid = true;
	}
	else
	{
		CUser* user = address_to_user[using_secure ? pxna->inaOnline.s_addr : pxna->ina.s_addr];
		memcpy(&user->xnaddr, pxna, sizeof(XNADDR));
	    TRACE_GAME_NETWORK_N("CUserManagement::CreateUser() user with ip/secure address %08X already exists! updating current data.", using_secure ? pxna->inaOnline.s_addr : pxna->ina.s_addr);
	}

	if (using_secure) 
	{
		ULONG addr = using_secure ? pxna->inaOnline.s_addr : pxna->ina.s_addr;
		if (H2Config_ip_wan == pxna->ina.s_addr)
		{
			std::pair <ULONG, SHORT> hostpair_or = std::make_pair(H2Config_ip_lan, pxna->wPortOnline);
			std::pair <ULONG, SHORT> hostpair_1000_or = std::make_pair(H2Config_ip_lan, ntohs(htons(pxna->wPortOnline) + 1));
			this->secure_map[hostpair_or] = addr;
			this->secure_map[hostpair_1000_or] = addr;
		}
		else
		{
			std::pair <ULONG, SHORT> hostpair = std::make_pair(pxna->ina.s_addr, pxna->wPortOnline);
			std::pair <ULONG, SHORT> hostpair_1000 = std::make_pair(pxna->ina.s_addr, ntohs(htons(pxna->wPortOnline) + 1));
			this->secure_map[hostpair] = addr;
			this->secure_map[hostpair_1000] = addr;
		}
	}
}

void CUserManagement::UnregisterSecureAddr(const IN_ADDR ina)
{
	if (using_secure) 
	{
		CUser* to_remove_user = address_to_user[ina.s_addr];
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
			delete to_remove_user;
			auto it = address_to_user.find(ina.s_addr);
			address_to_user.erase(it);
		}
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
		if (using_secure)
			delete[] this->secure_packet;

		local_user.bValid = false;
	}

	xFakeXuid[0] = xuid;
	SetUserUsername(username);
	memset(&local_user, NULL, sizeof(CUser));
	memcpy(&local_user.xnaddr, pxna, sizeof(XNADDR));

	if (using_secure) {
		this->secure_packet = new char[12 + sizeof(XNADDR)];
		memset(secure_packet, NULL, 12 + sizeof(XNADDR));
		*(DWORD*)&this->secure_packet[0] = annoyance_factor;
		memcpy(&this->secure_packet[4], &local_user.xnaddr, sizeof(XNADDR));
	}

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

	CUser* user = userManager.address_to_user[using_secure ? pxna->inaOnline.s_addr : pxna->ina.s_addr];

	if (user == nullptr)
		userManager.CreateUser(pxna);

	pina->s_addr = using_secure ? pxna->inaOnline.s_addr : pxna->ina.s_addr;

	return 0;
}

// #60: XNetInAddrToXnAddr
INT WINAPI XNetInAddrToXnAddr(const IN_ADDR ina, XNADDR * pxna, XNKID * pxnkid)
{
	TRACE_GAME_NETWORK_N("XNetInAddrToXnAddr() const ina: %08X", ina.s_addr);
	CUser* user = userManager.address_to_user[ina.s_addr];
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
	userManager.UnregisterSecureAddr(ina);
	return 0;
}