#include "stdafx.h"

#include "WinMainH2.h"

#include "Blam/Engine/shell/shell.h"
#include "Blam/Engine/Networking/Transport/NetworkObserver.h"
#include "Blam/Engine/saved_games/game_state.h"

#include "H2MOD/Modules/Shell/Config.h"
#include "H2MOD/Modules/Shell/H2MODShell.h""
#include "H2MOD/Utils/Utils.h"



const static int max_monitor_count = 9;

int WINAPI WinMain_Halo2(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd);

void InitH2WinMainPatches()
{
	if (Memory::IsDedicatedServer())
		return;

	WriteJmpTo(Memory::GetAddress(0x4544), shell_startup_flag_get);
	WriteJmpTo(Memory::GetAddress(0x7E43), WinMain_Halo2);
}

LRESULT WINAPI H2WndProc_hook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static FrequencyLimiter frqLimiter(150);
	bool* window_in_focus = Memory::GetAddress<bool*>(0x46DAD9);
	bool* unk_condition1 = Memory::GetAddress<bool*>(0x46DAD8);
	WNDPROC g_WndProc = Memory::GetAddress<WNDPROC>(0x790E);

	LRESULT result;
	if (uMsg != WM_SETCURSOR)
	{
		result = g_WndProc(hWnd, uMsg, wParam, lParam);
	}
	else
	{
		// if we have to set the cursor, limit the frequency of the cursor being set
		// because it's very heavy on the CPU
		bool enable_cursor = *window_in_focus && !*unk_condition1;
		// if the cursor is about to get disabled, reset the frqLimiter
		// and allow the cursor to be disabled
		if (!enable_cursor)
		{
			frqLimiter.Reset();
		}

		result = 1;

		// ShouldUpdate also updates the state of the frqLimiter
		if (frqLimiter.ShouldUpdate())
		{
			result = g_WndProc(hWnd, uMsg, wParam, lParam);
		}
	}

	return result;
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
	
	WriteValue(Memory::GetAddress(0x46D9D0), H2WndProc_hook); // g_WndProc_ptr

	if (!LOG_CHECK(InitPCCInfo()))
	{
		LOG_TRACE_FUNC("Failed to get PCC info / insufficient system resources");
		std::thread([]() {
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
	void(__cdecl * fn_c000285fd)() = Memory::GetAddress<void(__cdecl*)()>(0x285fd);

	for (int32 i = 0; i < k_startup_flags_count; i++)
	{
		shell_startup_flag_set((e_startup_flags)i, 0);
	}

	H2Config_voice_chat = false;
	shell_startup_flag_set(_startup_flag_nointro, H2Config_skip_intro);

	gfwl_gamestore_initialize();
	init_data_checksum_info();
	runtime_state_initialize();

	shell_startup_flag_set(_startup_flag_d3d9ex_enabled, H2Config_d3d9ex);
	shell_startup_flag_set(_startup_flag_disable_voice_chat, 1);

	int arg_count;
	wchar_t** cmd_line_args = LOG_CHECK(CommandLineToArgvW(GetCommandLineW(), &arg_count));
	if (cmd_line_args && arg_count > 1) {
		for (int i = 1; i < arg_count; i++) {
			wchar_t* cmd_line_arg = cmd_line_args[i];

			if (_wcsicmp(cmd_line_arg, L"-windowed") == 0) {
				shell_startup_flag_set(_startup_flag_windowed, 1);
			}
			else if (_wcsicmp(cmd_line_arg, L"-nosound") == 0) {
				shell_startup_flag_set(_startup_flag_nosound, 1);
				WriteValue(Memory::GetAddress(0x479EDC), 1);
			}
			else if (_wcsicmp(cmd_line_arg, L"-novsync") == 0) {
				shell_startup_flag_set(_startup_flag_novsync, 1);
			}
			else if (_wcsicmp(cmd_line_arg, L"-nointro") == 0) {
				shell_startup_flag_set(_startup_flag_nointro, 1);
			}
			else if (_wcsnicmp(cmd_line_arg, L"-monitor:", 9) == 0) {
				int monitor_id = _wtol(&cmd_line_arg[9]);
				shell_startup_flag_set(_startup_flag_monitor_count, MIN(MAX(0, monitor_id), max_monitor_count));
			}
			else if (_wcsicmp(cmd_line_arg, L"-highquality") == 0) {
				shell_startup_flag_set(_startup_flag_high_quality, 1);
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
				shell_startup_flag_set(_startup_flag_disable_voice_chat, 0);
				H2Config_voice_chat = true;
			}
#endif
#ifdef _DEBUG
			else if (_wcsnicmp(cmd_line_arg, L"-dev_flag:", 10) == 0) {
				int flag_id = _wtol(&cmd_line_arg[10]);
				shell_startup_flag_set((e_startup_flags)MIN(MAX(0, flag_id), e_startup_flags::k_startup_flags_count - 1), 1);
			}
#endif
		}
	}
	LocalFree(cmd_line_args);

	if (shell_startup_flag_is_set(_startup_flag_unk26))
		timing_initialize(1000 * shell_startup_flag_get(_startup_flag_unk26));
	real_math_initialize();
	async_initialize();
	game_preferences_initialize();

	c_network_observer::reset_network_observer_bandwidth_preferences();

	font_initialize();

	if (!LOG_CHECK(tag_files_open()))
		return false;

	game_state_shell_initialize();

	// modifies esi need to check what the caller sets that too
	// Update 1/13/2022:
	// initializes some interface that's used to download the network config from the bungie's website
	// but just a stub in release
	auto p_network_configuration_initialize = Memory::GetAddress<void(__cdecl*)()>(0x001a9de6);
	p_network_configuration_initialize();

	if (!LOG_CHECK(rasterizer_initialize()))
		return false;

	fn_c000285fd();

	input_initialize();
	sound_initialize();

	FakePBuffer** var_c00479e78 = Memory::GetAddress<FakePBuffer**>(0x479E78);
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

#pragma region Game Init functions

void real_math_initialize()
{
	typedef int(__cdecl* real_math_initialize_t)();
	auto p_real_math_initialize = Memory::GetAddress<real_math_initialize_t>(0x000340d7);
	p_real_math_initialize();
}

void async_initialize()
{
	typedef int(__cdecl* async_initialize_t)();
	auto p_async_initialize = Memory::GetAddress<async_initialize_t>(0x00032ce5);
	p_async_initialize();
}

bool gfwl_gamestore_initialize()
{
	typedef char(__cdecl* init_gfwl_gamestore_t)();
	auto p_init_gfwl_gamestore = Memory::GetAddress<init_gfwl_gamestore_t>(0x00202f3e);
	return p_init_gfwl_gamestore();
}

// not sure if this is all it does
HANDLE init_data_checksum_info()
{
	typedef HANDLE(__cdecl* init_data_checksum_info_t)();
	auto p_init_data_checksum_info = Memory::GetAddress<init_data_checksum_info_t>(0x000388d3);
	return p_init_data_checksum_info();
}

// returns memory
void* runtime_state_initialize()
{
	typedef void* (__cdecl* runtime_state_init_t)();
	auto p_runtime_state_init = Memory::GetAddress<runtime_state_init_t>(0x00037ed5);
	return p_runtime_state_init();
}

void game_preferences_initialize()
{
	typedef void(__cdecl* game_preferences_initialize_t)();
	auto p_game_preferences_initialize = Memory::GetAddress<game_preferences_initialize_t>(0x325FD);
	p_game_preferences_initialize();
}

void font_initialize()
{
	typedef void(__cdecl* font_initialize_t)();
	auto p_font_initialize = Memory::GetAddress<font_initialize_t>(0x00031dff);
	p_font_initialize();
}

bool tag_files_open()
{
	typedef bool(__cdecl* tag_files_open_t)();
	auto p_tag_files_open = Memory::GetAddress<tag_files_open_t>(0x30D58);
	return p_tag_files_open();
}

void timing_initialize(int a1)
{
	typedef void(__cdecl* init_timing_t)(int a1);
	auto p_init_timing = Memory::GetAddress<init_timing_t>(0x37E39);
	p_init_timing(a1);
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