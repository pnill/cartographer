#include "stdafx.h"
#include "XUserProperty.h"

#include "Globals.h"
#include "resource.h"
#include "XLive\xbox\xbox.h"
#include "XLive\XAM\xam.h"
#include "H2MOD\Discord\DiscordInterface.h"
#include "H2MOD\Modules\Config\Config.h"
#include <iostream>
#include <sstream>
#include <codecvt>
#include <unordered_map>

extern void Check_Overlapped(PXOVERLAPPED pOverlapped);

// 5288: XUserGetProperty
DWORD WINAPI XUserGetProperty(DWORD dwUserIndex, DWORD* pcbActual, XUSER_PROPERTY* pProperty, PXOVERLAPPED pOverlapped)
{
	return S_OK;
}

// #5276: XUserSetProperty
void WINAPI XUserSetProperty(DWORD dwUserIndex, DWORD dwPropertyId, DWORD cbValue, CONST VOID *pvValue)
{
	LOG_TRACE_XLIVE("XUserSetProperty");
	return;
}

// #5293: XUserSetPropertyEx
int WINAPI XUserSetPropertyEx(DWORD dwUserIndex, DWORD dwPropertyId, DWORD cbValue, void * pvValue, PXOVERLAPPED pOverlapped)
{
	LOG_TRACE_XLIVE("XUserSetPropertyEx  (userIndex = {0}, propertyId = {1:x}, cbValue = {2}, pvValue = {3:p}, pOverlapped = {4:p})",
		dwUserIndex, dwPropertyId, cbValue, (void*)pvValue, (void*)pOverlapped);


	if (pOverlapped == 0)
		return ERROR_SUCCESS;

	else
	{
		pOverlapped->InternalHigh = 0;
		pOverlapped->InternalLow = ERROR_SUCCESS;
		pOverlapped->dwExtendedError = ERROR_SUCCESS;


		Check_Overlapped(pOverlapped);

		return ERROR_IO_PENDING;
	}
}
