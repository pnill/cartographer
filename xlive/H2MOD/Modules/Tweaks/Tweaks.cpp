#include "stdafx.h"
#include "Tweaks.h"

#include "Blam/Engine/Game/game/game_time.h"
#include "Blam/Engine/Game/networking/messages/network_message_type_collection.h"
#include "Blam/Engine/Game/tag_files/files_windows.h"

#include "H2MOD/Modules/Accounts/AccountLogin.h"
#include "H2MOD/Modules/Shell/Config.h"
#include "H2MOD/Modules/CustomMenu/CustomMenu.h"
#include "H2MOD/Modules/CustomResolutions/CustomResolutions.h"
#include "H2MOD/Modules/HudElements/HudElements.h"
#include "H2MOD/Modules/MapManager/MapManager.h"
#include "H2MOD/Modules/OnScreenDebug/OnscreenDebug.h"
#include "H2MOD/Modules/UI/XboxLiveTaskProgress.h"
#include "H2MOD/Utils/Utils.h"
#include "H2MOD/Tags/TagInterface.h"
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

typedef char(__cdecl *hookChangePrivacy_t)(int);
hookChangePrivacy_t p_hookChangePrivacy;
char __cdecl HookChangePrivacy(int privacy) {
	char result =
		p_hookChangePrivacy(privacy);
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
		UpdateConnectionStatus();
		H2Config_master_ip = inet_addr("127.0.0.1");
		H2Config_master_port_relay = 2001;
	}
	int result = sub_20E1D8(a1, a2, a3, a4, a5, a6);
	return result;
}


typedef void(*Video_HUDSizeUpdate_t)(int hudSize, int safeArea);
Video_HUDSizeUpdate_t Video_HUDSizeUpdate_orig;

const float maxHUDTextScale = 1080 * 0.0010416667f; // you'd think we could use 1200 since that's the games normal max resolution, but seems 1200 still hides some text :(
const float maxUiScaleFonts = 1.049f; // >= 1.25 will make text disappear

void Video_HUDSizeUpdate_hook(int hudSize, int safeArea)
{
	Video_HUDSizeUpdate_orig(hudSize, safeArea);

	float* HUD_TextScale = Memory::GetAddress<float*>(0x464028); // gets set by the Video_HUDSizeUpdate_orig call above, affects HUD text and crosshair size
	if (*HUD_TextScale > maxHUDTextScale)
	{
		// textScale = resolution_height * 0.0010416667
		// but if it's too large the game won't render it some reason (max seems to be around 1.4, 1.3 when sword icon is in the text?)
		// lets just set it to the largest known good value for now, until the cause of large text sizes being hidden is figured out
		*HUD_TextScale = maxHUDTextScale;
	}

	// UI_Scale was updated just before we were called, so lets fix it real quick...
	float* UI_Scale = Memory::GetAddress<float*>(0xA3E424);
	if (*UI_Scale > maxUiScaleFonts)
	{
		// uiScale = resolution_height * 0.00083333335f
		// at higher resolutions text starts being cut off (pretty much any UI_Scale above 1 will result in text cut-off)
		// the sub_671B02_hook below fixes that by scaling the width of the element by UI_Scale (which the game doesn't do for some reason...)
		// however fonts will stop being rendered if the UI_Scale is too large (>= ~1.25)
		// so this'll make sure UI_Scale doesn't go above 1.249, but will result in the UI being drawn smaller
		*UI_Scale = maxUiScaleFonts;
	}
}

typedef int(__cdecl *sub_671B02_t)(ui_text_bounds* a1, ui_text_bounds* a2, int a3, int a4, int a5, float a6, int a7, int a8);
sub_671B02_t p_sub_671B02;

int __cdecl sub_671B02_hook(ui_text_bounds* a1, ui_text_bounds* a2, int a3, int a4, int a5, float a6, int a7, int a8)
{
	float UI_Scale = *Memory::GetAddress<float*>(0xA3E424);
	if (a2 && a2 != a1) // if a2 == a1 then this is a chapter name which scaling seems to break, so skip those ones
	{
		short width = a2->right - a2->left;
		a2->right = a2->left + (short)((float)width * UI_Scale); // scale width by UI_Scale, stops text elements from being cut-off when UI_Scale > 1
	}
	return p_sub_671B02(a1, a2, a3, a4, a5, a6, a7, a8);
}

bool __cdecl is_remote_desktop()
{
	LOG_TRACE_FUNC("check disabled");
	return false;
}

class test_engine : public c_game_engine_base
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
test_engine g_test_engine;

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

	float edgeDropFactorPerTick = edgeDropFactorConverted * time_globals::game_time_tick_length();

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
	return (DWORD)timeNow;
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

char filo_write__encrypted_data_hook(filo* file_ptr, DWORD nNumberOfBytesToWrite, LPVOID lpBuffer)
{
	DWORD file_size = GetFileSize(file_ptr->handle, NULL);

	if (file_size > nNumberOfBytesToWrite) // clear the file as unencrypted data is shorter then encrypted data.
		FiloInterface::change_size(file_ptr, 0);
	return FiloInterface::write(file_ptr, lpBuffer, nNumberOfBytesToWrite);
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

		if (H2Config_hiresfix) {
			// HUD text size fix for higher resolutions
			Video_HUDSizeUpdate_orig = (Video_HUDSizeUpdate_t)DetourFunc(Memory::GetAddress<BYTE*>(0x264A18), (BYTE*)Video_HUDSizeUpdate_hook, 7);

			// menu text fix for higher resolutions
			p_sub_671B02 = (sub_671B02_t)DetourFunc(Memory::GetAddress<BYTE*>(0x271B02), (BYTE*)sub_671B02_hook, 5);
		}

		// adds support for more monitor resolutions
		CustomResolution::Initialize();

		//Disables the ESRB warning (only occurs for English Language).
		//disables the one if no intro vid occurs.
		WriteValue<bool>(Memory::GetAddress(0x411030), 0);

		//disables the one after the intro video, by removing 0x40 flag from 0x7C6 bitmask
		WriteValue(Memory::GetAddress(0x39948 + 2), 0x7C6 & ~FLAG(6));

		//Set the LAN Server List Ping Frequency (milliseconds).
		//WriteValue(Memory::GetAddress(0x001e9a89), 3000);
		//Set the LAN Server List Delete Entry After (milliseconds).
		//WriteValue(Memory::GetAddress(0x001e9b0a), 9000);

		//hook the gui popup for when the player is booted.
		sub_20E1D8 = Memory::GetAddress<int(__cdecl*)(int, int, int, int, int, int)>(0x20E1D8);
		PatchCall(Memory::GetAddress(0x21754C), &sub_20E1D8_boot);

		HudElements::Init();

		H2Tweaks::SunflareFix();

		// patch to show game details menu in NETWORK serverlist too
		//NopFill(Memory::GetAddress(0x219D6D), 2);

		WriteJmpTo(Memory::GetAddress(0x39EA2), is_remote_desktop);

		//tags::on_map_load(fix_shaders_nvidia);
		tags::on_map_load(c_xbox_live_task_progress_menu::ApplyPatches);

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

static DWORD* get_scenario_global_address() {
	return Memory::GetAddress<DWORD*>(0x479e74);
}

static int get_scenario_volume_count() {
	int volume_count = *(int*)(*get_scenario_global_address() + 0x108);
	return volume_count;
}

static void kill_volume_disable(int volume_id) {
	auto p_kill_volume_disable = Memory::GetAddress<void(__cdecl*)(int volume_id)>(0xb3ab8);
	p_kill_volume_disable(volume_id);
}

void H2Tweaks::toggleKillVolumes(bool enable) {
	if (enable)
		return;

	//TODO 'bool enable'
	if (!Memory::IsDedicatedServer() && NetworkSession::LocalPeerIsSessionHost()) {
		for (int i = 0; i < get_scenario_volume_count(); i++) {
			kill_volume_disable(i);
		}
	}
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

void H2Tweaks::SunflareFix()
{
	if (Memory::IsDedicatedServer())
		return;

	//rasterizer_near_clip_distance <real>
	//Changed from game default of 0.06 to 0.0601
	WriteValue<float>(Memory::GetAddress(0x468150), 0.0601f);
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
