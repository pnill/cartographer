#include "CUser.h"
#include "stdafx.h"
#include "XLive\UserManagement\protobuf\packet.pb.h"
#include <time.h>
#include <sstream>
#include "H2MOD\Modules\Config\Config.h"
#include "H2MOD\Modules\OnScreenDebug\OnScreenDebug.h"
#include "H2MOD\Modules\Startup\Startup.h"
#include "H2MOD\Modules\Achievements\Achievements.h"

CUser Users[16];
CUserManagement userManager;

extern XUID xFakeXuid[4];
extern CHAR g_szUserName[4][16];

void CUserManagement::CreateUser(const XNADDR* pxna)
{
	TRACE_GAME_NETWORK_N("CUserManagement::CreateUser() ip address: %08X", pxna->ina.s_addr);
	if (iplong_to_user[pxna->ina.s_addr] == nullptr) 
	{
		CUser *nUser = new CUser;
		ULONG ipaddr = pxna->ina.s_addr;
		memset(&nUser->xnaddr, 0x00, sizeof(XNADDR));
		memcpy(&nUser->xnaddr, pxna, sizeof(XNADDR));
		this->iplong_to_user[ipaddr] = nUser;
		nUser->bValid = true;
		return;
	}
	TRACE_GAME_NETWORK_N("CUserManagement::CreateUser() user with ip address %08X already exists!", pxna->ina.s_addr);
}

// TODO: clear from time to time the array (maybe inside GameManager thread)
void CUserManagement::UnregisterSecureAddr(const IN_ADDR ina)
{
	//if (iplong_to_user[ina.s_addr] != 0)
	//		iplong_to_user[ina.s_addr]->bValid = false;
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
	return Users[0].bValid;
}

void CUserManagement::UnregisterLocal()
{
	if (!Users[0].bValid)
		return;

	Users[0].bValid = false;
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
	if (Users[0].bValid) {
		Users[0].bValid = false;
	}

	xFakeXuid[0] = xuid;
	memcpy(&Users[0].xnaddr, pxna, sizeof(XNADDR));

	SetUserUsername(username);

	Users[0].bValid = true;

	this->UpdateConnectionStatus();

	//We want achievements loaded as early as possible, but we can't do it until after we have the XUID.
	std::thread(GetAchievements).detach();
}

BOOL CUserManagement::GetLocalXNAddr(XNADDR* pxna)
{
	if (Users[0].bValid)
	{
		memcpy(pxna, &Users[0].xnaddr, sizeof(XNADDR));
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

	CUser* user = userManager.iplong_to_user[pxna->ina.s_addr];

	if (user == nullptr)
		userManager.CreateUser(pxna);

	pina->s_addr = pxna->ina.s_addr;

	return 0;
}

// #60: XNetInAddrToXnAddr
INT WINAPI XNetInAddrToXnAddr(const IN_ADDR ina, XNADDR * pxna, XNKID * pxnkid)
{
	TRACE_GAME_NETWORK_N("XNetInAddrToXnAddr() const ina: %08X", ina.s_addr);
	CUser* user = userManager.iplong_to_user[ina.s_addr];
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

	return 0;
}

// #63: XNetUnregisterInAddr
int WINAPI XNetUnregisterInAddr(const IN_ADDR ina)
{
	TRACE_GAME_NETWORK_N("XNetUnregisterInAddr(): %08X", ina.s_addr);
	userManager.UnregisterSecureAddr(ina);
	return 0;
}