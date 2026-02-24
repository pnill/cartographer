#pragma once

typedef struct
{
	//
	// Must be set to sizeof(XNetStartupParams).  There is no default.
	//
	BYTE    cfgSizeOfStruct;

	//
	// One or more of the XNET_STARTUP_xxx flags OR'd together.
	//
	// The default is 0 (no flags specified).
	BYTE    cfgFlags;

	//
	// The maximum number of SOCK_DGRAM (UDP or VDP) sockets that can be
	// opened at once.
	//
	// The default is 8 sockets.
	//
	BYTE    cfgSockMaxDgramSockets;

	//
	// The maximum number of SOCK_STREAM (TCP) sockets that can be opened at
	// once, including those sockets created as a result of incoming connection
	// requests.  Remember that a TCP socket may not be closed immediately
	// after 'closesocket' is called, depending on the linger options in place
	// (by default a TCP socket will linger).
	//
	// The default is 32 sockets.
	//
	BYTE    cfgSockMaxStreamSockets;

	//
	// The default receive buffer size for a socket, in units of K (1024 bytes).
	//
	// The default is 16 units (16K).
	//
	BYTE    cfgSockDefaultRecvBufsizeInK;

	//
	// The default send buffer size for a socket, in units of K (1024 bytes).
	//
	// The default is 16 units (16K).
	//
	BYTE    cfgSockDefaultSendBufsizeInK;

	//
	// The maximum number of XNKID / XNKEY pairs that can be registered at the
	// same time by calling XNetRegisterKey.
	//
	// The default is 8 key pair registrations.
	//
	BYTE    cfgKeyRegMax;

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
	BYTE    cfgSecRegMax;

	//
	// The maximum amount of QoS data, in units of DWORD (4 bytes), that can be
	// supplied to a call to XNetQosListen or returned in the result set of a
	// call to XNetQosLookup.
	//
	// The default is 64 (256 bytes).
	//
	BYTE    cfgQosDataLimitDiv4;

	//
	// The amount of time to wait for a response after sending a QoS packet
	// before sending it again (or giving up).  This should be set to the same
	// value on clients (XNetQosLookup callers) and servers (XNetQosListen
	// callers).
	//
	// The default is 2 seconds.
	//
	BYTE    cfgQosProbeTimeoutInSeconds;

	//
	// The maximum number of times to retry a given QoS packet when no response
	// is received.  This should be set to the same value on clients
	// (XNetQosLookup callers) and servers (XNetQosListen callers).
	//
	// The default is 3 retries.
	//
	BYTE    cfgQosProbeRetries;

	//
	// The maximum number of simultaneous QoS lookup responses that a QoS
	// listener supports.  Note that the bandwidth throttling parameter passed
	// to XNetQosListen may impact the number of responses queued, and thus
	// affects how quickly this limit is reached.
	//
	// The default is 8 responses.
	//
	BYTE    cfgQosSrvMaxSimultaneousResponses;

	//
	// The maximum amount of time for QoS listeners to wait for the second
	// packet in a packet pair.
	//
	// The default is 2 seconds.
	//
	BYTE    cfgQosPairWaitTimeInSeconds;

} XNetStartupParams;

INT WINAPI XNetCleanup();

INT WINAPI XNetConnect(const IN_ADDR ina);
INT WINAPI XNetGetConnectStatus(const IN_ADDR ina);

INT WINAPI XNetRandom(BYTE* pb, UINT cb);

INT WINAPI XNetCreateKey(XNKID* pxnkid, XNKEY* pxnkey);
INT WINAPI XNetRegisterKey(const XNKID* pxnkid, const XNKEY* pxnkey);
INT WINAPI XNetUnregisterKey(const XNKID* pxnkid);
INT WINAPI XNetReplaceKey(const XNKID* pxnkidUnregister, const XNKID* pxnkidReplace);

INT WINAPI XNetXnAddrToInAddr(const XNADDR* pxna, const XNKID* pxnkid, IN_ADDR* pina);
INT WINAPI XNetServerToInAddr(const IN_ADDR ina, DWORD dwServiceId, IN_ADDR* pina);
INT WINAPI XNetTsAddrToInAddr(const TSADDR* ptsa, DWORD dwServiceId, const XNKID* pxnkid, IN_ADDR* pina);
INT WINAPI XNetInAddrToXnAddr(const IN_ADDR ina, XNADDR* pxna, XNKID* pxnkid);
INT WINAPI XNetInAddrToServer(const IN_ADDR ina, IN_ADDR* pina);
INT WINAPI XNetInAddrToString(const IN_ADDR ina, char* pchBuf, INT cchBuf);
INT WINAPI XNetUnregisterInAddr(const IN_ADDR ina);
INT WINAPI XNetXnAddrToMachineId(const XNADDR* pxnaddr, ULONGLONG* pqwMachineId);

INT WINAPI XNetDnsLookup(const char* pszHost, WSAEVENT hEvent, XNDNS** ppxndns);

INT WINAPI XNetDnsRelease(XNDNS* pxndns);
