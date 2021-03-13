#include "stdafx.h"
#include "XUser.h"

#include "Globals.h"
#include "XLive\xbox\xbox.h"

WCHAR strw_XUser[8192];
extern void Check_Overlapped(PXOVERLAPPED pOverlapped);

bool signInChanged[4];
XUSER_SIGNIN_INFO usersSignInInfo[4];

bool signInStatusChanged()
{
	for (int i = 0; i < 4; i++)
	{
		if (signInChanged[i])
			return true;
	}
	return false;
}

bool userSignedIn(DWORD dwUserIndex)
{
	if (usersSignInInfo[dwUserIndex].UserSigninState == eXUserSigninState_SignedInLocally
		|| usersSignInInfo[dwUserIndex].UserSigninState == eXUserSigninState_SignedInToLive)
		return true;

	return false;
}


bool userSignedInLocally(DWORD dwUserIndex)
{
	if (usersSignInInfo[dwUserIndex].UserSigninState == eXUserSigninState_SignedInLocally)
		return true;

	return false;
}

bool userSignedOnline(DWORD dwUserIndex)
{
	if (usersSignInInfo[dwUserIndex].UserSigninState == eXUserSigninState_SignedInToLive)
		return true;

	return false;
}

void XUserSetup(DWORD dwUserIndex, XUID xuid, char* userName, bool online)
{
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
		usersSignInInfo[dwUserIndex].UserSigninState = eXUserSigninState_SignedInLocally;

	usersSignInInfo[dwUserIndex].xuid = xuid;
	usersSignInInfo[dwUserIndex].dwGuestNumber = 0;
	usersSignInInfo[dwUserIndex].dwSponsorUserIndex = 0;

	signInChanged[dwUserIndex] = true;
}

void XUserSignOut(DWORD dwUserIndex)
{
	SecureZeroMemory(&usersSignInInfo[dwUserIndex], sizeof(XUSER_SIGNIN_INFO));
	signInChanged[dwUserIndex] = true;
}

// #5261: XUserGetXUID
int WINAPI XUserGetXUID(DWORD dwUserIndex, PXUID pXuid)
{
	if (pXuid == NULL)
		return ERROR_INVALID_PARAMETER;

	if (dwUserIndex != 0)
		dwUserIndex = 0;

	static int print = 0;
	if (print < 15)
	{
		LOG_TRACE_XLIVE("XUserGetXUID()");

		print++;
	}

	memset(pXuid, 0, sizeof(XUID));

	if (!userSignedIn(dwUserIndex))
		return ERROR_NOT_LOGGED_ON;

	*pXuid = usersSignInInfo[dwUserIndex].xuid;
	return ERROR_SUCCESS;
}


// #5262: XUserGetSigninState
XUSER_SIGNIN_STATE WINAPI XUserGetSigninState(DWORD dwUserIndex)
{
	static int print = 0;

	if (dwUserIndex != 0)
		dwUserIndex = 0;

	XUSER_SIGNIN_STATE ret;

	switch (usersSignInInfo[dwUserIndex].UserSigninState)
	{
	case eXUserSigninState_SignedInToLive:
		ret = eXUserSigninState_SignedInToLive;
		if (print < 15) LOG_TRACE_XLIVE("XUserGetSigninState() - Online");
		break;

	case eXUserSigninState_SignedInLocally:
		ret = eXUserSigninState_SignedInLocally;
		if (print < 15) LOG_TRACE_XLIVE("XUserGetSigninState() - Local profile");
		break;

	case eXUserSigninState_NotSignedIn:
		ret = eXUserSigninState_NotSignedIn;
		if (print < 15) LOG_TRACE_XLIVE("XUserGetSigninState() - Not signed in");
		break;

	default:
		ret = eXUserSigninState_NotSignedIn;
	}

	print++;

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
		strncpy_s(szUserName, cchUserName, usersSignInInfo[dwUserIndex].szUserName, strnlen_s(usersSignInInfo[dwUserIndex].szUserName, XUSER_MAX_NAME_LENGTH));
	else
		return ERROR_NOT_LOGGED_ON;

	if (print < 15)
	{
		LOG_TRACE_XLIVE("XUserGetName  (userIndex = {0}, userName = {1}, cchUserName = {2})", dwUserIndex, szUserName, cchUserName);
		print++;
	}

	return ERROR_SUCCESS;
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
		if (print < 15) LOG_TRACE_XLIVE("XUserGetSigninInfo() - Not signed in");
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
	static int print = 0;

	if (print < 15)
	{
		if (privilegeType == XPRIVILEGE_MULTIPLAYER_SESSIONS)
			LOG_TRACE_XLIVE("- MULTIPLAYER_SESSIONS");

		else if (privilegeType == XPRIVILEGE_COMMUNICATIONS)
		{
			LOG_TRACE_XLIVE(" - COMMUNICATIONS");
		}

		else if (privilegeType == XPRIVILEGE_PROFILE_VIEWING)
			LOG_TRACE_XLIVE("- PROFILE_VIEWING");

		else if (privilegeType == XPRIVILEGE_PRESENCE)
			LOG_TRACE_XLIVE("- PRESCENCE");

		else
			LOG_TRACE_XLIVE("- UNKNOWN");

		LOG_TRACE_XLIVE("XUserCheckPrivilege  (userIndex = {0}, privilegeType = {1}, pfResult = {2:p})",
			dwUserIndex, privilegeType, (void*)pfResult);

		print++;
	}

	if (pfResult) {
		*pfResult = TRUE;
		return ERROR_SUCCESS;
	}

	return ERROR_NOT_LOGGED_ON;
}


// #5279: XUserReadAchievementPicture
int WINAPI XUserReadAchievementPicture(DWORD dwUserIndex, DWORD dwTitleId, DWORD dwPictureId, BYTE* pbTextureBuffer, DWORD dwPitch, DWORD dwHeight, PXOVERLAPPED pOverlapped)
{
	LOG_TRACE_XLIVE("XUserReadAchievementPicture");

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
	if (pcbResults)
	{
		// return size
		if (*pcbResults == 0)
		{
			*pcbResults = 4;

			if (pOverlapped)
			{
				pOverlapped->InternalLow = ERROR_INSUFFICIENT_BUFFER;
				pOverlapped->dwExtendedError = ERROR_INSUFFICIENT_BUFFER;


				Check_Overlapped(pOverlapped);

				return ERROR_IO_PENDING;
			}

			else
				return ERROR_INSUFFICIENT_BUFFER;
		}
	}


	if (pResults)
		*pResults = 0;


	if (pOverlapped)
	{
		pOverlapped->InternalLow = -1;
		pOverlapped->dwExtendedError = -1;


		Check_Overlapped(pOverlapped);

		return ERROR_IO_PENDING;
	}


	return ERROR_SUCCESS;
}

// #5284: XUserCreateStatsEnumeratorByRank
DWORD WINAPI XUserCreateStatsEnumeratorByRank(DWORD dwTitleId, DWORD dwRankStart, DWORD dwNumRows, DWORD dwNuStatSpec, CONST XUSER_STATS_SPEC* pSpecs, DWORD * pcbBuffer, PHANDLE ph)
{
	LOG_TRACE_XLIVE("XUserCreateStatsEnumeratorByRank");

	if (pcbBuffer)
		*pcbBuffer = 0;

	if (ph)
		*ph = 0;

	return 1;
}


// #5286: XUserCreateStatsEnumeratorByXuid
DWORD WINAPI XUserCreateStatsEnumeratorByXuid(DWORD dwTitleId, XUID XuidPivot, DWORD dwNumRows, DWORD dwNumStatsSpecs, CONST XUSER_STATS_SPEC* pSpecs, PDWORD pcbBuffer, PHANDLE ph)
{
	LOG_TRACE_XLIVE("XUserCreateStatsEnumeratorByXuid");

	if (pcbBuffer)
		*pcbBuffer = 0;

	if (ph)
		*ph = 0;

	return 1;
}

// #5285: XUserCreateStatsEnumeratorByRating
DWORD WINAPI XUserCreateStatsEnumeratorByRating(DWORD dwTitleId, LONGLONG i64Rating, DWORD dwNumRows, DWORD dwNumStatsSpecs, CONST XUSER_STATS_SPEC* pSpecs, PDWORD *pcbBuffer, PHANDLE ph)
{
	if (pcbBuffer)
		*pcbBuffer = 0;

	if (ph)
		*ph = 0;

	return 1;
}

// #5331: XUserReadProfileSettings
DWORD WINAPI XUserReadProfileSettings(DWORD dwTitleId, DWORD dwUserIndex, DWORD dwNumSettingIds,
	DWORD * pdwSettingIds, DWORD * pcbResults, XUSER_READ_PROFILE_SETTING_RESULT * pResults, PXOVERLAPPED pOverlapped)
{
	LOG_TRACE_XLIVE("XUserReadProfileSettings  (TitleId = {0}, UserIndex = {1}, NumSettingIds = {2}, pdwSettingIds = {3:p}, pcbResults = {4}, pResults = {5:p}, pOverlapped = {6:p})",
		dwTitleId, dwUserIndex, dwNumSettingIds, (void*)pdwSettingIds, *pcbResults, (void*)pResults, (void*)pOverlapped);

	if (dwUserIndex != 0)
		dwUserIndex = 0;

	BOOL async;

	if (pOverlapped)
		async = TRUE;

	else
		async = FALSE;



	if (pcbResults)
	{
		// find buffer size
		if (*pcbResults == 0)
		{
			int size;


			size = 0;
			for (DWORD lcv = 0; lcv < dwNumSettingIds; lcv++)
			{
				int settingType, settingSize, settingId;

				settingType = (pdwSettingIds[lcv] >> 28) & 0x0F;
				settingSize = (pdwSettingIds[lcv] >> 16) & 0xFFF;
				settingId = (pdwSettingIds[lcv] >> 0) & 0x3FFF;


				wsprintf(strw_XUser, L"- Settings %d: %X  (Type = %X, Size = %d", lcv + 1,
					pdwSettingIds[lcv], settingType, settingSize);


				switch (settingId)
				{
				case 3: wsprintf(strw_XUser, L"%s, id = XPROFILE_OPTION_CONTROLLER_VIBRATION)", strw_XUser); break;
				case 1: wsprintf(strw_XUser, L"%s, id = XPROFILE_GAMERCARD_NXE)", strw_XUser); break;
				case 2: wsprintf(strw_XUser, L"%s, id = XPROFILE_GAMER_YAXIS_INVERSION)", strw_XUser); break;
				case 24: wsprintf(strw_XUser, L"%s, id = XPROFILE_GAMER_CONTROL_SENSITIVITY)", strw_XUser); break;

				case 4: wsprintf(strw_XUser, L"%s, id = XPROFILE_GAMERCARD_ZONE)", strw_XUser); break;
				case 5: wsprintf(strw_XUser, L"%s, id = XPROFILE_GAMERCARD_REGION)", strw_XUser); break;
				case 6: wsprintf(strw_XUser, L"%s, id = XPROFILE_GAMERCARD_CRED)", strw_XUser); break;
				case 11: wsprintf(strw_XUser, L"%s, id = XPROFILE_GAMERCARD_REP)", strw_XUser); break;
				case 71: wsprintf(strw_XUser, L"%s, id = XPROFILE_GAMERCARD_YEARS)", strw_XUser); break;
				case 72: wsprintf(strw_XUser, L"%s, id = XPROFILE_GAMERCARD_BOUBLES)", strw_XUser); break;
				case 15: wsprintf(strw_XUser, L"%s, id = XPROFILE_GAMERCARD_PICTURE_KEY)", strw_XUser); break;
				case 64: wsprintf(strw_XUser, L"%s, id = XPROFILE_GAMERCARD_NAME)", strw_XUser); break;
				case 65: wsprintf(strw_XUser, L"%s, id = XPROFILE_GAMERCARD_LOCATION)", strw_XUser); break;
				case 17: wsprintf(strw_XUser, L"%s, id = XPROFILE_GAMERCARD_MOTTO)", strw_XUser); break;
				case 18: wsprintf(strw_XUser, L"%s, id = XPROFILE_GAMERCARD_TITLES_PLAYED)", strw_XUser); break;
				case 19: wsprintf(strw_XUser, L"%s, id = XPROFILE_GAMERCARD_ACHIEVEMENTS_EARNED)", strw_XUser); break;

				case 21: wsprintf(strw_XUser, L"%s, id = XPROFILE_GAMER_DIFFICULTY)", strw_XUser); break;

				case 29: wsprintf(strw_XUser, L"%s, id = XPROFILE_GAMER_PREFERRED_COLOR_FIRST)", strw_XUser); break;
				case 30: wsprintf(strw_XUser, L"%s, id = XPROFILE_GAMER_PREFERRED_COLOR_SECOND)", strw_XUser); break;

				case 34: wsprintf(strw_XUser, L"%s, id = XPROFILE_GAMER_ACTION_AUTO_AIM)", strw_XUser); break;
				case 35: wsprintf(strw_XUser, L"%s, id = XPROFILE_GAMER_ACTION_AUTO_CENTER)", strw_XUser); break;
				case 36: wsprintf(strw_XUser, L"%s, id = XPROFILE_GAMER_ACTION_MOVEMENT_CONTROL)", strw_XUser); break;

				case 38: wsprintf(strw_XUser, L"%s, id = XPROFILE_GAMER_RACE_TRANSMISSION)", strw_XUser); break;
				case 39: wsprintf(strw_XUser, L"%s, id = XPROFILE_GAMER_RACE_CAMERA_LOCATION)", strw_XUser); break;
				case 40: wsprintf(strw_XUser, L"%s, id = XPROFILE_GAMER_RACE_BRAKE_CONTROL)", strw_XUser); break;
				case 41: wsprintf(strw_XUser, L"%s, id = XPROFILE_GAMER_RACE_ACCELERATOR_CONTROL)", strw_XUser); break;

				case 56: wsprintf(strw_XUser, L"%s, id = XPROFILE_GAMERCARD_TITLE_CRED_EARNED)", strw_XUser); break;
				case 57: wsprintf(strw_XUser, L"%s, id = XPROFILE_GAMERCARD_TITLE_ACHIEVEMENTS_EARNED)", strw_XUser); break;
				case 67: wsprintf(strw_XUser, L"%s, id = XPROFILE_GAMERCARD_BIO)", strw_XUser); break;
				case 68: wsprintf(strw_XUser, L"%s, id = XPROFILE_AVATAR_METADATA)", strw_XUser); break;

				case 0x3FFF: wsprintf(strw_XUser, L"%s, id = XPROFILE_TITLE_SPECIFIC1)", strw_XUser); break;
				case 0x3FFE: wsprintf(strw_XUser, L"%s, id = XPROFILE_TITLE_SPECIFIC2)", strw_XUser); break;
				case 0x3FFD: wsprintf(strw_XUser, L"%s, id = XPROFILE_TITLE_SPECIFIC3)", strw_XUser); break;
				default: wsprintf(strw_XUser, L"%s, id = Unknown)", strw_XUser); break;
				}



				LOG_TRACE_XLIVE(strw_XUser);
				size += settingSize;
			}


			*pcbResults = size;
			*pcbResults += dwNumSettingIds * sizeof(XUSER_PROFILE_SETTING);
			*pcbResults += sizeof(XUSER_READ_PROFILE_SETTING_RESULT);


			LOG_TRACE_XLIVE("- ERROR_INSUFFICIENT_BUFFER  (pcbResults = {})", *pcbResults);

			if (async)
			{
				pOverlapped->InternalLow = ERROR_INSUFFICIENT_BUFFER;
				pOverlapped->InternalHigh = *pcbResults;
				pOverlapped->dwExtendedError = ERROR_INSUFFICIENT_BUFFER;


				Check_Overlapped(pOverlapped);

				return ERROR_IO_PENDING;
			}


			return ERROR_INSUFFICIENT_BUFFER;
		}



		SecureZeroMemory(pResults, *pcbResults);


		pResults->dwSettingsLen = dwNumSettingIds;
		pResults->pSettings = (XUSER_PROFILE_SETTING *)((BYTE *)pResults + sizeof(XUSER_READ_PROFILE_SETTING_RESULT));



		XUSER_PROFILE_SETTING *ptr = pResults->pSettings;
		BYTE *pSettingData = (BYTE *)ptr + dwNumSettingIds * sizeof(XUSER_PROFILE_SETTING);


		// read data values
		for (DWORD lcv = 0; lcv < dwNumSettingIds; lcv++)
		{
			int settingType, settingSize, settingId;


			settingType = (pdwSettingIds[lcv] >> 28) & 0x0F;
			settingSize = (pdwSettingIds[lcv] >> 16) & 0xFFF;
			settingId = (pdwSettingIds[lcv] >> 0) & 0x3FFF;


			//Local_Storage_W( dwUserIndex, strw );

			wcscat(strw_XUser, L"\\Offline\\");
			//CreateDirectory( strw, NULL );


			switch (settingId)
			{
			case 0x3FFF:
				LOG_TRACE_XLIVE("- XPROFILE_TITLE_SPECIFIC1");

				wcscat(strw_XUser, L"Title1.dat");
				break;


			case 0x3FFE:
				LOG_TRACE_XLIVE("- XPROFILE_TITLE_SPECIFIC2");

				wcscat(strw_XUser, L"Title2.dat");
				break;


			case 0x3FFD:
				LOG_TRACE_XLIVE("- XPROFILE_TITLE_SPECIFIC3");

				wcscat(strw_XUser, L"Title3.dat");
				break;


			default:
				wcscat(strw_XUser, L"Settings.txt");
				break;
			}




			FILE *fp;

			fp = _wfopen(strw_XUser, L"rb");
			if (!fp)
			{
				LOG_TRACE_XLIVE(L"- Not found: {}", strw_XUser);


				ptr->source = XSOURCE_NO_VALUE;
			}

			else
			{
				LOG_TRACE_XLIVE(L"- Found: {}", strw_XUser);


				if (settingType == 6)
				{
					fread(&ptr->data.binary.cbData, 1, 4, fp);
					fread(pSettingData, 1, ptr->data.binary.cbData, fp);


					ptr->data.binary.pbData = pSettingData;
					ptr->source = XSOURCE_TITLE;
				}


				fclose(fp);
			}


			ptr->data.type = settingType;
			ptr->dwSettingId = pdwSettingIds[lcv];
			ptr->user.dwUserIndex = 0;


			pSettingData += settingSize;
			ptr++;
		}
	}



	LOG_TRACE_XLIVE("- pcbResults = {}", *pcbResults);

	if (async)
	{
		pOverlapped->InternalLow = ERROR_SUCCESS;
		pOverlapped->InternalHigh = *pcbResults;
		pOverlapped->dwExtendedError = ERROR_SUCCESS;


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
	LOG_TRACE_XLIVE("XUserReadProfileSettingsByXuid");


	Check_Overlapped(pOverlapped);

	return ERROR_NOT_FOUND;
}


// #5337: XUserWriteProfileSettings
DWORD WINAPI XUserWriteProfileSettings(DWORD dwUserIndex, DWORD dwNumSettings, const PXUSER_PROFILE_SETTING pSettings, PXOVERLAPPED pOverlapped)
{
	LOG_TRACE_XLIVE("XUserWriteProfileSettings  (dwUserIndex = {0}, dwNumSettings = {1}, pSettings = {2:p}, pOverlapped = {3:p})",
		dwUserIndex, dwNumSettings, (void*)pSettings, (void*)pOverlapped);


	for (DWORD lcv = 0; lcv < dwNumSettings; lcv++)
	{
		int type, size, id;

		type = (pSettings[lcv].dwSettingId >> 28) & 0x0F;
		size = (pSettings[lcv].dwSettingId >> 16) & 0xFFF;
		id = (pSettings[lcv].dwSettingId >> 0) & 0x3FFF;



		LOG_TRACE_XLIVE("- [{0}] source = {1}, id = {2:x}, type = {3}, size = {4:x}, sub-id = {5:x}, type2 = {6}",
			lcv,
			pSettings[lcv].source,
			pSettings[lcv].dwSettingId,
			type,
			size,
			id,
			pSettings[lcv].data.type);



		//Local_Storage_W( dwUserIndex, strw );

		wcscat(strw_XUser, L"\\Offline\\");
		//CreateDirectory( strw, NULL );



		switch (id)
		{
		case 0x3FFF:
			LOG_TRACE_XLIVE("- XPROFILE_TITLE_SPECIFIC1  (cbData = {:x})", pSettings[lcv].data.binary.cbData);

			wcscat(strw_XUser, L"Title1.dat");
			break;


		case 0x3FFE:
			LOG_TRACE_XLIVE("- XPROFILE_TITLE_SPECIFIC2  (cbData = {:x})", pSettings[lcv].data.binary.cbData);

			wcscat(strw_XUser, L"Title2.dat");
			break;


		case 0x3FFD:
			LOG_TRACE_XLIVE("- XPROFILE_TITLE_SPECIFIC3  (cbData = {:x})", pSettings[lcv].data.binary.cbData);

			wcscat(strw_XUser, L"Title3.dat");
			break;
		}


		FILE *fp;

		fp = _wfopen(strw_XUser, L"wb");
		if (!fp) continue;


		if (type == 6)
		{
			fwrite(&pSettings[lcv].data.binary.cbData, 1, 4, fp);
			fwrite(pSettings[lcv].data.binary.pbData, 1, pSettings[lcv].data.binary.cbData, fp);
		}


		fclose(fp);
	}



	if (pOverlapped)
	{
		pOverlapped->InternalLow = ERROR_SUCCESS;
		pOverlapped->dwExtendedError = ERROR_SUCCESS;
		pOverlapped->InternalLow = 0;


		Check_Overlapped(pOverlapped);

		return ERROR_IO_PENDING;
	}


	return ERROR_SUCCESS;
}
