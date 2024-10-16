#include "stdafx.h"
#include "XCustomAction.h"

#include "XLive/xbox/xbox.h"

// #472
int WINAPI XCustomSetAction(DWORD dwActionIndex, LPCWSTR szActionText, DWORD dwFlags)
{
	LOG_TRACE_XLIVE("XCustomSetAction");
	return 0;
}

// #477
void WINAPI XCustomRegisterDynamicActions()
{
	LOG_TRACE_XLIVE("XCustomRegisterDynamicActions");
	return;
}

// #478
void WINAPI XCustomUnregisterDynamicActions()
{
	LOG_TRACE_XLIVE("XCustomUnregisterDynamicActions");
	return;
}

// #479
int WINAPI XCustomGetCurrentGamercard(DWORD* pdwUserIndex, XUID* pXuid)
{
	LOG_TRACE_XLIVE("XCustomGetCurrentGamercard");
	return 0;
}

// #474
int WINAPI XCustomSetDynamicActions(DWORD dwUserIndex, XUID xuid, CONST XCUSTOMACTION* pCustomActions, WORD cCustomActions)
{
	LOG_TRACE_XLIVE("XCustomSetDynamicActions");
	return 0;
}

// #473
BOOL WINAPI XCustomGetLastActionPress(DWORD* pdwUserIndex, DWORD* pdwActionIndex, XUID *pXUID)
{
	//LOG_TRACE_XLIVE("XCustomGetLastActionPress");
	return FALSE;
}

// #476
BOOL WINAPI XCustomGetLastActionPressEx(DWORD* pdwUserIndex, DWORD* pdwActionId, XUID *pXuid, BYTE* pbPayload, WORD* pcbPayload)
{
	LOG_TRACE_XLIVE("XCustomGetLastActionPressEx");
	return FALSE;
}
