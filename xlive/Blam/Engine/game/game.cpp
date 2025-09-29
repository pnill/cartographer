#include "stdafx.h"
#include "game.h"

#include "game_engine.h"
#include "game_grief.h"
#include "game_results.h"
#include "game_time.h"
#include "players.h"

#include "camera/director.h"
#include "cutscene/cinematics.h"
#include "cutscene/recorded_animations.h"
#include "effects/effects.h"
#include "effects/player_effects.h"
#include "hs/hs.h"
#include "interface/hud.h"
#include "interface/motion_sensor.h"
#include "interface/user_interface_networking.h"
#include "math/random_math.h"
#include "main/interpolator.h"
#include "main/main.h"
#include "main/main_game_time.h"
#include "networking/logic/life_cycle_manager.h"
#include "objects/lights.h"
#include "physics/havok.h"
#include "physics/impacts.h"
#include "sapien/editor.h"
#include "saved_games/game_state.h"
#include "shell/shell.h"
#include "simulation/simulation.h"
#include "sound/game_sound_deterministic.h"
#include "text/unicode.h"
#include "rasterizer/rasterizer_globals.h"
#include "rasterizer/dx9/rasterizer_dx9_main.h"

/* typedefs */

typedef void(__cdecl* game_frame_t)(real32);
typedef void(__cdecl* t_main_loop_process_global_state_changes)();

/* structures */


/* prototypes */

static void set_main_game_globals(game_globals_storage* main);

static void __cdecl main_loop_process_global_state_changes_hook(void);

static void game_info_initialize_for_new_map(const s_game_options* options);

static void __cdecl game_tick_pulse_random_seed_deterministic(const simulation_update* update);

static void __cdecl game_update_pvs(void);

static void __cdecl game_loss_update(void);

static void __cdecl game_finished_update(void);

static void __cdecl game_save_update(void);

/* globals */

game_frame_t p_game_frame;
t_main_loop_process_global_state_changes p_main_loop_process_global_state_changes;


void game_apply_pre_winmain_patches(void)
{
	PatchCall(Memory::GetAddress(0x86BE, 0x1EB86), game_initialize_for_new_map);
	PatchCall(Memory::GetAddress(0x9802, 0x1FAED), game_initialize_for_new_map);
	PatchCall(Memory::GetAddress(0x39D2A, 0xC0C0), game_update);
	PatchCall(Memory::GetAddress(0x39E42, 0xBA4F), game_initialize);

	PatchCall(Memory::GetAddress(0x1DD394, 0x1C4848), game_tick);	// c_simulation_world::time_set_immediate_update
	PatchCall(Memory::GetAddress(0x1DD49A, 0x1C495A), game_tick);	// c_simulation_world::handle_synchronous_update

	// inscrease the max player count to allow ragdolls and the ragdoll count
	WriteValue<int8>(Memory::GetAddress(0x49CCC, 0x42F4A) + 2, k_game_maximum_players_to_allow_ragdolls_new);
	WriteValue<int8>(Memory::GetAddress(0x49CDC, 0x42F5A) + 2, k_game_maximum_ragdolls_new);

	// Get original game_frame function
	if (!shell_is_dedicated_server())
	{
		p_game_frame = Memory::GetAddress<game_frame_t>(0x48CDC, 0x41F7D);

		PatchCall(Memory::GetAddress(0x39D45, 0xC0D4), game_frame);

		// main_loop_process_global_state
		// nop cmp
		NopFill(Memory::GetAddress(0x3978B), 2);
		// then force jmp
		WriteValue(Memory::GetAddress(0x3978D), (uint8)0xEB);
		DETOUR_ATTACH(p_main_loop_process_global_state_changes, Memory::GetAddress<t_main_loop_process_global_state_changes>(0x39783), main_loop_process_global_state_changes_hook);
	}
	return;
}

s_game_systems* get_game_systems(void)
{
	return Memory::GetAddress<s_game_systems*>(0x3A0468, 0x35D198);
}

game_globals_storage* get_main_game_globals(void)
{
	return *Memory::GetAddress<game_globals_storage**>(0x482D3C, 0x4CB520);
}

bool map_initialized(void)
{
	return get_main_game_globals() && get_main_game_globals()->map_active && get_main_game_globals()->active_structure_bsp_index != NONE;
}

s_game_options* game_options_get(void)
{
	game_globals_storage* game_globals = get_main_game_globals();
	// TODO: fix code in H2MOD that causes this assert
	//ASSERT(game_globals && (game_globals->initializing || game_globals->map_active));
	return &game_globals->options;
}

s_game_variant* current_game_variant(void)
{
	return &game_options_get()->game_variant;
}

e_game_mode game_mode_get(void)
{
	return get_main_game_globals()->options.game_mode;
}

bool game_in_editor(void)
{
	return false;
}

bool game_is_campaign(void)
{
	return game_options_get()->game_mode == _game_mode_campaign;
}

bool game_is_multiplayer(void)
{
	return game_options_get()->game_mode == _game_mode_multiplayer;
}

bool game_is_ui_shell(void)
{
	return game_options_get()->game_mode == _game_mode_ui_shell;
}

bool game_is_distributed(void)
{
	const e_game_simulation simulation_type = game_options_get()->simulation_type;
	return simulation_type == _game_simulation_distributed_client || simulation_type == _game_simulation_distributed_server;
}

bool game_is_server(void)
{
	const s_game_options* options = game_options_get();

	return !(options->simulation_type == _game_simulation_synchronous_client || options->simulation_type == _game_simulation_distributed_client);
}

int16 game_get_active_structure_bsp_index()
{
	return get_main_game_globals()->active_structure_bsp_index;
}

// TODO: saved films
bool game_is_playback(void)
{
	return false;
	//return _game_playback_none;
}

void __cdecl game_shell_set_in_progress(void)
{
	INVOKE(0x242E5B, 0x22054B, game_shell_set_in_progress);
	return;
}

bool game_is_predicted(void)
{
	return game_options_get()->simulation_type == _game_simulation_distributed_client;
}

bool game_in_progress(void)
{
	bool result = false;
	if (get_main_game_globals() != NULL && get_main_game_globals()->game_in_progress)
	{
		ASSERT(!get_main_game_globals()->initializing);
		ASSERT(get_main_game_globals()->map_active);

		result = true;
	}

	return result;
}

bool game_is_active(void)
{
	const game_globals_storage* g_main_game_globals = get_main_game_globals();
	return g_main_game_globals && g_main_game_globals->map_active && g_main_game_globals->active_structure_bsp_index != NONE;
}

bool game_is_authoritative(void)
{
	return game_options_get()->simulation_type != _game_simulation_distributed_client;
}

s_game_cluster_bit_vectors* game_get_cluster_activation(void)
{
	return &get_main_game_globals()->cluster_activation;
}

void __cdecl game_options_setup_default_players(int32 player_count, s_game_options* game_options)
{
	ASSERT(player_count > 0 && player_count <= k_number_of_controllers);

	INVOKE(0x49650, 0x428ED, game_options_setup_default_players, player_count, game_options);
	return;
}


void game_time_get_date_and_time(s_date_and_time* date_and_time)
{
	ASSERT(date_and_time);

	_SYSTEMTIME SystemTime = {};
	GetLocalTime(&SystemTime);
	date_and_time->year = SystemTime.wYear;
	date_and_time->month = SystemTime.wMonth;
	date_and_time->day = SystemTime.wDay;
	date_and_time->hour = SystemTime.wHour;
	date_and_time->minute = SystemTime.wMinute;
	date_and_time->second = SystemTime.wSecond;
	return;
}

void game_direct_connect_to_session(XNKID kid, XNKEY key, const XNADDR* addr, int8 exe_type, int32 exe_version, int32 comp_version)
{
	auto handler = (c_game_life_cycle_handler_joining*)c_game_life_cycle_manager::get()->m_life_cycle_handlers[_life_cycle_joining];
	handler->joining_xnkid = kid;
	handler->joining_xnkey = key;
	handler->joining_xnaddr = *addr;
	if (exe_type != EXECUTABLE_TYPE || exe_version != EXECUTABLE_VERSION || comp_version != COMPATIBLE_VERSION)
	{
		handler->join_attempt_result_code = 9;
	}
	else
	{
		c_game_life_cycle_handler_joining::check_joining_capability();
		wchar_t local_usernames[k_number_of_users][XUSER_NAME_SIZE] = {};
		s_player_identifier local_identifiers[k_number_of_users] = {};
		
		size_t valid_local_player_count = 0;
		
		for (int32 i = 0; i < k_number_of_users; i++)
		{
			s_player_identifier temp_identifier;
			s_player_properties temp_properties;
			if (network_session_interface_get_local_user_identifier(i, &temp_identifier) || network_session_interface_get_local_user_properties(i, 0, &temp_properties, 0, 0))
			{
				ustrncpy(local_usernames[valid_local_player_count], temp_properties.player_name, NUMBEROF(temp_properties.player_name));
				local_identifiers[valid_local_player_count] = temp_identifier;
				++valid_local_player_count;
			}
		}

		user_interface_networking_reset_player_counts();
		network_globals_switch_environment(2, 1);
		csmemcpy(&handler->player_identifiers, local_identifiers, sizeof(local_identifiers));
		csmemcpy(&handler->player_names, local_usernames, sizeof(local_usernames));
		handler->field_11 = 0; //Always 0 in the original function
		handler->field_12 = 0; //Always 0 in the original function
		handler->field_14 = 1;
		handler->joining_user_count = valid_local_player_count;
		handler->field_54 = 2; //Always 2 in original function
		handler->field_10 = true; //Always true in original function

		handler->join_attempt_result_code = 0; //Force valid result code, leave the denying the connection up to the host.
	}
	c_game_life_cycle_manager::get()->request_state_change(_life_cycle_joining, 0, 0);
	game_shell_set_in_progress();
}


void __cdecl game_initialize(void)
{
	game_state_initialize();
	game_globals_storage* main_game_globals = (game_globals_storage*)game_state_malloc("game globals", NULL, sizeof(game_globals_storage));
	csmemset(main_game_globals, 0, sizeof(game_globals_storage));
	main_game_globals->active_structure_bsp_index = NONE;
	*Memory::GetAddress<game_globals_storage**>(0x482D3C, 0x4CB520) = main_game_globals;    // Write allocated globals back to the original exe

	real_math_reset_precision();

	s_game_systems* g_game_systems = get_game_systems();
	for (int32 i = 0; i < 70; ++i)
	{
		g_game_systems[i].initialize_proc();
	}

	// Interpolation allocation
	const bool initialize_interpolation = shell_application_type() != _shell_application_tool && !shell_is_dedicated_server();
	if (initialize_interpolation)
	{
		halo_interpolator_initialize();
	}
	return;
}

void __cdecl game_dispose(void)
{
    set_main_game_globals(NULL);
    s_game_systems* g_game_systems = get_game_systems();
    for (int32 system_index = 69; system_index >= 0; --system_index)
    {
        ASSERT(g_game_systems[system_index].dispose_proc);
        g_game_systems[system_index].dispose_proc();
    }
    
	halo_interpolator_dispose();

    // reset time resolution to system default on game exit (initialization happens in main_game_time_initialize_hook())
    timeEndPeriod(SYSTEM_TIMER_RESOLUTION_MS);
    return;
}

bool __cdecl main_events_pending(void)
{
	return INVOKE(0x396B1, 0x411D0, main_events_pending);
}

void __cdecl game_tick(void)
{
	//INVOKE(0x4A4AF, 0x4372D, game_tick);

	game_globals_storage* game_globals = get_main_game_globals();
	ASSERT(game_globals && game_globals->map_active && game_globals->active_structure_bsp_index != NONE);

	struct simulation_update update;

	//main_status("game_tick", "time %d", game_time_get());

	real_math_reset_precision();
	simulation_build_update(&update);
	
	random_seed_allow_use();

	simulation_apply_before_game(&update);
	if (update.simulation_in_progress)
	{
		players_update_before_game(&update);
		game_tick_pulse_random_seed_deterministic(&update);
		ai_update();
		recorded_animations_update();
		game_sound_deterministic_update_timers();
		game_engine_update();
		game_results_update();
		editor_update();
		hs_update();
		game_update_pvs();
		
		objects_update();
		havok_update();
		objects_move();
		objects_post_update();
		impacts_update();
		effects_update();
		lights_update();
		
		game_engine_update_after_game();
		simulation_apply_after_game(&update);
		players_update_after_game(&update);
		
		game_allegiance_update();
		game_loss_update();
		game_finished_update();
		game_save_update();
		cinematic_update();
		game_grief_update();
		
		random_seed_disallow_use();

		first_person_weapons_update();
		player_effect_update();
		hud_update();
		observer_game_tick();
		director_game_tick();
	}

	simulation_update_aftermath(&update);
	if (update.simulation_in_progress)
	{
		game_time_advance();
	}

	//main_status("game_tick", NULL);

	return;
}

void __cdecl game_update(int32 desired_ticks, real32* elapsed_game_dt)
{
	int32 actual_ticks = 0;
	if (desired_ticks > 0)
	{
		while (!main_events_pending())
		{
			halo_interpolator_update_begin();
			game_tick();
			halo_interpolator_update_end();
			if (cinematic_sound_sync_complete())
			{
				break;
			}
			if (++actual_ticks >= desired_ticks)
			{
				return;
			}
		}
		if (actual_ticks < desired_ticks)
		{
			game_time_discard(desired_ticks, actual_ticks, elapsed_game_dt);
		}
	}
	else
	{
		simulation_get_globals()->world->queues_update_statistics();
	}

	return;
}

void __cdecl game_initialize_for_new_map(const s_game_options* options)
{
	game_globals_storage* game_globals = get_main_game_globals();

	ASSERT(options);
	// TODO: implement 
	//ASSERT(main_game_loaded_map());
	//ASSERT(wcscmp(options->scenario_path, main_game_loaded_map_name()) == 0);
	ASSERT(!game_globals->initializing);
	ASSERT(!game_globals->map_active);
	ASSERT(!game_globals->game_in_progress);
	ASSERT(game_globals->active_structure_bsp_index == NONE);

	halo_interpolator_reset();
	real_math_reset_precision();
	game_globals->initializing = true;
	game_info_initialize_for_new_map(options);

	s_game_systems* g_game_systems = get_game_systems();
	for (int32 i = 0; i < 70; i++)
	{
		if (g_game_systems[i].initialize_for_new_map_proc)
		{
			g_game_systems[i].initialize_for_new_map_proc();
		}
	}
	game_globals->initializing = false;
	game_globals->map_active = true;
	return;
}

void __cdecl game_frame(real32 dt)
{
	if (halo_frame_interpolator_enabled())
	{
		halo_interpolator_update_delta();
		motion_sensor_update_with_delta(dt);
	}

	INVOKE(0x48CDC, 0x41F7D, game_frame, dt);
	return;
}

/* private code */

static void set_main_game_globals(game_globals_storage* main)
{
	*Memory::GetAddress<game_globals_storage**>(0x482D3C, 0x4CB520) = main;
	return;
}

static void __cdecl main_loop_process_global_state_changes_hook(void)
{
	IDirect3DDevice9* d3d_device = rasterizer_dx9_device_get_interface();

	if (d3d_device && FAILED(d3d_device->TestCooperativeLevel()))
	{
		rasterizer_globals_get()->reset_screen = true;
	}
	p_main_loop_process_global_state_changes();
	return;
}

static void game_info_initialize_for_new_map(const s_game_options* options)
{
	game_globals_storage* game_globals = get_main_game_globals();

	game_globals->options = *options;
	game_globals->options.load_level_only = false;

	if (game_is_multiplayer() || game_globals->options.game_variant.variant_game_engine_index)
	{
		game_engine_variant_cleanup(&game_globals->options.game_variant.flags);
	}
	random_math_set_seed(game_globals->options.random_seed);
	game_globals->game_is_lost = false;
	game_globals->game_is_finished = false;
	game_globals->pvs_object_is_set = 0;
	game_globals->game_ragdoll_count = 0;
	return;
}

static void __cdecl game_tick_pulse_random_seed_deterministic(const struct simulation_update* update)
{
	INVOKE(0x49B02, 0x42D80, game_tick_pulse_random_seed_deterministic, update);
	return;
}

static void __cdecl game_update_pvs(void)
{
	INVOKE(0x4A16D, 0x433EB, game_update_pvs);
	return;
}

static void __cdecl game_loss_update(void)
{
	INVOKE(0x4A3FB, 0x43679, game_loss_update);
	return;
}

static void __cdecl game_finished_update(void)
{
	INVOKE(0x49B9D, 0x42E1B, game_finished_update);
	return;
}

static void __cdecl game_save_update(void)
{
	INVOKE(0x9E673, 0x90909, game_save_update);
	return;
}
