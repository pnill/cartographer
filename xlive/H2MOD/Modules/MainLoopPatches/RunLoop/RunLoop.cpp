#include "stdafx.h"
#include "RunLoop.h"

#include "game/game.h"
#include "game/game_time.h"
#include "shell/shell_windows.h"
#include "main/main_game_time.h"
#include "rasterizer/dx9/rasterizer_dx9_main.h"

#include "H2MOD/GUI/XLiveRendering.h"
#include "H2MOD/Modules/CustomMenu/CustomMenu.h"
#include "H2MOD/Modules/EventHandler/EventHandler.hpp"
#include "H2MOD/Modules/Input/ControllerInput.h"
#include "H2MOD/Modules/MapManager/MapManager.h"
#include "H2MOD/Modules/OnScreenDebug/OnscreenDebug.h"
#include "H2MOD/Modules/Shell/Config.h"
#include "H2MOD/Modules/Shell/H2MODShell.h"
#include "H2MOD/Modules/Shell/Startup/Startup.h"
#include "H2MOD/Modules/Stats/StatsHandler.h"
#include "H2MOD/Utils/Utils.h"


#include "XLive/xnet/IpManagement/XnIp.h"


// TODO Move most of this logic to Blam/Engine/main/main.cpp

#define TIMER_RESOLUTION_MS 1

H2Config_Experimental_Rendering_Mode g_experimental_rendering_mode = _rendering_mode_original_game_frame_limit;

void CartographerMainLoop() {
	static bool halo2WindowExists = false;
	if (!H2IsDediServer && !halo2WindowExists && H2hWnd != NULL) {
		halo2WindowExists = true;
		DWORD Display_Mode = 1;
		HKEY hKeyVideoSettings = NULL;
		if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Halo 2\\Video Settings", 0, KEY_READ, &hKeyVideoSettings) == ERROR_SUCCESS) {
			GetDWORDRegKey(hKeyVideoSettings, L"DisplayMode", &Display_Mode);
			RegCloseKey(hKeyVideoSettings);
		}
		if (Display_Mode) {
			SetWindowLong(H2hWnd, GWL_STYLE, GetWindowLong(H2hWnd, GWL_STYLE) | WS_SIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU);
		}

		if (_Shell::GetInstanceId() > 1) {
			wchar_t titleMod[256];
			wchar_t titleOriginal[256];
			GetWindowText(H2hWnd, titleOriginal, 256);
			wsprintf(titleMod, L"%ls (P%d)", titleOriginal, _Shell::GetInstanceId());
			SetWindowText(H2hWnd, titleMod);
		}
	}
	if(H2IsDediServer)
	{
		//StatsHandler::playerRanksUpdateTick();
	}
	//EventHandler::executeGameLoopCallbacks();
	/*
	static bool halo2ServerOnce1 = false;
	if (H2IsDediServer && !halo2ServerOnce1) {
		halo2ServerOnce1 = true;
		pushHostLobby();
		wchar_t* LanServerName = (wchar_t*)((BYTE*)H2BaseAddr + 0x52042A);
		if (strlen(H2Config_dedi_server_name) > 0) {
			swprintf(LanServerName, 32, L"%hs", H2Config_dedi_server_name);
		}
	}

	static int prevPartyPrivacy = 0;
	int partyPrivacy;
	if (H2IsDediServer) {
		partyPrivacy = *(int*)(H2BaseAddr + 0x534850);
	}
	else {
		partyPrivacy = *(int*)(H2BaseAddr + 0x50A398);
	}
	if (prevPartyPrivacy > 0 && partyPrivacy == 0) {
		pushHostLobby();
	}
	prevPartyPrivacy = partyPrivacy;
	*/
}

void __cdecl main_game_time_initialize_defaults_hook()
{
	// windows 10 version 2004 and above added behaviour changes to how windows timer resolution works, and we have to explicitly set the time resolution
	// and since they were added, when playing on a laptop on battery it migth add heavy stuttering when using a frame limiter based on Sleep function (or std::this_thread::sleep_for) implementation
	// the game sets them already but only during the loading screen period, then it resets to system default when the loading screen ends 
	// (tho i think in the new implementation is working on a per thread basis now instead of global frequency, since it still works even when the game resets after loading screen ends and loading screen runs in another thread)

	// More code in Tweaks.cpp in InitH2Tweaks

	// More details @ https://randomascii.wordpress.com/2020/10/04/windows-timer-resolution-the-great-rule-change/

	timeBeginPeriod(TIMER_RESOLUTION_MS);

	auto p_main_game_time_initialize_defaults = Memory::GetAddressRelative<void(__cdecl*)()>(0x42869F, 0x424841);
	return p_main_game_time_initialize_defaults();
}

void __cdecl game_modules_dispose_hook() {
	auto p_game_modules_dispose = Memory::GetAddress<void(__cdecl*)()>(0x48BBF, 0x41E60);
	p_game_modules_dispose();

	DeinitH2Startup();

	// reset time resolution to system default on game exit (initialization happens in main_game_time_initialize_defaults_hook())
	timeEndPeriod(TIMER_RESOLUTION_MS);
}

// rasterizer_present hook
// used to limit framerate using our implementation
void __cdecl rasterizer_present_hook(bitmap_data* bitmap) {
	//typedef void(__cdecl* rasterizer_present_t)(int);
	//auto p_rasterizer_present = Memory::GetAddress<rasterizer_present_t>(0x26271A);

	rasterizer_present(bitmap);
	shell_windows_throttle_framerate(H2Config_fps_limit);
}

typedef void(_cdecl* main_loop_body_t)();
main_loop_body_t p_main_loop_body;
void __cdecl main_loop_body() {
	CartographerMainLoop();

	EventHandler::GameLoopEventExecute(EventExecutionType::execute_before);
	if (!Memory::IsDedicatedServer())
	{
		mapManager->MapDownloadUpdateTick();
		gXnIpMgr.GetLocalUserXn()->m_pckStats.PckDataSampleUpdate();	// update local user network stats
	}
	p_main_loop_body();
	EventHandler::GameLoopEventExecute(EventExecutionType::execute_after);
}

void InitRunLoop() {
	LOG_INFO_GAME("{} - initializing", __FUNCTION__);

	addDebugText("Pre RunLoop hooking.");
	p_main_loop_body = Memory::GetAddress<main_loop_body_t>(0x399CC, 0xBFDE);
	PatchCall(Memory::GetAddress(0x39E64, 0xC684), main_loop_body);

	if (H2IsDediServer) {
		addDebugText("Hooking loop & shutdown Function");
	}
	else {
		addDebugText("Hooking loop Function");

		g_experimental_rendering_mode = H2Config_experimental_fps;

		// override
		g_experimental_rendering_mode = _rendering_mode_original_game_frame_limit;

		// present hooks for the frame limiter
		PatchCall(Memory::GetAddress(0x19073C), rasterizer_present_hook);
		PatchCall(Memory::GetAddress(0x19074C), rasterizer_present_hook);

		switch (g_experimental_rendering_mode)
		{
		case _rendering_mode_original_game_frame_limit:
			main_game_time_apply_patches();
			g_main_game_time_frame_limiter_enabled = false;
			break;

		case _rendering_mode_none:
		default:
			break;
		} // switch (g_experimental_rendering_mode)
	}

	PatchCall(Memory::GetAddressRelative(0x439E3D, 0x40BA40), main_game_time_initialize_defaults_hook);
	PatchCall(Memory::GetAddress(0x39E7C, 0xC6F7), game_modules_dispose_hook);

	addDebugText("Post RunLoop Hooking.");
}

void DeinitRunLoop() {

}
