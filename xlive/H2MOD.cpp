#include "stdafx.h"
#include "H2MOD.h"

#include "ai/ai.h"
#include "ai/ai_player.h"
#include "cache/cache_files.h"
#include "camera/camera.h"
#include "camera/dead_camera.h"
#include "camera/editor_camera.h"
#include "camera/first_person_camera.h"
#include "camera/following_camera.h"
#include "camera/observer.h"
#include "cartographer/discord/discord_interface.h"
#include "cartographer/tag_fixes/tag_fixes.h"
#include "cutscene/cinematics.h"
#include "effects/contrails.h"
#include "effects/effects.h"
#include "effects/particle_update.h"
#include "physics/character_physics_mode_melee.h"
#include "game/aim_assist.h"
#include "game/cheats.h"
#include "game/game.h"
#include "game/game_globals.h"
#include "game/game_options.h"
#include "game/game_time.h"
#include "game/multiplayer_globals.h"
#include "game/player_control.h"
#include "hs/hs.h"
#include "hs/hs_runtime.h"
#include "input/input_xinput.h"
#include "input/input_windows.h"
#include "input/input_abstraction.h"
#include "interface/hud.h"
#include "interface/hud_messaging.h"
#include "interface/interface.h"
#include "interface/motion_sensor.h"
#include "interface/first_person_weapons.h"
#include "interface/new_hud.h"
#include "interface/new_hud_draw.h"
#include "interface/user_interface.h"
#include "interface/user_interface_controller.h"
#include "interface/user_interface_text.h"
#include "interface/user_interface_utilities.h"
#include "interface/screens/screens_patches.h"
#include "items/weapon_definitions.h"
#include "kablam/kablam.h"
#include "main/levels.h"
#include "main/main.h"
#include "main/main_game.h"
#include "main/main_render.h"
#include "main/main_screenshot.h"
#include "main/main_time.h"
#include "networking/logic/life_cycle_manager.h"
#include "networking/logic/network_life_cycle.h"
#include "networking/session/network_session.h"
#include "networking/transport/transport.h"
#include "networking/network_event.h"
#include "networking/network_globals.h"
#include "networking/network_utilities.h"
#include "networking/network_memory.h"
#include "networking/network_configuration.h"
#include "networking/network_loading.h"
#include "physics/character_physics_mode_ground.h"
#include "units/bipeds.h"
#include "rasterizer/rasterizer_lens_flares.h"
#include "rasterizer/rasterizer_main.h"
#include "rasterizer/rasterizer_settings.h"
#include "rasterizer/dx9/rasterizer_dx9_bitmaps.h"
#include "rasterizer/dx9/rasterizer_dx9_dof.h"
#include "rasterizer/dx9/rasterizer_dx9_fog.h"
#include "rasterizer/dx9/rasterizer_dx9_fullscreen_passes.h"
#include "rasterizer/dx9/rasterizer_dx9_lens_flares.h"
#include "rasterizer/dx9/rasterizer_dx9_main.h"
#include "rasterizer/dx9/rasterizer_dx9_shader_submit_new.h"
#include "rasterizer/dx9/rasterizer_dx9_screen_effect.h"
#include "rasterizer/dx9/rasterizer_dx9_water.h"
#include "rasterizer/dx9/rasterizer_dx9_weather.h"
#include "render/render.h"
#include "render/render_cameras.h"
#include "render/render_submit.h"
#include "render/render_lod_new.h"
#include "render/render_weather.h"
#include "saved_games/cartographer_player_profile/cartographer_player_profile.h"
#include "saved_games/game_state_procs.h"
#include "scenario/scenario.h"
#include "shell/shell.h"
#include "simulation/simulation.h"
#include "simulation/simulation_players.h"
#include "simulation/game_interface/simulation_game_objects.h"
#include "simulation/game_interface/simulation_game_units.h"
#include "tag_files/files_windows.h"
#include "tag_files/tag_loader/tag_injection.h"
#include "text/font_cache.h"
#include "units/units.h"
#include "widgets/cloth.h"
#include "widgets/liquid.h"

#ifdef TERMINAL_ENABLED
#include "H2MOD/GUI/ImGui_Integration/Console/CommandCollection.h"
#endif
#include "H2MOD/Modules/Accounts/AccountLogin.h"
#include "H2MOD/Modules/DirectorHooks/DirectorHooks.h"
#include "H2MOD/Modules/EventHandler/EventHandler.hpp"
#include "H2MOD/Modules/GamePhysics/Patches/ProjectileFix.h"
#include "H2MOD/Modules/Input/KeyboardInput.h"
#include "H2MOD/Modules/MainMenu/MapSlots.h"
#include "H2MOD/Modules/MainMenu/Ranks.h"
#include "H2MOD/Modules/MapManager/MapManager.h"
#include "H2MOD/Modules/OnScreenDebug/OnscreenDebug.h"
#include "H2MOD/Modules/RenderHooks/RenderHooks.h"
#include "H2MOD/Modules/Shell/Config.h"
#include "H2MOD/Modules/SpecialEvents/SpecialEvents.h"
#include "H2MOD/Modules/TagFixes/TagFixes.h"
#include "H2MOD/Variants/VariantSystem.h"
#include "H2MOD/Variants/H2X/H2X.h"
#include "halo_playlist/halo_playlist.h"
#include "interface/multiplayer_variant_settings_interface_definition.h"
#include "items/weapons.h"
#include "physics/physics_constants.h"
#include "units/unit_action_system.h"

/* typedefs */

typedef void(__cdecl* user_interface_controller_set_desired_team_index_t)(e_controller_index controller_index, e_game_team team);
typedef bool(__cdecl* map_cache_load_t)(s_game_options* options);
typedef bool(__cdecl* player_spawn_t)(datum player_index);
typedef uint16(__cdecl* get_enabled_teams_flags_t)(c_network_session*);

/* globals */

static user_interface_controller_set_desired_team_index_t p_user_interface_controller_set_desired_team_index;
static map_cache_load_t p_map_cache_load;
static player_spawn_t p_player_spawn;
static get_enabled_teams_flags_t p_get_enabled_teams_flags;

bool g_h2x_enabled = false;
bool g_xbox_tickrate_enabled = false;

/* prototypes */

static void toggle_xbox_tickrate(s_game_options* options, bool toggle);

static bool __cdecl OnPlayerSpawn(datum player_index);

static void __cdecl OnPlayerDeath(datum player_index);

static bool __cdecl OnMapLoad(s_game_options* options);

static uint16 __cdecl get_enabled_team_flags(c_network_session* session);

static void h2mod_apply_hooks(void);

static void h2mod_apply_tweaks(void);

static void __cdecl user_interface_controller_set_desired_team_index_hook(e_controller_index controller_index, e_game_team team);

static int OnAutoPickUpHandler(datum player_datum, datum object_datum);

static bool GrenadeChainReactIsEngineMPCheck(void);

static bool BansheeBombIsEngineMPCheck(void);

static bool FlashlightIsEngineSPCheck(void);

/* public code  */

bool xbox_tickrate_is_enabled()
{
	return g_xbox_tickrate_enabled;
}

#pragma region PlayerFunctions

void call_give_player_weapon(datum player_index, datum weapon_tag_definition_index, bool resetLoadout)
{
	player_datum const* player = player_get(player_index);

	if (player->unit_index != NONE)
	{
		object_placement_data new_weapon_placement;

		object_placement_data_new(&new_weapon_placement, weapon_tag_definition_index, player->unit_index, 0);

		datum new_weapon_object_index = object_new(&new_weapon_placement);

		if (resetLoadout)
		{
			unit_delete_all_weapons(player->unit_index);
		}

		unit_add_weapon_to_inventory(player->unit_index, new_weapon_object_index, _weapon_addition_method_one);
	}
}

#pragma endregion

void H2MOD::disable_score_announcer_sounds(uint32 sound_flags)
{
	if (sound_flags)
	{
		const datum multiplayerGlobalsTagIndex = tag_loaded(_tag_group_multiplayer_globals, "multiplayer\\multiplayer_globals");
		if (multiplayerGlobalsTagIndex != NONE)
		{
			s_multiplayer_globals_definition* multiplayer_globals = (s_multiplayer_globals_definition*)tag_get_fast(multiplayerGlobalsTagIndex);
			if (multiplayer_globals->runtime.count)
			{
				auto* runtime_tag_block_data = multiplayer_globals->runtime[0];

				if (sound_flags & FLAG(_sound_type_slayer))
				{
					tag_block<s_multiplayer_event_response_definition>* slayer_events = &runtime_tag_block_data->events[_multiplayer_event_response_game_type_slayer];
					slayer_events->count = 0;
					slayer_events->data = 0;
				}

				if (sound_flags & ALL_SOUNDS_NO_SLAYER) // check if there is any point in running the code below
				{
					tag_block<s_multiplayer_event_response_definition>* general_events = &runtime_tag_block_data->events[_multiplayer_event_response_game_type_general];
					for (int i = 0; i < general_events->count; i++)
					{
						s_multiplayer_event_response_definition* general_event = (*general_events)[i];
						const e_multiplayer_event_response_event event = general_event->event;
						if (
							(sound_flags & FLAG(_sound_type_gained_the_lead) && (event == _multiplayer_event_response_general_gained_lead || event == _multiplayer_event_response_general_gained_team_lead))
							|| (sound_flags & FLAG(_sound_type_team_change) && event == _multiplayer_event_response_general_player_changed_team)
							|| (sound_flags & FLAG(_sound_type_lost_the_lead) && (event == _multiplayer_event_response_general_lost_lead))
							|| (sound_flags & FLAG(_sound_type_tied_leader) && (event == _multiplayer_event_response_general_tied_leader || event == _multiplayer_event_response_general_tied_team_leader))
							)
						{
							// disable all sounds from english to chinese
							for (size_t language = 0; language < k_language_count; ++language)
							{
								general_event->primary_sound.sounds[language].index = NONE;
							}
						}
					}
				}
			}
		}
	}
}

void H2MOD::custom_sound_play(const wchar_t* soundName, int delay)
{
	auto playSound = [=]()
	{
		//std::unique_lock<std::mutex> lck(H2MOD::sound_mutex);
		std::chrono::high_resolution_clock::time_point timePoint = std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(delay);

		LOG_TRACE_GAME(L"[H2MOD-SoundQueue] - attempting to play sound {0} - delaying {1} miliseconds first", soundName, delay);

		if (delay > 0)
			std::this_thread::sleep_until(timePoint);

		PlaySound(soundName, NULL, SND_FILENAME | SND_NODEFAULT);
	};

	if (!shell_is_dedicated_server())
		std::thread(playSound).detach();
}

static const real32 seconds_trigger_hold = 1.0f / 30.0f; // 0.033333333 seconds takes 2 60hz seconds

void H2MOD::player_position_increase_client_position_margin_of_error(bool enable)
{
	if (shell_is_dedicated_server())
		return;

	const real32 k_default_biped_distance_error_margin = 2.5f;
	const real32 k_default_vehicle_distance_error_margin = 7.5f;

	real32 biped_error_margin = !enable ? k_default_biped_distance_error_margin : 4.0f;
	real32 vehicle_error_margin = !enable ? k_default_vehicle_distance_error_margin : 10.0f;
	*Memory::GetAddress<real32*>(0x4F958C) = biped_error_margin;
	*Memory::GetAddress<real32*>(0x4F9594) = vehicle_error_margin;
}

void H2MOD::Initialize()
{
	event(_event_status, "h2mod: Initializing {}", DLL_VERSION_STR);
	event(_event_status, "h2mod: Image base address: 0x{:X}", Memory::baseAddress);

	DETOUR_BEGIN();

	h2mod_apply_tweaks();

	// Apply patches
	game_apply_pre_winmain_patches();

#ifdef TERMINAL_ENABLED
	CommandCollection::InitializeCommands();
#endif

	if (!shell_is_dedicated_server())
	{
		// Apply patches for the hud that need to be applied before WinMain is called
		hud_apply_pre_winmain_patches();

		// adds support for more monitor resolutions
		rasterizer_settings_apply_hooks();

		KeyboardInput::Initialize();
		
		RenderHooks::Initialize();
		DirectorHooks::Initialize();
		TEST_N_DEF(PC3);
	}
	else
	{
		kablam_apply_patches();
	}

	cartographer_player_profile_initialize();
	tag_injection_initialize();
	CustomVariantHandler::RegisterCustomVariants();
	MapSlots::Initialize();
	ProjectileFix::ApplyPatches();
	H2X::ApplyPatches();
	h2mod_apply_hooks();

	DETOUR_COMMIT();

	event(_event_status, "h2mod: Initialized");

	TEST_N_DEF(PC6);

	return;
}

/* private code */

static void toggle_xbox_tickrate(s_game_options* options, bool toggle)
{
	options->game_tick_rate = toggle ? 30 : 60;
	WriteValue<int32>(Memory::GetAddress(0x264ABB, 0x1DB8B) + 1, (int32)options->game_tick_rate);
	LOG_TRACE_GAME("[h2mod] set game options tickrate to {}", options->game_tick_rate);
	return;
}

static void __cdecl OnPlayerDeath(datum player_index)
{
	CustomVariantHandler::OnPlayerDeath(ExecTime::_preEventExec, player_index);
	INVOKE(0x5587B, 0x5DD73, OnPlayerDeath, player_index);
	CustomVariantHandler::OnPlayerDeath(ExecTime::_postEventExec, player_index);
	return;
}

static bool __cdecl OnPlayerSpawn(datum player_index)
{
	EventHandler::PlayerSpawnEventExecute(EventExecutionType::execute_before, player_index);
	CustomVariantHandler::OnPlayerSpawn(ExecTime::_preEventExec, player_index);

	bool ret = p_player_spawn(player_index);

	s_game_variant* variant = get_game_variant();

	if (game_is_multiplayer() && variant && variant->cartographer_settings.flags.test(_cartographer_variant_invincible_players))
	{
		player_datum* player = player_get(player_index);
		object_datum* object = object_get(player->unit_index);
		
		ASSERT(player);
		ASSERT(object);

		object->object.object_damage_flags.set(_object_is_immune_to_damage, true);
	}

	// check if the spawn was successful
	if (ret)
	{
		EventHandler::PlayerSpawnEventExecute(EventExecutionType::execute_after, player_index);
		CustomVariantHandler::OnPlayerSpawn(ExecTime::_postEventExec, player_index);
	}

	return ret;
}

static bool __cdecl OnMapLoad(s_game_options* options)
{
	static bool resetAfterMatch = false;

	EventHandler::MapLoadEventExecute(EventExecutionType::execute_before, options->game_mode);
	CustomVariantHandler::OnMapLoad(ExecTime::_preEventExec, options->game_mode);

	bool result = p_map_cache_load(options);
	if (result == false) // verify if the game didn't fail to load the map
		return false;

	TagFixes::OnMapLoad();

	if (options->game_mode == _game_mode_multiplayer)
	{
		game_globals_apply_tag_patches();
	}

	cartographer_player_profile_weapon_offsets_on_map_load();

	// when the game is minimized, the game might skip loading the main menu
	// this is where resetAfterMatch var comes in for help
	if (resetAfterMatch)
	{
		// here deinitialize the custom variant
		CustomVariantHandler::DisposeGameVariant();
		resetAfterMatch = false;
	}

	// reset everything
	toggle_xbox_tickrate(options, false);

	// reset custom gametypes state
	g_h2x_enabled = false;
	g_xbox_tickrate_enabled = false;

	bool game_mode_ui_shell = options->game_mode == _game_mode_ui_shell;

	if (game_mode_ui_shell)
	{
		addDebugText("Engine type: Main-Menu");
		if (!shell_is_dedicated_server())
		{
			MapSlots::OnMapLoad();
			UIRankPatch();
			screens_apply_patches_on_map_load();
			main_tag_fixes();
		}
	}
	else
	{
		event(_event_status, "h2mod: engine type: %d", options->game_mode);

		if (!shell_is_dedicated_server())
		{
			hud_patches_on_map_load();
			screens_apply_patches_on_mp_map_load();
			main_tag_fixes();
			hud_draw_on_map_load();
		}

		if (options->game_mode == _game_mode_multiplayer)
		{
			const wchar_t* variant_name = NetworkSession::GetGameVariantName();
			event(_event_status, "h2mod: game variant name: %ws", variant_name);

			addDebugText("Engine type: Multiplayer");
			load_special_event();

			// TODO: depreciate variant name shennanagins after Kantanomo finishes custom variant settings
			if (StrStrIW(variant_name, L"h2x"))
			{
				event(_event_status, "h2mod: H2X turned on!");
				g_h2x_enabled = true;
			}
			else if (StrStrIW(variant_name, L"ogh2"))
			{
				event(_event_status, "h2mod: OGH2 turned on!");
				g_xbox_tickrate_enabled = true;
			}

			if (options->game_variant.cartographer_settings.flags.test(_cartographer_variant_engine_mode))
			{
				event(_event_status, "h2mod: 30 tick engine mode enabled");
				g_xbox_tickrate_enabled = true;
			}

			toggle_xbox_tickrate(options, g_xbox_tickrate_enabled);
			if (!g_xbox_tickrate_enabled)
			{
				H2X::ApplyMapLoadPatches(g_h2x_enabled);
				ProjectileFix::ApplyProjectileVelocity();
			}

			if (network_life_cycle_get_state() == _life_cycle_state_in_game)
			{
				// here initialize custom variant
				// in case it is found
				CustomVariantHandler::GameVarianEnable(variant_name);
			}

			multiplayer_variant_settings_interface_on_map_load();
		}
		else if (options->game_mode == _game_mode_campaign)
		{
			//if anyone wants to run code on map load single player
			addDebugText("Engine type: Singleplayer");
			toggle_xbox_tickrate(options, true);
			if (H2Config_discord_enable)
			{
				discord_interface_set_difficulty(options->difficulty);
			}
		}

		resetAfterMatch = true;
	}


	EventHandler::MapLoadEventExecute(EventExecutionType::execute_after, options->game_mode);
	CustomVariantHandler::OnMapLoad(ExecTime::_postEventExec, options->game_mode);

	// Clear remaining handle open in the tag injector at the end of post loading injections
	tag_injection_clear_active_map();

	return result;
}

static uint16 __cdecl get_enabled_team_flags(c_network_session* session)
{
	uint16 default_teams_enabled_flags = p_get_enabled_teams_flags(session);
	uint16 new_teams_enabled_flags = (default_teams_enabled_flags & H2Config_team_bit_flags);
	const uint16 red_versus_blue_teams = FLAG(_game_team_red) | FLAG(_game_team_blue);
	const uint16 infection_teams = FLAG(_game_team_red) | FLAG(_game_team_green);

	std::wstring selected_map_file_name;

	// skip if we're not host, let the host control
	if (!session->is_host())
		return default_teams_enabled_flags;

	if (CustomVariantHandler::ContainsGameVariant(session->get_game_variant_name(), _id_infection))
	{
		// infection overrides H2Config
		// TODO get infection_teams through the interface
		new_teams_enabled_flags = infection_teams;
		if ((default_teams_enabled_flags & FLAG(_game_team_red)) == 0
			|| (default_teams_enabled_flags & FLAG(_game_team_green)) == 0)
		{
			error(_error_log, " - infection teams disabled in default enabled team flags");
			if (MapManager::GetMapFilename(selected_map_file_name))
				error(_error_log, " - perhaps current selected map - %ws doesn't support these teams?? overriding anyway", selected_map_file_name.c_str());
		}
	}
	else if (StrStrIW(session->get_game_variant_name(), L"rvb") != NULL)
	{
		// same with rvb, overrides H2Config
		new_teams_enabled_flags = red_versus_blue_teams;
		if ((default_teams_enabled_flags & FLAG(_game_team_red)) == 0
			|| (default_teams_enabled_flags & FLAG(_game_team_blue)) == 0)
		{
			error(_error_log, " - RvB teams disabled in default enabled team flags");
			if (MapManager::GetMapFilename(selected_map_file_name))
				error(_error_log, " - perhaps current selected map - %ws doesn't support these teams??", selected_map_file_name.c_str());
		}
	}

	return new_teams_enabled_flags;
}

__declspec(naked) static void object_function_value_adjust_primary_firing(void)
{
	__asm
	{
		// eax holds game_time_get()
		sub     eax, [ebx + 248h]

		push esi
		push eax
		// adjust the value first
		fld seconds_trigger_hold
		push eax
		fstp dword ptr[esp]
		call game_seconds_to_ticks_real
		fstp dword ptr[esp]
		cvttss2si esi, [esp]
		add esp, 4
		pop eax

		// WRONG
		// cmp eax, 1
		// RIGHT, compare with adjusted to tickrate
		cmp eax, esi

		pop esi

		retn
	}
}

static void h2mod_apply_hooks(void)
{
	event(_event_status, "h2mod: applying hooks");

	MapManager::ApplyPatches();

	/* Labeled "AutoPickup" handler may be proximity to vehicles and such as well */
	PatchCall(Memory::GetAddress(0x58789, 0x60C81), OnAutoPickUpHandler);

	// disable part of custom map tag verification
	NopFill(Memory::GetAddress(0x4FA0A, 0x56C0A), 6);

	ai_apply_patches();
	ai_player_apply_patches();

	cheats_apply_patches();
	main_apply_patches();
	main_time_apply_patches();
	game_statborg_apply_patches();
	simulation_game_objects_apply_patches();
	simulation_game_units_apply_patches();
	players_apply_patches();
	objects_apply_patches();

	particle_update_apply_patches();

	c_character_physics_mode_melee_datum::apply_hooks();
	character_physics_mode_ground_apply_patches();
	
	weapon_definitions_apply_patches();
	observer_apply_patches();

	network_globals_apply_patches();
	network_transport_apply_patches();
	network_session_apply_patches();
	network_session_interface_patches();
	bitstream_serialization_apply_patches();
	game_life_cycle_apply_patches();

	network_memory_apply_patches();
	game_engine_apply_patches();
	simulation_apply_patches();
	simulation_players_apply_patches();

	cache_files_apply_patches();
	network_configuration_apply_patches();
	network_utilities_apply_patches();
	damage_apply_patches();
	files_windows_apply_patches();

	// server/client detours 
	DETOUR_ATTACH(p_player_spawn, Memory::GetAddress<player_spawn_t>(0x55952, 0x5DE4A), OnPlayerSpawn);
	PatchCall(Memory::GetAddress(0x144919, 0x133769), OnPlayerDeath);
	DETOUR_ATTACH(p_map_cache_load, Memory::GetAddress<map_cache_load_t>(0x8F62, 0x1F35C), OnMapLoad);
	DETOUR_ATTACH(p_get_enabled_teams_flags, Memory::GetAddress<get_enabled_teams_flags_t>(0x1B087B, 0x19698B), get_enabled_team_flags);

	// possible fix for a network update race condition
	// where the loading network thread (the one started and running during map load)
	// would race with the newtork update in the function that waits for the loading network thread to finish just 1000 milliseconds
	// instead of waiting it to process everything then exit
	// also call simulation_update() when loading the map
	WriteValue<int32>(Memory::GetAddress(0x1AEA75, 0x1AD61C) + 1, INFINITE);
	network_loading_apply_patches();

	network_game_definitions_apply_patches();
	physics_constants_apply_patches();
	game_time_apply_patches();
	unit_action_system_apply_patches();
	weapons_apply_patches();

	// sound fix for hunter's weapons (assault cannon)
	// might be used for other game systems
	Codecave(Memory::GetAddress(0x15E8DC, 0x142B9C), object_function_value_adjust_primary_firing, 4);

	// below hooks applied to specific executables
	if (!shell_is_dedicated_server())
	{
		/* These hooks are only built for the client, don't enable them on the server! */

		event(_event_status, "h2mod: applying client hooks");

		DETOUR_ATTACH(p_user_interface_controller_set_desired_team_index, Memory::GetAddress<user_interface_controller_set_desired_team_index_t>(0x2068F2), user_interface_controller_set_desired_team_index_hook);

		PatchCall(Memory::GetAddress(0x182d6d), GrenadeChainReactIsEngineMPCheck);
		PatchCall(Memory::GetAddress(0x92C05), BansheeBombIsEngineMPCheck);
		PatchCall(Memory::GetAddress(0x13ff75), FlashlightIsEngineSPCheck);

		hs_apply_patches();
		hs_runtime_apply_patches();

		new_hud_apply_patches();
		motion_sensor_apply_patches();
		render_cameras_apply_patches();
		first_person_camera_apply_patches();
		first_person_weapons_apply_patches();
		following_camera_apply_patches();
		editor_camera_apply_patches();
		bipeds_apply_patches();
		unit_apply_patches();

		user_interface_text_apply_hooks();
		user_interface_controller_apply_patches();
		hud_messaging_apply_hooks();
		font_group_apply_hooks();
		screens_apply_patches();
		aim_assist_apply_patches();

		levels_apply_patches();
		main_game_apply_patches();
		main_render_apply_patches();
		main_screenshot_apply_patches();

		rasterizer_lens_flares_apply_patches();
		rasterizer_main_apply_patches();

		rasterizer_dx9_bitmaps_apply_patches();
		rasterizer_dx9_dof_apply_patches();
		rasterizer_dx9_fog_apply_patches();
		rasterizer_dx9_fullscreen_passes_apply_patches();
		rasterizer_dx9_lens_flares_apply_patches();
		rasterizer_dx9_main_apply_patches();
		rasterizer_dx9_screen_effect_apply_patches();
		rasterizer_dx9_shader_submit_new_apply_patches();
		rasterizer_dx9_targets_apply_patches();
		rasterizer_dx9_water_apply_patches();
		rasterizer_dx9_weather_apply_patches();

		render_lod_new_apply_patches();
		render_submit_apply_patches();
		render_weather_apply_patches();

		cinematics_apply_patches();
		game_state_procs_apply_patches();
		apply_dead_camera_patches();
		liquid_apply_patches();
		contrails_apply_patches();
		cloth_apply_patches();
		camera_apply_patches();
		player_control_apply_patches();
		effects_apply_patches();
		xinput_apply_patches();
		input_windows_apply_patches();
		input_abstraction_patches_apply();
		render_apply_patches();
		apply_interface_hooks();
		new_hud_draw_apply_patches();
		user_interface_utilities_apply_patches();
		scenario_apply_patches();

		
	}

	if (shell_is_dedicated_server())
	{
		halo_playlist_apply_patches();
	}
	return;
}

static void h2mod_apply_tweaks(void)
{
	if (shell_is_dedicated_server())
	{

	}
	else
	{
		bool intro_high_quality_flag = true;//clients should set on halo2.exe -highquality

		if (!H2Config_skip_intro && intro_high_quality_flag) {
			BYTE assmIntroHQ[] = { 0xEB };
			WriteBytes(Memory::GetAddress(0x221C29), assmIntroHQ, sizeof(assmIntroHQ));
		}

		// prevent game from setting timeBeginPeriod/timeEndPeriod, when rendering loading screen
		NopFill(Memory::GetAddressRelative(0x66BA7C), 8);
		NopFill(Memory::GetAddressRelative(0x66A092), 8);

		// disable symbol to emoji translation when dealing with player name
		// works only in game for now, because the name in the pregame lobby uses c_text_widget
		// and it's harder to deal with
		NopFill(Memory::GetAddressRelative(0x46C7C7), 5);
		NopFill(Memory::GetAddressRelative(0x45C338), 5);
		NopFill(Memory::GetAddressRelative(0x473C61), 5);
	}

	return;
}

static void __cdecl user_interface_controller_set_desired_team_index_hook(e_controller_index controller_index, e_game_team team)
{
	c_network_session* session;
	if (network_life_cycle_in_squad_session(&session))
	{
		// prevent team switch in the pregame lobby, when the game already started
		if (session->session_mode() != _network_session_mode_in_game
			|| network_life_cycle_get_state() != _life_cycle_state_pre_game)
		{
			p_user_interface_controller_set_desired_team_index(controller_index, team);
		}
	}

	return;
}

static int OnAutoPickUpHandler(datum player_datum, datum object_datum)
{
	auto p_auto_handle = Memory::GetAddress<int(_cdecl*)(datum, datum)>(0x57AA5, 0x5FF9D);

	int result = 0;

	bool handled = CustomVariantHandler::OnAutoPickupHandler(ExecTime::_preEventExec, player_datum, object_datum);

	if (!handled)
		result = p_auto_handle(player_datum, object_datum);

	CustomVariantHandler::OnAutoPickupHandler(ExecTime::_postEventExec, player_datum, object_datum);

	return result;
}

static bool GrenadeChainReactIsEngineMPCheck(void)
{
	return game_is_multiplayer();
}

static bool BansheeBombIsEngineMPCheck(void)
{
	return game_is_multiplayer();
}

static bool FlashlightIsEngineSPCheck(void)
{
	return game_is_campaign();
}
