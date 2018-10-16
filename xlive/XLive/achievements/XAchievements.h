#pragma once
extern int achieveinit;
extern HANDLE g_dwFakeAchievementContent;

#define XACHIEVEMENT_TYPE_COMPLETION            1
#define XACHIEVEMENT_TYPE_LEVELING              2
#define XACHIEVEMENT_TYPE_UNLOCK                3
#define XACHIEVEMENT_TYPE_EVENT                 4
#define XACHIEVEMENT_TYPE_TOURNAMENT            5
#define XACHIEVEMENT_TYPE_CHECKPOINT            6
#define XACHIEVEMENT_TYPE_OTHER                 7

#define XACHIEVEMENT_DETAILS_ALL                0xFFFFFFFF
#define XACHIEVEMENT_DETAILS_LABEL              0x00000001
#define XACHIEVEMENT_DETAILS_DESCRIPTION        0x00000002
#define XACHIEVEMENT_DETAILS_UNACHIEVED         0x00000004
#define XACHIEVEMENT_DETAILS_TFC                0x00000020

#define XACHIEVEMENT_DETAILS_MASK_TYPE          0x00000007
#define XACHIEVEMENT_DETAILS_SHOWUNACHIEVED     0x00000008
#define XACHIEVEMENT_DETAILS_ACHIEVED_ONLINE    0x00010000
#define XACHIEVEMENT_DETAILS_ACHIEVED           0x00020000

#define AchievementType(dwFlags)           (dwFlags & XACHIEVEMENT_DETAILS_MASK_TYPE)
#define AchievementShowUnachieved(dwFlags) (dwFlags & XACHIEVEMENT_DETAILS_SHOWUNACHIEVED ? TRUE : FALSE)
#define AchievementEarnedOnline(dwFlags)   (dwFlags & XACHIEVEMENT_DETAILS_ACHIEVED_ONLINE ? TRUE : FALSE)
#define AchievementEarned(dwFlags)         (dwFlags & XACHIEVEMENT_DETAILS_ACHIEVED ? TRUE : FALSE)

#define XACHIEVEMENT_MAX_LABEL_LENGTH   33
#define XACHIEVEMENT_MAX_DESC_LENGTH    101
#define XACHIEVEMENT_MAX_UNACH_LENGTH   101

#define XACHIEVEMENT_SIZE_BASE          (sizeof(XACHIEVEMENT_DETAILS))
#define XACHIEVEMENT_SIZE_STRINGS       (sizeof(WCHAR) * (XACHIEVEMENT_MAX_LABEL_LENGTH  + XACHIEVEMENT_MAX_DESC_LENGTH + XACHIEVEMENT_MAX_UNACH_LENGTH))
#define XACHIEVEMENT_SIZE_FULL          (XACHIEVEMENT_SIZE_BASE + XACHIEVEMENT_SIZE_STRINGS)

#define XACHIEVEMENT_INVALID_ID         ((DWORD)0xFFFFFFFF)

typedef struct
{
	DWORD                               dwId;
	LPWSTR                              pwszLabel;
	LPWSTR                              pwszDescription;
	LPWSTR                              pwszUnachieved;
	DWORD                               dwImageId;
	DWORD                               dwCred;
	FILETIME                            ftAchieved;
	DWORD                               dwFlags;
} XACHIEVEMENT_DETAILS, *PXACHIEVEMENT_DETAILS;