#include "stdafx.h"
#include "XUserProperty.h"

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
	return ERROR_SUCCESS;
}
