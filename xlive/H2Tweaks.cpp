#include "stdafx.h"
#include <ShellAPI.h>
#include "H2Tweaks.h"
#include "H2Startup.h"
#include "H2Config.h"
#include "GSUtils.h"
#include "Hook.h"
#include "H2OnscreenDebugLog.h"
#include "GSCustomMenu.h"
#include "H2MOD.h";
#include <string>

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


#pragma region SLDL_Hack
template <typename T = void>
static inline T *GetAddress(DWORD client, DWORD server = 0)
{
	return reinterpret_cast<T*>(H2BaseAddr + (H2IsDediServer ? server : client));
}

typedef bool(*tfn_c00004a6b)();
tfn_c00004a6b pfn_c00004a6b;
bool fn_c00004a6b()
{
	// Since engine_basic_init has been reversed and the anti-hack voided, this function (which appears to be the game's weird anti-hack initializer) is no longer required.
	bool result = true;//pfn_c00004a6b();
	return result;
}

typedef bool(*tfn_c00004567)();
tfn_c00004567 pfn_c00004567;

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
	unk14, // maybe UI?
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

	HANDLE(*fn_c000285fd)() = (HANDLE(*)())(GetAddress( 0x000285fd));

	init_gfwl_gamestore();
	init_data_checksum_info();
	runtime_state_init();

	int arg_count;
	wchar_t **cmd_line_args = CommandLineToArgvW(GetCommandLineW(), &arg_count);
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
#ifdef _DEBUG
			else if (wcsnicmp(cmd_line_arg, L"-dev_flag:", 10) == 0) {
				int flag_id = _wtol(&cmd_line_arg[10]);
				flags_array[min(max(0, flag_id), flags::count - 1)] = 1;
			}
#endif
		}
	} else {
		TRACE_GAME("Failed to get commandline args. LAST_ERROR: %X", GetLastError());
	}
	LocalFree(cmd_line_args);

	if (flags_array[flags::unk22])
		init_timing(1000 * flags_array[flags::unk22]);
	real_math_initialize();
	async_initialize();
	global_preferences_initialize();
	font_initialize();
	if (!tag_files_open())
		return false;
	void *var_c004ae8e0 = GetAddress(0x004ae8e0);
	game_state_initialize(var_c004ae8e0);

	// modifies esi need to check what the caller sets that too
	//char(*fn_c001a9de6)() = (char(*)())(GetAddress(0x001a9de6));
	//char result_c001a9de6 = fn_c001a9de6();
	if (!rasterizer_initialize())
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
	auto open_cache_header_impl = GetAddress<open_cache_header>(0x642D0);
	return open_cache_header_impl(lpFileName, cache_header_ptr, map_handle, 0);
}

int __cdecl validate_and_add_custom_map(BYTE *a1)
{
	cache_header header;
	HANDLE map_cache_handle;
	const wchar_t *file_name = (wchar_t*)a1 + 1216;
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
	// needed because the game loads the human readable map name and description from scenario after checks
	// without this the map is just called by it's file name

	// todo move the code for loading the descriptions to our code and get rid of this
	typedef int __cdecl validate_and_add_custom_map_interal(BYTE *a1);
	auto validate_and_add_custom_map_interal_impl = GetAddress<validate_and_add_custom_map_interal>(0x4F690);
	if (!validate_and_add_custom_map_interal_impl(a1))
	{
		TRACE_FUNC("warning \"%s\" has bad checksums or is blacklisted, map may not work correctly", file_name);
	}
	// load the map even if some of the checks failed, will still mostly work
	return true;
}

bool __cdecl is_supported_build(char *build)
{
	return true;
}

#pragma endregion

void InitH2Tweaks() {
	postConfig();

	addDebugText("Begin Startup Tweaks.");

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

		if (H2Config_skip_intro) {
			BYTE assmIntroSkip[] = { 0x3F };
			WriteBytes(H2BaseAddr + 0x221C0E, assmIntroSkip, 1);
		}

		if (!H2Config_skip_intro && IntroHQ) {
			BYTE assmIntroHQ[] = { 0xEB };
			WriteBytes(H2BaseAddr + 0x221C29, assmIntroHQ, 1);
		}

		//Allows unlimited clients
		BYTE assmUnlimitedClients[41];
		memset(assmUnlimitedClients, 0x00, 41);
		WriteBytes(H2BaseAddr + 0x39BCF0, assmUnlimitedClients, 41);

		//Allows on a remote desktop connection
		BYTE assmRemoteDesktop[] = { 0xEB };
		WriteBytes(H2BaseAddr + 0x7E54, assmRemoteDesktop, 1);

		//Disables the ESRB warning (only occurs for English Language).
		//disables the one if no intro vid occurs.
		BYTE& ESRB = *((BYTE*)H2BaseAddr + 0x411030);
		ESRB = 0;
		//disables the one after the intro video.
		BYTE assmIntroESRBSkip[] = { 0x00 };
		WriteBytes(H2BaseAddr + 0x3a0fa, assmIntroESRBSkip, 1);
		WriteBytes(H2BaseAddr + 0x3a1ce, assmIntroESRBSkip, 1);

		//Redirects the is_campaign call that the in-game chat renderer makes so we can show/hide it as we like.
		PatchCall(H2BaseAddr + 0x22667B, NotDisplayIngameChat);
		PatchCall(H2BaseAddr + 0x226628, NotDisplayIngameChat);

		//hook the gui popup for when the player is booted.
		sub_20E1D8 = (int(__cdecl*)(int, int, int, int, int, int))((char*)H2BaseAddr + 0x20E1D8);
		PatchCall(H2BaseAddr + 0x21754C, &sub_20E1D8_boot);

		//SLDL anti-hack removed and code is now accessible.
		//Side effect is that the multiplayer buttons at the mainmenu will never be greyed out due to a bad 'key'.
		pfn_c00004a6b = (tfn_c00004a6b)DetourFunc((BYTE*)H2BaseAddr + 0x00004a6b, (BYTE*)fn_c00004a6b, 5);
		VirtualProtect(pfn_c00004a6b, 4, PAGE_EXECUTE_READWRITE, &dwBack);
		pfn_c00004567 = (tfn_c00004567)DetourFunc((BYTE*)H2BaseAddr + 0x00004567, (BYTE*)engine_basic_init, 7);
		VirtualProtect(pfn_c00004567, 4, PAGE_EXECUTE_READWRITE, &dwBack);

		WriteJmpTo(H2BaseAddr + 0x4544, is_init_flag_set);
		WriteJmpTo(H2BaseAddr + 0x1467, is_supported_build);
		PatchCall(H2BaseAddr + 0x1E49A2, validate_and_add_custom_map);
		PatchCall(H2BaseAddr + 0x4D3BA, validate_and_add_custom_map);
		PatchCall(H2BaseAddr + 0x4CF26, validate_and_add_custom_map);
		PatchCall(H2BaseAddr + 0x8928, validate_and_add_custom_map);
	}
	addDebugText("End Startup Tweaks.");
}

void DeinitH2Tweaks() {

}

void setSens(short input_type, int sens) {

	if (H2IsDediServer)
		return;

	if (sens < 0)
		return; 

	if (input_type == 1) { //controller
		*(float*)(H2BaseAddr + 0x4A89BC) = (float)(40.0f + 10.0f * sens); //y-axis
		*(float*)(H2BaseAddr + 0x4A89B8) = (float)(80.0f + 20.0f * sens); //x-axis
	}
	else if (input_type == 0) { //mouse 
		*(float*)(H2BaseAddr + 0x4A89B4) = (float)(25.0f + 10.0f * sens); //y-axis
		*(float*)(H2BaseAddr + 0x4A89B0) = (float)(50.0f + 20.0f * sens); //x-axis
	}
}

void H2Tweaks::setFOV(int field_of_view_degrees) {

	if (H2IsDediServer)
		return;

	if (field_of_view_degrees > 0 && field_of_view_degrees <= 110)
	{
		float current_FOV = *(float*)(H2BaseAddr + 0x41D984);

		//int res_width = *(int*)(H2BaseAddr + 0xA3DA00); //wip
		//int res_height = *(int*)(H2BaseAddr + 0xA3DA04);

		const float default_radians_FOV = 70.0f * M_PI / 180.0f;

		float calculated_radians_FOV = ((float)field_of_view_degrees * M_PI / 180.0f) / default_radians_FOV;
		*(float*)(H2BaseAddr + 0x41D984) = calculated_radians_FOV; // First Person
		*(float*)(H2BaseAddr + 0x413780) = calculated_radians_FOV + 0.22f; // Third Person
	}
}

void H2Tweaks::setCrosshairPos(float crosshair_offset) {

	if (H2IsDediServer)
		return;

	if (!FloatIsNaN(crosshair_offset)) {
		DWORD CrosshairY = *(DWORD*)(H2BaseAddr + 0x479E70) + 0x1AF4 + 0xF0 + 0x1C;
		*(float*)CrosshairY = crosshair_offset;
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


	if (h2mod->get_engine_type() == EngineType::MULTIPLAYER_ENGINE) {

		
		for (int i = 0; i < 28; i++) {
			if (configArray[i] == 0) {
				*(int*)WEAPONS[i] = disabled[i];
			}
			else if (*configArray[i] == 1 || *configArray[i] < 0 || *configArray[i] > 32767) {
				*(int*)WEAPONS[i] = defaultSize[i];
			}
			else {
				*(int*)WEAPONS[i] = *configArray[i];
			}

		}
		

	}
}

void H2Tweaks::applyHitfix() {
	//at some point we need to find a better way to fix this crap

	int offset = 0x47CD54;
	//TRACE_GAME("[h2mod] Hitfix is being run on Client!");
	if (H2IsDediServer) {
		offset = 0x4A29BC;
		//TRACE_GAME("[h2mod] Hitfix is actually being run on the Dedicated Server!");
	}

	DWORD AddressOffset = *(DWORD*)(H2BaseAddr + offset);

	*(float*)(AddressOffset + 0xA4EC88) = 1200.0f; // battle_rifle_bullet.proj Initial Velocity 
	*(float*)(AddressOffset + 0xA4EC8C) = 1200.0f; //battle_rifle_bullet.proj Final Velocity
	*(float*)(AddressOffset + 0xB7F914) = 4000.0f; //sniper_bullet.proj Initial Velocity
	*(float*)(AddressOffset + 0xB7F918) = 4000.0f; //sniper_bullet.proj Final Velocity
	//FIXME COOP will break because of one of these tags not existing.
	*(float*)(AddressOffset + 0xCE4598) = 4000.0f; //beam_rifle_beam.proj Initial Velocity
	*(float*)(AddressOffset + 0xCE459C) = 4000.0f; //beam_rifle_beam.proj Final Velocity
	*(float*)(AddressOffset + 0x81113C) = 200.0f; //gauss_turret.proj Initial Velocity def 90
	*(float*)(AddressOffset + 0x811140) = 200.0f; //gauss_turret.proj Final Velocity def 90
	*(float*)(AddressOffset + 0x97A194) = 800.0f; //magnum_bullet.proj initial def 400
	*(float*)(AddressOffset + 0x97A198) = 800.0f; //magnum_bullet.proj final def 400
	*(float*)(AddressOffset + 0x7E7E20) = 2000.0f; //bullet.proj (chaingun) initial def 800
	*(float*)(AddressOffset + 0x7E7E24) = 2000.0f; //bullet.proj (chaingun) final def 800

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

	int THINGY = *(int*)(H2BaseAddr + 0xA40564);
	BYTE* assmOffset = (BYTE*)(THINGY + 0x800);
	const int assmlen = 20;
	BYTE assmOrigRankIcon[assmlen] = { 0x92,0x00,0x14,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x6D,0x74,0x69,0x62,0xCA,0x02,0xEC,0xE4 };
	BYTE assmPatchFixRankIcon[assmlen] = { 0xCC,0x01,0x1C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x6D,0x74,0x69,0x62,0xE6,0x02,0x08,0xE5 };
	bool shouldPatch = true;
	for (int i = 0; i < assmlen; i++) {
		if (*(assmOffset + i) != assmOrigRankIcon[i]) {
			shouldPatch = false;
			break;
		}
	}
	if (shouldPatch) {
		WriteBytes((DWORD)assmOffset, assmPatchFixRankIcon, assmlen);
		addDebugText("Patching Rank Icon Fix.");
	}
}
