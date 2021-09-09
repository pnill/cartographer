
#include "H2MOD\Modules\Config\Config.h"
#include "H2MOD\Modules\CustomMenu\CustomMenu.h"
#include "H2MOD\Modules\OnScreenDebug\OnScreenDebug.h"
#include "H2MOD\Modules\Tweaks\Tweaks.h"
#include "H2MOD\Modules\Utils\Utils.h"
#include "H2MOD\Variants\VariantMPGameEngine.h"
#include "XLive\xnet\IpManagement\XnIp.h"
#include "H2MOD\Modules\Accounts\AccountLogin.h"
#include "H2MOD\Modules\UI\XboxLiveTaskProgress.h"
#include "..\CustomResolutions\CustomResolutions.h"
#include "H2MOD/Modules/HudElements/HudElements.h"

#include "H2MOD\Tags\TagInterface.h"
#include "H2MOD/Modules/Networking/Networking.h"
#include "Util\Hooks\Hook.h"

#include "Blam\Engine\Game\GameTimeGlobals.h"

#pragma region Done_Tweaks

typedef int(__cdecl *thookServ1)(HKEY, LPCWSTR);
thookServ1 phookServ1;
int __cdecl LoadRegistrySettings(HKEY hKey, LPCWSTR lpSubKey) {
	char result = phookServ1(hKey, lpSubKey);
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

	if (frqLimiter.shouldUpdate())
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
	
	if (frqLimiter.shouldUpdate())
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

typedef char(__cdecl *thookChangePrivacy)(int);
thookChangePrivacy phookChangePrivacy;
char __cdecl HookChangePrivacy(int privacy) {
	char result =
		phookChangePrivacy(privacy);
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
	typedef int (real_math_initialize)();
	auto real_math_initialize_impl = Memory::GetAddress<real_math_initialize*>(0x000340d7);
	real_math_initialize_impl();
}

void async_initialize()
{
	typedef int (async_initialize)();
	auto async_initialize_impl = Memory::GetAddress<async_initialize*>(0x00032ce5);
	async_initialize_impl();
}

bool init_gfwl_gamestore()
{
	typedef char (init_gfwl_gamestore)();
	auto init_gfwl_gamestore_impl = Memory::GetAddress<init_gfwl_gamestore*>(0x00202f3e);
	return init_gfwl_gamestore_impl();
}
// not sure if this is all it does
HANDLE init_data_checksum_info()
{
	typedef HANDLE init_data_checksum_info();
	auto init_data_checksum_info_impl = Memory::GetAddress<init_data_checksum_info*>(0x000388d3);
	return init_data_checksum_info_impl();
}

// returns memory
void *runtime_state_init()
{
	typedef void *runtime_state_init();
	auto runtime_state_init_impl = Memory::GetAddress<runtime_state_init*>(0x00037ed5);
	return runtime_state_init_impl();
}

void game_preferences_initialize()
{
	typedef void game_preferences_initialize();
	auto global_preferences_initialize_impl = Memory::GetAddress<game_preferences_initialize*>(0x325FD);
	global_preferences_initialize_impl();
}

void font_initialize()
{
	typedef void __cdecl font_initialize();
	auto font_initialize_impl = Memory::GetAddress<font_initialize*>(0x00031dff);
	font_initialize_impl();
}

bool tag_files_open()
{
	typedef bool tag_files_open();
	auto tag_files_open_impl = Memory::GetAddress<tag_files_open*>(0x30D58);
	return tag_files_open_impl();
}

void init_timing(int a1)
{
	typedef DWORD (__cdecl init_timing)(int a1);
	auto init_timing_impl = Memory::GetAddress<init_timing*>(0x37E39);
	init_timing_impl(a1);
}

void game_state_initialize()
{
	typedef void (__cdecl* game_state_initialize)();
	auto game_state_initialize_impl = Memory::GetAddress<game_state_initialize>(0x00030aa6);
	game_state_initialize_impl();
}

bool rasterizer_initialize()
{
	typedef char rasterizer_initialize();
	auto rasterizer_initialize_impl = Memory::GetAddress<rasterizer_initialize*>(0x00263359);
	return rasterizer_initialize_impl();
}

bool input_initialize()
{
	typedef char input_initialize();
	auto input_initialize_impl = Memory::GetAddress<input_initialize*>(0x2FD23);
	return input_initialize_impl();
}

void sound_initialize()
{
	typedef void sound_initialize();
	auto sound_initialize_impl = Memory::GetAddress<sound_initialize*>(0x2979E);
	return sound_initialize_impl();
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
	unk15, // seen near xlive init code
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

typedef int(*Video_HUDSizeUpdate_ptr)(int hudSize, int safeArea);
Video_HUDSizeUpdate_ptr Video_HUDSizeUpdate_orig;

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

struct ui_text_bounds
{
	short top;
	short left;
	short bottom;
	short right;
};

typedef int(__cdecl *sub_671B02_ptr)(ui_text_bounds* a1, ui_text_bounds* a2, int a3, int a4, int a5, float a6, int a7, int a8);
sub_671B02_ptr sub_671B02_orig;

int __cdecl sub_671B02_hook(ui_text_bounds* a1, ui_text_bounds* a2, int a3, int a4, int a5, float a6, int a7, int a8)
{
	float UI_Scale = *Memory::GetAddress<float*>(0xA3E424);
	if (a2 && a2 != a1) // if a2 == a1 then this is a chapter name which scaling seems to break, so skip those ones
	{
		short width = a2->right - a2->left;
		a2->right = a2->left + (short)((float)width * UI_Scale); // scale width by UI_Scale, stops text elements from being cut-off when UI_Scale > 1
	}
	return sub_671B02_orig(a1, a2, a3, a4, a5, a6, a7, a8);
}

const static int max_mointor_count = 9;
bool engine_basic_init()
{
	DWORD* flags_array = Memory::GetAddress<DWORD*>(0x46d820);
	SecureZeroMemory(flags_array, startup_flags::count * sizeof(DWORD)); // should be zero initalized anyways but the game does it

	H2Config_voice_chat = false;
	flags_array[startup_flags::nointro] = H2Config_skip_intro;

	HANDLE(*fn_c000285fd)() = (HANDLE(*)())Memory::GetAddress<void*>(0x000285fd);

	init_gfwl_gamestore();
	init_data_checksum_info();
	runtime_state_init();

	if (H2Config_hiresfix) {
		// HUD text size fix for higher resolutions
		Video_HUDSizeUpdate_orig = (Video_HUDSizeUpdate_ptr)DetourFunc(Memory::GetAddress<BYTE*>(0x264A18), (BYTE*)Video_HUDSizeUpdate_hook, 7);

		// menu text fix for higher resolutions
		sub_671B02_orig = (sub_671B02_ptr)DetourFunc(Memory::GetAddress<BYTE*>(0x271B02), (BYTE*)sub_671B02_hook, 5);
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

	network_observer::ResetNetworkPreferences();

	font_initialize();

	if (!LOG_CHECK(tag_files_open()))
		return false;
	
	game_state_initialize();

	// modifies esi need to check what the caller sets that too
	//char(*fn_c001a9de6)() = (char(*)())(Memory::GetAddress(0x001a9de6));
	//char result_c001a9de6 = fn_c001a9de6();
	if (!LOG_CHECK(rasterizer_initialize()))
		return false;

	input_initialize();
	// not needed but bungie did it so there might be some reason
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

	// does some weird stuff with sound, might setup volume
	HANDLE result_c000285fd = fn_c000285fd();

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
	typedef bool __cdecl InitPCCInfo();
	auto InitPCCInfoImpl = Memory::GetAddress<InitPCCInfo*>(0x260DDD);
	return InitPCCInfoImpl();
}

void run_main_loop()
{
	typedef int __cdecl run_main_loop();
	auto run_main_loop_impl = Memory::GetAddress<run_main_loop*>(0x39E2C);
	run_main_loop_impl();
}

void main_engine_dispose()
{
	typedef int main_engine_dispose();
	auto main_engine_dispose_impl = Memory::GetAddress<main_engine_dispose*>(0x48A9);
	main_engine_dispose_impl();
}

void show_error_message_by_id(int id)
{
	typedef void __cdecl show_error_message_by_id(int id);
	auto show_error_message_by_id_impl = Memory::GetAddress<show_error_message_by_id*>(0x4A2E);
	show_error_message_by_id_impl(id);
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

int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
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

typedef char(__stdcall *tfn_c0024eeef)(DWORD*, int, int);
tfn_c0024eeef pfn_c0024eeef;
char __stdcall fn_c0024eeef(DWORD* thisptr, int a2, int a3)//__thiscall
{
	//char result = pfn_c0024eeef(thisptr, a2, a3);
	//return result;

	char(__thiscall* fn_c002139f8)(DWORD*, int, int, int, int*, int) = (char(__thiscall*)(DWORD*, int, int, int, int*, int))Memory::GetAddress(0x002139f8);

	int label_list[16];
	label_list[0] = 0;
	label_list[1] = 0xA0005D1;//"Change Map..."
	label_list[2] = 1;
	label_list[3] = 0xE0005D2;//"Change Rules..."
	label_list[4] = 2;
	label_list[5] = 0xD000428;//"Quick Options..."
	label_list[6] = 3;
	label_list[7] = 0x1000095D;//"Party Management..."
	label_list[8] = 4;
	label_list[9] = 0x0E0005D9;//"Switch To: Co-Op Game"
	label_list[10] = 5;
	label_list[11] = 0x150005D8;//"Switch To: Custom Game"
	label_list[12] = 6;
	label_list[13] = 0x130005DA;//"Switch To: Matchmaking"
	label_list[14] = 7;
	label_list[15] = 0xD0005D6;//"Change Match Settings..."

	//label_list[8] = ?;
	//label_list[9] = 0x120005D7;//"Switch To: Playlist"
	//label_list[10] = ?;
	//label_list[11] = 0x150005d8;//"Switch To: Custom Game"

	return fn_c002139f8(thisptr, a2, a3, 0, label_list, 8);
}

typedef int(__stdcall *tfn_c0024fa19)(DWORD*, int, int*);
tfn_c0024fa19 pfn_c0024fa19;
int __stdcall fn_c0024fa19(DWORD* thisptr, int a2, int* a3)//__thiscall
{
	//int result = pfn_c0024fa19(thisptr, a2, a3);
	//return result;

	auto fn_c0024f9a1 = Memory::GetAddress<int(__stdcall*)(int)>(0x24f9a1);
	auto fn_c0024f9dd = Memory::GetAddress<int(__stdcall*)(int)>(0x24f9dd);
	auto fn_c0024ef79 = Memory::GetAddress<int(__stdcall*)(int)>(0x24ef79);
	auto fn_c0024f5fd = Memory::GetAddress<int(__stdcall*)(int)>(0x24f5fd);
	auto fn_c0024f015 = Memory::GetAddress<int(__stdcall*)(int)>(0x24f015);
	auto fn_c0024f676 = Memory::GetAddress<int(__stdcall*)(int)>(0x24f676);
	auto fn_c0024f68a = Memory::GetAddress<int(__stdcall*)(int)>(0x24f68a);

	int result = *a3;
	if (*a3 != -1)
	{
		result = *(signed __int16 *)(*(DWORD *)(thisptr[28] + 68) + 4 * (unsigned __int16)result + 2);
		switch (result)
		{
		case 0:
			result = fn_c0024f9a1(a2);
			break;
		case 1:
			result = fn_c0024f9dd(a2);
			break;
		case 2:
			result = fn_c0024ef79(a2);
			break;
		case 3:
			result = fn_c0024f5fd(a2);//party management
			break;
		case 4:
			GSCustomMenuCall_AdvLobbySettings3();
			break;
		case 5:
			result = fn_c0024f015(a2);
			break;
		case 6:
			result = fn_c0024f676(a2);
			break;
		case 7:
			result = fn_c0024f68a(a2);
			break;
		default:
			return result;
		}
	}
	return result;
}

typedef DWORD*(__stdcall *tfn_c0024fabc)(DWORD*, int);
tfn_c0024fabc pfn_c0024fabc;
DWORD* __stdcall fn_c0024fabc(DWORD* thisptr, int a2)//__thiscall
{
	//DWORD* result = pfn_c0024fabc(thisptr, a2);
	//return result;

	DWORD* var_c003d9254 = Memory::GetAddress<DWORD*>(0x3d9254);
	DWORD* var_c003d9188 = Memory::GetAddress<DWORD*>(0x3d9188);

	DWORD*(__thiscall* fn_c00213b1c)(DWORD* thisptr, int) = (DWORD*(__thiscall*)(DWORD*, int))Memory::GetAddress(0x00213b1c);
	int(__thiscall* fn_c0000a551)(DWORD* thisptr) = (int(__thiscall*)(DWORD*))Memory::GetAddress(0x0000a551);
	DWORD*(__thiscall* fn_c0021ffc9)(DWORD* thisptr) = (DWORD*(__thiscall*)(DWORD*))(Memory::GetAddress(0x0021ffc9));
	void(__stdcall* fn_c0028870b)(int, int, int, DWORD*(__thiscall*)(DWORD*), int(__thiscall*)(DWORD*)) = (void(__stdcall*)(int, int, int, DWORD*(__thiscall*)(DWORD*), int(__thiscall*)(DWORD*)))Memory::GetAddress(0x0028870b);
	DWORD*(__thiscall* fn_c002113c6)(DWORD* thisptr) = (DWORD*(__thiscall*)(DWORD*))Memory::GetAddress(0x002113c6);
	int(__thiscall* fn_c0024fa19)(DWORD* thisptr, int, int*) = (int(__thiscall*)(DWORD*, int, int*))Memory::GetAddress(0x0024fa19);
	int(*fn_c00215ea9)() = (int(*)())Memory::GetAddress(0x00215ea9);
	int(__cdecl* fn_c0020d1fd)(char*, int numberOfButtons, int) = (int(__cdecl*)(char*, int, int))Memory::GetAddress(0x0020d1fd);
	int(__cdecl* fn_c00066b33)(int) = (int(__cdecl*)(int))Memory::GetAddress(0x00066b33);
	int(__cdecl* fn_c000667a0)(int) = (int(__cdecl*)(int))Memory::GetAddress(0x000667a0);
	int(*fn_c002152b0)() = (int(*)())Memory::GetAddress(0x002152b0);
	int(*fn_c0021525a)() = (int(*)())Memory::GetAddress(0x0021525a);
	int(__thiscall* fn_c002113d3)(DWORD* thisptr, DWORD*) = (int(__thiscall*)(DWORD*, DWORD*))Memory::GetAddress(0x002113d3);

	DWORD* v2 = thisptr;
	fn_c00213b1c(thisptr, a2);
	//*v2 = &c_squad_settings_list::`vftable';
	v2[0] = (DWORD)var_c003d9254;
	//*v2 = (DWORD)((BYTE*)Memory::GetAddress(0x003d9254);
	fn_c0028870b((int)(v2 + 44), 132, 7, fn_c0021ffc9, fn_c0000a551);
	fn_c002113c6(v2 + 276);
	//v2[275] = &c_slot2<c_squad_settings_list, s_event_record *, long>::`vftable';
	v2[275] = (DWORD)var_c003d9188;
	//v2[275] = (DWORD)((BYTE*)Memory::GetAddress(0x003d9188);
	v2[279] = (DWORD)v2;
	v2[280] = (DWORD)fn_c0024fa19;
	int v3 = fn_c00215ea9();
	int v4 = fn_c0020d1fd("squad setting list", 8, 4);
	v2[28] = v4;
	fn_c00066b33(v4);
	*((BYTE *)v2 + 1124) = 1;
	switch (v3)
	{
	case 1:
	case 3:
		*(WORD *)(*(DWORD *)(v2[28] + 68) + 4 * (unsigned __int16)fn_c000667a0(v2[28]) + 2) = 0;
		*(WORD *)(*(DWORD *)(v2[28] + 68) + 4 * (unsigned __int16)fn_c000667a0(v2[28]) + 2) = 1;
		*(WORD *)(*(DWORD *)(v2[28] + 68) + 4 * (unsigned __int16)fn_c000667a0(v2[28]) + 2) = 2;
		//*(WORD *)(*(DWORD *)(v2[28] + 68) + 4 * (unsigned __int16)fn_c000667a0(v2[28]) + 2) = 3;
		//*(WORD *)(*(DWORD *)(v2[28] + 68) + 4 * (unsigned __int16)fn_c000667a0(v2[28]) + 2) = 4;
		/*(WORD *)(*(DWORD *)(v2[28] + 68) + 4 * (unsigned __int16)fn_c000667a0(v2[28]) + 2) = 5;
		*(WORD *)(*(DWORD *)(v2[28] + 68) + 4 * (unsigned __int16)fn_c000667a0(v2[28]) + 2) = 6;
		*(WORD *)(*(DWORD *)(v2[28] + 68) + 4 * (unsigned __int16)fn_c000667a0(v2[28]) + 2) = 7;*/
		break;
	case 5:
		*(WORD *)(*(DWORD *)(v2[28] + 68) + 4 * (unsigned __int16)fn_c000667a0(v2[28]) + 2) = 0;
		*(WORD *)(*(DWORD *)(v2[28] + 68) + 4 * (unsigned __int16)fn_c000667a0(v2[28]) + 2) = 1;
		*(WORD *)(*(DWORD *)(v2[28] + 68) + 4 * (unsigned __int16)fn_c000667a0(v2[28]) + 2) = 2;
		//*(WORD *)(*(DWORD *)(v2[28] + 68) + 4 * (unsigned __int16)fn_c000667a0(v2[28]) + 2) = 4;
		if ((unsigned __int8)fn_c002152b0() && fn_c0021525a() > 1)
		{
			*(WORD *)(*(DWORD *)(v2[28] + 68) + 4 * (unsigned __int16)fn_c000667a0(v2[28]) + 2) = 3;
			*((BYTE *)v2 + 1124) = 0;
		}
		else
		{
			*((BYTE *)v2 + 1124) = 1;
		}
		break;
	case 6:
		*(WORD *)(*(DWORD *)(v2[28] + 68) + 4 * (unsigned __int16)fn_c000667a0(v2[28]) + 2) = 7;
		*(WORD *)(*(DWORD *)(v2[28] + 68) + 4 * (unsigned __int16)fn_c000667a0(v2[28]) + 2) = 5;
		break;
	default:
		break;
	}
	DWORD* v6;
	if (v2 == (DWORD *)-1100)
		v6 = 0;
	else
		v6 = v2 + 276;
	fn_c002113d3(v2 + 43, v6);
	return v2;
}

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

void InitH2Tweaks() {

	RefreshTogglexDelay();

	addDebugText("Begin Startup Tweaks.");

	//TODO(Num005) crashes dedis
	//custom_game_engines::init();
	//custom_game_engines::register_engine(c_game_engine_types::unknown5, &g_test_engine, king_of_the_hill);

	if (Memory::isDedicatedServer()) {
		phookServ1 = (thookServ1)DetourFunc(Memory::GetAddress<BYTE*>(0, 0x8EFA), (BYTE*)LoadRegistrySettings, 11);

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

		//Hook for advanced lobby options.
		pfn_c0024eeef = (tfn_c0024eeef)DetourClassFunc(Memory::GetAddress<BYTE*>(0x0024eeef), (BYTE*)fn_c0024eeef, 9);
		pfn_c0024fa19 = (tfn_c0024fa19)DetourClassFunc(Memory::GetAddress<BYTE*>(0x0024fa19), (BYTE*)fn_c0024fa19, 9);
		pfn_c0024fabc = (tfn_c0024fabc)DetourClassFunc(Memory::GetAddress<BYTE*>(0x0024fabc), (BYTE*)fn_c0024fabc, 13);

		WriteJmpTo(Memory::GetAddress(0x4544), is_init_flag_set);

		HudElements::Init();


		H2Tweaks::sunFlareFix();

		// patch to show game details menu in NETWORK serverlist too
		//NopFill(Memory::GetAddress(0x219D6D), 2);

		WriteJmpTo(Memory::GetAddress(0x7E43), WinMain);
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
	void(__cdecl* kill_volume_disable)(int volume_id);
	kill_volume_disable = Memory::GetAddress<void(__cdecl*)(int)>(0xb3ab8);
	kill_volume_disable(volume_id);
}

void H2Tweaks::toggleKillVolumes(bool enable) {
	if (enable)
		return;
	//TODO 'bool enable'
	if (!Memory::isDedicatedServer() && NetworkSession::localPeerIsSessionHost()) {
		for (int i = 0; i < get_scenario_volume_count(); i++) {
			kill_volume_disable(i);
		}
	}
}

void H2Tweaks::setHz() {

	if (Memory::isDedicatedServer())
		return;

	static bool refresh_redirected = false;
	if (!refresh_redirected) {
		WriteValue(Memory::GetAddress(0x25E869) + 3, std::addressof(H2Config_refresh_rate));
		refresh_redirected = true;
	}

	//*Memory::GetAddress<int*>(0xA3DA08) = H2Config_refresh_rate;
}

char ret_0() {
	return 0; //for 60 fps cinematics
}

void H2Tweaks::toggleUncappedCampaignCinematics(bool toggle) {
	if (Memory::isDedicatedServer())
		return;

	typedef char(__cdecl *is_cutscene_fps_cap)();
	auto p_is_cutscene_fps_cap = Memory::GetAddress<is_cutscene_fps_cap>(0x3A938);

	//60 fps cinematics enable
	PatchCall(Memory::GetAddress(0x97774), toggle ? ret_0 : p_is_cutscene_fps_cap);
	PatchCall(Memory::GetAddress(0x7C378), toggle ? ret_0 : p_is_cutscene_fps_cap);
}

void H2Tweaks::toggleAiMp(bool toggle) {
	WriteValue<BYTE>(Memory::GetAddress(0x30E684, 0x2B93F4), toggle ? JMP_OP_CODE : JNZ_OP_CODE);
}



void H2Tweaks::sunFlareFix()
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
