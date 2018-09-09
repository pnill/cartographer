#include "stdafx.h"
#include <io.h>
#include <fcntl.h>
#include "H2MOD.h"
#include <iostream>
#include <Shellapi.h>
#include "H2Startup.h"
#include "H2OnscreenDebugLog.h"
#include "GSRunLoop.h"
#include "H2ConsoleCommands.h"
#include "H2Config.h"
#include <sstream>
#include "ReadIniArguments.h"

extern ConsoleCommands* commands;

using namespace std;

#include "Detour.h"

HMODULE hThis = NULL;

CRITICAL_SECTION d_lock;

UINT g_online = 1;

UINT g_signin[4] = { 1,0,0,0 };
CHAR g_szUserName[4][16 + 1] = { "Cartographer1", "Cartographer2", "Cartographer3", "Cartographer4" };

XUID xFakeXuid[4] = { 0xEE100000DEADC0DE, 0xEE200000DEADC0DE, 0xEE300000DEADC0DE, 0xEE400000DEADC0DE };
//CHAR g_profileDirectory[512] = "Profiles";

std::wstring dlcbasepath;

#pragma region H2 GunGame Variables
	bool b_GunGame = 0;
	int weapon_one = 0;
	int weapon_two = 0;
	int weapon_three = 0;
	int weapon_four = 0;
	int weapon_five = 0;
	int weapon_six = 0;
	int weapon_seven = 0;
	int weapon_eight = 0;
	int weapon_nine = 0;
	int weapon_ten = 0;
	int weapon_eleven = 0;
	int weapon_tweleve = 0;
	int weapon_thirteen = 0;
	int weapon_fourteen = 0;
	int weapon_fiffteen = 0;
	int weapon_sixteen = 0;
#pragma endregion

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

#pragma region GunGame Levels
		if (b_GunGame == 1)
		{
			FILE* gfp;
			gfp = fopen("gungame.ini", "r");

			if (gfp)
			{
				TRACE("[GunGame Enabled] - Opened GunGame.ini!");
				while (!feof(gfp))
				{
					char gstr[256];

					fgets(gstr, 256, gfp);

					gCHECK_ARG("weapon_one =", weapon_one);
					gCHECK_ARG("weapon_two =", weapon_two);
					gCHECK_ARG("weapon_three =", weapon_three);
					gCHECK_ARG("weapon_four =", weapon_four);
					gCHECK_ARG("weapon_five =", weapon_five);
					gCHECK_ARG("weapon_six =", weapon_six);
					gCHECK_ARG("weapon_seven =", weapon_seven);
					gCHECK_ARG("weapon_eight =", weapon_eight);
					gCHECK_ARG("weapon_nine =", weapon_nine);
					gCHECK_ARG("weapon_ten =", weapon_ten);
					gCHECK_ARG("weapon_eleven =", weapon_eleven);
					gCHECK_ARG("weapon_tweleve =", weapon_tweleve);
					gCHECK_ARG("weapon_thirteen =", weapon_thirteen);
					gCHECK_ARG("weapon_fourteen =", weapon_fourteen);
					gCHECK_ARG("weapon_fifteen =", weapon_fiffteen);
					gCHECK_ARG("weapon_sixteen =", weapon_sixteen);

				}

				fclose(gfp);
			}
		}

#pragma endregion

		if (h2mod)
			h2mod->Initialize();
		else
			TRACE("H2MOD Failed to intialize");

		TRACE("[GunGame] : %i", b_GunGame);
		if (b_GunGame == 1)
		{
			TRACE("[GunGame] - weapon_one: %i", weapon_one);
			TRACE("[GunGame] - weapon_two: %i", weapon_two);
			TRACE("[GunGame] - weapon_three: %i", weapon_three);
		}

		WCHAR gameName[256];

		GetModuleFileNameW(NULL, (LPWCH)&gameName, sizeof(gameName));
		TRACE("%s", gameName);

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
	delete xlive_trace_log;
	delete h2mod_log;
	delete network_log;
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


