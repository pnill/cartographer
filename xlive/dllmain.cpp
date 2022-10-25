#include "stdafx.h"

#include "H2MOD.h"
#include "H2MOD\Modules\Shell\Startup\Startup.h"
#include "Util\Hooks\Detour.h"

HMODULE hThis = NULL;
CRITICAL_SECTION d_lock;

std::wstring dlcbasepath;
//CHAR g_profileDirectory[512] = "Profiles";

std::string ModulePathA(HMODULE hModule = NULL)
{
	char strPath[MAX_PATH];
	memset(strPath, 0, sizeof(strPath));
	GetModuleFileNameA(hModule, strPath, MAX_PATH);

	for (int i = strlen(strPath) - 1; i >= 0; i--)
	{ 
		if (strPath[i] == '\\') {
			strPath[i + 1] = '\0'; break;
		}
	}
	return std::string(strPath);
}

std::wstring ModulePathW(HMODULE hModule = NULL)
{
	wchar_t strPath[MAX_PATH];
	memset(strPath, 0, sizeof(strPath));
	GetModuleFileNameW(hModule, strPath, MAX_PATH);
	for (int i = wcslen(strPath) - 1; i >= 0; i--)
	{
		if (strPath[i] == L'\\') {
			strPath[i + 1] = L'\0'; break;
		}
	}
	return std::wstring(strPath);
}

void HeapDebugInitialize()
{
#if HEAP_DEBUG
	int CurrentFlags;
	CurrentFlags = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
	CurrentFlags |= _CRTDBG_DELAY_FREE_MEM_DF;
	CurrentFlags |= _CRTDBG_LEAK_CHECK_DF;
	CurrentFlags |= _CRTDBG_CHECK_ALWAYS_DF;
	_CrtSetDbgFlag(CurrentFlags);
#endif
}

void InitInstance()
{
	static bool init = false;

	if (!init)
	{
		init = true;
		InitializeCriticalSection(&d_lock);

		HeapDebugInitialize();
		H2DedicatedServerStartup();

		dlcbasepath = L"DLC";

		H2MOD::Initialize();

		//extern GunGame* gunGame;
		//gunGame->readWeaponLevels();
	}
}

extern CRITICAL_SECTION log_section;
void ExitInstance()
{
#ifndef NO_TRACE
	EnterCriticalSection(&log_section);
	delete xlive_log;
	delete h2mod_log;
	delete network_log;
	delete console_log;
	delete onscreendebug_log;
#if COMPILE_WITH_VOICE
	delete voice_log;
#endif
	LeaveCriticalSection(&log_section);
	DeleteCriticalSection(&log_section);
#endif
	DeleteCriticalSection(&d_lock);
	TerminateProcess(GetCurrentProcess(), 0);
}

//=============================================================================
// Entry Point
BOOL APIENTRY DllMain( HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		hThis = hModule;
		srand((unsigned int)time(NULL));
		InitH2Startup();
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
