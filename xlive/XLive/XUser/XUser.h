#pragma once

typedef struct _XUSER_DATA
{
	BYTE                                type;

	union
	{
		LONG                            nData;     // XUSER_DATA_TYPE_INT32
		LONGLONG                        i64Data;   // XUSER_DATA_TYPE_INT64
		double                          dblData;   // XUSER_DATA_TYPE_DOUBLE
		struct                                     // XUSER_DATA_TYPE_UNICODE
		{
			DWORD                       cbData;    // Includes null-terminator
			LPWSTR                      pwszData;
		} string;
		FLOAT                           fData;     // XUSER_DATA_TYPE_FLOAT
		struct                                     // XUSER_DATA_TYPE_BINARY
		{
			DWORD                       cbData;
			PBYTE                       pbData;
		} binary;
		FILETIME                        ftData;    // XUSER_DATA_TYPE_DATETIME
	};
} XUSER_DATA, *PXUSER_DATA;

typedef enum _XPRIVILEGE_TYPE
{
	XPRIVILEGE_MULTIPLAYER_SESSIONS = 254, // on|off

	XPRIVILEGE_COMMUNICATIONS = 252, // on (communicate w/everyone) | off (check _FO)
	XPRIVILEGE_COMMUNICATIONS_FRIENDS_ONLY = 251, // on (communicate w/friends only) | off (blocked)

	XPRIVILEGE_PROFILE_VIEWING = 249, // on (viewing allowed) | off (check _FO)
	XPRIVILEGE_PROFILE_VIEWING_FRIENDS_ONLY = 248, // on (view friend's only) | off (no details)

	XPRIVILEGE_USER_CREATED_CONTENT = 247, // on (allow viewing of UCC) | off (check _FO)
	XPRIVILEGE_USER_CREATED_CONTENT_FRIENDS_ONLY = 246, // on (view UCC from friends only) | off (blocked)

	XPRIVILEGE_PURCHASE_CONTENT = 245, // on (allow purchase) | off (blocked)

	XPRIVILEGE_PRESENCE = 244, // on (share presence info) | off (check _FO)
	XPRIVILEGE_PRESENCE_FRIENDS_ONLY = 243, // on (share w/friends only | off (don't share)

	XPRIVILEGE_TRADE_CONTENT = 238, // on (allow trading) | off (blocked)

	XPRIVILEGE_VIDEO_COMMUNICATIONS = 235, // on (communicate w/everyone) | off (check _FO)
	XPRIVILEGE_VIDEO_COMMUNICATIONS_FRIENDS_ONLY = 234, // on (communicate w/friends only) | off (blocked)

	XPRIVILEGE_MULTIPLAYER_DEDICATED_SERVER = 226, // on (allow) | off (disallow)
} XPRIVILEGE_TYPE;

typedef ULONGLONG XUID;
typedef XUID *PXUID;

#define INVALID_XUID                    ((XUID) 0)

#define XUSER_NAME_SIZE                 16
#define XUSER_MAX_NAME_LENGTH           (XUSER_NAME_SIZE - 1)

#define XUSER_GET_SIGNIN_INFO_ONLINE_XUID_ONLY      0x00000002
#define XUSER_GET_SIGNIN_INFO_OFFLINE_XUID_ONLY     0x00000001

#define XUSER_INFO_FLAG_LIVE_ENABLED    0x00000001
#define XUSER_INFO_FLAG_GUEST           0x00000002

typedef enum _XUSER_SIGNIN_STATE
{
	eXUserSigninState_NotSignedIn,
	eXUserSigninState_SignedInLocally,
	eXUserSigninState_SignedInToLive
} XUSER_SIGNIN_STATE;

typedef struct _XUSER_SIGNIN_INFO
{
	XUID                 xuid;
	DWORD                dwInfoFlags;
	XUSER_SIGNIN_STATE   UserSigninState;
	DWORD                dwGuestNumber;
	DWORD                dwSponsorUserIndex;
	CHAR                 szUserName[XUSER_NAME_SIZE];
} XUSER_SIGNIN_INFO, *PXUSER_SIGNIN_INFO;

extern bool signInChanged[4];
extern XUSER_SIGNIN_INFO usersSignInInfo[4];

void XUserSignInSetStatusChanged(DWORD dwUserIndex);
void XUserSetup(DWORD dwUserIndex, XUID xuid, char* userName, unsigned long xnaddr, unsigned long lanaddr, unsigned short baseport, const char* abEnet, const char* abOnline, bool online);
void XUserSignOut(DWORD dwUserIndex);

bool SignInStatusChanged();
bool UserSignedIn(DWORD dwUserIndex);
bool UserSignedOnline(DWORD dwUserIndex);
bool UserSignedInLocally(DWORD dwUserIndex);

int WINAPI XUserGetXUID(DWORD dwUserIndex, PXUID pXuid);
XUSER_SIGNIN_STATE WINAPI XUserGetSigninState(DWORD dwUserIndex);