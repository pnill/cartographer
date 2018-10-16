#pragma once
#define XNID(Version, Area, Index)      (DWORD)( (WORD)(Area) << 25 | (WORD)(Version) << 16 | (WORD)(Index))
#define XNID_VERSION(msgid)             (((msgid) >> 16) & 0x1FF)
#define XNID_AREA(msgid)                (((msgid) >> 25) & 0x3F)
#define XNID_INDEX(msgid)               ((msgid) & 0xFFFF)


//
// Notification Areas
//

#define XNOTIFY_SYSTEM                  (0x00000001)
#define XNOTIFY_LIVE                    (0x00000002)
#define XNOTIFY_FRIENDS                 (0x00000004)
#define XNOTIFY_CUSTOM                  (0x00000008)
#define XNOTIFY_XMP                     (0x00000020)
#define XNOTIFY_MSGR                    (0x00000040)
#define XNOTIFY_PARTY                   (0x00000080)
#define XNOTIFY_ALL                     (XNOTIFY_SYSTEM | XNOTIFY_LIVE | XNOTIFY_FRIENDS | XNOTIFY_CUSTOM | XNOTIFY_XMP | XNOTIFY_MSGR | XNOTIFY_PARTY)

//
// Bit numbers of each area (bit 0 is the least significant bit)
//

#define _XNAREA_SYSTEM                  (0)
#define _XNAREA_LIVE                    (1)
#define _XNAREA_FRIENDS                 (2)
#define _XNAREA_CUSTOM                  (3)
#define _XNAREA_XMP                     (5)
#define _XNAREA_MSGR                    (6)
#define _XNAREA_PARTY                   (7)

//
// System notifications
//

#define XN_SYS_FIRST                    XNID(0, _XNAREA_SYSTEM, 0x0001)
#define XN_SYS_UI                       XNID(0, _XNAREA_SYSTEM, 0x0009)
#define XN_SYS_SIGNINCHANGED            XNID(0, _XNAREA_SYSTEM, 0x000a)
#define XN_SYS_STORAGEDEVICESCHANGED    XNID(0, _XNAREA_SYSTEM, 0x000b)
#define XN_SYS_PROFILESETTINGCHANGED    XNID(0, _XNAREA_SYSTEM, 0x000e)
#define XN_SYS_MUTELISTCHANGED          XNID(0, _XNAREA_SYSTEM, 0x0011)
#define XN_SYS_INPUTDEVICESCHANGED      XNID(0, _XNAREA_SYSTEM, 0x0012)
#define XN_SYS_INPUTDEVICECONFIGCHANGED XNID(1, _XNAREA_SYSTEM, 0x0013)
#define XN_SYS_PLAYTIMERNOTICE          XNID(3, _XNAREA_SYSTEM, 0x0015)
#define XN_SYS_AVATARCHANGED            XNID(4, _XNAREA_SYSTEM, 0x0017)
#define XN_SYS_NUIHARDWARESTATUSCHANGED XNID(6, _XNAREA_SYSTEM, 0x0019)
#define XN_SYS_NUIPAUSE                 XNID(6, _XNAREA_SYSTEM, 0x001a)
#define XN_SYS_NUIUIAPPROACH            XNID(6, _XNAREA_SYSTEM, 0x001b)
#define XN_SYS_DEVICEREMAP              XNID(6, _XNAREA_SYSTEM, 0x001c)
#define XN_SYS_NUIBINDINGCHANGED        XNID(6, _XNAREA_SYSTEM, 0x001d)
#define XN_SYS_AUDIOLATENCYCHANGED      XNID(8, _XNAREA_SYSTEM, 0x001e)
#define XN_SYS_NUICHATBINDINGCHANGED    XNID(8, _XNAREA_SYSTEM, 0x001f)
#define XN_SYS_INPUTACTIVITYCHANGED     XNID(9, _XNAREA_SYSTEM, 0x0020)
#define XN_SYS_LAST                     XNID(0, _XNAREA_SYSTEM, 0x0023)

//
// Live notifications
//

#define XN_LIVE_FIRST                   XNID(0, _XNAREA_LIVE, 0x0001)
#define XN_LIVE_CONNECTIONCHANGED       XNID(0, _XNAREA_LIVE, 0x0001)
#define XN_LIVE_INVITE_ACCEPTED         XNID(0, _XNAREA_LIVE, 0x0002)
#define XN_LIVE_LINK_STATE_CHANGED      XNID(0, _XNAREA_LIVE, 0x0003)
#define XN_LIVE_CONTENT_INSTALLED       XNID(0, _XNAREA_LIVE, 0x0007)
#define XN_LIVE_MEMBERSHIP_PURCHASED    XNID(0, _XNAREA_LIVE, 0x0008)
#define XN_LIVE_VOICECHAT_AWAY          XNID(0, _XNAREA_LIVE, 0x0009)
#define XN_LIVE_PRESENCE_CHANGED        XNID(0, _XNAREA_LIVE, 0x000A)
#define XN_LIVE_LAST                    XNID(XNID_CURRENTVERSION+1, _XNAREA_LIVE, 0x0014)

//
// Friends notifications
//

#define XN_FRIENDS_FIRST                XNID(0, _XNAREA_FRIENDS, 0x0001)
#define XN_FRIENDS_PRESENCE_CHANGED     XNID(0, _XNAREA_FRIENDS, 0x0001)
#define XN_FRIENDS_FRIEND_ADDED         XNID(0, _XNAREA_FRIENDS, 0x0002)
#define XN_FRIENDS_FRIEND_REMOVED       XNID(0, _XNAREA_FRIENDS, 0x0003)
#define XN_FRIENDS_LAST                 XNID(XNID_CURRENTVERSION+1, _XNAREA_FRIENDS, 0x0009)

//
// Custom notifications
//

#define XN_CUSTOM_FIRST                 XNID(0, _XNAREA_CUSTOM, 0x0001)
#define XN_CUSTOM_ACTIONPRESSED         XNID(0, _XNAREA_CUSTOM, 0x0003)
#define XN_CUSTOM_GAMERCARD             XNID(1, _XNAREA_CUSTOM, 0x0004)
#define XN_CUSTOM_LAST                  XNID(XNID_CURRENTVERSION+1, _XNAREA_CUSTOM, 0x0005)


//
// XMP notifications
//

#define XN_XMP_FIRST                                     XNID(0, _XNAREA_XMP, 0x0001)
#define XN_XMP_STATECHANGED                              XNID(0, _XNAREA_XMP, 0x0001)
#define XN_XMP_PLAYBACKBEHAVIORCHANGED                   XNID(0, _XNAREA_XMP, 0x0002)
#define XN_XMP_PLAYBACKCONTROLLERCHANGED                 XNID(0, _XNAREA_XMP, 0x0003)
#define XN_XMP_LAST                                      XNID(XNID_CURRENTVERSION+1, _XNAREA_XMP, 0x000D)


//
// Party notifications
//

#define XN_PARTY_FIRST                                   XNID(0, _XNAREA_PARTY, 0x0001)
#define XN_PARTY_MEMBERS_CHANGED                         XNID(4, _XNAREA_PARTY, 0x0002)
#define XN_PARTY_LAST                                    XNID(XNID_CURRENTVERSION+1, _XNAREA_PARTY, 0x0006)

extern DWORD sys_ui;
extern int g_dwListener;
extern int g_xlive;
extern HANDLE g_dwFakeListener;