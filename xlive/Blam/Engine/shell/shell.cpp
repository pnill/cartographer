#include "stdafx.h"
#include "shell.h"

#include "cache/cache_file_verification.h"
#include "cseries/async.h"
#include "cseries/cseries.h"
#include "cseries/debug_memory.h"
#include "cseries/runtime_state.h"
#include "input/input_windows.h"
#include "main/game_preferences.h"
#include "main/licensing.h"
#include "math/real_math.h"
#include "networking/network_configuration.h"
#include "networking/session/network_observer.h"
#include "rasterizer/rasterizer_main.h"
#include "saved_games/game_state.h"
#ifdef _WINDOWS
#include "shell/shell_windows.h"
#endif
#include "sound/sound_manager.h"
#include "tag_files/tag_files.h"
#include "text/font_group.h"

/* structures*/

struct s_shell_build_version
{
	int32 version;
	const char* string;
};

/* constants */

const s_shell_build_version k_shell_build_versions[] =
{
	{11028, "11028.07.03.23.1927.main"},	// Preview build of Vista
	{11081, "11081.07.04.30.0934.main"},
	{11122, "11122.07.08.24.1808.main"},
};

/* globals */

bool g_shell_application_is_paused = false;

/* prototypes */


/* public code */

void shell_apply_patches(void)
{
	if (!shell_is_dedicated_server())
	{
		WriteJmpTo(Memory::GetAddress(0x4544), shell_command_line_flag_get);
	}
	else
	{
		PatchCall(Memory::GetAddress(0x0, 0xC9BE), shell_initialize);	// main_loop_initialize
	}

	return;
}

e_shell_application_type shell_application_type(void)
{
	return _shell_application_game;
}

// TODO: properly set this up
const char* shell_get_target(void)
{
	return "halo2_pc_cache_";
}

bool shell_application_is_paused(void)
{
	return g_shell_application_is_paused;
}

bool shell_is_dedicated_server(void)
{
	// This function originally returned a compiler argument for the server
	// We can't do that since we build a universal dll so just return the Memory global
	return Memory::g_memory_is_dedicated_server;
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

// TODO: properly set this up
const char* shell_get_version(void)
{
	// FORMAT: (target build_type platform build_number.branch)
	return "halo2_pc_cache_  release pc 11122.07.08.24.1808.main";
}

bool shell_initialize(void)
{
	bool result = false;

	if (!shell_is_dedicated_server())
	{
		// Don't check the result of this because we're not using GFWL
		gfwl_gamestore_initialize();
	}

	cache_file_verification_initialize();
	cseries_initialize();
	runtime_state_initialize();

	SYSTEM_DEBUG_MEMORY(cseries_initialize());

#ifdef _WINDOWS
	if (shell_platform_initialize())
#endif
	{
		SYSTEM_DEBUG_MEMORY(shell_platform_initialize());
		
#ifdef ERRORS_ENABLED
		errors_initialize();
		SYSTEM_DEBUG_MEMORY(errors_initialize());
#endif

		if (shell_command_line_flag_is_set(_shell_command_line_flag_unk26))
		{
			timing_initialize(1000 * shell_command_line_flag_get(_shell_command_line_flag_unk26));
		}

		real_math_initialize();
		SYSTEM_DEBUG_MEMORY(real_math_initialize());

		async_initialize();
		SYSTEM_DEBUG_MEMORY(async_initialize());

		global_preferences_initialize();
		SYSTEM_DEBUG_MEMORY(global_preferences_initialize());

		if (!shell_is_dedicated_server())
		{
			c_network_observer::reset_network_observer_bandwidth_preferences();
			font_initialize();
			SYSTEM_DEBUG_MEMORY(font_initialize());
		}
	
		result = tag_files_open();
		SYSTEM_DEBUG_MEMORY(tag_files_open());
		
		// Don't initialize the things below if we haven't loaded tags or are a dedi
		if (result && !shell_is_dedicated_server())
		{
			game_state_initialize();
			SYSTEM_DEBUG_MEMORY(game_state_initialize());
		
			network_configuration_initialize();
			SYSTEM_DEBUG_MEMORY(network_configuration_initialize());


			result = rasterizer_initialize();
			if (result)
			{
				SYSTEM_DEBUG_MEMORY(rasterizer_initialize());
				sub_285FD();

				FakePBuffer** var_c00479e78 = Memory::GetAddress<FakePBuffer**>(0x479E78);
				XLivePBufferAllocate(2, var_c00479e78);

				for (DWORD i = 0; i < 2; ++i)
				{
					XLivePBufferSetByte(*var_c00479e78, i, 0);
				}

				input_initialize();
				SYSTEM_DEBUG_MEMORY(input_initialize());
				sound_initialize();
				SYSTEM_DEBUG_MEMORY(sound_initialize());
			}
		}
	}

	return result;
}

void __cdecl shell_dispose(void)
{
	INVOKE(0x48A9, 0x4CFE, shell_dispose);
	return;
}

void __cdecl shell_idle(void)
{
	INVOKE(0x7902, 0xBA18, shell_idle);
	return;
}

void __cdecl timing_initialize(int32 a1)
{
	INVOKE(0x37E39, 0x2B4B6, timing_initialize, a1);
	return;
}

bool shell_build_string_is_compatible(const char* build_string)
{
	bool result = false;

	for (size_t i = 0; i < NUMBEROF(k_shell_build_versions); ++i)
	{
		if (!csstricmp(build_string, k_shell_build_versions[i].string))
		{
			result = true;
			break;
		}
	}

	return result;
}

/* private code */
