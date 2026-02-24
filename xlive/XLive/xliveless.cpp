// -No Copyright- 2010 Stanislav "listener" Golovin
// This file donated to the public domain

#include "stdafx.h"

#include "XLive/ServerList/ServerList.h"
#include "XLive/achievements/XAchievements.h"

#include "interface/user_interface_guide.h"

HANDLE g_dwFakeContent = INVALID_HANDLE_VALUE;
HANDLE g_dwMarketplaceContent = INVALID_HANDLE_VALUE;
HANDLE g_dwFakeFriendsEnumerator = INVALID_HANDLE_VALUE;

// TODO FIXME: add proper XHandle handling
// when closing the handles, we need to make sure we set the handles to INVALID_HANDLE_VALUE
// otherwise it'll interfere with XLocator Enumeration handles, causing serverlist enumeration to fail
// because closed handles could be reused for server enumeration
HANDLE* g_dwEnumeratorHandleObjects[] =
{
	&g_dwFakeContent,
	&g_dwMarketplaceContent,
	&g_dwFakeAchievementContent,
	&g_dwFakeFriendsEnumerator
};

void Check_Overlapped(PXOVERLAPPED pOverlapped)
{
	if( !pOverlapped ) return;

	if( pOverlapped->hEvent )
	{
		//LOG_TRACE_XLIVE( "- hEvent = %X", pOverlapped->hEvent );

		SetEvent( pOverlapped->hEvent );
	}


	if( pOverlapped->pCompletionRoutine )
	{
		//LOG_TRACE_XLIVE( "- pCompletionRoutine = %X", pOverlapped->pCompletionRoutine );


		pOverlapped->pCompletionRoutine( pOverlapped->InternalLow, pOverlapped->InternalHigh, pOverlapped->dwCompletionContext );
	}
}


// #############################################################
// #############################################################
// #############################################################
// #############################################################
// #############################################################

// === Start of xlive functions ===


// #1082: XGetOverlappedExtendedError
DWORD WINAPI XGetOverlappedExtendedError(PXOVERLAPPED pOverlapped)
{
	if (pOverlapped == nullptr)
	{
		return GetLastError();
	}

	if (pOverlapped->InternalLow != ERROR_IO_PENDING)
		return pOverlapped->dwExtendedError;

	return ERROR_IO_INCOMPLETE;
}


// #1083: XGetOverlappedResult
DWORD WINAPI XGetOverlappedResult(PXOVERLAPPED pOverlapped, LPDWORD pResult, BOOL bWait)
{
	//LOG_TRACE_XLIVE("XGetOverlappedResult  (bWait = %d)  (internalLow = %X, internalHigh = %X)",
	// bWait, pOverlapped->InternalLow, pOverlapped->InternalHigh );

	if( pResult )
	{
		*pResult = pOverlapped->InternalHigh;

		//LOG_TRACE_XLIVE( "- result = %d", *pResult );
	}


	//Check_Overlapped( pOverlapped );


	//LOG_TRACE_XLIVE( "- code = %X", pOverlapped->InternalLow );
	return pOverlapped->InternalLow;
}

// #5010: XLiveLess_5010
int WINAPI XLiveRegisterDataSection(int a1, int a2, int a3)
{
	LOG_TRACE_XLIVE("XLiveRegisterDataSection - a1: {0:x},a2: {1:x}, a3: {2:x}", a1, a2, a3);
	return 0;
}

// #5011 XLiveLess_5011
int WINAPI XLiveUnregisterDataSection(int a1)
{
	LOG_TRACE_XLIVE("XLiveUnregisterDataSection - a1: {:x}", a1);
	return 0;
}

// #5012 XLiveLess_5012
int WINAPI XLiveUpdateHashes(int a1, int a2)
{
	return 0;
}

// #5016: XLivePBufferAllocate
LONG WINAPI XLivePBufferAllocate (DWORD size, FakePBuffer **pBuffer)
{
	if(!pBuffer)
		return E_OUTOFMEMORY;

	HANDLE hHeap = GetProcessHeap();

	//initialize fake buffer
	*pBuffer = (FakePBuffer*)HeapAlloc(hHeap,HEAP_ZERO_MEMORY,sizeof(FakePBuffer));

	if(!*pBuffer)
	{
		LOG_ERROR_XLIVE("XLivePBufferAllocate() unable to allocate {} bytes", size);
		return E_OUTOFMEMORY;
	}

	(*pBuffer)->dwSize = size;
	(*pBuffer)->id = CreateMutex(NULL, NULL, NULL);
	(*pBuffer)->magic = 0xDEADC0DE;

	//initialize real buffer inside fake buffer
	(*pBuffer)->pbData = (PBYTE)HeapAlloc(hHeap, HEAP_ZERO_MEMORY, size);

	return 0;
}


// #5017: XLivePBufferFree
DWORD WINAPI XLivePBufferFree(FakePBuffer * pBuffer)
{
	return 0;
}


// #5022: XLiveGetUpdateInformation
HRESULT  WINAPI XLiveGetUpdateInformation (DWORD)
{
	LOG_TRACE_XLIVE("XLiveGetUpdateInformation");
	return S_FALSE; // no update
}


// #5024: XLiveUpdateSystem
HRESULT  WINAPI XLiveUpdateSystem (DWORD)
{
	LOG_TRACE_XLIVE("XLiveUpdateSystem");
	return S_FALSE; // no update
}


// #5031 XLiveSetDebugLevel
int WINAPI XLiveSetDebugLevel (DWORD xdlLevel, DWORD * pxdlOldLevel)
{
	LOG_TRACE_XLIVE("XLiveSetDebugLevel ({})", xdlLevel);
	return 0;
}


// #5251: XCloseHandle
BOOL WINAPI XCloseHandle (HANDLE hObject)
{
	//LOG_TRACE_XLIVE("XCloseHandle  (hObject = %X)", hObject );
	BOOL ret = 0;

	for (auto pHandle : g_dwEnumeratorHandleObjects)
	{
		if (*pHandle == hObject)
			*pHandle = INVALID_HANDLE_VALUE;
	}

	if(hObject) ret = CloseHandle(hObject);

	return ret;
}


// #5254: XCancelOverlapped
int WINAPI XCancelOverlapped (PXOVERLAPPED pOverlapped)
{
	LOG_TRACE_XLIVE("XCancelOverlapped  (pOverlapped = {:p})", (void*)pOverlapped);

	if (pOverlapped == NULL)
		return ERROR_INVALID_PARAMETER;

	return ERROR_SUCCESS;
}

// #5256: XEnumerate
int WINAPI XEnumerate(HANDLE hEnum, CHAR *pvBuffer, DWORD cbBuffer, PDWORD pcItemsReturned, PXOVERLAPPED pOverlapped)
{
	LOG_TRACE_XLIVE("XEnumerate (hEnum: {}, cbBuffer: {})", hEnum, cbBuffer);

	BOOL async;

	if( pOverlapped != 0 )
	{
		async = TRUE;

		// no items
		pOverlapped->InternalHigh = 0;
	}
	else if( pcItemsReturned )
	{
		async = FALSE;

		// no items
		*pcItemsReturned = 0;
	}
	else
	{
		LOG_TRACE_XLIVE( "- NULL ptr" );
		return ERROR_INVALID_PARAMETER;
	}

	if ( hEnum == g_dwFakeAchievementContent )
	{
		return AchievementEnumerator(cbBuffer, pvBuffer, pcItemsReturned, pOverlapped);
	}
	else 
	{
		// fall back to CServerList::Enumerate if previous handles didn't match other operations
		// it'll return ERROR_INVALID_HANDLE if it doesn't match this operation as well
		return CServerList::Enumerate(hEnum, cbBuffer, pvBuffer, pOverlapped);
	}

	/* UNREACHABLE
	if (async == FALSE)
	{
		if( *pcItemsReturned )
		{
			LIMITED_LOG(100, LOG_TRACE_XLIVE, "- ItemsReturned = {}", *pcItemsReturned);

			return ERROR_SUCCESS;
		}

		else
		{
			LIMITED_LOG(100, LOG_TRACE_XLIVE, "- NO_MORE_FILES");
			*pcItemsReturned = 0;
			return ERROR_SUCCESS;
		}
	}
	else
	{
		if( pOverlapped->InternalHigh )
		{
			LIMITED_LOG(100, LOG_TRACE_XLIVE, "- async items = {}", pOverlapped->InternalHigh);


			pOverlapped->InternalLow = ERROR_SUCCESS;
			pOverlapped->dwExtendedError = ERROR_SUCCESS;


			Check_Overlapped( pOverlapped );


			return ERROR_IO_PENDING;
		}

		else
		{
			LIMITED_LOG(100, LOG_TRACE_XLIVE, "- async = NO_MORE_FILES" );

			pOverlapped->InternalLow = ERROR_NO_MORE_FILES;
			pOverlapped->dwExtendedError = ERROR_NO_MORE_FILES;


			Check_Overlapped( pOverlapped );


			return ERROR_IO_PENDING;
		}
	}
	*/
}

// #5303: XStringVerify
DWORD WINAPI XStringVerify(DWORD dwFlags, const CHAR *szLocale, DWORD dwNumStrings, const STRING_DATA *pStringData, DWORD cbResults, STRING_VERIFY_RESPONSE *pResults, PXOVERLAPPED pOverlapped)
{
	LOG_TRACE_XLIVE("XStringVerify  (dwFlags = {0:x}, szLocale = {1}, dwNumStrings = {2}, pStringData = {3:p}, cbresults = {4}, pResults = {5:p}, pXOverlapped = {6:p})",
		dwFlags, szLocale, dwNumStrings, (void*)pStringData, cbResults, (void*)pResults, (void*)pOverlapped );



	if( pResults )
	{
		pResults->wNumStrings = (WORD)dwNumStrings;
		pResults->pStringResult = (HRESULT *) ((BYTE *) pResults + sizeof(STRING_VERIFY_RESPONSE));


		for( DWORD lcv = 0; lcv < dwNumStrings; lcv++ )
			pResults->pStringResult[lcv] = (HRESULT) S_OK;
	}


	if( pOverlapped )
	{
		pOverlapped->InternalLow = ERROR_SUCCESS;

		pOverlapped->dwExtendedError = 0;


		Check_Overlapped( pOverlapped );

		return ERROR_IO_PENDING;
	}


	return ERROR_SUCCESS;
}

// #5311: XOnlineCleanup
int WINAPI XOnlineCleanup()
{
	return 0;
}


// #5312: XFriendsCreateEnumerator
DWORD WINAPI XFriendsCreateEnumerator (DWORD dwUserIndex, DWORD dwStartingIndex, DWORD dwFriendstoReturn, DWORD *pcbBuffer, HANDLE * phEnum)
{
	LOG_TRACE_XLIVE("XFriendsCreateEnumerator");

	if (pcbBuffer) *pcbBuffer = dwFriendstoReturn * sizeof(XCONTENT_DATA);
	if (phEnum)
	{
		*phEnum = g_dwFakeFriendsEnumerator = CreateMutex(NULL, NULL, NULL);

		LOG_TRACE_XLIVE("- Handle = {:p}", (void*)*phEnum);
	}

	return ERROR_NO_MORE_FILES;
}


// #5313: XPresenceInitialize
int WINAPI XPresenceInitialize(int a1)
{
	LOG_TRACE_XLIVE("XPresenceInitialize");
	return 0;
}


// #5314: XUserMuteListQuery
int WINAPI XUserMuteListQuery(DWORD dwUserIndex, XUID XuidRemoteTalker, BOOL *pfOnMuteList)
{
	*pfOnMuteList = FALSE;
	//LOG_TRACE_XLIVE("XUserMuteListQuery");
	return 0;
}


// #5315: XInviteGetAcceptedInfo
int WINAPI XInviteGetAcceptedInfo(DWORD dwUserIndex, XINVITE_INFO* pInfo)
{
	LOG_TRACE_XLIVE("XInviteGetAcceptedInfo");
	pInfo->hostInfo = user_interface_guide_state_manager_get()->m_xsession_info;
	pInfo->fFromGameInvite = true;
	return 1;
}


// #5316: XInviteSend
int WINAPI XInviteSend(DWORD, DWORD, DWORD, DWORD, DWORD)
{
	return 0;
}


// #5324: XOnlineGetNatType
XONLINE_NAT_TYPE WINAPI XOnlineGetNatType()
{
	return XONLINE_NAT_OPEN;
}


// #5335: XTitleServerCreateEnumerator
DWORD WINAPI XTitleServerCreateEnumerator(LPCSTR pszServerInfo, DWORD cItem, DWORD * pcbBuffer, PHANDLE phEnum)
{
	return 1;
}

// #5338: XPresenceSubscribe
int WINAPI XPresenceSubscribe(int a1, int a2, int a3)
{
	return 0;
}

// #5340 XPresenceCreateEnumerator
int WINAPI XPresenceCreateEnumerator(DWORD dwUserIndex, DWORD cPeers, const XUID *pPeers, DWORD dwStartingIndex, DWORD dwPeersToReturn, DWORD *pcbBuffer, HANDLE *ph)
{
	return 1;
}


// #5346: XLiveLess_5346
DWORD WINAPI TitleExport_XUserEstimateRankForRating(DWORD a1, LPDWORD pdwResult, DWORD a3, void* a4, PXOVERLAPPED pOverlapped)
{
	return 0;
}


// #5349: XLiveProtectedVerifyFile
DWORD WINAPI XLiveProtectedVerifyFile(HANDLE hContentAccess, VOID * pvReserved, PCWSTR pszFilePath)
{
	return 0;
}


// #5350: XLiveContentCreateAccessHandle
DWORD WINAPI XLiveContentCreateAccessHandle(DWORD dwTitleId, PXCONTENT_DATA pContentInfo, DWORD dwLicenseInfoVersion, FakePBuffer *xebBuffer, DWORD dwOffset, HANDLE * phAccess, PXOVERLAPPED pOverlapped)
{
	return 0;
}


// #5352: XLiveContentUninstall
DWORD WINAPI XLiveContentUninstall (void * pContentInfo, XUID * pxuidFor, void * pInstallCallbackParams)
{
	return 0;
}


// #5355: XLiveContentGetPath
LONG WINAPI XLiveContentGetPath (DWORD dwUserIndex, PXCONTENT_DATA pContentData, wchar_t * pszPath, DWORD * pcchPath)
{
	return 0;
}


// #5356: XContentCreatePackage
DWORD WINAPI XContentCreatePackage(DWORD dwUserIndex, PXCONTENT_DATA pContentData, WCHAR *pszPath, DWORD *pcchPath)
{
	return 0;
}


// #5360: XContentCreateEnumerator
DWORD WINAPI XContentCreateEnumerator(DWORD MaxEnumerator, PDWORD a2, PDWORD pchBuffer, PHANDLE phEnum)
{
	return 0;
}


// #5361: XContentRetrieveOffersByDate
DWORD WINAPI XContentRetrieveOffersByDate (DWORD dwUserIndex, DWORD dwOffserInfoVersion,
		SYSTEMTIME * pstStartDate, void * pOffserInfoArray, DWORD * pcOfferInfo, PXOVERLAPPED pOverlapped)
{
	return 0;
}


// #5295: XLivePBufferSetByteArray
DWORD WINAPI XLivePBufferSetByteArray(FakePBuffer* pBuffer, DWORD offset, BYTE* source, DWORD size)
{
	return 0;
}


// #5294: XLivePBufferGetByteArray
DWORD WINAPI XLivePBufferGetByteArray (FakePBuffer * pBuffer, DWORD offset, BYTE * destination, DWORD size)
{
	return 0;
}


// #5019: XLivePBufferSetByte
DWORD WINAPI XLivePBufferSetByte (FakePBuffer * pBuffer, DWORD offset, BYTE value)
{
	LIMITED_LOG(35, LOG_TRACE_XLIVE, "XLivePBufferSetByte  (pBuffer = {:p}, offset = {:X}, value = {})",
		(void*)pBuffer, offset, value );


	if (!pBuffer || offset < 0 || offset+1 > pBuffer->dwSize)
	{
		LOG_TRACE_XLIVE("- Invalid parameter");
		return (DWORD)-1;
	}

	if( pBuffer->magic != 0xDEADC0DE )
	{
		LOG_TRACE_XLIVE("- bad magic" );
		return 0;
	}

	pBuffer->pbData[offset] = value;

	return 0;
}


// #5018: XLivePBufferGetByte
DWORD WINAPI XLivePBufferGetByte(FakePBuffer* pBuffer, DWORD offset, BYTE* value)
{
	LIMITED_LOG(35, LOG_TRACE_XLIVE, "XLivePBufferGetByte  (pBuffer = {:p}, offset = {:X}, value = {:p})",
		(void*)pBuffer, offset, (void*)value);

	if (!pBuffer || !value || offset < 0 || offset + 1 > pBuffer->dwSize)
	{
		//LOG_TRACE_XLIVE("- Invalid parameter");
		return (DWORD)-1;
	}


	if (pBuffer->magic != 0xDEADC0DE)
	{
		LOG_TRACE_XLIVE("- bad magic");
		return 0;
	}


	*value = pBuffer->pbData[offset];
	return 0;
}


// #5020: XLivePBufferGetDWORD
DWORD WINAPI XLivePBufferGetDWORD(FakePBuffer* pBuffer, DWORD dwOffset, DWORD* pdwValue)
{
	return 0;
}


// #5021: XLivePBufferSetDWORD
DWORD WINAPI XLivePBufferSetDWORD(FakePBuffer * pBuffer, DWORD dwOffset, DWORD dwValue)
{
	return 0;
}


// #5026: XLiveSetSponsorToken
DWORD WINAPI XLiveSetSponsorToken(LPCWSTR pwszToken, DWORD dwTitleId)
{
	return S_OK;
}


// #5034: XLiveProtectData
HRESULT WINAPI XLiveProtectData (BYTE * pInBuffer, DWORD dwInDataSize, BYTE * pOutBuffer, DWORD *pDataSize, HANDLE handle )
{
	LOG_TRACE_XLIVE("XLiveProtectData  (pInBuffer = {0:p}, dwInDataSize = {1}, pOutBuffer = {2:p}, pDataSize = {3:x}, Handle = {4:p})",
		(void*)pInBuffer, dwInDataSize, (void*)pOutBuffer, *pDataSize, (void*)handle);


	if( *pDataSize < dwInDataSize )
	{
		if( pDataSize ) *pDataSize = dwInDataSize;

		LOG_TRACE_XLIVE( "- Insufficient buffer = {}", pDataSize ? *pDataSize : -1 );

		return HRESULT_FROM_WIN32( ERROR_INSUFFICIENT_BUFFER );
	}


	if( pOutBuffer )
		memcpy( pOutBuffer, pInBuffer, dwInDataSize );

	if( pDataSize )
		*pDataSize = dwInDataSize;


	return 0;
}


// #5035: XLiveUnprotectData
HRESULT WINAPI XLiveUnprotectData(BYTE * pInBuffer, DWORD dwInDataSize, BYTE * pOutBuffer, DWORD * pDataSize, PHANDLE pHandle)
{
	return 0;
}


// #5036: XLiveCreateProtectedDataContext
DWORD WINAPI XLiveCreateProtectedDataContext(DWORD * dwType, PHANDLE pHandle)
{
	return 1;
}


// #5037: XLiveQueryProtectedDataInformation
DWORD WINAPI XLiveQueryProtectedDataInformation(HANDLE h, DWORD * p)
{
	return 0;
}


// #5038: XLiveCloseProtectedDataContext
DWORD WINAPI XLiveCloseProtectedDataContext(HANDLE h)
{
	LOG_TRACE_XLIVE("XLiveCloseProtectedDataContext  (handle = {:p})", (void*)h);
	CloseHandle(h);
	return 0;
}

// #5348: XLiveProtectedCreateFile
HRESULT WINAPI XLiveProtectedCreateFile(HANDLE hContentAccess, void * pvReserved, PCWSTR pszFilePath,
	DWORD dwDesiredAccess, DWORD dwShareMode, SECURITY_ATTRIBUTES * pSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, PHANDLE phModule)
{
	return 0;
}


// #5367: XContentGetMarketplaceCounts
DWORD WINAPI XContentGetMarketplaceCounts(DWORD dwUserIndex, DWORD dwContentCategories, DWORD cbResults, XOFFERING_CONTENTAVAILABLE_RESULT *pResults, PXOVERLAPPED pOverlapped )
{
	return ERROR_SUCCESS;
}

// #5372: XMarketplaceCreateOfferEnumerator
DWORD WINAPI XMarketplaceCreateOfferEnumerator(DWORD dwUserIndex, DWORD dwOfferType, DWORD dwContentCategories, DWORD cItem, PDWORD pcbBuffer, PHANDLE phEnum)
{
	return ERROR_SUCCESS;
}

// #5376: XMarketplaceCreateOfferEnumeratorByOffering
DWORD WINAPI XMarketplaceCreateOfferEnumeratorByOffering(DWORD dwUserIndex, DWORD cItem, const ULONGLONG *pqwNumOffersIds, WORD cOfferIDs, PDWORD pcbBuffer, PHANDLE phEnum )
{
	return ERROR_SUCCESS;
}

// #5029
DWORD WINAPI XLiveSecureFreeLibrary(HMODULE hLibModule)
{
	LOG_TRACE_XLIVE("XLiveSecureFreeLibrary");
	if(hLibModule) FreeLibrary(hLibModule);
	return ERROR_SUCCESS;
}


// #5347
DWORD WINAPI XLiveProtectedLoadLibrary(int a1, int a2, LPCWSTR lpLibFileName, DWORD dwFlags, HMODULE* a5)
{
	return ERROR_SUCCESS;
}


// #5354
DWORD WINAPI XLiveContentVerifyInstalledPackage(int a1, int a2)
{
	return 0;
}


// 5298
DWORD WINAPI XLiveGetGuideKey(int pKeyStroke)
{
	return 0;
}


// 5334
DWORD WINAPI XOnlineGetServiceInfo(int, int)
{
   return ERROR_SUCCESS;
}


// 5028: ??
DWORD WINAPI XLiveLoadLibraryEx(LPCWSTR libFileName, HINSTANCE *a2, DWORD dwFlags)
{
	LOG_TRACE_XLIVE(L"XLiveLoadLibraryEx (libFileName = {0}, a2 = {1:p}, flags = {2:x})",
		libFileName, (void*)a2, dwFlags);

	HINSTANCE hInstance = LoadLibraryEx(libFileName, NULL, dwFlags);

	if (!hInstance)
		return HRESULT_FROM_WIN32(ERROR_INVALID_PARAMETER);

	*a2 = hInstance;
	return 0;
}

// 5290: ??
DWORD WINAPI XUserGetReputationStars(DWORD a1)
{
	// not done - error now
	return S_OK;
}


// 5296: ??
DWORD WINAPI XLiveGetLocalOnlinePort(DWORD a1)
{
	// not done - error now
	return S_OK;
}


// 5304
DWORD WINAPI XStorageUploadFromMemoryGetProgress(DWORD a1, DWORD a2, DWORD a3, DWORD a4)
{
	// not done - error now
	return 0x57;
}


// 5362
DWORD WINAPI MarketplaceDoesContentIdMatch(CHAR *a1, DWORD a2)
{
	// not done - error now
	SetLastError(0x57);
	return 0x57;
}

// 5363
DWORD WINAPI XLiveContentGetLicensePath()
{
	// not done - error now
	SetLastError(0x57);
	return 0x80070057;
}


// 5370
DWORD WINAPI TitleExport_XMarketplaceConsumeAssets(DWORD a1, DWORD a2, DWORD a3, DWORD a4)
{
	// not done - error now
	return 0x57;
}


// 5371
DWORD WINAPI XMarketplaceCreateAssetEnumerator(DWORD a1, DWORD a2, DWORD a3, DWORD a4)
{
	// not done - error now
	return 0x57;
}

// 5023: XNetGetCurrentAdapter
DWORD WINAPI XNetGetCurrentAdapter(DWORD a1, DWORD a2)
{
	return 0;
}


// 5025: XNetGetCurrentAdapter
DWORD WINAPI XLiveGetLiveIdError(DWORD a1, DWORD a2, DWORD a3, DWORD a4)
{
	return 0;
}


// 5039
DWORD WINAPI XLiveVerifyDataFile(DWORD a1)
{
	return 0;
}


// 5255
DWORD WINAPI XEnumerateBack(DWORD a1, DWORD a2, DWORD a3, DWORD a4, DWORD a5)
{
	return 0;
}


// 5341
DWORD WINAPI TitleExport_XPresenceUnsubscribe(DWORD a1, DWORD a2, DWORD a3)
{
	return 0;
}


// 5351
DWORD WINAPI XLiveContentInstallPackage(DWORD a1, DWORD a2, DWORD a3)
{
	return 0;
}


// 5357
DWORD WINAPI XLiveContentGetThumbnail(DWORD a1, DWORD a2, DWORD a3, DWORD a4)
{
	return 0;
}


// 5358
DWORD WINAPI XLiveContentInstallLicense(DWORD a1, DWORD a2, DWORD a3)
{
	return 0;
}


// 5377
DWORD WINAPI TitleExport_XUserFindUsers(int, int, int, int, int, int, int)
{
	return 1;
}


// === end of xlive functions ===
