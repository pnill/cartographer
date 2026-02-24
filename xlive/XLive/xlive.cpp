#include "stdafx.h"
#include "xlive.h"

#include "H2MOD/Modules/Accounts/AccountLogin.h"

// 5257: XLiveManageCredentials
HRESULT WINAPI XLiveManageCredentials(LPCWSTR lpszLiveIdName, LPCWSTR lpszLiveIdPassword, DWORD dwCredFlags, PXOVERLAPPED pXOverlapped)
{
	LOG_TRACE_XLIVE(L"XLiveManageCredentials (lpszLiveIdName = {}, lpszLiveIdPassword = {}, dwCredFlags = {:#x}, pXOverlapped = {:p})",
		lpszLiveIdName, lpszLiveIdPassword, dwCredFlags, (void*)pXOverlapped);

	if (pXOverlapped)
	{
		pXOverlapped->InternalLow = ERROR_SUCCESS;
		pXOverlapped->InternalHigh = 0;
		pXOverlapped->dwExtendedError = 0;
	}

	// not done - error now
	return S_OK;
}

// #5259: XLiveSignin
HRESULT WINAPI XLiveSignin(PWSTR pszLiveIdName, PWSTR pszLiveIdPassword, DWORD dwFlags, PXOVERLAPPED pOverlapped)
{
	LOG_TRACE_XLIVE("XLiveSignin() - signin in");

	AccountDediLogin(pOverlapped);

	return S_OK;
}

// #5258: XLiveSignout
HRESULT WINAPI XLiveSignout(PXOVERLAPPED pXOverlapped)
{
	LOG_TRACE_XLIVE("XLiveSignout");

	XUserSignOut(0);

	if (pXOverlapped)
	{
		pXOverlapped->InternalLow = ERROR_SUCCESS;
		pXOverlapped->InternalHigh = 0;
		pXOverlapped->dwExtendedError = S_OK;
	}

	return S_OK;
}