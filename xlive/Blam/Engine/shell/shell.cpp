#include "stdafx.h"
#include "shell.h"

#include "cache/cache_file_verification.h"
#include "cseries/async.h"
#include "cseries/runtime_state.h"
#include "input/input_windows.h"
#include "main/game_preferences.h"
#include "main/licensing.h"
#include "math/real_math.h"
#include "networking/Transport/network_observer.h"
#include "rasterizer/dx9/rasterizer_dx9_main.h"
#include "saved_games/game_state.h"
#include "shell/shell_windows.h"
#include "sound/sound_manager.h"
#include "tag_files/tag_files.h"
#include "text/font_group.h"

#include "H2MOD/Modules/Shell/Config.h"

#define k_max_monitor_count 9

void shell_apply_patches(void)
{
	if (!Memory::IsDedicatedServer())
	{
		WriteJmpTo(Memory::GetAddress(0x4544), shell_command_line_flag_get);
	}

	return;
}

e_shell_tool_type shell_tool_type(void)
{
	return _shell_tool_type_game;
}

static int32* shell_startup_flags_get()
{
	return Memory::GetAddress<int32*>(0x46D820, 0x40E708);
}

bool shell_command_line_flag_is_set(e_shell_command_line_flags flag)
{
	ASSERT(VALID_INDEX(flag, k_number_of_shell_command_line_flags));
	return shell_startup_flags_get()[flag] != 0;
}

int32 __cdecl shell_command_line_flag_get(e_shell_command_line_flags flag)
{
	ASSERT(VALID_INDEX(flag, k_number_of_shell_command_line_flags));
	return shell_startup_flags_get()[flag];
}

void shell_command_line_flag_set(e_shell_command_line_flags flag, int32 state)
{
	shell_startup_flags_get()[flag] = state;
	return;
}

bool shell_initialize(void)
{
	for (int32 i = 0; i < k_number_of_shell_command_line_flags; i++)
	{
		shell_command_line_flag_set((e_shell_command_line_flags)i, 0);
	}

	shell_command_line_flag_set(_shell_command_line_flag_nointro, H2Config_skip_intro);

	// ### TODO FIXME: voice-chat is disabled for now
	shell_command_line_flag_set(_shell_command_line_flag_disable_voice_chat, true);

	// Don't check the result of this because we're not using GFWL
	gfwl_gamestore_initialize();

	cache_file_verification_initialize();
	runtime_state_initialize();

	int arg_count;
	wchar_t** cmd_line_args = LOG_CHECK(CommandLineToArgvW(GetCommandLineW(), &arg_count));
	if (cmd_line_args && arg_count > 1) {
		for (int i = 1; i < arg_count; i++) {
			wchar_t* cmd_line_arg = cmd_line_args[i];

			if (_wcsicmp(cmd_line_arg, L"-windowed") == 0) {
				shell_command_line_flag_set(_shell_command_line_flag_windowed, 1);
			}
			else if (_wcsicmp(cmd_line_arg, L"-nosound") == 0) {
				shell_command_line_flag_set(_shell_command_line_flag_nosound, 1);
				WriteValue(Memory::GetAddress(0x479EDC), 1);
			}
			else if (_wcsicmp(cmd_line_arg, L"-novsync") == 0) {
				shell_command_line_flag_set(_shell_command_line_flag_novsync, 1);
			}
			else if (_wcsicmp(cmd_line_arg, L"-nointro") == 0) {
				shell_command_line_flag_set(_shell_command_line_flag_nointro, 1);
			}
			else if (_wcsnicmp(cmd_line_arg, L"-monitor:", 9) == 0) {
				int monitor_id = _wtol(&cmd_line_arg[9]);
				shell_command_line_flag_set(_shell_command_line_flag_monitor_count, PIN(monitor_id, 0, k_max_monitor_count));
			}
			else if (_wcsicmp(cmd_line_arg, L"-highquality") == 0) {
				shell_command_line_flag_set(_shell_command_line_flag_high_quality, 1);
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
				shell_shell_command_line_flag_set(_shell_command_line_flag_disable_voice_chat, false);
				H2Config_voice_chat = true;
			}
#endif
#ifdef _DEBUG
			else if (_wcsnicmp(cmd_line_arg, L"-dev_flag:", 10) == 0) {
				int flag_id = _wtol(&cmd_line_arg[10]);
				shell_command_line_flag_set((e_shell_command_line_flags)PIN(0, flag_id, k_number_of_shell_command_line_flags - 1), 1);
			}
#endif
		}
	}
	LocalFree(cmd_line_args);

	if (shell_command_line_flag_is_set(_shell_command_line_flag_unk26))
		timing_initialize(1000 * shell_command_line_flag_get(_shell_command_line_flag_unk26));

	real_math_initialize();
	async_initialize();
	global_preferences_initialize();

	c_network_observer::reset_network_observer_bandwidth_preferences();

	font_initialize();
	
	bool result = tag_files_open();
	if (result)
	{
		game_state_initialize();
		result = rasterizer_initialize();
		if (result)
		{
			sub_285FD();

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
		}
	}

	return result;
}

void shell_dispose(void)
{
	INVOKE(0x48A9, 0x4CFE, shell_dispose);
	return;
}

void __cdecl timing_initialize(int32 a1)
{
	INVOKE(0x37E39, 0x0, timing_initialize, a1);
	return;
}
