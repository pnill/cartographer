#include "stdafx.h"
#include "main_game.h"

#include "main.h"

#include "cache/cache_files.h"
#include "cache/pc_texture_cache.h"
#include "game/game.h"
#include "game/game_globals.h"
#include "game/game_options.h"
#include "interface/damaged_media.h"
#include "interface/user_interface.h"
#include "networking/network_event.h"
#include "saved_games/game_state.h"
#include "saved_games/game_variant.h"

#include <XLive/XAM/xam.h>

/* constants */

enum e_game_loaded_status
{
	_game_loaded_status_none = 0,
	_game_loaded_status_map_loading,
	_game_loaded_status_map_loaded,
	_game_loaded_status_map_unloading,
	_game_loaded_status_map_reloading,
	_game_loaded_status_pregame,
	k_game_loaded_status_count,
};

/* structures */

struct s_main_game_globals
{
	int32 game_loaded_status;
	wchar_t game_loaded_scenario_path[MAX_PATH];
	bool map_reset_in_progress;
	bool map_advance_pending;
	bool map_change_pending;
	bool map_change_pending_unload;
	uint32 map_change_initiate_time;
	uint32 map_change_timer;
	bool unk_0;
	bool unk_1;
	bool unk_2;
	s_game_options pending_game_options;
};

// not compiled in the public version
typedef struct
{
	bool load_core_on_game_launch;
	char core_name[255];
} s_main_game_globals_debug;

s_main_game_globals_debug g_main_game_globals_debug;

/* prototypes */

static s_main_game_globals* main_game_globals_get(void);

// Setup default values for the options structure depending on the game mode set
static void main_game_launch_setup_game_mode_details(void);

// Perform validation on the game launch options for campaign
static void main_game_launch_set_campaign_details(void);

// Perform validation on the game launch options for multiplayer
static void main_game_launch_set_multiplayer_details(void);

// Set default details for the mainmenu
static void main_game_launch_set_ui_shell_details(void);

static void main_game_internal_pregame_load(void);

static void main_game_load_panic(void);

static void main_menu_build_game_options(s_game_options* options, int32 menu_context);

/* globals */

static const s_variant_description_map k_launch_multiplayer_variants[k_variant_count] =
{
	{ "slayer", _game_variant_description_slayer },
	{ "oddball", _game_variant_description_oddball },
	{ "juggernaut", _game_variant_description_juggernaut },
	{ "king", _game_variant_description_king },
	{ "ctf", _game_variant_description_ctf },
	{ "invasion", _game_variant_description_invasion },
	{ "territories", _game_variant_description_territories }
};

static bool g_main_menu_launch_delay_xlive_ui = true;

int32 g_main_game_launch_user_count = 1;
s_game_options g_main_game_launch_options;

bool debug_load_panic_to_main_menu;

/* public code */

void main_game_apply_patches(void)
{
	PatchCall(Memory::GetAddress(0x9844), main_game_load_panic);
	PatchCall(Memory::GetAddress(0x39675), main_game_load_panic);
	return;
}

void main_game_initialize(void)
{
	game_options_new(&g_main_game_launch_options);
	return;
}

bool __cdecl main_game_loaded_map(void)
{
	return main_game_globals_get()->game_loaded_status == _game_loaded_status_map_loaded;
}

bool __cdecl main_game_loaded_pregame(void)
{
	return main_game_globals_get()->game_loaded_status == _game_loaded_status_pregame;
}

void __cdecl main_game_launch_default(void)
{
	INVOKE(0x96EB, 0x0, main_game_launch_default);
	return;
}

void __cdecl main_game_reset_map(void)
{
	INVOKE(0x9763, 0x0, main_game_reset_map);
	return;
}

void __cdecl main_game_unload_and_prepare_for_next_game(void)
{
	INVOKE(0x8A7E, 0x0, main_game_unload_and_prepare_for_next_game);
	return;
}

bool __cdecl main_game_change_update(void)
{
	return INVOKE(0x985E, 0x1FB49, main_game_change_update);
}

void main_game_launch_set_map_name(const char* map_name)
{
	MultiByteToWideChar(CP_UTF8, 0, map_name, -1, g_main_game_launch_options.scenario_path, 260);
	return;
}

void main_game_load_from_core()
{
	main_game_load_from_core_name("core");
}

void main_game_load_from_core_name(const char* core_name)
{
	s_game_options game_options;
	if (game_state_get_game_options_from_core(core_name, &game_options))
	{
		csstrncpy(g_main_game_globals_debug.core_name, core_name, ARRAYSIZE(g_main_game_globals_debug.core_name));
		g_main_game_globals_debug.load_core_on_game_launch = true;
		main_game_change(&game_options);
	}
	else
	{
		event(_event_message, "Failed to get game options from core (so I can't load it!)");
	}
}

void main_game_load_post_game_launch()
{
	if (g_main_game_globals_debug.load_core_on_game_launch)
	{
		main_load_core_name(g_main_game_globals_debug.core_name);
		g_main_game_globals_debug.load_core_on_game_launch = false;
	}
}

// TODO rewrite the obfuscated function
bool __cdecl main_game_change(const s_game_options* options)
{
	return INVOKE(0x89BA, 0x1E4EC, main_game_change, options);
}

bool __cdecl main_game_change_immediate(const s_game_options* options)
{
	return INVOKE(0x911B, 0x1F523, main_game_change_immediate, options);
}

void main_game_launch_set_difficulty(int16 difficulty)
{
	ASSERT(VALID_INDEX(difficulty, k_campaign_difficulty_levels_count));
	g_main_game_launch_options.difficulty = difficulty;
	g_main_game_launch_options.game_mode = _game_mode_campaign;
	return;
}

void main_game_launch_set_coop_player_count(int32 player_count)
{
	if (IN_RANGE(player_count, 1, k_number_of_users))
	{
		g_main_game_launch_options.game_mode = _game_mode_campaign;
		g_main_game_launch_options.coop = player_count > 1;
		g_main_game_launch_user_count = player_count;
	}
	else
	{
		error(_error_silent, "%s: invalid player count %d (must be from 1-%d)", __FUNCTION__, player_count, k_number_of_users);
	}
	return;
}

void main_game_launch_set_multiplayer_splitscreen_count(int32 player_count)
{
	if (IN_RANGE(player_count, 1, k_number_of_users))
	{
		g_main_game_launch_options.game_mode = _game_mode_multiplayer;
		g_main_game_launch_user_count = player_count;
		if (g_main_game_launch_options.game_variant.variant_game_engine_index == _game_engine_type_none)
		{
			game_variant_build_default(&g_main_game_launch_options.game_variant, _game_variant_description_slayer);
			g_main_game_launch_options.game_variant.round_time_limit = 0;
		}
	}
	else
	{
		error(_error_silent, "%s: invalid player count %d (must be from 1-%d)", __FUNCTION__, player_count, k_number_of_users);
	}
	return;
}

void main_game_launch_set_multiplayer_variant(const char* variant_name)
{	
	size_t i = 0;
	for (; i < k_variant_count; ++i)
	{
		if (!csstricmp(variant_name, k_launch_multiplayer_variants[i].name))
		{
			break;
		}
	}

	if (i == k_variant_count)
	{
		error(_error_silent, "%s: invalid variant name [%s] provided, defaulting to slayer", __FUNCTION__, variant_name);
		game_variant_build_default(&g_main_game_launch_options.game_variant, _game_variant_description_slayer);
	}
	else
	{
		game_variant_build_default(&g_main_game_launch_options.game_variant, k_launch_multiplayer_variants[i].index);
	}

	g_main_game_launch_options.game_variant.round_time_limit = 0;
	g_main_game_launch_options.game_mode = _game_mode_multiplayer;
	return;
}

void main_game_launch_set_game_mode(int32 game_mode)
{
	if (IN_RANGE(game_mode, 1, (int)k_game_mode_count - 1))
	{
		g_main_game_launch_options.game_mode = (e_game_mode)game_mode;
	}
	else
	{
		error(_error_silent, "%s: invalid game mode [%d] provided", __FUNCTION__, game_mode);
	}
	return;
}

void main_game_launch_legacy(const char* map_name)
{
	damaged_media_clear_error();
	main_game_launch(map_name);
	return;
}

void main_game_launch(const char* map_name)
{
	cache_file_map_clear_all_failures();
	main_game_launch_set_map_name(map_name);
	main_game_launch_setup_game_mode_details();
	game_options_setup_default_players(g_main_game_launch_user_count, &g_main_game_launch_options);
	main_game_change(&g_main_game_launch_options);
	return;
}

void main_menu_launch(uint32 context)
{
	s_game_options options;

	if (game_in_progress() && game_is_ui_shell())
	{
		user_interface_enter_game_shell(context);
	}
	else
	{
		main_menu_build_game_options(&options, context);
		main_game_change(&options);
	}
	return;
}

void main_menu_launch_force(void)
{
	if (g_main_menu_launch_delay_xlive_ui)
	{
		XNotifyDelayUI(120000);
		main_menu_launch(9);
		g_main_menu_launch_delay_xlive_ui = false;
	}
	else
	{
		main_menu_launch(0);
	}
	return;
}

/* private code */

static s_main_game_globals* main_game_globals_get(void)
{
	return Memory::GetAddress<s_main_game_globals*>(0x46DAE4, 0x49E29C);
}

static void main_game_launch_setup_game_mode_details(void)
{
	switch (g_main_game_launch_options.game_mode)
	{
	case _game_mode_campaign:
	{
		main_game_launch_set_campaign_details();
		break;
	}
	case _game_mode_multiplayer:
	{
		main_game_launch_set_multiplayer_details();
		break;
	}
	case _game_mode_ui_shell:
	{
		main_game_launch_set_ui_shell_details();
		break;
	}
	default:
	{
		error(_error_silent, "%s: unknown game mode %d!", __FUNCTION__, (int32)g_main_game_launch_options.game_mode);
	}
	}

	return;
}

static void main_game_launch_set_campaign_details(void)
{
	// Ensure difficulty is between 0 and 3
	g_main_game_launch_options.difficulty = PIN(g_main_game_launch_options.difficulty, 0, k_campaign_difficulty_levels_count - 1);

	if (!IN_RANGE(g_main_game_launch_user_count, 1, k_number_of_users))
	{
		g_main_game_launch_user_count = 1;
	}
	else if (!g_main_game_launch_options.coop)
	{
		g_main_game_launch_user_count = 1;
	}

	// Removed this so we can campaign games with more than 4 local users
	// 4 players doesn't currently work in most campaign maps
	/*else if (g_main_game_launch_user_count > 2)
	{
		g_main_game_launch_user_count = 2;
	}*/
	return;
}

static void main_game_launch_set_multiplayer_details(void)
{
	// Ensure user count is between 1 and 4
	g_main_game_launch_user_count = PIN(g_main_game_launch_user_count, 1, k_number_of_users);
	return;
}

static void main_game_launch_set_ui_shell_details(void)
{
	g_main_game_launch_user_count = 1;
	g_main_game_launch_options.menu_context = 7;
	return;
}

static void main_game_internal_pregame_load(void)
{
	s_main_game_globals* main_game_globals = main_game_globals_get();
	ASSERT(main_game_globals->game_loaded_status == _game_loaded_status_none);

	texture_cache_open_pregame();
	main_game_globals->game_loaded_status = _game_loaded_status_none;
	return;
}

static void main_game_load_panic(void)
{
	main_game_unload_and_prepare_for_next_game();
	ASSERT(!main_game_loaded_map() && !main_game_loaded_pregame());

	static bool x_recursion_lock = false;
	if (debug_load_panic_to_main_menu)
	{
		error(_error_log, "### ERROR main_game_load_panic: recursion lock triggered (we must have failed to load the main menu from a panic state)");
		error(_error_log, "### ERROR main game load failed, unable to recover, aborting to pregame");
		main_game_internal_pregame_load();
		main_halt_and_display_errors();
	}
	else
	{
		if (x_recursion_lock)
		{
			error(_error_log, "### ERROR main game load failed, unable to recover, aborting to pregame");
			main_game_internal_pregame_load();
			main_halt_and_display_errors();
		}
	}

	x_recursion_lock = true;

	s_game_options options;
	main_menu_build_game_options(&options, 0);
	main_game_change_immediate(&options);

	x_recursion_lock = false;

	ASSERT(!main_game_loaded_map() && !main_game_loaded_pregame());
	return;
}

static void main_menu_build_game_options(s_game_options* options, int32 menu_context)
{
	ASSERT(options);
	
	game_options_new(options);
	options->game_mode = _game_mode_ui_shell;
	ustrncpy_debug(options->scenario_path, L"scenarios\\ui\\mainmenu\\mainmenu", MAX_PATH);

	options->menu_context = menu_context;
	game_options_setup_default_players(1, options);
	return;
}
