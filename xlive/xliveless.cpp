// -No Copyright- 2010 Stanislav "listener" Golovin
// This file donated to the public domain
#include "stdafx.h"
#include "resource.h"
#include <iostream>
#include <sstream>
#include <codecvt>
#include <unordered_map>
#include "Globals.h"
#include "discord/DiscordInterface.h"
#include "H2Config.h"
#include "H2MOD_ServerList.h"
#include "xlivedefs.h"
#include "CUser.h"
#include <winsock2.h>
#include <ws2tcpip.h>

using namespace std;


// Live connection state success codes
#define XONLINE_S_LOGON_CONNECTION_ESTABLISHED          _HRESULT_TYPEDEF_(0x001510F0L)
#define XONLINE_S_LOGON_DISCONNECTED                    _HRESULT_TYPEDEF_(0x001510F1L)



HANDLE g_dwFakeListener = (HANDLE) -2;
HANDLE g_dwFakePData = (HANDLE) -2;
HANDLE g_dwFakeContent = (HANDLE) -2;
HANDLE g_dwFakeAchievementContent = (HANDLE) -2;
HANDLE g_dwMarketplaceContent = (HANDLE) -2;


INT num_players;
XSESSION_LOCAL_DETAILS sessionDetails;
IXHV2ENGINE hv2Engine;

WSADATA wsaData;

DWORD sys_ui = -1;



struct NOTIFY_LISTEN
{
	HANDLE id;
	DWORD area;

	DWORD print;
};

static NOTIFY_LISTEN g_listener[50];
static int g_dwListener = 0;



extern UINT g_signin[4];
extern XUID xFakeXuid[4];
extern CHAR g_szUserName[4][16+1];
extern UINT g_online;
extern CHAR g_profileDirectory[];

extern void InitInstance();
extern void ExitInstance();
extern int H2GetInstanceId();

extern std::wstring dlcbasepath;


int dlcinit = 0;
int achieveinit = 0;


#define DEBUG_WAIT 0


INT achievementList[65536];
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


/*
void SaveAchievements()
{
	FILE *fp;


	Local_Storage_A( 0, str );
	strcat( str, "\\Achievements" );
	CreateDirectoryA( str, NULL );


	sprintf( str, "%s\\Achievements.txt", str );
	fp = fopen( str, "w" );
	if( !fp ) return;


	for( int lcv = 0; lcv < 65536; lcv++ )
		if( achievementList[lcv] )
			fprintf( fp, "%d\n", lcv );


	//fprintf( fp, "%08X", ( xFakeXuid >> 32 ) & 0xffffffff );
	//fprintf( fp, "%08X", ( xFakeXuid >> 0 ) & 0xffffffff );

	fclose(fp);
}
*/

/*
void LoadAchievements()
{
	char str[256];
	FILE *fp;


	//Local_Storage_A( 0, str );
	strcat( str, "\\Achievements" );
	CreateDirectoryA( str, NULL );


	sprintf( str, "%s\\Achievements.txt", str );
	fp = fopen( str, "r" );
	if( !fp )
	{
		// create dummy file
		SaveAchievements();
		return;
	}


	while( !feof(fp) )
	{
		int num;

		num = -1;
		fscanf( fp, "%d\n", &num );

		if( num == -1 ) break;


		achievementList[num] = 1;
	}


	fclose(fp);
}
*/


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


	TRACE( "- async routine" );


	if( pOverlapped->hEvent )
	{
		TRACE( "- hEvent = %X", pOverlapped->hEvent );

		SetEvent( pOverlapped->hEvent );
	}


	if( pOverlapped->pCompletionRoutine )
	{
		TRACE( "- pCompletionRoutine = %X", pOverlapped->pCompletionRoutine );


		pOverlapped->pCompletionRoutine( pOverlapped->InternalLow, pOverlapped->InternalHigh, pOverlapped->dwCompletionContext );
	}
}


// #############################################################
// #############################################################
// #############################################################
// #############################################################
// #############################################################

// === Start of xlive functions ===



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

// #3: XCreateSocket
SOCKET WINAPI XCreateSocket (int af, int type, int protocol)
{

	
  //  TRACE("XCreateSocket (%d, %d, %d)", af, type, protocol);
	if (protocol == 254)
	{
		protocol = IPPROTO_UDP; // We can't support VDP (Voice / Data Protocol) it's some encrypted crap which isn't standard.
	}

	SOCKET ret = socket(af, type, protocol);
	if (ret == INVALID_SOCKET)
	{
		TRACE("XCreateSocket - INVALID_SOCKET");
	}
	return ret;
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

PBYTE pb_data;
UINT cb_data;

//if (comm_socket == INVALID_SOCKET)
//{
//	comm_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

SOCKET QoSListenSocket = INVALID_SOCKET;
SOCKET QoSClientSocket = INVALID_SOCKET;
// #69: XNetQosListen
DWORD WINAPI XNetQosListen( XNKID *pxnkid, PBYTE pb, UINT cb, DWORD dwBitsPerSec, DWORD dwFlags )
{
    TRACE("XNetQosListen  (pxnkid = %X, pb = %X, cb = %d, bitsPerSec = %d, flags = %X)",
			pxnkid, pb, cb, dwBitsPerSec, dwFlags );
	WSADATA wsaData;
	int iResult;

	struct addrinfo *result = NULL;
	struct addrinfo hints;

	int iSendResult;
	char recvbuf[512];
	int recvbuflen = 512;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	iResult = getaddrinfo(NULL, to_string(H2Config_base_port+9).c_str(), &hints, &result);


	QoSListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	iResult = ::bind(QoSListenSocket, result->ai_addr, (int)result->ai_addrlen);

	freeaddrinfo(result);

	iResult = listen(QoSListenSocket, SOMAXCONN);

	/*if (cb > 0)
	{
		FILE* fh = fopen("pb_dump.bin", "w");
		fwrite(fh, cb, 1, fh);
		fclose(fh);
		/*pb_data = new BYTE[cb + 1];
		ZeroMemory(pb_data, cb + 1);
		memcpy(pb_data, pb, cb);

		TRACE("pb_data: %08X", &pb_data);*/
	/*}*/

    return 0;
}



// #70: XNetQosLookup
DWORD WINAPI XNetQosLookup (UINT cxna, XNADDR * apxna[], XNKID * apxnkid[],XNKEY * apxnkey[], UINT cina, IN_ADDR aina[], DWORD adwServiceId[], UINT cProbes, DWORD dwBitsPerSec, DWORD dwFlags, WSAEVENT hEvent, XNQOS** pxnqos)
{


    TRACE("XNetQosLookup ( cxna: %i, cina: %i, cProbes: %i, dwBitsPerSec: %i, hEvent: %X)",
		cxna, cina, cProbes, dwBitsPerSec, hEvent);
	TRACE("XNetQoSLookup( apxna: %X, apxnkid: %X, apxnkey: %X, aina: %X, adwServiceId: %X, pxnqos: %X)",
		apxna, apxnkid, apxnkey, aina, adwServiceId, pxnqos);


	/* Memory Leak  - FIX ME! (Need to do some kind of garbage collection somewhere and store data like this in an array to be cleared later */
	*pxnqos = new XNQOS;
	XNQOS* pqos = *pxnqos;
	pqos->cxnqos = 1;
	pqos->cxnqosPending = 0;
	memset(pqos->axnqosinfo, 0x00, sizeof(XNQOSINFO));
	pqos->axnqosinfo->bReserved = 0;
	pqos->axnqosinfo->cProbesXmit = cProbes;
	pqos->axnqosinfo->cProbesRecv = cProbes;
	
	if (cProbes > 0)
	{
		pqos->axnqosinfo->wRttMedInMsecs = 5;
		pqos->axnqosinfo->wRttMinInMsecs = 10;
		pqos->axnqosinfo->dwUpBitsPerSec = dwBitsPerSec;
		pqos->axnqosinfo->dwDnBitsPerSec = dwBitsPerSec;
		pqos->axnqosinfo->bFlags = XNET_XNQOSINFO_TARGET_CONTACTED | XNET_XNQOSINFO_COMPLETE | XNET_XNQOSINFO_DATA_RECEIVED;
	}
	else
	{
		pqos->axnqosinfo->wRttMedInMsecs = 5;
		pqos->axnqosinfo->wRttMinInMsecs = 10;
		pqos->axnqosinfo->dwUpBitsPerSec = dwBitsPerSec;
		pqos->axnqosinfo->dwDnBitsPerSec = dwBitsPerSec;
		pqos->axnqosinfo->bFlags = XNET_XNQOSINFO_TARGET_CONTACTED | XNET_XNQOSINFO_COMPLETE;

	}

    return 0;
}

// #71: XNetQosServiceLookup
DWORD WINAPI XNetQosServiceLookup (DWORD a1, DWORD a2, DWORD a3)
{
    TRACE("XNetQosServiceLookup");
    

		// not connected to LIVE - abort now
		// - wants a3 return ASYNC struct
		return ERROR_INVALID_PARAMETER;
		
		//return 0;
}

// #72: XNetQosRelease
DWORD WINAPI XNetQosRelease (DWORD)
{
    TRACE("XNetQosRelease");
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


// #77
DWORD WINAPI XNetQosGetListenStats(DWORD a1, DWORD a2)
{
	TRACE("XNetQosGetListenStats");
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

// #472
int WINAPI XCustomSetAction(int a1, int a2, int a3)
{
	TRACE("XCustomSetAction");
	return 0;
}

// #473
int WINAPI XCustomGetLastActionPress (DWORD, DWORD, DWORD)
{
    TRACE("XCustomGetLastActionPress");
    return 0;
}

// #474
int WINAPI XCustomSetDynamicActions(int a1, int a2, int a3, int a4, int a5)
{
	TRACE("XCustomSetDynamicActions");
	return 0;
}

// #476
BOOL WINAPI XCustomGetLastActionPressEx(int a1, int a2, int a3, int a4, int a5) //XCustomGetLastActionPressEx ?
{
    TRACE("XCustomGetLastActionPressEx");
    return FALSE;
}

// #477
void WINAPI XCustomRegisterDynamicActions()
{
	TRACE("XCustomRegisterDynamicActions");
	return;
}

// #478
void WINAPI XCustomUnregisterDynamicActions()
{
	TRACE("XCustomUnregisterDynamicActions");
	return;
}


// #479
int WINAPI XCustomGetCurrentGamercard(int a1, int a2)
{
	TRACE("XCustomGetCurrentGamercard");
	return 0;
}



DWORD WINAPI sysui_timer( LPVOID lpParam )
{
	Sleep(20);



	SetEvent( (HANDLE) lpParam );
	sys_ui = 2;


	TRACE( "- %X = XN_SYS_UI  (signal)", (HANDLE) lpParam );


	return 0;
}

int g_xlive = 0;

BOOL XNotifyGetNext_Compat(HANDLE hNotification, DWORD dwMsgFilter, PDWORD pdwId, PULONG_PTR pParam)
{
	static int print = 0;
	static int print_limit = 90000;


	// reset logger
	if( sys_ui == -1 )
	{
		sys_ui = 0;
		print = 0;
	}


	if( print < print_limit )
	{
		TRACE("XNotifyGetNext  (hNotification = %X, dwMsgFilter = %X, pdwId = %X, pParam = %X)",
			hNotification, dwMsgFilter, pdwId, pParam );

		//print++;
	}



	if(dwMsgFilter != 0 && dwMsgFilter != XNOTIFY_SYSTEM)
	{
		//if( print < print_limit )
		//	TRACE( "- discard message" );

		return FALSE;
	}


	EnterCriticalSection(&d_lock);


	static DWORD signin_ui = 0x7FFFFFFF;
	static DWORD controller = 0x7FFFFFFF;
	static DWORD controller_force = 0x7FFFFFFF;

	//if (g_xlive == 0)
	//{

	//}

	if(sys_ui == 0)
	{
		// showing ui
		if(pdwId)
			*pdwId = XN_SYS_UI;

		if(pParam)
			*pParam = 1;


		sys_ui++;


		if( print < print_limit )
			TRACE( "- XN_SYS_UI  (1)" );
	}

	// timer
	else if(sys_ui >= 1 && sys_ui <= 59)
		sys_ui++;

	else if(sys_ui == 60)
	{
		// hiding ui
		if(pdwId)
			*pdwId = XN_SYS_UI;

		if(pParam)
			*pParam = 0;


		sys_ui++;
		signin_ui = 0;


		if( print < print_limit )
			TRACE( "- XN_SYS_UI  (2)" );
	}

	else if(signin_ui == 0)
	{
		if(pdwId)
			*pdwId = XN_SYS_SIGNINCHANGED;


		if(pParam)
		{
			*pParam = 0;


			// player 1-4
			if( g_signin[0] ) *pParam |= 1;
			if( g_signin[1] ) *pParam |= 2;
			if( g_signin[2] ) *pParam |= 4;
			if( g_signin[3] ) *pParam |= 8;
		}


		signin_ui++;
		controller = 0;



		if( print < print_limit )
			TRACE( "- XN_SYS_SIGNINCHANGED  (1)" );
	}

	else if(controller == 0)
	{
		if(pdwId)
			*pdwId = XN_SYS_INPUTDEVICESCHANGED;


		controller++;
		controller_force = 0;


		if( print < print_limit )
			TRACE( "- XN_SYS_INPUTDEVICESCHANGED  (1)" );
	}

	else if(controller_force == 0)
	{
		if(pdwId)
			*pdwId = XN_SYS_INPUTDEVICECONFIGCHANGED;


		controller_force++;



		if( print < print_limit )
			TRACE( "- XN_SYS_INPUTDEVICECONFIGCHANGED  (1)" );
	}

	else
	{
		if( print < print_limit )
			TRACE("- message = FALSE");
		SetLastError(NO_ERROR);

		LeaveCriticalSection(&d_lock);
		return FALSE;
	}


	SetLastError(NO_ERROR);

	LeaveCriticalSection(&d_lock);
	return TRUE;
}


// #651: XNotifyGetNext
BOOL WINAPI XNotifyGetNext(HANDLE hNotification, DWORD dwMsgFilter, PDWORD pdwId, PULONG_PTR pParam)
{
	char gameName[256];


	/*
	hack - timing issue?? setevent problem??
	- Dawn of War II  (login)
	- Resident Evil: Operation Raccoon City  (gamepad)
	- Battlestations: Pacific  (login)
	*/

#if 1
	GetModuleFileNameA( NULL, (LPCH) &gameName, sizeof(gameName) );
	if( ( strstr( gameName, "DOW2.exe" ) != 0 ) ||
			( strstr( gameName, "RaccoonCity.exe" ) != 0 ) ||
			( strstr( gameName, "bsp.exe" ) != 0 ) )
		return XNotifyGetNext_Compat( hNotification, dwMsgFilter, pdwId, pParam );
#endif


		

	static int print_limit = 30;

	static DWORD sys_signin = 0x7FFFFFFF;
	static DWORD sys_storage = 0x7FFFFFFF;
	static DWORD sys_profile = 0x7FFFFFFF;
	static DWORD sys_controller = 0x7FFFFFFF;
	static DWORD sys_controller_force = 0x7FFFFFFF;

	static DWORD live_connection = 0x7FFFFFFF;
	static DWORD live_content = 0x7FFFFFFF;
	static DWORD live_membership = 0x7FFFFFFF;




	ResetEvent( hNotification );



	int curlist = 0;
	while( curlist < g_dwListener )
	{
		if( g_listener[curlist].id == hNotification ) break;

		curlist++;
	}

	if( curlist == g_dwListener ) 
	{
		TRACE("XNotifyGetNext  (hNotification = %X, dwMsgFilter = %X, pdwId = %X, pParam = %X)",
			hNotification, dwMsgFilter, pdwId, pParam );

		TRACE( "- unknown notifier" );

		return 0;
	}



	if( ( g_listener[curlist].area & ((XNID_AREA(dwMsgFilter)<<1)|1) ) == 0 )
	{
		TRACE("XNotifyGetNext  (hNotification = %X, dwMsgFilter = %X, pdwId = %X, pParam = %X)",
			hNotification, dwMsgFilter, pdwId, pParam );

		TRACE( "- bad area: %X ~ %X", g_listener[curlist].area, (XNID_AREA(dwMsgFilter)<<1)|1 );

		return 0;
	}



	// reset logger
	if( sys_ui == -1 )
	{
		sys_ui = 0;
		g_listener[curlist].print = 0;
	}



	if( g_listener[curlist].print < print_limit )
	{
		TRACE("XNotifyGetNext  (hNotification = %X, dwMsgFilter = %X, pdwId = %X, pParam = %X)",
			hNotification, dwMsgFilter, pdwId, pParam );


		g_listener[curlist].print++;
	}



	EnterCriticalSection(&d_lock);


	BOOL exit_code = FALSE;

	if(pdwId) *pdwId = dwMsgFilter;




	// set to next available message
	if( ( g_listener[curlist].area & XNOTIFY_SYSTEM ) &&
			dwMsgFilter == 0 )
	{
		if(0) {}

		else if( sys_ui == 0 || sys_ui == 2 )
			dwMsgFilter = XN_SYS_UI;

		else if( sys_signin == 0 )
			dwMsgFilter = XN_SYS_SIGNINCHANGED;

		// ex. GTA IV - recheck DLC containers
		else if( sys_storage == 0 )
			dwMsgFilter = XN_SYS_STORAGEDEVICESCHANGED;

		else if( sys_profile == 0 )
			dwMsgFilter = XN_SYS_PROFILESETTINGCHANGED;

		else if( sys_controller == 0 )
			dwMsgFilter = XN_SYS_INPUTDEVICESCHANGED;

		else if( sys_controller_force == 0 )
			dwMsgFilter = XN_SYS_INPUTDEVICECONFIGCHANGED;
	}


	if( ( g_listener[curlist].area & XNOTIFY_LIVE ) &&
			dwMsgFilter == 0 )
	{
		if( live_connection == 0 )
			dwMsgFilter = XN_LIVE_CONNECTIONCHANGED;

		else if( live_content == 0 )
			dwMsgFilter = XN_LIVE_CONTENT_INSTALLED;

		else if( live_membership == 0 )
			dwMsgFilter = XN_LIVE_MEMBERSHIP_PURCHASED;
		
	}


	if( dwMsgFilter == 0 )
	{
		LeaveCriticalSection( &d_lock );

		return FALSE;
	}



	switch( dwMsgFilter )
	{
	case XN_SYS_UI:
		if( sys_ui == 0 )
		{
			// show UI
			if( pParam ) *pParam = 1;


			DWORD threadid;

			sys_ui++;
			CreateThread( NULL, 0, &sysui_timer, (LPVOID) hNotification, NULL, &threadid );



			TRACE( "- %X = XN_SYS_UI (1)", hNotification );

			exit_code = TRUE;
		}

		else if( sys_ui == 2 )
		{
			// hide UI
			if( pParam ) *pParam = 0;

			sys_ui++;


			TRACE( "- %X = XN_SYS_UI (0)", hNotification );

			exit_code = TRUE;




			sys_signin = 0;
			sys_storage = 0;
			sys_profile = 0;
			sys_controller = 0;
			sys_controller_force = 0;

			live_connection = 0;
			live_content = 0;
			live_membership = 0;
		}
		break;


	case XN_SYS_SIGNINCHANGED:
		if( sys_signin == 0 )
		{
			*pdwId = XN_SYS_SIGNINCHANGED;
			if( pParam )
			{
				*pParam = 0;


				// player 1-4
				if( g_signin[0] ) *pParam |= 1;
				if( g_signin[1] ) *pParam |= 2;
				if( g_signin[2] ) *pParam |= 4;
				if( g_signin[3] ) *pParam |= 8;
			}


			sys_signin++;


			TRACE( "- %X = XN_SYS_SIGNINCHANGED (1) - %08X", hNotification, XN_SYS_SIGNINCHANGED );

			exit_code = TRUE;
		}
		break;


	case XN_SYS_STORAGEDEVICESCHANGED:
		if( sys_storage == 0 )
		{
			sys_storage++;


			TRACE( "- %X = XN_SYS_STORAGEDEVICESCHANGED (-)", hNotification );

			exit_code = TRUE;
		}
		break;


	case XN_SYS_PROFILESETTINGCHANGED:
		if( sys_profile == 0 )
		{
			if( pParam )
			{
				*pParam = 0;


				// player 1-4
				if( g_signin[0] ) *pParam |= 1;
				if( g_signin[1] ) *pParam |= 2;
				if( g_signin[2] ) *pParam |= 4;
				if( g_signin[3] ) *pParam |= 8;
			}


			sys_profile++;


			TRACE( "- %X = XN_SYS_PROFILESETTINGCHANGED (1)", hNotification );

			exit_code = TRUE;
		}
		break;


	case XN_SYS_INPUTDEVICESCHANGED:
		if( sys_controller == 0 )
		{
			sys_controller++;


			TRACE( "- %X = XN_SYS_INPUTDEVICESCHANGED (-)", hNotification );

			exit_code = TRUE;
		}
		break;


	case XN_SYS_INPUTDEVICECONFIGCHANGED:
		if( sys_controller_force == 0 )
		{
			if( pParam )
			{
				*pParam = 0;


				// player 1-4
				if( g_signin[0] ) *pParam |= 1;
				if( g_signin[1] ) *pParam |= 2;
				if( g_signin[2] ) *pParam |= 4;
				if( g_signin[3] ) *pParam |= 8;
			}


			sys_controller_force++;


			TRACE( "- %X = XN_SYS_INPUTDEVICECONFIGCHANGED (1)", hNotification );

			exit_code = TRUE;
		}
		break;


	case XN_LIVE_CONNECTIONCHANGED:
		if( live_connection == 0 )
		{
			live_connection++;
			
			*pdwId = XN_LIVE_CONNECTIONCHANGED;
			
			if( pParam ) *pParam = XONLINE_S_LOGON_CONNECTION_ESTABLISHED;


			TRACE( "- %X = XN_LIVE_CONNECTIONCHANGED (0) - %08X", hNotification, XN_LIVE_CONNECTIONCHANGED );

			exit_code = TRUE;
		}
		break;


	case XN_LIVE_CONTENT_INSTALLED:
		if( live_content == 0 )
		{
			live_content++;


			TRACE( "- %X = XN_LIVE_CONTENT_INSTALLED (-)", hNotification );

			exit_code = TRUE;
		}
		break;


	case XN_LIVE_MEMBERSHIP_PURCHASED:
		if( live_membership == 0 )
		{
			live_membership++;


			TRACE( "- %X = XN_LIVE_MEMBERSHIP_PURCHASED (-)", hNotification );

			exit_code = TRUE;
		}
		break;


	default:
		break;
	} // switch



	// check for more messages
	if( exit_code )
		SetEvent( hNotification );


	LeaveCriticalSection( &d_lock );
	return exit_code;
}


// #652: XNotifyPositionUI
DWORD WINAPI XNotifyPositionUI (DWORD dwPosition)
{
    TRACE("XNotifyPositionUI (%d)", dwPosition);
    return 0;
}

// #653
int WINAPI XNotifyDelayUI(int a1)
{
    //TRACE("XNotifyDelayUI");
	return 0;
}

// #1082: XGetOverlappedExtendedError
DWORD WINAPI XGetOverlappedExtendedError(PXOVERLAPPED pOverlapped)
{
	if( pOverlapped == 0 )
	{
		//TRACE( "XGetOverlappedExtendedError  (pOverlapped = NULL)" );

		return ERROR_INVALID_PARAMETER;
	}



//  TRACE("XGetOverlappedExtendedError  (pOverlapped = %X) (internalLow = %X, internalHigh = %X, hEvent = %X, error = %X)",
//		pOverlapped,
//		pOverlapped->InternalLow, pOverlapped->InternalHigh, pOverlapped->hEvent, pOverlapped->dwExtendedError );


	//Check_Overlapped( pOverlapped );


	return pOverlapped->dwExtendedError;
}


// #1083: XGetOverlappedResult
DWORD WINAPI XGetOverlappedResult(PXOVERLAPPED pOverlapped, DWORD * pResult, DWORD bWait)
{
	//TRACE("XGetOverlappedResult  (pOverlapped = %X, pResult = %X, bWait = %d)  (internalLow = %X, internalHigh = %X)",
	//	pOverlapped, pResult, bWait, pOverlapped->InternalLow, pOverlapped->InternalHigh );


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

// #5006: XShowMessagesUI
int WINAPI XShowMessagesUI (DWORD dwUserIndex)
{
    TRACE("XShowMessagesUI");
    return 1;   // ERROR_NOT_LOGGED_ON
}



// TODO: add correct structures
typedef void XHV_INIT_PARAMS;
typedef XHV_INIT_PARAMS* PXHV_INIT_PARAMS;


// #5008
int WINAPI XHVCreateEngine( PXHV_INIT_PARAMS pParams, PHANDLE phWorkerThread, PIXHV2ENGINE *ppEngine )
{
    TRACE("XHVCreateEngine  (pParams = %X, phWorkerThread = %X, pEngine = %X)",
			pParams, phWorkerThread, ppEngine );


		if( phWorkerThread )
		{
			*phWorkerThread = CreateMutex(0,0,0);

			TRACE( "- Handle = %X", *phWorkerThread );
		}


		if (ppEngine)
		{
			*ppEngine = (PIXHV2ENGINE) &hv2Engine;

			TRACE( "- hv2Engine = %X", *ppEngine );
		}


		return ERROR_SUCCESS;
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


// #5208: XShowGameInviteUI
int WINAPI XShowGameInviteUI (DWORD dwUserIndex, void * pXuidRecipients, DWORD cRecipients, LPCWSTR pszText)
{
    TRACE("XShowGameInviteUI");
    return 1; // ERROR_NOT_LOGGED_ON
}


// #5209: XShowMessageComposeUI
int WINAPI XShowMessageComposeUI (DWORD dwUserIndex, void * pXuidRecepients, DWORD cRecipients, void * wszText)
{
    TRACE("XShowMessageComposeUI");
    return 1;   // ERROR_NOT_LOGGED_ON
}


// #5210: XShowFriendRequestUI
int WINAPI XShowFriendRequestUI (DWORD dwUserIndex, XUID xuidUser)
{
    TRACE("XShowFriendRequestUI");
    return 1;
}


// #5212: XShowCustomPlayerListUI
DWORD WINAPI XShowCustomPlayerListUI(int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10, int a11, int a12)
{
	TRACE("XShowCustomPlayerListUI");
	return 1;
}


// #5214: XShowPlayerReviewUI
int WINAPI XShowPlayerReviewUI (DWORD, DWORD, DWORD)
{
    TRACE("XShowPlayerReviewUI");
    return 0;
}


// #5215: XShowGuideUI
int WINAPI XShowGuideUI (DWORD)
{
	TRACE("XShowGuideUI");


	// signin change
	sys_ui = -1;


	extern void GSCustomMenuCall_Guide();
	GSCustomMenuCall_Guide();


	return 0;
}


HWND hGameWnd = NULL;

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {

	DWORD wndPID;
	GetWindowThreadProcessId (hwnd, &wndPID);
	if (wndPID == *(DWORD*)&lParam)
	{
		hGameWnd = hwnd;
		return FALSE;
	}
	return TRUE;
}

struct XShowKeyboardUI_DATA
{
	LPCWSTR wseDefaultText;
	LPCWSTR wszTitleText;
	LPCWSTR wszDescriptionText;
	LPWSTR wszResultText;
	DWORD cchResultText;
	DWORD ret;
};


BOOL CALLBACK MyDlgProc_KeyboardUI(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static XShowKeyboardUI_DATA* keydata = NULL;

	switch (message)
	{
	case WM_INITDIALOG:
		{
			keydata = (XShowKeyboardUI_DATA*)lParam;
			if(keydata)
			{
				RECT desktop;
				RECT dialog;
				const HWND hDesktop = GetDesktopWindow();
				GetWindowRect(hDesktop, &desktop);
				GetWindowRect(hDlg, &dialog);
				SetWindowPos(hDlg,HWND_TOPMOST,(desktop.right/2) - (dialog.right/2),(desktop.bottom/2) - (dialog.bottom/2),NULL,NULL, SWP_NOSIZE);

				SetWindowText (hDlg, keydata->wszTitleText);
				SetDlgItemText(hDlg,IDC_EDIT1,keydata->wseDefaultText);
				SetDlgItemText(hDlg,IDC_DSC1,keydata->wszDescriptionText);
			}
			return TRUE;
		}

	case WM_COMMAND:
		if( LOWORD(wParam) == IDCANCEL )
		{
			keydata->ret = LOWORD(wParam);

			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}


		if (LOWORD(wParam) == IDOK )
		{
			keydata->ret = LOWORD(wParam);

			if(keydata && keydata->wszResultText && keydata->cchResultText)
				GetDlgItemText(hDlg, IDC_EDIT1, keydata->wszResultText, keydata->cchResultText);

			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
	}

	return FALSE;
}


// #5216: XShowKeyboardUI
DWORD WINAPI XShowKeyboardUI (DWORD dwUserIndex, DWORD dwFlags, LPCWSTR wseDefaultText, LPCWSTR wszTitleText, LPCWSTR wszDescriptionText, LPWSTR wszResultText, DWORD cchResultText, PXOVERLAPPED pOverlapped)
{
	TRACE("XShowKeyboardUI  (dwUserIndex = %d, dwFlags = %X, wseDefaultText = %s, wszTitleText = %s, wszDescriptionText = %s, wszResultText = %X, cchResultText = %X, pOverlapped = %X)",
		dwUserIndex, dwFlags, wseDefaultText, wszTitleText, wszDescriptionText, wszResultText, cchResultText, pOverlapped );


	DWORD dwPid = GetCurrentProcessId();
	EnumWindows(EnumWindowsProc,(LPARAM)&dwPid);
	//if(!IsWindow(hGameWnd))
		hGameWnd = NULL;


	XShowKeyboardUI_DATA keydata;


	if(cchResultText && wszResultText)
	{
		keydata.cchResultText = cchResultText;
		keydata.wseDefaultText = wseDefaultText;
		keydata.wszDescriptionText = wszDescriptionText;
		keydata.wszTitleText = wszTitleText;
		keydata.wszResultText = wszResultText;
	}


	DialogBoxParam(hThis, MAKEINTRESOURCE(IDD_XSHOWKEYBOARDUI), hGameWnd, MyDlgProc_KeyboardUI, (LPARAM)&keydata);


	if( keydata.ret == IDOK )
		keydata.ret = ERROR_SUCCESS;

	else
		keydata.ret = ERROR_CANCELLED;


	TRACE( "- code = %X", keydata.ret );


	if( pOverlapped )
	{
		pOverlapped->InternalLow = keydata.ret;
		pOverlapped->dwExtendedError = keydata.ret;


		Check_Overlapped( pOverlapped );


		return ERROR_IO_PENDING;
	}


	return keydata.ret;
}


// #5251: XCloseHandle
BOOL WINAPI XCloseHandle (HANDLE hObject)
{
	//TRACE("XCloseHandle  (hObject = %X)", hObject );
	BOOL ret = 0;
	
	if(hObject) ret = CloseHandle(hObject);
	
	return ret;
}


// #5252: XShowGamerCardUI
int WINAPI XShowGamerCardUI (DWORD, DWORD, DWORD)
{
	TRACE("XShowGamerCardUI");


	// signin change
	sys_ui = -1;


	return 0;
}


// #5254: XCancelOverlapped
int WINAPI XCancelOverlapped (DWORD a1)
{
    TRACE("XCancelOverlapped  (a1 = %X)", a1);


    return 0;
}


HANDLE ServerEnum = NULL;
bool ServerEnumRan = false;
ServerList LiveManager;
// #5256: XEnumerate
int WINAPI XEnumerate(HANDLE hEnum, CHAR *pvBuffer, DWORD cbBuffer, PDWORD pcItemsReturned, PXOVERLAPPED pOverlapped)
{
	static int print = 0;


	if( print < 100 )
	{
		TRACE("XEnumerate  (hEnum = %X, pvBuffer = %X, cbBuffer = %X, pcItemsReturned = %X, pOverlapped = %X)",
			hEnum, pvBuffer, cbBuffer, pcItemsReturned, pOverlapped);

		print++;
	}



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
				if( print < 100 )
					TRACE( "- Full enumerate" );
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
		for( 0; achieveinit < 65536; achieveinit++ )
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

		while (LiveManager.GetRunning() == true && LiveManager.GetTotalServers() == 0 || LiveManager.servers_left > 0)
		{
			
		}

		if (LiveManager.GetRunning() == false && LiveManager.GetTotalServers() == 0)
		{
			LiveManager.GetServers();

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

			
		}
	
	}

	if( async == FALSE )
	{
		if( *pcItemsReturned )
		{
			if( print < 100 )
				TRACE("- ItemsReturned = %d", *pcItemsReturned);
	
			return ERROR_SUCCESS;
		}

		else
		{
			if( print < 100 )
				TRACE("- NO_MORE_FILES");
			*pcItemsReturned = 0;
			return ERROR_SUCCESS;
		}
	}

	else
	{
		if( pOverlapped->InternalHigh )
		{
			if( print < 100 )
				TRACE( "- async items = %d", pOverlapped->InternalHigh );


			pOverlapped->InternalLow = ERROR_SUCCESS;
			pOverlapped->dwExtendedError = ERROR_SUCCESS;


			Check_Overlapped( pOverlapped );


			return ERROR_IO_PENDING;
		}

		else
		{
			if( print < 100 )
				TRACE( "- async = NO_MORE_FILES" );


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


// #5260: XShowSigninUI
int WINAPI XShowSigninUI( DWORD cPanes, DWORD dwFlags )
{
	TRACE("XShowSigninUI  (cPanes = %d, dwFlags = %X", cPanes, dwFlags);


	sys_ui = -1;


	return 0;
}


// #5261: XUserGetXUID
int WINAPI XUserGetXUID (DWORD dwUserIndex, PXUID pXuid)
{
	static int print = 0;


	if( print < 15 )
	{
	  TRACE("XUserGetXUID  (userIndex = %d, pXuid = %X)",
			dwUserIndex, pXuid );

		print++;
	}



	if(pXuid && g_signin[ dwUserIndex ])
	{
		wsprintf( strw, L"%I64x", xFakeXuid[dwUserIndex] );
		if( print < 15 ) TRACE("- Xuid = %s", strw );


		*pXuid = xFakeXuid[dwUserIndex];
		return ERROR_SUCCESS;
	}


	if( print < 15 ) TRACE("- No user" );


	// error
	return -1;
}


// #5262: XUserGetSigninState
XUSER_SIGNIN_STATE WINAPI XUserGetSigninState (DWORD dwUserIndex)
{
	XUSER_SIGNIN_STATE ret = eXUserSigninState_NotSignedIn;


	static int print = 0;


	if( print < 15 )
	{
		TRACE("XUserGetSigninState  (index = %d)", dwUserIndex );
		print++;
	}



	if(dwUserIndex == 0)
	{
		if( g_online )
		{
			ret = eXUserSigninState_SignedInToLive;


			if( print < 15 )
				TRACE("- Online - Live" );
		}

		else
		{
			ret = eXUserSigninState_SignedInLocally;


			if( print < 15 )
				TRACE("- Offline - not live" );
		}
	}

	else
	{
		if( print < 15 )
			TRACE("- Not signed in" );
	}


	return ret;
}


// #5263: XUserGetName
DWORD WINAPI XUserGetName(DWORD dwUserIndex, LPSTR szUserName, DWORD cchUserName)
{
	static int print = 0;


	if( print < 15 )
	{
		TRACE("XUserGetName  (userIndex = %d, userName = %X, cchUserName = %d)", dwUserIndex, szUserName, cchUserName);
		print++;
	}


	if(szUserName)
	{
		strncpy(szUserName, g_szUserName[dwUserIndex], 16);
		cchUserName = strlen( g_szUserName[dwUserIndex] );


		if( print < 15 )
		{
			mbstowcs( strw, szUserName, 16 );

	    TRACE("- name = %s, len = %d", strw, cchUserName);
		}


		return ERROR_SUCCESS;
	}

	return ERROR_NO_SUCH_USER;
}


// #5264: XUserAreUsersFriends
int WINAPI XUserAreUsersFriends(DWORD dwUserIndex, DWORD * pXuids, DWORD dwXuidCount, DWORD * pResult, PXOVERLAPPED pOverlapped)
{
    TRACE("XUserAreUsersFriends");
    return ERROR_NOT_LOGGED_ON;
}


// #5265: XUserCheckPrivilege
DWORD WINAPI XUserCheckPrivilege (DWORD dwUserIndex, XPRIVILEGE_TYPE privilegeType, PBOOL pfResult)
{
	static int print = 0;


	if( print < 15 )
	{
		


		if( privilegeType == XPRIVILEGE_MULTIPLAYER_SESSIONS )
			TRACE( "- MULTIPLAYER_SESSIONS" );

		else if (privilegeType == XPRIVILEGE_COMMUNICATIONS)
		{
			TRACE(" - COMMUNICATIONS");
		}

		else if( privilegeType == XPRIVILEGE_PROFILE_VIEWING )
			TRACE( "- PROFILE_VIEWING" );

		else if( privilegeType == XPRIVILEGE_PRESENCE )
			TRACE( "- PRESCENCE" );

		else
			TRACE( "- UNKNOWN" );

		TRACE("XUserCheckPrivilege  (userIndex = %d, privilegeType = %d, pfResult = %X)",
			dwUserIndex, privilegeType, pfResult);

		print++;
	}

	if(dwUserIndex == 0)
	{
		if( print < 15 )
			TRACE("- TRUE");



		if(pfResult) *pfResult = TRUE;
		return ERROR_SUCCESS;
	}
	else
		*pfResult = FALSE;


	return ERROR_NOT_LOGGED_ON;
}


// #5267: XUserGetSigninInfo
int WINAPI XUserGetSigninInfo (DWORD dwUserIndex, DWORD dwFlags, PXUSER_SIGNIN_INFO pSigninInfo)
{
	static int print = 0;


	if( print < 15 )
	{
		TRACE("XUserGetSigninInfo( userIndex = 0x%08x, dwFlags = 0x%08x, pSigninInfo = 0x%08x)",dwUserIndex,dwFlags,pSigninInfo);


		print++;
	}
	
	
	

#if 0
	while(1)
		Sleep(1);
#endif


	DWORD ret = ERROR_NO_SUCH_USER;
	SetLastError(ret);



	// return same xuid for any profile request
	if( pSigninInfo && g_signin[ dwUserIndex ] )
	{
		pSigninInfo->xuid = xFakeXuid[ dwUserIndex ];
		pSigninInfo->dwInfoFlags = XUSER_INFO_FLAG_LIVE_ENABLED;

		if( g_online != 0 )
		{
			if( print < 15 ) TRACE( "- Signed in: Online" );

			pSigninInfo->UserSigninState = eXUserSigninState_SignedInToLive;
		}

		else
		{
			if( print < 15 ) TRACE( "- Signed in: Offline" );

			pSigninInfo->UserSigninState = eXUserSigninState_SignedInLocally;
		}

		pSigninInfo->dwGuestNumber = 0;
		pSigninInfo->dwSponsorUserIndex = 0;
		strncpy( pSigninInfo->szUserName, g_szUserName[ dwUserIndex ], 16 );
		ret = ERROR_SUCCESS;
	}

	else
	{
		if( print < 15 ) TRACE( "- Not signed in" );

		pSigninInfo->xuid = INVALID_XUID;
	}



	SetLastError(ret);
	return ret;
}


// #5270: XNotifyCreateListener
HANDLE WINAPI XNotifyCreateListener( ULONGLONG qwAreas )
{
	TRACE("XNotifyCreateListener  (0x%016x)", qwAreas);


	g_dwFakeListener = CreateMutex(NULL,NULL,NULL);



	g_listener[g_dwListener].id = g_dwFakeListener;
	g_listener[g_dwListener].area = qwAreas;
	g_listener[g_dwListener].print = 0;
	g_dwListener++;


	SetEvent( g_dwFakeListener );


	TRACE("- handle = %X", g_dwFakeListener);
	return g_dwFakeListener;
}


// #5271: XShowPlayersUI
int WINAPI XShowPlayersUI (DWORD dwUserIndex)
{
    TRACE("XShowPlayersUI");
    return 1;
}


// #5273: XUserReadGamerpictureByKey
int WINAPI XUserReadGamerpictureByKey (DWORD, DWORD, DWORD, DWORD, DWORD, DWORD)
{
    TRACE("XUserReadGamerpictureByKey");
    return 0;
}


// #5275: XShowFriendsUI
int WINAPI XShowFriendsUI (DWORD)
{
    TRACE("XShowFriendsUI");
    return 0;
}

// #5276: XUserSetProperty
void WINAPI XUserSetProperty(DWORD dwUserIndex, DWORD dwPropertyId, DWORD cbValue, CONST VOID *pvValue)
{
    TRACE("XUserSetProperty");
    return;
}

static const std::unordered_map <std::string, std::string> singleplayer_maps
{
	{ "00a_introduction", "The Heretic" },
	{ "01a_tutorial", "The Armory" },
	{ "01b_spacestation", "Cairo Station" },
	{ "03a_oldmombasa", "Outskirts" },
	{ "03b_newmombasa", "Metropolis" },
	{ "04a_gasgiant", "The Arbiter" },
	{ "04b_floodlab", "The Oracle" },
	{ "05a_deltaapproach", "Delta Halo" },
	{ "05b_deltatowers", "Regret" },
	{ "06a_sentinelwalls", "Sacred Icon" },
	{ "06b_floodzone", "Quarantine Zone" },
	{ "07a_highcharity", "Gravemind" },
	{ "07b_forerunnership", "High Charity" },
	{ "08a_deltacliffs", "Uprising" },
	{ "08b_deltacontrol", "The Great Journey" }
};

static const std::unordered_map <int, std::pair<std::string, std::string>> campaign_difficulty_list
{
	{ 0,{ "campaign_easy", "Easy" } },
	{ 1,{ "campaign_normal", "Normal" } },
	{ 2,{ "campaign_medium", "Heroic" } },
	{ 3,{ "campaign_hard", "Legendary" } }
};

static const std::unordered_map <int, std::string> game_mode_list
{
	{ 1,"gamemode_ctf" },
	{ 2,"gamemode_slayer" },
	{ 3,"gamemode_oddball" },
	{ 4,"gamemode_KotH" },
	{ 7,"gamemode_juggernaut" },
	{ 8,"gamemode_territories" },
	{ 9,"gamemode_assault" }
};

void update_player_count()
{
	int player_count_a = *reinterpret_cast<BYTE*>(h2mod->GetBase() + 0x506974);
	int player_count_b = *reinterpret_cast<BYTE*>(h2mod->GetBase() + 0x50E4FC);
	int max_player_count_a = *reinterpret_cast<BYTE*>(h2mod->GetBase() + 0x50A3A0);
	int max_player_count_b = *reinterpret_cast<BYTE*>(h2mod->GetBase() + 0x511F28);

	DiscordInterface::SetPlayerCountInfo(
		player_count_a ? player_count_a : player_count_b,
		max_player_count_a ? max_player_count_a : max_player_count_b
	);
}

std::wstring_convert<std::codecvt_utf8<wchar_t>> wstring_to_string;
std::string getEnglishMapName()
{
	wchar_t* englishMapName = (wchar_t*)(h2mod->GetBase() + 0x97737C);
	return  wstring_to_string.to_bytes(englishMapName);
}

std::string getVariantName()
{
	std::wstring variant = reinterpret_cast<wchar_t*>(h2mod->GetBase() + 0x97777C);
	variant = variant.substr(0, variant.find_last_not_of(L"\xE008\t\n ") + 1);
	return wstring_to_string.to_bytes(variant);
}

std::string gamemode_id_to_string(int id)
{
	std::string gamemode = "gamemode_unknown";
	auto it = game_mode_list.find(id);
	if (it != game_mode_list.end())
		gamemode = it->second;
	return gamemode;
}

enum class ContextPresence {
	mainmenu,
	unknown,
	server_browser,
	singleplayer,
	lobby,
	results,
	public_game = 7,
	invite_only_game,
	closed_game
};

DWORD diff_level;
// #5277: XUserSetContext
DWORD WINAPI XUserSetContext(DWORD dwUserIndex, DWORD dwContextId, DWORD dwContextValue)
{
  TRACE("XUserSetContext  (userIndex = %d, contextId = %d, contextValue = %d)",
		dwUserIndex, dwContextId, dwContextValue );

  if (h2mod->Server || !H2Config_discord_enable || H2GetInstanceId() > 1)
	  return ERROR_SUCCESS;

  if (dwContextId == 0x00000003)
	  diff_level = dwContextValue;

	if( dwContextId == X_CONTEXT_PRESENCE )
	{
		TRACE( "- X_CONTEXT_PRESENCE = %d", dwContextValue );
		int GameGlobals = *reinterpret_cast<int*>(h2mod->GetBase() + 0x482D3C);
		int GameEngineGlobals = *reinterpret_cast<int*>(h2mod->GetBase() + 0x4BF8F8);

		std::wstring map_name_wide = (wchar_t*)(h2mod->GetBase() + 0x46DAE8);
		map_name_wide = map_name_wide.substr(map_name_wide.find_last_of(L"\\") + 1);

		std::string map_name = wstring_to_string.to_bytes(map_name_wide);
		TRACE_GAME("[Discord] map_name: %ws", map_name_wide.c_str());

		switch (static_cast<ContextPresence>(dwContextValue)) {
		case ContextPresence::singleplayer:
		{
			std::wstring map_name_wide = (wchar_t*)(h2mod->GetBase() + 0x46DD88);
			map_name_wide = map_name_wide.substr(map_name_wide.find_last_of(L"\\") + 1);

			std::string map_name = wstring_to_string.to_bytes(map_name_wide);

			std::string level_name = map_name;
			auto it = singleplayer_maps.find(map_name);
			if (it != singleplayer_maps.end())
				level_name = it->second;

			auto diff = campaign_difficulty_list.at(diff_level);
			DiscordInterface::SetGameState(
				map_name,
				"Campaign",
				level_name,
				diff.first,
				diff.second
			);
			break;
		}
		case ContextPresence::public_game:
		case ContextPresence::invite_only_game:
		case ContextPresence::closed_game:
		{
			int game_engine_type = *reinterpret_cast<BYTE*>(GameEngineGlobals + 0xC54);

			DiscordInterface::SetGameState(
				map_name,
				"Multiplayer",
				getEnglishMapName(),
				gamemode_id_to_string(game_engine_type),
				getVariantName()
			);
			update_player_count();
			break;
		}
		case ContextPresence::mainmenu:
		{
			DiscordInterface::SetGameState("default", "In main menu");
			break;
		}
		case ContextPresence::lobby:
		{
			DiscordInterface::SetGameState("default", "In Lobby");
			update_player_count();
			break;
		}
		case ContextPresence::server_browser:
		{
			DiscordInterface::SetGameState("default", "Browsing server list");
			break;
		}
		case ContextPresence::results:
		{
			DiscordInterface::SetGameState("default", "Reading carnage report", true);
			break;
		}
	}
}

	else if( dwContextId == X_CONTEXT_GAME_TYPE )
	{
		TRACE( "- X_CONTEXT_GAME_TYPE = %d", dwContextValue );

		sessionDetails.dwGameType = dwContextValue;
	}

	else if( dwContextId == X_CONTEXT_GAME_MODE )
	{
		TRACE( "- X_CONTEXT_GAME_MODE = %X", dwContextValue );

		sessionDetails.dwGameMode = dwContextValue;
	}



	return ERROR_SUCCESS;
}



// #5278: XUserWriteAchievements
DWORD WINAPI XUserWriteAchievements (DWORD count, PXUSER_ACHIEVEMENT pAchievement, LPVOID pOverlap )
{
    TRACE("XUserWriteAchievements  (count = %x, buffer = %x, overlap = %x)",
			count, pAchievement, pOverlap );

	if( count > 0 )
	{
		while( count > 0 )
		{
			achievementList[ pAchievement->dwAchievementId ] = 1;

			TRACE2("Achievement %d unlocked by Player %d", pAchievement->dwAchievementId, pAchievement->dwUserIndex);
			pAchievement++;

			count--;
		}

		// crash-protect progress
		//SaveAchievements();
	}
	
	return 997;
}


// #5279: XUserReadAchievementPicture
int WINAPI XUserReadAchievementPicture(int a1, int a2, int a3, int a4, int a5, int a6, int a7)
{
	TRACE("XUserReadAchievementPicture");

	return ERROR_INVALID_PARAMETER;
}


// #5280: XUserCreateAchievementEnumerator
DWORD WINAPI XUserCreateAchievementEnumerator(DWORD dwTitleId, DWORD dwUserIndex, XUID xuid, DWORD dwDetailFlags, DWORD dwStartingIndex, DWORD MaxEnumerator, PDWORD pchBuffer, PHANDLE phEnum )
{
	TRACE("XUserCreateAchievementEnumerator (dwStartingIndex = %d, MaxEnumerator = %d)", dwStartingIndex, MaxEnumerator);


	if(pchBuffer) *pchBuffer = MaxEnumerator * sizeof(XACHIEVEMENT_DETAILS);
	if(phEnum) *phEnum = g_dwFakeAchievementContent = CreateMutex(NULL,NULL,NULL);


	achieveinit = 0;


	TRACE("- Handle = %X, pchBuffer = %d", g_dwFakeAchievementContent, *pchBuffer );

	return ERROR_SUCCESS;
}

// #5281: XUserReadStats
DWORD WINAPI XUserReadStats (DWORD dwTitleId, DWORD dwNumXuids, CONST XUID *pXuids, DWORD dwNumStatsSpecs, DWORD *pSpecs, DWORD *pcbResults, DWORD *pResults, PXOVERLAPPED pOverlapped )
{
	TRACE("XUserReadStats  (titleId = %X, numXuids = %d, pXuids = %X, numStatsSpecs = %d, pSpecs = %X, pcbResults = %X, pResults = %X, pOverlapped = %X",
		dwTitleId, dwNumXuids, pXuids, dwNumStatsSpecs, pSpecs, pcbResults, pResults, pOverlapped );


	if (pcbResults)
	{
		// return size
		if( *pcbResults == 0 )
		{
			*pcbResults = 4;


			if( pOverlapped )
			{
					pOverlapped->InternalLow = ERROR_INSUFFICIENT_BUFFER;
					pOverlapped->dwExtendedError = ERROR_INSUFFICIENT_BUFFER;


					Check_Overlapped( pOverlapped );

					return ERROR_IO_PENDING;
			}

			else
				return ERROR_INSUFFICIENT_BUFFER;
		}
	}
    

	if (pResults)
		*pResults = 0;


	if( pOverlapped )
	{
		pOverlapped->InternalLow = -1;
		pOverlapped->dwExtendedError = -1;


		Check_Overlapped( pOverlapped );

		return ERROR_IO_PENDING;
	}


	return ERROR_SUCCESS;
}


// #5284: XUserCreateStatsEnumeratorByRank
DWORD WINAPI XUserCreateStatsEnumeratorByRank (DWORD dwTitleId, DWORD dwRankStart, DWORD dwNumRows, DWORD dwNuStatSpec, void * pSpecs, DWORD * pcbBuffer, PHANDLE phEnum)
{
	TRACE("XUserCreateStatsEnumeratorByRank");
    
	if (pcbBuffer)
		*pcbBuffer = 0;
   
	if(phEnum) 
		*phEnum = 0;
    
	return 1;
}


// #5285: XUserCreateStatsEnumeratorByRating
DWORD WINAPI XUserCreateStatsEnumeratorByRating (DWORD dwTitleId, LONGLONG, DWORD, DWORD, void *, DWORD * pcbBuffer, PHANDLE phEnum)
{
	TRACE("XUserCreateStatsEnumeratorByRating");
  
	if (pcbBuffer)
		*pcbBuffer = 0;
	
	if(phEnum) 
		*phEnum = 0;
    
	return 1;
}


// #5286: XUserCreateStatsEnumeratorByXuid
DWORD WINAPI XUserCreateStatsEnumeratorByXuid (DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD * pcbBuffer, PHANDLE phEnum)
{
	TRACE("XUserCreateStatsEnumeratorByXuid");
  
	if(pcbBuffer)
		*pcbBuffer = 0;

	if(phEnum) 
		*phEnum = 0;
	
	return 1;
}


// #5292: XUserSetContextEx
int WINAPI XUserSetContextEx (DWORD dwUserIndex, DWORD dwContextId, DWORD dwContextValue, PXOVERLAPPED pOverlapped)
{
	TRACE("XUserSetContextEx  (userIndex = %d, contextId = %d, contextValue = %X, pOverlapped = %X)",
		dwUserIndex, dwContextId, dwContextValue, pOverlapped );


	//return 0;


	if( dwContextId == X_CONTEXT_PRESENCE )
	{
		TRACE( "- X_CONTEXT_PRESENCE = %X", dwContextValue );
	}

	else if( dwContextId == X_CONTEXT_GAME_TYPE )
	{
		TRACE( "- X_CONTEXT_GAME_TYPE = %X", dwContextValue );

		sessionDetails.dwGameType = dwContextValue;
	}

	else if( dwContextId == X_CONTEXT_GAME_MODE )
	{
		TRACE( "- X_CONTEXT_GAME_MODE = %X", dwContextValue );

		sessionDetails.dwGameMode = dwContextValue;
	}

	
	
	if( pOverlapped == 0 )
		return ERROR_SUCCESS;

	else
	{
		pOverlapped->InternalHigh = 0;
		pOverlapped->InternalLow = ERROR_SUCCESS;
		pOverlapped->dwExtendedError = ERROR_SUCCESS;


		Check_Overlapped( pOverlapped );

		return ERROR_IO_PENDING;
	}
}


// #5293: XUserSetPropertyEx
int WINAPI XUserSetPropertyEx (DWORD dwUserIndex, DWORD dwPropertyId, DWORD cbValue, void * pvValue, PXOVERLAPPED pOverlapped)
{
	TRACE("XUserSetPropertyEx  (userIndex = %d, propertyId = %X, cbValue = %d, pvValue = %X, pOverlapped = %X)",
		dwUserIndex, dwPropertyId, cbValue, pvValue, pOverlapped );


	if( pOverlapped == 0 )
		return ERROR_SUCCESS;

	else
	{
		pOverlapped->InternalHigh = 0;
		pOverlapped->InternalLow = ERROR_SUCCESS;
		pOverlapped->dwExtendedError = ERROR_SUCCESS;


		Check_Overlapped( pOverlapped );

		return ERROR_IO_PENDING;
	}
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
int WINAPI XUserMuteListQuery (DWORD, DWORD, DWORD, DWORD)
{
    TRACE("XUserMuteListQuery");
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
	mapManager->cleanup();
    return 0;
}

// #5331: XUserReadProfileSettings
DWORD WINAPI XUserReadProfileSettings (DWORD dwTitleId, DWORD dwUserIndex, DWORD dwNumSettingIds,
									   DWORD * pdwSettingIds, DWORD * pcbResults, XUSER_READ_PROFILE_SETTING_RESULT * pResults, PXOVERLAPPED pOverlapped)
{
	TRACE("XUserReadProfileSettings  (TitleId = %d, UserIndex = %d, NumSettingIds = %d, pdwSettingIds = %X, pcbResults = %d, pResults = %X, pOverlapped = %X)",
		dwTitleId, dwUserIndex, dwNumSettingIds, pdwSettingIds, *pcbResults, pResults, pOverlapped );


	BOOL async;

	if( pOverlapped )
		async = TRUE;

	else
		async = FALSE;



	if( pcbResults )
	{
		// find buffer size
		if (*pcbResults == 0 )
		{
			int size;


			size = 0;
			for( int lcv = 0; lcv < dwNumSettingIds; lcv++ )
			{
				int settingType, settingSize, settingId;

				settingType = ( pdwSettingIds[lcv] >> 28 ) & 0x0F;
				settingSize = ( pdwSettingIds[lcv] >> 16 ) & 0xFFF;
				settingId = ( pdwSettingIds[lcv] >> 0 ) & 0x3FFF;


				wsprintf( strw, L"- Settings %d: %X  (Type = %X, Size = %d", lcv+1,
					pdwSettingIds[lcv], settingType, settingSize );


				switch( settingId )
				{
				case 3: wsprintf( strw, L"%s, id = XPROFILE_OPTION_CONTROLLER_VIBRATION)", strw ); break;
				case 1: wsprintf( strw, L"%s, id = XPROFILE_GAMERCARD_NXE)", strw ); break;
				case 2: wsprintf( strw, L"%s, id = XPROFILE_GAMER_YAXIS_INVERSION)", strw ); break;
				case 24: wsprintf( strw, L"%s, id = XPROFILE_GAMER_CONTROL_SENSITIVITY)", strw ); break;

				case 4: wsprintf( strw, L"%s, id = XPROFILE_GAMERCARD_ZONE)", strw ); break;
				case 5: wsprintf( strw, L"%s, id = XPROFILE_GAMERCARD_REGION)", strw ); break;
				case 6: wsprintf( strw, L"%s, id = XPROFILE_GAMERCARD_CRED)", strw ); break;
				case 11: wsprintf( strw, L"%s, id = XPROFILE_GAMERCARD_REP)", strw ); break;
				case 71: wsprintf( strw, L"%s, id = XPROFILE_GAMERCARD_YEARS)", strw ); break;
				case 72: wsprintf( strw, L"%s, id = XPROFILE_GAMERCARD_BOUBLES)", strw ); break;
				case 15: wsprintf( strw, L"%s, id = XPROFILE_GAMERCARD_PICTURE_KEY)", strw ); break;
				case 64: wsprintf( strw, L"%s, id = XPROFILE_GAMERCARD_NAME)", strw ); break;
				case 65: wsprintf( strw, L"%s, id = XPROFILE_GAMERCARD_LOCATION)", strw ); break;
				case 17: wsprintf( strw, L"%s, id = XPROFILE_GAMERCARD_MOTTO)", strw ); break;
				case 18: wsprintf( strw, L"%s, id = XPROFILE_GAMERCARD_TITLES_PLAYED)", strw ); break;
				case 19: wsprintf( strw, L"%s, id = XPROFILE_GAMERCARD_ACHIEVEMENTS_EARNED)", strw); break;
				
				case 21: wsprintf( strw, L"%s, id = XPROFILE_GAMER_DIFFICULTY)", strw ); break;

				case 29: wsprintf( strw, L"%s, id = XPROFILE_GAMER_PREFERRED_COLOR_FIRST)", strw ); break;
				case 30: wsprintf( strw, L"%s, id = XPROFILE_GAMER_PREFERRED_COLOR_SECOND)", strw ); break;
				
				case 34: wsprintf( strw, L"%s, id = XPROFILE_GAMER_ACTION_AUTO_AIM)", strw ); break;
				case 35: wsprintf( strw, L"%s, id = XPROFILE_GAMER_ACTION_AUTO_CENTER)", strw ); break;
				case 36: wsprintf( strw, L"%s, id = XPROFILE_GAMER_ACTION_MOVEMENT_CONTROL)", strw ); break;
				
				case 38: wsprintf( strw, L"%s, id = XPROFILE_GAMER_RACE_TRANSMISSION)", strw ); break;
				case 39: wsprintf( strw, L"%s, id = XPROFILE_GAMER_RACE_CAMERA_LOCATION)", strw ); break;
				case 40: wsprintf( strw, L"%s, id = XPROFILE_GAMER_RACE_BRAKE_CONTROL)", strw ); break;
				case 41: wsprintf( strw, L"%s, id = XPROFILE_GAMER_RACE_ACCELERATOR_CONTROL)", strw ); break;

				case 56: wsprintf( strw, L"%s, id = XPROFILE_GAMERCARD_TITLE_CRED_EARNED)", strw ); break;
				case 57: wsprintf( strw, L"%s, id = XPROFILE_GAMERCARD_TITLE_ACHIEVEMENTS_EARNED)", strw ); break;
				case 67: wsprintf( strw, L"%s, id = XPROFILE_GAMERCARD_BIO)", strw ); break;
				case 68: wsprintf( strw, L"%s, id = XPROFILE_AVATAR_METADATA)", strw ); break;

				case 0x3FFF: wsprintf( strw, L"%s, id = XPROFILE_TITLE_SPECIFIC1)", strw ); break;
				case 0x3FFE: wsprintf( strw, L"%s, id = XPROFILE_TITLE_SPECIFIC2)", strw ); break;
				case 0x3FFD: wsprintf( strw, L"%s, id = XPROFILE_TITLE_SPECIFIC3)", strw ); break;
				default: wsprintf( strw, L"%s, id = Unknown)", strw ); break;
				}



				TRACE( "%s", strw );
				size += settingSize;
			}


			*pcbResults = size;
			*pcbResults += dwNumSettingIds * sizeof(XUSER_PROFILE_SETTING);
			*pcbResults += sizeof(XUSER_READ_PROFILE_SETTING_RESULT);


			TRACE("- ERROR_INSUFFICIENT_BUFFER  (pcbResults = %d)", *pcbResults);

			if( async )
			{
				pOverlapped->InternalLow = ERROR_INSUFFICIENT_BUFFER;
				pOverlapped->InternalHigh = *pcbResults;
				pOverlapped->dwExtendedError = ERROR_INSUFFICIENT_BUFFER;


				Check_Overlapped( pOverlapped );
	
				return ERROR_IO_PENDING;
			}


			return ERROR_INSUFFICIENT_BUFFER;
		}



		memset( pResults, 0, *pcbResults );


		pResults->dwSettingsLen = dwNumSettingIds;
		pResults->pSettings = (XUSER_PROFILE_SETTING *) ((BYTE *) pResults + sizeof (XUSER_READ_PROFILE_SETTING_RESULT));



		XUSER_PROFILE_SETTING *ptr = pResults->pSettings;
		BYTE *pSettingData = (BYTE *) ptr + dwNumSettingIds * sizeof( XUSER_PROFILE_SETTING );


		// read data values
		for( int lcv = 0; lcv < dwNumSettingIds; lcv++ )
		{
			int settingType, settingSize, settingId;


			settingType = ( pdwSettingIds[lcv] >> 28 ) & 0x0F;
			settingSize = ( pdwSettingIds[lcv] >> 16 ) & 0xFFF;
			settingId = ( pdwSettingIds[lcv] >> 0 ) & 0x3FFF;


			//Local_Storage_W( dwUserIndex, strw );

			wcscat( strw, L"\\Offline\\" );
			//CreateDirectory( strw, NULL );


			switch( settingId )
			{
			case 0x3FFF:
				TRACE( "- XPROFILE_TITLE_SPECIFIC1" );

				wcscat( strw, L"Title1.dat" );
				break;


			case 0x3FFE:
				TRACE( "- XPROFILE_TITLE_SPECIFIC2" );

				wcscat( strw, L"Title2.dat" );
				break;


			case 0x3FFD:
				TRACE( "- XPROFILE_TITLE_SPECIFIC3" );

				wcscat( strw, L"Title3.dat" );
				break;


			default:
				wcscat( strw, L"Settings.txt" );
				break;
			}




			FILE *fp;

			fp = _wfopen( strw, L"rb" );
			if( !fp )
			{
				TRACE( "- Not found: %s", strw );


				ptr->source = XSOURCE_NO_VALUE;
			}

			else
			{
				TRACE( "- Found: %s", strw );


				if( settingType == 6 )
				{
					fread( &ptr->data.binary.cbData, 1, 4, fp );
					fread( pSettingData, 1, ptr->data.binary.cbData, fp );


					ptr->data.binary.pbData = pSettingData;
					ptr->source = XSOURCE_TITLE;
				}


				fclose( fp );
			}


			ptr->data.type = settingType;
			ptr->dwSettingId = pdwSettingIds[lcv];
			ptr->user.dwUserIndex = 0;


			pSettingData += settingSize;
			ptr++;
		}
	}



	TRACE("- pcbResults = %d", *pcbResults);

	if( async )
	{
		pOverlapped->InternalLow = ERROR_SUCCESS;
		pOverlapped->InternalHigh = *pcbResults;
		pOverlapped->dwExtendedError = ERROR_SUCCESS;


		Check_Overlapped( pOverlapped );
		
		return ERROR_IO_PENDING;
	}


	return ERROR_SUCCESS;
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


// #5337: XUserWriteProfileSettings
DWORD WINAPI XUserWriteProfileSettings( DWORD dwUserIndex, DWORD dwNumSettings, const PXUSER_PROFILE_SETTING pSettings, PXOVERLAPPED pOverlapped )
{
	TRACE("XUserWriteProfileSettings  (dwUserIndex = %d, dwNumSettings = %d, pSettings = %X, pOverlapped = %X)",
		dwUserIndex, dwNumSettings, pSettings, pOverlapped );


	for( int lcv = 0; lcv < dwNumSettings; lcv++ )
	{
		int type, size, id;

		type = ( pSettings[lcv].dwSettingId >> 28 ) & 0x0F;
		size = ( pSettings[lcv].dwSettingId >> 16 ) & 0xFFF;
		id = ( pSettings[lcv].dwSettingId >> 0 ) & 0x3FFF;



		TRACE( "- [%d] source = %d, id = %X, type = %d, size = %X, sub-id = %X, type2 = %d",
			lcv,
			pSettings[lcv].source,
			pSettings[lcv].dwSettingId,
			type,
			size,
			id,
			pSettings[lcv].data.type );



		//Local_Storage_W( dwUserIndex, strw );

		wcscat( strw, L"\\Offline\\" );
		//CreateDirectory( strw, NULL );



		switch( id )
		{
		case 0x3FFF:
			TRACE( "- XPROFILE_TITLE_SPECIFIC1  (cbData = %X)", pSettings[lcv].data.binary.cbData );

			wcscat( strw, L"Title1.dat" );
			break;


		case 0x3FFE:
			TRACE( "- XPROFILE_TITLE_SPECIFIC2  (cbData = %X)", pSettings[lcv].data.binary.cbData );

			wcscat( strw, L"Title2.dat" );
			break;


		case 0x3FFD:
			TRACE( "- XPROFILE_TITLE_SPECIFIC3  (cbData = %X)", pSettings[lcv].data.binary.cbData );

			wcscat( strw, L"Title3.dat" );
			break;
		}


		FILE *fp;

		fp = _wfopen( strw, L"wb" );
		if( !fp ) continue;


		if( type == 6 )
		{
			fwrite( &pSettings[lcv].data.binary.cbData, 1, 4, fp );
			fwrite( pSettings[lcv].data.binary.pbData, 1, pSettings[lcv].data.binary.cbData, fp );
		}


		fclose( fp );
	}



	if( pOverlapped )
	{
		pOverlapped->InternalLow = ERROR_SUCCESS;
		pOverlapped->dwExtendedError = ERROR_SUCCESS;
		pOverlapped->InternalLow = 0;


		Check_Overlapped( pOverlapped );
	
		return ERROR_IO_PENDING;
	}


	return ERROR_SUCCESS;
}


// #5338: XPresenceSubscribe
int WINAPI XPresenceSubscribe(int a1, int a2, int a3)
{
	TRACE("XPresenceSubscribe( a1: %08X, a2: %08X, a3: %08X", a1, a2, a3);
	return 0;
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
	TRACE("XUserReadProfileSettingsByXuid");


	Check_Overlapped( pOverlapped );

	return ERROR_NOT_FOUND;
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


// #5365: XShowMarketplaceUI
DWORD WINAPI XShowMarketplaceUI (DWORD dwUserIndex, DWORD dwEntryPoint, ULONGLONG dwOfferId, DWORD dwContentCategories)
{
    TRACE("XShowMarketplaceUI");
    return 1;
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

	if (offset == 0)
		pBuffer->pbData[offset] = 0; //no need of MF.dll anymore

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
		TRACE("- Invalid parameter");
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


// #5250
DWORD WINAPI XShowAchievementsUI(int a1)
{
    TRACE("XShowAchievementsUI");
    return 0;
}


struct XShowMessageBoxUI_DATA
{
	LPCWSTR wszTitleText;
	LPCWSTR wszDescriptionText;
	LPCWSTR *wszButtons;
	DWORD dwButtons;
	DWORD ret;
};


BOOL CALLBACK MyDlgProc_MessageBoxUI(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static XShowMessageBoxUI_DATA* keydata = NULL;

	switch (message)
	{
	case WM_INITDIALOG:
		{
			keydata = (XShowMessageBoxUI_DATA*)lParam;
			if(keydata)
			{
				RECT desktop;
				RECT dialog;
				const HWND hDesktop = GetDesktopWindow();
				GetWindowRect(hDesktop, &desktop);
				GetWindowRect(hDlg, &dialog);
				SetWindowPos(hDlg,HWND_TOPMOST,(desktop.right/2) - (dialog.right/2),(desktop.bottom/2) - (dialog.bottom/2),NULL,NULL, SWP_NOSIZE);


				SetWindowText (hDlg, keydata->wszTitleText);
				SetDlgItemText( hDlg, IDC_DSC1, keydata->wszDescriptionText ); 


				if( keydata->dwButtons >= 1 )
					SetDlgItemText(hDlg,IDOK,keydata->wszButtons[0]);

				if( keydata->dwButtons >= 2 )
					SetDlgItemText(hDlg,IDCANCEL,keydata->wszButtons[1]);
			}
			return TRUE;
		}

	case WM_COMMAND:
		if( LOWORD(wParam) == IDCANCEL )
		{
			keydata->ret = 1;

			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}


		if (LOWORD(wParam) == IDOK )
		{
			keydata->ret = 0;

			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
	}

	return FALSE;
}


// #5266: XShowMessageBoxUI
DWORD WINAPI XShowMessageBoxUI( DWORD dwUserIndex, LPCWSTR wszTitle, LPCWSTR wszText, DWORD cButtons, LPCWSTR *pwszButtons,
															  DWORD dwFocusButton, DWORD dwFlags, MESSAGEBOX_RESULT *pResult, XOVERLAPPED *pOverlapped )
{
  TRACE("XShowMessageBoxUI  (%s = %s)", wszTitle, wszText );


	// Checkme
	//if( dwFlags & XMB_PASSCODEMODE )
	//if( dwFlags & XMB_VERIFYPASSCODEMODE )


	DWORD dwPid = GetCurrentProcessId();
	EnumWindows(EnumWindowsProc,(LPARAM)&dwPid);
	//if(!IsWindow(hGameWnd))
		hGameWnd = NULL;


	XShowMessageBoxUI_DATA keydata;

	keydata.wszDescriptionText = wszText;
	keydata.wszTitleText = wszTitle;
	keydata.wszButtons = pwszButtons;
	keydata.dwButtons = cButtons;


	if( cButtons == 1 )
		DialogBoxParam(hThis, MAKEINTRESOURCE(IDD_XSHOWMESSAGEBOXUI_1), hGameWnd, MyDlgProc_MessageBoxUI, (LPARAM)&keydata);

	else if( cButtons == 2 )
		DialogBoxParam(hThis, MAKEINTRESOURCE(IDD_XSHOWMESSAGEBOXUI_2), hGameWnd, MyDlgProc_MessageBoxUI, (LPARAM)&keydata);


	//MessageBox( 0, wszText, wszTitle, MB_OK );


	if( pResult )
		pResult->dwButtonPressed = keydata.ret;



	if( pOverlapped )
	{
		pOverlapped->InternalLow = ERROR_SUCCESS;
		pOverlapped->dwExtendedError = ERROR_SUCCESS;

		return ERROR_IO_PENDING;
	}


	return ERROR_SUCCESS;
}


// #5274: XUserAwardGamerPicture
DWORD WINAPI XUserAwardGamerPicture(int a1, int a2, int a3, int a4)
{
    TRACE("XUserAwardGamerPicture");
    return 0;
}


// #5287
DWORD WINAPI XUserResetStatsView(int a1, int a2, int a3)
{
    TRACE("XUserResetStatsView");
    return 0;
}


// #5291
DWORD WINAPI XUserResetStatsViewAllUsers(int a1, int a2)
{
    TRACE("XUserResetStatsViewAllUsers");
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


// 5282: ??
DWORD WINAPI XUserReadGamerPicture( DWORD dwUserIndex, BOOL fSmall, PBYTE pbTextureBuffer, DWORD dwPitch, DWORD dwHeight, PXOVERLAPPED pOverlapped )
{
   TRACE("XUserReadGamerPicture  (*** checkme ***)  (userIndex = %d, small = %d, textureBuffer = %X, pitch = %d, height = %d, overlapped = %X)",
		 dwUserIndex, fSmall, pbTextureBuffer, dwPitch, dwHeight, pOverlapped );


	Check_Overlapped( pOverlapped );


	// error - not ready??
	return 0x15;
}


// 5374: ??
DWORD WINAPI XMarketplaceGetDownloadStatus( DWORD a1, DWORD a2, DWORD a3, DWORD a4 )
{
   TRACE("XMarketplaceGetDownloadStatus  (*** checkme ***)  (a1 = %X, a2 = %X, a3 = %X, a4 = %X)",
		 a1, a2, a3, a4 );



	// alt - error 0x57  (INVALID_PARAMETER)
	return 0;
}


// 5375: ??
DWORD WINAPI XMarketplaceGetImageUrl( char *a1, DWORD a2, DWORD a3, DWORD a4, WCHAR *a5 )
{
  TRACE("XMarketplaceGetImageUrl  (*** checkme ***)  (a1 = %X, a2 = %X, a3 = %X, a4 = %X, a5 = %X)",
		a1, a2, a3, a4, a5 );

	//unicode 0, <//global/t:%08x/marketplace/0/%016I64x>,0


	return 0;
}


// 5028: ??
DWORD WINAPI XLiveSecureLoadLibraryW( LPCWSTR libFileName, DWORD a2, DWORD dwFlags )
{
  TRACE("XLiveSecureLoadLibraryW  (?? - FIXME)  (libFileName = %s, a2 = %X, flags = %X)",
		libFileName, a2, dwFlags );


	// not done - error now
  return 0x80070032;
}


// 5230: ??



// 5231: ??
DWORD WINAPI XLocatorServerUnAdvertise( DWORD a1, DWORD a2 )
{
  TRACE("XLocatorServerUnAdvertise  (*** checkme ***)");


	// not done - error now
	return 0x80070057;
}


// 5233: ??
DWORD WINAPI XLocatorGetServiceProperty( DWORD dwUserIndex, DWORD cNumProperties, PXUSER_PROPERTY pProperties, DWORD pOverlapped )
{
  TRACE("XLocatorGetServiceProperty  (*** checkme ***) (dwUserIndex = %X, cNumProperties = %X, pProperties = %X, pOverlapped = %X)",
		dwUserIndex, cNumProperties, pProperties, pOverlapped);


  pProperties[0].value.nData = 1;
  pProperties[1].value.nData = 3;
  pProperties[2].value.nData = 3;
  pProperties[3].value.nData = 7;

	return S_OK;
}


// 5234: ??
DWORD WINAPI XLocatorCreateServerEnumerator(int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8, DWORD* pcbBuffer, PHANDLE phEnum)
{
  TRACE("XLocatorCreateServerEnumerator");
 
  *pcbBuffer = (DWORD)sizeof(XLOCATOR_SEARCHRESULT);

  if (phEnum)
  {
	  *phEnum = CreateMutex(NULL, NULL, NULL);

	  TRACE("- Handle = %X", *phEnum);
	  ServerEnum = *phEnum;
  }

	//PopulateList();
	
	// not done - error now
	return ERROR_SUCCESS;
}


// 5235: ??
DWORD WINAPI XLocatorCreateServerEnumeratorByIDs( DWORD a1, DWORD a2, DWORD a3, DWORD a4, DWORD a5, DWORD a6, DWORD a7, DWORD a8 )
{
  TRACE("XLocatorCreateServerEnumeratorByIDs");


	// not done - error now
	return 0x57;
}


// 5236: ??
DWORD WINAPI XLocatorServiceInitialize( DWORD a1, DWORD a2 )
{
  TRACE("XLocatorServiceInitialize  (a1 = %X, a2 = %X)",
		a1, a2 );


	/*
	Lost Planet, Gears of War
	- LocatorV1.434307DE.RTP.
	*/

#if 0
	while(1)
		Sleep(1);
#endif


	// GFWL offline
	return 0;
}


// 5237: ??
DWORD WINAPI XLocatorServiceUnInitialize( DWORD a1 )
{
  TRACE("XLocatorServiceUnInitialize(a1 = %X)", a1 );


	// not done - error now
	return 0x80004001;
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


// 5288: ??
DWORD WINAPI XUserGetProperty( DWORD a1, DWORD a2, DWORD a3, DWORD a4 )
{
  TRACE("XUserGetProperty  (*** checkme ***) (a1 = %X, a2 = %X, a3 = %X, a4 = %X)",
		a1, a2, a3, a4);


	// not done - error now
	return S_OK;
}


// 5289: ??
DWORD WINAPI XUserGetContext( DWORD a1, DWORD a2, DWORD a3 )
{
  TRACE("XUserGetContext  (*** checkme ***) (a1 = %X, a2 = %X, a3 = %X)",
		a1, a2, a3);


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


// 5299: ??
DWORD WINAPI XShowGuideKeyRemapUI( DWORD a1 )
{
  TRACE("XShowGuideKeyRemapUI  (*** checkme ***) (a1 = %X)",
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


// 5366
DWORD WINAPI XShowMarketplaceDownloadItemsUI( DWORD a1, DWORD a2, DWORD a3, DWORD a4, DWORD a5, DWORD a6 )
{
  TRACE("XShowMarketplaceDownloadItemsUI");


	// not done - error now
	return 0x57;
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



DWORD WINAPI XShowArcadeUI( DWORD a1 )
{
	TRACE( "XShowArcadeUI  (a1 = %X)",
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


DWORD WINAPI XLiveGetUPnPState( DWORD a1 )
{
	TRACE( "XLiveGetUPnPState  (a1 = %X)", a1 );


	return 0;
}

DWORD WINAPI TitleExport_XUserFindUsers(int, int, int, int, int, int, int)
{
	TRACE("TitleExport_XUserFindUsers");
	return 1;
}


// === end of xlive functions ===


INT IXHV2ENGINE::Dummy1( VOID *pThis )
{
	TRACE( "IXHV2Engine::Dummy1" );


	return 0;
}



INT IXHV2ENGINE::Dummy2( VOID *pThis )
{
	TRACE( "IXHV2Engine::Dummy2" );


	return 0;
}



HRESULT IXHV2ENGINE::Dummy3( VOID *pThis, int a )
{
	//TRACE( "IXHV2Engine::Dummy3  (a = %X)", a );

	// something about a == 1, a== 2


	return ERROR_SUCCESS;
}



HRESULT IXHV2ENGINE::StartLocalProcessingModes( VOID *pThis, DWORD dwUserIndex, /* CONST PXHV_PROCESSING_MODE*/ VOID *processingModes, DWORD dwNumProcessingModes )
{
//	TRACE( "IXHV2Engine::StartLocalProcessingModes  (dwUserIndex = %X, processingModes = %X, dwNumProcessingModes = %d)",
//		dwUserIndex, processingModes, dwNumProcessingModes );


//	TRACE( "- Voice chat on" );
	return S_OK;
}



HRESULT IXHV2ENGINE::StopLocalProcessingModes( VOID *pThis, DWORD dwUserIndex, /*CONST PXHV_PROCESSING_MODE*/ VOID *processingModes, DWORD dwNumProcessingModes )
{
//	TRACE( "IXHV2Engine::StopLocalProcessingModes  (dwUserIndex = %X, processingModes = %X, dwNumProcessingModes = %X)",
//		dwUserIndex, processingModes, dwNumProcessingModes );


//	TRACE( "- Stopping voice" );


	return S_OK;
}



HRESULT IXHV2ENGINE::StartRemoteProcessingModes( VOID *pThis, int a1, int a2, int a3, int a4 )
{
	TRACE( "IXHV2Engine::StartRemoteProcessingModes  (a1 = %X, a2 = %X, a3 = %X, a4 = %X)",
		a1, a2, a3, a4 );


	TRACE( "- Voice chat on" );
	return S_OK;
}



HRESULT IXHV2ENGINE::Dummy7( VOID *pThis, int a1, int a2, int a3, int a4 )
{
	TRACE( "IXHV2Engine::Dummy7  (a1 = %X, a2 = %X, a3 = %X, a4 = %X)",
		a1, a2, a3, a4 );


	return ERROR_SUCCESS;
}



HRESULT IXHV2ENGINE::Dummy8( VOID *pThis, int a1 )
{
	TRACE( "IXHV2Engine::Dummy8  (a1 = %X)", a1 );


	return ERROR_SUCCESS;
}



HRESULT IXHV2ENGINE::RegisterLocalTalker( VOID *pThis, DWORD dwUserIndex )
{
	//TRACE( "IXHV2Engine::RegisterLocalTalker  (dwUserIndex = %d)",
	///	dwUserIndex );


	//TRACE( "- user added" );
	return S_OK;
}



HRESULT IXHV2ENGINE::UnregisterLocalTalker( VOID *pThis, DWORD dwUserIndex )
{
	TRACE( "IXHV2Engine::UnregisterLocalTalker  (dwUserIndex = %d)",
		dwUserIndex );


	return ERROR_SUCCESS;
}



HRESULT IXHV2ENGINE::Dummy11( VOID *pThis, int a1, int a2, int a3, int a4, int a5 )
{
	TRACE( "IXHV2Engine::Dummy11  (a1 = %X, a2 = %X, a3 = %X, a4 = %X, a5 = %X)",
		a1, a2, a3, a4, a5 );


	return ERROR_SUCCESS;
}



HRESULT IXHV2ENGINE::UnregisterRemoteTalker( VOID *pThis, int a1, int a2 )
{
	TRACE( "IXHV2Engine::UnregisterRemoteTalker  (a1 = %X, a2 = %X)",
		a1, a2 );


	TRACE( "- Voice stopped" );


	return S_OK;
}


HRESULT IXHV2ENGINE::Dummy13( VOID *pThis, int a1, int a2 )
{
	static int print = 0;


	if( print < 15 )
	{
		TRACE( "IXHV2Engine::Dummy13  (a1 = %X, a2 = %X)",
			a1, a2 );


		print++;
	}



#if 0
	while(1)
		Sleep(1);
#endif


	// no data in??
	return -1;


	//return ERROR_SUCCESS;
}



INT IXHV2ENGINE::Dummy14( VOID *pThis, int a1 )
{
	static int print = 0;


	if( print < 15 )
	{
	//	TRACE( "IXHV2Engine::Dummy14  (a1 = %X)",
	//		a1 );


		print++;
	}


	return 0;
}



INT IXHV2ENGINE::Dummy15( VOID *pThis, int a1 )
{
	static int print = 0;


	if( print < 15 )
	{
		//TRACE( "IXHV2Engine::Dummy15  (a1 = %X)",
		//	a1 );
		
		print++;
	}


	return 0;
}



HRESULT IXHV2ENGINE::Dummy16( VOID *pThis, int a1, int a2 )
{
	TRACE( "IXHV2Engine::Dummy16  (a1 = %X, a2 = %X)",
		a1, a2 );


	return ERROR_SUCCESS;
}



DWORD IXHV2ENGINE::GetDataReadyFlags( VOID *pThis )
{
	static int print = 0;


	if( print < 15 )
	{
		TRACE( "IXHV2Engine::GetDataReadyFlags" );

		print++;
	}


#if 0
	while(1)
		Sleep(1);
#endif


	if( print < 15 )
	{
		TRACE( "- No user ready" );
	}


	// 0x1 = user 0
	// 0xF = user 0-3
	// 0xFF = user 0-7
	return 0;
}



HRESULT IXHV2ENGINE::GetLocalChatData( VOID *pThis, DWORD dwUserIndex, PBYTE pbData, PDWORD pdwSize, PDWORD pdwPackets )
{
	static int print = 0;


	if( print < 15 )
	{
		//TRACE( "IXHV2Engine::GetLocalChatData  (dwUserIndex = %X, pbData = %X, pdwSize = %X, pdwPackets = %X)",
		//	dwUserIndex, pbData, pdwSize, pdwPackets );

		print++;
	}


	if( pdwSize ) *pdwSize = 0;
	if( pdwPackets ) *pdwPackets = 0;


	if( print < 15 )
	{
		//TRACE( "- No local chat data" );
	}

	//return E_PENDING;


	return ERROR_SUCCESS;
}



HRESULT IXHV2ENGINE::SetPlaybackPriority( VOID *pThis, int a1, int a2, int a3, int a4 )
{
	TRACE( "IXHV2Engine::SetPlaybackPriority  (a1 = %X, a2 = %X, a3 = %X, a4 = %X)",
		a1, a2, a3, a4 );


	return ERROR_SUCCESS;
}



HRESULT IXHV2ENGINE::Dummy20( VOID *pThis, int a1, int a2, int a3, int a4 )
{
	TRACE( "IXHV2Engine::Dummy20  (a1 = %X, a2 = %X, a3 = %X, a4 = %X)",
		a1, a2, a3, a4 );


	return ERROR_SUCCESS;
}



HRESULT IXHV2ENGINE::Dummy21( VOID *pThis )
{
	TRACE( "IXHV2Engine::Dummy21" );


	return ERROR_SUCCESS;
}



HRESULT IXHV2ENGINE::Dummy22( VOID *pThis )
{
	TRACE( "IXHV2Engine::Dummy22" );


	return ERROR_SUCCESS;
}



HRESULT IXHV2ENGINE::Dummy23( VOID *pThis )
{
	TRACE( "IXHV2Engine::Dummy23" );


	return ERROR_SUCCESS;
}



HRESULT IXHV2ENGINE::Dummy24( VOID *pThis )
{
	TRACE( "IXHV2Engine::Dummy24" );


	return ERROR_SUCCESS;
}



HRESULT IXHV2ENGINE::Dummy25( VOID *pThis )
{
	TRACE( "IXHV2Engine::Dummy25" );


	return ERROR_SUCCESS;
}



HRESULT IXHV2ENGINE::Dummy26( VOID *pThis )
{
	TRACE( "IXHV2Engine::Dummy26" );


	return ERROR_SUCCESS;
}



HRESULT IXHV2ENGINE::Dummy27( VOID *pThis )
{
	TRACE( "IXHV2Engine::Dummy27" );


	return ERROR_SUCCESS;
}



HRESULT IXHV2ENGINE::Dummy28( VOID *pThis )
{
	TRACE( "IXHV2Engine::Dummy28" );


	return ERROR_SUCCESS;
}



HRESULT IXHV2ENGINE::Dummy29( VOID *pThis )
{
	TRACE( "IXHV2Engine::Dummy29" );


	return ERROR_SUCCESS;
}



HRESULT IXHV2ENGINE::Dummy30( VOID *pThis )
{
	TRACE( "IXHV2Engine::Dummy30" );


	return ERROR_SUCCESS;
}



HRESULT IXHV2ENGINE::Dummy31( VOID *pThis )
{
	TRACE( "IXHV2Engine::Dummy31" );


	return ERROR_SUCCESS;
}



HRESULT IXHV2ENGINE::Dummy32( VOID *pThis )
{
	TRACE( "IXHV2Engine::Dummy32" );


	return ERROR_SUCCESS;
}



IXHV2ENGINE::IXHV2ENGINE()
{
	funcTablePtr = &(funcPtr[0]);


	funcPtr[0] = (HV2FUNCPTR) &IXHV2ENGINE::Dummy1;
	funcPtr[1] = (HV2FUNCPTR) &IXHV2ENGINE::Dummy2;
	funcPtr[2] = (HV2FUNCPTR) &IXHV2ENGINE::Dummy3;
	funcPtr[3] = (HV2FUNCPTR) &IXHV2ENGINE::StartLocalProcessingModes;
	funcPtr[4] = (HV2FUNCPTR) &IXHV2ENGINE::StopLocalProcessingModes;
	funcPtr[5] = (HV2FUNCPTR) &IXHV2ENGINE::StartRemoteProcessingModes;
	funcPtr[6] = (HV2FUNCPTR) &IXHV2ENGINE::Dummy7;
	funcPtr[7] = (HV2FUNCPTR) &IXHV2ENGINE::Dummy8;

	funcPtr[8] = (HV2FUNCPTR) &IXHV2ENGINE::RegisterLocalTalker;
	funcPtr[9] = (HV2FUNCPTR) &IXHV2ENGINE::UnregisterLocalTalker;
	funcPtr[10] = (HV2FUNCPTR) &IXHV2ENGINE::Dummy11;
	funcPtr[11] = (HV2FUNCPTR) &IXHV2ENGINE::UnregisterRemoteTalker;

	funcPtr[12] = (HV2FUNCPTR) &IXHV2ENGINE::Dummy13;
	funcPtr[13] = (HV2FUNCPTR) &IXHV2ENGINE::Dummy14;
	funcPtr[14] = (HV2FUNCPTR) &IXHV2ENGINE::Dummy15;
	funcPtr[15] = (HV2FUNCPTR) &IXHV2ENGINE::Dummy16;

	funcPtr[16] = (HV2FUNCPTR) &IXHV2ENGINE::GetDataReadyFlags;
	funcPtr[17] = (HV2FUNCPTR) &IXHV2ENGINE::GetLocalChatData;
	funcPtr[18] = (HV2FUNCPTR) &IXHV2ENGINE::SetPlaybackPriority;
	funcPtr[19] = (HV2FUNCPTR) &IXHV2ENGINE::Dummy20;

	funcPtr[20] = (HV2FUNCPTR) &IXHV2ENGINE::Dummy21;
	funcPtr[21] = (HV2FUNCPTR) &IXHV2ENGINE::Dummy22;
	funcPtr[22] = (HV2FUNCPTR) &IXHV2ENGINE::Dummy23;
	funcPtr[23] = (HV2FUNCPTR) &IXHV2ENGINE::Dummy24;

	funcPtr[24] = (HV2FUNCPTR) &IXHV2ENGINE::Dummy25;
	funcPtr[25] = (HV2FUNCPTR) &IXHV2ENGINE::Dummy26;
	funcPtr[26] = (HV2FUNCPTR) &IXHV2ENGINE::Dummy27;
	funcPtr[27] = (HV2FUNCPTR) &IXHV2ENGINE::Dummy28;

	funcPtr[28] = (HV2FUNCPTR) &IXHV2ENGINE::Dummy29;
	funcPtr[29] = (HV2FUNCPTR) &IXHV2ENGINE::Dummy30;
	funcPtr[30] = (HV2FUNCPTR) &IXHV2ENGINE::Dummy31;
	funcPtr[31] = (HV2FUNCPTR) &IXHV2ENGINE::Dummy32;
}
