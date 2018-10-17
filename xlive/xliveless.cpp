// -No Copyright- 2010 Stanislav "listener" Golovin
// This file donated to the public domain
#include "stdafx.h"
#include "resource.h"
#include <iostream>
#include <sstream>
#include <codecvt>
#include <unordered_map>
#include "Globals.h"
#include "H2MOD/discord/DiscordInterface.h"
#include "H2MOD\Modules\Config\Config.h"
#include "XLive\Networking\ServerList.h"
#include "H2MOD\Modules\Achievements\Achievements.h"
#include "xlivedefs.h"
#include "XLive\UserManagement\CUser.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include "XLive\Networking\XLiveQoS.h"
#include "XLive\XAM\xam.h"
#include "XLive\achievements\XAchievements.h"
#include "XLive\xbox\xbox.h"
#include "XLive\XUser\XUser.h"

//#include "XLive\Globals.h"
using namespace std;

HANDLE g_dwFakePData = (HANDLE) -2;
HANDLE g_dwFakeContent = (HANDLE) -2;
HANDLE g_dwMarketplaceContent = (HANDLE) -2;

INT num_players;

WSADATA wsaData;


extern CHAR g_profileDirectory[];

extern void InitInstance();
extern void ExitInstance();
extern std::wstring dlcbasepath;


int dlcinit = 0;


#define DEBUG_WAIT 0


char filename[1024];
char str[8192];
WCHAR strw[8192];
WCHAR exePath[8192];




XMARKETPLACE_CONTENTOFFER_INFO marketplace[100];
int marketplaceCount = 0;

XMARKETPLACE_CONTENTOFFER_INFO marketplaceDlc[100];
int marketplaceDlcCount = 0;

int marketplaceEnumerate = 0;


/*
VOID Local_Storage_W( int player, WCHAR *str )
{
	WCHAR temp2[512];



	mbstowcs( temp2, g_profileDirectory, 512 );

	wcscpy( str, temp2 );
	wcscat( str, L"\\" );
	CreateDirectoryW( str, NULL );



	mbstowcs( temp2, g_szUserName[player], 256 );

	wcscat( str, temp2 );
	CreateDirectoryW( str, NULL );
}



VOID Local_Storage_A( int player, CHAR *strA )
{
	strcpy( strA, g_profileDirectory );
	strcat( strA, "\\" );
	CreateDirectoryA( strA, NULL );


	strcat( strA, g_szUserName[player] );
	CreateDirectoryA( strA, NULL );
}
*/


VOID ReadLine( FILE *fp, char *str )
{
	int lcv;


	lcv = 0;
	while( !feof(fp) )
	{
		char buf[2];


		// UTF-16LE
		fread( &buf, 1, 2, fp );


		str[lcv] = buf[0];


		if( buf[0] == 0x0D )
			str[lcv] = 0;

		if( buf[0] == 0x0A )
		{
			str[lcv] = 0;
			break;
		}


		lcv++;
	}
}


BOOL SkipText( FILE *fp, char *str )
{
	char line[1024];


	// Header
	while(1)
	{
		// UTF-16LE
		ReadLine( fp, line );



		//MessageBoxA(0,line,str,MB_OK );

		if( strcmp( line, str ) == 0 )
			return TRUE;

		if( feof(fp) )
			return FALSE;
	}
}

WCHAR dlcpath[256][512];
BOOL SetDlcBasepath( int num )
{
	static int pathinit = -1;


	if( pathinit == -1 )
	{
		pathinit = 0;
		marketplaceDlcCount = 0;


		WCHAR fileString[2048];
		WIN32_FIND_DATA ffd;
		HANDLE hFind = INVALID_HANDLE_VALUE;




		wcscpy( fileString, L"DLC\\*" );


		// Find the first file in the directory.
		hFind = FindFirstFile( fileString, &ffd );
		if( INVALID_HANDLE_VALUE != hFind )
		{
			// List all the files in the directory with some info about them.
			do
			{
				if( ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
				{
					if( wcscmp( ffd.cFileName, L"." ) == 0 )
						continue;
					if( wcscmp( ffd.cFileName, L".." ) == 0 )
						continue;



					// check valid folder
					wstring wnum;
					wnum = L"DLC\\";
					wnum += ffd.cFileName;
					wnum += L"\\content.xbx";


					// check file exists
					FILE *fp_dlc;
					fp_dlc = _wfopen( wnum.c_str(), L"rb" );
					if( !fp_dlc )
						continue;


					//TRACE( "- %s", wnum.c_str() );


					wcscpy( dlcpath[pathinit], ffd.cFileName );
					TRACE( "- DLC%d = %s", pathinit, ffd.cFileName );



					// skip UTF-16LE header
					fseek( fp_dlc, 2, SEEK_SET );


					SkipText( fp_dlc, "[All]" );


					// Premium
					ReadLine( fp_dlc, str );


					// TitleID
					ReadLine( fp_dlc, str );
					sscanf( str, "TitleID=%X", &marketplaceDlc[pathinit].dwTitleID );
					TRACE( "=== TitleID = %X", marketplaceDlc[pathinit].dwTitleID );


					// ContentPackageType, LicenseBits, BaseVersion, UpdateVersion, ThumbnailImage
					ReadLine( fp_dlc, str );
					ReadLine( fp_dlc, str );
					ReadLine( fp_dlc, str );
					ReadLine( fp_dlc, str );
					ReadLine( fp_dlc, str );


					// OfferingID
					ReadLine( fp_dlc, str );
					sscanf( str, "OfferingID=%I64X", &marketplaceDlc[pathinit].qwOfferID );
					TRACE( "=== OfferingID = %I64X", marketplaceDlc[pathinit].qwOfferID );


					// AdminFriendlyName, TitleName, PurchaseOnceOnly
					ReadLine( fp_dlc, str );
					ReadLine( fp_dlc, str );
					ReadLine( fp_dlc, str );


					// ContentFlags
					ReadLine( fp_dlc, str );


					// ContentID
					ReadLine( fp_dlc, str );


					// convert to hex format
					for( int lcv2 = 0; lcv2 < 40; lcv2 += 2 )
					{
						char a,b;

						a = *( str + strlen("ContentID=") + ( lcv2 + 0 ) );
						b = *( str + strlen("ContentID=") + ( lcv2 + 1 ) );

						if( a >= '0' && a <= '9' ) a -= '0' - 0;
						if( a >= 'A' && a <= 'F' ) a -= 'A' - 10;

						if( b >= '0' && b <= '9' ) b -= '0' - 0;
						if( b >= 'A' && b <= 'F' ) b -= 'A' - 10;




						marketplaceDlc[pathinit].contentId[ lcv2 / 2 ] = a * 16 + b;
					}


					strw[0] = 0;
					for( int lcv2 = 0; lcv2 < 20; lcv2++ )
						wsprintf( strw, L"%s%02X", strw, marketplaceDlc[pathinit].contentId[lcv2] );
					strw[40] = 0;

					TRACE( "=== ContentID = %s", strw );



					fclose( fp_dlc );


					marketplaceDlcCount++;
					pathinit++;
				}
			} while( FindNextFile(hFind, &ffd) != 0 );


			FindClose(hFind);
		}
	}


	if( num > pathinit )
		return FALSE;



	dlcbasepath = L"DLC\\";
	dlcbasepath += dlcpath[num];


	return TRUE;
}



void Check_Overlapped( PXOVERLAPPED pOverlapped )
{
	if( !pOverlapped ) return;


//	TRACE( "- async routine" );


	if( pOverlapped->hEvent )
	{
		//TRACE( "- hEvent = %X", pOverlapped->hEvent );

		SetEvent( pOverlapped->hEvent );
	}


	if( pOverlapped->pCompletionRoutine )
	{
		//TRACE( "- pCompletionRoutine = %X", pOverlapped->pCompletionRoutine );


		pOverlapped->pCompletionRoutine( pOverlapped->InternalLow, pOverlapped->InternalHigh, pOverlapped->dwCompletionContext );
	}
}


// #############################################################
// #############################################################
// #############################################################
// #############################################################
// #############################################################

// === Start of xlive functions ===



//TODO: Move XWSA and XSocket functions to XSocket.cpp

// #1: XWSAStartup
int WINAPI XWSAStartup (WORD wVersionRequested, LPWSADATA lpWsaData)
{
   // TRACE("XWSAStartup(%u, %p)", wVersionRequested, lpWsaData);
    return WSAStartup(wVersionRequested, lpWsaData);
}

// #2: XWSACleanup
int WINAPI XWSACleanup ()  	// XWSACleanup
{
    TRACE("XWSACleanup");
    return WSACleanup();
}



// #4
int WINAPI XSocketClose (SOCKET s)
{
    TRACE("XSocketClose");
    return closesocket(s);
}

// #5: XSocketShutdown
int WINAPI XSocketShutdown (SOCKET s, int how)
{
    TRACE("XSocketShutdown");
    return shutdown(s, how);
}

// #6: XSocketIOCTLSocket
int WINAPI XSocketIOCTLSocket (SOCKET s, __int32 cmd, u_long *argp)
{
    //TRACE("XSocketIOCTLSocket");
    return ioctlsocket(s, cmd, argp);
}

// #7: XSocketSetSockOpt
int WINAPI XSocketSetSockOpt (SOCKET s, int level, int optname, const char *optval, int optlen)
{
	int ret;

    TRACE("XSocketSetSockOpt  (socket = %X, level = %d, optname = %d, optval = %s, optlen = %d)",
			s, level, optname, optval ? optval : "", optlen );

		if ((level & SO_BROADCAST) > 0)
		{

			TRACE("XSocketSetSockOpt - SO_BROADCAST");
		}
		ret = setsockopt(s, level, optname, optval, optlen);
		if (ret == SOCKET_ERROR)
		{
			TRACE("XSocketSetSockOpt - SOCKET_ERROR");
		}
		
		TRACE( "- ret = %X", ret );
		return ret;
}

// #8: XSocketGetSockOpt
int WINAPI XSocketGetSockOpt(SOCKET s, int level, int optname, char *optval, int *optlen)
{
    //TRACE("XSocketGetSockOpt");
    return getsockopt(s, level, optname, optval, optlen);
}

// #9: XSocketGetSockName
int WINAPI XSocketGetSockName (SOCKET s, struct sockaddr *name, int *namelen)
{
    TRACE("XSocketGetSockName");
    return getsockname(s, name, namelen);
}

// #10
int WINAPI XSocketGetPeerName(SOCKET s, struct sockaddr *name, int *namelen)
{
    TRACE("XSocketGetPeerName");
    return getpeername(s, name, namelen);
}




// #12: XSocketConnect
int WINAPI XSocketConnect(SOCKET s, const struct sockaddr *name, int namelen)
{
    TRACE("XSocketConnect  (socket = %X, name = %X, namelen = %d)",
			s, name, namelen );

		return connect(s, name, namelen);
}


// #13: XSocketListen
int WINAPI XSocketListen(SOCKET s, int backlog)
{
    TRACE("XSocketListen  (socket = %X, backlog = %X)",
			s, backlog );

    return listen(s, backlog);
}


// #14: XSocketAccept
SOCKET WINAPI XSocketAccept(SOCKET s, struct sockaddr *addr, int *addrlen)
{
	static int print = 0;


	if( print < 25 )
	{
    TRACE("XSocketAccept  (socket = %X, addr = %X, addrlen = %d)",
			s, addr, *addrlen);

		print++;
	}
	
	
	return accept(s, addr, addrlen);
}


// #15: XSocketSelect
int WINAPI XSocketSelect(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, const struct timeval *timeout)
{
	static int print = 0;


	if( print < 15 )
	{
    TRACE("XSocketSelect");

		print++;
	}

    return select(nfds, readfds, writefds, exceptfds, timeout);
}

// #16
BOOL WINAPI XSocketWSAGetOverlappedResult(SOCKET s, LPWSAOVERLAPPED lpOverlapped, LPDWORD lpcbTransfer, BOOL fWait, LPDWORD lpdwFlags)
{
    TRACE("XSocketWSAGetOverlappedResult  (socket = %X, lpWSAOverlapped = %X, lpcbTransfer = %X, fWait = %d, lpdwFlags = %X)",
			s, lpOverlapped, lpcbTransfer, fWait, lpdwFlags );

    return WSAGetOverlappedResult(s, lpOverlapped, lpcbTransfer, fWait, lpdwFlags);
}

// #17
BOOL WINAPI XSocketWSACancelOverlappedIO(HANDLE hFile)
{
    TRACE("XSocketWSACancelOverlappedIO");
    return CancelIo(hFile);
}

// #18: XSocketRecv
int WINAPI XSocketRecv (SOCKET s, char * buf, int len, int flags)
{
    TRACE("XSocketRecv");
    return recv(s, buf, len, flags);
}

// #19
int WINAPI XSocketWSARecv(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesRecvd, LPDWORD lpFlags, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
    TRACE("XSocketWSARecv");
    return WSARecv(s, lpBuffers, dwBufferCount, lpNumberOfBytesRecvd, lpFlags, lpOverlapped, lpCompletionRoutine);
}


// #21
int WINAPI XSocketWSARecvFrom(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesRecvd, LPDWORD lpFlags, struct sockaddr *lpFrom, LPINT lpFromlen, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
    TRACE("XSocketWSARecvFrom");
    return WSARecvFrom(
               s,
               lpBuffers,
               dwBufferCount,
               lpNumberOfBytesRecvd,
               lpFlags,
               lpFrom,
               lpFromlen,
               lpOverlapped,
               lpCompletionRoutine);
}

// #22: XSocketSend
int WINAPI XSocketSend(SOCKET s, const char *buf, int len, int flags)
{
    TRACE("XSocketSend");
    return send(s, buf, len, flags);
}


// #65: XNetConnect
int WINAPI XNetConnect(const IN_ADDR ina)
{
	return 0;
}

// #23
int WINAPI XSocketWSASend(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesSent, DWORD dwFlags, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
    TRACE("XSocketWSASend");
    return WSASend(s, lpBuffers, dwBufferCount, lpNumberOfBytesSent, dwFlags, lpOverlapped, lpCompletionRoutine);
}

// #25
int WINAPI XSocketWSASendTo(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesSent, DWORD dwFlags, sockaddr *lpTo, int iTolen, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
    TRACE("XSocketWSASendTo");
    lpTo->sa_family = AF_INET;
    return WSASendTo(
               s,
               lpBuffers,
               dwBufferCount,
               lpNumberOfBytesSent,
               dwFlags,
               lpTo,
               iTolen,
               lpOverlapped,
               lpCompletionRoutine);
}

// #26: XSocketInet_Addr
LONG WINAPI XSocketInet_Addr(const char *cp)
{
    TRACE("XSocketInet_Addr");
    return inet_addr(cp);
}

// #27: XWSAGetLastError
INT WINAPI XSocketWSAGetLastError ()
{
	static int print = 0;

	int ret = WSAGetLastError();

	if( print < 15 )
	{
		//("XSocketWSAGetLastError");
		//TRACE("XSocketWSAGetLastError Ret = %i", ret);

		print++;
	}


    return ret;
}

// #28
VOID WINAPI XSocketWSASetLastError(int iError)
{
    TRACE("XSocketWSASetLastError");
    WSASetLastError(iError);
}

// #29
HANDLE WINAPI XSocketWSACreateEvent()
{
    TRACE("XSocketWSACreateEvent");
    return WSACreateEvent();
}

// #30
BOOL WINAPI XSocketWSACloseEvent(HANDLE hEvent)
{
    TRACE("XSocketWSACloseEvent");
    return WSACloseEvent(hEvent);
}

// #31
BOOL WINAPI XSocketWSASetEvent(HANDLE hEvent)
{
    TRACE("XSocketWSASetEvent");
    return WSASetEvent(hEvent);
}

// #32
BOOL WINAPI XSocketWSAResetEvent(HANDLE hEvent)
{
    TRACE("XSocketWSAResetEvent");
    return WSAResetEvent(hEvent);
}

// #33
DWORD WINAPI XSocketWSAWaitForMultipleEvents(DWORD cEvents, HANDLE *lphEvents, BOOL fWaitAll, DWORD dwTimeout, BOOL fAlertable)
{
    TRACE("XSocketWSAWaitForMultipleEvents");
    return WSAWaitForMultipleEvents(cEvents, lphEvents, fWaitAll, dwTimeout, fAlertable);
}

// #34
int WINAPI XSocketWSAFDIsSet(SOCKET fd, fd_set *a2)
{
    TRACE("XSocketWSAFDIsSet");
    return __WSAFDIsSet(fd, a2);
}

// #35
int WINAPI XSocketWSAEventSelect(SOCKET s, HANDLE hEventObject, __int32 lNetworkEvents)
{
    TRACE("XSocketWSAEventSelect");
    return WSAEventSelect(s, hEventObject, lNetworkEvents);
}

// #37: XSocketHTONL
u_long WINAPI XSocketHTONL(u_long hostlong)
{
	u_long ret = htonl(hostlong);
	
	return ret;
}

// #38: XSocketNTOHS
u_short WINAPI XSocketNTOHS (u_short netshort)
{
	u_short ret;

	ret = ntohs(netshort);

	return ret;
}


// #39: XSocketNTOHL
u_long WINAPI XSocketNTOHL(u_long netlong)
{	
	u_long ret = ntohl(netlong);
	
	return ret;
}


// #40: XSocketHTONS
u_short WINAPI XSocketHTONS (u_short a1)
{

	u_short ret = htons(a1);

	return ret;
}

// TODO: Move XNet* functions to xnet.cpp

// #51: XNetStartup
int WINAPI XNetStartup (void *a1)
{
    //TRACE("XNetStartup  (a1 = %X)", a1);
    return 0;
}

// #58: XNetServerToInAddr
INT   WINAPI XNetServerToInAddr(const IN_ADDR ina, DWORD dwServiceId, IN_ADDR * pina)
{
    TRACE("XNetServerToInAddr");
	if(pina)
		*pina = ina;
    return 0;
}


// #59: XNetXnAddrToInAddr
INT   WINAPI XNetTsAddrToInAddr(const TSADDR * ptsa, DWORD dwServiceId, const XNKID * pxnkid, IN_ADDR * pina)
{
	TRACE("XNetTsAddrToInAddr");
	if(pina)
		*pina = ptsa->ina;
	return 0;
}


// #61: XNetInAddrToServer
INT   WINAPI XNetInAddrToServer(const IN_ADDR ina, IN_ADDR *pina)
{
	TRACE("XNetInAddrToServer");
	if(pina)
		*pina = ina;
	return 0;
}


// #62: XNetInAddrToString
INT   WINAPI XNetInAddrToString(const IN_ADDR ina, char * pchBuf, INT cchBuf)
{
	TRACE("XNetInAddrToString");
	strncpy(pchBuf,inet_ntoa(ina),cchBuf);
	return 0;
}

// #64
INT WINAPI XNetXnAddrToMachineId(const XNADDR * pxnaddr, ULONGLONG * pqwMachineId)
{
    TRACE("XNetXnAddrToMachineId");


		// ???
		return -1;


		if(pqwMachineId)
        *pqwMachineId = 0xDEADC0DE;

    return 0;
}

// #66: XNetGetConnectStatus
int WINAPI XNetGetConnectStatus (const IN_ADDR ina)
{
	return XNET_CONNECT_STATUS_CONNECTED;
}


// #67: XNetDnsLookup
int WINAPI XNetDnsLookup (const char * pszHost, DWORD hEvent, void ** ppxndns)
{
    TRACE("XNetDnsLookup");
    if (ppxndns)
        *ppxndns = NULL;
    return 1;   // ERROR
}

// #68: XNetDnsRelease
int WINAPI XNetDnsRelease (void * pxndns)
{
    TRACE("XNetDnsRelease");
    return 0;
}

// #75
DWORD WINAPI XNetGetEthernetLinkStatus()
{
	static int print = 0;


	if( print < 15 )
	{
		//("XNetGetEthernetLinkStatus");
		//TRACE("- active: 100 mbps, full duplex");

		print++;
	}
	
	return XNET_ETHERNET_LINK_ACTIVE | XNET_ETHERNET_LINK_100MBPS | XNET_ETHERNET_LINK_FULL_DUPLEX;
}


// #76
DWORD WINAPI XNetGetBroadcastVersionStatus(DWORD a1)
{
	TRACE("XNetGetBroadcastVersionStatus");
	return 0;
}

// #78
INT WINAPI XNetGetOpt(DWORD dwOptId, BYTE * pbValue, DWORD * pdwValueSize)
{
    TRACE("XNetGetOpt");
    return WSAEINVAL;
}


// #79: XNetSetOpt
INT WINAPI XNetSetOpt(DWORD dwOptId, const BYTE * pbValue, DWORD dwValueSize)
{
	TRACE("XNetSetOpt");
	return WSAEINVAL;
}

// #80
int WINAPI XNetStartupEx(int a1, int a2, int a3)
{
	TRACE("XNetStartupEx");
	return 0;
}

// #81
int WINAPI XNetReplaceKey(const XNKID *pxnkidUnregister, const XNKID *pxnkidReplace)
{
	TRACE("XNetReplaceKey( pxnkidUnregister: %08X, pxnkidreplace: %08X)",pxnkidUnregister, pxnkidReplace);
	return 0;
}

// #82
int WINAPI XNetGetXnAddrPlatform(in_addr *a1, int a2)
{
	TRACE("XNetGetXnAddrPlatform");
	a2 = XNET_XNADDR_PLATFORM_WINPC;
	return 0;
}

// #83
int WINAPI XNetGetSystemLinkPort(DWORD *a1)
{
	TRACE("XNetGetSystemLinkPort");
	if(a1)
		*a1 = 0x00000C02;
	return 0;
}

// #84: XNetSetSystemLinkPort
DWORD WINAPI XNetSetSystemLinkPort( DWORD a1 )
{
    TRACE("XNetSetSystemLinkPort  (a1 = %X)", a1);
    return 0;
}

// #1082: XGetOverlappedExtendedError
DWORD WINAPI XGetOverlappedExtendedError(PXOVERLAPPED pOverlapped)
{
	if( pOverlapped == 0 )
	{
		return ERROR_INVALID_PARAMETER;
	}

	return pOverlapped->dwExtendedError;
}


// #1083: XGetOverlappedResult
DWORD WINAPI XGetOverlappedResult(PXOVERLAPPED pOverlapped, LPDWORD pResult, BOOL bWait)
{
	//TRACE("XGetOverlappedResult  (pOverlapped = %X, pResult = %X, bWait = %d)  (internalLow = %X, internalHigh = %X)",
	//	pOverlapped, pResult, bWait, pOverlapped->InternalLow, pOverlapped->InternalHigh );

	if (bWait)
	{
		while (pOverlapped->InternalLow == ERROR_IO_INCOMPLETE)
		{
		}
	}

	if( pResult )
	{
		*pResult = pOverlapped->InternalHigh;

		//TRACE( "- result = %d", *pResult );
	}


	//Check_Overlapped( pOverlapped );


	//TRACE( "- code = %X", pOverlapped->InternalLow );
	return pOverlapped->InternalLow;
}

// #5003: XLiveUninitialize
int WINAPI XLiveUninitialize ()
{
    TRACE("XLiveUninitialize");
    return 0;
}



// #5010: XLiveLess_5010
int WINAPI XLiveRegisterDataSection(int a1, int a2, int a3)
{
	TRACE("XLiveRegisterDataSection - a1: %08X,a2: %08X, a3: %08X", a1, a2, a3);
	return 0;
}

// #5011 XLiveLess_5011
int WINAPI XLiveUnregisterDataSection(int a1)
{
	TRACE("XLiveUnregisterDataSection - a1: %08X", a1);
	return 0;
}

// #5012 XLiveLess_5012
int WINAPI XLiveUpdateHashes(int a1, int a2)
{
	TRACE("XLiveUpdateHashes - a1: %08X,a2: %08x", a1, a2);
	return 0;
}



// === replacement ===
struct FakePBuffer {
	HANDLE id;
	DWORD dwSize;
	DWORD magic;
	LPBYTE pbData;
};


// #5016: XLivePBufferAllocate
LONG WINAPI XLivePBufferAllocate (DWORD size, FakePBuffer **pBuffer)
{
	static int print = 0;


	if( print < 35 )
	{
		print++;


		//TRACE("XLivePBufferAllocate  (XEncryptedAlloc) (size = %d, pBuffer = %X)",
		//	size, pBuffer);
	}


	if(!pBuffer)
		return E_OUTOFMEMORY;


	HANDLE hHeap = GetProcessHeap();

	//initialize fake buffer
	*pBuffer = (FakePBuffer*)HeapAlloc(hHeap,HEAP_ZERO_MEMORY,sizeof(FakePBuffer));

	(*pBuffer)->dwSize = size;
	(*pBuffer)->id = g_dwFakePData = CreateMutex(NULL,NULL,NULL);
	(*pBuffer)->magic = 0xDEADC0DE;

	//initialize real buffer inside fake buffer
	(*pBuffer)->pbData = (PBYTE)HeapAlloc(hHeap,HEAP_ZERO_MEMORY,size);


	if(!*pBuffer)
	{
		TRACE("ERROR: XLivePBufferAllocate unable to allocate %d bytes", size);
		return E_OUTOFMEMORY;
	}


	if( print < 35 )
	{
		TRACE("- buffer_new = %X, size = %d, handle = %X",
			*pBuffer, size, g_dwFakePData );
	}


	return 0;
}


// #5017: XLivePBufferFree
DWORD WINAPI XLivePBufferFree (FakePBuffer * pBuffer)
{
	static int print = 0;


	if( print < 35 )
	{
		print++;

	
		TRACE("XLivePBufferFree  (pBuffer = %X)", pBuffer);
	}


	if( !pBuffer ) return 0;


	if( pBuffer->magic != 0xDEADC0DE )
	{
		TRACE("- bad magic" );
		return 0;
	}


	HANDLE hHeap = GetProcessHeap();

	HeapFree(hHeap,NULL,pBuffer->pbData);
	HeapFree(hHeap,NULL,pBuffer);

	return 0;
}


// #5022: XLiveGetUpdateInformation
int WINAPI XLiveGetUpdateInformation (DWORD)
{
    TRACE("XLiveGetUpdateInformation");
    return -1; // no update
}


// #5024: XLiveUpdateSystem
int WINAPI XLiveUpdateSystem (DWORD)
{
    TRACE("XLiveUpdateSystem");
    return -1; // no update
}




// #5031 XLiveSetDebugLevel
int WINAPI XLiveSetDebugLevel (DWORD xdlLevel, DWORD * pxdlOldLevel)
{
    TRACE("XLiveSetDebugLevel (%d)", xdlLevel);
    return 0;
}


// #5251: XCloseHandle
BOOL WINAPI XCloseHandle (HANDLE hObject)
{
	//TRACE("XCloseHandle  (hObject = %X)", hObject );
	BOOL ret = 0;
	
	if(hObject) ret = CloseHandle(hObject);
	
	return ret;
}





// #5254: XCancelOverlapped
int WINAPI XCancelOverlapped (DWORD a1)
{
    TRACE("XCancelOverlapped  (a1 = %X)", a1);


    return 0;
}
 
// #5256: XEnumerate
int WINAPI XEnumerate(HANDLE hEnum, CHAR *pvBuffer, DWORD cbBuffer, PDWORD pcItemsReturned, PXOVERLAPPED pOverlapped)
{
	//static int print = 0;


	//if( print < 100 )
	//{
		//TRACE("XEnumerate  (hEnum = %X, pvBuffer = %X, cbBuffer = %X, pcItemsReturned = %X, pOverlapped = %X)",
		//	hEnum, pvBuffer, cbBuffer, pcItemsReturned, pOverlapped);

		//print++;
	//}



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
		TRACE( "- NULL ptr" );


		return ERROR_INVALID_PARAMETER;
	}



	if( hEnum == g_dwFakeContent && dlcinit != 0x7FFFFFFF )
	{
		int total;


		total = 0;
		while( dlcinit < 256 && dlcinit < marketplaceDlcCount )
		{
			XCONTENT_DATA aaa;


			// check max
			if( total >= cbBuffer )
			{
			//	if( print < 100 )
			//		TRACE( "- Full enumerate" );
				break;
			}



			// Credit: virusek, JorjVirus69
			aaa.ContentNum = dlcinit;
			aaa.ContentPackageType = 2;
			memcpy( &aaa.TitleId, &marketplaceDlc[ dlcinit ].dwTitleID, sizeof(aaa.TitleId) );
			memcpy( &aaa.ContentId, &marketplaceDlc[ dlcinit ].contentId, sizeof(aaa.ContentId) );



			TRACE( "- [%d] DLC = %08X", dlcinit, *((DWORD *) aaa.ContentId) );
			dlcinit++;



			if( async == FALSE )
				(*pcItemsReturned)++;

			else
				pOverlapped->InternalHigh++;



			if(pvBuffer) 
			{
				memcpy(pvBuffer,&aaa,sizeof(aaa));
				
				
				pvBuffer += 32;
				total += 32;
			}
		}



		if( async == FALSE )
		{
			if( *pcItemsReturned == 0 )
				dlcinit = 0x7fffffff;
		}

		else
		{
			if( pOverlapped->InternalHigh == 0 )
				dlcinit = 0x7fffffff;
		}
	}




	if( hEnum == g_dwFakeAchievementContent && achieveinit != 0x7FFFFFFF )
	{
		int total;


		total = 0;
		for( 0; achieveinit < 42; achieveinit++ )
		{
			XACHIEVEMENT_DETAILS aaa;


			// check max
			if( total >= cbBuffer )
				break;


			if( achievementList[ achieveinit ] == 1 )
			{
				SYSTEMTIME systemTime;
				FILETIME fileTime;


				GetSystemTime( &systemTime );
				SystemTimeToFileTime( &systemTime, &fileTime );


				aaa.dwId = achieveinit;
				aaa.pwszLabel = L"";
				aaa.pwszDescription = L"";
				aaa.pwszUnachieved = L"";
				aaa.dwImageId = 0;
				aaa.dwCred = 0;
				aaa.ftAchieved = fileTime;
				aaa.dwFlags = XACHIEVEMENT_DETAILS_ACHIEVED_ONLINE | XACHIEVEMENT_DETAILS_ACHIEVED;



				if( async == FALSE )
					(*pcItemsReturned)++;

				else
					pOverlapped->InternalHigh++;



				if(pvBuffer) 
				{
					memcpy(pvBuffer,&aaa,sizeof(aaa));
					pvBuffer += sizeof(aaa);


					total += sizeof(aaa);
				}
			}
		}



		if( async == FALSE )
		{
			if( *pcItemsReturned == 0 )
				achieveinit = 0x7fffffff;
		}

		else
		{
			if( pOverlapped->InternalHigh == 0 )
				achieveinit = 0x7fffffff;
		}
	}



	if( hEnum == g_dwMarketplaceContent && marketplaceEnumerate < marketplaceCount )
	{
		// TODO: check full buffer
		memcpy( pvBuffer, &marketplace, sizeof(XMARKETPLACE_CONTENTOFFER_INFO) * marketplaceCount );


		if( async == FALSE )
			*pcItemsReturned = marketplaceCount;

		else
			pOverlapped->InternalHigh = marketplaceCount;


		marketplaceEnumerate += marketplaceCount;
	}

	if (hEnum == ServerEnum)
	{
		pOverlapped->InternalHigh = ERROR_IO_INCOMPLETE;
		pOverlapped->InternalLow = ERROR_IO_INCOMPLETE;

		if (!LiveManager.GetRunning() && LiveManager.servers_left == 0)
			LiveManager.GetServers(pOverlapped, pvBuffer);


		return ERROR_IO_PENDING;

		/*
		while (LiveManager.GetRunning() == true && LiveManager.GetTotalServers() == 0 || LiveManager.servers_left > 0)
		{
			
		}

		if (LiveManager.GetRunning() == false && LiveManager.GetTotalServers() == 0)
		{
			LiveManager.GetServers(pOverlapped,pvBuffer);

		}
		
		if (LiveManager.GetRunning() == false && LiveManager.GetTotalServers() > 0 && LiveManager.servers_left == 0)
		{
			for (int i = 0; i < LiveManager.GetTotalServers(); i++)
			{
				memcpy(pvBuffer + (sizeof(_XLOCATOR_SEARCHRESULT) * i), &LiveManager.servers[i], sizeof(_XLOCATOR_SEARCHRESULT));
			}

			if (async == FALSE)
				*pcItemsReturned = LiveManager.GetTotalServers();
			else
				pOverlapped->InternalHigh = LiveManager.GetTotalServers();

				LiveManager.total_servers = 0;

			
		}*/
	
	}

	if( async == FALSE )
	{
		if( *pcItemsReturned )
		{
			//if( print < 100 )
			//	TRACE("- ItemsReturned = %d", *pcItemsReturned);
	
			return ERROR_SUCCESS;
		}

		else
		{
			//if( print < 100 )
			//	TRACE("- NO_MORE_FILES");
			*pcItemsReturned = 0;
			return ERROR_SUCCESS;
		}
	}

	else
	{
		if( pOverlapped->InternalHigh )
		{
			//if( print < 100 )
				//TRACE( "- async items = %d", pOverlapped->InternalHigh );


			pOverlapped->InternalLow = ERROR_SUCCESS;
			pOverlapped->dwExtendedError = ERROR_SUCCESS;


			Check_Overlapped( pOverlapped );


			return ERROR_IO_PENDING;
		}

		else
		{
			//if( print < 100 )
			//	TRACE( "- async = NO_MORE_FILES" );


			pOverlapped->InternalLow = ERROR_NO_MORE_FILES;
			pOverlapped->dwExtendedError = ERROR_NO_MORE_FILES;

			
			Check_Overlapped( pOverlapped );


			return ERROR_IO_PENDING;
		}
	}
}


// #5258: XLiveSignout
int WINAPI XLiveSignout(int a1)
{
	TRACE("XLiveSignout");
	return 0;
}


// #5259: XLiveSignin
HRESULT WINAPI XLiveSignin (PWSTR pszLiveIdName, PWSTR pszLiveIdPassword, DWORD dwFlags, PXOVERLAPPED pOverlapped)
{
	TRACE("XLiveSignin");


	sys_ui = -1;


	return S_OK;
}


// #5297: XLiveInitializeEx
int WINAPI XLiveInitializeEx (void * pXii, DWORD dwVersion)
{
	InitInstance();

#if 0
		while(1)
		Sleep(1);
#endif


  TRACE("XLiveInitializeEx  (a1 = %X, a2 = %X)", pXii, dwVersion);
  return 0;
}

//TODO: Move XSession* to XSession.cpp
// #5300: XSessionCreate
LONG WINAPI XSessionCreate( DWORD dwFlags, DWORD dwUserIndex, DWORD dwMaxPublicSlots, DWORD dwMaxPrivateSlots, ULONGLONG *pqwSessionNonce, PXSESSION_INFO pSessionInfo, PXOVERLAPPED pOverlapped, HANDLE *phEnum )
{
	TRACE("XSessionCreate  (flags = %X, userIndex = %d, maxPublicSlots = %d, maxPrivateSlots = %d, sessionNonce = %X, pSessionInfo = %X, pOverlapped = %X, handle = %X)",
		dwFlags, dwUserIndex, dwMaxPublicSlots, dwMaxPrivateSlots, pqwSessionNonce, pSessionInfo, pOverlapped, phEnum );


	if(phEnum) *phEnum = CreateMutex(NULL,NULL,NULL);

	if ((dwFlags & XSESSION_CREATE_HOST) > 0)
	{
		TRACE("XSessionCreate - XSESSION_CREATE_HOST");

	}

	if ((dwFlags & XSESSION_CREATE_USES_ARBITRATION) > 0)
	{
		TRACE("XSessionCreate - XSESSION_CREATE_USES_ARBITRATION");
	}

	if ((dwFlags & XSESSION_CREATE_USES_PEER_NETWORK) > 0)
	{
		TRACE("XSessionCreate - XSESSION_CREATE_USES_PEER_NETWORK");
	}

	if ((dwFlags & XSESSION_CREATE_USES_PRESENCE) > 0)
	{
		TRACE("XSessionCreate - XSESSION_CREATE_USES_PRESENCE");
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



	TRACE( "- handle = %X", *phEnum );


	if( pOverlapped == 0 )
		return ERROR_SUCCESS;


	pOverlapped->InternalLow = ERROR_SUCCESS;
	pOverlapped->dwExtendedError = ERROR_SUCCESS;

	if (H2Config_voice_chat) {
		if (gameManager->isHost()) {
			if (server == NULL) {
				//TODO: move into method
				server = new TSServer(true);
				server->setPort(H2Config_base_port + 7);
				//startup the teamspeak client
				client = new TSClient(true);

				//only player 1 gets to use voice, guests don't
				WCHAR strw[32];
				//needs to live on the heap for the duration of the entire process, cause we reuse ts clients to connect to different ts servers
				char* strw3 = new char[16];
				wsprintf(strw, L"%I64x", xFakeXuid[0]);
				wcstombs(strw3, strw, 32);
				client->setNickname(strw3);

			}
			server->startListening();

			//set the local loopback address
			char strAddr[] = "127.0.0.1";
			DWORD ip = inet_addr(strAddr);
			clientMachineAddress.S_un.S_addr = ip;

			client->setServerAddress(clientMachineAddress);
			client->setServerPort(H2Config_base_port + 7);
			client->startChatting();
		}
		else {
			if (client == NULL) {
				//TODO: move into method
				//startup the teamspeak client
				client = new TSClient(true);

				//only player 1 gets to use voice, guests don't
				WCHAR strw[8192];
				//needs to live on the heap for the duration of the entire process, cause we reuse ts clients to connect to different ts servers
				char* strw3 = new char[4096];
				wsprintf(strw, L"%I64x", xFakeXuid[0]);
				wcstombs(strw3, strw, 8192);
				client->setNickname(strw3);

			}
			client->setServerAddress(join_game_xn.ina);
			client->setServerPort(ntohs(join_game_xn.wPortOnline) + 7);
			client->startChatting();
		}
	}

	gameManager->start();

	Check_Overlapped( pOverlapped );

	return ERROR_IO_PENDING;
}


// #5303: XStringVerify
DWORD WINAPI XStringVerify( DWORD dwFlags, const CHAR *szLocale, DWORD dwNumStrings, const STRING_DATA *pStringData, DWORD cbResults, STRING_VERIFY_RESPONSE *pResults, PXOVERLAPPED pOverlapped )
{
	TRACE("XStringVerify  (dwFlags = %X, szLocale = %X, dwNumStrings = %d, pStringData = %X, cbresults = %d, pResults = %X, pXOverlapped = %X)",
		dwFlags, szLocale, dwNumStrings, pStringData, cbResults, pResults, pOverlapped );



	if( pResults )
	{
		pResults->wNumStrings = dwNumStrings;
		pResults->pStringResult = (HRESULT *) ((BYTE *) pResults + sizeof(STRING_VERIFY_RESPONSE));


		for( int lcv = 0; lcv < dwNumStrings; lcv++ )
			pResults->pStringResult[lcv] = (HRESULT) S_OK;
	}


	if( pOverlapped )
	{
		pOverlapped->InternalLow = ERROR_SUCCESS;
		pOverlapped->dwExtendedError = ERROR_SUCCESS;


		Check_Overlapped( pOverlapped );

		return ERROR_IO_PENDING;
	}


	return ERROR_SUCCESS;
}


// #5305: XStorageUploadFromMemory
DWORD WINAPI XStorageUploadFromMemory( DWORD dwUserIndex, const WCHAR *wszServerPath, DWORD dwBufferSize, const BYTE *pbBuffer, PXOVERLAPPED pOverlapped )
{
	TRACE("XStorageUploadFromMemory  (dwUserIndex = %d, wszServerPath = %s, dwBufferSize = %X, pbBuffer = %X, pXOverlapped = %X)",
		dwUserIndex, wszServerPath, dwBufferSize, pbBuffer, pOverlapped );



	FILE *fp;

	fp = _wfopen( wszServerPath, L"wb" );
	if( fp )
	{
		fwrite( pbBuffer, 1, dwBufferSize, fp );
		fclose( fp );
	}



	if( pOverlapped )
	{
		pOverlapped->InternalLow = ERROR_SUCCESS;
		pOverlapped->dwExtendedError = ERROR_SUCCESS;


		Check_Overlapped( pOverlapped );

		return ERROR_IO_PENDING;
	}


	return ERROR_SUCCESS;
}


// #5306: XStorageEnumerate
int WINAPI XStorageEnumerate( DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD)   // XStorageEnumerate
{
    TRACE("XStorageEnumerate");
    return 0;
}


// #5309: XStorageBuildServerPathByXuid
int WINAPI XStorageBuildServerPathByXuid(int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8)
{
  TRACE("XStorageBuildServerPathByXuid  (a1 = %X, a2 = %X, a3 = %X, a4 = %X, a5 = %X, a6 = %X, a7 = %X, a8 = %X",
		a1, a2, a3, a4, a5, a6, a7, a8);

	return 0;
}



// #5311: XOnlineCleanup
int WINAPI XOnlineCleanup ()
{
    TRACE("XOnlineCleanup");
    return 0;
}


// #5312: XFriendsCreateEnumerator
DWORD WINAPI XFriendsCreateEnumerator (DWORD dwUserIndex, DWORD dwStartingIndex, DWORD dwFriendstoReturn, DWORD *pcbBuffer, HANDLE * phEnum)
{
		TRACE("XFriendsCreateEnumerator");
    
		if(pcbBuffer) *pcbBuffer = dwFriendstoReturn * sizeof(XCONTENT_DATA);
		if(phEnum)
		{
			*phEnum = CreateMutex(NULL,NULL,NULL);

			TRACE("- Handle = %X", *phEnum);
		}

		return ERROR_SUCCESS;
}


// #5313: XPresenceInitialize
int WINAPI XPresenceInitialize(int a1)
{
	TRACE("XPresenceInitialize");

	return 0;
}


// #5314: XUserMuteListQuery
int WINAPI XUserMuteListQuery (DWORD dwUserIndex, XUID XuidRemoteTalker, BOOL *pfOnMuteList)
{
	*pfOnMuteList = FALSE;
    //TRACE("XUserMuteListQuery");
    return 0;
}


// #5315: XInviteGetAcceptedInfo
int WINAPI XInviteGetAcceptedInfo (DWORD, DWORD)
{
    TRACE("XInviteGetAcceptedInfo");
    return 1;
}


// #5316: XInviteSend
int WINAPI XInviteSend (DWORD, DWORD, DWORD, DWORD, DWORD)
{
    TRACE("XInviteSend");
    return 0;
}


// #5317: XSessionWriteStats
DWORD WINAPI XSessionWriteStats (DWORD, DWORD, DWORD, DWORD, DWORD, PXOVERLAPPED pOverlapped )
{
	TRACE("XSessionWriteStats  (pOverlapped = %X)",
		pOverlapped );


	Check_Overlapped( pOverlapped );

	return ERROR_SUCCESS;
}


// #5318: XSessionStart
int WINAPI XSessionStart (HANDLE hSession, DWORD dwFlags, PXOVERLAPPED pOverlapped )
{
	TRACE("XSessionStart  (hSession = %X, dwFlags = %X, pOverlapped = %X",
		hSession, dwFlags, pOverlapped );


	if( pOverlapped )
	{
		pOverlapped->InternalLow = ERROR_SUCCESS;
		pOverlapped->dwExtendedError = ERROR_SUCCESS;
		pOverlapped->InternalHigh = 0;


		Check_Overlapped( pOverlapped );

		return ERROR_IO_PENDING;
	}

	return ERROR_SUCCESS;
}


// #5319: XSessionSearchEx
DWORD WINAPI XSessionSearchEx (DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD)
{
    TRACE("XSessionSearchEx");
    return 0;
}


// #5320: XSessionSearchByID
DWORD WINAPI XSessionSearchByID (DWORD xnkid1, DWORD xnkid2, DWORD, DWORD * pcbResultsBuffer, void *, PXOVERLAPPED pOverlapped)
{
	TRACE("XSessionSearchByID");
   
	if (pcbResultsBuffer)
		*pcbResultsBuffer = 0;
  
	
	Check_Overlapped( pOverlapped );

	return 0;
}


// #5321: XSessionSearch
DWORD WINAPI XSessionSearch (DWORD, DWORD, DWORD, WORD, WORD, void *, void *, DWORD * pcbResultsBuffer, void *, PXOVERLAPPED pOverlapped)
{
	TRACE("XSessionSearch");
  
	if (pcbResultsBuffer)
		*pcbResultsBuffer = 0;
    
	
	Check_Overlapped( pOverlapped );

	return 0;
}


// #5322: XSessionModify
DWORD WINAPI XSessionModify (DWORD, DWORD, DWORD, DWORD, DWORD)
{
    TRACE("XSessionModify");
    return 0;
}


// #5323: XSessionMigrateHost
DWORD WINAPI XSessionMigrateHost (DWORD, DWORD, DWORD, DWORD)
{
    TRACE("XSessionMigrateHost");
    return 0;
}


// #5324: XOnlineGetNatType
XONLINE_NAT_TYPE WINAPI XOnlineGetNatType ()
{
    TRACE("XOnlineGetNatType");
		TRACE("- NAT_OPEN" );
    
		
		return XONLINE_NAT_OPEN ;
}


// #5325: XSessionLeaveLocal
DWORD WINAPI XSessionLeaveLocal (DWORD, DWORD, DWORD, DWORD)
{
    TRACE("XSessionLeaveLocal");
    return 0;
}


// #5326: XSessionJoinRemote
DWORD WINAPI XSessionJoinRemote (DWORD, DWORD, DWORD, DWORD, DWORD)
{
    TRACE("XSessionJoinRemote");
    return 0;
}


// #5327: XSessionJoinLocal
DWORD WINAPI XSessionJoinLocal( HANDLE hSession, DWORD dwUserCount, const DWORD *pdwUserIndexes, const BOOL *pfPrivateSlots, PXOVERLAPPED pOverlapped )
{
	TRACE("XSessionJoinLocal  (hSession = %X, dwUserCount = %X, pdwUserIndexes = %X, pfPrivateSlots = %X, pOverlapped = %X)",
		hSession, dwUserCount, pdwUserIndexes, pfPrivateSlots, pOverlapped );


	for( int lcv = 0; lcv < dwUserCount; lcv++ )
	{
		TRACE( "- user %d = %d  (%s)", lcv+1, pdwUserIndexes[lcv], pfPrivateSlots[lcv] ? L"Private" : L"Public" );
	}


	num_players++;

	if( pOverlapped == 0 )
		return ERROR_SUCCESS;


	pOverlapped->InternalHigh = 0;
	pOverlapped->InternalLow = ERROR_SUCCESS;
	pOverlapped->dwExtendedError = ERROR_SUCCESS;


	Check_Overlapped( pOverlapped );

	return ERROR_IO_PENDING;
}


// #5328: XSessionGetDetails
DWORD WINAPI XSessionGetDetails( HANDLE hSession, PDWORD pcbResultsBuffer, PXSESSION_LOCAL_DETAILS pSessionDetails, PXOVERLAPPED pOverlapped )
{
	INT max_size;


	TRACE("XSessionGetDetails  (hSession = %X, pcbResultsBuffer = %X (%X), pSessionDetails = %X, pOverlapped = %X)",
		hSession, pcbResultsBuffer, *pcbResultsBuffer, pSessionDetails, pOverlapped );



	// max allowed
	max_size = sizeof(XSESSION_LOCAL_DETAILS) + (sessionDetails.dwMaxPrivateSlots + sessionDetails.dwMaxPublicSlots) * sizeof(XSESSION_MEMBER);
	if( *pcbResultsBuffer < max_size )
	{
		*pcbResultsBuffer = max_size;


		if( pOverlapped == 0 )
		{
			TRACE( "- ERROR_INSUFFICIENT_BUFFER = %X", max_size );
			return ERROR_INSUFFICIENT_BUFFER;
		}

		else
		{
			pOverlapped->InternalHigh = 0;
			pOverlapped->InternalLow = ERROR_INSUFFICIENT_BUFFER;
			pOverlapped->dwExtendedError = ERROR_INSUFFICIENT_BUFFER;


			Check_Overlapped( pOverlapped );
	
			return ERROR_IO_PENDING;
		}
	}




	// sent in blank template, refill values
	memset( pSessionDetails, 0xff, max_size );
	memcpy( pSessionDetails, &sessionDetails, sizeof(sessionDetails) );


	// fixme
	sessionDetails.dwReturnedMemberCount = 0;
	sessionDetails.pSessionMembers = 0;



	if( pOverlapped == 0 )
		return ERROR_SUCCESS;



	pOverlapped->InternalHigh = 0;
	pOverlapped->InternalLow = ERROR_SUCCESS;
	pOverlapped->dwExtendedError = ERROR_SUCCESS;


	Check_Overlapped( pOverlapped );
	
	return ERROR_IO_PENDING;
}


// #5329: XSessionFlushStats
int WINAPI XSessionFlushStats (DWORD, DWORD)
{
    TRACE("XSessionFlushStats");
    return 0;
}


// #5330: XSessionDelete
DWORD WINAPI XSessionDelete (DWORD, DWORD)
{
    TRACE("XSessionDelete");
		if (client != NULL) {
			client->disconnect();
		}
		if (server != NULL) {
			server->destroyVirtualServer();
		}
		mapManager->cleanup();
    return 0;
}



// #5333: XSessionArbitrationRegister
DWORD WINAPI XSessionArbitrationRegister (DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD)
{
    TRACE("XSessionArbitrationRegister");
    return 0;
}


// #5335: XTitleServerCreateEnumerator
DWORD WINAPI XTitleServerCreateEnumerator (LPCSTR pszServerInfo, DWORD cItem, DWORD * pcbBuffer, PHANDLE phEnum)
{
    TRACE("XTitleServerCreateEnumerator (cItem=> %d)", cItem);
    if(phEnum) 
		*phEnum = 0;
    return 1;
}


// #5336: XSessionLeaveRemote
DWORD WINAPI XSessionLeaveRemote (DWORD, DWORD, DWORD, DWORD)
{
    TRACE("XSessionLeaveRemote");
    return 0;
}

// #5338: XPresenceSubscribe
int WINAPI XPresenceSubscribe(int a1, int a2, int a3)
{
	TRACE("XPresenceSubscribe( a1: %08X, a2: %08X, a3: %08X", a1, a2, a3);
	return 0;
}

// #5340 XPresenceCreateEnumerator
int WINAPI XPresenceCreateEnumerator(DWORD dwUserIndex, DWORD cPeers, const XUID *pPeers, DWORD dwStartingIndex,DWORD dwPeersToReturn, DWORD *pcbBuffer, HANDLE *ph)
{
	TRACE("XPresenceCreateEnumerator(dwUserIndex: %08X, cPeers: %08X)", dwUserIndex, cPeers);
	if(pcbBuffer)
		*(DWORD *)pcbBuffer = 0;

	return 1;
}


// #5343: XSessionCalculateSkill
DWORD WINAPI XSessionCalculateSkill (DWORD, DWORD, DWORD, DWORD, DWORD)
{
    TRACE("XSessionCalculateSkill");
    return 0;
}


// #5344: XStorageBuildServerPath
DWORD WINAPI XStorageBuildServerPath( DWORD dwUserIndex, XSTORAGE_FACILITY StorageFacility,
                                      const void *pvStorageFacilityInfo, DWORD dwStorageFacilityInfoSize,
                                      LPCWSTR *pwszItemName, WCHAR *pwszServerPath, DWORD *pdwServerPathLength )
{
	//TRACE("XStorageBuildServerPath  (dwUserIndex = %d, StorageFacility = %d, pvStorageFacilityInfo = %X, dwStorageFacilityInfoSize = %X, pwszItemName = %s, pwszServerPath = %X, pdwServerPathLength = %X )",
	//	dwUserIndex, StorageFacility, pvStorageFacilityInfo, dwStorageFacilityInfoSize, pwszItemName, pwszServerPath, pdwServerPathLength );


	if( pwszServerPath )
	{
		//Local_Storage_W( 0, strw );


		wcscat( strw, L"\\Online\\" );
		//CreateDirectory( strw, NULL );


		wcscat( strw, (WCHAR *) pwszItemName );
		wcscpy( pwszServerPath, strw );
		*pdwServerPathLength = wcslen( strw ) + 1;


		//TRACE( "- %s", strw );
	}

	return 0;
}


// #5345: XStorageDownloadToMemory
DWORD WINAPI XStorageDownloadToMemory( DWORD dwUserIndex, const WCHAR *wszServerPath, DWORD dwBufferSize, const BYTE *pbBuffer, DWORD cbResults, XSTORAGE_DOWNLOAD_TO_MEMORY_RESULTS *pResults, PXOVERLAPPED pOverlapped )
{
	TRACE("XStorageDownloadToMemory  (dwUserIndex = %d, wszServerPath = %s, dwBufferSize = %X, pbBuffer = %X, cbResults = %d, pResults = %X, pXOverlapped = %X)",
		dwUserIndex, wszServerPath, dwBufferSize, pbBuffer, cbResults, pResults, pOverlapped );


	pResults->dwBytesTotal = 0;
	memcpy( &pResults->xuidOwner, &xFakeXuid[dwUserIndex], sizeof(xFakeXuid[dwUserIndex]) );


	FILE *fp;
	fp = _wfopen( wszServerPath, L"rb" );
	if( !fp )
	{
		TRACE( "- ERROR: file does not exist" );

		return -1;
	}


	int size;


	fseek( fp, 0, SEEK_END );
	size = ftell(fp);


	if( dwBufferSize < size )
	{
		TRACE( "- ERROR_INSUFFICIENT_BUFFER = %X", ftell(fp) );

		return ERROR_INSUFFICIENT_BUFFER;
	}



	fseek( fp, 0, SEEK_SET );
	fread( (void *) pbBuffer, 1, size, fp );


	pResults->dwBytesTotal = size;
	memcpy( &pResults->xuidOwner, &xFakeXuid[dwUserIndex], sizeof(xFakeXuid[dwUserIndex]) );
	//pResults->ftCreated;




	if( pOverlapped )
	{
		pOverlapped->InternalLow = ERROR_SUCCESS;
		pOverlapped->dwExtendedError = ERROR_SUCCESS;
		
		pOverlapped->InternalHigh = 0;


		Check_Overlapped( pOverlapped );
	
		return ERROR_IO_PENDING;
	}


	return ERROR_SUCCESS;
}


// #5346: XLiveLess_5346
DWORD WINAPI TitleExport_XUserEstimateRankForRating(DWORD a1, LPDWORD pdwResult, DWORD a3, void* a4, PXOVERLAPPED pOverlapped)
{
	TRACE("TitleExport_XUserEstimateRankForRating");
	return 0;
}


// #5349: XLiveProtectedVerifyFile
DWORD WINAPI XLiveProtectedVerifyFile (HANDLE hContentAccess, VOID * pvReserved, PCWSTR pszFilePath)
{
    TRACE("XLiveProtectedVerifyFile  (hContentAccess = %X, pvReserved = %X, pszFilePath = %s",
			hContentAccess, pvReserved, pszFilePath );
    return 0;
}


// #5350: XLiveContentCreateAccessHandle
DWORD WINAPI XLiveContentCreateAccessHandle (DWORD dwTitleId, PXCONTENT_DATA pContentInfo, DWORD dwLicenseInfoVersion, FakePBuffer *xebBuffer, DWORD dwOffset, HANDLE * phAccess, PXOVERLAPPED pOverlapped)
{
	TRACE("XLiveContentCreateAccessHandle  (titleId = %d, contentInfo = %X, licenseInfo = %d, pBuffer = %X, offset = %d, handle = %X, overlapped = %X",
		dwTitleId, pContentInfo, dwLicenseInfoVersion, xebBuffer, dwOffset, phAccess, pOverlapped );


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
	TRACE(" - phAccess = %X", *phAccess );


	TRACE(" - TitleID = %X", pContentInfo->TitleId );
	TRACE(" - package_num = %X", package_num );


	return 0;
}


// #5352: XLiveContentUninstall
DWORD WINAPI XLiveContentUninstall (void * pContentInfo, XUID * pxuidFor, void * pInstallCallbackParams)
{
	TRACE("XLiveContentUninstall");
	return 0;
}


// #5355: XLiveContentGetPath
LONG WINAPI XLiveContentGetPath (DWORD dwUserIndex, PXCONTENT_DATA pContentData, wchar_t * pszPath, DWORD * pcchPath)
{
	TRACE("XLiveContentGetPath  (dwUserIndex = %x, pXContentData = %x, pszPath = %x, pcchPath = %d)",
		dwUserIndex, pContentData, pszPath, *pcchPath );


	// for breakpoint debugging
#if 0
	while(1)
		Sleep(1);
#endif


	if( dwUserIndex || !pContentData || !pcchPath || ( !pszPath && (*pcchPath) ) )
	{
		TRACE("- ERROR_INVALID_PARAMETER" );

		return HRESULT_FROM_WIN32( ERROR_INVALID_PARAMETER );
	}


	TRACE("- ContentNum = %d, TitleId = %08X, ContentId = %02X%02X%02X%02X",
		pContentData->ContentNum, pContentData->TitleId, pContentData->ContentId[0], pContentData->ContentId[1], pContentData->ContentId[2], pContentData->ContentId[3] );


	SetDlcBasepath( pContentData->ContentNum );



	if(pszPath == 0 && (*pcchPath) == 0)
	{
		*pcchPath = dlcbasepath.size() + + 1;


		TRACE("- ERROR_INSUFFICIENT_BUFFER  (pcchPath = %d)", *pcchPath );

		return HRESULT_FROM_WIN32( ERROR_INSUFFICIENT_BUFFER );
	}


	// copy name to Buffer
	if( pszPath )
	{
		wcscpy(pszPath, dlcbasepath.c_str());

		*pcchPath = wcslen( pszPath ) + 1;
	}
	else
	{
		*pcchPath = dlcbasepath.size() + 1;
		return HRESULT_FROM_WIN32( ERROR_INSUFFICIENT_BUFFER );
	}


	TRACE("- pszPath = %s, pcchPath = %d", pszPath, *pcchPath );
	return 0;
}


// #5356: XContentCreatePackage
DWORD WINAPI XContentCreatePackage(DWORD dwUserIndex, PXCONTENT_DATA pContentData, WCHAR *pszPath, DWORD *pcchPath)
{
	TRACE("XContentCreatePackage (?) (dwUserIndex = %x, pXContentData = %x, pszPath = %x, pcchPath = %d)",
		dwUserIndex, pContentData, pszPath, *pcchPath );


	// for breakpoint debugging
#if 0
	while(1)
		Sleep(1);
#endif


	if( dwUserIndex || !pContentData || !pcchPath || ( !pszPath && (*pcchPath) ) )
	{
		TRACE("- ERROR_INVALID_PARAMETER" );

		return HRESULT_FROM_WIN32( ERROR_INVALID_PARAMETER );
	}



	TRACE("- ContentNum = %d, TitleId = %08X, ContentId = %02X%02X%02X%02X",
		pContentData->ContentNum, pContentData->TitleId, pContentData->ContentId[0], pContentData->ContentId[1], pContentData->ContentId[2], pContentData->ContentId[3] );


	SetDlcBasepath( pContentData->ContentNum );



	if(pszPath == 0 && (*pcchPath) == 0)
	{
		TRACE("- pszPath = NULL, pcchPath = 0  (ERROR_INSUFFICIENT_BUFFER)" );


		// return size XContent string = 128 max  (BioShock 2)
		*pcchPath = dlcbasepath.size() + 1;
		return HRESULT_FROM_WIN32( ERROR_INSUFFICIENT_BUFFER );
	}


	// copy name to Buffer
	if( pszPath )
	{
		wcscpy(pszPath,dlcbasepath.c_str());
		*pcchPath = dlcbasepath.size() + 1;
	}
	else
	{
		*pcchPath = dlcbasepath.size() + 1;
		return HRESULT_FROM_WIN32( ERROR_INSUFFICIENT_BUFFER );
	}


	TRACE("- pszPath = %s, pcchPath = %d", pszPath, *pcchPath );
	return 0;
}


// #5360: XContentCreateEnumerator
DWORD WINAPI XContentCreateEnumerator(DWORD MaxEnumerator, PDWORD a2, PDWORD pchBuffer, PHANDLE phEnum)
{
	static int print = 0;


	if( print < 15 )
	{
    TRACE("XContentCreateEnumerator  (MaxEnumerator = %X, a2 = %X, pchBuffer = %X, phEnum = %X",
			MaxEnumerator, a2, pchBuffer, phEnum );

		print++;
	}


	// init DLC list
	SetDlcBasepath(0);


	if(pchBuffer) *pchBuffer = MaxEnumerator * sizeof(XCONTENT_DATA);
	if(phEnum) *phEnum = g_dwFakeContent = CreateMutex(NULL,NULL,NULL);


	// recount DLCs again
	dlcinit = 0;
	

	if( print < 15 )
	{
		if( pchBuffer ) TRACE("- pchBuffer = %X  (%d)", *pchBuffer, *pchBuffer / sizeof(XCONTENT_DATA) );
	  TRACE("- phEnum = %X", *phEnum );
	}

			
	return 0;
}


// #5361: XContentRetrieveOffersByDate
DWORD WINAPI XContentRetrieveOffersByDate (DWORD dwUserIndex, DWORD dwOffserInfoVersion,
        SYSTEMTIME * pstStartDate, void * pOffserInfoArray, DWORD * pcOfferInfo, PXOVERLAPPED pOverlapped)
{
    TRACE("XLiveContentRetrieveOffersByDate");
    return 0;
}





// #5295: XLivePBufferSetByteArray
DWORD WINAPI XLivePBufferSetByteArray (FakePBuffer * pBuffer, DWORD offset, BYTE *source, DWORD size)
{
	static int print = 0;


	if( print < 35 )
	{
		print++;

    TRACE("XLivePBufferSetByteArray  (pBuffer = %X, offset = %X, source = %X, size = %X)",
			pBuffer, offset, source, size );
	}


	if( !pBuffer || !source || offset < 0 || offset+size > pBuffer->dwSize )
	{
		TRACE("- Invalid parameter");
		return -1;
	}

	if( pBuffer->magic != 0xDEADC0DE )
	{
		TRACE("- bad magic" );
		return 0;
	}


	memcpy( pBuffer->pbData + offset, source, size );
	return 0;
}


// #5294: XLivePBufferGetByteArray
DWORD WINAPI XLivePBufferGetByteArray (FakePBuffer * pBuffer, DWORD offset, BYTE * destination, DWORD size)
{
	static int print = 0;


	if( print < 35 )
	{
		print++;


    TRACE("XLivePBufferGetByteArray  (pBuffer = %X, pBuffer->Id = %X, offset = %d, dest = %X, size = %d",
			pBuffer, pBuffer->id, offset, destination, size );
	}


	if (!pBuffer || !destination || offset < 0 || offset+size > pBuffer->dwSize)
	{
		TRACE("- Invalid parameter");
		return -1;
	}


	if( pBuffer->magic != 0xDEADC0DE )
	{
		TRACE("- bad magic" );
		return 0;
	}


	memcpy (destination, pBuffer->pbData+offset, size);
	return 0;
}


// #5019: XLivePBufferSetByte
DWORD WINAPI XLivePBufferSetByte (FakePBuffer * pBuffer, DWORD offset, BYTE value)
{
	static int print = 0;


	if( print < 35 )
	{
		//TRACE("XLivePBufferSetByte  (pBuffer = %X, offset = %X, value = %X)",
		//	pBuffer, offset, value );


		print++;
	}


	if (!pBuffer || offset < 0 || offset+1 > pBuffer->dwSize)
	{
		TRACE("- Invalid parameter");
		return -1;
	}


	if( pBuffer->magic != 0xDEADC0DE )
	{
		TRACE("- bad magic" );
		return 0;
	}

	pBuffer->pbData[offset] = value;

	return 0;
}


// #5018: XLivePBufferGetByte
DWORD WINAPI XLivePBufferGetByte (FakePBuffer * pBuffer, DWORD offset, BYTE * value)
{
	static int print = 0;


	if( print < 35 )
	{
      //TRACE("XLivePBufferGetByte  (pBuffer = %X, offset = %X, value = %X)",
	//		pBuffer, offset, value );


		print++;
	}


  if (!pBuffer || !value || offset < 0 || offset+1 > pBuffer->dwSize)
	{
		//TRACE("- Invalid parameter");
		return -1;
	}


	if( pBuffer->magic != 0xDEADC0DE )
	{
		TRACE("- bad magic" );
		return 0;
	}


	*value = pBuffer->pbData[offset];
  return 0;
}


// #5020: XLivePBufferGetDWORD
DWORD WINAPI XLivePBufferGetDWORD (FakePBuffer * pBuffer, DWORD dwOffset, DWORD * pdwValue)
{
	static int print = 0;


	if( print < 35 )
	{
    TRACE("XLivePBufferGetDWORD  (pBuffer = %X, dwOffset = %X, pdwValue = %X)",
			pBuffer, dwOffset, pdwValue );


		print++;
	}


	if (!pBuffer || dwOffset < 0 || dwOffset+4 > pBuffer->dwSize || !pdwValue)
	{
		TRACE("- Invalid parameter");
		return -1;
	}


	if( pBuffer->magic != 0xDEADC0DE )
	{
		TRACE("- bad magic" );
		return 0;
	}


	*pdwValue = *((DWORD *)(pBuffer->pbData+dwOffset));
	return 0;
}


// #5021: XLivePBufferSetDWORD
DWORD WINAPI XLivePBufferSetDWORD( FakePBuffer * pBuffer, DWORD dwOffset, DWORD dwValue )
{
	static int print = 0;


	if( print < 35 )
	{
		TRACE("XLivePBufferSetDWORD  (pBuffer = %X, dwOffset = %X, dwValue = %X)",
			pBuffer, dwOffset, dwValue );


		print++;
	}


	if (!pBuffer || dwOffset < 0 || dwOffset+4 > pBuffer->dwSize )
	{
		TRACE("- Invalid parameter");
		return -1;
	}


	if( pBuffer->magic != 0xDEADC0DE )
	{
		TRACE("- bad magic" );
		return 0;
	}


	*((DWORD *)(pBuffer->pbData + dwOffset)) = dwValue;
	return 0;
}



// #5026: XLiveSetSponsorToken
DWORD WINAPI XLiveSetSponsorToken (LPCWSTR pwszToken, DWORD dwTitleId)
{
    TRACE("XLiveSetSponsorToken (, 0x%08x)", dwTitleId);
    return S_OK;
}



// #5034: XLiveProtectData
HRESULT WINAPI XLiveProtectData (BYTE * pInBuffer, DWORD dwInDataSize, BYTE * pOutBuffer, DWORD *pDataSize, HANDLE handle )
{
	TRACE("XLiveProtectData  (pInBuffer = %X, dwInDataSize = %d, pOutBuffer = %X, pDataSize = %X, Handle = %X)", pInBuffer, dwInDataSize, pOutBuffer, *pDataSize, (DWORD)handle);


	if( *pDataSize < dwInDataSize )
	{
		if( pDataSize ) *pDataSize = dwInDataSize;

		TRACE( "- Insufficient buffer = %d", pDataSize ? *pDataSize : -1 );

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
#if 0
	while(1)
		Sleep(1);
#endif


	TRACE("XLiveUnprotectData  (pInBuffer = %X, dwInDataSize = %d, pOutBuffer = %X, pDataSize = %X, Handle = %X)", pInBuffer, dwInDataSize, pOutBuffer, *pDataSize, (DWORD)pHandle);


	if( *pDataSize < dwInDataSize )
	{
		if( pDataSize ) *pDataSize = dwInDataSize;

		TRACE( "- Insufficient buffer = %d", pDataSize ? *pDataSize : -1 );

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
    TRACE("XLiveCreateProtectedDataContext");
	if (pHandle)
	{
		*pHandle = CreateMutex(NULL,NULL,NULL);


		TRACE("- Handle = %X", *pHandle);
		return 0;
	}

	return 1;
}


// #5037: XLiveQueryProtectedDataInformation
DWORD WINAPI XLiveQueryProtectedDataInformation (HANDLE h, DWORD * p)
{
    TRACE("XLiveQueryProtectedDataInformation  (h = %X, p = %X)", h, p );
    return 0;
}


// #5038: XLiveCloseProtectedDataContext
DWORD WINAPI XLiveCloseProtectedDataContext (HANDLE h)
{
    TRACE("XLiveCloseProtectedDataContext  (handle = %X)", h);
	CloseHandle(h);
    return 0;
}


// #5342: XSessionModifySkill
DWORD WINAPI XSessionModifySkill (HANDLE, DWORD, void * rgXuid, PXOVERLAPPED pOverlapped)
{
    TRACE("XSessionModifySkill");
    return 0;
}


// #5348: XLiveProtectedCreateFile
HRESULT WINAPI XLiveProtectedCreateFile (HANDLE hContentAccess, void * pvReserved, PCWSTR pszFilePath,
	DWORD dwDesiredAccess, DWORD dwShareMode, SECURITY_ATTRIBUTES * pSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, PHANDLE phModule)
{
	TRACE("XLiveProtectedCreateFile  (hContentAccess = %X, pvReserved = %X, pszFilePath = %s, dwDesiredAccess = %d, dwShareMode = %d, dwSecurityAttributes = %X, dwCreationDisposition = %d, flagsAndAttributes = %d, phModule = %X)",
		hContentAccess, pvReserved, pszFilePath, dwDesiredAccess, dwShareMode, pSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, phModule );


	WCHAR dlcfile[2048];


	// FIXME: assume DLC folder handle
	if( hContentAccess )
	{
		wcscpy( dlcfile, dlcbasepath.c_str() );
		wcscat( dlcfile, L"\\Content\\" );
		wcscat( dlcfile, pszFilePath );
	}

	else
		wcscpy( dlcfile, pszFilePath );


	*phModule = CreateFileW(
		dlcfile,
		dwDesiredAccess,
		dwShareMode,
		pSecurityAttributes,
		dwCreationDisposition,
		dwFlagsAndAttributes,
		0 );


	if( (HANDLE) (*phModule) == (HANDLE) INVALID_HANDLE_VALUE )
	{
		TRACE( "- INVALID_HANDLE" );

		return -1;
	}


	TRACE( "- File opened" );
	return 0;
}


// #5367: XContentGetMarketplaceCounts
DWORD WINAPI XContentGetMarketplaceCounts( DWORD dwUserIndex, DWORD dwContentCategories, DWORD cbResults, XOFFERING_CONTENTAVAILABLE_RESULT *pResults, PXOVERLAPPED pOverlapped )
{
	TRACE( "XContentGetMarketplaceCounts  (dwUserIndex = %d, dwContentCategories = %X, cbResults = %X, pResults = %X, pOverlapped = %X)",
		dwUserIndex, dwContentCategories, cbResults, pResults, pOverlapped );


	if( pResults )
	{
		pResults->dwNewOffers = 0;
		pResults->dwTotalOffers = marketplaceDlcCount;
	}


	if( pOverlapped )
	{
		pOverlapped->InternalHigh = marketplaceDlcCount;

		pOverlapped->InternalLow = ERROR_SUCCESS;
		pOverlapped->dwExtendedError = ERROR_SUCCESS;


		Check_Overlapped( pOverlapped );
		
		return ERROR_IO_PENDING;
	}

	
	return ERROR_SUCCESS;
}


// #5372: XMarketplaceCreateOfferEnumerator
DWORD WINAPI XMarketplaceCreateOfferEnumerator( DWORD dwUserIndex, DWORD dwOfferType, DWORD dwContentCategories, DWORD cItem, PDWORD pcbBuffer, PHANDLE phEnum )
{
	TRACE("XMarketplaceCreateOfferEnumerator  (dwUserIndex = %d, dwOfferType = %X, dwContentCategories = %X, cItem = %d, pchBuffer = %x, phEnum = %x",
		dwUserIndex, dwOfferType, dwContentCategories, cItem, pcbBuffer, phEnum );


	if( pcbBuffer ) *pcbBuffer = sizeof( XMARKETPLACE_CONTENTOFFER_INFO ) * cItem;
	if( phEnum )
	{
		*phEnum = g_dwMarketplaceContent = CreateMutex(NULL,NULL,NULL);


		TRACE("- Handle = %X", *phEnum);


		marketplaceEnumerate = 0;

		marketplaceCount = marketplaceDlcCount;
		memcpy( &marketplace, &marketplaceDlc, sizeof(XMARKETPLACE_CONTENTOFFER_INFO) * marketplaceDlcCount );


		for( int lcv = 0; lcv < marketplaceDlcCount; lcv++ )
		{

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
				TRACE( "- [%d] TitleId = %08X", lcv, marketplace[lcv].dwTitleID );


			TRACE( "- [%d] OfferingId = %I64X", lcv, marketplace[lcv].qwOfferID );


			strw[0] = 0;
			for( int lcv2 = 0; lcv2 < 20; lcv2++ )
				swprintf( strw, L"%s%02X", strw, marketplace[lcv].contentId[lcv2] );
			strw[40] = 0;

			TRACE( "- [%d] ContentId = %s", lcv, strw );
		}
	}


	return ERROR_SUCCESS;
}


DWORD WINAPI XMarketplaceCreateOfferEnumeratorByOffering( DWORD dwUserIndex, DWORD cItem, const ULONGLONG *pqwNumOffersIds, WORD cOfferIDs, PDWORD pcbBuffer, PHANDLE phEnum )
{
	TRACE("XMarketplaceCreateOfferEnumeratorByOffering  (dwUserIndex = %d, cItem = %d, pqwNumOffersIds = %X, cOfferIDs = %d, pcbBuffer = %X, phEnum = %X)",
		dwUserIndex, cItem, pqwNumOffersIds, cOfferIDs, pcbBuffer, phEnum );


	if( pcbBuffer ) *pcbBuffer = sizeof( XMARKETPLACE_CONTENTOFFER_INFO ) * cItem;
	if( phEnum )
	{
		*phEnum = g_dwMarketplaceContent = CreateMutex(NULL,NULL,NULL);
		
		TRACE("- Handle = %X", *phEnum);
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
			TRACE( "- [%d] TitleId = %08X", lcv, marketplace[lcv].dwTitleID );

		TRACE( "- [%d] OfferingID = %I64X", lcv, pqwNumOffersIds[lcv] );
	}

	marketplaceCount = cOfferIDs;
	marketplaceEnumerate = 0;


	return ERROR_SUCCESS;
}

// #5029
DWORD WINAPI XLiveSecureFreeLibrary(HMODULE hLibModule)
{
    TRACE("XLiveSecureFreeLibrary");
    if(hLibModule) FreeLibrary(hLibModule);
    return 0;
}


// #5347
DWORD WINAPI XLiveProtectedLoadLibrary(int a1, int a2, LPCWSTR lpLibFileName, DWORD dwFlags, HMODULE* a5)
{
    TRACE("XLiveProtectedLoadLibrary");
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
    TRACE(" XLiveContentVerifyInstalledPackage");
    return 0;
}


DWORD WINAPI TitleExport_XUserFinderUsers(int a1, int a2, int a3, void* a4, unsigned int a5, unsigned __int8 *a6, PXOVERLAPPED *a7)
{
    TRACE("TitleExport_XUserFinderUsers");
    return 1;
}


DWORD WINAPI XLiveGetGuideKey(int pKeyStroke)
{
    TRACE("XLiveGetGuideKey(pKeyStroke: %08x)",pKeyStroke);

		// a[0] = x[0]
		// a[4] = x[4]

    return 0;
}


// 5334
DWORD WINAPI XOnlineGetServiceInfo( int, int )
{
   TRACE("XOnlineGetServiceInfo  (*** checkme ***)");

  
	// ?? one path
   return ERROR_SUCCESS;
   //return 0x4DB;
}

// 5028: ??
DWORD WINAPI XLiveLoadLibraryEx(LPCWSTR libFileName, HINSTANCE *a2, DWORD dwFlags)
{
	TRACE("XLiveLoadLibraryEx (?? - FIXME)  (libFileName = %s, a2 = %X, flags = %X)",
		libFileName, a2, dwFlags);

	HINSTANCE hInstance = LoadLibraryExW(libFileName, NULL, dwFlags);

	if (!hInstance)
		return 0x80070057;

	*a2 = hInstance;
	return 0;
}




// 5238: ??
DWORD WINAPI XLocatorCreateKey( XNKID* pxnkid, XNKEY* xnkey )
{
  TRACE("XLocatorCreateKey  (a1 = %X, a2 = %X)",
		pxnkid, xnkey );

	XNetCreateKey(pxnkid, xnkey);
	// GFWL offline
	return S_OK;
}


// 5257: ??
DWORD WINAPI XLiveManageCredentials( DWORD a1, DWORD a2, DWORD a3, DWORD dwData )
{
  TRACE("XLiveManageCredentials  (*** checkme ***) (a1 = %X, a2 = %X, a3 = %X, dwData = %X)",
		a1, a2, a3, dwData);


	// not done - error now
	return S_OK;
}

// 5290: ??
DWORD WINAPI XUserGetReputationStars( DWORD a1 )
{
  TRACE("XUserGetReputationStars  (*** checkme ***) (a1 = %X)",
		a1);


	// not done - error now
	return S_OK;
}


// 5296: ??
DWORD WINAPI XLiveGetLocalOnlinePort( DWORD a1 )
{
  TRACE("XLiveGetLocalOnlinePort  (*** checkme ***) (a1 = %X)",
		a1);


	// not done - error now
	return S_OK;
}



// 5304
DWORD WINAPI XStorageUploadFromMemoryGetProgress( DWORD a1, DWORD a2, DWORD a3, DWORD a4 )
{
  TRACE("XStorageUploadFromMemoryGetProgress  (*** checkme ***) (a1 = %X, a2 = %X, a3 = %X, a4 = %X)",
		a1, a2, a3, a4 );


	// not done - error now
	return 0x57;
}


// 5307
DWORD WINAPI XStorageDownloadToMemoryGetProgress( DWORD a1, DWORD a2, DWORD a3, DWORD a4 )
{
  TRACE("XStorageDownloadToMemoryGetProgress  (*** checkme ***) (a1 = %X, a2 = %X, a3 = %X, a4 = %X)",
		a1, a2, a3, a4 );


	// not done - error now
	return 0x57;
}


// 5308
DWORD WINAPI XStorageDelete( DWORD a1, DWORD a2, DWORD a3 )
{
  TRACE("XStorageDelete  (*** checkme ***) (a1 = %X, a2 = %X, a3 = %X)",
		a1, a2, a3);


	// not done - error now
	SetLastError( 0x57 );
	return 0x57;
}


// 5362
DWORD WINAPI MarketplaceDoesContentIdMatch( CHAR *a1, DWORD a2 )
{
  TRACE("MarketplaceDoesContentIdMatch  (*** checkme ***) (a1 = %s, a2 = %X)",
		a1, a2);


	// not done - error now
	SetLastError( 0x57 );
	return 0x57;
}


// 5363
DWORD WINAPI XLiveContentGetLicensePath()
{
  TRACE("XLiveContentGetLicensePath  (*** checkme ***)");


	// not done - error now
	SetLastError( 0x57 );
	return 0x80070057;
}




// 5370: ??
DWORD WINAPI TitleExport_XMarketplaceConsumeAssets( DWORD a1, DWORD a2, DWORD a3, DWORD a4 )
{
  TRACE("TitleExport_XMarketplaceConsumeAssets (a1 = %X, a2 = %X, a3 = %X, a4 = %X)",
		a1, a2, a3, a4);


	// not done - error now
	return 0x57;
}


// 5371: ??
DWORD WINAPI XMarketplaceCreateAssetEnumerator( DWORD a1, DWORD a2, DWORD a3, DWORD a4 )
{
  TRACE("XMarketplaceCreateAssetEnumerator (a1 = %X, a2 = %X, a3 = %X, a4 = %X)",
		a1, a2, a3, a4);


	// not done - error now
	return 0x57;
}



DWORD WINAPI XNetGetCurrentAdapter( DWORD a1, DWORD a2 )
{
	TRACE( "XNetGetCurrentAdapter  (a1 = %X, a2 = %X)",
		a1, a2 );


	return 0;
}



DWORD WINAPI XLiveGetLiveIdError( DWORD a1, DWORD a2, DWORD a3, DWORD a4 )
{
	TRACE( "XLiveGetLiveIdError  (a1 = %X, a2 = %X, a3 = %X, a4 = %X)",
		a1, a2, a3, a4 );


	return 0;
}



DWORD WINAPI XLiveVerifyDataFile( DWORD a1 )
{
	TRACE( "XLiveVerifyDataFile  (a1 = %X)",
		a1 );


	return 0;
}






DWORD WINAPI XEnumerateBack( DWORD a1, DWORD a2, DWORD a3, DWORD a4, DWORD a5 )
{
	TRACE( "XEnumerateBack  (a1 = %X, a2 = %X, a3 = %X, a4 = %X, a5 = %X)",
		a1, a2, a3, a4, a5 );


	return 0;
}


DWORD WINAPI TitleExport_XPresenceUnsubscribe( DWORD a1, DWORD a2, DWORD a3 )
{
	TRACE( "TitleExport_XPresenceUnsubscribe  (a1 = %X, a2 = %X, a3 = %X)",
		a1, a2, a3 );


	return 0;
}


DWORD WINAPI XLiveContentInstallPackage( DWORD a1, DWORD a2, DWORD a3 )
{
	TRACE( "XLiveContentInstallPackage  (a1 = %X, a2 = %X, a3 = %X)",
		a1, a2, a3 );


	return 0;
}


DWORD WINAPI XLiveContentGetThumbnail( DWORD a1, DWORD a2, DWORD a3, DWORD a4 )
{
	TRACE( "XLiveContentGetThumbnail  (a1 = %X, a2 = %X, a3 = %X, a4 = %X)",
		a1, a2, a3, a4);


	return 0;
}


DWORD WINAPI XLiveContentInstallLicense( DWORD a1, DWORD a2, DWORD a3 )
{
	TRACE( "XLiveContentInstallLicense  (a1 = %X, a2 = %X, a3 = %X)",
		a1, a2, a3);


	return 0;
}



DWORD WINAPI TitleExport_XUserFindUsers(int, int, int, int, int, int, int)
{
	TRACE("TitleExport_XUserFindUsers");
	return 1;
}


// === end of xlive functions ===