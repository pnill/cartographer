#include "stdafx.h"

#include "WinMainH2.h"
#include "..\Shell.h"
#include "..\Config.h"

#include "Blam\Engine\Networking\Transport\NetworkObserver.h"
#include "Util\Hooks\Hook.h"

const static int max_monitor_count = 9;

BOOL __cdecl is_init_flag_set(e_startup_flags id);
int WINAPI WinMain_Halo2(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd);

void InitH2WinMainPatches()
{
	if (Memory::IsDedicatedServer())
		return;

	WriteJmpTo(Memory::GetAddress(0x4544), is_init_flag_set);
	WriteJmpTo(Memory::GetAddress(0x7E43), WinMain_Halo2);
}

// WinMain replacement
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
		std::thread([=] {
			_Shell::OpenMessageBox(NULL, MB_ICONEXCLAMATION, "PCC Error", "Failed to get compatibility info."); }).detach();

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
	}
	else
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
	}
	else
	{
		return 0;
	}
}

bool engine_basic_init()
{
	DWORD* flags_array = Memory::GetAddress<DWORD*>(0x46d820);
	ZeroMemory(flags_array, e_startup_flags::count * sizeof(DWORD)); // should be zero initalized anyways but the game does it

	H2Config_voice_chat = false;
	flags_array[e_startup_flags::nointro] = H2Config_skip_intro;

	void(__cdecl * fn_c000285fd)() = (void(__cdecl*)())Memory::GetAddress<void*>(0x000285fd);

	gfwl_gamestore_initialize();
	init_data_checksum_info();
	runtime_state_initialize();

	if (H2Config_d3dex) {
		flags_array[e_startup_flags::allow_d3d_ex_version] = 1;
	}

	int arg_count;
	wchar_t** cmd_line_args = LOG_CHECK(CommandLineToArgvW(GetCommandLineW(), &arg_count));
	if (cmd_line_args && arg_count > 1) {
		for (int i = 1; i < arg_count; i++) {
			wchar_t* cmd_line_arg = cmd_line_args[i];

			if (_wcsicmp(cmd_line_arg, L"-windowed") == 0) {
				flags_array[e_startup_flags::windowed] = 1;
			}
			else if (_wcsicmp(cmd_line_arg, L"-nosound") == 0) {
				flags_array[e_startup_flags::nosound] = 1;
				WriteValue(Memory::GetAddress(0x479EDC), 1);
			}
			else if (_wcsicmp(cmd_line_arg, L"-novsync") == 0) {
				flags_array[e_startup_flags::novsync] = 1;
			}
			else if (_wcsicmp(cmd_line_arg, L"-nointro") == 0) {
				flags_array[e_startup_flags::nointro] = 1;
			}
			else if (_wcsnicmp(cmd_line_arg, L"-monitor:", 9) == 0) {
				int monitor_id = _wtol(&cmd_line_arg[9]);
				flags_array[e_startup_flags::monitor_count] = min(max(0, monitor_id), max_monitor_count);
			}
			else if (_wcsicmp(cmd_line_arg, L"-highquality") == 0) {
				flags_array[e_startup_flags::high_quality] = 1;
			}
			else if (_wcsicmp(cmd_line_arg, L"-disabledepthbias") == 0)
			{
				// Check github issue #118
				/* g_depth_bias always NULL rather than taking any value from
				shader tag before calling g_D3DDevice->SetRenderStatus(D3DRS_DEPTHBIAS, g_depth_bias); */
				NopFill(Memory::GetAddress(0x269FD5), 8);
			}
#if COMPILE_WITH_VOICE
			else if (_wcsicmp(cmd_line_arg, L"-voicechat") == 0)
			{
				flags_array[e_startup_flags::disable_voice_chat] = 0;
				H2Config_voice_chat = true;
			}
#endif
#ifdef _DEBUG
			else if (_wcsnicmp(cmd_line_arg, L"-dev_flag:", 10) == 0) {
				int flag_id = _wtol(&cmd_line_arg[10]);
				flags_array[min(max(0, flag_id), e_startup_flags::count - 1)] = 1;
			}
#endif
		}
	}
	LocalFree(cmd_line_args);

	if (flags_array[e_startup_flags::unk26])
		timing_initialize(1000 * flags_array[e_startup_flags::unk26]);
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
	// but just a stub in release
	void(__cdecl * network_configuration_initialize)() = (void(__cdecl*)())(Memory::GetAddress(0x001a9de6));
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
	LONG(__stdcall * XLivePBufferAllocate)(DWORD size, FakePBuffer * *pBuffer) = (LONG(__stdcall*)(DWORD, FakePBuffer**))Memory::GetAddress(0xe886);
	DWORD(__stdcall * XLivePBufferSetByte)(FakePBuffer * pBuffer, DWORD offset, BYTE value) = (DWORD(__stdcall*)(FakePBuffer*, DWORD, BYTE))Memory::GetAddress(0xe880);
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

BOOL __cdecl is_init_flag_set(e_startup_flags id)
{
	static int flag_log_count[e_startup_flags::count];
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

#pragma region Game Init functions

void real_math_initialize()
{
	typedef int(__cdecl* real_math_initialize_t)();
	auto p_real_math_initialize_impl = Memory::GetAddress<real_math_initialize_t>(0x000340d7);
	p_real_math_initialize_impl();
}

void async_initialize()
{
	typedef int(__cdecl* async_initialize_t)();
	auto p_async_initialize_impl = Memory::GetAddress<async_initialize_t>(0x00032ce5);
	p_async_initialize_impl();
}

bool gfwl_gamestore_initialize()
{
	typedef char(__cdecl* init_gfwl_gamestore_t)();
	auto p_init_gfwl_gamestore_impl = Memory::GetAddress<init_gfwl_gamestore_t>(0x00202f3e);
	return p_init_gfwl_gamestore_impl();
}

// not sure if this is all it does
HANDLE init_data_checksum_info()
{
	typedef HANDLE(__cdecl* init_data_checksum_info_t)();
	auto p_init_data_checksum_info_impl = Memory::GetAddress<init_data_checksum_info_t>(0x000388d3);
	return p_init_data_checksum_info_impl();
}

// returns memory
void* runtime_state_initialize()
{
	typedef void* (__cdecl* runtime_state_init_t)();
	auto p_runtime_state_init_impl = Memory::GetAddress<runtime_state_init_t>(0x00037ed5);
	return p_runtime_state_init_impl();
}

void game_preferences_initialize()
{
	typedef void(__cdecl* game_preferences_initialize_t)();
	auto p_global_preferences_initialize_impl = Memory::GetAddress<game_preferences_initialize_t>(0x325FD);
	p_global_preferences_initialize_impl();
}

void font_initialize()
{
	typedef void(__cdecl* font_initialize_t)();
	auto p_font_initialize_impl = Memory::GetAddress<font_initialize_t>(0x00031dff);
	p_font_initialize_impl();
}

bool tag_files_open()
{
	typedef bool(__cdecl* tag_files_open_t)();
	auto p_tag_files_open_impl = Memory::GetAddress<tag_files_open_t>(0x30D58);
	return p_tag_files_open_impl();
}

void timing_initialize(int a1)
{
	typedef DWORD(__cdecl* init_timing_t)(int a1);
	auto p_init_timing_impl = Memory::GetAddress<init_timing_t>(0x37E39);
	p_init_timing_impl(a1);
}

void game_state_initialize()
{
	typedef void(__cdecl* game_state_initialize_t)();
	auto p_game_state_initialize = Memory::GetAddress<game_state_initialize_t>(0x00030aa6);
	p_game_state_initialize();
}

bool rasterizer_initialize()
{
	typedef char(__cdecl* rasterizer_initialize_t)();
	auto p_rasterizer_initialize = Memory::GetAddress<rasterizer_initialize_t>(0x00263359);
	return p_rasterizer_initialize();
}

bool input_initialize()
{
	typedef char(__cdecl* input_initialize_t)();
	auto p_input_initialize = Memory::GetAddress<input_initialize_t>(0x2FD23);
	return p_input_initialize();
}

void sound_initialize()
{
	typedef void(__cdecl* sound_initialize_t)();
	auto p_sound_initialize = Memory::GetAddress<sound_initialize_t>(0x2979E);
	return p_sound_initialize();
}

bool InitPCCInfo()
{
	typedef bool(__cdecl* InitPCCInfo_t)();
	auto p_InitPCCInfo = Memory::GetAddress<InitPCCInfo_t>(0x260DDD);
	return p_InitPCCInfo();
}

void run_main_loop()
{
	typedef int(__cdecl* run_main_loop_t)();
	auto p_run_main_loop = Memory::GetAddress<run_main_loop_t>(0x39E2C);
	p_run_main_loop();
}

void main_engine_dispose()
{
	typedef int(__cdecl* main_engine_dispose_t)();
	auto p_main_engine_dispose = Memory::GetAddress<main_engine_dispose_t>(0x48A9);
	p_main_engine_dispose();
}

void show_error_message_by_id(int error_id)
{
	typedef void(__cdecl* show_error_message_by_id_t)(int id);
	auto p_show_error_message_by_id = Memory::GetAddress<show_error_message_by_id_t>(0x4A2E);
	p_show_error_message_by_id(error_id);
}

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

#pragma endregion