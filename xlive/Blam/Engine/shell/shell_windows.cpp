#include "stdafx.h"
#include "shell_windows.h"

#include "shell.h"
#include "shell_windows_internals.h"
#include "shell_windows_ipc.h"
#include "shell_windows_pcc.h"

#include "input/input_windows.h"
#include "main/main.h"
#include "rasterizer/dx9/rasterizer_dx9_main.h"
#include "interface/user_interface_networking.h"

#include "H2MOD/Modules/CustomMenu/CustomLanguage.h"
#include "H2MOD/Modules/OnScreenDebug/OnscreenDebug.h"
#include "H2MOD/Modules/Shell/Config.h"
#include "H2MOD/Modules/Shell/H2MODShell.h"
#include "H2MOD/Modules/Shell/Startup/Startup.h"

#ifndef IMGUI_DISABLE

#include "imgui.h"

/* prototypes */

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#endif

/* constants */

enum
{
	k_max_monitor_count = 9
};

/* globals */

static LARGE_INTEGER g_startup_counter;
static DWORD(WINAPI* p_timeGetTime)() = timeGetTime;

static bool(__cdecl* p_shell_set_game_cursor_state)(bool enabled);

static bool g_custom_mouse_cursor_enabled = false;

static uint32 g_instance_number = 0;

/* prototypes */

static DWORD WINAPI timeGetTime_hook();

// WinMain replacement
static int WINAPI H2WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd);

// Destroys global windows used by the game
static void destroy_windows(void);

static void __cdecl show_fatal_error(int32 error_id);

static LRESULT WINAPI H2WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

static LARGE_INTEGER shell_get_startup_counter(void);

static unsigned long long shell_time_from_counter(LARGE_INTEGER counter, LARGE_INTEGER freq, unsigned long long denominator);

static void shell_system_set_timer_resolution_max(bool enable);

static unsigned long long shell_time_diff(LARGE_INTEGER t2, unsigned long long denominator);

static void shell_windows_throttle_framerate_initialize();

static void shell_windows_yield_thread(LARGE_INTEGER last_time, real32 desired_framerate);

// Adjust name of window to display the instance number when we have more than 1 window open
static void shell_windows_adjust_name(void);

static void shell_windows_calculate_instance_num(void);

static void shell_windows_url_decode_command_line_buffer(wchar_t* argument_buffer);

static void shell_windows_tokenize_command_line_buffer(const wchar_t* argument_buffer, wchar_t** args, int32 max_arg_count, int32* arg_count);

static void shell_windows_initialize_arguments(void);

static bool __cdecl shell_windows_is_remote_desktop(void);

static bool shell_set_game_cursor_state_hook(bool enabled);

static void DuplicateDataBlob(DATA_BLOB* pDataIn, DATA_BLOB* pDataOut);

static BOOL WINAPI CryptProtectDataHook(
	_In_       DATA_BLOB* pDataIn,
	_In_opt_   LPCWSTR                   szDataDescr,
	_In_opt_   DATA_BLOB* pOptionalEntropy,
	_Reserved_ PVOID                     pvReserved,
	_In_opt_   CRYPTPROTECT_PROMPTSTRUCT* pPromptStruct,
	_In_       DWORD                     dwFlags,
	_Out_      DATA_BLOB* pDataOut
);

static BOOL WINAPI CryptUnprotectDataHook(
	_In_       DATA_BLOB* pDataIn,
	_Out_opt_  LPWSTR* ppszDataDescr,
	_In_opt_   DATA_BLOB* pOptionalEntropy,
	_Reserved_ PVOID                     pvReserved,
	_In_opt_   CRYPTPROTECT_PROMPTSTRUCT* pPromptStruct,
	_In_       DWORD                     dwFlags,
	_Out_      DATA_BLOB* pDataOut
);

static void shell_windows_setup_cartographer_protocol();

/* public code */

s_window_globals* window_globals_get(void)
{
	return Memory::GetAddress<s_window_globals*>(0x46D9B8);
}

uint32 shell_get_instance_num(void)
{
	ASSERT(g_instance_number > 0);
	return g_instance_number;
}

bool shell_platform_initialize(void)
{
	const bool is_dedi = shell_is_dedicated_server();

	shell_windows_calculate_instance_num();

	if (!is_dedi)
	{
		shell_windows_setup_cartographer_protocol();

		shell_windows_ipc_initialize();
	}

	shell_windows_initialize_arguments();

	shell_windows_throttle_framerate_initialize();

	InitOnScreenDebugText();

	// TODO: initialize the ini config in the same place between client and dedi 
	if (!is_dedi)
	{
		InitH2Config();
		PostH2Config();
	}

	h2log_initialize();

	// If the intro is already disabled via command line flag don't try and set it via the config option
	if (shell_command_line_flag_get(_shell_command_line_flag_nointro) == false)
	{
		shell_command_line_flag_set(_shell_command_line_flag_nointro, H2Config_skip_intro);
	}
	shell_command_line_flag_set(_shell_command_line_flag_disable_voice_chat, true);			// ### TODO FIXME: voice-chat is disabled for now

	if (!is_dedi)
	{
		shell_windows_adjust_name();
	
		// TODO: remove this garbage
		custom_language_initialize();
	}
	else
	{
		H2DedicatedServerStartup();
	}

	return true;
}

// mess around with xlive (not calling XLiveInitialize etc)
bool* should_initilize_xlive_get(void)
{
	return Memory::GetAddress<bool*>(0x4FAD98, 0x520AC8);
}

bool* xlive_initilized_get(void)
{
	return Memory::GetAddress<bool*>(0x4FAD99, 0x520AC9);
}

int32* fatal_error_id_get(void)
{
	return Memory::GetAddress<int32*>(0x46DAD4);
}

void shell_windows_apply_patches(void)
{
	DETOUR_ATTACH(p_timeGetTime, timeGetTime, timeGetTime_hook);
	if (!shell_is_dedicated_server())
	{
		WriteJmpTo(Memory::GetAddress(0x7E43), H2WinMain);
		DETOUR_ATTACH(p_shell_set_game_cursor_state, Memory::GetAddress<decltype(p_shell_set_game_cursor_state)>(0x2E0FB), shell_set_game_cursor_state_hook);
	}

	// disables profiles/game saves encryption
	PatchWinAPICall(Memory::GetAddress(0x9B08A, 0x85F5E), CryptProtectDataHook);
	PatchWinAPICall(Memory::GetAddress(0x9AF9E, 0x352538), CryptUnprotectDataHook);	
	return;
}

void shell_windows_initialize(void)
{
	QueryPerformanceCounter(&g_startup_counter);
	return;
}

uint32 __cdecl system_milliseconds(void)
{
	return INVOKE(0x37E51, 0x2B4CE, system_milliseconds);
}

LARGE_INTEGER shell_time_counter_freq()
{
	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);
	return freq;
}

LARGE_INTEGER shell_time_counter_now(LARGE_INTEGER* freq)
{
	LARGE_INTEGER counter;
	if (freq) 
	{
		*freq = shell_time_counter_freq();
	}
	QueryPerformanceCounter(&counter);
	counter.QuadPart -= shell_get_startup_counter().QuadPart;
	return counter;
}

LARGE_INTEGER shell_time_counter_diff(LARGE_INTEGER c1, LARGE_INTEGER c2)
{
	c1.QuadPart -= c2.QuadPart;
	return c1;
}

unsigned long long shell_time_now(unsigned long long denominator)
{
	LARGE_INTEGER counter, freq;
	counter = shell_time_counter_now(&freq);
	return shell_time_from_counter(counter, freq, denominator) + (k_process_system_time_startup_offset_sec * denominator);
}

unsigned long long shell_time_now_sec()
{
	return shell_time_now(k_shell_time_sec_denominator);
}

unsigned long long shell_time_now_msec()
{
	return shell_time_now(k_shell_time_msec_denominator);
}

void shell_windows_throttle_framerate_initialize()
{
}

void shell_windows_throttle_framerate(LARGE_INTEGER last_time, int desired_framerate)
{
	if (desired_framerate > 0)
	{
		desired_framerate = MAX(desired_framerate, 15);
		shell_windows_yield_thread(last_time, (real32)desired_framerate);
	}

	return;
}

bool __cdecl gfwl_gamestore_initialize(void)
{
	return INVOKE(0x202F3E, 0x0, gfwl_gamestore_initialize);
}

uint32 shell_windows_get_monitor_index(void)
{
	uint32 result = 0;
	if (shell_command_line_flag_is_set(_shell_command_line_flag_monitor_count))
	{
		result = shell_command_line_flag_get(_shell_command_line_flag_monitor_count);

		// Set monitor index to 0 if the monitor index set by the shell flag isn't a valid monitor
		if (result >= rasterizer_dx9_main_globals_get()->global_d3d_interface->GetAdapterCount())
		{
			result = 0;
		}
	}
	return result;
}

/* private code */

DWORD WINAPI timeGetTime_hook()
{
	unsigned long long current_time_msec = shell_time_now_msec();
	return (DWORD)current_time_msec;
}
static_assert(std::is_same_v<decltype(timeGetTime), decltype(timeGetTime_hook)>, "Invalid timeGetTime_hook signature");

static int WINAPI H2WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	s_window_globals* window_globals = window_globals_get();

	// set args
	window_globals->lpCmdLine = lpCmdLine;
	window_globals->hInstance = hInstance;
	window_globals->show_cmd = nShowCmd;

	// window setup
	ustrncpy(window_globals->class_name, L"halo", NUMBEROF(window_globals->class_name));
	ustrncpy(window_globals->window_name, L"Halo 2 - Project Cartographer", NUMBEROF(window_globals->window_name));
	window_globals->wnd_proc = H2WndProc;

	bool pcc_result = shell_windows_pcc_initialize();
	if (!pcc_result)
	{
		error(_error_delayed, "Failed to get PCC info / insufficient system resources");
		
		// ### TODO change this?
		std::thread([]() {
			_Shell::OpenMessageBox(NULL, MB_ICONEXCLAMATION, "PCC Error", "Failed to get compatibility info.");
		}).detach();

		show_fatal_error(108);
	}

	window_globals->cursor = LoadCursor(NULL, MAKEINTRESOURCE(0x7F00));

	*should_initilize_xlive_get() = true;

	// intialize some basic game subsystems
	if (shell_initialize())
	{
		main_loop();		// actually run game
		shell_windows_ipc_shutdown(); // end and shutdown the IPC thread
		shell_dispose();	// cleanup
	}
	else if (int32 g_fatal_error_id = *fatal_error_id_get(); g_fatal_error_id)
	{
		error(_error_delayed, "shell_initialize failed!");
		show_fatal_error(g_fatal_error_id);
	}

	int result;
	int32 g_fatal_error_id = *fatal_error_id_get();
	if (g_fatal_error_id)	// check if the game exited cleanly
	{
		destroy_windows();
		show_fatal_error(g_fatal_error_id);
		result = 1;
	}
	else
	{
		result = 0;
	}

	return result;
}

static void destroy_windows(void)
{
	const s_window_globals* window_globals = window_globals_get();
	if (window_globals->hWnd)
	{
		DestroyWindow(window_globals->hWnd);
	}

	if (window_globals->hWndPresentTarget)
	{
		DestroyWindow(window_globals->hWndPresentTarget);
	}
	return;
}

static void __cdecl show_fatal_error(int32 error_id)
{
	error(_error_delayed, "error_id: %d", error_id);
	INVOKE(0x4A2E, 0x0, show_fatal_error, error_id);
	return;
}

static void __cdecl shell_disable_cursor()
{
	if (g_custom_mouse_cursor_enabled)
	{
		INVOKE(0x497B, 0x0, shell_disable_cursor);
		g_custom_mouse_cursor_enabled = false;
	}
}

static void __cdecl shell_enable_cursor()
{
	if (!g_custom_mouse_cursor_enabled)
	{
		INVOKE(0x2EDC4, 0x0, shell_enable_cursor);
		g_custom_mouse_cursor_enabled = true;
	}
}

static bool shell_set_game_cursor_state_hook(bool enabled)
{
	g_custom_mouse_cursor_enabled = enabled;
	return p_shell_set_game_cursor_state(enabled);
}

static LRESULT WINAPI H2WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	bool* window_in_focus = Memory::GetAddress<bool*>(0x46DAD9);
	bool* shell_window_proc_game_message_during_map_load = Memory::GetAddress<bool*>(0x46DAD8);
	const WNDPROC g_WndProc = Memory::GetAddress<WNDPROC>(0x790E);

#ifndef IMGUI_DISABLE
	if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
	{
		return 1;
	}
#endif

	LRESULT result = 1;

	bool exec_base_wndproc = true;

	switch (uMsg)
	{
	case WM_KEYUP:
	case WM_SYSKEYUP:
		input_windows_release_key((WCHAR)wParam);
		break;
	case WM_SETFOCUS:
		g_custom_mouse_cursor_enabled = false;
		input_windows_clear_keyboard_input_state();
		break;
	case WM_KILLFOCUS:
		g_custom_mouse_cursor_enabled = true;
		input_windows_clear_keyboard_input_state();
		break;
	case WM_SETCURSOR:
		// check if the cursor is actually in client area
		if (LOWORD(lParam) == HTCLIENT)
		{
			if (GetCursor() == NULL)
			{
				g_custom_mouse_cursor_enabled = false;
			}
			exec_base_wndproc = false;
			if (*window_in_focus && *shell_window_proc_game_message_during_map_load)
			{
				shell_disable_cursor();
			}
			else
			{
				shell_enable_cursor();
			}
		}
		else
		{
			exec_base_wndproc = false;
			g_custom_mouse_cursor_enabled = false;
		}

		break;
	}

	if (exec_base_wndproc)
	{
		result = g_WndProc(hWnd, uMsg, wParam, lParam);
	}

	return result;
}

static LARGE_INTEGER shell_get_startup_counter(void)
{
	return g_startup_counter;
}

static unsigned long long shell_time_from_counter(LARGE_INTEGER counter, LARGE_INTEGER freq, unsigned long long denominator)
{
	unsigned long long _Whole, _Part;

	_Whole = (counter.QuadPart / freq.QuadPart) * denominator;
	_Part = (counter.QuadPart % freq.QuadPart) * denominator / freq.QuadPart;

	return _Whole + _Part;
}

static void shell_system_set_timer_resolution_max(bool enable)
{
	ULONG ulMinimumResolution, ulMaximumResolution, ulCurrentResolution;
	NtQueryTimerResolutionHelper(&ulMinimumResolution, &ulMaximumResolution, &ulCurrentResolution);
	NtSetTimerResolutionHelper(ulMaximumResolution, enable, &ulCurrentResolution);
}

static unsigned long long shell_time_diff(LARGE_INTEGER t2, unsigned long long denominator)
{
	LARGE_INTEGER counter, freq;
	counter = shell_time_counter_now(&freq);
	counter = shell_time_counter_diff(counter, t2);
	return shell_time_from_counter(counter, freq, denominator);
}

static void shell_windows_yield_thread(LARGE_INTEGER last_counter, real32 desired_framerate)
{
	const int k_thread_sleep_api_time_slice_percentage = 70; // 70% of the time
	const int k_thread_sleep_api_min_time_to_sleep_usec = 3000; // 3ms

	unsigned long long min_frametime_usec = (unsigned long long)((real32)k_shell_time_usec_denominator / desired_framerate);
	unsigned long long dt_usec = shell_time_diff(last_counter, k_shell_time_usec_denominator);

	if (dt_usec < min_frametime_usec)
	{
		unsigned long long sleep_time_usec = min_frametime_usec - dt_usec;

		// sleep threadWaitTimePercentage out of the target render time using thread sleep or timer wait
		unsigned long long system_yield_time_usec = (k_thread_sleep_api_time_slice_percentage * sleep_time_usec) / 100;

		// sleep just milliseconds
		system_yield_time_usec = system_yield_time_usec - (system_yield_time_usec % 1000);

		// skip system thread sleep if time to sleep is lower than 3ms
		// because the system timer isn't precise enough to guarantee the time slept is close to the one desired
		if (system_yield_time_usec > k_thread_sleep_api_min_time_to_sleep_usec)
		{
			ULONG ulMinimumResolution, ulMaximumResolution, ulCurrentResolution;
			NtQueryTimerResolutionHelper(&ulMinimumResolution, &ulMaximumResolution, &ulCurrentResolution);

			// shell_system_set_timer_resolution_max(true);

			if (10ll * system_yield_time_usec > ulMaximumResolution)
			{
				LARGE_INTEGER liDueTime = {};
				liDueTime.QuadPart = -10ll * system_yield_time_usec;
				
				// Wait for the timer.
				NtWaitForSingleObjectHelper(GetCurrentThread(), FALSE, &liDueTime);
			}
			
			/*int sleepTimeMs = system_yield_time_usec / 1000ll;
			if (sleepTimeMs >= 0)
				Sleep(sleepTimeMs);*/
		}

		// spin-lock the remaining time slice
		while (true)
		{
			if (shell_time_diff(last_counter, k_shell_time_usec_denominator) >= min_frametime_usec)
			{
				break;
			}
			else
			{
				_mm_pause();
			}
		}
	}
}

static void shell_windows_adjust_name(void)
{
	if (shell_get_instance_num() > 1)
	{
		s_window_globals* window_globals = window_globals_get();
		usnprintf(window_globals->window_name, NUMBEROF(window_globals->window_name), L"%ws (P%d)", window_globals->window_name, shell_get_instance_num());
	}
	return;
}

static void shell_windows_calculate_instance_num(void)
{
	DWORD lastErr;
	do
	{
		++g_instance_number;
		wchar_t mutexName[64];
		swprintf(mutexName, ARRAYSIZE(mutexName), (shell_is_dedicated_server() ? L"Halo2Server%d" : L"Halo2Player%d"), g_instance_number);
		HANDLE mutex = CreateMutexW(0, TRUE, mutexName);
		lastErr = GetLastError();
		if (lastErr == ERROR_ALREADY_EXISTS && mutex != NULL)
		{
			CloseHandle(mutex);
		}
	}
	while (lastErr == ERROR_ALREADY_EXISTS);
	return;
}

static void shell_windows_url_decode_command_line_buffer(wchar_t* argument_buffer)
{
	if (!argument_buffer) return;

	// read the argument buffer and replace instances of %20 with a space
	// this is for the custom protocol launching of the application which
	// is URI encoded, doing a full URI decode shouldn't be necessary as
	// the only thing that should ever need to be encoded is a space (%20)

	int32 read_index = 0;
	int32 write_index = 0;
	while (argument_buffer[read_index])
	{
		if (argument_buffer[read_index] == L'%' && argument_buffer[read_index + 1] == L'2' && argument_buffer[read_index + 2] == L'0')
		{
			argument_buffer[write_index++] = L' ';
			read_index += 3;
		}
		else
		{
			argument_buffer[write_index++] = argument_buffer[read_index++];
		}
	}
	argument_buffer[write_index] = L'\0';
}

static void shell_windows_tokenize_command_line_buffer(const wchar_t* argument_buffer, wchar_t** args, int32 max_arg_count, int32* arg_count)
{
	void* p_shell_windows_tokenize_command_line_buffer = Memory::GetAddress<void*>(0x1014, 0x1000);
	__asm
	{
		push arg_count
		push max_arg_count
		push args
		mov eax, argument_buffer
		call p_shell_windows_tokenize_command_line_buffer
		add esp, 12
	}
	return;
}

static void shell_windows_initialize_arguments(void)
{
	for (int32 i = 0; i < k_number_of_shell_command_line_flags; i++)
	{
		shell_command_line_flag_set((e_shell_command_line_flags)i, 0);
	}

	const LPWSTR command_line = GetCommandLineW();
	if (command_line)
	{
		// Copy the buffer
		wchar_t argument_buffer[INT16_MAX + 1];

		ustrncpy(argument_buffer, command_line, NUMBEROF(argument_buffer));

		// Initialize our list of args
		wchar_t* args[1024];
		csmemset(args, 0, sizeof(args));

		// URL decode
		shell_windows_url_decode_command_line_buffer(argument_buffer);

		// Tokenize the argument buffer into our list of args
		int32 arg_count;
		shell_windows_tokenize_command_line_buffer(argument_buffer, args, NUMBEROF(args), &arg_count);

		for (int32 i = 0; i < arg_count; i++)
		{
			const wchar_t* current_argument = args[i];

			// if the game is launched from the custom protocol the first argument will start with carto:
			// adjust the pointer by 6 to move past the protocol
			if (_wcsnicmp(current_argument, L"carto:", 6) == 0)
			{
				current_argument = &args[i][6];
			}

			if (_wcsicmp(current_argument, L"-windowed") == 0)
			{
				shell_command_line_flag_set(_shell_command_line_flag_windowed, 1);
			}
			else if (_wcsicmp(current_argument, L"-nosound") == 0)
			{
				shell_command_line_flag_set(_shell_command_line_flag_nosound, 1);
				WriteValue(Memory::GetAddress(0x479EDC), 1);
			}
			else if (_wcsicmp(current_argument, L"-novsync") == 0)
			{
				shell_command_line_flag_set(_shell_command_line_flag_novsync, 1);
			}
			else if (_wcsicmp(current_argument, L"-nointro") == 0)
			{
				shell_command_line_flag_set(_shell_command_line_flag_nointro, 1);
			}
			else if (_wcsnicmp(current_argument, L"-monitor:", 9) == 0)
			{
				int monitor_id = utol(&current_argument[9]);
				shell_command_line_flag_set(_shell_command_line_flag_monitor_count, PIN(monitor_id, 0, k_max_monitor_count));
			}
			else if (_wcsicmp(current_argument, L"-highquality") == 0)
			{
				shell_command_line_flag_set(_shell_command_line_flag_high_quality, 1);
			}
			else if (_wcsicmp(current_argument, L"-disabledepthbias") == 0)
			{
				// Check github issue #118
				/* g_depth_bias always NULL rather than taking any value from
				shader tag before calling g_D3DDevice->SetRenderStatus(D3DRS_DEPTHBIAS, g_depth_bias); */
				NopFill(Memory::GetAddress(0x269FD5), 8);
			}
#ifdef _DEBUG
			else if (_wcsnicmp(current_argument, L"-dev_flag:", 10) == 0)
			{
				int flag_id = utol(&current_argument[10]);
				shell_command_line_flag_set((e_shell_command_line_flags)PIN(0, flag_id, k_number_of_shell_command_line_flags - 1), 1);
			}
#endif
			else if (_wcsnicmp(current_argument, L"-join:", 6) == 0)
			{
				const wchar_t* session_code = &current_argument[6];
				XSESSION_INFO join_session;
				uint8* session_bytes = (uint8*)&join_session;

				for (uint32_t j = 0; j < sizeof(XSESSION_INFO); j++)
				{
					uint32 v = 0;
					swscanf_s(session_code + (j * 2), L"%02X", &v);
					session_bytes[j] = (uint8)(v & 0xFF);
				}

				if (shell_windows_ipc_notify_session_info(&join_session))
				{
					// another instance of the game is open and the session info was forwarded to it
					// we can just close the game gracefully
					// not using main_exit() because using that will let the game still create the game window
					// and then close
					exit(0);
				}

				g_game_auto_join.do_auto_join = true;
				g_game_auto_join.auto_join_session = join_session;
			}
		}
	}
	return;
}

static bool __cdecl shell_windows_is_remote_desktop(void)
{
	return INVOKE(0x39EA2, 0x0, shell_windows_is_remote_desktop);
}

static void DuplicateDataBlob(DATA_BLOB* pDataIn, DATA_BLOB* pDataOut)
{
	pDataOut->cbData = pDataIn->cbData;
	pDataOut->pbData = static_cast<BYTE*>(LocalAlloc(LMEM_FIXED, pDataIn->cbData));
	CopyMemory(pDataOut->pbData, pDataIn->pbData, pDataIn->cbData);
	return;
}

static BOOL WINAPI CryptProtectDataHook(
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

static BOOL WINAPI CryptUnprotectDataHook(
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

static void shell_windows_setup_cartographer_protocol()
{
	// create and reset the registry class every time the game is run
	// in case someone moves their games install or has multiple installs (different versions for devs mostly)
	// the exe path is resolved from any potential symbolic junctions (dev shit)
	// or if someone is crazy enough to play the game from a UNC network folder
	//
	// the custom windows launch protocol is created in the CurrentUser registry hive
	// so that elevated permissions aren't required to create it for the system

	c_static_wchar_string<MAX_PATH> exe_path;

	GetModuleFileNameW(NULL, exe_path.get_buffer(), MAX_PATH);

	HANDLE file_handle = CreateFileW(exe_path.get_string(), 0,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	ASSERT(file_handle != INVALID_HANDLE_VALUE);

	uint32 len = GetFinalPathNameByHandleW(file_handle, nullptr, 0, 0);

	exe_path.clear();

	GetFinalPathNameByHandleW(file_handle, exe_path.get_buffer(), len, 0);

	CloseHandle(file_handle);

	c_static_wchar_string<MAX_PATH> final_path;

	if (wcsncmp(exe_path.get_string(), L"\\\\?\\UNC\\", 8) == 0)
	{
		final_path.set(L"\\\\");
		final_path.append(&exe_path.get_string()[8]);
	}
	else if (wcsncmp(exe_path.get_string(), L"\\\\?\\", 4) == 0)
	{
		final_path.set(&exe_path.get_string()[4]);
	}

	HKEY hKey = NULL;
	wchar_t buffer[MAX_PATH + 32];

	RegOpenKeyExW(
		HKEY_CURRENT_USER,
		L"SOFTWARE\\Classes\\carto",
		0,
		KEY_READ,
		&hKey
	);

	RegCreateKeyExW(
		HKEY_CURRENT_USER,
		L"SOFTWARE\\Classes\\carto",
		0, NULL,
		REG_OPTION_NON_VOLATILE,
		KEY_WRITE,
		NULL,
		&hKey,
		NULL
	);

	const wchar_t* protocolDesc = L"URL:Cartographer Protocol";
	RegSetValueExW(hKey, NULL, 0, REG_SZ, (const BYTE*)protocolDesc, (DWORD)(wcslen(protocolDesc) + 1) * sizeof(wchar_t));
	RegSetValueExW(hKey, L"URL Protocol", 0, REG_SZ, (const BYTE*)L"", sizeof(wchar_t));
	RegCloseKey(hKey);

	RegCreateKeyExW(
		HKEY_CURRENT_USER,
		L"SOFTWARE\\Classes\\carto\\DefaultIcon",
		0, NULL,
		REG_OPTION_NON_VOLATILE,
		KEY_WRITE,
		NULL,
		&hKey,
		NULL
	);

	_snwprintf_s(buffer, _countof(buffer), L"\"%s,1\"", final_path.get_buffer());
	RegSetValueExW(hKey, NULL, 0, REG_SZ, (const BYTE*)buffer, (DWORD)(wcslen(buffer) + 1) * sizeof(wchar_t));
	RegCloseKey(hKey);

	RegCreateKeyExW(
		HKEY_CURRENT_USER,
		L"SOFTWARE\\Classes\\carto\\shell\\open\\command",
		0, NULL,
		REG_OPTION_NON_VOLATILE,
		KEY_WRITE,
		NULL,
		&hKey,
		NULL
	);

	_snwprintf_s(buffer, _countof(buffer), L"\"%s\" %%1", final_path.get_buffer());
	RegSetValueExW(hKey, NULL, 0, REG_SZ, (const BYTE*)buffer, (DWORD)(wcslen(buffer) + 1) * sizeof(wchar_t));
	RegCloseKey(hKey);
}
