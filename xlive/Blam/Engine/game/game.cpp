#include "stdafx.h"
#include "game.h"

#include "game_engine.h"
#include "game_grief.h"
#include "game_options.h"
#include "game_results.h"
#include "game_time.h"
#include "players.h"

#include "ai/ai.h"
#include "camera/director.h"
#include "cutscene/cinematics.h"
#include "cutscene/recorded_animations.h"
#include "effects/effects.h"
#include "effects/player_effects.h"
#include "hs/hs.h"
#include "interface/hud.h"
#include "interface/motion_sensor.h"
#include "math/random_math.h"
#include "main/interpolator.h"
#include "main/console.h"
#include "main/main.h"
#include "main/main_time.h"
#include "objects/lights.h"
#include "objects/objects.h"
#include "physics/havok.h"
#include "physics/impacts.h"
#include "sapien/editor.h"
#include "saved_games/game_state.h"
#include "scenario/scenario.h"
#include "scenario/scenario_definitions.h"
#include "shell/shell.h"
#include "simulation/simulation.h"
#include "simulation/simulation_update.h"
#include "simulation/simulation_world.h"
#include "structures/cluster_partitions.h"
#include "sound/game_sound_deterministic.h"

/* globals */

char const* global_campaign_difficulty_level_names[4]
{
	"easy",
	"normal",
	"heroic",
	"legendary"
};

char const* k_game_playback_names[k_game_simulation_count]
{
	"none"
	"local",
	"sync-client",
	"sync-server",
	"dist-client",
	"dist-server",
	"none"
};

/* structures */

struct game_globals_storage
{
	bool initializing;
	bool map_active;
	int16 active_structure_bsp_index;
	int32 unused_0;
	s_game_options options;
	bool game_in_progress;
	bool game_is_lost;
	int32 ticks_to_reset_game;
	bool game_is_finished;
	bool game_sounds_disabled;
	int16 pad_1;
	uint32 ticks_till_end;
	int32 game_ragdoll_count;
	int32 unused_1;
	s_game_cluster_bit_vectors cluster_pvs;
	s_game_cluster_bit_vectors cluster_pvs_local;
	s_game_cluster_bit_vectors cluster_activation;
	uint8 enable_scripted_camera_pvs;
	uint8 pad_2;
	uint16 pvs_activation_mode;
	datum pvs_object_datum;
};
ASSERT_STRUCT_SIZE(game_globals_storage, 0x1270);

/* prototypes */

static void set_main_game_globals(game_globals_storage* main);
static game_globals_storage* get_main_game_globals(void);
static void game_info_initialize_for_new_map(const s_game_options* options);

static void __cdecl game_tick_pulse_random_seed_deterministic(const struct simulation_update* update);

static void __cdecl game_update_pvs(void);

static void __cdecl game_loss_update(void);

static void __cdecl game_finished_update(void);

static void __cdecl game_save_update(void);

/* public code */

void game_apply_pre_winmain_patches(void)
{
	PatchCall(Memory::GetAddress(0x86BE, 0x1EB86), game_initialize_for_new_map);
	PatchCall(Memory::GetAddress(0x9802, 0x1FAED), game_initialize_for_new_map);
	PatchCall(Memory::GetAddress(0x39D2A, 0xC0C0), game_update);
	PatchCall(Memory::GetAddress(0x39E42, 0xBA4F), game_initialize);

	PatchCall(Memory::GetAddress(0x1DD394, 0x1C4848), game_tick);	// c_simulation_world::time_set_immediate_update
	PatchCall(Memory::GetAddress(0x1DD49A, 0x1C495A), game_tick);	// c_simulation_world::handle_synchronous_update

	// increase the max player count to allow ragdolls and the ragdoll count
	WriteValue<int8>(Memory::GetAddress(0x49CCC, 0x42F4A) + 2, k_game_maximum_players_to_allow_ragdolls_new);
	WriteValue<int8>(Memory::GetAddress(0x49CDC, 0x42F5A) + 2, k_game_maximum_ragdolls_new);
	
	// Replace function so we can initialize ai in MP
	WritePointer((uintptr_t)&get_game_systems()[59].initialize_for_new_map_proc, ai_initialize_for_new_map);

	if (shell_is_dedicated_server())
	{
		PatchCall(Memory::GetAddress(0x0, 0xC6F7), game_dispose);	// main_loop
	}

	PatchCall(Memory::GetAddress(0xA57AB, 0x9790F), random_math_get_globals);
	PatchCall(Memory::GetAddress(0xA5892, 0x979828), random_math_get_globals);

	return;
}

s_game_systems* get_game_systems(void)
{
	return Memory::GetAddress<s_game_systems*>(0x3A0468, 0x35D198);
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

bool game_is_cooperative(void)
{
	return game_options_get()->game_mode == _game_mode_campaign && game_options_get()->coop;
}

bool game_is_distributed(void)
{
	const e_game_simulation simulation_type = game_simulation_get();
	return simulation_type == _game_simulation_distributed_client || simulation_type == _game_simulation_distributed_server;
}

e_game_simulation game_simulation_get(void)
{
	return (e_game_simulation)game_options_get()->game_simulation;
}

bool game_is_networked(void)
{
	e_game_simulation simulation_type = game_simulation_get();
	return simulation_type >= _game_simulation_synchronous_client && simulation_type <= _game_simulation_distributed_server;
}

bool game_is_server(void)
{
	const s_game_options* options = game_options_get();

	return !(options->game_simulation == _game_simulation_synchronous_client || options->game_simulation == _game_simulation_distributed_client);
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
	return game_options_get()->game_simulation == _game_simulation_distributed_client;
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
	return game_options_get()->game_simulation != _game_simulation_distributed_client;
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

void __cdecl game_initialize(void)
{
	game_state_initialize();
	game_globals_storage* main_game_globals = (game_globals_storage*)game_state_malloc("game globals", NULL, sizeof(game_globals_storage));
	csmemset(main_game_globals, 0, sizeof(game_globals_storage));
	main_game_globals->active_structure_bsp_index = NONE;
	*Memory::GetAddress<game_globals_storage**>(0x482D3C, 0x4CB520) = main_game_globals;    // Write allocated globals back to the original exe

	real_math_reset_precision();

	s_game_systems* g_game_systems = get_game_systems();
	for (int32 i = 0; i < k_game_system_count; ++i)
	{
		g_game_systems[i].initialize_proc();
	}

	// Interpolation allocation
	const bool initialize_interpolation = shell_application_type() != _shell_application_tool && !shell_is_dedicated_server();
	if (initialize_interpolation)
	{
		halo_interpolator_initialize();
	}

	// TODO: Remove this when we rewrite main_loop_initialize for dedicated servers
	if (shell_is_dedicated_server())
	{
		console_execute_initial_commands();
	}

	return;
}

void __cdecl game_dispose(void)
{
	set_main_game_globals(NULL);
	s_game_systems* g_game_systems = get_game_systems();
	for (int32 system_index = k_game_system_count - 1; system_index >= 0; --system_index)
	{
		ASSERT(g_game_systems[system_index].dispose_proc);
		g_game_systems[system_index].dispose_proc();
	}
	
	halo_interpolator_dispose();
	return;
}

bool __cdecl main_events_pending(void)
{
	return INVOKE(0x396B1, 0x411D0, main_events_pending);
}

void __cdecl game_tick(void)
{
	//INVOKE(0x4A4AF, 0x4372D, game_tick);

#if ASSERTS_ENABLED
	game_globals_storage* game_globals = get_main_game_globals();
	ASSERT(game_globals && game_globals->map_active && game_globals->active_structure_bsp_index != NONE);
#endif

	struct simulation_update update;

	main_status("game_tick", "time %d", game_time_get());

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
	else
	{
		random_seed_disallow_use();
	}

	simulation_update_aftermath(&update);
	if (update.simulation_in_progress)
	{
		game_time_advance();
	}

	main_status("game_tick", NULL);

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
		simulation_get_world()->queues_update_statistics();
	}

	return;
}

void __cdecl game_initialize_for_new_map(
	const s_game_options* options)
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
	for (int32 i = 0; i < k_game_system_count; ++i)
	{
		if (g_game_systems[i].initialize_for_new_map_proc)
		{
			g_game_systems[i].initialize_for_new_map_proc();
		}
	}
	game_globals->initializing = false;
	game_globals->map_active = true;

	// The permutation - history bit vector lives in a system_heap_alloc'd buffer (HeapAlloc, NOT
	// zeroed - the pointer at IDA 0x8CE86C, 0x1880 bytes) and is allocated once per process by
	// game_sound_deterministic_initialize. With the picks now drawn from the shared deterministic
	// seed, the remaining machine-local input is this buffer's INITIAL contents: heap garbage,
	// plus whatever history a machine accumulated in menus or earlier sessions. Zeroing it at map
	// init - the same point that seeds the deterministic rng (game_info_initialize_for_new_map) -
	// gives both machines an identical starting state, so the history evolves identically from
	// tick 0. Retail's own intent was clearly this: it allocates a game-state-heap twin of this
	// buffer ("deterministic game sound globals") and then never uses it.
	uint8 *sound_permutation_state = *Memory::GetAddress<uint8**>(0x4CE86C, 0x4F5058);
	if (sound_permutation_state)
	{
		csmemset(sound_permutation_state, 0, 0x1880);
	}

	return;
}

void __cdecl game_frame(real32 dt)
{
	if (!shell_is_dedicated_server())
	{
		motion_sensor_update_with_delta(dt);
	}
	INVOKE(0x48CDC, 0x41F7D, game_frame, dt);
	return;
}

int16 game_get_active_structure_bsp_index(
	void)
{
	const game_globals_storage* game_globals = get_main_game_globals();

	//ASSERT(game_options_verify(&game_globals->options));
	ASSERT(game_globals->map_active);

	
	int16 structure_bsp_index = game_globals->active_structure_bsp_index;
	ASSERT(structure_bsp_index==NONE || (structure_bsp_index>=0 && structure_bsp_index<global_scenario_get()->structure_bsp_references.count));
	return structure_bsp_index;
}

void game_simulation_set(
	e_game_simulation game_simulation)
{
	game_globals_storage* game_globals = get_main_game_globals();
	ASSERT(game_simulation>_game_simulation_none && game_simulation<k_game_simulation_count);
	ASSERT(game_globals && (game_globals->initializing || game_globals->map_active));
	
	game_globals->options.game_simulation = (int8)game_simulation;
	main_status("game_simulation", "%s", k_game_playback_names[game_simulation]);

	return;
}

void game_globals_storage_print_debug_contents(
	FILE* file)
{
	game_globals_storage* game_globals = get_main_game_globals();

	ASSERT(file);

	fwprintf(file, L"GAME GLOBALS\n");
	fwprintf(file, L"%ls", L"=============================================\n");

	fwprintf(file, L"Initializing: ");
	fwprintf(file, L"%ls\n", (game_globals->initializing ? L"True" : L"False"));

	fwprintf(file, L"Map Active: ");
	fwprintf(file, L"%ls\n", (game_globals->map_active ? L"True" : L"False"));

	fwprintf(file, L"Active Structure BSP Index: %hd", game_globals->active_structure_bsp_index);

	fwprintf(file, L"Game In Progress: ");
	fwprintf(file, L"%ls\n", (game_globals->game_in_progress ? L"True" : L"False"));

	fwprintf(file, L"Game Is Lost: ");
	fwprintf(file, L"%ls\n", (game_globals->game_is_lost ? L"True" : L"False"));

	fwprintf(file, L"Ticks To Reset Game: %d\n", game_globals->ticks_to_reset_game);

	fwprintf(file, L"Game Is Finished: ");
	fwprintf(file, L"%ls\n", (game_globals->game_is_finished ? L"True" : L"False"));

	fwprintf(file, L"Game Sounds Disabled: ");
	fwprintf(file, L"%ls\n", (game_globals->game_sounds_disabled ? L"True" : L"False"));

	fwprintf(file, L"Ticks Till End: %u\n", game_globals->ticks_till_end);

	fwprintf(file, L"Game Ragdoll Count: %d\n", game_globals->game_ragdoll_count);

	fwprintf(file, L"Enable Scripted Camera PVS: ");
	fwprintf(file, L"%ls\n", (game_globals->enable_scripted_camera_pvs ? L"True" : L"False"));

	fwprintf(file, L"PVS Object Is Set: %hu\n", game_globals->pvs_activation_mode);

	fwprintf(file, L"PVS Object Datum: %d\n", game_globals->pvs_object_datum);

	return;
}

/* private code */

static void set_main_game_globals(
	game_globals_storage* main)
{
	*Memory::GetAddress<game_globals_storage**>(0x482D3C, 0x4CB520) = main;
	return;
}

static game_globals_storage* get_main_game_globals(
	void)
{
	return *Memory::GetAddress<game_globals_storage**>(0x482D3C, 0x4CB520);
}

static void game_info_initialize_for_new_map(const s_game_options* options)
{
	game_globals_storage* game_globals = get_main_game_globals();

	game_globals->options = *options;
	game_globals->options.load_level_only = false;

	if (game_is_multiplayer() || game_globals->options.game_variant.variant_game_engine_index)
	{
		game_engine_variant_cleanup(&game_globals->options.game_variant);
	}
	random_math_set_seed(game_globals->options.verify_random_seed);
	game_globals->game_is_lost = false;
	game_globals->game_is_finished = false;
	game_globals->pvs_activation_mode = 0;
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

