
#include "XLive\xnet\IpManagement\XnIp.h"
#include "H2MOD\Modules\MapManager\MapManager.h"

XSESSION_LOCAL_DETAILS sessionDetails;
extern void Check_Overlapped(PXOVERLAPPED pOverlapped);


// #5300: XSessionCreate
LONG WINAPI XSessionCreate(DWORD dwFlags, DWORD dwUserIndex, DWORD dwMaxPublicSlots, DWORD dwMaxPrivateSlots, ULONGLONG *pqwSessionNonce, PXSESSION_INFO pSessionInfo, PXOVERLAPPED pOverlapped, HANDLE *phEnum)
{
	LOG_TRACE_XLIVE("XSessionCreate  (flags = {0:x}, userIndex = {1}, maxPublicSlots = {2}, maxPrivateSlots = {3}, sessionNonce = {4:p}, pSessionInfo = {5:p}, pOverlapped = {6:p}, handle = {7:p})",
		dwFlags, dwUserIndex, dwMaxPublicSlots, dwMaxPrivateSlots, (void*)pqwSessionNonce, (void*)pSessionInfo, (void*)pOverlapped, (void*)phEnum);


	if (phEnum) *phEnum = CreateMutex(NULL, NULL, NULL);

	if ((dwFlags & XSESSION_CREATE_HOST))
	{
		if (pqwSessionNonce) XNetRandom((BYTE*)pqwSessionNonce, sizeof(*pqwSessionNonce));

		if (pSessionInfo)
		{
			ipManager.GetLocalXNAddr(&pSessionInfo->hostAddress);
			ipManager.getRegisteredKeys(&pSessionInfo->sessionID, &pSessionInfo->keyExchangeKey);
		}

		LOG_TRACE_XLIVE("XSessionCreate - XSESSION_CREATE_HOST");
	}

	if ((dwFlags & XSESSION_CREATE_USES_ARBITRATION) > 0)
	{
		LOG_TRACE_XLIVE("XSessionCreate - XSESSION_CREATE_USES_ARBITRATION");
	}

	if ((dwFlags & XSESSION_CREATE_USES_PEER_NETWORK) > 0)
	{
		LOG_TRACE_XLIVE("XSessionCreate - XSESSION_CREATE_USES_PEER_NETWORK");
	}

	if ((dwFlags & XSESSION_CREATE_USES_PRESENCE) > 0)
	{
		LOG_TRACE_XLIVE("XSessionCreate - XSESSION_CREATE_USES_PRESENCE");
	}


	// local cache
	sessionDetails.dwUserIndexHost = 0;

	// already filled - SetContext
	//sessionDetails.dwGameType = 0;
	//sessionDetails.dwGameMode = 0;

	sessionDetails.dwFlags = dwFlags;

	sessionDetails.dwMaxPublicSlots = dwMaxPublicSlots;
	sessionDetails.dwMaxPrivateSlots = dwMaxPrivateSlots;
	sessionDetails.dwAvailablePublicSlots = dwMaxPublicSlots;
	sessionDetails.dwAvailablePrivateSlots = dwMaxPrivateSlots;

	sessionDetails.dwActualMemberCount = 0;
	sessionDetails.dwReturnedMemberCount = 0;

	sessionDetails.eState = XSESSION_STATE_LOBBY;
	sessionDetails.qwNonce = *pqwSessionNonce;

	// skipme
	//sessionDetails.sessionInfo = 0;
	//sessionDetails.xnkidArbitration = 0;

	//sessionDetails.pSessionMembers = 0;

	if (phEnum)
		LOG_TRACE_XLIVE("- handle = {}", *phEnum);

	if (pOverlapped)
	{
		pOverlapped->InternalLow = ERROR_SUCCESS;
		pOverlapped->InternalHigh = 0;
		pOverlapped->dwExtendedError = ERROR_SUCCESS;
		Check_Overlapped(pOverlapped);
		return ERROR_IO_PENDING;
	}
	
	return ERROR_SUCCESS;
}

// #5332: XSessionEnd
int WINAPI XSessionEnd(HANDLE hSession,
	PXOVERLAPPED pXOverlapped)
{
	LOG_TRACE_XLIVE("XSessionEnd()");
	return 0;
}

// #5317: XSessionWriteStats
DWORD WINAPI XSessionWriteStats(DWORD, DWORD, DWORD, DWORD, DWORD, PXOVERLAPPED pOverlapped)
{
	LOG_TRACE_XLIVE("XSessionWriteStats  (pOverlapped = {:p})",
		(void*)pOverlapped);


	Check_Overlapped(pOverlapped);

	return ERROR_SUCCESS;
}


// #5318: XSessionStart
int WINAPI XSessionStart(HANDLE hSession, DWORD dwFlags, PXOVERLAPPED pOverlapped)
{
	LOG_TRACE_XLIVE("XSessionStart  (hSession = {0:p}, dwFlags = {1:x}, pOverlapped = {2:p}",
		(void*)hSession, dwFlags, (void*)pOverlapped);


	if (pOverlapped)
	{
		pOverlapped->InternalLow = ERROR_SUCCESS;
		pOverlapped->dwExtendedError = ERROR_SUCCESS;
		pOverlapped->InternalHigh = 0;


		Check_Overlapped(pOverlapped);

		return ERROR_IO_PENDING;
	}

	return ERROR_SUCCESS;
}


// #5319: XSessionSearchEx
DWORD WINAPI XSessionSearchEx(DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD)
{
	LOG_TRACE_XLIVE("XSessionSearchEx");
	return 0;
}


// #5320: XSessionSearchByID
DWORD WINAPI XSessionSearchByID(DWORD xnkid1, DWORD xnkid2, DWORD, DWORD * pcbResultsBuffer, void *, PXOVERLAPPED pOverlapped)
{
	LOG_TRACE_XLIVE("XSessionSearchByID");

	if (pcbResultsBuffer)
		*pcbResultsBuffer = 0;


	Check_Overlapped(pOverlapped);

	return 0;
}


// #5321: XSessionSearch
DWORD WINAPI XSessionSearch(DWORD, DWORD, DWORD, WORD, WORD, void *, void *, DWORD * pcbResultsBuffer, void *, PXOVERLAPPED pOverlapped)
{
	LOG_TRACE_XLIVE("XSessionSearch");

	if (pcbResultsBuffer)
		*pcbResultsBuffer = 0;


	Check_Overlapped(pOverlapped);

	return 0;
}


// #5322: XSessionModify
DWORD WINAPI XSessionModify(DWORD, DWORD, DWORD, DWORD, DWORD)
{
	LOG_TRACE_XLIVE("XSessionModify");
	return 0;
}


// #5323: XSessionMigrateHost
DWORD WINAPI XSessionMigrateHost(DWORD, DWORD, DWORD, DWORD)
{
	LOG_TRACE_XLIVE("XSessionMigrateHost");
	return 0;
}


// #5325: XSessionLeaveLocal
DWORD WINAPI XSessionLeaveLocal(HANDLE hSession, DWORD dwUserCount, const DWORD *pdwUserIndexes, PXOVERLAPPED pXOverlapped)
{
	LOG_TRACE_XLIVE("XSessionLeaveLocal");

	if (!hSession)
		return ERROR_INVALID_PARAMETER;
	if (!pdwUserIndexes)
		return ERROR_INVALID_PARAMETER;

	//TODO XSessionLeaveLocal
	if (pXOverlapped) {
		//asynchronous

		pXOverlapped->InternalLow = ERROR_SUCCESS;
		pXOverlapped->InternalHigh = ERROR_SUCCESS;
		pXOverlapped->dwExtendedError = ERROR_SUCCESS;

		Check_Overlapped(pXOverlapped);

		return ERROR_IO_PENDING;
	}
	else {
		//synchronous
		//return result;
	}
	return ERROR_SUCCESS;
}


// #5326: XSessionJoinRemote
DWORD WINAPI XSessionJoinRemote(HANDLE hSession, DWORD dwXuidCount, const XUID *pXuids, const BOOL *pfPrivateSlots, PXOVERLAPPED pXOverlapped)
{
	LOG_TRACE_XLIVE("XSessionJoinRemote");

	if (pXOverlapped)
	{
		pXOverlapped->InternalHigh = 0;
		pXOverlapped->InternalLow = ERROR_SUCCESS;
		pXOverlapped->dwExtendedError = ERROR_SUCCESS;

		Check_Overlapped(pXOverlapped);
		return ERROR_IO_PENDING;
	}
	return ERROR_SUCCESS;
}


// #5327: XSessionJoinLocal
DWORD WINAPI XSessionJoinLocal(HANDLE hSession, DWORD dwUserCount, const DWORD *pdwUserIndexes, const BOOL *pfPrivateSlots, PXOVERLAPPED pOverlapped)
{
	LOG_TRACE_XLIVE("XSessionJoinLocal  (hSession = {0:p}, dwUserCount = {1:x}, pdwUserIndexes = {2:p}, pfPrivateSlots = {3:p}, pOverlapped = {4:p})",
		(void*)hSession, dwUserCount, (void*)pdwUserIndexes, (void*)pfPrivateSlots, (void*)pOverlapped);


	for (DWORD lcv = 0; lcv < dwUserCount; lcv++)
	{
		LOG_TRACE_XLIVE("- user {0} = {1}  ({2})", lcv + 1, pdwUserIndexes[lcv], pfPrivateSlots[lcv] ? "Private" : "Public");
	}

	if (pOverlapped == 0)
		return ERROR_SUCCESS;


	pOverlapped->InternalHigh = 0;
	pOverlapped->InternalLow = ERROR_SUCCESS;
	pOverlapped->dwExtendedError = ERROR_SUCCESS;


	Check_Overlapped(pOverlapped);

	return ERROR_IO_PENDING;
}


// #5328: XSessionGetDetails
DWORD WINAPI XSessionGetDetails(HANDLE hSession, PDWORD pcbResultsBuffer, PXSESSION_LOCAL_DETAILS pSessionDetails, PXOVERLAPPED pOverlapped)
{
	DWORD max_size;


	LOG_TRACE_XLIVE("XSessionGetDetails  (hSession = {0:p}, pcbResultsBuffer = {1:p} ({2:p}), pSessionDetails = {3:p}, pOverlapped = {4:p})",
		(void*)hSession, (void*)pcbResultsBuffer, (void*)*pcbResultsBuffer, (void*)pSessionDetails, (void*)pOverlapped);



	// max allowed
	max_size = sizeof(XSESSION_LOCAL_DETAILS) + (sessionDetails.dwMaxPrivateSlots + sessionDetails.dwMaxPublicSlots) * sizeof(XSESSION_MEMBER);
	if (*pcbResultsBuffer < max_size)
	{
		*pcbResultsBuffer = max_size;


		if (pOverlapped == 0)
		{
			LOG_TRACE_XLIVE("- ERROR_INSUFFICIENT_BUFFER = {:x}", max_size);
			return ERROR_INSUFFICIENT_BUFFER;
		}

		else
		{
			pOverlapped->InternalHigh = 0;
			pOverlapped->InternalLow = ERROR_INSUFFICIENT_BUFFER;
			pOverlapped->dwExtendedError = ERROR_INSUFFICIENT_BUFFER;


			Check_Overlapped(pOverlapped);

			return ERROR_IO_PENDING;
		}
	}

	// sent in blank template, refill values
	memset(pSessionDetails, 0xff, max_size);
	memcpy(pSessionDetails, &sessionDetails, sizeof(sessionDetails));


	// fixme
	sessionDetails.dwReturnedMemberCount = 0;
	sessionDetails.pSessionMembers = 0;



	if (pOverlapped == 0)
		return ERROR_SUCCESS;


	pOverlapped->InternalHigh = 0;
	pOverlapped->InternalLow = ERROR_SUCCESS;
	pOverlapped->dwExtendedError = ERROR_SUCCESS;


	Check_Overlapped(pOverlapped);

	return ERROR_IO_PENDING;
}


// #5329: XSessionFlushStats
int WINAPI XSessionFlushStats(DWORD, DWORD)
{
	LOG_TRACE_XLIVE("XSessionFlushStats");
	return 0;
}


// #5330: XSessionDelete
DWORD WINAPI XSessionDelete(HANDLE hSession, PXOVERLAPPED pXOverlapped)
{
	LOG_TRACE_XLIVE("XSessionDelete");

	if (!hSession)
		return ERROR_INVALID_PARAMETER;

	DWORD ret = 0;
	//TODO XSessionDelete
	if (pXOverlapped) {
		//asynchronous

		pXOverlapped->InternalLow = ERROR_SUCCESS;
		pXOverlapped->InternalHigh = ERROR_SUCCESS;
		pXOverlapped->dwExtendedError = ERROR_SUCCESS;

		Check_Overlapped(pXOverlapped);

		ret = ERROR_IO_PENDING;
	}
	else {
		//synchronous
	}

	return ret;
}

// #5333: XSessionArbitrationRegister
DWORD WINAPI XSessionArbitrationRegister(DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD)
{
	LOG_TRACE_XLIVE("XSessionArbitrationRegister");
	return 0;
}


// #5336: XSessionLeaveRemote
DWORD WINAPI XSessionLeaveRemote(HANDLE hSession, DWORD dwXuidCount, const XUID *pXuids, XOVERLAPPED *pXOverlapped)
{
	LOG_TRACE_XLIVE("XSessionLeaveRemote");
	if (!hSession)
		return ERROR_INVALID_PARAMETER;
	if (!dwXuidCount)
		return ERROR_INVALID_PARAMETER;
	if (!pXuids)
		return ERROR_INVALID_PARAMETER;

	//TODO XSessionLeaveRemote
	if (pXOverlapped) {
		//asynchronous

		pXOverlapped->InternalLow = ERROR_SUCCESS;
		pXOverlapped->InternalHigh = ERROR_SUCCESS;
		pXOverlapped->dwExtendedError = ERROR_SUCCESS;

		Check_Overlapped(pXOverlapped);

		return ERROR_IO_PENDING;
	}
	else {
		//synchronous
		//return result;
	}
	return ERROR_SUCCESS;
}


// #5343: XSessionCalculateSkill
DWORD WINAPI XSessionCalculateSkill(DWORD, DWORD, DWORD, DWORD, DWORD)
{
	LOG_TRACE_XLIVE("XSessionCalculateSkill");
	return 0;
}

// #5342: XSessionModifySkill
DWORD WINAPI XSessionModifySkill(HANDLE, DWORD, void * rgXuid, PXOVERLAPPED pOverlapped)
{
	LOG_TRACE_XLIVE("XSessionModifySkill");
	return 0;
}
