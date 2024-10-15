#include "stdafx.h"

#include "XUser.h"

#include "XLive/xbox/xbox.h"
#include "XLive/xnet/IpManagement/XnIp.h"

#include "H2MOD/Modules/Achievements/Achievements.h"

extern void Check_Overlapped(PXOVERLAPPED pOverlapped);

bool signInChanged[4];
XUSER_SIGNIN_INFO usersSignInInfo[4];

bool UserSignInChanged()
{
	for (int i = 0; i < 4; i++)
	{
		if (signInChanged[i])
			return true;
	}
	return false;
}

bool UserSignedIn(DWORD dwUserIndex)
{
	if (usersSignInInfo[dwUserIndex].UserSigninState == eXUserSigninState_SignedInLocally
		|| usersSignInInfo[dwUserIndex].UserSigninState == eXUserSigninState_SignedInToLive)
		return true;

	return false;
}

bool UserSignedInLocally(DWORD dwUserIndex)
{
	if (usersSignInInfo[dwUserIndex].UserSigninState == eXUserSigninState_SignedInLocally)
		return true;

	return false;
}

bool UserSignedOnline(DWORD dwUserIndex)
{
	if (usersSignInInfo[dwUserIndex].UserSigninState == eXUserSigninState_SignedInToLive)
		return true;

	return false;
}

XUSER_SIGNIN_INFO* UserGetSignInInfo(DWORD dwUserIndex)
{
	if (dwUserIndex != 0)
		dwUserIndex = 0;

	return &usersSignInInfo[dwUserIndex];
}

void XUserSetupGuests(XUID primary_xuid,bool online)
{
	for (DWORD dwUserIndex = 1; dwUserIndex < 4; dwUserIndex++)
	{
		if (online)
		{
			usersSignInInfo[dwUserIndex].dwInfoFlags |= XUSER_INFO_FLAG_LIVE_ENABLED | XUSER_INFO_FLAG_GUEST;
			usersSignInInfo[dwUserIndex].UserSigninState = eXUserSigninState_SignedInToLive;
			// primary_xuid in ONLINE is sure to have last 2 bits 0
			// we can just use OR operator to append the guest_index to make them different
			// this is then used to create online_guest_names which are tied to this guest_index
			usersSignInInfo[dwUserIndex].xuid = primary_xuid | dwUserIndex;
		}
		else
		{
			usersSignInInfo[dwUserIndex].dwInfoFlags |= XUSER_INFO_FLAG_GUEST;
			usersSignInInfo[dwUserIndex].UserSigninState = eXUserSigninState_SignedInLocally;
			// primary_xuid in OFFLINE is returned by rand() and we have no idea which bits are set
			// in this case we will just add the local_user_index to make the xuids slightly different
			// offline_guest_names are determined by profile names so it doesnt matter what we set the xuid as
			usersSignInInfo[dwUserIndex].xuid = primary_xuid + dwUserIndex;
		}

		usersSignInInfo[dwUserIndex].dwGuestNumber = dwUserIndex;
		usersSignInInfo[dwUserIndex].dwSponsorUserIndex = 0;
	}
}

void XUserSetup(DWORD dwUserIndex, XUID xuid, const char* userName, unsigned long xnaddr, unsigned long lanaddr, unsigned short baseport, const char* abEnet, const char* abOnline, bool online)
{
	// GFWL supports only 1 user logged in at the time
	if (dwUserIndex != 0)
		dwUserIndex = 0;

	if (online)
	{
		usersSignInInfo[dwUserIndex].dwInfoFlags |= XUSER_INFO_FLAG_LIVE_ENABLED;
		usersSignInInfo[dwUserIndex].UserSigninState = eXUserSigninState_SignedInToLive;
		strncpy_s(usersSignInInfo[dwUserIndex].szUserName, userName, strnlen_s(userName, XUSER_MAX_NAME_LENGTH));
		GetAchievements(xuid);
	}
	else
	{
		usersSignInInfo[dwUserIndex].UserSigninState = eXUserSigninState_SignedInLocally;
	}

	usersSignInInfo[dwUserIndex].xuid = xuid;
	usersSignInInfo[dwUserIndex].dwGuestNumber = 0;
	usersSignInInfo[dwUserIndex].dwSponsorUserIndex = 0;

	//fixes guest_signins
	XUserSetupGuests(xuid, online);

	gXnIpMgr.SetupLocalConnectionInfo(xnaddr, lanaddr, baseport, abEnet, abOnline);
}

void XUserSignInSetStatusChanged(DWORD dwUserIndex)
{
	signInChanged[dwUserIndex] = true;
}

void XUserSignOut(DWORD dwUserIndex)
{
	// clear user sign in
	SecureZeroMemory(&usersSignInInfo[dwUserIndex], sizeof(XUSER_SIGNIN_INFO));
	// clear connection data
	gXnIpMgr.UnregisterLocalConnectionInfo();
	signInChanged[dwUserIndex] = true;
}

// #5261: XUserGetXUID
int WINAPI XUserGetXUID(DWORD dwUserIndex, PXUID pXuid)
{
	if (pXuid == NULL)
		return ERROR_INVALID_PARAMETER;


	LIMITED_LOG(35, LOG_TRACE_XLIVE, "XUserGetXUID()");

	memset(pXuid, 0, sizeof(XUID));

	if (!UserSignedIn(dwUserIndex))
		return ERROR_NOT_LOGGED_ON;

	*pXuid = usersSignInInfo[dwUserIndex].xuid;
	return ERROR_SUCCESS;
}


// #5262: XUserGetSigninState
XUSER_SIGNIN_STATE WINAPI XUserGetSigninState(DWORD dwUserIndex)
{
	if (dwUserIndex != 0)
		dwUserIndex = 0;

	XUSER_SIGNIN_STATE ret;

	switch (usersSignInInfo[dwUserIndex].UserSigninState)
	{
	case eXUserSigninState_SignedInToLive:
		ret = eXUserSigninState_SignedInToLive;
		LIMITED_LOG(15, LOG_TRACE_XLIVE, "XUserGetSigninState() - Online");
		break;

	case eXUserSigninState_SignedInLocally:
		ret = eXUserSigninState_SignedInLocally;
		LIMITED_LOG(15, LOG_TRACE_XLIVE, "XUserGetSigninState() - Local profile");
		break;

	case eXUserSigninState_NotSignedIn:
		ret = eXUserSigninState_NotSignedIn;
		LIMITED_LOG(15, LOG_TRACE_XLIVE, "XUserGetSigninState() - Not signed in");
		break;

	default:
		ret = eXUserSigninState_NotSignedIn;
		break;
	}

	return ret;
}


// #5263: XUserGetName
DWORD WINAPI XUserGetName(DWORD dwUserIndex, LPSTR szUserName, DWORD cchUserName)
{
	static int print = 0;
	if (szUserName == NULL)
		return ERROR_INVALID_PARAMETER;

	if (dwUserIndex != 0)
		dwUserIndex = 0;

	if (usersSignInInfo[dwUserIndex].UserSigninState != eXUserSigninState_NotSignedIn)
	{
		strncpy_s(szUserName, cchUserName, usersSignInInfo[dwUserIndex].szUserName, strnlen_s(usersSignInInfo[dwUserIndex].szUserName, XUSER_MAX_NAME_LENGTH));
		LIMITED_LOG(15, LOG_TRACE_XLIVE, "XUserGetName  (userIndex = {}, userName = {}, cchUserName = {})", dwUserIndex, szUserName, cchUserName);

		return ERROR_SUCCESS;
	}
	
	return ERROR_NOT_LOGGED_ON;
}

// #5267: XUserGetSigninInfo
int WINAPI XUserGetSigninInfo(DWORD dwUserIndex, DWORD dwFlags, PXUSER_SIGNIN_INFO pSigninInfo)
{
	static int print = 0;

	if (dwUserIndex != 0)
		dwUserIndex = 0;

	if (pSigninInfo == NULL)
		return ERROR_INVALID_PARAMETER;

	if (print < 15)
	{
		LOG_TRACE_XLIVE("XUserGetSigninInfo( userIndex = {0:x}, dwFlags = {1:x})", dwUserIndex, dwFlags);
		print++;
	}

	memset(pSigninInfo, 0, sizeof(XUSER_SIGNIN_INFO));

	if (usersSignInInfo[dwUserIndex].UserSigninState != eXUserSigninState_NotSignedIn)
	{
		*pSigninInfo = usersSignInInfo[dwUserIndex];
		return ERROR_SUCCESS;
	}
	else
	{
		LIMITED_LOG(15, LOG_TRACE_XLIVE, "XUserGetSigninInfo() - Not signed in");
		return ERROR_NO_SUCH_USER;
	}
}

// #5264: XUserAreUsersFriends
int WINAPI XUserAreUsersFriends(DWORD dwUserIndex, DWORD * pXuids, DWORD dwXuidCount, DWORD * pResult, PXOVERLAPPED pOverlapped)
{
	LOG_TRACE_XLIVE("XUserAreUsersFriends");
	return ERROR_NOT_LOGGED_ON;
}

// #5265: XUserCheckPrivilege
DWORD WINAPI XUserCheckPrivilege(DWORD dwUserIndex, XPRIVILEGE_TYPE privilegeType, PBOOL pfResult)
{
	LIMITED_LOG(15, LOG_TRACE_XLIVE, "XUserCheckPrivilege  (userIndex = {}, privilegeType = {})", dwUserIndex, (DWORD)privilegeType);
	bool result_valid = pfResult != NULL;

	if (result_valid)
	{
		*pfResult = true;
	}

	return (result_valid ? ERROR_SUCCESS : ERROR_NOT_LOGGED_ON);
}


// #5279: XUserReadAchievementPicture
int WINAPI XUserReadAchievementPicture(DWORD dwUserIndex, DWORD dwTitleId, DWORD dwPictureId, BYTE* pbTextureBuffer, DWORD dwPitch, DWORD dwHeight, PXOVERLAPPED pOverlapped)
{
	return ERROR_INVALID_PARAMETER;
}


// 5282: XUserReadGamerPicture
DWORD WINAPI XUserReadGamerPicture(DWORD dwUserIndex, BOOL fSmall, PBYTE pbTextureBuffer, DWORD dwPitch, DWORD dwHeight, PXOVERLAPPED pOverlapped)
{
	Check_Overlapped(pOverlapped);

	// error - not ready??
	return 0x15;
}

// #5273: XUserReadGamerpictureByKey
int WINAPI XUserReadGamerpictureByKey(CONST PXUSER_DATA pGamercardPictureKey, BOOL fSmall, PBYTE pbTextureBuffer, DWORD dwPitch, DWORD dwHeight, PXOVERLAPPED pOverlapped)
{
	return 0;
}

// #5274: XUserAwardGamerPicture
DWORD WINAPI XUserAwardGamerPicture(DWORD dwUserIndex, DWORD dwPictureId, DWORD dwReserved, PXOVERLAPPED pXOverlapped)
{
	LOG_TRACE_XLIVE("XUserAwardGamerPicture");
	return 0;
}

// #5287 XUserResetStatsView
DWORD WINAPI XUserResetStatsView(DWORD dwUserIndex, DWORD dwViewId, PXOVERLAPPED pOverlapped)
{
	return 0;
}

// #5291 XUserResetStatsViewAllUsers
DWORD WINAPI XUserResetStatsViewAllUsers(DWORD dwViewId, PXOVERLAPPED pOverlapped)
{
	return 0;
}

// #5281: XUserReadStats
DWORD WINAPI XUserReadStats(DWORD dwTitleId, DWORD dwNumXuids, CONST XUID *pXuids, DWORD dwNumStatsSpecs, DWORD *pSpecs, DWORD *pcbResults, DWORD *pResults, PXOVERLAPPED pOverlapped)
{
	return ERROR_SUCCESS;
}

// #5284: XUserCreateStatsEnumeratorByRank
DWORD WINAPI XUserCreateStatsEnumeratorByRank(DWORD dwTitleId, DWORD dwRankStart, DWORD dwNumRows, DWORD dwNuStatSpec, CONST XUSER_STATS_SPEC* pSpecs, DWORD * pcbBuffer, PHANDLE ph)
{
	return 1;
}


// #5286: XUserCreateStatsEnumeratorByXuid
DWORD WINAPI XUserCreateStatsEnumeratorByXuid(DWORD dwTitleId, XUID XuidPivot, DWORD dwNumRows, DWORD dwNumStatsSpecs, CONST XUSER_STATS_SPEC* pSpecs, PDWORD pcbBuffer, PHANDLE ph)
{
	LOG_TRACE_XLIVE("XUserCreateStatsEnumeratorByXuid");
	return 1;
}

// #5285: XUserCreateStatsEnumeratorByRating
DWORD WINAPI XUserCreateStatsEnumeratorByRating(DWORD dwTitleId, LONGLONG i64Rating, DWORD dwNumRows, DWORD dwNumStatsSpecs, CONST XUSER_STATS_SPEC* pSpecs, PDWORD *pcbBuffer, PHANDLE ph)
{
	return 1;
}

std::wstring XProfileSettingIdToString(DWORD settingId)
{
	switch (settingId)
	{
	case 3: return L"XPROFILE_OPTION_CONTROLLER_VIBRATION";
	case 1: return L"XPROFILE_GAMERCARD_NXE";
	case 2: return L"XPROFILE_GAMER_YAXIS_INVERSION";
	case 24: return L"XPROFILE_GAMER_CONTROL_SENSITIVITY";

	case 4: return L"XPROFILE_GAMERCARD_ZONE";
	case 5: return L"XPROFILE_GAMERCARD_REGION";
	case 6: return L"XPROFILE_GAMERCARD_CRED";
	case 11: return L"XPROFILE_GAMERCARD_REP";
	case 71: return L"XPROFILE_GAMERCARD_YEARS";
	case 72: return L"XPROFILE_GAMERCARD_BOUBLES";
	case 15: return L"XPROFILE_GAMERCARD_PICTURE_KEY";
	case 64: return L"XPROFILE_GAMERCARD_NAME";
	case 65: return L"XPROFILE_GAMERCARD_LOCATION";
	case 17: return L"XPROFILE_GAMERCARD_MOTTO";
	case 18: return L"XPROFILE_GAMERCARD_TITLES_PLAYED";
	case 19: return L"XPROFILE_GAMERCARD_ACHIEVEMENTS_EARNED";

	case 21: return L"XPROFILE_GAMER_DIFFICULTY";

	case 29: return L"XPROFILE_GAMER_PREFERRED_COLOR_FIRST";
	case 30: return L"XPROFILE_GAMER_PREFERRED_COLOR_SECOND";

	case 34: return L"XPROFILE_GAMER_ACTION_AUTO_AIM";
	case 35: return L"XPROFILE_GAMER_ACTION_AUTO_CENTER";
	case 36: return L"XPROFILE_GAMER_ACTION_MOVEMENT_CONTROL";

	case 38: return L"XPROFILE_GAMER_RACE_TRANSMISSION";
	case 39: return L"XPROFILE_GAMER_RACE_CAMERA_LOCATION";
	case 40: return L"XPROFILE_GAMER_RACE_BRAKE_CONTROL";
	case 41: return L"XPROFILE_GAMER_RACE_ACCELERATOR_CONTROL";

	case 56: return L"XPROFILE_GAMERCARD_TITLE_CRED_EARNED";
	case 57: return L"XPROFILE_GAMERCARD_TITLE_ACHIEVEMENTS_EARNED";
	case 67: return L"XPROFILE_GAMERCARD_BIO";
	case 68: return L"XPROFILE_AVATAR_METADATA";

	case 0x3FFF: return L"XPROFILE_TITLE_SPECIFIC1";
	case 0x3FFE: return L"XPROFILE_TITLE_SPECIFIC2";
	case 0x3FFD: return L"XPROFILE_TITLE_SPECIFIC3";
	
	default: return L"<unknown>";
	}
}

// #5331: XUserReadProfileSettings
DWORD WINAPI XUserReadProfileSettings(DWORD dwTitleId, DWORD dwUserIndex, DWORD dwNumSettingIds,
	DWORD * pdwSettingIds, DWORD * pcbResults, XUSER_READ_PROFILE_SETTING_RESULT * pResults, PXOVERLAPPED pOverlapped)
{
	LOG_TRACE_XLIVE("XUserReadProfileSettings  (TitleId = {0}, UserIndex = {1}, NumSettingIds = {2}, pdwSettingIds = {3:p}, pcbResults = {4}, pResults = {5:p}, pOverlapped = {6:p})",
		dwTitleId, dwUserIndex, dwNumSettingIds, (void*)pdwSettingIds, *pcbResults, (void*)pResults, (void*)pOverlapped);

	if (dwUserIndex != 0)
		dwUserIndex = 0;

	if (!UserSignedOnline(dwUserIndex))
		return ERROR_NOT_FOUND;

	BOOL async = pOverlapped != NULL;

	if (pcbResults)
	{
		// find buffer size
		DWORD size = 0;

		std::wstring XUserProfileSettingsLog;

		for (DWORD lcv = 0; lcv < dwNumSettingIds; lcv++)
		{
			DWORD settingType, settingSize, settingId;

			settingType = (pdwSettingIds[lcv] >> 28) & 0x0F;
			settingSize = (pdwSettingIds[lcv] >> 16) & 0xFFF;
			settingId = (pdwSettingIds[lcv] >> 0) & 0x3FFF;

			XUserProfileSettingsLog += (lcv == 0 ? L"id = (" : L", id: (" ) + XProfileSettingIdToString(settingId) + L")";

			if (settingType == XUSER_DATA_TYPE_BINARY || settingType == XUSER_DATA_TYPE_UNICODE)
				size += settingSize;
		}

		if (XUserProfileSettingsLog.length() > 0)
			LOG_TRACE_XLIVE(XUserProfileSettingsLog);
		
		size += dwNumSettingIds * sizeof(XUSER_PROFILE_SETTING);
		size += sizeof(XUSER_READ_PROFILE_SETTING_RESULT);


		if (*pcbResults < size)
		{
			// when checking the buffer, we don't use an asynchronous operation
			// because we can calculate the size of the buffer without any I/O operation
			// we know the size of each field

			LOG_TRACE_XLIVE("- ERROR_INSUFFICIENT_BUFFER  (pcbResults = {})", *pcbResults);
			*pcbResults = size;

			return ERROR_INSUFFICIENT_BUFFER;
		}

		*pcbResults = size;
		SecureZeroMemory(pResults, *pcbResults);

		pResults->dwSettingsLen = dwNumSettingIds;
		pResults->pSettings = (XUSER_PROFILE_SETTING *)((BYTE *)pResults + sizeof(XUSER_READ_PROFILE_SETTING_RESULT));

		XUSER_PROFILE_SETTING* profileSettings = pResults->pSettings;
		BYTE* pSettingData = (BYTE*)profileSettings + (dwNumSettingIds * sizeof(XUSER_PROFILE_SETTING));

		// read data values
		for (DWORD lcv = 0; lcv < dwNumSettingIds; lcv++)
		{
			int settingType, settingSize, settingId;


			settingType = (pdwSettingIds[lcv] >> 28) & 0x0F;
			settingSize = (pdwSettingIds[lcv] >> 16) & 0xFFF;
			settingId = (pdwSettingIds[lcv] >> 0) & 0x3FFF;

			switch (settingId)
			{

			case 0x3FFF:
				LOG_TRACE_XLIVE("- XPROFILE_TITLE_SPECIFIC1");
				break;


			case 0x3FFE:
				LOG_TRACE_XLIVE("- XPROFILE_TITLE_SPECIFIC2");
				break;


			case 0x3FFD:
				LOG_TRACE_XLIVE("- XPROFILE_TITLE_SPECIFIC3");
				break;


			default:
				break;
			}

			switch (settingType)
			{
			case XUSER_DATA_TYPE_INT32:
				profileSettings->data.nData = 0;
				break;

			case XUSER_DATA_TYPE_INT64:
				profileSettings->data.i64Data = (LONGLONG)0;
				break;

			case XUSER_DATA_TYPE_DOUBLE:
				profileSettings->data.dblData = 0.0;
				break;

			case XUSER_DATA_TYPE_UNICODE:
				profileSettings->data.string.cbData = settingSize;
				profileSettings->data.string.pwszData = (LPWSTR)pSettingData;
				ZeroMemory(profileSettings->data.string.pwszData, settingSize);
				break;

			case XUSER_DATA_TYPE_FLOAT:
				profileSettings->data.fData = 0.0f;
				break;

			case XUSER_DATA_TYPE_BINARY:
				profileSettings->data.binary.cbData = settingSize;
				profileSettings->data.binary.pbData = pSettingData;
				ZeroMemory(profileSettings->data.binary.pbData, settingSize);
				break;

			case XUSER_DATA_TYPE_DATETIME:
				SYSTEMTIME systemTime;
				GetSystemTime(&systemTime);
				SystemTimeToFileTime(&systemTime, &profileSettings->data.ftData);

				break;

			case XUSER_DATA_TYPE_NULL:
			default:
				break;
			}

			profileSettings->user.dwUserIndex = 0;
			profileSettings->data.type = settingType;
			profileSettings->dwSettingId = pdwSettingIds[lcv];

			if (settingType == XUSER_DATA_TYPE_BINARY || settingType == XUSER_DATA_TYPE_UNICODE)
				pSettingData += settingSize;

			profileSettings++;
		}
	}
	else
	{
		return ERROR_INVALID_PARAMETER;
	}

	LOG_TRACE_XLIVE("- pcbResults = {}", *pcbResults);

	if (async)
	{
		pOverlapped->InternalLow = ERROR_SUCCESS;
		pOverlapped->InternalHigh = *pcbResults;
		pOverlapped->dwExtendedError = 0;

		Check_Overlapped(pOverlapped);

		return ERROR_IO_PENDING;
	}

	return ERROR_SUCCESS;
}


// #5339: XUserReadProfileSettingsByXuid
DWORD WINAPI XUserReadProfileSettingsByXuid(
	DWORD dwTitleId,
	DWORD dwUserIndexRequester,
	DWORD dwNumFor,
	const XUID *pxuidFor,
	DWORD dwNumSettingIds,
	const DWORD *pdwSettingIds,
	DWORD *pcbResults,
	PXUSER_READ_PROFILE_SETTING_RESULT pResults,
	PXOVERLAPPED pOverlapped
)
{
	return ERROR_NOT_FOUND;
}


// #5337: XUserWriteProfileSettings
DWORD WINAPI XUserWriteProfileSettings(DWORD dwUserIndex, DWORD dwNumSettings, const PXUSER_PROFILE_SETTING pSettings, PXOVERLAPPED pOverlapped)
{
	return ERROR_SUCCESS;
}
