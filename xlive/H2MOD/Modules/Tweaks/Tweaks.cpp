#include "stdafx.h"
#include "Tweaks.h"

#include "Blam/Engine/interface/hud.h"
#include "Blam/Engine/game/game_time.h"
#include "Blam/Engine/rasterizer/rasterizer_settings.h"
#include "Blam/Engine/tag_files/files_windows.h"

#include "H2MOD/Modules/Accounts/AccountLogin.h"
#include "H2MOD/Modules/MapManager/MapManager.h"
#include "H2MOD/Modules/OnScreenDebug/OnscreenDebug.h"
#include "H2MOD/Modules/Shell/Config.h"
#include "H2MOD/Utils/Utils.h"
#include "H2MOD/Variants/VariantMPGameEngine.h"

#include "Util/Hooks/Hook.h"
#include "XLive/xnet/IpManagement/XnIp.h"

#pragma region Done_Tweaks

typedef int(__cdecl * hookServ1_t)(HKEY, LPCWSTR);
hookServ1_t p_hookServ1;
int __cdecl LoadRegistrySettings(HKEY hKey, LPCWSTR lpSubKey) {
	char result = p_hookServ1(hKey, lpSubKey);
	addDebugText("Post Server Registry Read.");
	if (strlen(H2Config_dedi_server_playlist) > 0) {
		wchar_t* ServerPlaylist = Memory::GetAddress<wchar_t*>(0, 0x3B3704);
		swprintf(ServerPlaylist, 256, L"%hs", H2Config_dedi_server_playlist);
	}
	return result;
}

void __cdecl update_keyboard_buttons_state_hook(BYTE *a1, WORD *a2, BYTE *a3, bool a4, int a5)
{
	auto p_update_keyboard_buttons_state_hook = Memory::GetAddressRelative<decltype(&update_keyboard_buttons_state_hook)>(0x42E4C5);

	BYTE keyboardState[256] = {};
	if (!H2Config_disable_ingame_keyboard 
		&& GetKeyboardState(keyboardState))
	{
		for (int i = 0; i < 256; i++)
		{
			if (i != VK_SCROLL)
			{
				bool state = keyboardState[i] & 0x80;

				// these keys need to be queried using GetAsyncKeyState because the Window Processing (WndProc) may consume the keys
				if (i == VK_RSHIFT
					|| i == VK_LSHIFT
					|| i == VK_RCONTROL
					|| i == VK_LCONTROL
					|| i == VK_RMENU
					|| i == VK_LMENU)
				{
					SHORT asyncKeyState = GetAsyncKeyState(i);

					state = asyncKeyState & 0x8000;
				}

				p_update_keyboard_buttons_state_hook(&a1[i], &a2[i], &a3[i], state, a5);
			}
		}
	}
	else
	{
		for (int i = 0; i < 256; i++)
			if (i != VK_SCROLL)
				p_update_keyboard_buttons_state_hook(&a1[i], &a2[i], &a3[i], false, a5);
	}
}

typedef bool(__cdecl* session_change_party_visibility_t)(int);
session_change_party_visibility_t p_session_change_party_visibility;
bool __cdecl session_change_party_privacy_hook(int privacy) {
	bool result =
		p_session_change_party_visibility(privacy);
	if (result == 1 && privacy == 0) {
		pushHostLobby();
	}
	return result;
}

#pragma endregion

int(__cdecl* sub_20E1D8)(int, int, int, int, int, int);

int __cdecl sub_20E1D8_boot(int a1, int a2, int a3, int a4, int a5, int a6) {
	//a2 == 0x5 - system link lost connection
	if (a2 == 0xb9) {
		//boot them offline.
		XUserSignOut(0);
		UpdateMasterLoginStatus();
		H2Config_master_ip = inet_addr("127.0.0.1");
		H2Config_master_port_relay = 2001;
	}
	int result = sub_20E1D8(a1, a2, a3, a4, a5, a6);
	return result;
}

bool __cdecl is_remote_desktop()
{
	LOG_TRACE_FUNC("check disabled");
	return false;
}

class c_test_engine : public c_game_engine_base
{
	bool is_team_enemy(int team_a, int team_b)
	{
		return false;
	}

	void render_in_world(size_t arg1)
	{
	}

	float player_speed_multiplier(int arg1)
	{
		return 0.2f;
	}

};
c_test_engine g_test_engine;

void __stdcall biped_ground_mode_update_hook(int thisx,
	void* physics_output,
	void* physics_input,
	void* a4,
	int a5)
{
	// value converted from h2x tickrate
	static float edgeDropFactorConverted = 0.117f * 30.f;

	typedef void(__thiscall* biped_ground_mode_update_t)(int, void*, void*, void*, int, float);
	auto p_biped_ground_mode_update = Memory::GetAddress<biped_ground_mode_update_t>(0x1067F0, 0xF8B10);

	float edgeDropFactorPerTick = edgeDropFactorConverted * time_globals::get_seconds_per_tick();

	// push last parameter despite the function taking just 5 parameters
	p_biped_ground_mode_update(thisx, physics_output, physics_input, a4, a5, edgeDropFactorPerTick);

	// account for the last parameter that doesn't get handled by the actual function
	__asm add esp, 4;
}

__declspec(naked) void biped_ground_mode_update_to_stdcall()
{
	__asm
	{
		pop eax // pop return address
		push ecx // push ecx as first param
		push eax // push the return address back on stack
		jmp biped_ground_mode_update_hook
	}
}

// fixes the biped unit movement physics from applying too much movement, especially when edge-dropping by adjusting the default constant (0.117) value to tickrate
__declspec(naked) void update_biped_ground_mode_physics_constant()
{
#define _stack_pointer_offset 4Ch + 4h
#define _last_param_offset 14h
	__asm
	{
		movss xmm2, [esp + _stack_pointer_offset + _last_param_offset]
		ret
	}
#undef _stack_pointer_offset
#undef _last_param_offset
}

static DWORD (WINAPI* p_timeGetTime)() = timeGetTime;
DWORD WINAPI timeGetTime_hook()
{
	LARGE_INTEGER currentCounter, frequency;
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&currentCounter);

	currentCounter.QuadPart = currentCounter.QuadPart - _Shell::QPCGetStartupCounter().QuadPart;
	const long long timeNow = _Shell::QPCToTime(std::milli::den, currentCounter, frequency);
	// don't start exactly from 0 (60 min)
	return (DWORD)(PROCESS_SYSTEM_TIME_STARTUP_OFFSET + timeNow);
}
static_assert(std::is_same_v<decltype(timeGetTime), decltype(timeGetTime_hook)>, "Invalid timeGetTime_hook signature");

void DuplicateDataBlob(DATA_BLOB* pDataIn, DATA_BLOB* pDataOut)
{
	pDataOut->cbData = pDataIn->cbData;
	pDataOut->pbData = static_cast<BYTE*>(LocalAlloc(LMEM_FIXED, pDataIn->cbData));
	CopyMemory(pDataOut->pbData, pDataIn->pbData, pDataIn->cbData);
}

BOOL WINAPI CryptProtectDataHook(
	_In_       DATA_BLOB* pDataIn,
	_In_opt_   LPCWSTR                   szDataDescr,
	_In_opt_   DATA_BLOB* pOptionalEntropy,
	_Reserved_ PVOID                     pvReserved,
	_In_opt_   CRYPTPROTECT_PROMPTSTRUCT* pPromptStruct,
	_In_       DWORD                     dwFlags,
	_Out_      DATA_BLOB* pDataOut
)
{
	DuplicateDataBlob(pDataIn, pDataOut);

	return TRUE;
}

BOOL WINAPI CryptUnprotectDataHook(
	_In_       DATA_BLOB* pDataIn,
	_Out_opt_  LPWSTR* ppszDataDescr,
	_In_opt_   DATA_BLOB* pOptionalEntropy,
	_Reserved_ PVOID                     pvReserved,
	_In_opt_   CRYPTPROTECT_PROMPTSTRUCT* pPromptStruct,
	_In_       DWORD                     dwFlags,
	_Out_      DATA_BLOB* pDataOut
)
{
	if (CryptUnprotectData(pDataIn, ppszDataDescr, pOptionalEntropy, pvReserved, pPromptStruct, dwFlags, pDataOut) == FALSE) {
		DuplicateDataBlob(pDataIn, pDataOut); // if decrypting the data fails just assume it's unencrypted
	}

	return TRUE;
}

bool __cdecl filo_write__encrypted_data_hook(s_file_reference* file_ptr, DWORD nNumberOfBytesToWrite, LPVOID lpBuffer)
{
	DWORD file_size = GetFileSize(file_ptr->handle, NULL);

	if (file_size > nNumberOfBytesToWrite) // clear the file as unencrypted data is shorter then encrypted data.
		file_change_size(file_ptr, 0);
	return file_write(file_ptr, lpBuffer, nNumberOfBytesToWrite);
}

static BOOL (WINAPI* p_IsDebuggerPresent)() = IsDebuggerPresent;
BOOL WINAPI IsDebuggerPresent_hook() {
	return false;
}

void H2Tweaks::ApplyPatches() {
	addDebugText("Begin Startup Tweaks.");
	
	H2Tweaks::RefreshTogglexDelay();

	//TODO(Num005) crashes dedis
	//custom_game_engines::init();
	//custom_game_engines::register_engine(c_game_engine_types::unknown5, &g_test_engine, king_of_the_hill);

	DETOUR_BEGIN();

	DETOUR_ATTACH(p_timeGetTime, timeGetTime, timeGetTime_hook);

	if (Memory::IsDedicatedServer()) {
		p_hookServ1 = (hookServ1_t)DetourFunc(Memory::GetAddress<BYTE*>(0, 0x8EFA), (BYTE*)LoadRegistrySettings, 11);

		// set the additional pcr time
		WriteValue<BYTE>(Memory::GetAddress(0, 0xE590) + 2, H2Config_additional_pcr_time);

		// fix human turret variant setting not working on dedicated servers
		WriteValue<int>(Memory::GetAddress(0x0, 0x3557FC), 1);
	}
	else {//is client

		//Hook a function which changes the party privacy to detect if the lobby becomes open.
		//Problem is if you want to set it via mem poking, it won't push the lobby to the master automatically.
		//phookChangePrivacy = (thookChangePrivacy)DetourFunc((BYTE*)Memory::GetAddress(0x2153ce), (BYTE*)HookChangePrivacy, 11);

		bool IntroHQ = true;//clients should set on halo2.exe -highquality

		if (!H2Config_skip_intro && IntroHQ) {
			BYTE assmIntroHQ[] = { 0xEB };
			WriteBytes(Memory::GetAddress(0x221C29), assmIntroHQ, 1);
		}

		// Apply patches for the hud that need to be applied before WinMain is called
		hud_apply_pre_winmain_patches();

		// adds support for more monitor resolutions
		rasterizer_settings_apply_hooks();

		//Set the LAN Server List Ping Frequency (milliseconds).
		//WriteValue(Memory::GetAddress(0x001e9a89), 3000);
		//Set the LAN Server List Delete Entry After (milliseconds).
		//WriteValue(Memory::GetAddress(0x001e9b0a), 9000);

		//hook the gui popup for when the player is booted.
		sub_20E1D8 = Memory::GetAddress<int(__cdecl*)(int, int, int, int, int, int)>(0x20E1D8);
		PatchCall(Memory::GetAddress(0x21754C), &sub_20E1D8_boot);

		// patch to show game details menu in NETWORK serverlist too
		//NopFill(Memory::GetAddress(0x219D6D), 2);

		WriteJmpTo(Memory::GetAddress(0x39EA2), is_remote_desktop);

		// disable cloth debugging that writes to cloth.txt
		WriteValue<bool>(Memory::GetAddress(0x41F650), false);

		// prevent game from setting timeBeginPeriod/timeEndPeriod, when rendering loading screen
		NopFill(Memory::GetAddressRelative(0x66BA7C), 8);
		NopFill(Memory::GetAddressRelative(0x66A092), 8);

		// disable gamma correction by using D3D9::SetGammaRamp, TODO: implement a shader to take care of this, because D3D9::SetGammaRamp function seems to have 2 issues:
		// 1) it's very heavy on NVIDIA/Intel (not sure about AMD) GPUs (or there is something wrong with the drivers), causing stuttering on maps that override gamma (like Warlock, Turf, Backwash)
		// 2) it doesn't apply the gamma override when playing in windowed mode (thus why some people like using windowed mode, because it doesn't cause stuttering on these maps)

		// maybe we could find a way to use the gamma shader built in by converting the override gamma ramp to something that shader could understand
		BYTE SetGammaRampSkipBytes[] = { 0xE9, 0x94, 0x00, 0x00, 0x00, 0x90 };
		NopFill(Memory::GetAddressRelative(0x66192F), 15);
		WriteBytes(Memory::GetAddressRelative(0x66193E), SetGammaRampSkipBytes, sizeof(SetGammaRampSkipBytes));

		// nop a call to SetCursor(), to improve the FPS framedrops when hovering the mouse around in the main menus or where the cursor is used, mainly when using mice that use 1000 polling rate
		// it'll get called anyway by the D3D9Device::ShowCursor() API after
		//NopFill(Memory::GetAddressRelative(0x48A99C), 8);

		NopFill(Memory::GetAddressRelative(0x42FA8A), 3);
		NopFill(Memory::GetAddressRelative(0x42FAB9), 8);
		PatchCall(Memory::GetAddressRelative(0x42FAAB), update_keyboard_buttons_state_hook);

		// don't mess with the cursor during loading screen
		NopFill(Memory::GetAddressRelative(0x66BAEB), 5);

		// disable symbol to emoji translation when dealing with player name
		// works only in game for now, because the name in the pregame lobby uses c_text_widget
		// and it's harder to deal with
		NopFill(Memory::GetAddressRelative(0x46C7C7), 5);
		NopFill(Memory::GetAddressRelative(0x45C338), 5);
		NopFill(Memory::GetAddressRelative(0x473C61), 5);

		//TODO: turn on if you want to debug halo2.exe from start of process
		// DETOUR_ATTACH(p_IsDebuggerPresent, IsDebuggerPresent, IsDebuggerPresent_hook);

		// stop Hold to Zoom
		NopFill(Memory::GetAddress(0x9355C), 4);
	}

	// disables profiles/game saves encryption
	PatchWinAPICall(Memory::GetAddress(0x9B08A, 0x85F5E), CryptProtectDataHook);
	PatchWinAPICall(Memory::GetAddress(0x9AF9E, 0x352538), CryptUnprotectDataHook);
	PatchCall(Memory::GetAddress(0x9B09F, 0x85F73), filo_write__encrypted_data_hook);

	// fixes edge drop fast fall when using higher tickrates than 30
	PatchCall(Memory::GetAddressRelative(0x5082B4, 0x4FA5D4), biped_ground_mode_update_to_stdcall);
	Codecave(Memory::GetAddressRelative(0x506E23, 0x4F9143), update_biped_ground_mode_physics_constant, 3);

	// custom map hooks
	MapManager::ApplyPatches();

	DETOUR_COMMIT();

	addDebugText("End Startup Tweaks.");
}

void H2Tweaks::DisposePatches() {

}

void H2Tweaks::SetScreenRefreshRate() {

	if (Memory::IsDedicatedServer())
		return;

	{
		static bool refresh_redirected = false;
		if (!refresh_redirected) {
			WritePointer(Memory::GetAddress(0x25E869) + 3, &H2Config_refresh_rate);
			refresh_redirected = true;
		}
	}
}

void H2Tweaks::WarpFix(bool enable)
{
	if (Memory::IsDedicatedServer())
		return;

	//Improves warping issues 
	if (enable)
	{
		WriteValue<float>(Memory::GetAddress(0x4F958C), 4.0f);
		WriteValue<float>(Memory::GetAddress(0x4F9594), 10.0f);
	}
	else
	{
		WriteValue<float>(Memory::GetAddress(0x4F958C), 2.5f);
		WriteValue<float>(Memory::GetAddress(0x4F9594), 7.5f);
	}	
}

void H2Tweaks::RefreshTogglexDelay() {
	BYTE xDelayJMP[] = { 0x74 };
	if (!H2Config_xDelay)
		xDelayJMP[0] = 0xEB;
	WriteBytes(Memory::GetAddress(0x1c9d8e, 0x1a1316), xDelayJMP, 1);
}
