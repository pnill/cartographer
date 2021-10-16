
#include "xnet.h"

// #58: XNetServerToInAddr
INT   WINAPI XNetServerToInAddr(const IN_ADDR ina, DWORD dwServiceId, IN_ADDR * pina)
{
	LOG_TRACE_NETWORK("XNetServerToInAddr");
	if (pina)
		*pina = ina;
	return 0;
}


// #59: XNetTsAddrToInAddr
INT   WINAPI XNetTsAddrToInAddr(const TSADDR * ptsa, DWORD dwServiceId, const XNKID * pxnkid, IN_ADDR * pina)
{
	LOG_TRACE_NETWORK("XNetTsAddrToInAddr");
	if (pina)
		*pina = ptsa->ina;
	return 0;
}


// #61: XNetInAddrToServer
INT   WINAPI XNetInAddrToServer(const IN_ADDR ina, IN_ADDR *pina)
{
	LOG_TRACE_NETWORK("XNetInAddrToServer");
	if (pina)
		*pina = ina;
	return 0;
}


// #62: XNetInAddrToString
INT   WINAPI XNetInAddrToString(const IN_ADDR ina, char * pchBuf, INT cchBuf)
{
	LOG_TRACE_NETWORK("XNetInAddrToString");
	strncpy(pchBuf, inet_ntoa(ina), cchBuf);
	return 0;
}

// #64
INT WINAPI XNetXnAddrToMachineId(const XNADDR * pxnaddr, ULONGLONG * pqwMachineId)
{
	LOG_TRACE_NETWORK("XNetXnAddrToMachineId");


	// ???
	return -1;


	if (pqwMachineId)
		*pqwMachineId = 0xDEADC0DE;

	return 0;
}

// #67: XNetDnsLookup
int WINAPI XNetDnsLookup(const char * pszHost, DWORD hEvent, void ** ppxndns)
{
	LOG_TRACE_NETWORK("XNetDnsLookup");
	if (ppxndns)
		*ppxndns = NULL;
	return 1;   // ERROR
}

// #68: XNetDnsRelease
int WINAPI XNetDnsRelease(void * pxndns)
{
	LOG_TRACE_NETWORK("XNetDnsRelease");
	return 0;
}

// #75
DWORD WINAPI XNetGetEthernetLinkStatus()
{
	static int print = 0;


	if (print < 15)
	{
		//LOG_TRACE_NETWORK("XNetGetEthernetLinkStatus() - active: 100 mbps, full duplex");

		print++;
	}

	return XNET_ETHERNET_LINK_ACTIVE | XNET_ETHERNET_LINK_100MBPS | XNET_ETHERNET_LINK_FULL_DUPLEX;
}


// #76
DWORD WINAPI XNetGetBroadcastVersionStatus(DWORD a1)
{
	LOG_TRACE_NETWORK("XNetGetBroadcastVersionStatus");
	return 0;
}

// #78
INT WINAPI XNetGetOpt(DWORD dwOptId, BYTE * pbValue, DWORD * pdwValueSize)
{
	LOG_TRACE_NETWORK("XNetGetOpt");
	return WSAEINVAL;
}


// #79: XNetSetOpt
INT WINAPI XNetSetOpt(DWORD dwOptId, const BYTE * pbValue, DWORD dwValueSize)
{
	LOG_TRACE_NETWORK("XNetSetOpt");
	return WSAEINVAL;
}

// #80
int WINAPI XNetStartupEx(int a1, int a2, int a3)
{
	LOG_TRACE_NETWORK("XNetStartupEx");
	return 0;
}

// #81
int WINAPI XNetReplaceKey(const XNKID *pxnkidUnregister, const XNKID *pxnkidReplace)
{
	LOG_TRACE_NETWORK("XNetReplaceKey( pxnkidUnregister: {0:p}, pxnkidreplace: {1:p})", (void*)pxnkidUnregister, (void*)pxnkidReplace);
	return 0;
}

// #82
int WINAPI XNetGetXnAddrPlatform(in_addr *a1, int a2)
{
	LOG_TRACE_NETWORK("XNetGetXnAddrPlatform()");
	a2 = XNET_XNADDR_PLATFORM_WINPC;
	return 0;
}

// #83
int WINAPI XNetGetSystemLinkPort(DWORD *a1)
{
	LOG_TRACE_NETWORK("XNetGetSystemLinkPort()");
	if (a1)
		*a1 = 3074;
	return 0;
}

// #84: XNetSetSystemLinkPort
DWORD WINAPI XNetSetSystemLinkPort(DWORD a1)
{
	LOG_TRACE_NETWORK("XNetSetSystemLinkPort  (a1 = {:x})", a1);
	return 0;
}
