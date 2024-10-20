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

extern void initialize_instance();
extern void exit_instance();

XMARKETPLACE_CONTENTOFFER_INFO marketplace[100];
int marketplaceCount = 0;

XMARKETPLACE_CONTENTOFFER_INFO marketplaceDlc[100];
int marketplaceDlcCount = 0;

int marketplaceEnumerate = 0;

void Check_Overlapped( PXOVERLAPPED pOverlapped )
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
	LOG_TRACE_XLIVE("XLiveUpdateHashes - a1: {0:x},a2: {1:x}", a1, a2);
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

	(*pBuffer)->dwSize = size;
	(*pBuffer)->id = CreateMutex(NULL,NULL,NULL);
	(*pBuffer)->magic = 0xDEADC0DE;

	//initialize real buffer inside fake buffer
	(*pBuffer)->pbData = (PBYTE)HeapAlloc(hHeap,HEAP_ZERO_MEMORY,size);

	if(!*pBuffer)
	{
		LOG_ERROR_XLIVE("XLivePBufferAllocate() unable to allocate {} bytes", size);
		return E_OUTOFMEMORY;
	}

	return 0;
}


// #5017: XLivePBufferFree
DWORD WINAPI XLivePBufferFree (FakePBuffer * pBuffer)
{
	LIMITED_LOG(35, LOG_TRACE_XLIVE, "XLivePBufferFree  (pBuffer = {:X})", (void*)pBuffer);

	if( !pBuffer ) return 0;


	if( pBuffer->magic != 0xDEADC0DE )
	{
		LOG_TRACE_XLIVE("- bad magic" );
		return 0;
	}


	HANDLE hHeap = GetProcessHeap();

	CloseHandle(pBuffer->id);
	HeapFree(hHeap,NULL,pBuffer->pbData);
	HeapFree(hHeap,NULL,pBuffer);

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
	else if( hEnum == g_dwMarketplaceContent && marketplaceEnumerate < marketplaceCount )
	{
		// TODO: check full buffer
		memcpy( pvBuffer, &marketplace, sizeof(XMARKETPLACE_CONTENTOFFER_INFO) * marketplaceCount );

		if( async == FALSE )
			*pcItemsReturned = marketplaceCount;

		else
			pOverlapped->InternalHigh = marketplaceCount;


		marketplaceEnumerate += marketplaceCount;
	}
	else 
	{
		// fall back to CServerList::Enumerate if previous handles didn't match other operations
		// it'll return ERROR_INVALID_HANDLE if it doesn't match this operation as well
		return CServerList::Enumerate(hEnum, cbBuffer, pvBuffer, pOverlapped);
	}

	if( async == FALSE )
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
}

// #5303: XStringVerify
DWORD WINAPI XStringVerify( DWORD dwFlags, const CHAR *szLocale, DWORD dwNumStrings, const STRING_DATA *pStringData, DWORD cbResults, STRING_VERIFY_RESPONSE *pResults, PXOVERLAPPED pOverlapped )
{
	LOG_TRACE_XLIVE("XStringVerify  (dwFlags = {0:x}, szLocale = {1}, dwNumStrings = {2}, pStringData = {3:p}, cbresults = {4}, pResults = {5:p}, pXOverlapped = {6:p})",
		dwFlags, szLocale, dwNumStrings, (void*)pStringData, cbResults, (void*)pResults, (void*)pOverlapped );



	if( pResults )
	{
		pResults->wNumStrings = dwNumStrings;
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
int WINAPI XOnlineCleanup ()
{
    LOG_TRACE_XLIVE("XOnlineCleanup");
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
int WINAPI XUserMuteListQuery (DWORD dwUserIndex, XUID XuidRemoteTalker, BOOL *pfOnMuteList)
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
	LOG_TRACE_XLIVE("XInviteSend");
	return 0;
}


// #5324: XOnlineGetNatType
XONLINE_NAT_TYPE WINAPI XOnlineGetNatType()
{
	LOG_TRACE_XLIVE("XOnlineGetNatType - NAT_OPEN");
	return XONLINE_NAT_OPEN;
}


// #5335: XTitleServerCreateEnumerator
DWORD WINAPI XTitleServerCreateEnumerator (LPCSTR pszServerInfo, DWORD cItem, DWORD * pcbBuffer, PHANDLE phEnum)
{
    LOG_TRACE_XLIVE("XTitleServerCreateEnumerator (cItem=> {})", cItem);
    if(phEnum)
		*phEnum = 0;
    return 1;
}

// #5338: XPresenceSubscribe
int WINAPI XPresenceSubscribe(int a1, int a2, int a3)
{
	LOG_TRACE_XLIVE("XPresenceSubscribe( a1: {0:x}, a2: {1:x}, a3: {2:x}", a1, a2, a3);
	return 0;
}

// #5340 XPresenceCreateEnumerator
int WINAPI XPresenceCreateEnumerator(DWORD dwUserIndex, DWORD cPeers, const XUID *pPeers, DWORD dwStartingIndex, DWORD dwPeersToReturn, DWORD *pcbBuffer, HANDLE *ph)
{
	LOG_TRACE_XLIVE("XPresenceCreateEnumerator(dwUserIndex: {0:x}, cPeers: {1:x})", dwUserIndex, cPeers);
	if(pcbBuffer)
		*(DWORD *)pcbBuffer = 0;

	return 1;
}

// #5346: XLiveLess_5346
DWORD WINAPI TitleExport_XUserEstimateRankForRating(DWORD a1, LPDWORD pdwResult, DWORD a3, void* a4, PXOVERLAPPED pOverlapped)
{
	LOG_TRACE_XLIVE("TitleExport_XUserEstimateRankForRating");
	return 0;
}


// #5349: XLiveProtectedVerifyFile
DWORD WINAPI XLiveProtectedVerifyFile (HANDLE hContentAccess, VOID * pvReserved, PCWSTR pszFilePath)
{
    LOG_TRACE_XLIVE(L"XLiveProtectedVerifyFile  (hContentAccess = {0:p}, pvReserved = {1:p}, pszFilePath = {2}",
		(void*)hContentAccess, (void*)pvReserved, pszFilePath );
    return 0;
}


// #5350: XLiveContentCreateAccessHandle
DWORD WINAPI XLiveContentCreateAccessHandle (DWORD dwTitleId, PXCONTENT_DATA pContentInfo, DWORD dwLicenseInfoVersion, FakePBuffer *xebBuffer, DWORD dwOffset, HANDLE * phAccess, PXOVERLAPPED pOverlapped)
{
	LOG_TRACE_XLIVE("XLiveContentCreateAccessHandle  (titleId = {0}, contentInfo = {1:p}, licenseInfo = {2}, pBuffer = {3:p}, offset = {4}, handle = {5:p}, overlapped = {6:p}",
		dwTitleId, (void*)pContentInfo, dwLicenseInfoVersion, (void*)xebBuffer, dwOffset, (void*)phAccess, (void*)pOverlapped );


	//while(1)
		//Sleep(1);

	DWORD package_num;

	package_num = marketplaceDlc[ pContentInfo->ContentNum ].qwOfferID & 0xFFFF;




	// ???
	memcpy( xebBuffer->pbData, &pContentInfo->TitleId, 4);


	// GTA IV - DLC verification
	memcpy( xebBuffer->pbData+4, &pContentInfo->ContentId, 20);



	char gameName[256];


	// GTA IV hack
	GetModuleFileNameA( NULL, (LPCH) &gameName, sizeof(gameName) );
	if( strstr( gameName, "GTAIV.exe" ) != 0 )
	{
		// license = 01 or 02
		memcpy( xebBuffer->pbData+24, &package_num, 4);
	}

	else
	{
		// license = 0xFFFFFFFF
		memset( xebBuffer->pbData+24, 0xFF, 4);
	}



	*phAccess = CreateMutex(NULL,NULL,NULL);
	LOG_TRACE_XLIVE(" - phAccess = {:p}", (void*)*phAccess );


	LOG_TRACE_XLIVE(" - TitleID = {:x}", pContentInfo->TitleId );
	LOG_TRACE_XLIVE(" - package_num = {:x}", package_num );


	return 0;
}


// #5352: XLiveContentUninstall
DWORD WINAPI XLiveContentUninstall (void * pContentInfo, XUID * pxuidFor, void * pInstallCallbackParams)
{
	LOG_TRACE_XLIVE("XLiveContentUninstall");
	return 0;
}


// #5355: XLiveContentGetPath
LONG WINAPI XLiveContentGetPath (DWORD dwUserIndex, PXCONTENT_DATA pContentData, wchar_t * pszPath, DWORD * pcchPath)
{
	LOG_TRACE_XLIVE("XLiveContentGetPath  (dwUserIndex = {0:x}, pXContentData = {1:p}, pszPath = {2:p}, pcchPath = {3})",
		dwUserIndex, (void*)pContentData, (void*)pszPath, *pcchPath );
	return 0;
}


// #5356: XContentCreatePackage
DWORD WINAPI XContentCreatePackage(DWORD dwUserIndex, PXCONTENT_DATA pContentData, WCHAR *pszPath, DWORD *pcchPath)
{
	LOG_TRACE_XLIVE(L"XContentCreatePackage (?) (dwUserIndex = {0:x}, pXContentData = {1:p}, pszPath = {2:p}, pcchPath = {3})",
		dwUserIndex, (void*)pContentData, (void*)pszPath, *pcchPath );
	return 0;
}


// #5360: XContentCreateEnumerator
DWORD WINAPI XContentCreateEnumerator(DWORD MaxEnumerator, PDWORD a2, PDWORD pchBuffer, PHANDLE phEnum)
{
	LIMITED_LOG(15, LOG_TRACE_XLIVE, "XContentCreateEnumerator  (MaxEnumerator = {0:x}, a2 = {1:p}, pchBuffer = {2:p}, phEnum = {3:p}",
		MaxEnumerator, (void*)a2, (void*)pchBuffer, (void*)phEnum);
	return 0;
}


// #5361: XContentRetrieveOffersByDate
DWORD WINAPI XContentRetrieveOffersByDate (DWORD dwUserIndex, DWORD dwOffserInfoVersion,
        SYSTEMTIME * pstStartDate, void * pOffserInfoArray, DWORD * pcOfferInfo, PXOVERLAPPED pOverlapped)
{
    LOG_TRACE_XLIVE("XLiveContentRetrieveOffersByDate");
    return 0;
}


// #5295: XLivePBufferSetByteArray
DWORD WINAPI XLivePBufferSetByteArray(FakePBuffer* pBuffer, DWORD offset, BYTE* source, DWORD size)
{
	LIMITED_LOG(35, LOG_TRACE_XLIVE, "XLivePBufferSetByteArray  (pBuffer = {:p}, offset = {:x}, source = {:p}, size = {})",
		(void*)pBuffer, offset, (void*)source, size);

	if (!pBuffer || !source || offset < 0 || offset + size > pBuffer->dwSize)
	{
		LOG_TRACE_XLIVE("- Invalid parameter");
		return -1;
	}

	if (pBuffer->magic != 0xDEADC0DE)
	{
		LOG_TRACE_XLIVE("- bad magic");
		return 0;
	}


	memcpy(pBuffer->pbData + offset, source, size);
	return 0;
}


// #5294: XLivePBufferGetByteArray
DWORD WINAPI XLivePBufferGetByteArray (FakePBuffer * pBuffer, DWORD offset, BYTE * destination, DWORD size)
{
	LIMITED_LOG(35, LOG_TRACE_XLIVE, "XLivePBufferGetByteArray  (pBuffer = {0:p}, pBuffer->Id = {}, offset = {2}, dest = {3:p}, size = {4}",
		(void*)pBuffer, pBuffer->id, offset, (void*)destination, size);

	if (!pBuffer || !destination || offset < 0 || offset+size > pBuffer->dwSize)
	{
		LOG_TRACE_XLIVE("- Invalid parameter");
		return -1;
	}


	if( pBuffer->magic != 0xDEADC0DE )
	{
		LOG_TRACE_XLIVE("- bad magic" );
		return 0;
	}


	memcpy (destination, pBuffer->pbData+offset, size);
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
		return -1;
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
		return -1;
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
	LIMITED_LOG(35, LOG_TRACE_XLIVE, "XLivePBufferGetDWORD  (pBuffer = {:p}, dwOffset = {:X}, pdwValue = {:p})",
		(void*)pBuffer, dwOffset, (void*)pdwValue);

	if (!pBuffer || dwOffset < 0 || dwOffset + 4 > pBuffer->dwSize || !pdwValue)
	{
		LOG_TRACE_XLIVE("- Invalid parameter");
		return -1;
	}

	if (pBuffer->magic != 0xDEADC0DE)
	{
		LOG_TRACE_XLIVE("- bad magic");
		return 0;
	}


	*pdwValue = *((DWORD*)(pBuffer->pbData + dwOffset));
	return 0;
}


// #5021: XLivePBufferSetDWORD
DWORD WINAPI XLivePBufferSetDWORD( FakePBuffer * pBuffer, DWORD dwOffset, DWORD dwValue )
{
	LIMITED_LOG(35, LOG_TRACE_XLIVE, "XLivePBufferSetDWORD  (pBuffer = {:p}, dwOffset = {:X}, dwValue = {})",
			(void*)pBuffer, dwOffset, dwValue );

	if (!pBuffer || dwOffset < 0 || dwOffset+4 > pBuffer->dwSize )
	{
		LOG_TRACE_XLIVE("- Invalid parameter");
		return -1;
	}


	if( pBuffer->magic != 0xDEADC0DE )
	{
		LOG_TRACE_XLIVE("- bad magic" );
		return 0;
	}


	*((DWORD *)(pBuffer->pbData + dwOffset)) = dwValue;
	return 0;
}



// #5026: XLiveSetSponsorToken
DWORD WINAPI XLiveSetSponsorToken (LPCWSTR pwszToken, DWORD dwTitleId)
{
    LOG_TRACE_XLIVE("XLiveSetSponsorToken ({:X})", dwTitleId);
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
HRESULT WINAPI XLiveUnprotectData (BYTE * pInBuffer, DWORD dwInDataSize, BYTE * pOutBuffer, DWORD * pDataSize, PHANDLE pHandle)
{

	LOG_TRACE_XLIVE("XLiveUnprotectData  (pInBuffer = {0:p}, dwInDataSize = {1}, pOutBuffer = {2:p}, pDataSize = {3:x}, Handle = {4:p})",
		(void*)pInBuffer, dwInDataSize, (void*)pOutBuffer, *pDataSize, (void*)pHandle);


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


// #5036: XLiveCreateProtectedDataContext
DWORD WINAPI XLiveCreateProtectedDataContext (DWORD * dwType, PHANDLE pHandle)
{
    LOG_TRACE_XLIVE("XLiveCreateProtectedDataContext");
	if (pHandle)
	{
		*pHandle = CreateMutex(NULL,NULL,NULL);


		LOG_TRACE_XLIVE("- Handle = {:x}", *pHandle);
		return 0;
	}

	return 1;
}


// #5037: XLiveQueryProtectedDataInformation
DWORD WINAPI XLiveQueryProtectedDataInformation (HANDLE h, DWORD * p)
{
    LOG_TRACE_XLIVE("XLiveQueryProtectedDataInformation  (h = {0:p}, p = {1:p})", (void*)h, (void*)p );
    return 0;
}


// #5038: XLiveCloseProtectedDataContext
DWORD WINAPI XLiveCloseProtectedDataContext (HANDLE h)
{
    LOG_TRACE_XLIVE("XLiveCloseProtectedDataContext  (handle = {:p})", (void*)h);
	CloseHandle(h);
    return 0;
}

// #5348: XLiveProtectedCreateFile
HRESULT WINAPI XLiveProtectedCreateFile (HANDLE hContentAccess, void * pvReserved, PCWSTR pszFilePath,
	DWORD dwDesiredAccess, DWORD dwShareMode, SECURITY_ATTRIBUTES * pSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, PHANDLE phModule)
{
	LOG_TRACE_XLIVE(L"XLiveProtectedCreateFile  (hContentAccess = {0:p}, pvReserved = {1:p}, pszFilePath = {2}, dwDesiredAccess = {3}, dwShareMode = {4}, dwSecurityAttributes = {5:p}, dwCreationDisposition = {6:p}, flagsAndAttributes = {7}, phModule = {8:p})",
		(void*)hContentAccess, pvReserved, pszFilePath, dwDesiredAccess, dwShareMode, (void*)pSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, (void*)phModule );
	return 0;
}


// #5367: XContentGetMarketplaceCounts
DWORD WINAPI XContentGetMarketplaceCounts( DWORD dwUserIndex, DWORD dwContentCategories, DWORD cbResults, XOFFERING_CONTENTAVAILABLE_RESULT *pResults, PXOVERLAPPED pOverlapped )
{
	LOG_TRACE_XLIVE( "XContentGetMarketplaceCounts  (dwUserIndex = {0}, dwContentCategories = {1:x}, cbResults = {2:x}, pResults = {3:p}, pOverlapped = {4:p})",
		dwUserIndex, dwContentCategories, cbResults, (void*)pResults, (void*)pOverlapped );
	return ERROR_SUCCESS;
}

// #5372: XMarketplaceCreateOfferEnumerator
DWORD WINAPI XMarketplaceCreateOfferEnumerator(DWORD dwUserIndex, DWORD dwOfferType, DWORD dwContentCategories, DWORD cItem, PDWORD pcbBuffer, PHANDLE phEnum)
{
	LOG_TRACE_XLIVE("XMarketplaceCreateOfferEnumerator  (dwUserIndex = {0}, dwOfferType = {1:x}, dwContentCategories = {2:x}, cItem = {3}, pchBuffer = {4:p}, phEnum = {5:p}",
		dwUserIndex, dwOfferType, dwContentCategories, cItem, (void*)pcbBuffer, (void*)phEnum);
	return ERROR_SUCCESS;
}

DWORD WINAPI XMarketplaceCreateOfferEnumeratorByOffering( DWORD dwUserIndex, DWORD cItem, const ULONGLONG *pqwNumOffersIds, WORD cOfferIDs, PDWORD pcbBuffer, PHANDLE phEnum )
{
	LOG_TRACE_XLIVE("XMarketplaceCreateOfferEnumeratorByOffering  (dwUserIndex = {0}, cItem = {1}, pqwNumOffersIds = {2:p}, cOfferIDs = {3}, pcbBuffer = {4:p}, phEnum = {5:p})",
		dwUserIndex, cItem, (void*)pqwNumOffersIds, cOfferIDs, (void*)pcbBuffer, (void*)phEnum );


	if( pcbBuffer ) *pcbBuffer = sizeof( XMARKETPLACE_CONTENTOFFER_INFO ) * cItem;
	if( phEnum )
	{
		*phEnum = g_dwMarketplaceContent = CreateMutex(NULL,NULL,NULL);

		LOG_TRACE_XLIVE("- Handle = {:p}", (void*)*phEnum);
	}


	for( int lcv = 0; lcv < cOfferIDs; lcv++ )
	{
		memcpy( &marketplace[lcv].dwTitleID, ((DWORD *) (&pqwNumOffersIds[lcv])) + 1, 4 );
		memcpy( &marketplace[lcv].qwOfferID, &pqwNumOffersIds[lcv], sizeof(ULONGLONG) );

		marketplace[lcv].wszTitleName = 0;
		marketplace[lcv].wszOfferName = 0;

		marketplace[lcv].dwOfferNameLength = 0;
		marketplace[lcv].dwSellTextLength = 0;
		marketplace[lcv].dwPackageSize = 0;

		marketplace[lcv].dwOfferType = XMARKETPLACE_OFFERING_TYPE_CONTENT;
		marketplace[lcv].dwContentCategory = XCONTENTTYPE_MARKETPLACE;

		marketplace[lcv].fIsUnrestrictedLicense = FALSE;
		marketplace[lcv].fUserHasPurchased = TRUE;
		marketplace[lcv].dwLicenseMask = 0xFFFFFFFF;



		if( lcv == 0 )
			LOG_TRACE_XLIVE( "- [{0}] TitleId = {1:x}", lcv, marketplace[lcv].dwTitleID );

		LOG_TRACE_XLIVE( "- [{0}] OfferingID = {1:x}", lcv, pqwNumOffersIds[lcv] );
	}

	marketplaceCount = cOfferIDs;
	marketplaceEnumerate = 0;


	return ERROR_SUCCESS;
}

// #5029
DWORD WINAPI XLiveSecureFreeLibrary(HMODULE hLibModule)
{
    LOG_TRACE_XLIVE("XLiveSecureFreeLibrary");
    if(hLibModule) FreeLibrary(hLibModule);
    return 0;
}


// #5347
DWORD WINAPI XLiveProtectedLoadLibrary(int a1, int a2, LPCWSTR lpLibFileName, DWORD dwFlags, HMODULE* a5)
{
    LOG_TRACE_XLIVE("XLiveProtectedLoadLibrary");
    int result; // eax@2
    HMODULE v6; // eax@3

    if ( a1 )
    {
        result = 0;
    }
    else
    {
        v6 = LoadLibraryExW(lpLibFileName, 0, dwFlags);
        if ( a5 )
            *a5 = v6;
        result = 0;
    }
    return result;
}


// #5354
DWORD WINAPI XLiveContentVerifyInstalledPackage(int a1, int a2)
{
    LOG_TRACE_XLIVE(" XLiveContentVerifyInstalledPackage");
    return 0;
}


DWORD WINAPI TitleExport_XUserFinderUsers(int a1, int a2, int a3, void* a4, unsigned int a5, unsigned __int8 *a6, PXOVERLAPPED *a7)
{
    LOG_TRACE_XLIVE("TitleExport_XUserFinderUsers");
    return 1;
}


DWORD WINAPI XLiveGetGuideKey(int pKeyStroke)
{
    LOG_TRACE_XLIVE("XLiveGetGuideKey(pKeyStroke: {:x})",pKeyStroke);

    return 0;
}


// 5334
DWORD WINAPI XOnlineGetServiceInfo( int, int )
{
   LOG_TRACE_XLIVE("XOnlineGetServiceInfo  (*** checkme ***)");


	// ?? one path
   return ERROR_SUCCESS;
   //return 0x4DB;
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
DWORD WINAPI XUserGetReputationStars( DWORD a1 )
{
  LOG_TRACE_XLIVE("XUserGetReputationStars  (*** checkme ***) (a1 = {:x})",
		a1);


	// not done - error now
	return S_OK;
}


// 5296: ??
DWORD WINAPI XLiveGetLocalOnlinePort( DWORD a1 )
{
  LOG_TRACE_XLIVE("XLiveGetLocalOnlinePort  (*** checkme ***) (a1 = {:x})",
		a1);


	// not done - error now
	return S_OK;
}



// 5304
DWORD WINAPI XStorageUploadFromMemoryGetProgress( DWORD a1, DWORD a2, DWORD a3, DWORD a4 )
{
  LOG_TRACE_XLIVE("XStorageUploadFromMemoryGetProgress  (*** checkme ***) (a1 = {0:x}, a2 = {1:x}, a3 = {2:x}, a4 = {3:x})",
		a1, a2, a3, a4 );


	// not done - error now
	return 0x57;
}

// 5362
DWORD WINAPI MarketplaceDoesContentIdMatch( CHAR *a1, DWORD a2 )
{
  LOG_TRACE_XLIVE("MarketplaceDoesContentIdMatch  (*** checkme ***) (a1 = {}, a2 = {:x})",
		a1, a2);


	// not done - error now
	SetLastError( 0x57 );
	return 0x57;
}


// 5363
DWORD WINAPI XLiveContentGetLicensePath()
{
  LOG_TRACE_XLIVE("XLiveContentGetLicensePath  (*** checkme ***)");


	// not done - error now
	SetLastError( 0x57 );
	return 0x80070057;
}




// 5370: ??
DWORD WINAPI TitleExport_XMarketplaceConsumeAssets( DWORD a1, DWORD a2, DWORD a3, DWORD a4 )
{
  LOG_TRACE_XLIVE("TitleExport_XMarketplaceConsumeAssets (a1 = {0:x}, a2 = {1:x}, a3 = {2:x}, a4 = {3:x})",
		a1, a2, a3, a4);


	// not done - error now
	return 0x57;
}


// 5371: ??
DWORD WINAPI XMarketplaceCreateAssetEnumerator( DWORD a1, DWORD a2, DWORD a3, DWORD a4 )
{
  LOG_TRACE_XLIVE("XMarketplaceCreateAssetEnumerator (a1 = {0:x}, a2 = {1:x}, a3 = {2:x}, a4 = {3:x})",
		a1, a2, a3, a4);


	// not done - error now
	return 0x57;
}



DWORD WINAPI XNetGetCurrentAdapter( DWORD a1, DWORD a2 )
{
	LOG_TRACE_XLIVE( "XNetGetCurrentAdapter  (a1 = {0:x}, a2 = {1:x})",
		a1, a2 );


	return 0;
}



DWORD WINAPI XLiveGetLiveIdError( DWORD a1, DWORD a2, DWORD a3, DWORD a4 )
{
	LOG_TRACE_XLIVE( "XLiveGetLiveIdError  (a1 = {0:x}, a2 = {1:x}, a3 = {2:x}, a4 = {3:x})",
		a1, a2, a3, a4 );


	return 0;
}



DWORD WINAPI XLiveVerifyDataFile( DWORD a1 )
{
	LOG_TRACE_XLIVE( "XLiveVerifyDataFile  (a1 = {:x})",
		a1 );


	return 0;
}






DWORD WINAPI XEnumerateBack( DWORD a1, DWORD a2, DWORD a3, DWORD a4, DWORD a5 )
{
	LOG_TRACE_XLIVE( "XEnumerateBack  (a1 = {0:x}, a2 = {1:x}, a3 = {2:x}, a4 = {3:x}, a5 = {4:x})",
		a1, a2, a3, a4, a5 );


	return 0;
}


DWORD WINAPI TitleExport_XPresenceUnsubscribe( DWORD a1, DWORD a2, DWORD a3 )
{
	LOG_TRACE_XLIVE( "TitleExport_XPresenceUnsubscribe  (a1 = {0:x}, a2 = {1:x}, a3 = {2:x})",
		a1, a2, a3 );


	return 0;
}


DWORD WINAPI XLiveContentInstallPackage( DWORD a1, DWORD a2, DWORD a3 )
{
	LOG_TRACE_XLIVE( "XLiveContentInstallPackage  (a1 = {0:x}, a2 = {1:x}, a3 = {2:x})",
		a1, a2, a3 );


	return 0;
}


DWORD WINAPI XLiveContentGetThumbnail( DWORD a1, DWORD a2, DWORD a3, DWORD a4 )
{
	LOG_TRACE_XLIVE( "XLiveContentGetThumbnail  (a1 = {0:x}, a2 = {1:x}, a3 = {2:x}, a4 = {3:x})",
		a1, a2, a3, a4);


	return 0;
}


DWORD WINAPI XLiveContentInstallLicense( DWORD a1, DWORD a2, DWORD a3 )
{
	LOG_TRACE_XLIVE( "XLiveContentInstallLicense  (a1 = {0:x}, a2 = {1:x}, a3 = {2:x})",
		a1, a2, a3);


	return 0;
}



DWORD WINAPI TitleExport_XUserFindUsers(int, int, int, int, int, int, int)
{
	LOG_TRACE_XLIVE("TitleExport_XUserFindUsers");
	return 1;
}


// === end of xlive functions ===
