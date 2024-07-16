#include "stdafx.h"

#include "cartographer/discord/discord_interface.h"

#include "H2MOD.h"
#include "H2MOD/Modules/Shell/Startup/Startup.h"
#include "H2MOD/Modules/Shell/Config.h"

HMODULE hThis = NULL;


#define k_discord_dll_filename L"discord_game_sdk.dll"

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
#if CART_HEAP_DEBUG
	int CurrentFlags;
	CurrentFlags = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
	CurrentFlags |= _CRTDBG_DELAY_FREE_MEM_DF;
	CurrentFlags |= _CRTDBG_LEAK_CHECK_DF;
	CurrentFlags |= _CRTDBG_CHECK_ALWAYS_DF;
	_CrtSetDbgFlag(CurrentFlags);
#endif
}

void discord_initialize()
{
	HMODULE module = LoadLibraryW(k_discord_dll_filename);
	if (module && !Memory::IsDedicatedServer() && H2Config_discord_enable)
	{
		discord_game_status_create(module);
	}

	return;
}

void InitInstance()
{
	static bool init = false;

	if (!init)
	{
		init = true;

		HeapDebugInitialize();
		H2DedicatedServerStartup();

		discord_initialize();
	}
}

extern CRITICAL_SECTION log_section;
void ExitInstance()
{
	if (!Memory::IsDedicatedServer() && H2Config_discord_enable)//&& _Shell::GetInstanceId() == 1)
	{
		discord_game_status_dispose();
	}

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
