#include "stdafx.h"
#include "XUserContext.h"

#include "cartographer/discord/discord_interface.h"


extern XSESSION_LOCAL_DETAILS sessionDetails;

// 5289: XUserGetContext
DWORD WINAPI XUserGetContext(DWORD dwUserIndex, XUSER_CONTEXT* pContext, PXOVERLAPPED  pOverlapped)
{
	return S_OK;
}

// #5277: XUserSetContext
DWORD WINAPI XUserSetContext(DWORD dwUserIndex, DWORD dwContextId, DWORD dwContextValue)
{
	LOG_TRACE_XLIVE("XUserSetContext  (userIndex = {0}, contextId = {1}, contextValue = {2})", dwUserIndex, dwContextId, dwContextValue);
	discord_interface_set_context(dwContextId, dwContextValue);
	return ERROR_SUCCESS;
}


// #5292: XUserSetContextEx
int WINAPI XUserSetContextEx(DWORD dwUserIndex, DWORD dwContextId, DWORD dwContextValue, PXOVERLAPPED pOverlapped)
{
	LOG_TRACE_XLIVE("XUserSetContextEx  (userIndex = {0}, contextId = {1}, contextValue = {2:x}, pOverlapped = {3:p})", dwUserIndex, dwContextId, dwContextValue, (void*)pOverlapped);
	return ERROR_SUCCESS;
}
