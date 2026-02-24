#include "stdafx.h"

#include "cartographer/discord/discord_interface.h"
#include "interface/new_hud_draw.h"
#include "shell/shell.h"
#include "tag_files/tag_loader/tag_injection.h"

#include "H2MOD/Modules/Shell/Startup/Startup.h"
#include "H2MOD/Modules/Shell/Config.h"

#ifdef TEST_DISCORD_INSTANCE
#include "H2MOD/Modules/Shell/H2MODShell.h"
#endif

#include <time.h>

/* externs */

extern void DeinitCustomLanguage();

extern void h2log_dispose(void);

/* prototypes */

static void initialize_instance(void);

static void discord_dispose(void);

static void heap_debug_initialize(void);

static void exit_instance(void);

/* globals */

HMODULE hThis = NULL;

/* public code */

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		hThis = hModule;
		srand((unsigned int)time(NULL));
		initialize_instance();
		break;

	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;

	case DLL_PROCESS_DETACH:
		exit_instance();
		break;
	}
	return TRUE;
}

static void initialize_instance(void)
{
	heap_debug_initialize();
	InitH2Startup();
	return;
}


/* private code */

static void discord_dispose(void)
{
	if (!shell_is_dedicated_server()
		&& H2Config_discord_enable
#ifdef TEST_DISCORD_INSTANCE
		&& g_instance_number == 1
#endif
		)
	{
		discord_game_status_dispose();
	}
	return;
}

static void heap_debug_initialize(void)
{
#if CARTOGRAPHER_HEAP_DEBUG
	int CurrentFlags;
	CurrentFlags = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
	CurrentFlags |= _CRTDBG_DELAY_FREE_MEM_DF;
	CurrentFlags |= _CRTDBG_LEAK_CHECK_DF;
	CurrentFlags |= _CRTDBG_CHECK_ALWAYS_DF;
	_CrtSetDbgFlag(CurrentFlags);
#endif
	return;
}

static void exit_instance(void)
{
	discord_dispose();
	tag_injection_deinitialize();
	new_hud_draw_deinitialize();

	DeinitCustomLanguage();
	DeinitH2Config();

	h2log_dispose();

#if CARTOGRAPHER_HEAP_DEBUG
	_CrtDumpMemoryLeaks();
#endif
	TerminateProcess(GetCurrentProcess(), 0);
	return;
}
