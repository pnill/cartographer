#include "stdafx.h"
#include "main.h"

#include "console.h"
#include "game_preferences.h"
#include "interpolator.h"
#include "loading.h"
#include "main_game.h"
#include "main_render.h"
#include "map_repository.h"
#include "main_time.h"

#include "achievements/achievement_live_interface.h"
#include "bink/wmv_playback.h"
#include "cache/cache_files_windows.h"
#include "camera/director.h"
#include "camera/observer.h"
#include "cartographer/discord/discord_interface.h"
#include "cseries/async.h"
#include "cseries/cseries_windows_debug.h"
#include "cseries/debug_memory.h"
#ifdef PROFILE_ENABLED
#include "cseries/profile.h"
#endif
#include "cseries/stack_walk_windows.h"
#ifdef DEBUG_MENU_ENABLED
#include "debug/menu/debug_menu_main.h"
#endif
#include "game/game.h"
#include "game/game_time.h"
#include "game/player_vibration.h"
#include "input/input_windows.h"
#include "interface/terminal.h"
#include "interface/user_interface.h"
#include "networking/logic/life_cycle_manager.h"
#include "networking/logic/network_search.h"
#include "networking/network_globals.h"
#ifdef PROFILE_ENABLED
#include "physics/collision_usage.h"
#endif
#include "rasterizer/rasterizer_globals.h"
#include "rasterizer/rasterizer_main.h"
#include "saved_games/game_state.h"
#include "saved_games/saved_game_files.h"
#include "shell/shell.h"
#include "shell/shell_windows.h"
#include "simulation/simulation.h"
#include "sound/sound_manager.h"
#include "text/font_group.h"

#include "H2MOD/Modules/EventHandler/EventHandler.hpp"
#include "H2MOD/Modules/MapManager/MapManager.h"

#include <WinSock2.h>	// Needed for gXnIpMgr
#include "XLive/xnet/IpManagement/XnIp.h"

/* structures */

struct s_main_globals
{
	bool startup_sequence;
	bool halted_with_errors;
	bool run_xdemos;
	bool exit_game;
	bool save_core;
	bool load_core;
	char core_file_name[64];
	bool reset_map;
	bool reset_map_random_seed;
	bool save_map;
	bool save_map_and_exit;
	bool skip_cinematic;
	bool prepare_to_switch_bsp;
	bool revert_keep_playing_cinematic_outros;
	bool revert_by_scripting;
	bool ui_saving_files;
	bool field_4F;
	int16 current_bsp_index;
	bool cinematic_sound_sync_in_progress;
	bool cinematic_sound_sync_completed;
};


/* prototypes */

static s_main_globals* main_globals_get(void);

static void main_loop_initialize(void);

static void main_save_map_private(void);

static void __cdecl main_game_reset_map_blue_screen_detection(void);

/* globals */

bool debug_no_drawing = false;
bool debug_console_pauses_game = true;
bool force_crash_uploads = false;

/* public code */

void main_apply_patches(void)
{
	PatchCall(Memory::GetAddress(0x397F6, 0x4130F), main_game_reset_map_blue_screen_detection);
	if (shell_is_dedicated_server())
	{
		PatchCall(Memory::GetAddress(0x0, 0xBA40), main_time_initialize);	// main_loop_initialize
		PatchCall(Memory::GetAddress(0x0, 0xC684), main_loop_body_server);	// main_loop		
	}
	else
	{
		// main_save_map_private patches, allow the function to update
		// even when TestCooperativeLevel returns an error 
		// nop cmp
		NopFill(Memory::GetAddress(0x3978B), 2);
		// then force jmp
		WriteValue(Memory::GetAddress(0x3978D), (uint8)0xEB);
	}
	return;
}

bool __cdecl cinematic_sound_sync_complete(void)
{
	return INVOKE(0x39480, 0x40FA1, cinematic_sound_sync_complete);
}

void main_halt_and_display_errors(void)
{
	s_main_globals* main_globals = main_globals_get();
	main_globals->halted_with_errors = true;
	main_game_change(NULL);
	return;
}

void main_loop(void)
{
	const s_main_globals* main_globals = main_globals_get();

	main_loop_initialize();
	while (!main_globals->exit_game)
	{
		main_loop_body();
	}

	main_game_unload_and_prepare_for_next_game();
	game_dispose();
#ifdef TERMINAL_ENABLED
	console_dispose();
#endif
	main_loading_dispose();
	return;
}

void main_reset_map(void)
{
	s_main_globals* main_globals = main_globals_get();
	main_globals->reset_map = true;
	return;
}

void main_quit(void)
{
	s_main_globals* main_globals = main_globals_get();
	main_globals->exit_game = true;
	return;
}

void __cdecl main_switch_structure_bsp(void)
{
	INVOKE(0x39639, 0x0, main_switch_structure_bsp);
	return;
}

void __cdecl main_loop_pregame(int32 a1, int32 a2)
{
	INVOKE(0x3948C, 0x0, main_loop_pregame, a1, a2);
	return;
}

void __cdecl main_reset_map_immediate()
{
	INVOKE(0x9763, 0x1FA4E, main_reset_map_immediate);
	return;
}

void main_load_core()
{
	s_main_globals* main_globals = main_globals_get();
	csstrncpy(main_globals->core_file_name, "core", ARRAYSIZE(main_globals->core_file_name));
	main_globals->load_core = true;
	return;
}

void main_load_core_name(const char* name)
{
	s_main_globals* main_globals = main_globals_get();
	csstrncpy(main_globals->core_file_name, name, ARRAYSIZE(main_globals->core_file_name));
	main_globals->load_core = true;
	return;
}

void main_save_core()
{
	s_main_globals* main_globals = main_globals_get();
	csstrncpy(main_globals->core_file_name, "core", ARRAYSIZE(main_globals->core_file_name));
	main_globals->save_core = true;
	return;
}

void main_save_core_name(const char* name)
{
	s_main_globals* main_globals = main_globals_get();
	csstrncpy(main_globals->core_file_name, name, ARRAYSIZE(main_globals->core_file_name));
	main_globals->save_core = true;
	return;
}

void main_loop_body(void)
{
	EventHandler::GameLoopEventExecute(EventExecutionType::execute_before);
	
	// TODO: figure out a better place to put this
	if (!shell_is_dedicated_server())
	{
		mapManager->MapDownloadUpdateTick();
		gXnIpMgr.GetLocalUserXn()->m_pckStats.PckDataSampleUpdate();	// update local user network stats
	}
	
	//INVOKE(0x399CC, 0xBFDE, main_loop_body);

	s_main_globals* main_globals = main_globals_get();
	
	if (game_in_editor())
	{
		if (!main_game_change_update())
		{
			return;
		}

		main_globals->skip_cinematic = (main_globals->skip_cinematic ? false : main_globals->skip_cinematic);
		main_globals->reset_map_random_seed = (main_globals->reset_map_random_seed ? false : main_globals->reset_map_random_seed);

		if (main_globals->reset_map && game_in_progress() && !game_time_get_paused())
		{
			main_game_reset_map();
			main_globals->reset_map = false;
		}

		if (main_globals->prepare_to_switch_bsp)
		{
			main_switch_structure_bsp();
		}

		main_globals->save_map = (main_globals->save_map ? false : main_globals->save_map);
		main_globals->save_map_and_exit = (main_globals->save_map_and_exit ? false : main_globals->save_map_and_exit);
		main_globals->save_core = (main_globals->save_core ? false : main_globals->save_core);
		main_globals->load_core = (main_globals->load_core ? false : main_globals->load_core);
		main_globals->ui_saving_files = (main_globals->ui_saving_files ? false : main_globals->ui_saving_files);
	}
	else
	{
		main_globals->run_xdemos = (main_globals->run_xdemos ? false : main_globals->run_xdemos);
		main_game_launch_default();

		if (!main_game_change_update())
		{
			return;
		}

		main_game_load_post_game_launch();

		if (main_globals->save_core)
		{
			game_state_save_core(main_globals->core_file_name);
			main_globals->save_core = false;
		}

		main_save_map_private();
	}

#ifdef PROFILE_ENABLED
	profile_frame_start();
	profile_attribute_enter(0, _profile_attribution_subsystem_0);
	collision_log_begin_frame();
#endif

	if (main_time_should_reset())
	{
		main_time_continue();
	}
	else
	{
		input_windows_update();
		if (!main_time_is_throttled())
		{
			input_abstraction_update();
		}

		saved_game_files_update();
		global_preferences_update();
		font_idle();
		async_idle();
		shell_idle();
#ifdef ERRORS_ENABLED
		error_idle();
#endif
		cache_files_copy_do_work();
		main_loading_idle();
		c_map_manager* map_manager = map_manager_get();
		map_manager->map_synchronize_process(true);

		if (!shell_application_is_paused())
		{
			real32 dt = 0.f;
			real32 world_dt = 0.f;
			real32 game_dt = 0.f;
			int32 target_game_ticks = 0;

			dt = main_time_update();
			world_dt = main_time_halted() ? 0.f : dt;

#ifdef TERMINAL_ENABLED
			terminal_update(world_dt);
			console_update(world_dt);
#endif

#ifdef XBOX
			telnet_console_update();
			xbox_connection_update();
			remote_command_process();
#endif

#ifdef DEBUG_MENU_ENABLED
			debug_menu_update();
#endif

			if (!main_time_is_throttled())
			{
				user_interface_update(dt);
				wmv_update();
			}

			network_receive();
			network_update();
			simulation_update();
			life_cycle_update();

			if (simulation_in_progress())
			{
				if (shell_command_line_flag_is_set(_shell_command_line_flag_custom_map_entry_test_map_name_instead_of_hash))
				{
					// ### TODO FIXME
				}

				game_time_update(world_dt, &game_dt, &target_game_ticks);
				player_control_update(world_dt, game_dt);
				game_update(target_game_ticks, &game_dt);
				network_send();
				halo_interpolator_update_delta();
				game_frame(game_dt);
				director_update(world_dt);
				observer_update(world_dt);
				vibration_update(world_dt);
			}
			else
			{
				simulation_update_pregame();
				network_send();
				vibration_clear_all_now();
			}

			achievement_live_interface_get()->start_upload();

			if (main_time_is_throttled())
			{
				vibration_clear_all_now();
			}
			else
			{
				const uint32 time_before_render = system_milliseconds();
				main_render();
				const uint32 render_time = system_milliseconds() - time_before_render;
				if (!shell_command_line_flag_is_set(_shell_command_line_flag_nosound))
				{
					sound_render();
				}

				if (main_globals->cinematic_sound_sync_in_progress)
				{
					main_globals->cinematic_sound_sync_in_progress = false;
					main_globals->cinematic_sound_sync_completed = true;
					main_time_advance(render_time);
				}
			}

			exceptions_update();

#ifdef PROFILE_ENABLED
			collision_log_end_frame();
			profile_attribute_exit(0, _profile_attribution_subsystem_0);
			profile_frame_end();
#endif
		}
	}

	EventHandler::GameLoopEventExecute(EventExecutionType::execute_after);
	return;
}

void __cdecl main_loop_body_server(void)
{
	EventHandler::GameLoopEventExecute(EventExecutionType::execute_before);

	if (!main_game_loaded_map() && !main_game_loaded_pregame())
	{
		main_game_change_immediate(NULL);
		user_interface_set_context_presence(_context_presence_network_in_game);
	}

	main_game_change_update();
	main_save_map_private();

#ifdef ERRORS_ENABLED
	error_idle();
#endif

	cache_files_copy_do_work();
	if (!shell_application_is_paused())
	{
		real32 dt = 0.f;
		real32 world_dt = 0.f;
		real32 game_dt = 0.f;
		int32 target_game_ticks = 0;

		dt = main_time_update();
		world_dt = main_time_halted() ? 0.f : dt;

#ifdef TERMINAL_ENABLED
#if false					// Disable on dedi because we can't render the terminal
		terminal_update(world_dt);
		console_update(world_dt);
#endif
#endif

		network_receive();
		network_update();
		simulation_update();
		life_cycle_update();
		network_search_update();
		if (simulation_in_progress())
		{
			game_time_update(world_dt, &game_dt, &target_game_ticks);
			player_control_update(world_dt, game_dt);
			game_update(target_game_ticks, &game_dt);
			network_send();
			game_frame(game_dt);
			director_update(world_dt);
			observer_update(world_dt);
		}
		else
		{
			simulation_update_pregame();
			network_send();
		}
	}

	EventHandler::GameLoopEventExecute(EventExecutionType::execute_after);
	return;
}

void main_status(const char* status_type, const char* format, ...)
{
	// TODO: implement

	ASSERT(status_type != NULL);
	return;
}

void main_halt_and_catch_fire(void)
{
	// TODO: implement
	return;
}

void main_crash(const char* str)
{
	stack_walk(0);
	if (!csstricmp(str, "test_fatal_error"))
	{
		handle_fatal_error(-1, "testing fatal error yay!");
	}
	else if (!csstricmp(str, "assert"))
	{
		DISPLAY_ASSERT("asserting on command");
	}
	else if (!csstricmp(str, "now"))
	{
		char** null = NULL;
		*null = "chucky was here!  NULL belongs to me!!!!!";
	}
	else if (!csstricmp(str, "gpu"))
	{
		// nullsub()
	}
	else if (!csstricmp(str, "halt"))
	{
		main_halt_and_catch_fire();
	}
	return;
}

/* private code */

static s_main_globals* main_globals_get(void)
{
	return Memory::GetAddress<s_main_globals*>(0x482208, 0x4A7080);
}

static void main_loop_initialize(void)
{
	s_main_globals* main_globals = main_globals_get();
	main_globals->startup_sequence = true;

	main_loading_initialize();
	SYSTEM_DEBUG_MEMORY(main_loading_initialize());
	main_game_initialize();
	SYSTEM_DEBUG_MEMORY(main_game_initialize());
	main_time_initialize();
	SYSTEM_DEBUG_MEMORY(main_time_initialize());
#ifdef TERMINAL_ENABLED
	console_initialize();
	SYSTEM_DEBUG_MEMORY(console_initialize());
#endif
	game_initialize();
	SYSTEM_DEBUG_MEMORY(game_initialize());
#ifdef TERMINAL_ENABLED
	console_execute_initial_commands();
	SYSTEM_DEBUG_MEMORY(console_execute_initial_commands());
#endif

	c_map_manager* g_map_manager = map_manager_get();
	g_map_manager->load_data();
	g_map_manager->start_map_synchronize();
	SYSTEM_DEBUG_MEMORY("after starting map manager synchronize()");
	return;
}

static void main_save_map_private(void)
{
	// Patch added for tabbing in and out?
	if (!shell_is_dedicated_server())
	{
		if (rasterizer_device_is_lost())
		{
			rasterizer_globals_get()->reset_screen = true;
		}
	}
	
	INVOKE(0x39783, 0x4129C, main_save_map_private);
	return;
}

static void __cdecl main_game_reset_map_blue_screen_detection(void)
{
	s_simulation_globals* sim_globals = simulation_get_globals();
	if (sim_globals->simulation_reset_in_progress)
	{
		EventHandler::BlueScreenEventExecute(EventExecutionType::execute_before);
	}
	main_reset_map_immediate();
	if (sim_globals->simulation_reset_in_progress)
	{
		EventHandler::BlueScreenEventExecute(EventExecutionType::execute_after);
	}
}

