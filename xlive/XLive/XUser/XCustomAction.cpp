#include "Globals.h"
#include "xlivedefs.h"
#include "xliveless.h"
#include "resource.h"
#include "XLive\xbox\xbox.h"
#include "XLive\XAM\xam.h"
#include "H2MOD\Discord\DiscordInterface.h"
#include "H2MOD\Modules\Config\Config.h"
#include <iostream>
#include <sstream>
#include <codecvt>
#include <unordered_map>

// #472
int WINAPI XCustomSetAction(DWORD dwActionIndex, LPCWSTR szActionText, DWORD dwFlags)
{
	TRACE("XCustomSetAction");
	return 0;
}

// #477
void WINAPI XCustomRegisterDynamicActions()
{
	TRACE("XCustomRegisterDynamicActions");
	return;
}

// #478
void WINAPI XCustomUnregisterDynamicActions()
{
	TRACE("XCustomUnregisterDynamicActions");
	return;
}

// #479
int WINAPI XCustomGetCurrentGamercard(DWORD* pdwUserIndex, XUID* pXuid)
{
	TRACE("XCustomGetCurrentGamercard");
	return 0;
}

// #474
int WINAPI XCustomSetDynamicActions(DWORD dwUserIndex, XUID xuid, CONST XCUSTOMACTION* pCustomActions, WORD cCustomActions)
{
	TRACE("XCustomSetDynamicActions");
	return 0;
}

// #473
int WINAPI XCustomGetLastActionPress(DWORD* pdwUserIndex, DWORD* pdwActionIndex, XUID *pXUID)
{
	//TRACE("XCustomGetLastActionPress");
	return 0;
}

// #476
BOOL WINAPI XCustomGetLastActionPressEx(DWORD* pdwUserIndex, DWORD* pdwActionId, XUID *pXuid, BYTE* pbPayload, WORD* pcbPayload)
{
	TRACE("XCustomGetLastActionPressEx");
	return FALSE;
}
