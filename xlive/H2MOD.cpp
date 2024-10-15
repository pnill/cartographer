#include "stdafx.h"
#include "H2MOD.h"

#include "camera/dead_camera.h"
#include "camera/observer.h"
#include "cartographer/discord/discord_interface.h"
#include "cartographer/tag_fixes/tag_fixes.h"
#include "cutscene/cinematics.h"
#include "effects/contrails.h"
#include "effects/effects.h"
#include "effects/particle_update.h"
#include "game/aim_assist.h"
#include "game/cheats.h"
#include "game/game.h"
#include "game/game_globals.h"
#include "game/game_time.h"
#include "game/player_control.h"
#include "game/player_vibration.h"
#include "input/input_xinput.h"
#include "input/input_windows.h"
#include "input/input_abstraction.h"
#include "interface/hud.h"
#include "interface/hud_messaging.h"
#include "interface/interface.h"
#include "interface/motion_sensor.h"
#include "interface/first_person_weapons.h"
#include "interface/new_hud.h"
#include "interface/user_interface_text.h"
#include "interface/user_interface_controller.h"
#include "interface/screens/screens_patches.h"
#include "items/weapon_definitions.h"
#include "main/levels.h"
#include "main/loading.h"
#include "main/main_game.h"
#include "main/main_render.h"
#include "networking/memory/networking_memory.h"
#include "networking/network_configuration.h"
#include "networking/Transport/transport.h"
#include "units/bipeds.h"
#include "rasterizer/rasterizer_lens_flares.h"
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
#include "saved_games/game_state_procs.h"
#include "simulation/simulation.h"
#include "simulation/simulation_players.h"
#include "simulation/game_interface/simulation_game_objects.h"
#include "simulation/game_interface/simulation_game_units.h"
#include "tag_files/tag_loader/tag_injection.h"
#include "text/font_cache.h"
#include "units/units.h"
#include "widgets/cloth.h"
#include "widgets/liquid.h"

#include "Blam/Cache/TagGroups/multiplayer_globals_definition.hpp"
#include "H2MOD/EngineHooks/EngineHooks.h"
#include "H2MOD/GUI/ImGui_Integration/Console/CommandCollection.h"
#include "H2MOD/GUI/ImGui_Integration/ImGui_Handler.h"
#include "H2MOD/Modules/CustomVariantSettings/CustomVariantSettings.h"
#include "H2MOD/Modules/DirectorHooks/DirectorHooks.h"
#include "H2MOD/Modules/EventHandler/EventHandler.hpp"
#include "H2MOD/Modules/GamePhysics/Patches/MeleeFix.h"
#include "H2MOD/Modules/GamePhysics/Patches/ProjectileFix.h"
#include "H2MOD/Modules/HaloScript/HaloScript.h"
#include "H2MOD/Modules/Input/KeyboardInput.h"
#include "H2MOD/Modules/KantTesting/KantTesting.h"
#include "H2MOD/Modules/MainMenu/MapSlots.h"
#include "H2MOD/Modules/MainMenu/Ranks.h"
#include "H2MOD/Modules/MapManager/MapManager.h"
#ifndef NDEBUG
#include "H2MOD/Modules/ObserverMode/ObserverMode.h"
#endif
#include "camera/first_person_camera.h"
#include "H2MOD/Modules/OnScreenDebug/OnscreenDebug.h"
#include "H2MOD/Modules/PlaylistLoader/PlaylistLoader.h"
#include "H2MOD/Modules/RenderHooks/RenderHooks.h"
#include "H2MOD/Modules/Shell/Config.h"
#include "H2MOD/Modules/SpecialEvents/SpecialEvents.h"
#include "H2MOD/Modules/TagFixes/TagFixes.h"
#include "H2MOD/Tags/MetaExtender.h"
#include "H2MOD/Variants/VariantSystem.h"
#include "H2MOD/Variants/H2X/H2X.h"
#include "interface/new_hud_draw.h"
#include "saved_games/cartographer_player_profile.h"

std::unique_ptr<H2MOD> h2mod(std::make_unique<H2MOD>());

bool H2XFirerateEnabled = false;
bool g_xbox_tickrate_enabled = false;

bool xbox_tickrate_is_enabled()
{
	return g_xbox_tickrate_enabled;
}

std::unordered_map<const wchar_t*, bool&> GametypesMap
{
	{ L"h2x", H2XFirerateEnabled },
	{ L"ogh2", g_xbox_tickrate_enabled },
};

#pragma region engine calls

// Used to get damage on any object
typedef void(__cdecl* object_cause_damage_t)(s_damage_data* damage_data, int damaged_object_indexes, __int16 a4, __int16 a5, __int16 a6, int a7);
object_cause_damage_t p_object_cause_damage;

// Engine call to set damage applied on an object by a projectile
void __cdecl projectile_collision_object_cause_damage(s_damage_data* damage_data, int damaged_object_indexes, __int16 a4, __int16 a5, __int16 a6, int a7)
{
	// Hook on call to prevent guardian glitching on Infection gametype
	if (CustomVariantHandler::VariantEnabled(_id_infection)) {
		if (damage_data->creator_datum != NONE && damage_data->field_10 != NONE)
		{
			LOG_TRACE_GAME(
				"{} {} {:X} {:X} {:X} {:X} {:X} {:X} {:X} {:X}",
				__FUNCTION__,
				damage_data->flags,
				damage_data->damage_tag_index,
				damage_data->creator_datum,
				damage_data->field_10, //TODO reverse what field_10 is
				damage_data->field_14,
				damage_data->field_18,
				damage_data->field_1C,
				damage_data->field_24,
				damage_data->field_28
			);
			p_object_cause_damage(damage_data, damaged_object_indexes, a4, a5, a6, a7);
		}
		else
		{
			LOG_TRACE_GAME("GUARDIAN GLITCH PREVENTED");
		}
	}
	else
	{
		//Calls basic engine function when not in zombies game
		p_object_cause_damage(damage_data, damaged_object_indexes, a4, a5, a6, a7);
	}
}
#pragma endregion

typedef int(__cdecl* show_error_screen_t)(int a1, int a2, int a3, __int16 a4, int a5, int a6);
show_error_screen_t p_show_error_screen;

int __cdecl showErrorScreen(int a1, int widget_type, int a3, __int16 a4, int a5, int a6) {
	if (widget_type == 0x117)
	{
		LOG_TRACE_FUNC("Ignoring need to reinstall maps");
		return 0;
	}
	return p_show_error_screen(a1, widget_type, a3, a4, a5, a6);
}

typedef int(__cdecl* wcsncpy_s_hook_t)(int a1, unsigned int a2, wchar_t* a3, int a4);
wcsncpy_s_hook_t p_wcsncpy_s_hook;

//lets you follow the call path of any string that is displayed (in a debugger)
int __cdecl stringDisplayHook(int a1, unsigned int a2, wchar_t* a3, int a4) {
	return p_wcsncpy_s_hook(a1, a2, a3, a4);
}

#pragma region PlayerFunctions

void call_give_player_weapon(int playerIndex, datum weaponId, bool resetLoadout)
{
	//LOG_TRACE_GAME("GivePlayerWeapon(PlayerIndex: %08X, WeaponId: %08X)", PlayerIndex, WeaponId);

	datum unit_datum = s_player::get_unit_index(playerIndex);
	if (unit_datum != NONE)
	{
		object_placement_data nObject;

		object_placement_data_new(&nObject, weaponId, unit_datum, 0);

		datum object_idx = object_new(&nObject);

		if (resetLoadout)
			unit_delete_all_weapons(unit_datum);

		unit_add_weapon_to_inventory(unit_datum, object_idx, _weapon_addition_method_one);
	}
}

int H2MOD::get_player_index_from_unit_datum_index(datum unit_datum_index)
{
	player_iterator playersIt;
	while (playersIt.get_next_active_player())
	{
		datum unit_datum_index_check = playersIt.get_current_player_data()->unit_index;
		LOG_TRACE_FUNC("Checking datum: {0:x} - index: {1} against datum: {2:x}", unit_datum_index_check, playersIt.get_current_player_index(), unit_datum_index);

		if (unit_datum_index == unit_datum_index_check)
			return playersIt.get_current_player_index();
	}
	return NONE;
}

void H2MOD::set_unit_speed_patch(bool hackit) {
	static BYTE oldBytes[8];
	static bool oldBytesRead = false;
	DWORD address = Memory::GetAddress(0x6AB7F, 0x6A3BA);

	if (oldBytesRead == false)
	{
		ReadBytesProtected(address, oldBytes, sizeof(oldBytes));
		oldBytesRead = true;
	}

	if (hackit)
	{
		NopFill(address, sizeof(oldBytes));
	}
	else
	{
		WriteBytes(address, oldBytes, sizeof(oldBytes));
	}
}

#pragma endregion

void H2MOD::disable_score_announcer_sounds(int sound_flags)
{
	if (sound_flags)
	{
		datum multiplayerGlobalsTagIndex = tag_loaded(_tag_group_multiplayer_globals, "multiplayer\\multiplayer_globals");

		if (multiplayerGlobalsTagIndex != NONE)
		{
			s_multiplayer_globals_group_definition* multiplayerGlobalsTag = (s_multiplayer_globals_group_definition*)tag_get_fast(multiplayerGlobalsTagIndex);

			if (multiplayerGlobalsTag->runtime.count)
			{
				auto* runtime_tag_block_data = multiplayerGlobalsTag->runtime[0];

				if (sound_flags & FLAG(_sound_type_slayer))
				{
					runtime_tag_block_data->slayer_events.count = 0;
					runtime_tag_block_data->slayer_events.data = 0;
				}

				if (sound_flags & ALL_SOUNDS_NO_SLAYER) // check if there is any point in running the code below
				{
					for (int i = 0; i < runtime_tag_block_data->general_events.count; i++)
					{
						using sound_events = s_multiplayer_globals_group_definition::s_runtime_block::s_general_events_block::e_event;
						auto* general_event = runtime_tag_block_data->general_events[i];
						auto event = general_event->event;
						if (
							(sound_flags & FLAG(_sound_type_gained_the_lead) && (event == sound_events::gained_lead || event == sound_events::gained_team_lead))
							|| (sound_flags & FLAG(_sound_type_team_change) && event == sound_events::player_changed_team)
							|| (sound_flags & FLAG(_sound_type_lost_the_lead) && (event == sound_events::lost_lead))
							|| (sound_flags & FLAG(_sound_type_tied_leader) && (event == sound_events::tied_leader || event == sound_events::tied_team_leader))
							)
						{
							// disable all sounds from english to chinese
							for (int j = 0; j < 8; j++)
							{
								(&general_event->sound)[j].index = NONE;
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
		//std::unique_lock<std::mutex> lck(h2mod->sound_mutex);
		std::chrono::high_resolution_clock::time_point timePoint = std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(delay);

		LOG_TRACE_GAME(L"[H2MOD-SoundQueue] - attempting to play sound {0} - delaying {1} miliseconds first", soundName, delay);

		if (delay > 0)
			std::this_thread::sleep_until(timePoint);

		PlaySound(soundName, NULL, SND_FILENAME | SND_NODEFAULT);
	};

	if (!Memory::IsDedicatedServer())
		std::thread(playSound).detach();
}

typedef void(__cdecl* player_died_t)(datum player_index);
player_died_t p_player_died;

void __cdecl OnPlayerDeath(datum playerIdx)
{
	CustomVariantHandler::OnPlayerDeath(ExecTime::_preEventExec, playerIdx);
	p_player_died(playerIdx);
	CustomVariantHandler::OnPlayerDeath(ExecTime::_postEventExec, playerIdx);
}

/* This is technically closer to object death than player-death as it impacts anything with health at all. */
typedef void(__cdecl* object_deplete_body_internal_t)(datum unit_datum_index, int a2, bool a3, bool a4);
object_deplete_body_internal_t p_object_deplete_body_internal;

void __cdecl OnObjectDamage(datum unit_datum_index, int a2, bool a3, bool a4)
{
	//LOG_TRACE_GAME("OnPlayerDeath(unit_datum_index: %08X, a2: %08X, a3: %08X, a4: %08X)", unit_datum_index,a2,a3,a4);

	/* The first value within a2 ( *(DWORD*)(a2) ) appears to be the datum_index of a player from the gamestate_player_table */

	EventHandler::ObjectDamageEventExecute(EventExecutionType::execute_before, unit_datum_index, *(datum*)(a2));
	CustomVariantHandler::OnObjectDamage(ExecTime::_preEventExec, unit_datum_index, a2, a3, a4);

	p_object_deplete_body_internal(unit_datum_index, a2, a3, a4);

	CustomVariantHandler::OnObjectDamage(ExecTime::_postEventExec, unit_datum_index, a2, a3, a4);
	EventHandler::ObjectDamageEventExecute(EventExecutionType::execute_after, unit_datum_index, *(datum*)(a2));
}

// Client Sided Patch
// TODO: refactor this and rewrite the player_examine_nearby_weapons function
// to check if the player_index is a local user and do a comparison to a flags check if they are able to pickup weapons
void H2MOD::disable_weapon_pickup(bool enable)
{
	static BYTE oldBytes[5];
	static BYTE oldBytesRead = false;
	DWORD address = Memory::GetAddress(0x55EFA);

	if (oldBytesRead == false)
	{
		ReadBytesProtected(address, oldBytes, sizeof(oldBytes));
		oldBytesRead = true;
	}

	if (enable)
	{
		WriteBytes(address, oldBytes, sizeof(oldBytes));
	}
	else
	{
		NopFill(address, sizeof(oldBytes));
	}
}

int OnAutoPickUpHandler(datum player_datum, datum object_datum)
{
	auto p_auto_handle = Memory::GetAddress<int(_cdecl*)(datum, datum)>(0x57AA5, 0x5FF9D);

	int result = 0;

	bool handled = CustomVariantHandler::OnAutoPickupHandler(ExecTime::_preEventExec, player_datum, object_datum);

	if (!handled)
		result = p_auto_handle(player_datum, object_datum);

	CustomVariantHandler::OnAutoPickupHandler(ExecTime::_postEventExec, player_datum, object_datum);

	return result;
}

void toggle_xbox_tickrate(s_game_options* options, bool toggle)
{
	options->game_tick_rate = toggle ? 30 : 60;
	WriteValue<int32>(Memory::GetAddress(0x264ABB, 0x1DB8B) + 1, (int32)options->game_tick_rate);
	LOG_TRACE_GAME("[h2mod] set game options tickrate to {}", options->game_tick_rate);
}

void H2MOD::toggle_ai_multiplayer(bool toggle)
{
	WriteValue<BYTE>(Memory::GetAddress(0x30E684, 0x2B93F4), toggle ? JMP_OP_CODE : JNZ_OP_CODE);
}

typedef bool(__cdecl *map_cache_load_t)(s_game_options* map_load_settings);
map_cache_load_t p_map_cache_load;

bool __cdecl OnMapLoad(s_game_options* options)
{
	static bool resetAfterMatch = false;

	// set the light suppressor flag to false
	if (H2Config_light_suppressor)
	{
		WriteValue(Memory::GetAddress(0x41F6B1), 0);
	}

	EventHandler::MapLoadEventExecute(EventExecutionType::execute_before, options->game_mode);
	CustomVariantHandler::OnMapLoad(ExecTime::_preEventExec, options);

	bool result = p_map_cache_load(options);
	if (result == false) // verify if the game didn't fail to load the map
		return false;


	MetaExtender::free_tag_blocks();
	TagFixes::OnMapLoad();
	
	game_globals_apply_tag_patches(options);
	ImGuiHandler::WeaponOffsets::MapLoad();

	// when the game is minimized, the game might skip loading the main menu
	// this is where resetAfterMatch var comes in for help
	if (resetAfterMatch)
	{
		// here deinitialize the custom variant
		CustomVariantHandler::DisposeGameVariant();
		resetAfterMatch = false;
	}

	// reset everything
	h2mod->toggle_ai_multiplayer(false);
	toggle_xbox_tickrate(options, false);

	// reset custom gametypes state
	for (auto& gametype_it : GametypesMap)
		gametype_it.second = false;

	bool game_mode_ui_shell = options->game_mode == _game_mode_ui_shell;

	if (game_mode_ui_shell)
	{
		addDebugText("Engine type: Main-Menu");
		if (!Memory::IsDedicatedServer())
		{
			MapSlots::OnMapLoad();
			UIRankPatch();
			screens_apply_patches_on_map_load();
			main_tag_fixes();
		}

	}
	else
	{
		wchar_t* variant_name = NetworkSession::GetGameVariantName();
		LOG_INFO_GAME(L"[h2mod] engine type: {}, game variant name: {}", (int)options->game_mode, variant_name);

		if (!Memory::IsDedicatedServer())
		{
			hud_patches_on_map_load();
			main_tag_fixes();
		}

		if (options->game_mode == _game_mode_multiplayer)
		{
			addDebugText("Engine type: Multiplayer");
			load_special_event();

			for (const auto& gametype_it : GametypesMap)
			{
				if (StrStrIW(variant_name, gametype_it.first)) {
					LOG_INFO_GAME(L"{} - {} custom gametype turned on!", __FUNCTIONW__, gametype_it.first);
					gametype_it.second = true;
					break;
				}
			}

			toggle_xbox_tickrate(options, g_xbox_tickrate_enabled);
			if (!g_xbox_tickrate_enabled)
			{
				H2X::ApplyMapLoadPatches(H2XFirerateEnabled);
				ProjectileFix::ApplyProjectileVelocity();
			}

			h2mod->toggle_ai_multiplayer(true);
			if (get_game_life_cycle() == _life_cycle_in_game)
			{
				// send server map checksums to client
				//MapChecksumSync::SendState();

				// here initialize custom variant
				// in case it is found
				CustomVariantHandler::GameVarianEnable(variant_name);
			}
		}
		else if (options->game_mode == _game_mode_campaign)
		{
			//if anyone wants to run code on map load single player
			addDebugText("Engine type: Singleplayer");
			toggle_xbox_tickrate(options, true);
			if (H2Config_discord_enable)
			{
				c_static_wchar_string<260> scenario_path(game_options_get()->scenario_path);
				int32 index = scenario_path.last_index_of(L"\\");
				const wchar_t* scenario_name_wide = &scenario_path.get_string()[index + 1];
				utf8 scenario_name[MAX_PATH];
				wchar_string_to_utf8_string(scenario_name_wide, scenario_name, sizeof(scenario_name));

				context_update_map_info_campaign(options->map_id, scenario_name);
				discord_interface_set_difficulty(options->difficulty);
			}
		}

		resetAfterMatch = true;
	}
	EventHandler::MapLoadEventExecute(EventExecutionType::execute_after, options->game_mode);
	CustomVariantHandler::OnMapLoad(ExecTime::_postEventExec, options);
	return result;
}

typedef bool(__cdecl* player_spawn_t)(datum playerDatumIndex);
player_spawn_t p_player_spawn;

bool __cdecl OnPlayerSpawn(datum playerDatumIdx)
{
	//LOG_TRACE_GAME("OnPlayerSpawn(a1: %08X)", a1);
	
	EventHandler::PlayerSpawnEventExecute(EventExecutionType::execute_before, playerDatumIdx);
	CustomVariantHandler::OnPlayerSpawn(ExecTime::_preEventExec, playerDatumIdx);

	bool ret = p_player_spawn(playerDatumIdx);

	// check if the spawn was successful
	if (ret)
	{
		EventHandler::PlayerSpawnEventExecute(EventExecutionType::execute_after, playerDatumIdx);
		CustomVariantHandler::OnPlayerSpawn(ExecTime::_postEventExec, playerDatumIdx);
	}

	return ret;
}

typedef void(__cdecl* change_team_t)(int a1, int a2);
change_team_t p_change_local_team;

void __cdecl changeTeam(int localPlayerIndex, int teamIndex)
{
	c_network_session* session = NetworkSession::GetActiveNetworkSession();

	// prevent team switch in the pregame lobby, when the game already started
	if (session) {
		if ((session->parameters[0].session_mode == 4
			&& get_game_life_cycle() == _life_cycle_pre_game))
			return;
	}
	p_change_local_team(localPlayerIndex, teamIndex);
}

void __cdecl print_to_console(const char* output)
{
	std::string finalOutput("[HSC Print] "); finalOutput += output;
	addDebugText(finalOutput.c_str());
}

bool GrenadeChainReactIsEngineMPCheck() {
	return game_is_multiplayer();
}

bool BansheeBombIsEngineMPCheck() {
	return game_is_multiplayer();
}

bool FlashlightIsEngineSPCheck() {
	return game_is_campaign();
}

typedef uint16(__cdecl* get_enabled_teams_flags_t)(c_network_session*);
get_enabled_teams_flags_t p_get_enabled_teams_flags;

uint16 __cdecl get_enabled_team_flags(c_network_session* session)
{
	uint16 default_teams_enabled_flags = p_get_enabled_teams_flags(session);
	uint16 new_teams_enabled_flags = (default_teams_enabled_flags & H2Config_team_bit_flags);
	const uint16 red_versus_blue_teams = FLAG(_game_team_red) | FLAG(_game_team_blue);
	const uint16 infection_teams = FLAG(_game_team_red) | FLAG(_game_team_green);

	std::wstring selected_map_file_name;

	// skip if we're not host, let the host control
	if (!NetworkSession::LocalPeerIsSessionHost())
		return default_teams_enabled_flags;

	if (CustomVariantHandler::ContainsGameVariant(NetworkSession::GetGameVariantName(), _id_infection))
	{
		// infection overrides H2Config
		// TODO get infection_teams through the interface
		new_teams_enabled_flags = infection_teams;
		if ((default_teams_enabled_flags & FLAG(_game_team_red)) == 0
			|| (default_teams_enabled_flags & FLAG(_game_team_green)) == 0)
		{
			LOG_WARNING_FUNC(" - infection teams disabled in default enabled team flags");
			if (MapManager::GetMapFilename(selected_map_file_name))
				LOG_WARNING_FUNCW(" - perhaps current selected map - {} doesn't support these teams?? overriding anyway", selected_map_file_name.c_str());
		}
	}
	else if (StrStrIW(NetworkSession::GetGameVariantName(), L"rvb") != NULL)
	{
		// same with rvb, overrides H2Config
		new_teams_enabled_flags = red_versus_blue_teams;
		if ((default_teams_enabled_flags & FLAG(_game_team_red)) == 0
			|| (default_teams_enabled_flags & FLAG(_game_team_blue)) == 0)
		{
			LOG_WARNING_FUNC(" - RvB teams disabled in default enabled team flags");
			if (MapManager::GetMapFilename(selected_map_file_name))
				LOG_WARNING_FUNCW(" - perhaps current selected map - {} doesn't support these teams??", selected_map_file_name.c_str());
		}
	}

	return new_teams_enabled_flags;
}

typedef int(__cdecl* get_next_hill_index_t)(int previousHill);
get_next_hill_index_t p_get_next_hill_index;
int __cdecl get_next_hill_index(int previousHill)
{
	int hillCount = *Memory::GetAddress<int*>(0x4dd0a8, 0x5008e8);
	if (previousHill + 1 >= hillCount) 
	{
		LOG_TRACE_GAME("[KoTH Behavior] Hill count: {} current hill: {} next hill: {}", hillCount, previousHill, 0);
		return 0;
	}
	LOG_TRACE_GAME("[KoTH Behavior] Hill count: {} current hill: {} next hill: {}", hillCount, previousHill, previousHill + 1);
	return previousHill + 1;
}

int32 get_active_count_from_bitflags(uint16 teams_bit_flags)
{
	int32 count = 0;
	for (int32 i = 0; i < _game_team_neutral; i++)
	{
		if (TEST_BIT(teams_bit_flags, i))
			count++;
	}
	return count;
}

bool __cdecl should_start_pregame_countdown_hook()
{
	// dedicated server only
	auto p_should_start_pregame_countdown = Memory::GetAddress<decltype(&should_start_pregame_countdown_hook)>(0x0, 0xBC2A);

	// if the game already thinks the game timer doesn't need to start, return false and skip any processing
	if (!p_should_start_pregame_countdown()
		|| !NetworkSession::LocalPeerIsSessionLeader())
		return false; 

	bool minimumPlayersConditionMet = true;
	if (H2Config_minimum_player_start > 0)
	{
		if (NetworkSession::GetPlayerCount() >= H2Config_minimum_player_start)
		{
			LOG_INFO_GAME(L"{} - minimum Player count met", __FUNCTIONW__);
			minimumPlayersConditionMet = true;
		}
		else
		{
			minimumPlayersConditionMet = false;
			ServerConsole::SendMsg(L"Waiting for Players | Esperando a los jugadores", true);
		}
	}

	if (!minimumPlayersConditionMet)
		return false;

	/*if (H2Config_even_shuffle_teams
		&& NetworkSession::IsVariantTeamPlay())
	{
		std::mt19937 mt_rand(rd());
		std::vector<int32> activePlayersIndices = NetworkSession::GetActivePlayerIndicesList();
		uint16 activeTeamsFlags = get_enabled_team_flags(NetworkSession::GetActiveNetworkSession());

		int32 max_teams = PIN(get_active_count_from_bitflags(activeTeamsFlags), 2, (int32)k_game_multiplayer_team_count);
		LOG_INFO_GAME("{} - balancing teams", __FUNCTION__);

		ServerConsole::SendMsg(L"Balancing Teams | Equilibrar equipos", true);
		
		int32 maxPlayersPerTeam = MAX(1, NetworkSession::GetPlayerCount() / max_teams);
		LOG_DEBUG_GAME("Players Per Team: {}", maxPlayersPerTeam);

		for (int32 i = 0; i < k_game_multiplayer_team_count; i++)
		{
			int32 currentTeamPlayers = 0;

			if (activePlayersIndices.empty())
				break;

			// check if the team is available for play
			if (!TEST_BIT(activeTeamsFlags, i))
				continue;

			std::uniform_int_distribution<int32> dist(0, activePlayersIndices.size() - 1);

			for (; currentTeamPlayers < maxPlayersPerTeam; currentTeamPlayers++)
			{
				int32 vecPlayerIdx = dist(mt_rand);
				int32 playerIndexSelected = activePlayersIndices[vecPlayerIdx];
				// swap the player index with the last one, then just pop the last element
				std::swap(activePlayersIndices[vecPlayerIdx], activePlayersIndices[activePlayersIndices.size() - 1]);
				activePlayersIndices.pop_back();

				NetworkMessage::SendTeamChange(NetworkSession::GetPeerIndex(playerIndexSelected), (e_game_team)i);
			}
		}
	}*/

	EventHandler::CountdownStartEventExecute(EventExecutionType::execute_after);
	return true;
}

//TODO: Move this.
void vip_lock(e_game_life_cycle state)
{
	if(state == _life_cycle_post_game)
	{
		ServerConsole::ClearVip();
		*Memory::GetAddress<byte*>(0, 0x534850) = 0;
	}
	if(state == _life_cycle_in_game)
	{
		for (int i = 0; i < k_maximum_players; i++)
		{
			if (NetworkSession::PlayerIsActive(i))
				ServerConsole::AddVip(NetworkSession::GetPlayerName(i));
		}
		*Memory::GetAddress<byte*>(0, 0x534850) = 2;
	}
}

void H2MOD::RegisterEvents()
{
	if (Memory::IsDedicatedServer())
	{
		// Server only callbacks
		// Setup Events for H2Config_vip_lock
		if (H2Config_vip_lock)
			EventHandler::register_callback(vip_lock, EventType::gamelifecycle_change, EventExecutionType::execute_after);
	}
	else 
	{
		// Client only callbacks	
	}
}

static real32 seconds_trigger_hold = 1.0f / 30.0f; // 0.033333333 seconds takes 2 60hz seconds

__declspec(naked) void object_function_value_adjust_primary_firing()
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
		fstp dword ptr [esp]
		call time_globals::seconds_to_ticks_real
		fstp dword ptr [esp]
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

void H2MOD::ApplyHooks() {
	/* Should store all offsets in a central location and swap the variables based on h2server/halo2.exe*/
	/* We also need added checks to see if someone is the host or not, if they're not they don't need any of this handling. */
	LOG_INFO_GAME("{} - applying hooks", __FUNCTION__);

	DETOUR_BEGIN();

	EngineHooks::ApplyHooks();

	/* Labeled "AutoPickup" handler may be proximity to vehicles and such as well */
	PatchCall(Memory::GetAddress(0x58789, 0x60C81), OnAutoPickUpHandler);

	// disable part of custom map tag verification
	NopFill(Memory::GetAddress(0x4FA0A, 0x56C0A), 6);

	//Disable lightsupressor function
	if (H2Config_light_suppressor)
	{
		NopFill(Memory::GetAddress(0x1922d9), 7);
	}

	//Guardian Patch
	p_object_cause_damage = Memory::GetAddress<object_cause_damage_t>(0x17AD81, 0x1525E1);
	PatchCall(Memory::GetAddress(0x147DB8, 0x172D55), projectile_collision_object_cause_damage);

	apply_cheat_hooks();
	game_statborg_apply_patches();
	simulation_game_objects_apply_patches();
	simulation_game_units_apply_patches();
	players_apply_patches();
	objects_apply_patches();
	weapon_definitions_apply_patches();
	observer_apply_patches();
	network_transport_apply_patches();
	network_session_apply_patches();
	bitstream_serialization_apply_patches();

	network_memory_apply_patches();

	simulation_apply_patches();
	simulation_players_apply_patches();

	cache_files_apply_patches();
	network_configuration_apply_patches();

	// server/client detours 
	DETOUR_ATTACH(p_player_spawn, Memory::GetAddress<player_spawn_t>(0x55952, 0x5DE4A), OnPlayerSpawn);
	DETOUR_ATTACH(p_player_died, Memory::GetAddress<player_died_t>(0x5587B, 0x5DD73), OnPlayerDeath);
	DETOUR_ATTACH(p_map_cache_load, Memory::GetAddress<map_cache_load_t>(0x8F62, 0x1F35C), OnMapLoad);
	DETOUR_ATTACH(p_object_deplete_body_internal, Memory::GetAddress<object_deplete_body_internal_t>(0x17B674, 0x152ED4), OnObjectDamage);
	DETOUR_ATTACH(p_get_enabled_teams_flags, Memory::GetAddress<get_enabled_teams_flags_t>(0x1B087B, 0x19698B), get_enabled_team_flags);

	// below hooks applied to specific executables
	if (!Memory::IsDedicatedServer()) {

		LOG_INFO_GAME("{} - applying client hooks", __FUNCTION__);

		// ### TODO dedi offset
		Codecave(Memory::GetAddress(0x15E8DC, 0x0), object_function_value_adjust_primary_firing, 4);

		/* These hooks are only built for the client, don't enable them on the server! */

		//Shader display hook
		//c_test_hook = Memory::GetAddress<p_test_hook*>(0x1A2AEE);
		//PatchCall(Memory::GetAddress(0x1a10de), test_shader_hook);
		//PatchCall(Memory::GetAddress(0x1a1324), test_hook);
		//PatchCall(Memory::GetAddress(0x1A2FF6), test_shader_hook);
		//PatchCall(Memory::GetAddress(0x1a316B), test_hook);

		// DETOUR_ATTACH(p_load_wgit, Memory::GetAddress<load_wgit_t>(0x2106A2), OnWgitLoad);

		DETOUR_ATTACH(p_show_error_screen, Memory::GetAddress<show_error_screen_t>(0x20E15A), showErrorScreen);

		//TODO: expensive, use for debugging/searching
		//string_display_hook_method = (string_display_hook)DetourFunc(Memory::GetAddress<BYTE*>(0x287AB5), (BYTE*)stringDisplayHook, 5);

		//pResetRound = (ResetRounds)DetourFunc(Memory::GetAddress<BYTE*>(0x6B1C8), (BYTE*)OnNextRound, 7);
		
		DETOUR_ATTACH(p_change_local_team, Memory::GetAddress<change_team_t>(0x2068F2), changeTeam);

		// hook the print command to redirect the output to our console
		PatchCall(Memory::GetAddress(0xE9E50), print_to_console);

		// set max model quality to L6
		WriteValue(Memory::GetAddress(0x190B38 + 1), 5);

		PatchCall(Memory::GetAddress(0x182d6d), GrenadeChainReactIsEngineMPCheck);
		PatchCall(Memory::GetAddress(0x92C05), BansheeBombIsEngineMPCheck);
		PatchCall(Memory::GetAddress(0x13ff75), FlashlightIsEngineSPCheck);



		levels_apply_patches();
		new_hud_apply_patches();
		motion_sensor_apply_patches();
		render_cameras_apply_patches();
		first_person_camera_apply_patches();
		first_person_weapons_apply_patches();
		bipeds_apply_patches();
		unit_apply_patches();

		user_interface_text_apply_hooks();
		user_interface_controller_apply_patches();
		hud_messaging_apply_hooks();
		font_group_apply_hooks();
		screens_apply_patches();
		aim_assist_apply_patches();
		main_game_apply_patches();
		
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
		render_weather_apply_patches();

		cinematics_apply_patches();
		game_time_apply_patches();
		game_state_procs_apply_patches();
		apply_particle_update_patches();
		apply_dead_camera_patches();
		loading_apply_patches();
		rasterizer_lens_flares_apply_patches();
		liquid_apply_patches();
		contrails_apply_patches();
		render_submit_apply_patches();
		cloth_apply_patches();
		camera_apply_patches();
		player_control_apply_patches();
		main_render_apply_patches();
		effects_apply_patches();
		xinput_apply_patches();
		player_vibration_apply_patches();
		input_windows_apply_patches();
		input_abstraction_patches_apply();
		render_apply_patches();
		apply_interface_hooks();

		new_hud_draw_apply_patches();
	}
	else {
		LOG_INFO_GAME("{} - applying dedicated server hooks", __FUNCTION__);

		PatchCall(Memory::GetAddress(0x0, 0xBF43), should_start_pregame_countdown_hook);
		ServerConsole::ApplyHooks();
	}

	DETOUR_COMMIT();
}

void H2MOD::Initialize()
{
	LOG_INFO_GAME("H2MOD - Initializing {}", DLL_VERSION_STR);
	LOG_INFO_GAME("H2MOD - Image base address: 0x{:X}", Memory::baseAddress);

	if (!Memory::IsDedicatedServer())
	{
		KeyboardInput::Initialize();
		
		RenderHooks::Initialize();
		DirectorHooks::Initialize();
		ImGuiHandler::WeaponOffsets::Initialize();
#ifndef NDEBUG
		ObserverMode::Initialize();
#endif
		TEST_N_DEF(PC3);
	}
	else
	{
		playlist_loader::initialize();
	}
	cartographer_player_profile_initialize();
	tag_injection_initialize();
	CommandCollection::InitializeCommands();
	CustomVariantHandler::RegisterCustomVariants();
	CustomVariantSettings::Initialize();
	MeleeFix::Initialize();
	MapSlots::Initialize();
	HaloScript::Initialize();
	KantTesting::Initialize();
	ProjectileFix::ApplyPatches();
	H2X::ApplyPatches();
	H2MOD::ApplyHooks();
	H2MOD::RegisterEvents();

	//StatsHandler::Initialize();

	LOG_INFO_GAME("H2MOD - Initialized");
}

void H2MOD::Deinitialize() 
{
}
