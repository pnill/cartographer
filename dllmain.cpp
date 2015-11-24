// -No Copyright- 2010 Stanislav "listener" Golovin
// This file donated to the public domain

// virusek public domain

#include "stdafx.h"
#include <io.h>
#include <fcntl.h>
#include "H2MOD.h"
#include <iostream>
using namespace std;


//#include "Ini.h"
#include "Detour.h"


HMODULE hThis = NULL;

CRITICAL_SECTION d_lock;

BOOL consoleEnabled = FALSE;
UINT g_online = 0;
UINT g_debug = 0;
UINT g_server = 0;
ULONG broadcast_server = inet_addr("167.114.128.187");
BOOL isHost = FALSE;

UINT g_signin[4] = { 1,0,0,0 };
CHAR g_szUserName[4][16+1] = { "xLiveLess", "xLiveLess", "xLiveLess", "xLiveLess" };
XUID xFakeXuid[4] = { 0xEE000000DEADC0DE, 0xEE000000DEADC0DE, 0xEE000000DEADC0DE, 0xEE000000DEADC0DE };
CHAR g_profileDirectory[512] = "Profiles";


std::wstring dlcbasepath;



VOID Console()
{
#ifdef NDEBUG
	return;
#endif


	AllocConsole();

	HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);
	int hCrt = _open_osfhandle((long) handle_out, _O_TEXT);
	FILE* hf_out = _wfdopen(hCrt, L"w");
	setvbuf(hf_out, NULL, _IONBF, 1);
	*stdout = *hf_out;
	consoleEnabled = TRUE;
}

std::string ModulePathA(HMODULE hModule = NULL)
{
	static char strPath[MAX_PATH];
	GetModuleFileNameA(hModule, strPath, MAX_PATH);
	//PathRemoveFileSpecA(strPath);
	//PathAddBackslashA(strPath);
	return strPath;
}

std::wstring ModulePathW(HMODULE hModule = NULL)
{
	static wchar_t strPath[MAX_PATH];
	GetModuleFileNameW(hModule, strPath, MAX_PATH);
	//PathRemoveFileSpecW(strPath);
	//PathAddBackslashW(strPath);
	return strPath;
}

#ifndef NO_TRACE
FILE * logfile = NULL;
FILE * loggame = NULL;
FILE * loggamen = NULL;

void trace(LPWSTR message, ...)
{
	if (!logfile)
		return;

	if(!consoleEnabled) Console();

	EnterCriticalSection (&d_lock);
	SYSTEMTIME	t;
	GetLocalTime (&t);

	fwprintf (logfile, L"%02d/%02d/%04d %02d:%02d:%02d.%03d ", t.wDay, t.wMonth, t.wYear, t.wHour, t.wMinute, t.wSecond, t.wMilliseconds);
	wprintf (L"%02d/%02d/%04d %02d:%02d:%02d.%03d ", t.wDay, t.wMonth, t.wYear, t.wHour, t.wMinute, t.wSecond, t.wMilliseconds);

	va_list	arg;
	va_start (arg, message);

	vwprintf (message, arg);
	wprintf(L"\n");

	vfwprintf (logfile, message, arg);
	fwprintf(logfile, L"\n");

	fflush (stdin);
	fflush (logfile);
	va_end (arg);
	LeaveCriticalSection (&d_lock);
}


void trace2(LPWSTR message, ...)
{
	if (!logfile)
		return;

	if(!consoleEnabled) Console();

	EnterCriticalSection (&d_lock);
	SYSTEMTIME	t;
	GetLocalTime (&t);

	fwprintf (logfile, L"%02d/%02d/%04d %02d:%02d:%02d.%03d ", t.wDay, t.wMonth, t.wYear, t.wHour, t.wMinute, t.wSecond, t.wMilliseconds);
	wprintf (L"%02d/%02d/%04d %02d:%02d:%02d.%03d ", t.wDay, t.wMonth, t.wYear, t.wHour, t.wMinute, t.wSecond, t.wMilliseconds);

	va_list	arg;
	va_start (arg, message);

	vwprintf (message, arg);
	wprintf(L"\n");

	vfwprintf (logfile, message, arg);
	fwprintf(logfile, L"\n");

	fflush (stdin);
	fflush (logfile);
	va_end (arg);
	LeaveCriticalSection (&d_lock);
}

void trace_game(LPWSTR message, ...)
{
	if (!loggame)
		return;

	if (!consoleEnabled) Console();

	EnterCriticalSection(&d_lock);
	SYSTEMTIME	t;
	GetLocalTime(&t);

	fwprintf(loggame, L"%02d/%02d/%04d %02d:%02d:%02d.%03d ", t.wDay, t.wMonth, t.wYear, t.wHour, t.wMinute, t.wSecond, t.wMilliseconds);

	va_list	arg;
	va_start(arg, message);

	//vwprintf(message, arg);
	//wprintf(L"\n");

	vfwprintf(loggame, message, arg);
	fwprintf(loggame, L"\n");

	fflush(stdin);
	fflush(loggame);
	va_end(arg);
	LeaveCriticalSection(&d_lock);
}

void trace_game_network(LPSTR message, ...)
{
	if (!loggamen)
		return;

	if (!consoleEnabled) Console();

	EnterCriticalSection(&d_lock);
	SYSTEMTIME	t;
	GetLocalTime(&t);

	fprintf(loggamen, "%02d/%02d/%04d %02d:%02d:%02d.%03d ", t.wDay, t.wMonth, t.wYear, t.wHour, t.wMinute, t.wSecond, t.wMilliseconds);
	printf("%02d/%02d/%04d %02d:%02d:%02d.%03d ", t.wDay, t.wMonth, t.wYear, t.wHour, t.wMinute, t.wSecond, t.wMilliseconds);

	va_list	arg;
	va_start(arg, message);

	vprintf(message, arg);
	printf("\n");

	vfprintf(loggamen, message, arg);
	fprintf(loggamen, "\n");

	fflush(stdin);
	fflush(loggamen);
	va_end(arg);
	LeaveCriticalSection(&d_lock);
}

void trace_game_narrow(LPSTR message, ...)
{
	if (!loggame)
		return;

	if (!consoleEnabled) Console();

	EnterCriticalSection(&d_lock);
	SYSTEMTIME	t;
	GetLocalTime(&t);

	fprintf(loggame, "%02d/%02d/%04d %02d:%02d:%02d.%03d ", t.wDay, t.wMonth, t.wYear, t.wHour, t.wMinute, t.wSecond, t.wMilliseconds);

	va_list	arg;
	va_start(arg, message);

	//vprintf(message, arg);
	//printf("\n");

	vfprintf(loggame, message, arg);
	fprintf(loggame, "\n");

	fflush(stdin);
	fflush(loggame);
	va_end(arg);
	LeaveCriticalSection(&d_lock);
}
#endif

void InitInstance()
{
	static bool init = true;

	if(init)
	{
		init = false;

#ifdef _DEBUG
		int CurrentFlags;
		CurrentFlags = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
		CurrentFlags |= _CRTDBG_DELAY_FREE_MEM_DF;
		CurrentFlags |= _CRTDBG_LEAK_CHECK_DF;
		CurrentFlags |= _CRTDBG_CHECK_ALWAYS_DF;
		_CrtSetDbgFlag(CurrentFlags);
#endif
		InitializeCriticalSection (&d_lock);

#if 0
		patchCode ();
		loadPlugins ("*.asi");
		loadPlugins ("plugins\\*.asi");
		loadPlugins ("plugins\\*.dll");
#endif

		//dlcbasepath = ModulePathW() + L"DLC";
		dlcbasepath = L"DLC";


		strcpy( g_szUserName[0], "Player1" );
		strcpy( g_szUserName[1], "Player2" );
		strcpy( g_szUserName[2], "Player3" );
		strcpy( g_szUserName[3], "Player4" );




		FILE *fp;
		fp = fopen( "xlive.ini", "r" );

		if( !fp )
			fp = fopen( "c:\\xlive.ini", "r" );



		if( fp )
		{
			while( !feof(fp) )
			{
				char str[256];


				fgets( str, 256, fp );



#define CHECK_ARG_STR(x,y) \
	if( strstr( str,x ) == str ) \
	{ \
		sscanf( str + strlen(x), "%s", &y ); \
		continue; \
	}


#define CHECK_ARG(x,y) \
	if( strstr( str,x ) == str ) \
	{ \
		sscanf( str + strlen(x), "%d", &y ); \
		continue; \
	}


#define CHECK_ARG_I64(x,y) \
	if( strstr( str,x ) == str ) \
	{ \
		sscanf( str + strlen(x), "%I64x", &y ); \
		continue; \
	}



				CHECK_ARG( "signin player 1 =", g_signin[0] );
				CHECK_ARG( "signin player 2 =", g_signin[1] );
				CHECK_ARG( "signin player 3 =", g_signin[2] );
				CHECK_ARG( "signin player 4 =", g_signin[3] );

				CHECK_ARG_STR( "profile name 1 =", g_szUserName[0] );
				CHECK_ARG_STR( "profile name 2 =", g_szUserName[1] );
				CHECK_ARG_STR( "profile name 3 =", g_szUserName[2] );
				CHECK_ARG_STR( "profile name 4 =", g_szUserName[3] );

				CHECK_ARG_I64( "profile xuid 1 =", xFakeXuid[0] );
				CHECK_ARG_I64( "profile xuid 2 =", xFakeXuid[1] );
				CHECK_ARG_I64( "profile xuid 3 =", xFakeXuid[2] );
				CHECK_ARG_I64( "profile xuid 4 =", xFakeXuid[3] );

				CHECK_ARG( "online profile =", g_online );
				CHECK_ARG_STR( "save directory =", g_profileDirectory );
				CHECK_ARG( "debug log =", g_debug );
				CHECK_ARG( "server =" ,g_server)
			}

			
			fclose(fp);
		}


#ifndef NO_TRACE
		if( g_debug )
		{
			if ( logfile = _wfopen(L"xlive_trace.log", L"wt") )
				TRACE("Log started (xLiveLess 2.0a4)\n");

			if ( loggame = _wfopen(L"h2mod.log", L"wt") )
				 TRACE_GAME("Log started (H2MOD 0.1a1)\n");

			if (loggamen = _wfopen(L"h2network.log", L"wt"))
				TRACE_GAME_NETWORK("Log started (H2MOD - Network 0.1a1)\n");

			if (h2mod)
				h2mod->Initialize();
			else
				TRACE("H2MOD Failed to intialize");

			TRACE("g_server param = %i", g_server);
			WCHAR gameName[256];
			
			GetModuleFileNameW( NULL, (LPWCH) &gameName, sizeof(gameName) );
			TRACE( "%s", gameName );

		}
#endif

		
/*
		Ini ini;
		ini.open(ModulePathA() + "xlive.ini");

		if(ini.is_open())
		{
			std::string name = ini.GetString("XLive","UserName");
			if(!name.empty()) strncpy_s(g_szUserName,16,name.c_str(),15);

			std::string xuidstr = ini.GetString("XLive","XUID");
			XUID xuid =  _strtoui64(xuidstr.c_str(),0,16);
			if(xuid) xFakeXuid = xuid;
		}
		*/



		extern void LoadAchievements();
		LoadAchievements();
	}
}

void ExitInstance()
{
	EnterCriticalSection (&d_lock);

#ifndef NO_TRACE
	TRACE( "Shutting down" );


	if (logfile)
	{
		fflush (logfile);
		fclose (logfile);
		fflush (loggame);
		fclose (loggame);
		logfile = NULL;
		loggame = NULL;
	}
#endif


	LeaveCriticalSection (&d_lock);
	DeleteCriticalSection (&d_lock);



	extern void SaveAchievements();
	SaveAchievements();
}

//=============================================================================
// Entry Point
BOOL APIENTRY DllMain( HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		hThis = hModule;
		

		Detour();
		break;


	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;


	case DLL_PROCESS_DETACH:
		ExitInstance();
		break;
	}
	return TRUE;
}

#if 0
enum GameVersion {
    GvUnknown       = 0x00000000,   // unknown game
    IvPatch1        = 0x00010001,   // GTA IV 1.0.1.0
    IvPatch2        = 0x00010002,   // GTA IV 1.0.2.0
    IvPatch3        = 0x00010003,   // GTA IV 1.0.3.0
    IvPatch4        = 0x00010004,   // GTA IV 1.0.4.0
    IvPatch5        = 0x00010005,   // GTA IV 1.0.0.4
    IvPatch6        = 0x00010006,   // GTA IV 1.0.6.0
    IvPatch6J       = 0x00010006,   // GTA IV 1.0.4.2
    IvPatch7        = 0x00010007,   // GTA IV 1.0.7.0
    RfgUnpatched    = 0x00020000,   // Red Faction: Guerilla
    EflcPatch1      = 0x00030001,   // EfLC 1.1.1.0
    EflcPatch2      = 0x00030001,   // EfLC 1.1.2.0
		
		
		EflcPatch1R			= 0x00030001,
		DR2Unpatched    = 0x00040000
};

// for C/C++ plugins
XLIVELESS_API GameVersion   dwGameVersion;  // game 
XLIVELESS_API DWORD         dwLoadOffset;   // Offset to the "real" loding address 

// for Delphi plugins (same as two variables above)
XLIVELESS_API GameVersion   getGameVersion();
XLIVELESS_API DWORD         getLoadOffset ();


//=============================================================================
// asi loader and patching stuff
GameVersion dwGameVersion = GvUnknown;
DWORD	dwLoadOffset = 0x400000;

// Delphi don't support importing variables from DLL
GameVersion getGameVersion ()
{
	return dwGameVersion;
}
DWORD getLoadOffset ()
{
	return dwLoadOffset;
}

void injectFunction (DWORD dwAddress, DWORD pfnReplacement)
{
	dwAddress += dwLoadOffset;
	BYTE * patch = (BYTE *)dwAddress;
	*patch = 0xE9;	// JMP
	*(DWORD *)(patch+1) = (pfnReplacement-(dwAddress+5));
}

static char * pszPath = "";

// change savefile path to "%USERPROFILE%\Documents\Rockstar Games\GTA IV\savegames\"
void getSavefilePath (int __unused, char * pBuffer, char * pszSaveName)
{
	return;
	/*
	strcpy_s (pBuffer, 256, pszPath);
	strcat_s (pBuffer, 256, "savegames");

	// check path and create directory if necessary
	DWORD attrs = GetFileAttributes (pBuffer);
	if (attrs == INVALID_FILE_ATTRIBUTES)
		CreateDirectory (pBuffer, NULL);
	else if (!(attrs & FILE_ATTRIBUTE_DIRECTORY))
	{
		//TRACE("ERROR: unable to create directory '%s', file '%s' already exists\n", pBuffer);
		strcpy_s (pBuffer, 256, pszSaveName);
		return;
	}

	if (pszSaveName)
	{
		strcat_s (pBuffer, 256, "\\");
		strcat_s (pBuffer, 256, pszSaveName);
	}
	//TRACE("[getSavefilePath]: '%s'\n", pBuffer);
	*/
}

// === miscellaneous patches ===
void patch101 ()
{
	dwGameVersion = IvPatch1;	// GTA IV 1.0.1 (patch 1)

	// process patches
	*(WORD *)(0x608C35+dwLoadOffset) = 0x9090; // NOP; NOP	- save file CRC32 check
	*(BYTE *)(0x7A14B0+dwLoadOffset) = 0xC3;	// RETN - enable debugger in error menu (don't load WER.dll)
	*(BYTE *)(0x470DB0+dwLoadOffset) = 0xC3;	// RETN - skip files.txt hash check
	*(WORD *)(0x4702FF+dwLoadOffset) = 0x9090;	// NOP; NOP - another files.txt hash check
	*(BYTE *)(0x7A2530+dwLoadOffset) = 0xC3;	// RETN - skip files.txt hash check

	// disable RGSC (no more this stupid window!)
	*(BYTE *)(0x46FB80+dwLoadOffset) = 0xC3;	// RETN - remove connect to the RGSC
	*(WORD *)(0x47030A+dwLoadOffset) = 0x9090;	// NOP; NOP - RGSC initialization check
	*(WORD *)(0x47030F+dwLoadOffset) = 0x9090;	// NOP; NOP - RGSC initialization check
	*(BYTE *)(0x470317+dwLoadOffset) = 0x1D;	// mov dword_15B47DC, esi => mov ..., ebx - another RGSC related object

	// replace getSavefilePath
	injectFunction (0x608660, (DWORD)getSavefilePath);
	pszPath = (char *)(0xFA7878+dwLoadOffset);	// char pszPathPersonal[128]; // "%USERPROFILE%\Documents\Rockstar Games\GTA IV\"

	//TRACE("Patching OK (1.0.1)\n");
}

void patch102 ()
{
	dwGameVersion = IvPatch2;	// GTA IV 1.0.2 (patch 2)

	// process patches
	*(WORD *)(0x6086B5+dwLoadOffset) = 0x9090; 	// NOP; NOP - save file CRC32 check
	*(BYTE *)(0x79F710+dwLoadOffset) = 0xC3;	// RETN - enable debugger in error menu (don't load WER.dll)
	*(BYTE *)(0x7A0740+dwLoadOffset) = 0xC3;	// RETN - skip files.txt hash check

	*(BYTE *)(0x46FA7D+dwLoadOffset) = 0xE9;	// JMP
	*(DWORD *)(0x46FA7E+dwLoadOffset)= 0x16;	// jmp target
	*(BYTE *)(0x4701C0+dwLoadOffset) = 0xC3;	// RETN - certificates check
	*(BYTE *)(0x46F120+dwLoadOffset) = 0xC2;	// RETN - remove connect to the RGSC
	*(BYTE *)(0x46F121+dwLoadOffset) = 0x04;	// RETN - remove connect to the RGSC
	*(BYTE *)(0x46F122+dwLoadOffset) = 0x00;	// RETN - remove connect to the RGSC
	memset ((BYTE *)(0x46FB16+dwLoadOffset), 0x90, 0x1B);
	*(WORD *)(0x46FAAA+dwLoadOffset) = 0x9090;	// NOP; NOP - RGSC initialization check
	*(WORD *)(0x46FB36+dwLoadOffset) = 0x9090;	// NOP; NOP - last RGSC init check
	*(DWORD *)(0x46FB38+dwLoadOffset) = 0x90909090;
	*(WORD *)(0x46FAB1+dwLoadOffset) = 0xC033;	// XOR eax, eax - RGSC initialization check
	*(WORD *)(0x46FAB8+dwLoadOffset) = 0xA390;	// NOP; MOV [], eax

	*(DWORD *)(0x401825+dwLoadOffset) = 1;		// disable sleep

	// replace getSavefilePath
	injectFunction (0x6080E0, (DWORD)getSavefilePath);
	pszPath = (char *)(0xFA0308+dwLoadOffset);	// char pszPathPersonal[128]; // "%USERPROFILE%\Documents\Rockstar Games\GTA IV\"

	//TRACE("Patching OK (1.0.2)\n");
}

void patch103 ()
{
	dwGameVersion = IvPatch3;	// GTA IV 1.0.3 (patch 3)

	// process patches
	*(WORD *)(0x60C095+dwLoadOffset) = 0x9090; 	// NOP; NOP - save file CRC32 check
	*(BYTE *)(0x7A3A70+dwLoadOffset) = 0xC3;	// RETN - enable debugger in error menu (don't load WER.dll)
	*(BYTE *)(0x7A4750+dwLoadOffset) = 0xC3;	// RETN - skip files.txt hash check

	*(BYTE  *)(0x471CBD+dwLoadOffset) = 0xE9;	// JMP
	*(DWORD *)(0x471CBE+dwLoadOffset) = 0x16;	// jmp target
	//	*(BYTE  *)(0x472E50+dwLoadOffset) = 0xC3;	// RETN - certificates check
	*(BYTE  *)(0x472E50+dwLoadOffset) = 0xC2;	// RETN - certificates check
	*(BYTE  *)(0x472E51+dwLoadOffset) = 0x08;	// RETN - certificates check
	*(BYTE  *)(0x472E52+dwLoadOffset) = 0x00;	// RETN - certificates check
	*(BYTE  *)(0x471360+dwLoadOffset) = 0xC2;	// RETN - remove connect to the RGSC
	*(BYTE  *)(0x471361+dwLoadOffset) = 0x04;	// RETN - remove connect to the RGSC
	*(BYTE  *)(0x471362+dwLoadOffset) = 0x00;	// RETN - remove connect to the RGSC
	memset ((BYTE *)(0x471D60+dwLoadOffset), 0x90, 0x1B);
	*(WORD  *)(0x471CEA+dwLoadOffset) = 0x9090;	// NOP; NOP - RGSC initialization check
	*(DWORD *)(0x471D80+dwLoadOffset) = 0x90909090;	// NOP*4- last RGSC init check
	*(WORD  *)(0x471D84+dwLoadOffset) = 0x9090;	// NOP*4- last RGSC init check
	*(WORD *)(0x471CF1+dwLoadOffset) = 0xC033;	// XOR eax, eax - RGSC initialization check
	*(WORD *)(0x471CF8+dwLoadOffset) = 0xA390;	// NOP; MOV [], eax

	*(DWORD *)(0x401815+dwLoadOffset) = 1;		// disable sleep

	injectFunction (0x60BAC0, (DWORD)getSavefilePath); // replace getSavefilePath
	pszPath = (char *)(0xFBF860+dwLoadOffset);	// char pszPathPersonal[128]; // "%USERPROFILE%\Documents\Rockstar Games\GTA IV\"

	memset ((BYTE *)(0x451E19+dwLoadOffset), 0x90, 0x1BF);	// EFC20
	*(DWORD *)(0xA0D1E0+dwLoadOffset) = 0x90C301B0;	// mov al, 1; retn
	*(DWORD *)(0xA0D200+dwLoadOffset) = 0x90C301B0;
	*(DWORD *)(0xA0D210+dwLoadOffset) = 0x90C301B0;
	*(DWORD *)(0xA0D240+dwLoadOffset) = 0x90C301B0;

	//TRACE("Patching OK (1.0.3)\n");
}

void patch104 ()
{
	dwGameVersion = IvPatch4;	// GTA IV 1.0.4 (patch 4)

	// process patches
	*(WORD *)(0x60C1A5+dwLoadOffset) = 0x9090; 	// NOP; NOP - save file CRC32 check
	*(BYTE *)(0x7A4900+dwLoadOffset) = 0xC3;	// RETN - enable debugger in error menu (don't load WER.dll)
	*(BYTE *)(0x7A4A10+dwLoadOffset) = 0xC3;	// RETN - skip files.txt hash check

	*(BYTE  *)(0x471E5D+dwLoadOffset) = 0xE9;	// JMP
	*(DWORD *)(0x471E5E+dwLoadOffset) = 0x16;	// jmp target
	//	*(BYTE  *)(0x472FF0+dwLoadOffset) = 0xC3;	// RETN - certificates check
	*(BYTE  *)(0x472FF0+dwLoadOffset) = 0xC2;	// RETN - certificates check
	*(BYTE  *)(0x472FF1+dwLoadOffset) = 0x08;	// RETN - certificates check
	*(BYTE  *)(0x472FF2+dwLoadOffset) = 0x00;	// RETN - certificates check

	*(BYTE  *)(0x471500+dwLoadOffset) = 0xC2;	// RETN - remove connect to the RGSC
	*(BYTE  *)(0x471501+dwLoadOffset) = 0x04;	// RETN - remove connect to the RGSC
	*(BYTE  *)(0x471502+dwLoadOffset) = 0x00;	// RETN - remove connect to the RGSC
	memset ((BYTE *)(0x471F00+dwLoadOffset), 0x90, 0x1B);
	*(WORD  *)(0x471E8A+dwLoadOffset) = 0x9090;	// NOP; NOP - RGSC initialization check
	*(DWORD *)(0x471F20+dwLoadOffset) = 0x90909090;	// NOP*4- last RGSC init check
	*(WORD  *)(0x471F24+dwLoadOffset) = 0x9090;	// NOP*4- last RGSC init check
	*(WORD *)(0x471E91+dwLoadOffset) = 0xC033;	// XOR eax, eax - RGSC initialization check
	*(WORD *)(0x471E98+dwLoadOffset) = 0xA390;	// NOP; MOV [], eax

	*(DWORD *)(0x4017F5+dwLoadOffset) = 1;		// disable sleep

	injectFunction (0x60BBD0, (DWORD)getSavefilePath); // replace getSavefilePath
	pszPath = (char *)(0xFC4B00+dwLoadOffset);	// char pszPathPersonal[128]; // "%USERPROFILE%\Documents\Rockstar Games\GTA IV\"

	memset ((BYTE *)(0x452129+dwLoadOffset), 0x90, 0x1BF);	// EFC20

	*(DWORD *)(0xA0D9C0+dwLoadOffset) = 0x90C301B0;	// mov al, 1; retn
	*(DWORD *)(0xA0D9E0+dwLoadOffset) = 0x90C301B0;
	*(DWORD *)(0xA0D9F0+dwLoadOffset) = 0x90C301B0;
	*(DWORD *)(0xA0DA20+dwLoadOffset) = 0x90C301B0;

	//TRACE("Patching OK (1.0.4)\n");
}

void patch105 ()
{
	dwGameVersion = IvPatch5;	// GTA IV 1.0.0.4 (patch 5)

	// process patches
	*(BYTE *)(0x7B82A0+dwLoadOffset) = 0xC3;	// RETN - enable debugger in error menu (don't load WER.dll)
	*(DWORD *)(0x401847+dwLoadOffset) = 1;		// disable sleep
	*(DWORD *)(0x527D90+dwLoadOffset) = 0x900008C2;	// RETN 8 - certificates check
	*(DWORD *)(0x5269B9+dwLoadOffset) = 0x06E9C033;	// xor eax, eax - address of the RGSC object
	*(DWORD *)(0x5269BD+dwLoadOffset) = 0x90000002;	// jmp 526BC6 (skip RGSC connect and EFC checks)
	*(WORD *)(0x526BCB+dwLoadOffset) = 0xA390;	// NOP; MOV [g_rgsc], eax

	memset ((BYTE *)(0x526C30+dwLoadOffset), 0x90, 0x29);
	*(DWORD *)(0x526C5F+dwLoadOffset) = 0x90909090;	// NOP*4- last RGSC init check
	*(WORD  *)(0x526C63+dwLoadOffset) = 0x9090;	// NOP*2- last RGSC init check

	// skip missing tests...
	*(WORD *)(0x474D0B+dwLoadOffset) = 0xC033;
	*(DWORD *)(0x474D0D+dwLoadOffset) = 0x90909090;
	memset ((BYTE *)(0x481225+dwLoadOffset), 0x90, 3);
	memset ((BYTE *)(0x48122B+dwLoadOffset), 0x90, 11);
	memset ((BYTE *)(0x4812F5+dwLoadOffset), 0x90, 3);
	memset ((BYTE *)(0x4812FB+dwLoadOffset), 0x90, 11);
	memset ((BYTE *)(0x481375+dwLoadOffset), 0x90, 11);
	memset ((BYTE *)(0x4813EF+dwLoadOffset), 0x90, 32);
	memset ((BYTE *)(0x4815E3+dwLoadOffset), 0x90, 25);
	memset ((BYTE *)(0x483BF5+dwLoadOffset), 0x90, 8);
	memset ((BYTE *)(0x483EA2+dwLoadOffset), 0x90, 8);
	memset ((BYTE *)(0x526E4F+dwLoadOffset), 0x90, 14);
	memset ((BYTE *)(0x526F77+dwLoadOffset), 0x90, 16);
	*(BYTE *)(0x527930+dwLoadOffset) = 0xC3;
	memset ((BYTE *)(0x5280B0+dwLoadOffset), 0x90, 14);

	// savegames
	*(WORD *)(0x6C31A5+dwLoadOffset) = 0x9090; 	// NOP; NOP - save file CRC32 check
	injectFunction (0x6C2BD0, (DWORD)getSavefilePath); // replace getSavefilePath
	pszPath = (char *)(0x12898B0+dwLoadOffset);

	*(DWORD *)(0xB3E190+dwLoadOffset) = 0x90C301B0;	// mov al, 1; retn
	*(DWORD *)(0xB3E1B0+dwLoadOffset) = 0x90C301B0;
	*(DWORD *)(0xB3E1C0+dwLoadOffset) = 0x90C301B0;
	*(DWORD *)(0xB3E1F0+dwLoadOffset) = 0x90C301B0;

	//TRACE("Patching OK (1.0.0.4 - update 5)\n");
}

void patch106 ()
{
	dwGameVersion = IvPatch6;	// GTA IV 1.0.6.0 (patch 6)

	// process patches
	*(DWORD *)(0x401855+dwLoadOffset) = 1;		// disable sleep
	*(BYTE  *)(0xD35310+dwLoadOffset) = 0xC3;	// RETN - enable debugger in error menu (don't load WER.dll)
	*(DWORD *)(0x403F30+dwLoadOffset) = 0x900008C2;	// RETN 8 - certificates check
	*(DWORD *)(0x40264D+dwLoadOffset) = 0x4AE9C033;	// xor eax, eax - address of the RGSC object
	*(DWORD *)(0x402651+dwLoadOffset) = 0x90000002;	// jmp 40289E (skip RGSC connect and EFC checks)
	*(WORD *)(0x4028A3+dwLoadOffset) = 0xA390;	// NOP; MOV [g_rgsc], eax
	memset ((BYTE *)(0x40290D+dwLoadOffset), 0x90, 0x2A);
	*(DWORD *)(0x40293D+dwLoadOffset) = 0x90909090;	// NOP*4- last RGSC init check
	*(WORD  *)(0x402941+dwLoadOffset) = 0x9090;	// NOP*2- last RGSC init check

	// skip missing tests...
	memset ((BYTE *)(0x402B32+dwLoadOffset), 0x90, 14);
	memset ((BYTE *)(0x402D37+dwLoadOffset), 0x90, 14);
	*(DWORD *)(0x403890+dwLoadOffset) = 0x90C3C033;	// xor eax, eax; retn
	*(DWORD *)(0x404270+dwLoadOffset) = 0x90C3C033;	// xor eax, eax; retn

	// savegames
	*(WORD *)(0x5B0505+dwLoadOffset) = 0x9090; 	// NOP; NOP - save file CRC32 check
	injectFunction (0x5AFF30, (DWORD)getSavefilePath); // replace getSavefilePath
	pszPath = (char *)(0x10F1100+dwLoadOffset);

	*(DWORD *)(0xBABFB0+dwLoadOffset) = 0x90C301B0;	// mov al, 1; retn
	*(DWORD *)(0xBABFD0+dwLoadOffset) = 0x90C301B0;
	*(DWORD *)(0xBABFE0+dwLoadOffset) = 0x90C301B0;
	*(DWORD *)(0xBAC010+dwLoadOffset) = 0x90C301B0;

	//TRACE("Patching OK (1.0.6.0 - update 6)\n");
}

void patch106J ()
{
	dwGameVersion = IvPatch6J;	// GTA IV 1.0.4.2 (patch 6)

	// process patches
	*(DWORD *)(0x401835+dwLoadOffset) = 1;		// disable sleep
	*(BYTE  *)(0xD35180+dwLoadOffset) = 0xC3;	// RETN - enable debugger in error menu (don't load WER.dll)
	*(DWORD *)(0x403F30+dwLoadOffset) = 0x900008C2;	// RETN 8 - certificates check
	*(DWORD *)(0x40264D+dwLoadOffset) = 0x4AE9C033;	// xor eax, eax - address of the RGSC object
	*(DWORD *)(0x402651+dwLoadOffset) = 0x90000002;	// jmp 40289E (skip RGSC connect and EFC checks)
	*(WORD *)(0x4028A3+dwLoadOffset) = 0xA390;	// NOP; MOV [g_rgsc], eax
	memset ((BYTE *)(0x40290D+dwLoadOffset), 0x90, 0x2A);
	*(DWORD *)(0x40293D+dwLoadOffset) = 0x90909090;	// NOP*4- last RGSC init check
	*(WORD  *)(0x402941+dwLoadOffset) = 0x9090;	// NOP*2- last RGSC init check

	// skip missing tests...
	memset ((BYTE *)(0x402B32+dwLoadOffset), 0x90, 14);
	memset ((BYTE *)(0x402D37+dwLoadOffset), 0x90, 14);
	*(DWORD *)(0x403890+dwLoadOffset) = 0x90C3C033;	// xor eax, eax; retn
	*(DWORD *)(0x404270+dwLoadOffset) = 0x90C3C033;	// xor eax, eax; retn

	// savegames
	*(WORD *)(0x5B0215+dwLoadOffset) = 0x9090; 	// NOP; NOP - save file CRC32 check
	injectFunction (0x5AFC40, (DWORD)getSavefilePath); // replace getSavefilePath
	pszPath = (char *)(0x10F1100+dwLoadOffset);

	*(DWORD *)(0xBAFA10+dwLoadOffset) = 0x90C301B0;	// mov al, 1; retn
	*(DWORD *)(0xBAFA30+dwLoadOffset) = 0x90C301B0;
	*(DWORD *)(0xBAFA40+dwLoadOffset) = 0x90C301B0;
	*(DWORD *)(0xBAFA70+dwLoadOffset) = 0x90C301B0;

	//TRACE("Patching OK (1.0.4.2 - update 6)\n");
}

void patch107 ()
{
	dwGameVersion = IvPatch7;	// GTA IV 1.0.7.0 (patch 7)

	// process patches
	*(DWORD *)(0x401835+dwLoadOffset) = 1;		    // disable sleep
	*(BYTE  *)(0xD356D0+dwLoadOffset) = 0xC3;	    // RETN - enable debugger in error menu (don't load WER.dll)
	*(DWORD *)(0x403F10+dwLoadOffset) = 0x900008C2;	// RETN 8 - certificates check
	*(DWORD *)(0x40262D+dwLoadOffset) = 0x4AE9C033;	// xor eax, eax - address of the RGSC object
	*(DWORD *)(0x402631+dwLoadOffset) = 0x90000002;	// jmp 40289E (skip RGSC connect and EFC checks)
	*(WORD  *)(0x402883+dwLoadOffset) = 0xA390;	    // NOP; MOV [g_rgsc], eax
	memset ((BYTE *)(0x4028ED+dwLoadOffset), 0x90, 0x2A);
	*(DWORD *)(0x40291D+dwLoadOffset) = 0x90909090;	// NOP*4- last RGSC init check
	*(WORD  *)(0x402921+dwLoadOffset) = 0x9090;	    // NOP*2- last RGSC init check

	// skip missing tests...
	memset ((BYTE *)(0x402B12+dwLoadOffset), 0x90, 14);
	memset ((BYTE *)(0x402D17+dwLoadOffset), 0x90, 14);
	*(DWORD *)(0x403870+dwLoadOffset) = 0x90C3C033;	// xor eax, eax; retn
	*(DWORD *)(0x404250+dwLoadOffset) = 0x90C3C033;	// xor eax, eax; retn

	// savegames
	*(WORD *)(0x5B06E5+dwLoadOffset) = 0x9090; 	// NOP; NOP - save file CRC32 check
	injectFunction (0x5B0110, (DWORD)getSavefilePath); // replace getSavefilePath
	pszPath = (char *)(0x10F1DA0+dwLoadOffset);

	*(DWORD *)(0xBAC160+dwLoadOffset) = 0x90C301B0;	// mov al, 1; retn
	*(DWORD *)(0xBAC180+dwLoadOffset) = 0x90C301B0;
	*(DWORD *)(0xBAC190+dwLoadOffset) = 0x90C301B0;
	*(DWORD *)(0xBAC1C0+dwLoadOffset) = 0x90C301B0;

	//TRACE("Patching OK (1.0.7.0 - update 7)\n");
}

void patchEflc1 ()
{
	dwGameVersion = EflcPatch1;	// EfLC 1.1.1.0 (patch 1)

	// savegames
	*(WORD *)(0x6DEFB5+dwLoadOffset) = 0x9090; 	// NOP; NOP - save file CRC32 check
	injectFunction (0x6DE9E0, (DWORD)getSavefilePath); // replace getSavefilePath
	pszPath = (char *)(0x10DF298+dwLoadOffset);	    // szSavegamePath[512]

	// process patches
	*(DWORD *)(0x401835+dwLoadOffset) = 1;		// disable sleep
	*(BYTE  *)(0x7CA680+dwLoadOffset) = 0xC3;	// RETN - enable debugger in error menu (don't load WER.dll)

	*(DWORD *)(0x474C20+dwLoadOffset) = 0x900008C2;	// RETN 8 - certificates check
	*(DWORD *)(0x47334D+dwLoadOffset) = 0x4AE9C033;	// xor eax, eax - address of the RGSC object
	*(DWORD *)(0x473351+dwLoadOffset) = 0x90000002;	// jmp 40289E (skip RGSC connect and EFC checks)
	*(WORD *)(0x4735A3+dwLoadOffset) = 0xA390;	// NOP; MOV [g_rgsc], eax
	memset ((BYTE *)(0x47360D+dwLoadOffset), 0x90, 0x2A);   // data integrity checks
	*(DWORD *)(0x47363D+dwLoadOffset) = 0x90909090;	// NOP*4- last RGSC init check
	*(WORD  *)(0x473641+dwLoadOffset) = 0x9090;	// NOP*2- last RGSC init check

	// skip missing tests...
	memset ((BYTE *)(0x473832+dwLoadOffset), 0x90, 14);
	memset ((BYTE *)(0x473A27+dwLoadOffset), 0x90, 14);
	memset ((BYTE *)(0x49412C+dwLoadOffset), 0x90, 24);
	*(DWORD *)(0x474580+dwLoadOffset) = 0x90C3C033;	// xor eax, eax; retn
	*(DWORD *)(0x474F60+dwLoadOffset) = 0x90C3C033;	// xor eax, eax; retn

	// >> TEST
	*(DWORD *)(0x474FD0+dwLoadOffset) = 0x90C3C033;	// xor eax, eax; retn
	*(BYTE *)(0x7CAD20+dwLoadOffset) = 0xC3;
	// *(DWORD *)(0xD2CB1C+dwLoadOffset) = 0xC340C033; // xor eax, eax; inc eax; retn - DFA_init => TODO: move to noSDFA.dll
	// *(WORD *)(0x7E1DF7+dwLoadOffset) = 0x9090;  // isInternetConnectionPresent

	// fix messed sequences
	*(DWORD *)(0xC25490+dwLoadOffset) = 0x90C301B0;	// mov al, 1; retn
	*(DWORD *)(0xC254B0+dwLoadOffset) = 0x90C301B0;
	*(DWORD *)(0xC254C0+dwLoadOffset) = 0x90C301B0;
	*(DWORD *)(0xC254F0+dwLoadOffset) = 0x90C301B0;

	//TRACE("Patching OK (EfLC 1.1.1.0 - update 6)\n");
	return;

	// *(BYTE *)(0xBBBF70+dwLoadOffset) = 0xC3; // startContentEnumerateThread
	// injectFunction (0x9BCEC0, (DWORD)getModelByHash);
}

void patchEflc1R ()
{
	dwGameVersion = EflcPatch1R;  // EfLC 1.1.0.1 (patch 1 rus)

	// savegames
	*(WORD *)(0x6DEB45+dwLoadOffset) = 0x9090;   // NOP; NOP - save file CRC32 check
	injectFunction (0x6DE570, (DWORD)getSavefilePath); // replace getSavefilePath
	pszPath = (char *)(0x1055AA8+dwLoadOffset);      // szSavegamePath[512]

	// process patches
	*(DWORD *)(0x401835+dwLoadOffset) = 1;    // disable sleep
	*(BYTE  *)(0x7CA310+dwLoadOffset) = 0xC3;  // RETN - enable debugger in error menu (don't load WER.dll)

	*(DWORD *)(0x474660+dwLoadOffset) = 0x900008C2;  // RETN 8 - certificates check
	*(DWORD *)(0x472D7D+dwLoadOffset) = 0x4AE9C033;  // xor eax, eax - address of the RGSC object
	*(DWORD *)(0x472D81+dwLoadOffset) = 0x90000002;  // jmp 40289E (skip RGSC connect and EFC checks)
	*(WORD *)(0x472FD3+dwLoadOffset) = 0xA390;  // NOP; MOV [g_rgsc], eax
	memset ((BYTE *)(0x47303D+dwLoadOffset), 0x90, 0x2A);   // data integrity checks
	*(DWORD *)(0x47306D+dwLoadOffset) = 0x90909090;  // NOP*4- last RGSC init check
	*(WORD  *)(0x473071+dwLoadOffset) = 0x9090;  // NOP*2- last RGSC init check

	// skip missing tests...
	memset ((BYTE *)(0x473262+dwLoadOffset), 0x90, 14);
	memset ((BYTE *)(0x473467+dwLoadOffset), 0x90, 14);
	memset ((BYTE *)(0x493B8C+dwLoadOffset), 0x90, 24);
	*(DWORD *)(0x473FC0+dwLoadOffset) = 0x90C3C033;  // xor eax, eax; retn
	*(DWORD *)(0x4749A0+dwLoadOffset) = 0x90C3C033;  // xor eax, eax; retn

	// >> TEST
	*(DWORD *)(0x474A10+dwLoadOffset) = 0x90C3C033;  // xor eax, eax; retn
	*(BYTE *)(0x7FE170+dwLoadOffset) = 0xC3;
	// *(DWORD *)(0xD2CB1C+dwLoadOffset) = 0xC340C033; // xor eax, eax; inc eax; retn - DFA_init => TODO: move to noSDFA.dll
	// *(WORD *)(0x7E1DF7+dwLoadOffset) = 0x9090;  // isInternetConnectionPresent

	// fix messed sequences
	*(DWORD *)(0xC1C8E0+dwLoadOffset) = 0x90C301B0;  // mov al, 1; retn
	*(DWORD *)(0xC1C900+dwLoadOffset) = 0x90C301B0;
	*(DWORD *)(0xC1C910+dwLoadOffset) = 0x90C301B0;
	*(DWORD *)(0xC1C940+dwLoadOffset) = 0x90C301B0;

	//TRACE("Patching OK (EfLC 1.1.0.1 - update 7)\n");
	return;
}


void patchEflc2 ()
{
	dwGameVersion = EflcPatch2;	// EfLC 1.1.2.0 (patch 2)

	// process patches
	*(DWORD *)(0x401855+dwLoadOffset) = 1;		// disable sleep
	*(BYTE  *)(0x7CA700+dwLoadOffset) = 0xC3;	// RETN - enable debugger in error menu (don't load WER.dll)
	*(DWORD *)(0x474800+dwLoadOffset) = 0x900008C2;	// RETN 8 - certificates check
	*(DWORD *)(0x472F1D+dwLoadOffset) = 0x4AE9C033;	// xor eax, eax - address of the RGSC object
	*(DWORD *)(0x472F21+dwLoadOffset) = 0x90000002;	// jmp 40289E (skip RGSC connect and EFC checks)
	*(WORD  *)(0x473173+dwLoadOffset) = 0xA390;	// NOP; MOV [g_rgsc], eax
	memset ((BYTE *)(0x4731DD+dwLoadOffset), 0x90, 0x2A);   // data integrity checks
	*(DWORD *)(0x47320D+dwLoadOffset) = 0x90909090;	// NOP*4- last RGSC init check
	*(WORD  *)(0x473211+dwLoadOffset) = 0x9090;	// NOP*2- last RGSC init check

	// skip missing tests...
	*(DWORD *)(0x474160+dwLoadOffset) = 0x90C3C033;	// xor eax, eax; retn
	*(DWORD *)(0x474B40+dwLoadOffset) = 0x90C3C033;	// xor eax, eax; retn
	memset ((BYTE *)(0x473402+dwLoadOffset), 0x90, 14);
	memset ((BYTE *)(0x473607+dwLoadOffset), 0x90, 14);
	memset ((BYTE *)(0x493D4C+dwLoadOffset), 0x90, 24);

	// savegames
	*(WORD *)(0x6DF085+dwLoadOffset) = 0x9090; 	// NOP; NOP - save file CRC32 check
	injectFunction (0x6DEAB0, (DWORD)getSavefilePath); // replace getSavefilePath
	pszPath = (char *)(0x10562C8+dwLoadOffset);	    // szSavegamePath[512]

	// fix messed sequences
	*(DWORD *)(0xC1AD20+dwLoadOffset) = 0x90C301B0;	// mov al, 1; retn
	*(DWORD *)(0xC1AD40+dwLoadOffset) = 0x90C301B0;
	*(DWORD *)(0xC1AD50+dwLoadOffset) = 0x90C301B0;
	*(DWORD *)(0xC1AD80+dwLoadOffset) = 0x90C301B0;

	//TRACE("Patching OK (EfLC 1.1.2.0 - update 7)\n");
	return;
}


void patchRFG ()
{
	dwGameVersion = RfgUnpatched;	// RedFaction:Guerilla

	// disable savegame check
	*(WORD*)(0x522A38+dwLoadOffset) = 0x9090;
	*(WORD*)(0x522A3E+dwLoadOffset) = 0x9090;
	*(WORD*)(0x522A44+dwLoadOffset) = 0x9090;
	//TRACE("Patching OK (RF:G)\n");
}

void patchDR2 ()
{
	dwGameVersion = DR2Unpatched;   // Dead Rising 2

	*(WORD *)(0x8D838A+dwLoadOffset) = 0x5EEB;
	*(DWORD *)(0x8D9D60+dwLoadOffset) = 0x90C3C033;
	//TRACE("Patching OK (Dead Rising 2)\n");
}

void patchCode ()
{
	// get load address of the exe
	dwLoadOffset = (DWORD)GetModuleHandle (NULL);
	//TRACE("GetModuleHandle returns %08x\n", dwLoadOffset);

	// Unprotect image - make .text and .rdata section writeable
	BYTE * pImageBase = reinterpret_cast<BYTE *>(dwLoadOffset);
	PIMAGE_DOS_HEADER   pDosHeader = reinterpret_cast<PIMAGE_DOS_HEADER> (dwLoadOffset);
	PIMAGE_NT_HEADERS   pNtHeader  = reinterpret_cast<PIMAGE_NT_HEADERS> (pImageBase+pDosHeader->e_lfanew);
	PIMAGE_SECTION_HEADER pSection = IMAGE_FIRST_SECTION(pNtHeader);
	// //TRACE("[EXE] NtHeader contains %d sections\n", pNtHeaders->FileHeader.NumberOfSections);

	for (int iSection = 0; iSection < pNtHeader->FileHeader.NumberOfSections; ++iSection, ++pSection)
	{
		char * pszSectionName = reinterpret_cast<char *>(pSection->Name);
		if (!strcmp (pszSectionName, ".text") || !strcmp (pszSectionName, ".rdata"))
		{
			DWORD dwPhysSize = (pSection->Misc.VirtualSize + 4095) & ~4095;
			//TRACE("[EXE] unprotecting section '%s': addr = 0x%08x, size = 0x%08x\n", pSection->Name, pSection->VirtualAddress, dwPhysSize);

			DWORD	oldProtect;
			DWORD   newProtect = (pSection->Characteristics & IMAGE_SCN_MEM_EXECUTE) ? PAGE_EXECUTE_READWRITE : PAGE_READWRITE;
			if (!VirtualProtect (reinterpret_cast <VOID *>(dwLoadOffset+pSection->VirtualAddress), dwPhysSize, newProtect, &oldProtect))
			{
				//TRACE("[EXE] Virtual protect error\n");
				ExitProcess (0);
			}
		}
	}

	dwLoadOffset -= 0x400000;
	// ExitProcess (0);

	// version check
	DWORD signature = *(DWORD *)(0x608C34+dwLoadOffset);
	if (signature == 0x831F7518)
		patch101 ();
	else if (signature == 0xC483FFE4)
		patch102 ();
	else if (signature == 0x280F0000)
		patch103 ();
	else if (signature == 0x110FF300)
		patch104 ();
	else if (signature == 0xf3385058)
		patch105 ();
	else if (signature == 0x00a42494)
		patch106 ();
	else if (signature == 0xda280f30)
		patch106J ();
	else if (signature == 0x1006e857)
		patch107 ();
	else if (signature == 0x0f14247c)
		patchEflc1 ();
	else if (signature == 0x0d5c0ff3)
		patchEflc2 ();
	else if (signature == 0x41110ff3)
		patchEflc1R ();
	else if (signature == 0x108b1874)
		patchRFG ();
	else if (signature == 0x1B10044)
		patchDR2 ();
	else
		//TRACE("Unknown game version, skipping patches (signature = 0x%08x)\n", signature);
}

// Plugin Loader
void loadPlugins (char * pszMask)
{
	/*
	if (!pszMask)
		return;
	char * pszType = strrchr (pszMask, '.');
	DWORD typeMask = pszType ? *(DWORD *)pszType : 0x6c6c642e; // '.dll'

	WIN32_FIND_DATA fd;
	char pathName[MAX_PATH];	// module name buffer
	char * p = strrchr (pszMask, '\\');
	char * namePtr = pathName;
	DWORD nameLen = p-pszMask+1;
	if (p)
	{
		strcpy_s (pathName, MAX_PATH, pszMask);
		pathName[nameLen] = '\0';
		namePtr = pathName + nameLen;
	}


	HANDLE asiFile = FindFirstFile (pszMask, &fd);
	if (asiFile == INVALID_HANDLE_VALUE)
		return;
	do
	{
		if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			int pos = 0;
			while (fd.cFileName[pos])
				pos++;
			DWORD type = *(DWORD *)(fd.cFileName+pos-4);
			type |= 0x20202020; // convert to lowercase
			if (type == typeMask)
			{
				strcpy_s (namePtr, MAX_PATH-nameLen, fd.cFileName);
				if (!LoadLibrary (pathName))
					//TRACE("Error loading library %d\n", GetLastError ());
				//TRACE("plugin loader: loaded '%s'\n", pathName);
			}

		}
	}
	while (FindNextFile (asiFile, &fd));
	FindClose (asiFile);
	*/
}
#endif
