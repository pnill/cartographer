#include <windows.h>
#include "MinHook\include\MinHook.h"

#include "Detour.h"
#include <stdio.h>
#include <Softpub.h>


#pragma comment(lib, "libMinHook.x86.lib")
#pragma comment(lib, "Wintrust.lib")


//#define DEBUG_FILE


typedef HANDLE (WINAPI *CREATEFILE) (
	LPCTSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode,
  LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition,
  DWORD dwFlagsAndAttributes, HANDLE hTemplateFile );


typedef LONG (WINAPI* WINVERIFYTRUST) (
	HWND hwnd, GUID *pgActionID, LPVOID pWVTData );




CREATEFILE pCreateFileW = NULL;
WINVERIFYTRUST pWinVerifyTrust = NULL;



WCHAR exePathW[8192];
WCHAR xlivePathW[8192];


static void __stdcall Debug_Log_W( WCHAR *name )
{
	static FILE *fp_w = 0;


#ifndef DEBUG_FILE
	return;
#endif



	if( fp_w == 0 )
	{
		fp_w = _wfopen( L"xlive-log-w.txt", L"w" );
		fwprintf( fp_w, L"%s\n", xlivePathW );
	}


	fwprintf( fp_w, L"%s\n", name );
	fflush( fp_w );
}



static void __stdcall Path_Strip_W( WCHAR *name )
{
	int len = wcslen( name );


	while( name[len] != L'\\' )
		len--;


	name[len+1] = 0;
}



static void __stdcall DetourCreateFileW_code( HANDLE &result, LPCTSTR lpFileName )
{
	Debug_Log_W( L"===  DetourCreateFileW" );
	Debug_Log_W( (WCHAR *) lpFileName );


		
	// bounce xlive.dll check
	//if( wcscmp( xlivePathW, (WCHAR *) lpFileName ) == 0 )
		//return INVALID_HANDLE_VALUE;
}



static HANDLE WINAPI DetourCreateFileW( LPCTSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode,
  LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition,
  DWORD dwFlagsAndAttributes, HANDLE hTemplateFile )
{
	HANDLE result;


	// get result first - avoid register smashing by compiler!
	result = pCreateFileW( lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes,
		dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile );


	// stdcall - no register smashing beyond this point!!
	DetourCreateFileW_code( result, lpFileName );


	return result;
}




static LONG WINAPI DetourWinVerifyTrust( HWND hwnd, GUID *pgActionID, LPVOID pWVTData )
{
	// xlive.dll verified
	return 0;
}



void Detour()
{
	GetModuleFileNameW( NULL, (LPWCH) &exePathW, 8192 );

	wcscpy( xlivePathW, exePathW );
	Path_Strip_W( xlivePathW );
	wcscat( xlivePathW, L"xlive.dll" );


#ifdef DEBUG_FILE
	Debug_Log_W( L"DetourCreateFileW" );
#endif




	MH_Initialize();


	// kernel32 = already known
	//MH_CreateHook( &CreateFileW, &DetourCreateFileW, reinterpret_cast<void**>(&pCreateFileW) );
	//MH_QueueEnableHook( &CreateFileW );


	// Wintrust.WinVerifyTrust = not known yet
	// - jmp dword ptr [x]
	// - FF 25 <32-bit addr>

	DWORD *addr = (DWORD *) (&WinVerifyTrust);
	addr = (DWORD *) ( (BYTE *) addr + 2 );
	addr = (DWORD *) ( *addr );

	MH_CreateHook( (VOID *) ( *addr ), DetourWinVerifyTrust, reinterpret_cast<void**>(&pWinVerifyTrust) );
	MH_QueueEnableHook( (VOID *) ( *addr ) );



	MH_ApplyQueued();
}
