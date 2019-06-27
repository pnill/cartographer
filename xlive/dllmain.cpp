#include "stdafx.h"
#include "H2MOD.h"
#include "H2MOD\Modules\Startup\Startup.h"
#include "H2MOD\Modules\Console\ConsoleCommands.h"
#include "H2MOD\Variants\GunGame\GunGame.h"
#include "Util\ReadIniArguments.h"
#include "Util\Hooks\Detour.h"

extern ConsoleCommands* commands;

HMODULE hThis = NULL;

CRITICAL_SECTION d_lock;

UINT g_online = 1;

UINT g_signin[4] = { 1,0,0,0 };
CHAR g_szUserName[4][16] = { "Cartographer1", "Cartographer2", "Cartographer3", "Cartographer4" };

XUID xFakeXuid[4] = { 0xEE100000DEADC0DE, 0xEE200000DEADC0DE, 0xEE300000DEADC0DE, 0xEE400000DEADC0DE };
//CHAR g_profileDirectory[512] = "Profiles";

std::wstring dlcbasepath;

std::string ModulePathA(HMODULE hModule = NULL)
{
	static char strPath[MAX_PATH];
	GetModuleFileNameA(hModule, strPath, MAX_PATH);
	return strPath;
}

std::wstring ModulePathW(HMODULE hModule = NULL)
{
	static wchar_t strPath[MAX_PATH];
	GetModuleFileNameW(hModule, strPath, MAX_PATH);
	return strPath;
}

void InitInstance()
{
	static bool init = true;

	if (init)
	{
		init = false;

		InitH2Startup2();

#ifdef _DEBUG
		int CurrentFlags;
		CurrentFlags = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
		CurrentFlags |= _CRTDBG_DELAY_FREE_MEM_DF;
		CurrentFlags |= _CRTDBG_LEAK_CHECK_DF;
		CurrentFlags |= _CRTDBG_CHECK_ALWAYS_DF;
		_CrtSetDbgFlag(CurrentFlags);
#endif
		InitializeCriticalSection(&d_lock);

		dlcbasepath = L"DLC";

		if (h2mod)
			h2mod->Initialize();
		else
			LOG_CRITICAL_GAME("H2MOD Failed to intialize");

		//extern GunGame* gunGame;
		//gunGame->readWeaponLevels();

		//extern void LoadAchievements();
		//LoadAchievements();
	}
}

extern CRITICAL_SECTION log_section;
void ExitInstance()
{
	//extern void SaveAchievements();
	//SaveAchievements();
#ifndef NO_TRACE
	EnterCriticalSection(&log_section);
	delete xlive_log;
	delete h2mod_log;
	delete network_log;
	delete console_log;
	delete onscreendebug_log;
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


