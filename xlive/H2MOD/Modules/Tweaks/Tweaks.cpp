#include "stdafx.h"

#include "Globals.h"
#include "H2MOD\Modules\Config\Config.h"
#include "H2MOD\Modules\CustomMenu\CustomMenu.h"
#include "H2MOD\Modules\HudElements\RadarPatch.h"
#include "H2MOD\Modules\OnScreenDebug\OnScreenDebug.h"
#include "H2MOD\Modules\Tweaks\Tweaks.h"
#include "H2MOD\Modules\Utils\Utils.h"
#include "H2MOD\Variants\VariantMPGameEngine.h"
#include "XLive\xnet\IpManagement\XnIp.h"
#include "H2MOD\Modules\Accounts\AccountLogin.h"
#include "Blam\Cache\TagGroups\shad.h"
#include "..\CustomResolutions\CustomResolutions.h"
#include "..\H2MOD\Tags\TagInterface.h"
#define _USE_MATH_DEFINES
#include "math.h"

extern bool H2IsDediServer;

#pragma region Done_Tweaks

typedef int(__cdecl *thookServ1)(HKEY, LPCWSTR);
thookServ1 phookServ1;
int __cdecl LoadRegistrySettings(HKEY hKey, LPCWSTR lpSubKey) {
	char result =
		phookServ1(hKey, lpSubKey);
	addDebugText("Post Server Registry Read.");
	if (strlen(H2Config_dedi_server_name) > 0) {
		wchar_t* PreLoadServerName = h2mod->GetAddress<wchar_t*>(0, 0x3B49B4);
		swprintf(PreLoadServerName, 15, L"%hs", H2Config_dedi_server_name);
		//char temp[27];
		//snprintf(temp, 27, "%ws", dedi_server_name);
		//MessageBoxA(NULL, temp, "Server Pre name thingy", MB_OK);
		wchar_t* LanServerName = h2mod->GetAddress<wchar_t*>(0, 0x52042A);
		swprintf(LanServerName, 2, L"");
	}
	if (strlen(H2Config_dedi_server_playlist) > 0) {
		wchar_t* ServerPlaylist = h2mod->GetAddress<wchar_t*>(0, 0x3B3704);
		swprintf(ServerPlaylist, 256, L"%hs", H2Config_dedi_server_playlist);
	}
	return result;
}

static bool NotDisplayIngameChat() {
	int GameGlobals = *h2mod->GetAddress<int*>(0x482D3C);
	DWORD* GameEngine = (DWORD*)(GameGlobals + 0x8);
	BYTE* GameState = h2mod->GetAddress<BYTE*>(0x420FC4);

	if (H2Config_hide_ingame_chat) {
		PlayerIterator playerIt;
		datum local_player_datum_index = h2mod->get_player_datum_index_from_controller_index(0);
		if (playerIt.get_data_at_index(local_player_datum_index.Index)->is_chatting == 2) {
			extern void hotkeyFuncToggleHideIngameChat();
			hotkeyFuncToggleHideIngameChat();
		}
		return true;
	}

	else if (*GameEngine != 3 && *GameState == 3) {
		//Enable chat in engine mode and game state mp.
		return false;
	}
	else {
		//original test - if is campaign
		return *GameEngine == 1;
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

void postConfig() {

	wchar_t mutexName2[255];
	swprintf(mutexName2, ARRAYSIZE(mutexName2), L"Halo2BasePort#%d", H2Config_base_port);
	HANDLE mutex2 = CreateMutex(0, TRUE, mutexName2);
	DWORD lastErr2 = GetLastError();
	if (lastErr2 == ERROR_ALREADY_EXISTS) {
		char NotificationPlayerText[120];
		sprintf(NotificationPlayerText, "Base port %d is already bound to!\nExpect MP to not work!", H2Config_base_port);
		addDebugText(NotificationPlayerText);
		MessageBoxA(NULL, NotificationPlayerText, "BASE PORT BIND WARNING!", MB_OK);
	}
	char NotificationText5[120];
	sprintf(NotificationText5, "Base port: %d.", H2Config_base_port);
	addDebugText(NotificationText5);

	RefreshTogglexDelay();
}

#pragma endregion

int(__cdecl* sub_20E1D8)(int, int, int, int, int, int);

int __cdecl sub_20E1D8_boot(int a1, int a2, int a3, int a4, int a5, int a6) {
	//a2 == 0x5 - system link lost connection
	if (a2 == 0xb9) {
		//boot them offline.
		XUserSignOut(0);
		ipManager.UnregisterLocalConnectionInfo();
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
	auto real_math_initialize_impl = h2mod->GetAddress<real_math_initialize*>(0x000340d7);
	real_math_initialize_impl();
}

void async_initialize()
{
	typedef int (async_initialize)();
	auto async_initialize_impl = h2mod->GetAddress<async_initialize*>(0x00032ce5);
	async_initialize_impl();
}

bool init_gfwl_gamestore()
{
	typedef char (init_gfwl_gamestore)();
	auto init_gfwl_gamestore_impl = h2mod->GetAddress<init_gfwl_gamestore*>(0x00202f3e);
	return init_gfwl_gamestore_impl();
}
// not sure if this is all it does
HANDLE init_data_checksum_info()
{
	typedef HANDLE init_data_checksum_info();
	auto init_data_checksum_info_impl = h2mod->GetAddress<init_data_checksum_info*>(0x000388d3);
	return init_data_checksum_info_impl();
}

// returns memory
void *runtime_state_init()
{
	typedef void *runtime_state_init();
	auto runtime_state_init_impl = h2mod->GetAddress<runtime_state_init*>(0x00037ed5);
	return runtime_state_init_impl();
}

void global_preferences_initialize()
{
	typedef void global_preferences_initialize();
	auto global_preferences_initialize_impl = h2mod->GetAddress<global_preferences_initialize*>(0x325FD);
	global_preferences_initialize_impl();
}

void font_initialize()
{
	typedef void __cdecl font_initialize();
	auto font_initialize_impl = h2mod->GetAddress<font_initialize*>(0x00031dff);
	font_initialize_impl();
}

bool tag_files_open()
{
	typedef bool tag_files_open();
	auto tag_files_open_impl = h2mod->GetAddress<tag_files_open*>(0x30D58);
	return tag_files_open_impl();
}

void init_timing(int a1)
{
	typedef DWORD (__cdecl init_timing)(int a1);
	auto init_timing_impl = h2mod->GetAddress<init_timing*>(0x37E39);
	init_timing_impl(a1);
}

void game_state_initialize()
{
	typedef void (__cdecl* game_state_initialize)();
	auto game_state_initialize_impl = h2mod->GetAddress<game_state_initialize>(0x00030aa6);
	game_state_initialize_impl();
}

bool rasterizer_initialize()
{
	typedef char rasterizer_initialize();
	auto rasterizer_initialize_impl = h2mod->GetAddress<rasterizer_initialize*>(0x00263359);
	return rasterizer_initialize_impl();
}

bool input_initialize()
{
	typedef char input_initialize();
	auto input_initialize_impl = h2mod->GetAddress<input_initialize*>(0x2FD23);
	return input_initialize_impl();
}

void sound_initialize()
{
	typedef void sound_initialize();
	auto sound_initialize_impl = h2mod->GetAddress<sound_initialize*>(0x2979E);
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
	DWORD* init_flags_array = h2mod->GetAddress<DWORD*>(0x46d820);
	return init_flags_array[id] != 0;
}

typedef int(*Video_HUDSizeUpdate_ptr)(int hudSize, int safeArea);
Video_HUDSizeUpdate_ptr Video_HUDSizeUpdate_orig;

const float maxHUDTextScale = 1080 * 0.0010416667f; // you'd think we could use 1200 since that's the games normal max resolution, but seems 1200 still hides some text :(
const float maxUiScaleFonts = 1.249f; // >= 1.25 will make text disappear

int Video_HUDSizeUpdate_hook(int hudSize, int safeArea)
{
	int retVal = Video_HUDSizeUpdate_orig(hudSize, safeArea);

	float* HUD_TextScale = h2mod->GetAddress<float*>(0x464028); // gets set by the Video_HUDSizeUpdate_orig call above, affects HUD text and crosshair size
	if (*HUD_TextScale > maxHUDTextScale)
	{
		// textScale = resolution_height * 0.0010416667
		// but if it's too large the game won't render it some reason (max seems to be around 1.4, 1.3 when sword icon is in the text?)
		// lets just set it to the largest known good value for now, until the cause of large text sizes being hidden is figured out
		*HUD_TextScale = maxHUDTextScale;
	}

	// UI_Scale was updated just before we were called, so lets fix it real quick...
	float* UI_Scale = h2mod->GetAddress<float*>(0xA3E424);
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
	float UI_Scale = *h2mod->GetAddress<float*>(0xA3E424);
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
	DWORD* flags_array = h2mod->GetAddress<DWORD*>(0x46d820);
	SecureZeroMemory(flags_array, startup_flags::count * sizeof(DWORD)); // should be zero initalized anyways but the game does it

	H2Config_voice_chat = false;
	flags_array[startup_flags::disable_voice_chat] = 1; // disables voice chat (XHV engine)
	flags_array[startup_flags::nointro] = H2Config_skip_intro;

	HANDLE(*fn_c000285fd)() = (HANDLE(*)())h2mod->GetAddress<void*>(0x000285fd);

	init_gfwl_gamestore();
	init_data_checksum_info();
	runtime_state_init();

	if (H2Config_hiresfix) {
		// HUD text size fix for higher resolutions
		Video_HUDSizeUpdate_orig = (Video_HUDSizeUpdate_ptr)DetourFunc(h2mod->GetAddress<BYTE*>(0x264A18), (BYTE*)Video_HUDSizeUpdate_hook, 7);

		// menu text fix for higher resolutions
		sub_671B02_orig = (sub_671B02_ptr)DetourFunc(h2mod->GetAddress<BYTE*>(0x271B02), (BYTE*)sub_671B02_hook, 5);
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
				WriteValue(h2mod->GetAddress(0x479EDC), 1);
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
				NopFill(h2mod->GetAddress(0x269FD5), 8);
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
	global_preferences_initialize();
	font_initialize();

	if (!LOG_CHECK(tag_files_open()))
		return false;
	
	game_state_initialize();

	// modifies esi need to check what the caller sets that too
	//char(*fn_c001a9de6)() = (char(*)())(h2mod->GetAddress(0x001a9de6));
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
	LONG(__stdcall* XLivePBufferAllocate)(DWORD size, FakePBuffer **pBuffer) = (LONG(__stdcall*)(DWORD, FakePBuffer**))h2mod->GetAddress(0xe886);
	DWORD(__stdcall* XLivePBufferSetByte)(FakePBuffer * pBuffer, DWORD offset, BYTE value) = (DWORD(__stdcall*)(FakePBuffer*, DWORD, BYTE))h2mod->GetAddress(0xe880);
	FakePBuffer** var_c00479e78 = h2mod->GetAddress<FakePBuffer**>(0x00479e78);
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
	auto InitPCCInfoImpl = h2mod->GetAddress<InitPCCInfo*>(0x260DDD);
	return InitPCCInfoImpl();
}

void run_main_loop()
{
	typedef int __cdecl run_main_loop();
	auto run_main_loop_impl = h2mod->GetAddress<run_main_loop*>(0x39E2C);
	run_main_loop_impl();
}

void main_engine_dispose()
{
	typedef int main_engine_dispose();
	auto main_engine_dispose_impl = h2mod->GetAddress<main_engine_dispose*>(0x48A9);
	main_engine_dispose_impl();
}

void show_error_message_by_id(int id)
{
	typedef void __cdecl show_error_message_by_id(int id);
	auto show_error_message_by_id_impl = h2mod->GetAddress<show_error_message_by_id*>(0x4A2E);
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

	HWND hWnd = *h2mod->GetAddress<HWND*>(0x46D9C4);
	HWND d3d_window = *h2mod->GetAddress<HWND*>(0x46D9C8); // not sure what this window is actual for, used in IDirect3DDevice9::Present
	destory_window(hWnd);
	destory_window(d3d_window);
	show_error_message_by_id(error_id);
}

int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	// set args
	WriteValue(h2mod->GetAddress(0x46D9BC), lpCmdLine); // command_line_args
	WriteValue(h2mod->GetAddress(0x46D9C0), hInstance); // g_instance
	WriteValue(h2mod->GetAddress(0x46D9CC), nShowCmd); // g_CmdShow

	// window setup
	wcscpy_s(h2mod->GetAddress<wchar_t*>(0x46D9D4), 0x40, L"halo"); // ClassName
	wcscpy_s(h2mod->GetAddress<wchar_t*>(0x46DA54), 0x40, L"Halo 2 - Project Cartographer"); // WindowName
	WNDPROC g_WndProc = h2mod->GetAddress<WNDPROC>(0x790E);
	WriteValue(h2mod->GetAddress(0x46D9D0), g_WndProc); // g_WndProc_ptr

	if (!LOG_CHECK(InitPCCInfo()))
	{
		LOG_TRACE_FUNC("Failed to get PCC info / insufficient system resources");
		run_async(
			MessageBoxA(NULL, "Failed to get compatibility info.", "PCC Error", S_OK);
		)
		show_error_message_by_id(108);
		// todo: load some default values here?
	}
	// mouse cursor setup
	HCURSOR cursor = LOG_CHECK(LoadCursor(NULL, MAKEINTRESOURCE(0x7F00)));
	WriteValue(h2mod->GetAddress(0x46D9B8), cursor); // g_hCursor

	// mess around with xlive (not calling XLiveInitialize etc)
	WriteValue<BYTE>(h2mod->GetAddress(0x4FAD98), 1);

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

	int g_fatal_error_id = *h2mod->GetAddress<int*>(0x46DAD4);
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

#pragma region custom map checks

bool open_cache_header(const wchar_t *lpFileName, tags::cache_header *cache_header_ptr, HANDLE *map_handle)
{
	typedef char (__cdecl open_cache_header)(const wchar_t *lpFileName, tags::cache_header *lpBuffer, HANDLE *map_handle, DWORD NumberOfBytesRead);
	auto open_cache_header_impl = h2mod->GetAddress<open_cache_header*>(0x642D0, 0x4C327);
	return open_cache_header_impl(lpFileName, cache_header_ptr, map_handle, 0);
}

void close_cache_header(HANDLE *map_handle)
{
	typedef void __cdecl close_cache_header(HANDLE *a1);
	auto close_cache_header_impl = h2mod->GetAddress<close_cache_header*>(0x64C03, 0x4CC5A);
	close_cache_header_impl(map_handle);
}

static std::wstring_convert<std::codecvt_utf8<wchar_t>> wstring_to_string;

int __cdecl validate_and_add_custom_map(BYTE *a1)
{
	tags::cache_header header;
	HANDLE map_cache_handle;
	wchar_t *file_name = (wchar_t*)a1 + 1216;
	if (!open_cache_header(file_name, &header, &map_cache_handle))
		return false;
	if (header.magic != 'head' || header.foot != 'foot' || header.file_size <= 0 || header.engine_gen != 8)
	{
		LOG_TRACE_FUNCW(L"\"{}\" has invalid header", file_name);
		return false;
	}
	if (header.type > 5 || header.type < 0)
	{
		LOG_TRACE_FUNCW(L"\"{}\" has bad scenario type", file_name);
		return false;
	}
	if (strnlen_s(header.name, 0x20u) >= 0x20 || strnlen_s(header.version, 0x20) >= 0x20)
	{
		LOG_TRACE_FUNCW(L"\"{}\" has invalid version or name string", file_name);
		return false;
	}
	if (header.type != tags::cache_header::scnr_type::Multiplayer && header.type != tags::cache_header::scnr_type::SinglePlayer)
	{
		LOG_TRACE_FUNCW(L"\"{}\" is not playable", file_name);
		return false;
	}

	close_cache_header(&map_cache_handle);
	// needed because the game loads the human readable map name and description from scenario after checks
	// without this the map is just called by it's file name

	// todo move the code for loading the descriptions to our code and get rid of this
	typedef int __cdecl validate_and_add_custom_map_interal(BYTE *a1);
	auto validate_and_add_custom_map_interal_impl = h2mod->GetAddress<validate_and_add_custom_map_interal*>(0x4F690, 0x56890);
	if (!validate_and_add_custom_map_interal_impl(a1))
	{
		LOG_TRACE_FUNCW(L"warning \"{}\" has bad checksums or is blacklisted, map may not work correctly", file_name);
		std::wstring fallback_name;
		if (strnlen_s(header.name, sizeof(header.name)) > 0) {
			fallback_name = wstring_to_string.from_bytes(header.name, &header.name[sizeof(header.name) - 1]);
		} else {
			std::wstring full_file_name = file_name;
			auto start = full_file_name.find_last_of('\\');
			fallback_name = full_file_name.substr(start != std::wstring::npos ? start : 0, full_file_name.find_last_not_of('.'));
		}
		wcsncpy_s(reinterpret_cast<wchar_t*>(a1 + 32), 0x20, fallback_name.c_str(), fallback_name.size());
	}
	// load the map even if some of the checks failed, will still mostly work
	return true;
}

bool __cdecl is_supported_build(char *build)
{
	const static std::unordered_set<std::string> offically_supported_builds{ "11122.07.08.24.1808.main", "11081.07.04.30.0934.main" };
	if (offically_supported_builds.count(build) == 0)
	{
		LOG_TRACE_FUNC("Build '{}' is not offically supported consider repacking and updating map with supported tools", build);
	}
	return true;
}

#pragma endregion

typedef char(__stdcall *tfn_c0024eeef)(DWORD*, int, int);
tfn_c0024eeef pfn_c0024eeef;
char __stdcall fn_c0024eeef(DWORD* thisptr, int a2, int a3)//__thiscall
{
	//char result = pfn_c0024eeef(thisptr, a2, a3);
	//return result;

	char(__thiscall* fn_c002139f8)(DWORD*, int, int, int, int*, int) = (char(__thiscall*)(DWORD*, int, int, int, int*, int))h2mod->GetAddress(0x002139f8);

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

	auto fn_c0024f9a1 = h2mod->GetAddress<int(__stdcall*)(int)>(0x24f9a1);
	auto fn_c0024f9dd = h2mod->GetAddress<int(__stdcall*)(int)>(0x24f9dd);
	auto fn_c0024ef79 = h2mod->GetAddress<int(__stdcall*)(int)>(0x24ef79);
	auto fn_c0024f5fd = h2mod->GetAddress<int(__stdcall*)(int)>(0x24f5fd);
	auto fn_c0024f015 = h2mod->GetAddress<int(__stdcall*)(int)>(0x24f015);
	auto fn_c0024f676 = h2mod->GetAddress<int(__stdcall*)(int)>(0x24f676);
	auto fn_c0024f68a = h2mod->GetAddress<int(__stdcall*)(int)>(0x24f68a);

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

	DWORD* var_c003d9254 = h2mod->GetAddress<DWORD*>(0x3d9254);
	DWORD* var_c003d9188 = h2mod->GetAddress<DWORD*>(0x3d9188);

	DWORD*(__thiscall* fn_c00213b1c)(DWORD* thisptr, int) = (DWORD*(__thiscall*)(DWORD*, int))h2mod->GetAddress(0x00213b1c);
	int(__thiscall* fn_c0000a551)(DWORD* thisptr) = (int(__thiscall*)(DWORD*))h2mod->GetAddress(0x0000a551);
	DWORD*(__thiscall* fn_c0021ffc9)(DWORD* thisptr) = (DWORD*(__thiscall*)(DWORD*))(h2mod->GetAddress(0x0021ffc9));
	void(__stdcall* fn_c0028870b)(int, int, int, DWORD*(__thiscall*)(DWORD*), int(__thiscall*)(DWORD*)) = (void(__stdcall*)(int, int, int, DWORD*(__thiscall*)(DWORD*), int(__thiscall*)(DWORD*)))h2mod->GetAddress(0x0028870b);
	DWORD*(__thiscall* fn_c002113c6)(DWORD* thisptr) = (DWORD*(__thiscall*)(DWORD*))h2mod->GetAddress(0x002113c6);
	int(__thiscall* fn_c0024fa19)(DWORD* thisptr, int, int*) = (int(__thiscall*)(DWORD*, int, int*))h2mod->GetAddress(0x0024fa19);
	int(*fn_c00215ea9)() = (int(*)())h2mod->GetAddress(0x00215ea9);
	int(__cdecl* fn_c0020d1fd)(char*, int numberOfButtons, int) = (int(__cdecl*)(char*, int, int))h2mod->GetAddress(0x0020d1fd);
	int(__cdecl* fn_c00066b33)(int) = (int(__cdecl*)(int))h2mod->GetAddress(0x00066b33);
	int(__cdecl* fn_c000667a0)(int) = (int(__cdecl*)(int))h2mod->GetAddress(0x000667a0);
	int(*fn_c002152b0)() = (int(*)())h2mod->GetAddress(0x002152b0);
	int(*fn_c0021525a)() = (int(*)())h2mod->GetAddress(0x0021525a);
	int(__thiscall* fn_c002113d3)(DWORD* thisptr, DWORD*) = (int(__thiscall*)(DWORD*, DWORD*))h2mod->GetAddress(0x002113d3);

	DWORD* v2 = thisptr;
	fn_c00213b1c(thisptr, a2);
	//*v2 = &c_squad_settings_list::`vftable';
	v2[0] = (DWORD)var_c003d9254;
	//*v2 = (DWORD)((BYTE*)h2mod->GetAddress(0x003d9254);
	fn_c0028870b((int)(v2 + 44), 132, 7, fn_c0021ffc9, fn_c0000a551);
	fn_c002113c6(v2 + 276);
	//v2[275] = &c_slot2<c_squad_settings_list, s_event_record *, long>::`vftable';
	v2[275] = (DWORD)var_c003d9188;
	//v2[275] = (DWORD)((BYTE*)h2mod->GetAddress(0x003d9188);
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


void fix_shader_template_nvidia(const std::string &template_name, const std::string &bitmap_name, size_t bitmap_idx)
{
	datum bitmap_to_fix    = tags::find_tag('bitm', bitmap_name);
	datum borked_template  = tags::find_tag('stem', template_name);

	LOG_DEBUG_FUNC("bitmap {0}, borked_template {1}", bitmap_to_fix.data, borked_template.data);

	if (bitmap_to_fix.IsNull() || borked_template.IsNull())
		return;

	LOG_DEBUG_FUNC("Fixing: template {}, bitmap {}", template_name, bitmap_name);

	tags::ilterator shaders('shad');
	while (!shaders.next().IsNull())
	{
		auto *shader = LOG_CHECK(tags::get_tag<'shad',Blam::Cache::TagGroups::shad>(shaders.m_datum));
		if (shader && shader->shader_template.TagIndex == borked_template && LOG_CHECK(shader->postprocessDefinition.size > 0))
		{
			LOG_DEBUG_FUNC("shader {} has borked template", tags::get_tag_name(shaders.m_datum));
			auto *post_processing = shader->postprocessDefinition[0];
			if (LOG_CHECK(post_processing->bitmaps.size >= (bitmap_idx + 1)))
			{
				auto *bitmap_block = post_processing->bitmaps[bitmap_idx];
				if (bitmap_block->bitmapGroup == bitmap_to_fix)
				{
					LOG_DEBUG_FUNC("Nulled bitmap {}", bitmap_idx);
					bitmap_block->bitmapGroup = datum::Null;
				}
			}
		}
	}
}

void fix_shaders_nvidia()
{
	fix_shader_template_nvidia(
		"shaders\\shader_templates\\opaque\\tex_bump_alpha_test_single_pass", 
		"shaders\\default_bitmaps\\bitmaps\\alpha_white",
		4
	);

	fix_shader_template_nvidia(
		"shaders\\shader_templates\\opaque\\tex_bump_alpha_test",
		"shaders\\default_bitmaps\\bitmaps\\gray_50_percent",
		1
	);
}

// probably MCC does this because it's 64 bit, but w/e, should work just fine
int system_get_time()
{
	LARGE_INTEGER PerformanceCount;
	LARGE_INTEGER PerformanceFrequency;

	QueryPerformanceCounter(&PerformanceCount);
	QueryPerformanceFrequency(&PerformanceFrequency);
	return (unsigned int)(PerformanceCount.QuadPart / (PerformanceFrequency.QuadPart / 1000));
}

void InitH2Tweaks() {
	postConfig();

	addDebugText("Begin Startup Tweaks.");

	//TODO(Num005) crashes dedis
	//custom_game_engines::init();
	//custom_game_engines::register_engine(c_game_engine_types::unknown5, &g_test_engine, king_of_the_hill);

	if (H2IsDediServer) {
		phookServ1 = (thookServ1)DetourFunc(h2mod->GetAddress<BYTE*>(0, 0x8EFA), (BYTE*)LoadRegistrySettings, 11);

		// set the additional pcr time
		WriteValue<BYTE>(h2mod->GetAddress(0, 0xE590) + 2, H2Config_additional_pcr_time);
	}
	else {//is client

		//Hook a function which changes the party privacy to detect if the lobby becomes open.
		//Problem is if you want to set it via mem poking, it won't push the lobby to the master automatically.
		//phookChangePrivacy = (thookChangePrivacy)DetourFunc((BYTE*)h2mod->GetAddress(0x2153ce), (BYTE*)HookChangePrivacy, 11);

		bool IntroHQ = true;//clients should set on halo2.exe -highquality

		if (!H2Config_skip_intro && IntroHQ) {
			BYTE assmIntroHQ[] = { 0xEB };
			WriteBytes(h2mod->GetAddress(0x221C29), assmIntroHQ, 1);
		}

		// adds support for more monitor resolutions
		CustomResolution::Initialize();

		//Disables the ESRB warning (only occurs for English Language).
		//disables the one if no intro vid occurs.
		WriteValue<BYTE>(h2mod->GetAddress(0x411030), 0);

		//disables the one after the intro video, by removing 0x40 flag from 0x7C6 bitmask
		WriteValue(h2mod->GetAddress(0x39948 + 2), 0x7C6 & ~0x40);

		//Set the LAN Server List Ping Frequency (milliseconds).
		//WriteValue(h2mod->GetAddress(0x001e9a89), 3000);
		//Set the LAN Server List Delete Entry After (milliseconds).
		//WriteValue(h2mod->GetAddress(0x001e9b0a), 9000);

		//Redirects the is_campaign call that the in-game chat renderer makes so we can show/hide it as we like.
		PatchCall(h2mod->GetAddress(0x22667B), NotDisplayIngameChat);
		PatchCall(h2mod->GetAddress(0x226628), NotDisplayIngameChat);

		//hook the gui popup for when the player is booted.
		sub_20E1D8 = h2mod->GetAddress<int(__cdecl*)(int, int, int, int, int, int)>(0x20E1D8);
		PatchCall(h2mod->GetAddress(0x21754C), &sub_20E1D8_boot);

		//Hook for advanced lobby options.
		pfn_c0024eeef = (tfn_c0024eeef)DetourClassFunc(h2mod->GetAddress<BYTE*>(0x0024eeef), (BYTE*)fn_c0024eeef, 9);
		pfn_c0024fa19 = (tfn_c0024fa19)DetourClassFunc(h2mod->GetAddress<BYTE*>(0x0024fa19), (BYTE*)fn_c0024fa19, 9);
		pfn_c0024fabc = (tfn_c0024fabc)DetourClassFunc(h2mod->GetAddress<BYTE*>(0x0024fabc), (BYTE*)fn_c0024fabc, 13);

		WriteJmpTo(h2mod->GetAddress(0x4544), is_init_flag_set);

		RadarPatch();
		H2Tweaks::sunFlareFix();

		// patch to show game details menu in NETWORK serverlist too
		//NopFill(h2mod->GetAddress(0x219D6D), 2);

		WriteJmpTo(h2mod->GetAddress(0x7E43), WinMain);
		WriteJmpTo(h2mod->GetAddress(0x39EA2), is_remote_desktop);

		tags::on_map_load(fix_shaders_nvidia);
	}

	// Both server and client
	WriteJmpTo(h2mod->GetAddress(0x1467, 0x12E2), is_supported_build);
	PatchCall(h2mod->GetAddress(0x1E49A2, 0x1EDF0), validate_and_add_custom_map);
	PatchCall(h2mod->GetAddress(0x4D3BA, 0x417FE), validate_and_add_custom_map);
	PatchCall(h2mod->GetAddress(0x4CF26, 0x41D4E), validate_and_add_custom_map);
	PatchCall(h2mod->GetAddress(0x8928, 0x1B6482), validate_and_add_custom_map);

	WriteJmpTo(h2mod->GetAddress(0x37E51, 0x2B4CE), system_get_time);

	addDebugText("End Startup Tweaks.");
}

void DeinitH2Tweaks() {

}

static DWORD* get_scenario_global_address() {
	return h2mod->GetAddress<DWORD*>(0x479e74);
}

static int get_scenario_volume_count() {
	int volume_count = *(int*)(*get_scenario_global_address() + 0x108);
	return volume_count;
}

static void kill_volume_disable(int volume_id) {
	void(__cdecl* kill_volume_disable)(int volume_id);
	kill_volume_disable = h2mod->GetAddress<void(__cdecl*)(int)>(0xb3ab8);
	kill_volume_disable(volume_id);
}

void H2Tweaks::toggleKillVolumes(bool enable) {
	if (enable)
		return;
	//TODO 'bool enable'
	if (!h2mod->Server && NetworkSession::localPeerIsSessionHost()) {
		for (int i = 0; i < get_scenario_volume_count(); i++) {
			kill_volume_disable(i);
		}
	}
}

void H2Tweaks::setSens(std::string input_type, int sens) {

	if (H2IsDediServer)
		return;

	if (sens < 0)
		return;

	int absSensIndex = sens - 1;

	if (input_type == "controller") {
		*h2mod->GetAddress<float*>(0x4A89BC) = 40.0f + 10.0f * static_cast<float>(absSensIndex); //y-axis
		*h2mod->GetAddress<float*>(0x4A89B8) = 80.0f + 20.0f * static_cast<float>(absSensIndex); //x-axis
	}
	else if (input_type == "mouse") {
		*h2mod->GetAddress<float*>(0x4A89B4) = 25.0f + 10.0f * static_cast<float>(absSensIndex); //y-axis
		*h2mod->GetAddress<float*>(0x4A89B0) = 50.0f + 20.0f * static_cast<float>(absSensIndex); //x-axis
	}
}

void H2Tweaks::setSavedSens() {
	if (H2Config_mouse_sens != 0)
		H2Tweaks::setSens("mouse", (H2Config_mouse_sens));

	if (H2Config_controller_sens != 0)
		H2Tweaks::setSens("controller", (H2Config_controller_sens));
}

void H2Tweaks::setFOV(int field_of_view_degrees) {

	if (H2IsDediServer)
		return;

	static float fov = 70.0f * M_PI / 180.0f;
	static bool fov_redirected = false;
	if (field_of_view_degrees > 0 && field_of_view_degrees <= 110)
	{
		if (!fov_redirected)
		{
			BYTE opcode[6] = { 0xD9, 0x05, 0x00, 0x00, 0x00, 0x00 };
			WritePointer((DWORD)&opcode[2], &fov);
			WriteBytes(h2mod->GetAddress(0x907F3), opcode, sizeof(opcode)); // fld dword ptr[fov]

			fov_redirected = true;
		}

		//const double default_radians_field_of_view = 70.0f * M_PI / 180.0f;
		fov = (float)field_of_view_degrees * M_PI / 180.0f;
	}
}

void H2Tweaks::setVehicleFOV(int field_of_view_degrees) {

	if (H2IsDediServer)
		return;

	if (field_of_view_degrees > 0 && field_of_view_degrees <= 110)
	{
		float calculated_radians_FOV = (float)field_of_view_degrees * M_PI / 180.0f;
		WriteValue(h2mod->GetAddress(0x413780), calculated_radians_FOV); // Third Person
	}
}

void H2Tweaks::setHz() {

	if (H2IsDediServer)
		return;

	*h2mod->GetAddress<int*>(0xA3DA08) = H2Config_refresh_rate;
}

void H2Tweaks::setCrosshairPos(float crosshair_offset) {

	if (H2IsDediServer)
		return;

	if (!FloatIsNaN(crosshair_offset)) {

		tags::tag_data_block* player_controls_block = reinterpret_cast<tags::tag_data_block*>(tags::get_matg_globals_ptr() + 240);
		if (player_controls_block->block_count > 0)
		{
			for (int i = 0; i < player_controls_block->block_count; i++)
				*(float*)(tags::get_tag_data() + player_controls_block->block_data_offset + 128 * i + 28) = crosshair_offset;
		}
	}
}

void H2Tweaks::setCrosshairSize(int size, bool preset) {
	if (H2IsDediServer)
		return;

	DWORD BATRIF1 = (DWORD)tags::get_matg_globals_ptr() + 0x7aa750;
	DWORD BATRIF2 = (DWORD)tags::get_matg_globals_ptr() + 0x7aa752;
	DWORD SMG1 = (DWORD)tags::get_matg_globals_ptr() + 0x7A9F9C;
	DWORD SMG2 = (DWORD)tags::get_matg_globals_ptr() + 0x7A9F9E;
	DWORD CRBN1 = (DWORD)tags::get_matg_globals_ptr() + 0x7ab970;
	DWORD CRBN2 = (DWORD)tags::get_matg_globals_ptr() + 0x7ab972;
	DWORD BEAMRIF1 = (DWORD)tags::get_matg_globals_ptr() + 0x7AA838;
	DWORD BEAMRIF2 = (DWORD)tags::get_matg_globals_ptr() + 0x7AA83A;
	DWORD MAG1 = (DWORD)tags::get_matg_globals_ptr() + 0x7AA33C;
	DWORD MAG2 = (DWORD)tags::get_matg_globals_ptr() + 0x7AA33E;
	DWORD PLASRIF1 = (DWORD)tags::get_matg_globals_ptr() + 0x7AA16C;
	DWORD PLASRIF2 = (DWORD)tags::get_matg_globals_ptr() + 0x7AA16E;
	DWORD SHTGN1 = (DWORD)tags::get_matg_globals_ptr() + 0x7AA424;
	DWORD SHTGN2 = (DWORD)tags::get_matg_globals_ptr() + 0x7AA426;
	DWORD SNIP1 = (DWORD)tags::get_matg_globals_ptr() + 0x7AA994;
	DWORD SNIP2 = (DWORD)tags::get_matg_globals_ptr() + 0x7AA996;
	DWORD SWRD1 = (DWORD)tags::get_matg_globals_ptr() + 0x7AA8AC;
	DWORD SWRD2 = (DWORD)tags::get_matg_globals_ptr() + 0x7AA8AE;
	DWORD ROCKLAUN1 = (DWORD)tags::get_matg_globals_ptr() + 0x7AA3B0;
	DWORD ROCKLAUN2 = (DWORD)tags::get_matg_globals_ptr() + 0x7AA3B2;
	DWORD PLASPI1 = (DWORD)tags::get_matg_globals_ptr() + 0x7AA0F8;
	DWORD PLASPI2 = (DWORD)tags::get_matg_globals_ptr() + 0x7AA0FA;
	DWORD BRUTESHOT1 = (DWORD)tags::get_matg_globals_ptr() + 0x7AA7C4;
	DWORD BRUTESHOT2 = (DWORD)tags::get_matg_globals_ptr() + 0x7AA7C6;
	DWORD NEED1 = (DWORD)tags::get_matg_globals_ptr() + 0x7AA254;
	DWORD NEED2 = (DWORD)tags::get_matg_globals_ptr() + 0x7AA256;
	DWORD SENTBEAM1 = (DWORD)tags::get_matg_globals_ptr() + 0x7AB5D0;
	DWORD SENTBEAM2 = (DWORD)tags::get_matg_globals_ptr() + 0x7AB5D2;

	DWORD WEAPONS[] = { BATRIF1, BATRIF2, SMG1, SMG2, CRBN1, CRBN2, BEAMRIF1, BEAMRIF2, MAG1, MAG2, PLASRIF1, PLASRIF2, SHTGN1, SHTGN2, SNIP1, SNIP2, SWRD1, SWRD2, ROCKLAUN1, ROCKLAUN2, PLASPI1, PLASPI2, BRUTESHOT1, BRUTESHOT2, NEED1, NEED2, SENTBEAM1, SENTBEAM2 };

	int disabled[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	int defaultSize[] = { 70, 70, 110, 110, 78, 52, 26, 10, 50, 50, 90, 90,110, 110, 20, 20, 110, 106, 126, 126, 106, 91, 102, 124, 112, 34, 70, 38 };
	int verySmall[] = { 30, 30, 40, 40, 39, 26, 26, 10, 25, 25, 45, 45, 65, 65, 12, 12, 55, 53, 63, 63, 53, 45, 51, 62, 56, 17, 35, 19 };
	int small[] = { 40, 40, 65, 65, 57, 38, 26, 10, 35, 35, 55, 55, 80, 80, 15, 15, 82, 79, 90, 90, 79, 68, 76, 93, 84, 25, 52, 27};
	int large[] = { 80, 80, 130, 130, 114, 76, 52, 20, 70, 70, 110, 110, 160, 160, 30, 30, 164, 158, 180, 180, 158, 136, 152, 186, 168, 50, 104, 57 };
	int* configArray[] = { &H2Config_BATRIF_WIDTH, &H2Config_BATRIF_HEIGHT, &H2Config_SMG_WIDTH, &H2Config_SMG_HEIGHT, &H2Config_CRBN_WIDTH, &H2Config_CRBN_HEIGHT, &H2Config_BEAMRIF_WIDTH, &H2Config_BEAMRIF_HEIGHT, &H2Config_MAG_WIDTH, &H2Config_MAG_HEIGHT, &H2Config_PLASRIF_WIDTH, &H2Config_PLASRIF_HEIGHT, &H2Config_SHTGN_WIDTH, &H2Config_SHTGN_HEIGHT, &H2Config_SNIP_WIDTH, &H2Config_SNIP_HEIGHT, &H2Config_SWRD_WIDTH, &H2Config_SWRD_HEIGHT, &H2Config_ROCKLAUN_WIDTH, &H2Config_ROCKLAUN_HEIGHT, &H2Config_PLASPI_WIDTH, &H2Config_PLASPI_HEIGHT, &H2Config_BRUTESHOT_WIDTH, &H2Config_BRUTESHOT_HEIGHT, &H2Config_NEED_WIDTH, &H2Config_NEED_HEIGHT, &H2Config_SENTBEAM_WIDTH, &H2Config_SENTBEAM_HEIGHT};
	int* tempArray;

	if (preset) {
		switch (size) {
		case 1:
			tempArray = disabled;
			break;
		case 2:
			tempArray = verySmall;
			break;
		case 3:
			tempArray = small;
			break;
		case 4:
			tempArray = large;
			break;
		default:
			tempArray = defaultSize;
			break;
		}

		for (int i = 0; i < 28; i++) {
			*configArray[i] = tempArray[i];
		}
	}

	if (h2mod->GetMapType() == MapType::MULTIPLAYER_MAP) {

		for (int i = 0; i < 28; i++) {
			if (configArray[i] == 0) {
				*reinterpret_cast<short*>(WEAPONS[i]) = disabled[i];
			}
			else if (*configArray[i] == 1 || *configArray[i] < 0 || *configArray[i] > 65535) {
				*reinterpret_cast<short*>(WEAPONS[i]) = defaultSize[i];
			}
			else {
				*reinterpret_cast<short*>(WEAPONS[i]) = *configArray[i];
			}
		}
	}
}

char ret_0() {
	return 0; //for 60 fps cinematics
}

void H2Tweaks::toggleUncappedCampaignCinematics(bool toggle) {
	if (H2IsDediServer)
		return;

	typedef char(__cdecl *is_cutscene_fps_cap)();
	auto p_is_cutscene_fps_cap = h2mod->GetAddress<is_cutscene_fps_cap>(0x3A938);

	//60 fps cinematics enable
	PatchCall(h2mod->GetAddress(0x97774), toggle ? ret_0 : p_is_cutscene_fps_cap);
	PatchCall(h2mod->GetAddress(0x7C378), toggle ? ret_0 : p_is_cutscene_fps_cap);
}

void H2Tweaks::toggleAiMp(bool toggle) {
	WriteValue<BYTE>(h2mod->GetAddress(0x30E684, 0x2B93F4), toggle ? JMP_OP_CODE : JNZ_OP_CODE);
}

void H2Tweaks::applyMeleePatch(bool toggle)
{
	//static float melee_lunge_distance_factor = toggle ? 0.33333334f * 2 : 0.33333334f;
	//WritePointer(h2mod->GetAddress(0x10B160, 0xFD790) + 4, &melee_lunge_distance_factor);
	//WritePointer(h2mod->GetAddress(0x10B354, 0xFD984) + 4, &melee_lunge_distance_factor); // distance when mid air
	//WritePointer(h2mod->GetAddress(0x10B6A3, 0xFDCD3) + 4, &melee_lunge_distance_factor); // acceleration
	//WritePointer(h2mod->GetAddress(0x10BB78, 0xFE1A8) + 4, &melee_lunge_distance_factor);

	WriteValue<BYTE>(h2mod->GetAddress(0x10B408, 0xFDA38) + 2, toggle ? 5 : 6); // sword
	WriteValue<BYTE>(h2mod->GetAddress(0x10B40B, 0xFDA3B) + 2, toggle ? 2 : 1); // generic weapon
}

void H2Tweaks::sunFlareFix()
{
	if (H2IsDediServer)
		return;

	//rasterizer_near_clip_distance <real>
	//Changed from game default of 0.06 to 0.0601
	WriteValue<float>(h2mod->GetAddress(0x468150), 0.0601f);
}

void H2Tweaks::WarpFix(bool enable)
{
	if (H2IsDediServer)
		return;

	//Improves warping issues 
	if (enable)
	{
		WriteValue<float>(h2mod->GetAddress(0x4F958C), 4.0);
		WriteValue<float>(h2mod->GetAddress(0x4F9594), 10.0);
	}
	else
	{
		WriteValue<float>(h2mod->GetAddress(0x4F958C), 2.5);
		WriteValue<float>(h2mod->GetAddress(0x4F9594), 7.5);
	}	
}