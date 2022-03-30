#include "stdafx.h"

#include "Tweaks.h"
#include "Blam\Engine\Game\GameTimeGlobals.h"
#include "Blam\FileSystem\FiloInterface.h"
#include "Blam\Engine\Networking\NetworkMessageTypeCollection.h"
#include "H2MOD\Modules\Accounts\AccountLogin.h"
#include "H2MOD\Modules\Config\Config.h"
#include "H2MOD\Modules\CustomMenu\CustomMenu.h"
#include "H2MOD\Modules\CustomResolutions\CustomResolutions.h"
#include "H2MOD\Modules\HudElements\HudElements.h"
#include "H2MOD\Modules\OnScreenDebug\OnscreenDebug.h"
#include "H2MOD\Modules\UI\XboxLiveTaskProgress.h"
#include "H2MOD\Modules\Utils\Utils.h"
#include "H2MOD\Tags\TagInterface.h"
#include "H2MOD\Variants\VariantMPGameEngine.h"
#include "Util\Hooks\Hook.h"
#include "XLive\xnet\IpManagement\XnIp.h"
#include "H2MOD\Modules\MapManager\MapManager.h"

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

HCURSOR __cdecl disable_mouse_cursor()
{
	auto p_disable_mouse_cursor = Memory::GetAddressRelative<decltype(&disable_mouse_cursor)>(0x40497B);
	
	static HCURSOR hCur;
	static FrequencyLimiter frqLimiter(150);

	if (frqLimiter.ShouldUpdate())
	{
		hCur = p_disable_mouse_cursor();
		return hCur;
	}
	else
	{
		return hCur;
	}
}

void __cdecl enable_custom_cursor()
{
	auto p_enable_custom_cursor = Memory::GetAddressRelative<decltype(&enable_custom_cursor)>(0x42EDC4);

	static FrequencyLimiter frqLimiter(150);
	
	if (frqLimiter.ShouldUpdate())
	{
		return p_enable_custom_cursor();
	}
	else
	{
		return;
	}
}

void __cdecl update_keyboard_buttons_state_hook(BYTE *a1, WORD *a2, BYTE *a3, bool a4, int a5)
{
	auto p_update_keyboard_buttons_state_hook = Memory::GetAddressRelative<decltype(&update_keyboard_buttons_state_hook)>(0x42E4C5);
	
	if (H2Config_disable_ingame_keyboard)
	{
		for (int i = 0; i < 256; i++)
			if (i != VK_SCROLL)
				p_update_keyboard_buttons_state_hook(&a1[i], &a2[i], &a3[i], false, a5);
		
		return;
	}

	BYTE keyboardState[256] = {};
	GetKeyboardState(keyboardState);

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


#pragma region init hook

#pragma region func_wrappers
void real_math_initialize()
{
	typedef int (__cdecl* real_math_initialize_t)();
	auto p_real_math_initialize_impl = Memory::GetAddress<real_math_initialize_t>(0x000340d7);
	p_real_math_initialize_impl();
}

void async_initialize()
{
	typedef int (__cdecl* async_initialize_t)();
	auto p_async_initialize_impl = Memory::GetAddress<async_initialize_t>(0x00032ce5);
	p_async_initialize_impl();
}

bool init_gfwl_gamestore()
{
	typedef char (__cdecl* init_gfwl_gamestore_t)();
	auto p_init_gfwl_gamestore_impl = Memory::GetAddress<init_gfwl_gamestore_t>(0x00202f3e);
	return p_init_gfwl_gamestore_impl();
}
// not sure if this is all it does
HANDLE init_data_checksum_info()
{
	typedef HANDLE (__cdecl* init_data_checksum_info_t)();
	auto p_init_data_checksum_info_impl = Memory::GetAddress<init_data_checksum_info_t>(0x000388d3);
	return p_init_data_checksum_info_impl();
}

// returns memory
void* runtime_state_init()
{
	typedef void* (__cdecl* runtime_state_init_t)();
	auto p_runtime_state_init_impl = Memory::GetAddress<runtime_state_init_t>(0x00037ed5);
	return p_runtime_state_init_impl();
}

void game_preferences_initialize()
{
	typedef void (__cdecl* game_preferences_initialize_t)();
	auto p_global_preferences_initialize_impl = Memory::GetAddress<game_preferences_initialize_t>(0x325FD);
	p_global_preferences_initialize_impl();
}

void font_initialize()
{
	typedef void (__cdecl* font_initialize_t)();
	auto p_font_initialize_impl = Memory::GetAddress<font_initialize_t>(0x00031dff);
	p_font_initialize_impl();
}

bool tag_files_open()
{
	typedef bool (__cdecl* tag_files_open_t)();
	auto p_tag_files_open_impl = Memory::GetAddress<tag_files_open_t>(0x30D58);
	return p_tag_files_open_impl();
}

void init_timing(int a1)
{
	typedef DWORD (__cdecl* init_timing_t)(int a1);
	auto p_init_timing_impl = Memory::GetAddress<init_timing_t>(0x37E39);
	p_init_timing_impl(a1);
}

void game_state_initialize()
{
	typedef void (__cdecl* game_state_initialize_t)();
	auto p_game_state_initialize = Memory::GetAddress<game_state_initialize_t>(0x00030aa6);
	p_game_state_initialize();
}

bool rasterizer_initialize()
{
	typedef char (__cdecl* rasterizer_initialize_t)();
	auto p_rasterizer_initialize = Memory::GetAddress<rasterizer_initialize_t>(0x00263359);
	return p_rasterizer_initialize();
}

bool input_initialize()
{
	typedef char (__cdecl* input_initialize_t)();
	auto p_input_initialize = Memory::GetAddress<input_initialize_t>(0x2FD23);
	return p_input_initialize();
}

void sound_initialize()
{
	typedef void (__cdecl* sound_initialize_t)();
	auto p_sound_initialize = Memory::GetAddress<sound_initialize_t>(0x2979E);
	return p_sound_initialize();
}

#pragma endregion

enum startup_flags : int
{
	windowed,
	disable_voice_chat,
	nosound,
	allow_d3d_ex_version, // allows the d3d ex version to be used instead
	disable_hardware_vertex_processing, // force hardware vertex processing off
	novsync,
	unk6, // squad browser/xlive/ui?
	nointro, // disables intro movie
	unk8, // some tag thing?
	unk9, // some tag thing?
	unk10, // some tag thing?
	unk11, // some tag thing?
	unk12, // some tag thing?
	unk13, // some tag thing?
	unk14, // some tag thing?
	custom_map_entry_test_map_name_instead_of_hash, // flag to test the map name instead of the hash of the custom map
	unk16,
	xbox_live_silver_account, // if true, disables 'gold-only' features, like quickmatch etc
	unk18, // fuzzer/automated testing? (sapien)
	ui_fast_test_no_start, // same as below but doesn't start a game
	ui_fast_test, // auto navigates the UI selecting the default option
	unk21, // player controls related, is checked when using a vehicle
	monitor_count,
	unk23,
	unk24,
	unk25, // something to do with game time?
	unk26,
	unk27, // network? value seems unused?
	high_quality, // forced sound reverb ignoring CPU score and disable forcing low graphical settings (sapien)

	count
};
static_assert(startup_flags::count == 29, "Bad flags count");

int flag_log_count[startup_flags::count];
BOOL __cdecl is_init_flag_set(startup_flags id)
{
	if (flag_log_count[id] < 10)
	{
		LOG_TRACE_GAME("is_init_flag_set() : flag {}", id);
		flag_log_count[id]++;
		if (flag_log_count[id] == 10)
			LOG_TRACE_GAME("is_init_flag_set() : flag {} logged to many times ignoring", id);
	}
	DWORD* init_flags_array = Memory::GetAddress<DWORD*>(0x46d820);
	return init_flags_array[id] != 0;
}

typedef int(*Video_HUDSizeUpdate_t)(int hudSize, int safeArea);
Video_HUDSizeUpdate_t Video_HUDSizeUpdate_orig;

const float maxHUDTextScale = 1080 * 0.0010416667f; // you'd think we could use 1200 since that's the games normal max resolution, but seems 1200 still hides some text :(
const float maxUiScaleFonts = 1.249f; // >= 1.25 will make text disappear

int Video_HUDSizeUpdate_hook(int hudSize, int safeArea)
{
	int retVal = Video_HUDSizeUpdate_orig(hudSize, safeArea);

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
		// at higher resolutions text starts being cut off (retty much any UI_Scale above 1 will result in text cut-off)
		// the sub_671B02_hook below fixes that by scaling the width of the element by UI_Scale (which the game doesn't do for some reason...)
		// however fonts will stop being rendered if the UI_Scale is too large (>= ~1.25)
		// so this'll make sure UI_Scale doesn't go above 1.249, but will result in the UI being drawn smaller
		*UI_Scale = maxUiScaleFonts;
	}

	return retVal;
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

const static int max_mointor_count = 9;
bool engine_basic_init()
{
	DWORD* flags_array = Memory::GetAddress<DWORD*>(0x46d820);
	ZeroMemory(flags_array, startup_flags::count * sizeof(DWORD)); // should be zero initalized anyways but the game does it

	H2Config_voice_chat = false;
	flags_array[startup_flags::nointro] = H2Config_skip_intro;

	void(__cdecl* fn_c000285fd)() = (void(__cdecl*)())Memory::GetAddress<void*>(0x000285fd);

	init_gfwl_gamestore();
	init_data_checksum_info();
	runtime_state_init();

	if (H2Config_hiresfix) {
		// HUD text size fix for higher resolutions
		Video_HUDSizeUpdate_orig = (Video_HUDSizeUpdate_t)DetourFunc(Memory::GetAddress<BYTE*>(0x264A18), (BYTE*)Video_HUDSizeUpdate_hook, 7);

		// menu text fix for higher resolutions
		p_sub_671B02 = (sub_671B02_t)DetourFunc(Memory::GetAddress<BYTE*>(0x271B02), (BYTE*)sub_671B02_hook, 5);
	}

	if (H2Config_d3dex) {
		flags_array[startup_flags::allow_d3d_ex_version] = 1;
	}

	int arg_count;
	wchar_t **cmd_line_args = LOG_CHECK(CommandLineToArgvW(GetCommandLineW(), &arg_count));
	if (cmd_line_args && arg_count > 1) {
		for (int i = 1; i < arg_count; i++) {
			wchar_t* cmd_line_arg = cmd_line_args[i];

			if (_wcsicmp(cmd_line_arg, L"-windowed") == 0) {
				flags_array[startup_flags::windowed] = 1;
			}
			else if (_wcsicmp(cmd_line_arg, L"-nosound") == 0) {
				flags_array[startup_flags::nosound] = 1;
				WriteValue(Memory::GetAddress(0x479EDC), 1);
			}
			else if (_wcsicmp(cmd_line_arg, L"-novsync") == 0) {
				flags_array[startup_flags::novsync] = 1;
			}
			else if (_wcsicmp(cmd_line_arg, L"-nointro") == 0) {
				flags_array[startup_flags::nointro] = 1;
			}
			else if (_wcsnicmp(cmd_line_arg, L"-monitor:", 9) == 0) {
				int monitor_id = _wtol(&cmd_line_arg[9]);
				flags_array[startup_flags::monitor_count] = min(max(0, monitor_id), max_mointor_count);
			}
			else if (_wcsicmp(cmd_line_arg, L"-highquality") == 0) {
				flags_array[startup_flags::high_quality] = 1;
			}
			else if (_wcsicmp(cmd_line_arg, L"-depthbiasfix") == 0)
			{
				// Fixes issue #118
			    /* g_depth_bias always NULL rather than taking any value from
			    shader tag before calling g_D3DDevice->SetRenderStatus(D3DRS_DEPTHBIAS, g_depth_bias); */
				NopFill(Memory::GetAddress(0x269FD5), 8);
			}
#if COMPILE_WITH_VOICE
			else if (_wcsicmp(cmd_line_arg, L"-voicechat") == 0)
			{
				flags_array[startup_flags::disable_voice_chat] = 0;
				H2Config_voice_chat = true;
			}
#endif
#ifdef _DEBUG
			else if (_wcsnicmp(cmd_line_arg, L"-dev_flag:", 10) == 0) {
				int flag_id = _wtol(&cmd_line_arg[10]);
				flags_array[min(max(0, flag_id), startup_flags::count - 1)] = 1;
			}
#endif
		}
	}
	LocalFree(cmd_line_args);

	if (flags_array[startup_flags::unk26])
		init_timing(1000 * flags_array[startup_flags::unk26]);
	real_math_initialize();
	async_initialize();
	game_preferences_initialize();

	s_network_observer::ResetNetworkPreferences();

	font_initialize();

	if (!LOG_CHECK(tag_files_open()))
		return false;
	
	game_state_initialize();

	// modifies esi need to check what the caller sets that too
	// Update 1/13/2022:
	// initializes some interface that's used to download the network config from the bungie's website
	// but a stub in release
	void(__cdecl*network_configuration_initialize)() = (void(__cdecl*)())(Memory::GetAddress(0x001a9de6));
	network_configuration_initialize();

	if (!LOG_CHECK(rasterizer_initialize()))
		return false;

	// does some weird stuff with sound, might setup volume
	fn_c000285fd();

	input_initialize();
	sound_initialize();

	struct FakePBuffer {
		HANDLE id;
		DWORD dwSize;
		DWORD magic;
		LPBYTE pbData;
	};
	//extern LONG WINAPI XLivePBufferAllocate(DWORD size, FakePBuffer **pBuffer);
	//extern DWORD WINAPI XLivePBufferSetByte(FakePBuffer * pBuffer, DWORD offset, BYTE value);
	LONG(__stdcall* XLivePBufferAllocate)(DWORD size, FakePBuffer **pBuffer) = (LONG(__stdcall*)(DWORD, FakePBuffer**))Memory::GetAddress(0xe886);
	DWORD(__stdcall* XLivePBufferSetByte)(FakePBuffer * pBuffer, DWORD offset, BYTE value) = (DWORD(__stdcall*)(FakePBuffer*, DWORD, BYTE))Memory::GetAddress(0xe880);
	FakePBuffer** var_c00479e78 = Memory::GetAddress<FakePBuffer**>(0x00479e78);
	XLivePBufferAllocate(2, var_c00479e78);
	XLivePBufferSetByte(*var_c00479e78, 0, 0);
	XLivePBufferSetByte(*var_c00479e78, 1, 0);

	//SLDLInitialize

	//SLDLOpen
	//SLDLConsumeRight
	//SLDLClose
	// This call would disable the multiplayer buttons in the mainmenu. Likely setup this way from SLDL.
	//XLivePBufferSetByte((FakePBuffer*)var_c00479e78, 0, 1);

	return true;
}

#pragma region func_wrappers
bool InitPCCInfo()
{
	typedef bool (__cdecl* InitPCCInfo_t)();
	auto p_InitPCCInfo = Memory::GetAddress<InitPCCInfo_t>(0x260DDD);
	return p_InitPCCInfo();
}

void run_main_loop()
{
	typedef int (__cdecl* run_main_loop_t)();
	auto p_run_main_loop = Memory::GetAddress<run_main_loop_t>(0x39E2C);
	p_run_main_loop();
}

void main_engine_dispose()
{
	typedef int (__cdecl* main_engine_dispose_t)();
	auto p_main_engine_dispose = Memory::GetAddress<main_engine_dispose_t>(0x48A9);
	p_main_engine_dispose();
}

void show_error_message_by_id(int id)
{
	typedef void (__cdecl* show_error_message_by_id_t)(int id);
	auto p_show_error_message_by_id = Memory::GetAddress<show_error_message_by_id_t>(0x4A2E);
	p_show_error_message_by_id(id);
}
#pragma endregion

void show_fatal_error(int error_id)
{
	LOG_TRACE_FUNC("error_id : {}", error_id);

	auto destory_window = [](HWND handle) {
		if (handle)
			DestroyWindow(handle);
	};

	HWND hWnd = *Memory::GetAddress<HWND*>(0x46D9C4);
	HWND d3d_window = *Memory::GetAddress<HWND*>(0x46D9C8); // not sure what this window is actual for, used in IDirect3DDevice9::Present
	destory_window(hWnd);
	destory_window(d3d_window);
	show_error_message_by_id(error_id);
}

int WINAPI WinMain_Halo2(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	// set args
	WriteValue(Memory::GetAddress(0x46D9BC), lpCmdLine); // command_line_args
	WriteValue(Memory::GetAddress(0x46D9C0), hInstance); // g_instance
	WriteValue(Memory::GetAddress(0x46D9CC), nShowCmd); // g_CmdShow

	// window setup
	wcscpy_s(Memory::GetAddress<wchar_t*>(0x46D9D4), 0x40, L"halo"); // ClassName
	wcscpy_s(Memory::GetAddress<wchar_t*>(0x46DA54), 0x40, L"Halo 2 - Project Cartographer"); // WindowName
	WNDPROC g_WndProc = Memory::GetAddress<WNDPROC>(0x790E);
	WriteValue(Memory::GetAddress(0x46D9D0), g_WndProc); // g_WndProc_ptr

	if (!LOG_CHECK(InitPCCInfo()))
	{
		LOG_TRACE_FUNC("Failed to get PCC info / insufficient system resources");
		std::thread( [=]{ MessageBoxA(NULL, "Failed to get compatibility info.", "PCC Error", S_OK); }).detach();

		show_error_message_by_id(108);
		// todo: load some default values here?
	}
	// mouse cursor setup
	HCURSOR cursor = LOG_CHECK(LoadCursor(NULL, MAKEINTRESOURCE(0x7F00)));
	WriteValue(Memory::GetAddress(0x46D9B8), cursor); // g_hCursor

	// mess around with xlive (not calling XLiveInitialize etc)
	WriteValue<BYTE>(Memory::GetAddress(0x4FAD98), 1);

	// intialize some basic game subsystems
	if (LOG_CHECK(engine_basic_init()))
	{
		run_main_loop(); // actually run game
		main_engine_dispose(); // cleanup
	} else
	{
		LOG_TRACE_FUNC("Engine startup failed!");
		show_fatal_error(108);
		return 1;
	}

	int g_fatal_error_id = *Memory::GetAddress<int*>(0x46DAD4);
	if (g_fatal_error_id) // check if the game exited cleanly
	{
		show_fatal_error(g_fatal_error_id);
		return 1;
	} else
	{
		return 0;
	}
}
#pragma endregion

char is_remote_desktop()
{
	LOG_TRACE_FUNC("check disabled");
	return 0;
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

// fixes the biped unit movement physics from applying too much movement, especially when edge-dropping by adjusting the default constant (0.117) value to tickrate
__declspec(naked) void update_biped_ground_mode_physics_constant()
{
	static float edgeDropFactorConverted = 0.117f * 30.f; // value converted from h2x tickrate

	__asm
	{
		PUSHAD // preserve registers on stack, until we are done
		PUSHFD
		call time_globals::get // get the game time globals pointer in eax register
		movss xmm2, edgeDropFactorConverted // multiply the edge drop value 
		mulss xmm2, dword ptr[eax + 0x4] // multiply by seconds per tick or game tick length
		POPFD // restore registers from stack
		POPAD
		ret
	}
}

static LARGE_INTEGER startupCounter;
DWORD WINAPI timeGetTime_hook()
{
	LARGE_INTEGER currentCounter, frequency;
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&currentCounter);

	// algorithm used from Microsoft's high resolution clock now() implementation
	const long long counterSinceProgramEpoch = currentCounter.QuadPart - startupCounter.QuadPart;
	const long long _Whole = (counterSinceProgramEpoch / frequency.QuadPart) * 1000;
	const long long _Part = (counterSinceProgramEpoch % frequency.QuadPart) * 1000 / frequency.QuadPart;

	return _Whole + _Part;
}
static_assert(std::is_same<decltype(timeGetTime), decltype(timeGetTime_hook)>::value, "Invalid timeGetTime_hook signature");

void initializeTimeHooks()
{
	QueryPerformanceCounter(&startupCounter);
	WritePointer(Memory::GetAddressRelative(0x79B568, 0x752540), (void*)timeGetTime_hook);
}

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

void InitH2Tweaks() {

	RefreshTogglexDelay();

	addDebugText("Begin Startup Tweaks.");

	//TODO(Num005) crashes dedis
	//custom_game_engines::init();
	//custom_game_engines::register_engine(c_game_engine_types::unknown5, &g_test_engine, king_of_the_hill);

	initializeTimeHooks();
	mapManager->ApplyHooks();

	if (Memory::isDedicatedServer()) {
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

		// adds support for more monitor resolutions
		CustomResolution::Initialize();

		//Disables the ESRB warning (only occurs for English Language).
		//disables the one if no intro vid occurs.
		WriteValue<BYTE>(Memory::GetAddress(0x411030), 0);

		//disables the one after the intro video, by removing 0x40 flag from 0x7C6 bitmask
		WriteValue(Memory::GetAddress(0x39948 + 2), 0x7C6 & ~FLAG(6));

		//Set the LAN Server List Ping Frequency (milliseconds).
		//WriteValue(Memory::GetAddress(0x001e9a89), 3000);
		//Set the LAN Server List Delete Entry After (milliseconds).
		//WriteValue(Memory::GetAddress(0x001e9b0a), 9000);

		//hook the gui popup for when the player is booted.
		sub_20E1D8 = Memory::GetAddress<int(__cdecl*)(int, int, int, int, int, int)>(0x20E1D8);
		PatchCall(Memory::GetAddress(0x21754C), &sub_20E1D8_boot);

		WriteJmpTo(Memory::GetAddress(0x4544), is_init_flag_set);

		HudElements::Init();

		H2Tweaks::SunflareFix();

		// patch to show game details menu in NETWORK serverlist too
		//NopFill(Memory::GetAddress(0x219D6D), 2);

		WriteJmpTo(Memory::GetAddress(0x7E43), WinMain_Halo2);
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
		BYTE SetGammaRampSkipBytes[] = { 0x90, 0x90, 0x90, 0xE9, 0x94, 0x00, 0x00, 0x00, 0x90 };
		WriteBytes(Memory::GetAddressRelative(0x66193B), SetGammaRampSkipBytes, sizeof(SetGammaRampSkipBytes));

		// nop a call to SetCursor(), to improve the FPS framedrops when hovering the mouse around in the main menus or where the cursor is used, mainly when using mice that use 1000 polling rate
		// it'll get called anyway by the D3D9Device::ShowCursor() API after
		//NopFill(Memory::GetAddressRelative(0x48A99C), 8);

		PatchCall(Memory::GetAddressRelative(0x407BFA), disable_mouse_cursor);
		PatchCall(Memory::GetAddressRelative(0x407BE6), enable_custom_cursor);

		NopFill(Memory::GetAddressRelative(0x42FABF), 2);
		NopFill(Memory::GetAddressRelative(0x42FA8A), 3);
		PatchCall(Memory::GetAddressRelative(0x42FAAB), update_keyboard_buttons_state_hook);

		// don't mess with the cursor during loading screen
		NopFill(Memory::GetAddressRelative(0x66BAEB), 5);

		// disable symbol to emoji translation when dealing with player name
		// works only in game for now, because the name in the pregame lobby uses c_text_widget
		// and it's harder to deal with
		NopFill(Memory::GetAddressRelative(0x46C7C7), 5);
		NopFill(Memory::GetAddressRelative(0x45C338), 5);
		NopFill(Memory::GetAddressRelative(0x473C61), 5);
	}

	// disables profiles/game saves encryption
	PatchWinAPICall(Memory::GetAddress(0x9B08A, 0x85F5E), CryptProtectDataHook);
	PatchWinAPICall(Memory::GetAddress(0x9AF9E, 0x352538), CryptUnprotectDataHook);
	PatchCall(Memory::GetAddress(0x9B09F, 0x85F73), filo_write__encrypted_data_hook);

	// fixes edge drop fast fall when using higher tickrates than 30
	Codecave(Memory::GetAddressRelative(0x506E23, 0x4F9143), update_biped_ground_mode_physics_constant, 3);

	addDebugText("End Startup Tweaks.");
}

void DeinitH2Tweaks() {

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
	if (!Memory::isDedicatedServer() && NetworkSession::LocalPeerIsSessionHost()) {
		for (int i = 0; i < get_scenario_volume_count(); i++) {
			kill_volume_disable(i);
		}
	}
}

void H2Tweaks::setHz() {

	if (Memory::isDedicatedServer())
		return;

	{
		static bool refresh_redirected = false;
		if (!refresh_redirected) {
			WritePointer(Memory::GetAddress(0x25E869) + 3, &H2Config_refresh_rate);
			refresh_redirected = true;
		}
	}
}

char ret_0() {
	return 0; //for 60 fps cinematics
}

void H2Tweaks::toggleUncappedCampaignCinematics(bool toggle) {
	if (Memory::isDedicatedServer())
		return;

	typedef char(__cdecl *is_cutscene_fps_cap_t)();
	auto p_is_cutscene_fps_cap = Memory::GetAddress<is_cutscene_fps_cap_t>(0x3A938);

	//60 fps cinematics enable
	PatchCall(Memory::GetAddress(0x97774), toggle ? ret_0 : p_is_cutscene_fps_cap);
	PatchCall(Memory::GetAddress(0x7C378), toggle ? ret_0 : p_is_cutscene_fps_cap);
}

void H2Tweaks::toggleAiMp(bool toggle) {
	WriteValue<BYTE>(Memory::GetAddress(0x30E684, 0x2B93F4), toggle ? JMP_OP_CODE : JNZ_OP_CODE);
}

void H2Tweaks::SunflareFix()
{
	if (Memory::isDedicatedServer())
		return;

	//rasterizer_near_clip_distance <real>
	//Changed from game default of 0.06 to 0.0601
	WriteValue<float>(Memory::GetAddress(0x468150), 0.0601f);
}

void H2Tweaks::WarpFix(bool enable)
{
	if (Memory::isDedicatedServer())
		return;

	//Improves warping issues 
	if (enable)
	{
		WriteValue<float>(Memory::GetAddress(0x4F958C), 4.0);
		WriteValue<float>(Memory::GetAddress(0x4F9594), 10.0);
	}
	else
	{
		WriteValue<float>(Memory::GetAddress(0x4F958C), 2.5);
		WriteValue<float>(Memory::GetAddress(0x4F9594), 7.5);
	}	
}
