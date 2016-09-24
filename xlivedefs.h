#ifndef XLIVEDEFS_H
#define XLIVEDEFS_H


//typedef unsigned __int64 ULONG_PTR, *PULONG_PTR;
//typedef unsigned long DWORD_PTR;


// Xbox Secure Network Library ------------------------------------------------

//
// XNetStartup is called to load the Xbox Secure Network Library.  It takes an
// optional pointer to a parameter structure.  To initialize the library with
// the default set of parameters, simply pass NULL for this argument.  To
// initialize the library with a different set of parameters, place an
// XNetStartupParams on the stack, zero it out, set the cfgSizeOfStruct to
// sizeof(XNetStartupParams), set any of the parameters you want to configure
// (leaving the remaining ones zeroed), and pass a pointer to this structure to
// XNetStartup.
//
// By default the Xbox Secure Network Library operates in secure mode, which
// means that communication to untrusted hosts (such as a PC) is disallowed.
// However, the devkit version of the library allows you to bypass this
// security by specifying the XNET_STARTUP_BYPASS_SECURITY flag in the cfgFlags
// parameter.  Here is an example of how to do this:
//
//      XNetStartupParams xnsp;
//      memset(&xnsp, 0, sizeof(xnsp));
//      xnsp.cfgSizeOfStruct = sizeof(XNetStartupParams);
//      xnsp.cfgFlags = XNET_STARTUP_BYPASS_SECURITY;
//      INT err = XNetStartup(&xnsp);
//
// Attempting to bypass security when not using the devkit version of the
// library does not return an error, it is simply ignored.  Attempts to send or
// receive packets from untrusted hosts will fail.
//


//
// This devkit-only flag tells the XNet stack to allow insecure
// communication to untrusted hosts (such as a PC).  This flag is silently
// ignored by the secure versions of the library.
//
#define XNET_STARTUP_BYPASS_SECURITY                0x01

//
// This flag instructs XNet to pre-allocate memory for the maximum number of
// datagram (UDP and VDP) sockets during the 'XNetStartup' call and store the
// objects in an internal pool.  Otherwise, sockets are allocated on demand (by
// the 'socket' function).  In either case, SOCK_DGRAM sockets are returned to
// the internal pool once closed.  The memory will remain allocated until
// XNetCleanup is called.
//
#define XNET_STARTUP_ALLOCATE_MAX_DGRAM_SOCKETS     0x02

//
// This flag instructs XNet to pre-allocate memory for the maximum number of
// stream (TCP) sockets during the 'XNetStartup' call and store the objects in
// an internal pool.  Otherwise, sockets are allocated on demand (by the
// 'socket', 'listen', and 'accept' functions).  Note that 'listen' will still
// attempt to pre-allocate the specified maximum backlog number of sockets even
// without this flag set.  The 'accept' function will always return a socket
// retrieved from the pool, though it will also attempt to allocate a
// replacement if the cfgSockMaxStreamSockets limit and memory permit.
// In all cases, SOCK_STREAM sockets are returned to the internal pool once
// closed. The memory will remain allocated until XNetCleanup is called.
//
#define XNET_STARTUP_ALLOCATE_MAX_STREAM_SOCKETS    0x04

//
// This devkit-only flag tells the XNet stack to disable encryption for
// communication between peers.  This flag is silently ignored by the secure
// versions of the library.
//
#define XNET_STARTUP_DISABLE_PEER_ENCRYPTION        0x08



// Creation Flags
#define XSESSION_CREATE_USES_MASK                       0x0000003F

#define XSESSION_CREATE_HOST                            0x00000001  // Set only on the host of a multiplayer session. The user who sets the host flag is the user that interacts with Live
#define XSESSION_CREATE_USES_PRESENCE                   0x00000002  // Session is used across games to keep players together. Advertises state via Presence
#define XSESSION_CREATE_USES_STATS                      0x00000004  // Session is used for stats tracking
#define XSESSION_CREATE_USES_MATCHMAKING                0x00000008  // Session is advertised in matchmaking for searching
#define XSESSION_CREATE_USES_ARBITRATION                0x00000010  // Session stats are arbitrated (and therefore tracked for everyone in the game)
#define XSESSION_CREATE_USES_PEER_NETWORK               0x00000020  // Session XNKey is registered and PC settings are enforced

// Optional modifiers to sessions that are created with XSESSION_CREATE_USES_PRESENCE
#define XSESSION_CREATE_MODIFIERS_MASK                  0x00000F80

#define XSESSION_CREATE_SOCIAL_MATCHMAKING_ALLOWED      0x00000080  // Session may be converted to an Social Matchmaking session
#define XSESSION_CREATE_INVITES_DISABLED                0x00000100  // Game Invites cannot be sent by the HUD for this session
#define XSESSION_CREATE_JOIN_VIA_PRESENCE_DISABLED      0x00000200  // Session will not ever be displayed as joinable via Presence
#define XSESSION_CREATE_JOIN_IN_PROGRESS_DISABLED       0x00000400  // Session will not be joinable between XSessionStart and XSessionEnd
#define XSESSION_CREATE_JOIN_VIA_PRESENCE_FRIENDS_ONLY  0x00000800  // Session is only joinable via presence for friends of the host



typedef struct
{

    //
    // Must be set to sizeof(XNetStartupParams).  There is no default.
    //
    BYTE        cfgSizeOfStruct;

    //
    // One or more of the XNET_STARTUP_xxx flags OR'd together.
    //
    // The default is 0 (no flags specified).
    BYTE        cfgFlags;

    //
    // The maximum number of SOCK_DGRAM (UDP or VDP) sockets that can be
    // opened at once.
    //
    // The default is 8 sockets.
    //
    BYTE        cfgSockMaxDgramSockets;

    //
    // The maximum number of SOCK_STREAM (TCP) sockets that can be opened at
    // once, including those sockets created as a result of incoming connection
    // requests.  Remember that a TCP socket may not be closed immediately
    // after 'closesocket' is called, depending on the linger options in place
    // (by default a TCP socket will linger).
    //
    // The default is 32 sockets.
    //
    BYTE        cfgSockMaxStreamSockets;

    //
    // The default receive buffer size for a socket, in units of K (1024 bytes).
    //
    // The default is 16 units (16K).
    //
    BYTE        cfgSockDefaultRecvBufsizeInK;

    //
    // The default send buffer size for a socket, in units of K (1024 bytes).
    //
    // The default is 16 units (16K).
    //
    BYTE        cfgSockDefaultSendBufsizeInK;

    //
    // The maximum number of XNKID / XNKEY pairs that can be registered at the
    // same time by calling XNetRegisterKey.
    //
    // The default is 8 key pair registrations.
    //
    BYTE        cfgKeyRegMax;

    //
    // The maximum number of security associations that can be registered at
    // the same time.  Security associations are created for each unique
    // XNADDR / XNKID pair passed to XNetXnAddrToInAddr.  Security associations
    // are also implicitly created for each secure host that establishes an
    // incoming connection with this host on a given registered XNKID.  Note
    // that there will only be one security association between a pair of hosts
    // on a given XNKID no matter how many sockets are actively communicating
    // on that secure connection.
    //
    // The default is 32 security associations.
    //
    BYTE        cfgSecRegMax;

    //
    // The maximum amount of QoS data, in units of DWORD (4 bytes), that can be
    // supplied to a call to XNetQosListen or returned in the result set of a
    // call to XNetQosLookup.
    //
    // The default is 64 (256 bytes).
    //
    BYTE        cfgQosDataLimitDiv4;

    //
    // The amount of time to wait for a response after sending a QoS packet
    // before sending it again (or giving up).  This should be set to the same
    // value on clients (XNetQosLookup callers) and servers (XNetQosListen
    // callers).
    //
    // The default is 2 seconds.
    //
    BYTE        cfgQosProbeTimeoutInSeconds;

    //
    // The maximum number of times to retry a given QoS packet when no response
    // is received.  This should be set to the same value on clients
    // (XNetQosLookup callers) and servers (XNetQosListen callers).
    //
    // The default is 3 retries.
    //
    BYTE        cfgQosProbeRetries;

    //
    // The maximum number of simultaneous QoS lookup responses that a QoS
    // listener supports.  Note that the bandwidth throttling parameter passed
    // to XNetQosListen may impact the number of responses queued, and thus
    // affects how quickly this limit is reached.
    //
    // The default is 8 responses.
    //
    BYTE        cfgQosSrvMaxSimultaneousResponses;

    //
    // The maximum amount of time for QoS listeners to wait for the second
    // packet in a packet pair.
    //
    // The default is 2 seconds.
    //
    BYTE        cfgQosPairWaitTimeInSeconds;

} XNetStartupParams;

typedef struct
{
    IN_ADDR     ina;                            // IP address (zero if not static/DHCP)
    IN_ADDR     inaOnline;                      // Online IP address (zero if not online)
    WORD        wPortOnline;                    // Online port
    BYTE        abEnet[6];                      // Ethernet MAC address
    BYTE        abOnline[20];                   // Online identification
} XNADDR;

typedef struct
{
    BYTE        ab[8];                          // xbox to xbox key identifier
} XNKID;

typedef XNADDR TSADDR;


#define XNET_XNKID_MASK                 0xF0    // Mask of flag bits in first byte of XNKID
#define XNET_XNKID_SYSTEM_LINK          0x00    // Peer to peer system link session
#define XNET_XNKID_SYSTEM_LINK_XPLAT    0x40    // Peer to peer system link session for cross-platform
#define XNET_XNKID_ONLINE_PEER          0x80    // Peer to peer online session
#define XNET_XNKID_ONLINE_SERVER        0xC0    // Client to server online session
#define XNET_XNKID_ONLINE_TITLESERVER   0xE0    // Client to title server online session

#define XNetXnKidIsSystemLinkXbox(pxnkid)       (((pxnkid)->ab[0] & 0xE0) == XNET_XNKID_SYSTEM_LINK)
#define XNetXnKidIsSystemLinkXPlat(pxnkid)      (((pxnkid)->ab[0] & 0xE0) == XNET_XNKID_SYSTEM_LINK_XPLAT)
#define XNetXnKidIsSystemLink(pxnkid)           (XNetXnKidIsSystemLinkXbox(pxnkid) || XNetXnKidIsSystemLinkXPlat(pxnkid))
#define XNetXnKidIsOnlinePeer(pxnkid)           (((pxnkid)->ab[0] & 0xE0) == XNET_XNKID_ONLINE_PEER)
#define XNetXnKidIsOnlineServer(pxnkid)         (((pxnkid)->ab[0] & 0xE0) == XNET_XNKID_ONLINE_SERVER)
#define XNetXnKidIsOnlineTitleServer(pxnkid)    (((pxnkid)->ab[0] & 0xE0) == XNET_XNKID_ONLINE_TITLESERVER)


typedef struct
{
    BYTE        ab[16];                         // xbox to xbox key exchange key
} XNKEY;

typedef struct
{
    INT         iStatus;                        // WSAEINPROGRESS if pending; 0 if success; error if failed
    UINT        cina;                           // Count of IP addresses for the given host
    IN_ADDR     aina[8];                        // Vector of IP addresses for the given host
} XNDNS;

#define XNET_XNQOSINFO_COMPLETE         0x01    // Qos has finished processing this entry
#define XNET_XNQOSINFO_TARGET_CONTACTED 0x02    // Target host was successfully contacted
#define XNET_XNQOSINFO_TARGET_DISABLED  0x04    // Target host has disabled its Qos listener
#define XNET_XNQOSINFO_DATA_RECEIVED    0x08    // Target host supplied Qos data
#define XNET_XNQOSINFO_PARTIAL_COMPLETE 0x10    // Qos has unfinished estimates for this entry
#define XNET_XNQOSINFO_UNK	0x11

typedef struct
{
    BYTE        bFlags;                         // See XNET_XNQOSINFO_*
    BYTE        bReserved;                      // Reserved
    WORD        cProbesXmit;                    // Count of Qos probes transmitted
    WORD        cProbesRecv;                    // Count of Qos probes successfully received
    WORD        cbData;                         // Size of Qos data supplied by target (may be zero)
    BYTE *      pbData;                         // Qos data supplied by target (may be NULL)
    WORD        wRttMinInMsecs;                 // Minimum round-trip time in milliseconds
    WORD        wRttMedInMsecs;                 // Median round-trip time in milliseconds
    DWORD       dwUpBitsPerSec;                 // Upstream bandwidth in bits per second
    DWORD       dwDnBitsPerSec;                 // Downstream bandwidth in bits per second
} XNQOSINFO;

typedef struct
{
    UINT        cxnqos;                         // Count of items in axnqosinfo[] array
    UINT        cxnqosPending;                  // Count of items still pending
    XNQOSINFO   axnqosinfo[1];                  // Vector of Qos results
} XNQOS;

typedef struct
{
    DWORD       dwSizeOfStruct;                 // Structure size, must be set prior to calling XNetQosGetListenStats
    DWORD       dwNumDataRequestsReceived;      // Number of client data request probes received
    DWORD       dwNumProbesReceived;            // Number of client probe requests received
    DWORD       dwNumSlotsFullDiscards;         // Number of client requests discarded because all slots are full
    DWORD       dwNumDataRepliesSent;           // Number of data replies sent
    DWORD       dwNumDataReplyBytesSent;        // Number of data reply bytes sent
    DWORD       dwNumProbeRepliesSent;          // Number of probe replies sent
} XNQOSLISTENSTATS;



INT   WINAPI XNetStartup(const XNetStartupParams * pxnsp);
//INT   WINAPI XNetCleanup();

INT   WINAPI XNetRandom(BYTE * pb, UINT cb);

INT   WINAPI XNetCreateKey(XNKID * pxnkid, XNKEY * pxnkey);
INT   WINAPI XNetRegisterKey(const XNKID * pxnkid, const XNKEY * pxnkey);
INT   WINAPI XNetUnregisterKey(const XNKID * pxnkid);
INT   WINAPI XNetReplaceKey(const XNKID * pxnkidUnregister, const XNKID * pxnkidReplace);

INT   WINAPI XNetXnAddrToInAddr(const XNADDR * pxna, const XNKID * pxnkid, IN_ADDR * pina);
INT   WINAPI XNetServerToInAddr(const IN_ADDR ina, DWORD dwServiceId, IN_ADDR * pina);
INT   WINAPI XNetTsAddrToInAddr(const TSADDR * ptsa, DWORD dwServiceId, const XNKID * pxnkid, IN_ADDR * pina);
INT   WINAPI XNetInAddrToXnAddr(const IN_ADDR ina, XNADDR * pxna, XNKID * pxnkid);
INT   WINAPI XNetInAddrToServer(const IN_ADDR ina, IN_ADDR *pina);
INT   WINAPI XNetInAddrToString(const IN_ADDR ina, char * pchBuf, INT cchBuf);
INT   WINAPI XNetUnregisterInAddr(const IN_ADDR ina);
INT   WINAPI XNetXnAddrToMachineId(const XNADDR * pxnaddr, ULONGLONG * pqwMachineId);


#define XNET_XNADDR_PLATFORM_XBOX1          0x00000000 // Platform type is original Xbox
#define XNET_XNADDR_PLATFORM_XBOX360        0x00000001 // Platform type is Xbox 360
#define XNET_XNADDR_PLATFORM_WINPC          0x00000002 // Platform type is Windows PC

INT   WINAPI XNetGetXnAddrPlatform(const XNADDR * pxnaddr, DWORD * pdwPlatform);


#define XNET_CONNECT_STATUS_IDLE            0x00000000 // Connection not started; use XNetConnect or send packet
#define XNET_CONNECT_STATUS_PENDING         0x00000001 // Connecting in progress; not complete yet
#define XNET_CONNECT_STATUS_CONNECTED       0x00000002 // Connection is established
#define XNET_CONNECT_STATUS_LOST            0x00000003 // Connection was lost

INT   WINAPI XNetConnect(const IN_ADDR ina);
INT WINAPI XNetGetConnectStatus(const IN_ADDR ina);


INT   WINAPI XNetDnsLookup(const char * pszHost, WSAEVENT hEvent, XNDNS ** ppxndns);
INT   WINAPI XNetDnsRelease(XNDNS * pxndns);


#define XNET_QOS_LISTEN_ENABLE              0x00000001 // Responds to queries on the given XNKID
#define XNET_QOS_LISTEN_DISABLE             0x00000002 // Rejects queries on the given XNKID
#define XNET_QOS_LISTEN_SET_DATA            0x00000004 // Sets the block of data to send back to queriers
#define XNET_QOS_LISTEN_SET_BITSPERSEC      0x00000008 // Sets max bandwidth that query reponses may consume
#define XNET_QOS_LISTEN_RELEASE             0x00000010 // Stops listening on given XNKID and releases memory

#define XNET_QOS_LOOKUP_RESERVED            0x00000000 // No flags defined yet for XNetQosLookup

#define XNET_QOS_SERVICE_LOOKUP_RESERVED    0x00000000 // No flags defined yet for XNetQosServiceLookup

INT   WINAPI XNetQosListen(const XNKID * pxnkid,
                           const BYTE * pb,
                           UINT cb,
                           DWORD dwBitsPerSec, DWORD dwFlags);
INT   WINAPI XNetQosLookup(UINT cxna,
                           const XNADDR * apxna[],
                           const XNKID * apxnkid[],
                           const XNKEY * apxnkey[],
                           UINT cina,
                           const IN_ADDR aina[],
                           const DWORD adwServiceId[],
                           UINT cProbes, DWORD dwBitsPerSec, DWORD dwFlags, WSAEVENT hEvent, XNQOS ** ppxnqos);
INT   WINAPI XNetQosServiceLookup(DWORD dwFlags, WSAEVENT hEvent, XNQOS ** ppxnqos);
INT   WINAPI XNetQosRelease(XNQOS * pxnqos);
INT   WINAPI XNetQosGetListenStats(const XNKID * pxnkid, XNQOSLISTENSTATS * pQosListenStats);


#define XNET_GET_XNADDR_PENDING             0x00000000 // Address acquisition is not yet complete
#define XNET_GET_XNADDR_NONE                0x00000001 // XNet is uninitialized or no debugger found
#define XNET_GET_XNADDR_ETHERNET            0x00000002 // Host has ethernet address (no IP address)
#define XNET_GET_XNADDR_STATIC              0x00000004 // Host has statically assigned IP address
#define XNET_GET_XNADDR_DHCP                0x00000008 // Host has DHCP assigned IP address
#define XNET_GET_XNADDR_PPPOE               0x00000010 // Host has PPPoE assigned IP address
#define XNET_GET_XNADDR_GATEWAY             0x00000020 // Host has one or more gateways configured
#define XNET_GET_XNADDR_DNS                 0x00000040 // Host has one or more DNS servers configured
#define XNET_GET_XNADDR_ONLINE              0x00000080 // Host is currently connected to online service
#define XNET_GET_XNADDR_TROUBLESHOOT        0x00008000 // Network configuration requires troubleshooting

DWORD WINAPI XNetGetTitleXnAddr(XNADDR * pxna);
DWORD WINAPI XNetGetDebugXnAddr(XNADDR * pxna);


#define XNET_ETHERNET_LINK_ACTIVE           0x00000001 // Ethernet cable is connected and active
#define XNET_ETHERNET_LINK_100MBPS          0x00000002 // Ethernet link is set to 100 Mbps
#define XNET_ETHERNET_LINK_10MBPS           0x00000004 // Ethernet link is set to 10 Mbps
#define XNET_ETHERNET_LINK_FULL_DUPLEX      0x00000008 // Ethernet link is in full duplex mode
#define XNET_ETHERNET_LINK_HALF_DUPLEX      0x00000010 // Ethernet link is in half duplex mode
#define XNET_ETHERNET_LINK_WIRELESS         0x00000020 // Ethernet link is wireless (802.11 based)

//DWORD WINAPI XNetGetEthernetLinkStatus();


#define XNET_BROADCAST_VERSION_OLDER        0x00000001 // Got broadcast packet(s) from incompatible older version of title
#define XNET_BROADCAST_VERSION_NEWER        0x00000002 // Got broadcast packet(s) from incompatible newer version of title

DWORD WINAPI XNetGetBroadcastVersionStatus(BOOL fReset);


//
// Value = XNetStartupParams
// Get   = Returns the XNetStartupParams values that were used at
//         initialization time.
// Set   = Not allowed.
//
#define XNET_OPTID_STARTUP_PARAMS                   1

//
// Value = ULONGLONG
// Get   = Returns total number of bytes sent by the NIC hardware since system
//         boot, including sizes of all protocol headers.
// Set   = Not allowed.
//
#define XNET_OPTID_NIC_XMIT_BYTES                   2

//
// Value = DWORD
// Get   = Returns total number of frames sent by the NIC hardware since system
//         boot.
// Set   = Not allowed.
//
#define XNET_OPTID_NIC_XMIT_FRAMES                  3

//
// Value = ULONGLONG
// Get   = Returns total number of bytes received by the NIC hardware since
//         system boot, including sizes of all protocol headers.
// Set   = Not allowed.
//
#define XNET_OPTID_NIC_RECV_BYTES                   4

//
// Value = DWORD
// Get   = Returns total number of frames received by the NIC hardware since
//         system boot.
// Set   = Not allowed.
//
#define XNET_OPTID_NIC_RECV_FRAMES                  5

//
// Value = ULONGLONG
// Get   = Returns the number of bytes sent by the caller since XNetStartup/
//         WSAStartup, including sizes of all protocol headers.
// Set   = Not allowed.
//
#define XNET_OPTID_CALLER_XMIT_BYTES                6

//
// Value = DWORD
// Get   = Returns total number of frames sent by the caller since XNetStartup/
//         WSAStartup.
// Set   = Not allowed.
//
#define XNET_OPTID_CALLER_XMIT_FRAMES               7

//
// Value = ULONGLONG
// Get   = Returns total number of bytes received by the caller since
//         XNetStartup/WSAStartup, including sizes of all protocol headers.
// Set   = Not allowed.
//
#define XNET_OPTID_CALLER_RECV_BYTES                8

//
// Value = DWORD
// Get   = Returns total number of frames received by the caller since
//         XNetStartup/WSAStartup.
// Set   = Not allowed.
//
#define XNET_OPTID_CALLER_RECV_FRAMES               9

// Get   = Retrieves 0 for default behavior
// Set   = Set 0 for default behavior

INT    WINAPI XNetGetOpt(DWORD dwOptId, BYTE * pbValue, DWORD * pdwValueSize);
INT    WINAPI XNetSetOpt(DWORD dwOptId, const BYTE * pbValue, DWORD dwValueSize);

#define XNET_CONNECT_STATUS_IDLE            0x00000000 // Connection not started; use XNetConnect or send packet
#define XNET_CONNECT_STATUS_PENDING         0x00000001 // Connecting in progress; not complete yet
#define XNET_CONNECT_STATUS_CONNECTED       0x00000002 // Connection is established
#define XNET_CONNECT_STATUS_LOST            0x00000003 // Connection was lost


//
// Since our socket handles are not file handles, apps can NOT call CancelIO API to cancel
// outstanding overlapped I/O requests. Apps must call WSACancelOverlappedIO function instead.
//


//------------------------------------------------------------------------------
// Notification apis
//------------------------------------------------------------------------------

//  Notification ids are 32 bit values layed out as follows:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +-+-----------+-----------------+-------------------------------+
//  |R|    Area   |    Version      |            Index              |
//  +-+-----+-----+-----------------+-------------------------------+
//
//  where
//
//      R - is a reserved bit (internal)
//
//      Area - is the area ranging from 0 - 63 (6 bits)
//
//      Version - is used to ensure that new notifications are not sent to
//          titles built to previous XDKs
//
//      Index - is the id of the notification within the area.  Each area can
//           have up to 65535 notifications starting at 1 (zero being invalid).
//

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




//typedef ULONGLONG XUID;
//typedef XUID *PXUID;

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
} XUSER_SIGNIN_INFO, * PXUSER_SIGNIN_INFO;

// Xbox-specific Overlapped

typedef struct _XOVERLAPPED             XOVERLAPPED, *PXOVERLAPPED;

typedef
	VOID
	(WINAPI *PXOVERLAPPED_COMPLETION_ROUTINE)(
	   DWORD dwErrorCode,
	   DWORD dwNumberOfBytesTransfered,
		 DWORD pOverlapped
	);


typedef struct _XOVERLAPPED {
	ULONG_PTR                           InternalLow;
	ULONG_PTR                           InternalHigh;
	ULONG_PTR                           InternalContext;
	HANDLE                              hEvent;
	PXOVERLAPPED_COMPLETION_ROUTINE     pCompletionRoutine;
	DWORD_PTR                           dwCompletionContext;
	DWORD                               dwExtendedError;
} XOVERLAPPED, *PXOVERLAPPED;

typedef enum _XUSER_PROFILE_SOURCE
{
	XSOURCE_NO_VALUE = 0,
	XSOURCE_DEFAULT,
	XSOURCE_TITLE,
	XSOURCE_PERMISSION_DENIED
} XUSER_PROFILE_SOURCE;

typedef struct  {
	BYTE type;

	union {
		LONG nData;
		LONGLONG i64Data;
		double dblData;
		struct {
			DWORD cbData;
			LPWSTR pwszData;
		}string;
		float fData;
		struct {
			DWORD cbData;
			LPBYTE pbData;
		}binary;
		FILETIME ftData;
	};
} XUSER_DATA, *PXUSER_DATA;


typedef struct _XUSER_PROFILE_SETTING {
	XUSER_PROFILE_SOURCE source;

	union {
		DWORD dwUserIndex;
		XUID xuid;
	}user;

	DWORD dwSettingId;

	XUSER_DATA data;
} XUSER_PROFILE_SETTING, *PXUSER_PROFILE_SETTING;

typedef struct _XUSER_READ_PROFILE_SETTING_RESULT {
	DWORD dwSettingsLen;
	XUSER_PROFILE_SETTING *pSettings;
} XUSER_READ_PROFILE_SETTING_RESULT, *PXUSER_READ_PROFILE_SETTING_RESULT;

// XContent

// Content types
#define XCONTENTTYPE_SAVEDGAME                      0x00000001
#define XCONTENTTYPE_MARKETPLACE                    0x00000002
#define XCONTENTTYPE_PUBLISHER                      0x00000003

#define XCONTENTTYPE_GAMEDEMO                       0x00080000
#define XCONTENTTYPE_ARCADE                         0x000D0000

//  Content creation/open flags
#define XCONTENTFLAG_NONE                           0x00000000
#define XCONTENTFLAG_CREATENEW                      CREATE_NEW
#define XCONTENTFLAG_CREATEALWAYS                   CREATE_ALWAYS
#define XCONTENTFLAG_OPENEXISTING                   OPEN_EXISTING
#define XCONTENTFLAG_OPENALWAYS                     OPEN_ALWAYS
#define XCONTENTFLAG_TRUNCATEEXISTING               TRUNCATE_EXISTING

//  Content attributes
#define XCONTENTFLAG_NOPROFILE_TRANSFER             0x00000010
#define XCONTENTFLAG_NODEVICE_TRANSFER              0x00000020
#define XCONTENTFLAG_STRONG_SIGNED                  0x00000040
#define XCONTENTFLAG_ALLOWPROFILE_TRANSFER          0x00000080
#define XCONTENTFLAG_MOVEONLY_TRANSFER              0x00000800

//  Device selector flags
#define XCONTENTFLAG_MANAGESTORAGE                  0x00000100
#define XCONTENTFLAG_FORCE_SHOW_UI                  0x00000200

//  Enumeration scoping
#define XCONTENTFLAG_ENUM_EXCLUDECOMMON             0x00001000


#define XCONTENT_MAX_DISPLAYNAME_LENGTH 128
#define XCONTENT_MAX_FILENAME_LENGTH    42
#define XCONTENTDEVICE_MAX_NAME_LENGTH  27

typedef DWORD                           XCONTENTDEVICEID, *PXCONTENTDEVICEID;

typedef struct _XCONTENT_DATA {
	DWORD ContentNum;
	DWORD TitleId;
	DWORD ContentPackageType;
	BYTE ContentId[20];
} XCONTENT_DATA, *PXCONTENT_DATA;


typedef struct _XUSER_ACHIEVEMENT {
	DWORD dwUserIndex;
	DWORD dwAchievementId;
} XUSER_ACHIEVEMENT, *PXUSER_ACHIEVEMENT;


typedef struct {
    XNKID sessionID;
    XNADDR hostAddress;
    XNKEY keyExchangeKey;
} XSESSION_INFO, *PXSESSION_INFO;
 


typedef enum _XSESSION_STATE
{
    XSESSION_STATE_LOBBY = 0,
    XSESSION_STATE_REGISTRATION,
    XSESSION_STATE_INGAME,
    XSESSION_STATE_REPORTING,
    XSESSION_STATE_DELETED
} XSESSION_STATE;
 

typedef struct {
    XUID xuidOnline;
    DWORD dwUserIndex;
    DWORD dwFlags;
} XSESSION_MEMBER;


typedef struct {
    DWORD dwUserIndexHost;
    DWORD dwGameType;
    DWORD dwGameMode;
    DWORD dwFlags;
    DWORD dwMaxPublicSlots;
    DWORD dwMaxPrivateSlots;
    DWORD dwAvailablePublicSlots;
    DWORD dwAvailablePrivateSlots;
    DWORD dwActualMemberCount;
    DWORD dwReturnedMemberCount;
    XSESSION_STATE eState;
    ULONGLONG qwNonce;
    XSESSION_INFO sessionInfo;
    XNKID xnkidArbitration;
    XSESSION_MEMBER *pSessionMembers;
} XSESSION_LOCAL_DETAILS, *PXSESSION_LOCAL_DETAILS;
  

typedef enum 
{
    XONLINE_NAT_OPEN = 1,
    XONLINE_NAT_MODERATE,
    XONLINE_NAT_STRICT
} XONLINE_NAT_TYPE;
 


/*
typedef struct _XHV_INIT_PARAMS {
    DWORD dwMaxRemoteTalkers;
    DWORD dwMaxLocalTalkers;
    PXHV_PROCESSING_MODE localTalkerEnabledModes;
    DWORD dwNumLocalTalkerEnabledModes;
    PXHV_PROCESSING_MODE remoteTalkerEnabledModes;
    DWORD dwNumRemoteTalkerEnabledModes;
    BOOL bCustomVADProvided;
    BOOL bRelaxPrivileges;
    PFNMICRAWDATAREADY pfnMicrophoneRawDataReady;
    XAUDIO2_EFFECT_CHAIN **ppfxDefaultRemoteTalkerFX;
    XAUDIO2_EFFECT_CHAIN **ppfxDefaultTalkerPairFX;
    XAUDIO2_EFFECT_CHAIN *pfxOutputFX;
    IXAudio2 *pXAudio2;
} XHV_INIT_PARAMS, *PXHV_INIT_PARAMS;
*/


typedef struct _XUSER_PROPERTY
{
	DWORD                               dwPropertyId;
	XUSER_DATA                          value;
} XUSER_PROPERTY, *PXUSER_PROPERTY;


typedef struct _XUSER_CONTEXT
{
	DWORD                               dwContextId;
	DWORD                               dwValue;
} XUSER_CONTEXT, *PXUSER_CONTEXT;


typedef struct _XSESSION_SEARCHRESULT
{
	XSESSION_INFO   info;
	DWORD           dwOpenPublicSlots;
	DWORD           dwOpenPrivateSlots;
	DWORD           dwFilledPublicSlots;
	DWORD           dwFilledPrivateSlots;
	DWORD           cProperties;
	DWORD           cContexts;
	PXUSER_PROPERTY pProperties;
	PXUSER_CONTEXT  pContexts;
} XSESSION_SEARCHRESULT, *PXSESSION_SEARCHRESULT;


typedef struct _XSESSION_SEARCHRESULT_HEADER
{
	DWORD dwSearchResults;
	XSESSION_SEARCHRESULT *pResults;
} XSESSION_SEARCHRESULT_HEADER, *PXSESSION_SEARCHRESULT_HEADER;



typedef struct _XSESSION_REGISTRANT
{
	ULONGLONG qwMachineID;
	DWORD bTrustworthiness;
	DWORD bNumUsers;
	XUID *rgUsers;
} XSESSION_REGISTRANT;


typedef struct _XSESSION_REGISTRATION_RESULTS
{
	DWORD wNumRegistrants;
	XSESSION_REGISTRANT *rgRegistrants;
} XSESSION_REGISTRATION_RESULTS, *PXSESSION_REGISTRATION_RESULTS;



#define X_CONTEXT_PRESENCE              0x00008001	// ??
#define X_CONTEXT_GAME_TYPE             0x0000800A	// DR2
#define X_CONTEXT_GAME_MODE             0x0000800B


// Values for X_CONTEXT_GAME_TYPE
#define X_CONTEXT_GAME_TYPE_RANKED      0
#define X_CONTEXT_GAME_TYPE_STANDARD    1


typedef enum _XPRIVILEGE_TYPE
{
    XPRIVILEGE_MULTIPLAYER_SESSIONS = 254,
    XPRIVILEGE_COMMUNICATIONS = 252,
    XPRIVILEGE_COMMUNICATIONS_FRIENDS_ONLY = 251,
    XPRIVILEGE_PROFILE_VIEWING = 249,
    XPRIVILEGE_PROFILE_VIEWING_FRIENDS_ONLY = 248,
    XPRIVILEGE_USER_CREATED_CONTENT = 247,
    XPRIVILEGE_USER_CREATED_CONTENT_FRIENDS_ONLY = 246,
    XPRIVILEGE_PURCHASE_CONTENT = 245,
    XPRIVILEGE_PRESENCE = 244,
    XPRIVILEGE_PRESENCE_FRIENDS_ONLY = 243,
    XPRIVILEGE_SHARE_CONTENT_OUTSIDE_LIVE = 211,
    XPRIVILEGE_TRADE_CONTENT = 238,
    XPRIVILEGE_VIDEO_COMMUNICATIONS = 235,
    XPRIVILEGE_VIDEO_COMMUNICATIONS_FRIENDS_ONLY = 234,
    XPRIVILEGE_CONTENT_AUTHOR = 222
} XPRIVILEGE_TYPE;



typedef enum 
{
    XMARKETPLACE_OFFERING_TYPE_CONTENT = 0x00000002,
    XMARKETPLACE_OFFERING_TYPE_GAME_DEMO = 0x00000020,
    XMARKETPLACE_OFFERING_TYPE_GAME_TRAILER = 0x00000040,
    XMARKETPLACE_OFFERING_TYPE_THEME = 0x00000080,
    XMARKETPLACE_OFFERING_TYPE_TILE = 0x00000800,
    XMARKETPLACE_OFFERING_TYPE_ARCADE = 0x00002000,
    XMARKETPLACE_OFFERING_TYPE_VIDEO = 0x00004000,
    XMARKETPLACE_OFFERING_TYPE_CONSUMABLE = 0x00010000,
    XMARKETPLACE_OFFERING_TYPE_AVATARITEM = 0x00100000
} XMARKETPLACE_OFFERING_TYPE;
 


// fixme
class IXHV2ENGINE
{
public:
	IXHV2ENGINE::IXHV2ENGINE();


	// 2F0 bytes = actual size
	// - note: check all INT return values - may not be true

	INT Dummy1( VOID *pThis );	// 00
	INT Dummy2( VOID *pThis );	// 04
	HRESULT Dummy3( VOID *pThis, int a );	// 08

	HRESULT StartLocalProcessingModes( VOID *pThis, DWORD dwUserIndex, /* CONST PXHV_PROCESSING_MODE*/ VOID *processingModes, DWORD dwNumProcessingModes );
	HRESULT StopLocalProcessingModes( VOID *pThis, DWORD dwUserIndex, /*CONST PXHV_PROCESSING_MODE*/ VOID *processingModes, DWORD dwNumProcessingModes );

	HRESULT StartRemoteProcessingModes( VOID *pThis, int a1, int a2, int a3, int a4 );
	HRESULT Dummy7( VOID *pThis, int a1, int a2, int a3, int a4 );	// 18

	HRESULT Dummy8( VOID *pThis, int a1 );	// 1C

	BOOL IsLocalTalking(VOID *pThis, DWORD dwUserIndex);
	BOOL isRemoteTalking(VOID *pThis, XUID xuid);
	BOOL IsHeadsetPresent(VOID *pThis, DWORD dwUserIndex);

	HRESULT RegisterLocalTalker( VOID *pThis, DWORD dwUserIndex );
	HRESULT UnregisterLocalTalker( VOID *pThis, DWORD dwUserIndex );

	HRESULT Dummy11( VOID *pThis, XUID id, int a3, int a4, int a5 );	// 28
	HRESULT RegisterRemoteTalker(VOID *pThis, XUID id, LPVOID v1, LPVOID v2, LPVOID v3);
	HRESULT UnregisterRemoteTalker( VOID *pThis, XUID id);
	DWORD XSessionLeaveRemote(HANDLE hSession, DWORD dxXuidCount, const XUID *pXuids, PXOVERLAPPED pXOverlapped);

	HRESULT Dummy13( VOID *pThis, int a1, int a2 );	// 30
	INT Dummy14( VOID *pThis, int a1 );	// 34
	INT Dummy15( VOID *pThis, int a1 );	// 38
	HRESULT Dummy16( VOID *pThis, int a1, int a2 );	// 3C

	DWORD GetDataReadyFlags( VOID *pThis );

	HRESULT GetLocalChatData( VOID *pThis, DWORD dwUserIndex, PBYTE pbData, PDWORD pdwSize, PDWORD pdwPackets );
	HRESULT SetPlaybackPriority( VOID *pThis, int a1, int a2, int a3, int a4 );

	HRESULT Dummy20( VOID *pThis, int a1, int a2, int a3, int a4 );	// 4C



	// possible does not exist
	HRESULT Dummy21( VOID *pThis );	// 54
	HRESULT Dummy22( VOID *pThis );	// 58
	HRESULT Dummy23( VOID *pThis );	// 5C
	HRESULT Dummy24( VOID *pThis );	// 60

	HRESULT Dummy25( VOID *pThis );	// 64
	HRESULT Dummy26( VOID *pThis );	// 68
	HRESULT Dummy27( VOID *pThis );	// 6C
	HRESULT Dummy28( VOID *pThis );	// 70

	HRESULT Dummy29( VOID *pThis );	// 74
	HRESULT Dummy30( VOID *pThis );	// 78
	HRESULT Dummy31( VOID *pThis );	// 7C
	HRESULT Dummy32( VOID *pThis );	// 80





	typedef void (IXHV2ENGINE::*HV2FUNCPTR)(void);

	// ugly, low-skilled hackaround
	HV2FUNCPTR *funcTablePtr;
	HV2FUNCPTR funcPtr[100];
	HV2FUNCPTR func2;
};

typedef IXHV2ENGINE *PIXHV2ENGINE;


typedef struct {
    DWORD dwId;
    LPWSTR pwszLabel;
    LPWSTR pwszDescription;
    LPWSTR pwszUnachieved;
    DWORD dwImageId;
    DWORD dwCred;
    FILETIME ftAchieved;
    DWORD dwFlags;
} XACHIEVEMENT_DETAILS, *PXACHIEVEMENT_DETAILS;
 

#define XACHIEVEMENT_DETAILS_ACHIEVED_ONLINE 0x10000
#define XACHIEVEMENT_DETAILS_ACHIEVED 0x20000



 
typedef struct _MESSAGEBOX_RESULT {
	union {
		DWORD dwButtonPressed;
		WORD rgwPasscode[4];
	};
}	MESSAGEBOX_RESULT, *PMESSAGEBOX_RESULT;
 

typedef enum _XSTORAGE_FACILITY
{
    XSTORAGE_FACILITY_GAME_CLIP = 1,
    XSTORAGE_FACILITY_PER_TITLE = 2,
    XSTORAGE_FACILITY_PER_USER_TITLE = 3
} XSTORAGE_FACILITY;
 

typedef struct _XSTORAGE_DOWNLOAD_TO_MEMORY_RESULTS {
    DWORD dwBytesTotal;
    XUID xuidOwner;
    FILETIME ftCreated;
} XSTORAGE_DOWNLOAD_TO_MEMORY_RESULTS;
 

typedef struct {
    DWORD dwNewOffers;
    DWORD dwTotalOffers;
} XOFFERING_CONTENTAVAILABLE_RESULT;
 


#define XMARKETPLACE_CONTENT_ID_LEN 20

typedef struct {
    ULONGLONG qwOfferID;
    ULONGLONG qwPreviewOfferID;
    DWORD dwOfferNameLength;
    WCHAR *wszOfferName;
    DWORD dwOfferType;
    BYTE contentId[ XMARKETPLACE_CONTENT_ID_LEN ];
    BOOL fIsUnrestrictedLicense;
    DWORD dwLicenseMask;
    DWORD dwTitleID;
    DWORD dwContentCategory;
    DWORD dwTitleNameLength;
    WCHAR *wszTitleName;
    BOOL fUserHasPurchased;
    DWORD dwPackageSize;
    DWORD dwInstallSize;
    DWORD dwSellTextLength;
    WCHAR *wszSellText;
    DWORD dwAssetID;
    DWORD dwPurchaseQuantity;
    DWORD dwPointsPrice;
} XMARKETPLACE_CONTENTOFFER_INFO, *PXMARKETPLACE_CONTENTOFFER_INFO;


typedef struct _STRING_DATA {
    WORD wStringSize;
    WCHAR *pszString;
} STRING_DATA;
 

#pragma pack( push,1 )
typedef struct _STRING_VERIFY_RESPONSE {
	WORD wNumStrings;
	HRESULT *pStringResult;
} STRING_VERIFY_RESPONSE;
#pragma pack( pop )


#endif
