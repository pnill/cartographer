#include "stdafx.h"
#include <ShellAPI.h>
#include <string>
#include <unordered_set>
#include <codecvt>
#include "H2MOD\Modules\Startup\Startup.h"
#include "H2MOD\Modules\Config\Config.h"
#include "H2MOD\Modules\Utils\Utils.h"
#include "H2MOD\Modules\OnScreenDebug\OnScreenDebug.h"
#include "H2MOD\Modules\CustomMenu\CustomMenu.h"
#include "H2MOD.h"
#include "Globals.h"
#include "Util\Hooks\Hook.h"
#include "Util/filesys.h"
#include "XLive\UserManagement\CUser.h"
#include "H2MOD\Modules\MapChecksum\MapChecksumSync.h"
#include "H2MOD\Modules\Tweaks\Tweaks.h"


#define _USE_MATH_DEFINES
#include "math.h"

#pragma region Done_Tweaks

typedef int(__cdecl *thookServ1)(HKEY, LPCWSTR);
thookServ1 phookServ1;
int __cdecl LoadRegistrySettings(HKEY hKey, LPCWSTR lpSubKey) {
	char result =
		phookServ1(hKey, lpSubKey);
	addDebugText("Post Server Registry Read.");
	if (strlen(H2Config_dedi_server_name) > 0) {
		wchar_t* PreLoadServerName = (wchar_t*)((BYTE*)H2BaseAddr + 0x3B49B4);
		swprintf(PreLoadServerName, 15, L"%hs", H2Config_dedi_server_name);
		//char temp[27];
		//snprintf(temp, 27, "%ws", dedi_server_name);
		//MessageBoxA(NULL, temp, "Server Pre name thingy", MB_OK);
		wchar_t* LanServerName = (wchar_t*)((BYTE*)H2BaseAddr + 0x52042A);
		swprintf(LanServerName, 2, L"");
	}
	if (strlen(H2Config_dedi_server_playlist) > 0) {
		wchar_t* ServerPlaylist = (wchar_t*)((BYTE*)H2BaseAddr + 0x3B3704);
		swprintf(ServerPlaylist, 256, L"%hs", H2Config_dedi_server_playlist);
	}
	return result;
}

typedef int(__stdcall *thookServ2)();
thookServ2 phookServ2;
int __stdcall PreReadyLoad() {
	MessageBoxA(NULL, "aaaaaaas", "PreCrash", MB_OK);
	int result =
		phookServ2();
	MessageBoxA(NULL, "aaaaaaasaaaaaa", "PostCrash", MB_OK);
	if (strlen(H2Config_dedi_server_name) > 0) {
		wchar_t* PreLoadServerName = (wchar_t*)((BYTE*)H2BaseAddr + 0x52042A);
		swprintf(PreLoadServerName, 32, L"%hs", H2Config_dedi_server_name);
		char temp[27];
		snprintf(temp, 32, H2Config_dedi_server_name);
		MessageBoxA(NULL, temp, "AAA Server Pre name thingy", MB_OK);
	}
	return result;
}

static bool NotDisplayIngameChat() {
	int GameGlobals = (int)*(int*)((DWORD)H2BaseAddr + 0x482D3C);
	DWORD* GameEngine = (DWORD*)(GameGlobals + 0x8);
	BYTE* GameState = (BYTE*)((DWORD)H2BaseAddr + 0x420FC4);

	if (H2Config_hide_ingame_chat) {
		int GameTimeGlobals = (int)*(int*)((DWORD)H2BaseAddr + 0x4C06E4);
		DWORD* ChatOpened = (DWORD*)(GameTimeGlobals + 0x354);//MP Only?
		if (*ChatOpened == 2) {
			extern void hotkeyFuncToggleHideIngameChat();
			hotkeyFuncToggleHideIngameChat();
		}
	}

	if (H2Config_hide_ingame_chat) {
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
		H2Config_master_ip = inet_addr("127.0.0.1");
		H2Config_master_port_relay = 2001;
		extern int MasterState;
		MasterState = 2;
		extern char* ServerStatus;
		snprintf(ServerStatus, 250, "Status: Offline");
	}
	int result = sub_20E1D8(a1, a2, a3, a4, a5, a6);
	return result;
}


#pragma region init hook
template <typename T = void>
static inline T *GetAddress(DWORD client, DWORD server = 0)
{
	return reinterpret_cast<T*>(H2BaseAddr + (H2IsDediServer ? server : client));
}

#pragma region func_wrappers
void real_math_initialize()
{
	typedef int (real_math_initialize)();
	auto real_math_initialize_impl = GetAddress<real_math_initialize>(0x000340d7);
	real_math_initialize_impl();
}

void async_initialize()
{
	typedef int (async_initialize)();
	auto async_initialize_impl = GetAddress<async_initialize>(0x00032ce5);
	async_initialize_impl();
}

bool init_gfwl_gamestore()
{
	typedef char (init_gfwl_gamestore)();
	auto init_gfwl_gamestore_impl = GetAddress<init_gfwl_gamestore>(0x00202f3e);
	return init_gfwl_gamestore_impl();
}
// not sure if this is all it does
HANDLE init_data_checksum_info()
{
	typedef HANDLE init_data_checksum_info();
	auto init_data_checksum_info_impl = GetAddress<init_data_checksum_info>(0x000388d3);
	return init_data_checksum_info_impl();
}

// returns memory
void *runtime_state_init()
{
	typedef void *runtime_state_init();
	auto runtime_state_init_impl = GetAddress<runtime_state_init>(0x00037ed5);
	return runtime_state_init_impl();
}

void global_preferences_initialize()
{
	typedef void global_preferences_initialize();
	auto global_preferences_initialize_impl = GetAddress<global_preferences_initialize>(0x325FD);
	global_preferences_initialize_impl();
}

void font_initialize()
{
	typedef void __cdecl font_initialize();
	auto font_initialize_impl = GetAddress<font_initialize>(0x00031dff);
	font_initialize_impl();
}

bool tag_files_open()
{
	typedef bool tag_files_open();
	auto tag_files_open_impl = GetAddress<tag_files_open>(0x30D58);
	return tag_files_open_impl();
}

void init_timing(int a1)
{
	typedef DWORD (__cdecl init_timing)(int a1);
	auto init_timing_impl = GetAddress<init_timing>(0x37E39);
	init_timing_impl(a1);
}

void game_state_initialize(void *data)
{
	typedef void __fastcall game_state_initialize(void *data);
	auto game_state_initialize_impl = GetAddress<game_state_initialize>(0x00030aa6);
	game_state_initialize_impl(data);
}

bool rasterizer_initialize()
{
	typedef char rasterizer_initialize();
	auto rasterizer_initialize_impl = GetAddress<rasterizer_initialize>(0x00263359);
	return rasterizer_initialize_impl();
}

bool input_initialize()
{
	typedef char input_initialize();
	auto input_initialize_impl = GetAddress<input_initialize>(0x2FD23);
	return input_initialize_impl();
}

void sound_initialize()
{
	typedef void sound_initialize();
	auto sound_initialize_impl = GetAddress<sound_initialize>(0x2979E);
	return sound_initialize_impl();
}

#pragma endregion

enum flags : int
{
	windowed,
	unk, // some network thing
	nosound,
	unk1, // disable vista needed version check?
	disable_hardware_vertex_processing, // force hardware vertex processing off
	novsync,
	unk3, // squad browser/xlive/ui?
	nointro, // disables intro movie
	unk5, // some tag thing?
	unk6, // some tag thing?
	unk7, // some tag thing?
	unk8, // some tag thing?
	unk9, // some tag thing?
	unk10, // some tag thing?
	unk11, // some tag thing?
	unk12, // seen near xlive init code
	unk13,
	xbox_live_silver_account, // if true, disables 'gold-only' features, like quickmatch etc
	unk15, // fuzzer/automated testing? (sapien)
	ui_fast_test_no_start, // same as below but doesn't start a game
	ui_fast_test, // auto navigates the UI selecting the default option
	unk18, // player controls related (sapien)
	monitor_count,
	unk19,
	unk20,
	unk21, // something to do with game time?
	unk22,
	unk23, // network? value seems unused?
	high_quality, // forced sound reverb ignoring CPU score and disable forcing low graphical settings (sapien)
	unk24,

	count
};
static_assert(flags::count == 30, "Bad flags count");

int flag_log_count[flags::count];
BOOL __cdecl is_init_flag_set(flags id)
{
	if (flag_log_count[id] < 10)
	{
		TRACE_GAME("is_init_flag_set() : flag %i", id);
		flag_log_count[id]++;
		if (flag_log_count[id] == 10)
			TRACE_GAME("is_init_flag_set() : flag %i logged to many times ignoring", id);
	}
	DWORD* init_flags_array = reinterpret_cast<DWORD*>(H2BaseAddr + 0x0046d820);
	return init_flags_array[id] != 0;
}

const static int max_mointor_count = 9;
bool engine_basic_init()
{
	DWORD* flags_array = reinterpret_cast<DWORD*>(H2BaseAddr + 0x0046d820);
	memset(flags_array, 0x00, flags::count); // should be zero initalized anyways but the game does it

	flags_array[flags::nointro] = H2Config_skip_intro;

	HANDLE(*fn_c000285fd)() = (HANDLE(*)())(GetAddress(0x000285fd));

	init_gfwl_gamestore();
	init_data_checksum_info();
	runtime_state_init();

	int arg_count;
	wchar_t **cmd_line_args = LOG_CHECK(CommandLineToArgvW(GetCommandLineW(), &arg_count));
	if (cmd_line_args && arg_count > 1) {
		for (int i = 1; i < arg_count; i++) {
			wchar_t* cmd_line_arg = cmd_line_args[i];

			if (wcsicmp(cmd_line_arg, L"-windowed") == 0) {
				flags_array[flags::windowed] = 1;
			}
			else if (wcsicmp(cmd_line_arg, L"-nosound") == 0) {
				flags_array[flags::nosound] = 1;
				WriteValue(H2BaseAddr + 0x479EDC, 1);
			}
			else if (wcsicmp(cmd_line_arg, L"-novsync") == 0) {
				flags_array[flags::novsync] = 1;
			}
			else if (wcsicmp(cmd_line_arg, L"-nointro") == 0) {
				flags_array[flags::nointro] = 1;
			}
			else if (wcsnicmp(cmd_line_arg, L"-monitor:", 9) == 0) {
				int monitor_id = _wtol(&cmd_line_arg[9]);
				flags_array[flags::monitor_count] = min(max(0, monitor_id), max_mointor_count);
			}
			else if (wcsicmp(cmd_line_arg, L"-highquality") == 0) {
				flags_array[flags::high_quality] = 1;
			}
			else if (wcsicmp(cmd_line_arg, L"-depthbiasfix") == 0)
			{
				// Fixes issue #118
			    /* g_depth_bias always NULL rather than taking any value from
			    shader tag before calling g_D3DDevice->SetRenderStatus(D3DRS_DEPTHBIAS, g_depth_bias); */
				NopFill<8>(reinterpret_cast<DWORD>(GetAddress(0x269FD5)));
			}
#ifdef _DEBUG
			else if (wcsnicmp(cmd_line_arg, L"-dev_flag:", 10) == 0) {
				int flag_id = _wtol(&cmd_line_arg[10]);
				flags_array[min(max(0, flag_id), flags::count - 1)] = 1;
			}
#endif
		}
	}
	LocalFree(cmd_line_args);

	if (flags_array[flags::unk22])
		init_timing(1000 * flags_array[flags::unk22]);
	real_math_initialize();
	async_initialize();
	global_preferences_initialize();
	font_initialize();

	if (!LOG_CHECK(tag_files_open()))
		return false;
	void *var_c004ae8e0 = GetAddress(0x004ae8e0);
	game_state_initialize(var_c004ae8e0);

	// modifies esi need to check what the caller sets that too
	//char(*fn_c001a9de6)() = (char(*)())(GetAddress(0x001a9de6));
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
	LONG(__stdcall* XLivePBufferAllocate)(DWORD size, FakePBuffer **pBuffer) = (LONG(__stdcall*)(DWORD, FakePBuffer**))(GetAddress( 0x0000e886));
	DWORD(__stdcall* XLivePBufferSetByte)(FakePBuffer * pBuffer, DWORD offset, BYTE value) = (DWORD(__stdcall*)(FakePBuffer*, DWORD, BYTE))(GetAddress( 0x0000e880));
	DWORD* var_c00479e78 = GetAddress<DWORD>(0x00479e78);
	XLivePBufferAllocate(2, (FakePBuffer**)&var_c00479e78);
	XLivePBufferSetByte((FakePBuffer*)var_c00479e78, 0, 0);
	XLivePBufferSetByte((FakePBuffer*)var_c00479e78, 1, 0);

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
	auto InitPCCInfoImpl = GetAddress<InitPCCInfo>(0x260DDD);
	return InitPCCInfoImpl();
}

void run_main_loop()
{
	typedef int __cdecl run_main_loop();
	auto run_main_loop_impl = GetAddress<run_main_loop>(0x39E2C);
	run_main_loop_impl();
}

void main_engine_dispose()
{
	typedef int main_engine_dispose();
	auto main_engine_dispose_impl = GetAddress<main_engine_dispose>(0x48A9);
	main_engine_dispose_impl();
}

void show_error_message_by_id(int id)
{
	typedef void __cdecl show_error_message_by_id(int id);
	auto show_error_message_by_id_impl = GetAddress<show_error_message_by_id>(0x4A2E);
	show_error_message_by_id_impl(id);
}
#pragma endregion

void show_fatal_error(int error_id)
{
	TRACE_FUNC("error_id : %d", error_id);

	auto destory_window = [](HWND handle) {
		if (handle)
			DestroyWindow(handle);
	};

	HWND hWnd = *GetAddress<HWND>(0x46D9C4);
	HWND d3d_window = *GetAddress<HWND>(0x46D9C8); // not sure what this window is actual for, used in IDirect3DDevice9::Present
	destory_window(hWnd);
	destory_window(d3d_window);
	show_error_message_by_id(error_id);
}

int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	// set args
	WriteValue(H2BaseAddr + 0x46D9BC, lpCmdLine); // command_line_args
	WriteValue(H2BaseAddr + 0x46D9C0, hInstance); // g_instance
	WriteValue(H2BaseAddr + 0x46D9CC, nShowCmd); // g_CmdShow

	// window setup
	wcscpy_s(reinterpret_cast<wchar_t*>(H2BaseAddr + 0x46D9D4), 0x40, L"halo"); // ClassName
	wcscpy_s(reinterpret_cast<wchar_t*>(H2BaseAddr + 0x46DA54), 0x40, L"Halo 2 - Project Cartographer"); // WindowName
	WNDPROC g_WndProc = reinterpret_cast<WNDPROC>(H2BaseAddr + 0x790E);
	WriteValue(H2BaseAddr + 0x46D9D0, g_WndProc); // g_WndProc_ptr
	if (!LOG_CHECK(InitPCCInfo()))
	{
		TRACE_FUNC("Failed to get PCC info / insufficient system resources");
		run_async(
			MessageBoxA(NULL, "Failed to get compatibility info.", "PCC Error", S_OK);
		)
		show_error_message_by_id(108);
		// todo: load some default values here?
	}
	// mouse cursor setup
	HCURSOR cursor = LOG_CHECK(LoadCursor(NULL, MAKEINTRESOURCE(0x7F00)));
	WriteValue(H2BaseAddr + 0x46D9B8, cursor); // g_hCursor

	// mess around with xlive (not calling XLiveInitialize etc)
	WriteValue<BYTE>(H2BaseAddr + 0x4FAD98, 1);

	// intialize some basic game subsystems
	if (LOG_CHECK(engine_basic_init()))
	{
		run_main_loop(); // actually run game
		main_engine_dispose(); // cleanup
	} else
	{
		TRACE_FUNC("Engine startup failed!");
		show_fatal_error(108);
		return 1;
	}

	int g_fatal_error_id = *reinterpret_cast<int*>(H2BaseAddr + 0x46DAD4);
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

struct cache_header
{
	int magic;
	int engine_gen;
	int file_size;
	int field_C;
	int tag_offset;
	int data_offset;
	int data_size;
	int tag_size;
	int tag_offset_mask;
	int field_24;
	BYTE padding[260];
	char version[32];
	enum scnr_type : int
	{
		SinglePlayer = 0,
		Multiplayer = 1,
		MainMenu = 2,
		MultiplayerShared = 3,
		SinglePlayerShared = 4
	};
	scnr_type type;
	int shared_type;
	int crc_uiid;
	char field_158;
	char tracked__maybe;
	char field_15A;
	char field_15B;
	int field_15C;
	int field_160;
	int field_164;
	int field_168;
	int string_block_offset;
	int string_table_count;
	int string_table_size;
	int string_idx_offset;
	int string_table_offset;
	int extern_deps;
	int time_low;
	int time_high;
	int main_menu_time_low;
	int main_menu_time_high;
	int shared_time_low;
	int shared_time_high;
	int campaign_time_low;
	int campaign_time_high;
	char name[32];
	int field_1C4;
	char tag_name[256];
	int minor_version;
	int TagNamesCount;
	int TagNamesBufferOffset;
	int TagNamesBufferSize;
	int TagNamesIndicesOffset;
	int LanguagePacksOffset;
	int LanguagePacksSize;
	int SecondarySoundGestaltDatumIndex;
	int FastLoadGeometryBlockOffset;
	int FastLoadGeometryBlockSize;
	int Checksum;
	int MoppCodesChecksum;
	BYTE field_2F8[1284];
	int foot;
};

static_assert(sizeof(cache_header) == 0x800, "Bad cache header size");

bool open_cache_header(const wchar_t *lpFileName, cache_header *cache_header_ptr, HANDLE *map_handle)
{
	typedef char __cdecl open_cache_header(const wchar_t *lpFileName, cache_header *lpBuffer, HANDLE *map_handle, DWORD NumberOfBytesRead);
	auto open_cache_header_impl = GetAddress<open_cache_header>(0x642D0, 0x4C327);
	return open_cache_header_impl(lpFileName, cache_header_ptr, map_handle, 0);
}

void close_cache_header(HANDLE *map_handle)
{
	typedef void __cdecl close_cache_header(HANDLE *a1);
	auto close_cache_header_impl = GetAddress<close_cache_header>(0x64C03, 0x4CC5A);
	close_cache_header_impl(map_handle);
}

static std::wstring_convert<std::codecvt_utf8<wchar_t>> wstring_to_string;

int __cdecl validate_and_add_custom_map(BYTE *a1)
{
	cache_header header;
	HANDLE map_cache_handle;
	wchar_t *file_name = (wchar_t*)a1 + 1216;
	if (!open_cache_header(file_name, &header, &map_cache_handle))
		return false;
	if (header.magic != 'head' || header.foot != 'foot' || header.file_size <= 0 || header.engine_gen != 8)
	{
		TRACE_FUNC("\"%s\" has invalid header", file_name);
		return false;
	}
	if (header.type > 5 || header.type < 0)
	{
		TRACE_FUNC("\"%s\" has bad scenario type", file_name);
		return false;
	}
	if (strnlen_s(header.name, 0x20u) >= 0x20 || strnlen_s(header.version, 0x20) >= 0x20)
	{
		TRACE_FUNC("\"%s\" has invalid version or name string", file_name);
		return false;
	}
	if (header.type != cache_header::scnr_type::Multiplayer && header.type != cache_header::scnr_type::SinglePlayer)
	{
		TRACE_FUNC("\"%s\" is not playable", file_name);
		return false;
	}

	close_cache_header(&map_cache_handle);
	// needed because the game loads the human readable map name and description from scenario after checks
	// without this the map is just called by it's file name

	// todo move the code for loading the descriptions to our code and get rid of this
	typedef int __cdecl validate_and_add_custom_map_interal(BYTE *a1);
	auto validate_and_add_custom_map_interal_impl = GetAddress<validate_and_add_custom_map_interal>(0x4F690, 0x56890);
	if (!validate_and_add_custom_map_interal_impl(a1))
	{
		TRACE_FUNC("warning \"%s\" has bad checksums or is blacklisted, map may not work correctly", file_name);
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
		TRACE_FUNC_N("Build '%s' is not offically supported consider repacking and updating map with supported tools", build);
	}
	return true;
}

#pragma endregion

typedef int(__cdecl *tfn_c0017a25d)(DWORD, DWORD*);
tfn_c0017a25d pfn_c0017a25d;
int __cdecl fn_c0017a25d(DWORD a1, DWORD* a2)
{
	if (H2Config_hitmarker_sound) {
		typedef unsigned long long QWORD;
		QWORD xuid_p1 = *(QWORD*)((BYTE*)H2BaseAddr + 0x51A629);

		int i = 0;
		for (; i < 16; i++) {
			QWORD xuid_list_ele = *(QWORD*)((BYTE*)H2BaseAddr + 0x968F68 + (8 * i));
			if (xuid_list_ele == xuid_p1) {
				break;
			}
		}

		int local_player_datum = i < 16 ? h2mod->get_unit_datum_from_player_index(i) : -1;

		//if (local_player_datum != -1 && a1 == local_player_datum && a2[4] != -1) {
		if (local_player_datum != -1 && a2[4] == local_player_datum && a1 != -1 && a1 != local_player_datum) {
			for (i = 0; i < 16; i++) {
				int other_datum = h2mod->get_unit_datum_from_player_index(i);
				if (a1 == other_datum) {
					if (h2mod->get_unit_team_index(local_player_datum) != h2mod->get_unit_team_index(other_datum)) {
						std::unique_lock<std::mutex> lck(h2mod->sound_mutex);
						h2mod->SoundMap[L"sounds/Halo1PCHitSound.wav"] = 0;
						//unlock immediately after modifying sound map
						lck.unlock();
						h2mod->sound_cv.notify_one();
					}
					break;
				}
			}
		}
	}
	int result = pfn_c0017a25d(a1, a2);
	return result;
}


typedef char(__stdcall *tfn_c0024eeef)(DWORD*, int, int);
tfn_c0024eeef pfn_c0024eeef;
char __stdcall fn_c0024eeef(DWORD* thisptr, int a2, int a3)//__thiscall
{
	//char result = pfn_c0024eeef(thisptr, a2, a3);
	//return result;

	char(__thiscall* fn_c002139f8)(DWORD*, int, int, int, int*, int) = (char(__thiscall*)(DWORD*, int, int, int, int*, int))(GetAddress(0x002139f8));

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

	int(__stdcall* fn_c0024f9a1)(int) = (int(__stdcall*)(int))(GetAddress(0x0024f9a1));
	int(__stdcall* fn_c0024f9dd)(int) = (int(__stdcall*)(int))(GetAddress(0x0024f9dd));
	int(__stdcall* fn_c0024ef79)(int) = (int(__stdcall*)(int))(GetAddress(0x0024ef79));
	int(__stdcall* fn_c0024f5fd)(DWORD* thisptr, int) = (int(__stdcall*)(DWORD*, int))(GetAddress(0x0024f5fd));
	int(__thiscall* fn_c0024f015)(DWORD* thisptr, int) = (int(__thiscall*)(DWORD*, int))(GetAddress(0x0024f015));
	int(__thiscall* fn_c0024f676)(DWORD* thisptr, int) = (int(__thiscall*)(DWORD*, int))(GetAddress(0x0024f676));
	int(__thiscall* fn_c0024f68a)(DWORD* thisptr, int) = (int(__thiscall*)(DWORD*, int))(GetAddress(0x0024f68a));

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
			result = fn_c0024f5fd(thisptr, a2);//party management
			break;
		case 4:
			GSCustomMenuCall_AdvLobbySettings3();
			break;
		case 5:
			result = fn_c0024f015(thisptr, a2);
			break;
		case 6:
			result = fn_c0024f676(thisptr, a2);
			break;
		case 7:
			result = fn_c0024f68a(thisptr, a2);
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

	DWORD* var_c003d9254 = (DWORD*)(GetAddress(0x003d9254));
	DWORD* var_c003d9188 = (DWORD*)(GetAddress(0x003d9188));

	DWORD*(__thiscall* fn_c00213b1c)(DWORD* thisptr, int) = (DWORD*(__thiscall*)(DWORD*, int))(GetAddress(0x00213b1c));
	int(__thiscall* fn_c0000a551)(DWORD* thisptr) = (int(__thiscall*)(DWORD*))(GetAddress(0x0000a551));
	DWORD*(__thiscall* fn_c0021ffc9)(DWORD* thisptr) = (DWORD*(__thiscall*)(DWORD*))(GetAddress(0x0021ffc9));
	void(__stdcall* fn_c0028870b)(int, int, int, DWORD*(__thiscall*)(DWORD*), int(__thiscall*)(DWORD*)) = (void(__stdcall*)(int, int, int, DWORD*(__thiscall*)(DWORD*), int(__thiscall*)(DWORD*)))(GetAddress(0x0028870b));
	DWORD*(__thiscall* fn_c002113c6)(DWORD* thisptr) = (DWORD*(__thiscall*)(DWORD*))(GetAddress(0x002113c6));
	int(__thiscall* fn_c0024fa19)(DWORD* thisptr, int, int*) = (int(__thiscall*)(DWORD*, int, int*))(GetAddress(0x0024fa19));
	int(*fn_c00215ea9)() = (int(*)())(GetAddress(0x00215ea9));
	int(__cdecl* fn_c0020d1fd)(char*, int numberOfButtons, int) = (int(__cdecl*)(char*, int, int))(GetAddress(0x0020d1fd));
	int(__cdecl* fn_c00066b33)(int) = (int(__cdecl*)(int))(GetAddress(0x00066b33));
	int(__cdecl* fn_c000667a0)(int) = (int(__cdecl*)(int))(GetAddress(0x000667a0));
	int(*fn_c002152b0)() = (int(*)())(GetAddress(0x002152b0));
	int(*fn_c0021525a)() = (int(*)())(GetAddress(0x0021525a));
	int(__thiscall* fn_c002113d3)(DWORD* thisptr, DWORD*) = (int(__thiscall*)(DWORD*, DWORD*))(GetAddress(0x002113d3));

	DWORD* v2 = thisptr;
	fn_c00213b1c(thisptr, a2);
	//*v2 = &c_squad_settings_list::`vftable';
	v2[0] = (DWORD)var_c003d9254;
	//*v2 = (DWORD)((BYTE*)H2BaseAddr + 0x003d9254);
	fn_c0028870b((int)(v2 + 44), 132, 7, fn_c0021ffc9, fn_c0000a551);
	fn_c002113c6(v2 + 276);
	//v2[275] = &c_slot2<c_squad_settings_list, s_event_record *, long>::`vftable';
	v2[275] = (DWORD)var_c003d9188;
	//v2[275] = (DWORD)((BYTE*)H2BaseAddr + 0x003d9188);
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
		*(WORD *)(*(DWORD *)(v2[28] + 68) + 4 * (unsigned __int16)fn_c000667a0(v2[28]) + 2) = 4;
		/*(WORD *)(*(DWORD *)(v2[28] + 68) + 4 * (unsigned __int16)fn_c000667a0(v2[28]) + 2) = 5;
		*(WORD *)(*(DWORD *)(v2[28] + 68) + 4 * (unsigned __int16)fn_c000667a0(v2[28]) + 2) = 6;
		*(WORD *)(*(DWORD *)(v2[28] + 68) + 4 * (unsigned __int16)fn_c000667a0(v2[28]) + 2) = 7;*/
		break;
	case 5:
		*(WORD *)(*(DWORD *)(v2[28] + 68) + 4 * (unsigned __int16)fn_c000667a0(v2[28]) + 2) = 0;
		*(WORD *)(*(DWORD *)(v2[28] + 68) + 4 * (unsigned __int16)fn_c000667a0(v2[28]) + 2) = 1;
		*(WORD *)(*(DWORD *)(v2[28] + 68) + 4 * (unsigned __int16)fn_c000667a0(v2[28]) + 2) = 2;
		*(WORD *)(*(DWORD *)(v2[28] + 68) + 4 * (unsigned __int16)fn_c000667a0(v2[28]) + 2) = 4;
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

//Patch Call to modify tags just after map load
char _cdecl LoadTagsandMapBases(int a)
{
	char(__cdecl* LoadTagsandMapBases_Orig)(int) = (char(__cdecl*)(int))(GetAddress(0x00031348));

	char result = LoadTagsandMapBases_Orig(a);

	H2Tweaks::RadarPatch();  //PATCHES RADAR IN MULTIPLAYER  DOCUMENTATION FOR YOSHI  #HPV

	return result;
}

char is_remote_desktop()
{
	TRACE_FUNC("check disabled");
	return 0;
}

void InitH2Tweaks() {
	postConfig();

	addDebugText("Begin Startup Tweaks.");

	MapChecksumSync::Init();
	
	if (H2IsDediServer) {
		DWORD dwBack;

		phookServ1 = (thookServ1)DetourFunc((BYTE*)H2BaseAddr + 0x8EFA, (BYTE*)LoadRegistrySettings, 11);
		VirtualProtect(phookServ1, 4, PAGE_EXECUTE_READWRITE, &dwBack);

		//phookServ2 = (thookServ2)DetourFunc((BYTE*)H2BaseAddr + 0xBA3C, (BYTE*)PreReadyLoad, 11);
		//VirtualProtect(phookServ2, 4, PAGE_EXECUTE_READWRITE, &dwBack);
	}
	else {//is client

		DWORD dwBack;
		//Hook a function which changes the party privacy to detect if the lobby becomes open.
		//Problem is if you want to set it via mem poking, it won't push the lobby to the master automatically.
		//phookChangePrivacy = (thookChangePrivacy)DetourFunc((BYTE*)H2BaseAddr + 0x2153ce, (BYTE*)HookChangePrivacy, 11);
		//VirtualProtect(phookChangePrivacy, 4, PAGE_EXECUTE_READWRITE, &dwBack);

		//Scrapped for now, maybe.
		DWORD tempResX = 0;
		DWORD tempResY = 0;

		HKEY hKeyResolution = NULL;
		if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Halo 2\\Video Settings", 0, KEY_READ, &hKeyResolution) == ERROR_SUCCESS) {
			GetDWORDRegKey(hKeyResolution, L"ScreenResX", &tempResX);
			GetDWORDRegKey(hKeyResolution, L"ScreenResY", &tempResY);
			RegCloseKey(hKeyResolution);
		}

		if (H2Config_custom_resolution_x > 0 && H2Config_custom_resolution_y > 0) {
			if (H2Config_custom_resolution_x != (int)tempResX || H2Config_custom_resolution_y != (int)tempResY) {
				LSTATUS err;
				if (err = RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Halo 2\\Video Settings", 0, KEY_ALL_ACCESS, &hKeyResolution) == ERROR_SUCCESS) {
					RegSetValueEx(hKeyResolution, L"ScreenResX", NULL, REG_DWORD, (const BYTE*)&H2Config_custom_resolution_x, sizeof(H2Config_custom_resolution_x));
					RegSetValueEx(hKeyResolution, L"ScreenResY", NULL, REG_DWORD, (const BYTE*)&H2Config_custom_resolution_y, sizeof(H2Config_custom_resolution_y));
					RegCloseKey(hKeyResolution);
				}
				else {
					char errorMsg[200];
					sprintf(errorMsg, "Error: 0x%x. Unable to make Screen Resolution changes.\nPlease try restarting Halo 2 with Administrator Privileges.", err);
					addDebugText(errorMsg);
					MessageBoxA(NULL, errorMsg, "Registry Write Error", MB_OK);
					exit(EXIT_FAILURE);
				}
			}
		}

		bool IntroHQ = true;//clients should set on halo2.exe -highquality

		if (!H2Config_skip_intro && IntroHQ) {
			BYTE assmIntroHQ[] = { 0xEB };
			WriteBytes(H2BaseAddr + 0x221C29, assmIntroHQ, 1);
		}

		//Disables the ESRB warning (only occurs for English Language).
		//disables the one if no intro vid occurs.
		WriteValue<BYTE>(H2BaseAddr + 0x411030, 0);
		//disables the one after the intro video.
		BYTE assmIntroESRBSkip[] = { 0x00 };
		WriteBytes(H2BaseAddr + 0x3a0fa, assmIntroESRBSkip, 1);
		WriteBytes(H2BaseAddr + 0x3a1ce, assmIntroESRBSkip, 1);

		//Set the LAN Server List Ping Frequency (milliseconds).
		WriteValue(H2BaseAddr + 0x001e9a89, 3000);
		//Set the LAN Server List Delete Entry After (milliseconds).
		WriteValue(H2BaseAddr + 0x001e9b0a, 9000);

		//Redirects the is_campaign call that the in-game chat renderer makes so we can show/hide it as we like.
		PatchCall(H2BaseAddr + 0x22667B, NotDisplayIngameChat);
		PatchCall(H2BaseAddr + 0x226628, NotDisplayIngameChat);

		//hook the gui popup for when the player is booted.
		sub_20E1D8 = (int(__cdecl*)(int, int, int, int, int, int))((char*)H2BaseAddr + 0x20E1D8);
		PatchCall(H2BaseAddr + 0x21754C, &sub_20E1D8_boot);

		//Hook for Hitmarker sound effect.
		pfn_c0017a25d = (tfn_c0017a25d)DetourFunc((BYTE*)H2BaseAddr + 0x0017a25d, (BYTE*)fn_c0017a25d, 10);
		VirtualProtect(pfn_c0017a25d, 4, PAGE_EXECUTE_READWRITE, &dwBack);

		//Hook for advanced lobby options.
		pfn_c0024eeef = (tfn_c0024eeef)DetourClassFunc((BYTE*)H2BaseAddr + 0x0024eeef, (BYTE*)fn_c0024eeef, 9);
		VirtualProtect(pfn_c0024eeef, 4, PAGE_EXECUTE_READWRITE, &dwBack);
		pfn_c0024fa19 = (tfn_c0024fa19)DetourClassFunc((BYTE*)H2BaseAddr + 0x0024fa19, (BYTE*)fn_c0024fa19, 9);
		VirtualProtect(pfn_c0024fa19, 4, PAGE_EXECUTE_READWRITE, &dwBack);
		pfn_c0024fabc = (tfn_c0024fabc)DetourClassFunc((BYTE*)H2BaseAddr + 0x0024fabc, (BYTE*)fn_c0024fabc, 13);
		VirtualProtect(pfn_c0024fabc, 4, PAGE_EXECUTE_READWRITE, &dwBack);

		WriteJmpTo(H2BaseAddr + 0x4544, is_init_flag_set);
		PatchCall(H2BaseAddr + 0x3166B, (DWORD)LoadTagsandMapBases);

		WriteJmpTo(GetAddress(0x7E43), WinMain);
		WriteJmpTo(GetAddress(0x39EA2), is_remote_desktop);

		//Redirect the variable for the server name to ours.
		WriteValue(H2BaseAddr + 0x001b2ce8, (DWORD)ServerLobbyName);
	}	
  
	// Both server and client
	WriteJmpTo(GetAddress(0x1467, 0x12E2), is_supported_build);
	PatchCall(GetAddress(0x1E49A2, 0x1EDF0), validate_and_add_custom_map);
	PatchCall(GetAddress(0x4D3BA, 0x417FE), validate_and_add_custom_map);
	PatchCall(GetAddress(0x4CF26, 0x41D4E), validate_and_add_custom_map);
	PatchCall(GetAddress(0x8928, 0x1B6482), validate_and_add_custom_map);

	H2Tweaks::applyPlayersActionsUpdateRatePatch();
	
	addDebugText("End Startup Tweaks.");
}

void DeinitH2Tweaks() {

}

static DWORD* get_scenario_global_address() {
	return (DWORD*)(H2BaseAddr + 0x479e74);
}

static int get_scenario_volume_count() {
	int volume_count = *(int*)(*get_scenario_global_address() + 0x108);
	return volume_count;
}

static void kill_volume_disable(int volume_id) {
	void(__cdecl* kill_volume_disable)(int volume_id);
	kill_volume_disable = (void(__cdecl*)(int))((char*)H2BaseAddr + 0xb3ab8);
	kill_volume_disable(volume_id);
}

void H2Tweaks::toggleKillVolumes(bool enable) {
	if (enable)
		return;
	//TODO 'bool enable'
	if (!h2mod->Server && gameManager->isHost()) {
		for (int i = 0; i < get_scenario_volume_count(); i++) {
			kill_volume_disable(i);
		}
	}
}

void H2Tweaks::setSens(InputType input_type, int sens) {

	if (H2IsDediServer)
		return;

	if (sens < 0)
		return; 

	int absSensIndex = sens - 1;

	if (input_type == CONTROLLER) { 
		*reinterpret_cast<float*>(H2BaseAddr + 0x4A89BC) = 40.0f + 10.0f * static_cast<float>(absSensIndex); //y-axis
		*reinterpret_cast<float*>(H2BaseAddr + 0x4A89B8) = 80.0f + 20.0f * static_cast<float>(absSensIndex); //x-axis
	}
	else if (input_type == MOUSE) { 
		*reinterpret_cast<float*>(H2BaseAddr + 0x4A89B4) = 25.0f + 10.0f * static_cast<float>(absSensIndex); //y-axis
		*reinterpret_cast<float*>(H2BaseAddr + 0x4A89B0) = 50.0f + 20.0f * static_cast<float>(absSensIndex); //x-axis
	}
}

void H2Tweaks::setFOV(int field_of_view_degrees) {

	if (H2IsDediServer)
		return;

	if (field_of_view_degrees > 0 && field_of_view_degrees <= 110)
	{
		float current_FOV = *reinterpret_cast<float*>(H2BaseAddr + 0x41D984);

		//int res_width = *(int*)(H2BaseAddr + 0xA3DA00); //wip
		//int res_height = *(int*)(H2BaseAddr + 0xA3DA04);

		const float default_radians_FOV = 70.0f * M_PI / 180.0f;

		float calculated_radians_FOV = ((float)field_of_view_degrees * M_PI / 180.0f) / default_radians_FOV;
		*reinterpret_cast<float*>(H2BaseAddr + 0x41D984) = calculated_radians_FOV; // First Person
		*reinterpret_cast<float*>(H2BaseAddr + 0x413780) = calculated_radians_FOV + 0.22f; // Third Person
	}
}

void H2Tweaks::setCrosshairPos(float crosshair_offset) {

	if (H2IsDediServer)
		return;

	if (!FloatIsNaN(crosshair_offset)) {
		DWORD CrosshairY = *(DWORD*)(H2BaseAddr + 0x479E70) + 0x1AF4 + 0xF0 + 0x1C;
		*reinterpret_cast<float*>(CrosshairY) = crosshair_offset;
	}
}

void H2Tweaks::setCrosshairSize(int size, bool preset) {
	if (H2IsDediServer)
		return;

	DWORD BATRIF1 = *(DWORD*)(H2BaseAddr + 0x479E70) + 0x7aa750;
	DWORD BATRIF2 = *(DWORD*)(H2BaseAddr + 0x479E70) + 0x7aa752;
	DWORD SMG1 = *(DWORD*)(H2BaseAddr + 0x479E70) + 0x7A9F9C;
	DWORD SMG2 = *(DWORD*)(H2BaseAddr + 0x479E70) + 0x7A9F9E;
	DWORD CRBN1 = *(DWORD*)(H2BaseAddr + 0x479E70) + 0x7ab970;
	DWORD CRBN2 = *(DWORD*)(H2BaseAddr + 0x479E70) + 0x7ab972;
	DWORD BEAMRIF1 = *(DWORD*)(H2BaseAddr + 0x479E70) + 0x7AA838;
	DWORD BEAMRIF2 = *(DWORD*)(H2BaseAddr + 0x479E70) + 0x7AA83A;
	DWORD MAG1 = *(DWORD*)(H2BaseAddr + 0x479E70) + 0x7AA33C;
	DWORD MAG2 = *(DWORD*)(H2BaseAddr + 0x479E70) + 0x7AA33E;
	DWORD PLASRIF1 = *(DWORD*)(H2BaseAddr + 0x479E70) + 0x7AA16C;
	DWORD PLASRIF2 = *(DWORD*)(H2BaseAddr + 0x479E70) + 0x7AA16E;
	DWORD SHTGN1 = *(DWORD*)(H2BaseAddr + 0x479E70) + 0x7AA424;
	DWORD SHTGN2 = *(DWORD*)(H2BaseAddr + 0x479E70) + 0x7AA426;
	DWORD SNIP1 = *(DWORD*)(H2BaseAddr + 0x479E70) + 0x7AA994;
	DWORD SNIP2 = *(DWORD*)(H2BaseAddr + 0x479E70) + 0x7AA996;
	DWORD SWRD1 = *(DWORD*)(H2BaseAddr + 0x479E70) + 0x7AA8AC;
	DWORD SWRD2 = *(DWORD*)(H2BaseAddr + 0x479E70) + 0x7AA8AE;
	DWORD ROCKLAUN1 = *(DWORD*)(H2BaseAddr + 0x479E70) + 0x7AA3B0;
	DWORD ROCKLAUN2 = *(DWORD*)(H2BaseAddr + 0x479E70) + 0x7AA3B2;
	DWORD PLASPI1 = *(DWORD*)(H2BaseAddr + 0x479E70) + 0x7AA0F8;
	DWORD PLASPI2 = *(DWORD*)(H2BaseAddr + 0x479E70) + 0x7AA0FA;
	DWORD BRUTESHOT1 = *(DWORD*)(H2BaseAddr + 0x479E70) + 0x7AA7C4;
	DWORD BRUTESHOT2 = *(DWORD*)(H2BaseAddr + 0x479E70) + 0x7AA7C6;
	DWORD NEED1 = *(DWORD*)(H2BaseAddr + 0x479E70) + 0x7AA254;
	DWORD NEED2 = *(DWORD*)(H2BaseAddr + 0x479E70) + 0x7AA256;
	DWORD SENTBEAM1 = *(DWORD*)(H2BaseAddr + 0x479E70) + 0x7AB5D0;
	DWORD SENTBEAM2 = *(DWORD*)(H2BaseAddr + 0x479E70) + 0x7AB5D2;

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


	if (h2mod->GetEngineType() == EngineType::MULTIPLAYER_ENGINE) {

		
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

void H2Tweaks::applyShaderTweaks() {
	//patches in order to get elite glowing lights back on
	//thanks Himanshu for help

	if (H2IsDediServer)
		return;

	int materialIndex = 0;
	DWORD currentMaterial;
	DWORD shader;
	DWORD sharedMapAddr = *(DWORD*)(H2BaseAddr + 0x482290);
	DWORD tagMemOffset = 0xE67D7C;

	DWORD tagMem = sharedMapAddr + tagMemOffset;
	DWORD MaterialsMem = *(DWORD*)(tagMem + 0x60 + 4) + sharedMapAddr; // reflexive materials block
	//int count = *(int*)(tagMem + 0x60);

	for (materialIndex = 0; materialIndex < 19; materialIndex++) { // loop through materials
		currentMaterial = MaterialsMem + (0x20 * materialIndex);
		shader = currentMaterial + 0x8;

		if (materialIndex == 3 || materialIndex == 7) { // arms/hands shaders 
			*(DWORD*)(shader + 4) = 0xE3652900;
		}

		else if (materialIndex == 5 || materialIndex == 8) { //legs shaders
			*(DWORD*)(shader + 4) = 0xE371290C;
		}

		else if (materialIndex == 15) { // inset_lights_mp shader
			*(DWORD*)(shader + 4) = 0xE38E2929;
		}
	 }
}

char ret_0() {
	return 0; //for 60 fps cinematics
}

void H2Tweaks::enable60FPSCutscenes() {

	if (H2IsDediServer)
		return;

	//60 fps cinematics enable
	PatchCall(H2BaseAddr + 0x97774, ret_0);
	PatchCall(H2BaseAddr + 0x7C378, ret_0);

}

void H2Tweaks::disable60FPSCutscenes() {

	if (H2IsDediServer)
		return;

	typedef char(__cdecl *is_cutscene_fps_cap)(); //restore orig func
	is_cutscene_fps_cap pIs_cutscene_fps_cap = (is_cutscene_fps_cap)(H2BaseAddr + 0x3A938);

	//60 fps cinematics disable
	PatchCall(H2BaseAddr + 0x97774, pIs_cutscene_fps_cap);
	PatchCall(H2BaseAddr + 0x7C378, pIs_cutscene_fps_cap);
}

void H2Tweaks::enableAI_MP() {

	if (H2IsDediServer) //TODO: get server offset
		return;

	BYTE jmp[1] = { JMP_RAW_BYTE };
	WriteBytes(H2BaseAddr + 0x30E684, jmp, 0x1); //AI_MP enable patch
}

void H2Tweaks::disableAI_MP() {

	if (H2IsDediServer) 
		return;

	BYTE jnz[1] = { JNZ_RAW_BYTE };
	WriteBytes(H2BaseAddr + 0x30E684, jnz, 0x1); //AI_MP disable patch
}

void H2Tweaks::PatchPingMeterCheck() {
	//halo2.exe+1D4E35 

	if (H2IsDediServer)
		return;

	BYTE assmPatchPingCheck[2] = { 0x75, 0x18 };
	WriteBytes(H2BaseAddr + 0x1D4E35, assmPatchPingCheck, 2);
}

void H2Tweaks::FixRanksIcons() {

	if (H2IsDediServer)
		return;

	DWORD shared_Meta_Data_ptr = *(DWORD*)(H2BaseAddr + 0x47CD64);

	//Tag : ui\player_skins\player_skin_lobby.skin
	const DWORD tag_offset_pre = 0x0049DE90;	//Property : Bitmap Buttons

	//Tag : ui\player_skins\pcr_1.skin
	const DWORD tag_offset_pcr1 = 0x00485C70;	//Property : Bitmap Buttons

	//Tag : ui\player_skins\pcr_2.skin
	const DWORD tag_offset_pcr2 = 0x00485F50;	//Property : Bitmap Buttons


	const BYTE index_offset_p = 0x38;			//Property size per chunk
	const int index_number = 2;					//Property chunk index number (2 represents rank field)
	const BYTE x_pos_off = 0x0C;				//Definition : Horizontal position on mainmenu
	const WORD x_pos_pre = 0x01C4;				//Value : 452 (decimal)
	const WORD x_pos_pcr = 0x0131;				//Value : 305 (decimal)
	const BYTE y_pos_off = 0x0E;				//Definition : Vertical position on mainmenu
	const WORD y_pos_pre = 0x001A;				//Value : 26 (decimal)
	const WORD y_pos_pcr = 0x0017;				//Value : 23 (decimal)
	const BYTE bitm_offset = 0x18;				//Definition : Bitm (bitmap loaded based on datum index)
	DWORD bitm_type;							//Value : Datum index value for Bitm definition

	//Tag : ui\global_bitmaps\rank_icons.bitm
	const DWORD rank_icons = 0xE50802E6;		//Bitmap Datum Index
	const DWORD tag_offset_r = 0x0047E9A8;		//Property : Bitmap

	//Tag : ui\global_bitmaps\rank_icons.bitm
	const DWORD rank_icons_sm = 0xE4EC02CA;		//Bitmap Datum Index
	const DWORD tag_offset_sm = 0x00476D3C;		//Property : Bitmap


	const BYTE index_offset_rank = 0x74;		//Property size per chunk
	int index_number_rank;						//Property chunk index number (0-49)
	const BYTE width_offset = 0x04;				//Definition : Width
	const WORD width_value = 0x0020;			//Value : 32 (decimal)
	const BYTE height_offset = 0x06;			//Definition : Height
	const WORD height_value = 0x0020;			//Value : 32 (decimal)


	if (h2mod->GetEngineType() == EngineType::MAIN_MENU_ENGINE) {
		//Sets Pregame Lobby 
		WriteValue(shared_Meta_Data_ptr + tag_offset_pre + (index_number * index_offset_p) + x_pos_off, x_pos_pre);
		WriteValue(shared_Meta_Data_ptr + tag_offset_pre + (index_number * index_offset_p) + y_pos_off, y_pos_pre);
		WriteValue(shared_Meta_Data_ptr + tag_offset_pre + (index_number * index_offset_p) + bitm_offset, rank_icons_sm);

		//Sets Postgame Carnage Report 1
		WriteValue(shared_Meta_Data_ptr + tag_offset_pcr1 + (index_number * index_offset_p) + x_pos_off, x_pos_pcr);
		WriteValue(shared_Meta_Data_ptr + tag_offset_pcr1 + (index_number * index_offset_p) + y_pos_off, y_pos_pcr);
		WriteValue(shared_Meta_Data_ptr + tag_offset_pcr1 + (index_number * index_offset_p) + bitm_offset, rank_icons_sm);

		//Sets Postgame Carnage Report 2
		WriteValue(shared_Meta_Data_ptr + tag_offset_pcr2 + (index_number * index_offset_p) + x_pos_off, x_pos_pcr);
		WriteValue(shared_Meta_Data_ptr + tag_offset_pcr2 + (index_number * index_offset_p) + y_pos_off, y_pos_pcr);
		WriteValue(shared_Meta_Data_ptr + tag_offset_pcr2 + (index_number * index_offset_p) + bitm_offset, rank_icons_sm);

		//Sets Ranks 
		for (index_number_rank = 0; index_number_rank < 49; index_number_rank++)
		{
			//Setting Rank bitmap size
			WriteValue(shared_Meta_Data_ptr + tag_offset_r + (index_number_rank * index_offset_rank) + width_offset, width_value);
			WriteValue(shared_Meta_Data_ptr + tag_offset_r + (index_number_rank * index_offset_rank) + height_offset, height_value);

			//Setting Small Rank bitmap size
			WriteValue(shared_Meta_Data_ptr + tag_offset_sm + (index_number_rank * index_offset_rank) + width_offset, width_value);
			WriteValue(shared_Meta_Data_ptr + tag_offset_sm + (index_number_rank * index_offset_rank) + height_offset, height_value);
		}
	}
}

void H2Tweaks::RadarPatch() {

	if (H2IsDediServer)
		return;

	DWORD& MapHeaderType = *(DWORD*)(H2BaseAddr + 0x47CD68 + 0x14C);

	if (MapHeaderType != 1)
		return;

	DWORD shared_Meta_Data_ptr = *(DWORD*)(H2BaseAddr + 0x47CD64);

	//Tag : ui\hud\bitmaps\hud_sweeper.bitm
	const DWORD tag_offset = 0x00D93BA8;	//Property : Bitmap
	const BYTE format_offset = 0x0C;		//Definition : Format Offset
	const WORD format_type = 0x000A;		//Definition : Format Enum

	WriteValue<WORD>(shared_Meta_Data_ptr + tag_offset + format_offset, format_type);
}

float* xb_tickrate_flt;
__declspec(naked) void calculate_delta_time(void)
{
	__asm
	{
		mov eax, xb_tickrate_flt
		fld dword ptr[eax]
		fmul dword ptr[esp + 4]
		retn
	}
}


void H2Tweaks::applyPlayersActionsUpdateRatePatch()
{
	xb_tickrate_flt = GetAddress<float>(0x3BBEB4, 0x378C84);
	PatchCall(GetAddress(0x1E12FB, 0x1C8327), calculate_delta_time); // inside update_player_actions()
}
