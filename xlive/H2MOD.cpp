#include "stdafx.h"
#include "H2MOD.h"

#include "Blam/Cache/TagGroups/multiplayer_globals_definition.hpp"
#include "Blam/Engine/game/cheats.h"
#include "Blam/Engine/interface/hud.h"
#include "Blam/Engine/interface/hud_messaging.h"
#include "Blam/Engine/interface/motion_sensor.h"
#include "Blam/Engine/interface/first_person_camera.h"
#include "Blam/Engine/interface/first_person_weapons.h"
#include "Blam/Engine/interface/new_hud.h"
#include "Blam/Engine/interface/user_interface_text.h"
#include "Blam/Engine/interface/screens/screens_patches.h"
#include "Blam/Engine/Networking/NetworkMessageTypeCollection.h"
#include "Blam/Engine/objects/damage.h"
#include "Blam/Engine/rasterizer/rasterizer_lens_flares.h"
#include "Blam/Engine/rasterizer/rasterizer_settings.h"
#include "Blam/Engine/render/render_cameras.h"
#include "Blam/Engine/text/font_cache.h"
#include "Blam/Engine/units/units.h"

#include "H2MOD/EngineHooks/EngineHooks.h"
#include "H2MOD/GUI/ImGui_Integration/ImGui_Handler.h"
#include "H2MOD/Modules/CustomVariantSettings/CustomVariantSettings.h"
#include "H2MOD/Modules/DirectorHooks/DirectorHooks.h"
#include "H2MOD/Modules/EventHandler/EventHandler.hpp"
#include "H2MOD/Modules/GamePhysics/Patches/MeleeFix.h"
#include "H2MOD/Modules/GamePhysics/Patches/ProjectileFix.h"
#include "H2MOD/Modules/HaloScript/HaloScript.h"
#include "H2MOD/Modules/Input/ControllerInput.h"
#include "H2MOD/Modules/Input/KeyboardInput.h"
#include "H2MOD/Modules/Input/Mouseinput.h"
#include "H2MOD/Modules/Input/PlayerControl.h"
#include "H2MOD/Modules/KantTesting/KantTesting.h"
#include "H2MOD/Modules/MainMenu/MapSlots.h"
#include "H2MOD/Modules/MainMenu/Ranks.h"
#include "H2MOD/Modules/MapManager/MapManager.h"
#include "H2MOD/Modules/ObserverMode/ObserverMode.h"
#include "H2MOD/Modules/OnScreenDebug/OnscreenDebug.h"
#include "H2MOD/Modules/PlayerRepresentation/PlayerRepresentation.h"
#include "H2MOD/Modules/PlaylistLoader/PlaylistLoader.h"
#include "H2MOD/Modules/RenderHooks/RenderHooks.h"
#include "H2MOD/Modules/Shell/Config.h"
#include "H2MOD/Modules/Shell/Shell.h"
#include "H2MOD/Modules/SpecialEvents/SpecialEvents.h"
#include "H2MOD/Modules/Stats/StatsHandler.h"
#include "H2MOD/Modules/TagFixes/TagFixes.h"
#include "H2MOD/Modules/Tweaks/Tweaks.h"
#include "H2MOD/Tags/MetaExtender.h"
#include "H2MOD/Tags/MetaLoader/tag_loader.h"
#include "H2MOD/Variants/Variants.h"
#include "Util/Hooks/Hook.h"

FLOATING_POINT_ENV_ACCESS();

std::unique_ptr<H2MOD> h2mod(std::make_unique<H2MOD>());

bool H2XFirerateEnabled = false;
bool xboxTickrateEnabled = false;

std::unordered_map<const wchar_t*, bool&> GametypesMap
{
	{ L"h2x", H2XFirerateEnabled },
	{ L"ogh2", xboxTickrateEnabled },
};

#pragma region engine calls

TEST_N_DEF(PC1);

// Used to get damage on any object
typedef void(__cdecl* object_cause_damage_t)(s_damage_data* damage_data, int damaged_object_indexes, __int16 a4, __int16 a5, __int16 a6, int a7);
object_cause_damage_t p_object_cause_damage;

// Engine call to set damage applied on an object by a projectile
void __cdecl projectile_collision_object_cause_damage(s_damage_data* damage_data, int damaged_object_indexes, __int16 a4, __int16 a5, __int16 a6, int a7)
{
	// Hook on call to prevent guardian glitching on Infection gametype
	if (CustomVariantHandler::VariantEnabled(_id_infection)) {
		if (!DATUM_IS_NONE(damage_data->creator_datum) 
			&& damage_data->field_10 != -1)
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

/* This looks at the actors table to get the character datum which is assigned to the specific actor. */
int get_char_datum_from_actor(int actor_datum)
{
	__int16 actor_index = actor_datum & 0xFFFF;
	DWORD actor_table_ptr = *Memory::GetAddress<DWORD*>(0xA965DC);
	DWORD actor_table = *(DWORD*)((BYTE*)actor_table_ptr + 0x44);
	DWORD actor = (DWORD)((BYTE*)actor_table + (actor_index * 0x898));
	int character_datum = *(int*)((BYTE*)actor+0x54);

	return character_datum;
}

/*This is to get the datum of the last player who damaged the datum/unit provided */
int get_damage_owner(datum damaged_unit_index)
{
	char* damaged_player_ptr = (char*)object_try_and_get_and_verify_type(damaged_unit_index, FLAG(e_object_type::biped) | FLAG(e_object_type::vehicle));
	if (damaged_player_ptr)
	{
		return *(int*)(damaged_player_ptr + 0xC8); // player_ptr/unit_ptr + 0xC8 = damaging player this works on vehicles/AI and such too.
	}

	return NONE;
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

/* controller index aka local player index -> player index */
datum H2MOD::get_player_datum_index_from_controller_index(int controller_index) 
{
	typedef int(__cdecl* get_local_player_index_t)(int controller_index); 
	auto p_get_local_player_index = Memory::GetAddress<get_local_player_index_t>(0x5141D);
	return p_get_local_player_index(controller_index); 
}

#pragma region PlayerFunctions

float H2MOD::get_distance(int playerIndex1, int playerIndex2) {
	real_point3d points_distance;
	real_point3d* player1 = nullptr;
	real_point3d* player2 = nullptr;

	player1 = h2mod->get_player_unit_coords(playerIndex1);
	player2 = h2mod->get_player_unit_coords(playerIndex2);
	
	points_distance.x = abs(player1->x - player2->x);
	points_distance.y = abs(player1->y - player2->y);
	points_distance.z = abs(player1->z - player2->z);

	return sqrt(pow(points_distance.x, 2) + pow(points_distance.y, 2) + pow(points_distance.z, 2));
}

real_point3d* H2MOD::get_player_unit_coords(int playerIndex) {
	BYTE* player_unit = get_player_unit_from_player_index(playerIndex);
	if (player_unit != nullptr)
		return reinterpret_cast<real_point3d*>(player_unit + 0x64);

	return nullptr;
}

BYTE* H2MOD::get_player_unit_from_player_index(int playerIndex) {
	datum unit_datum = s_player::GetPlayerUnitDatumIndex(playerIndex);
	if (DATUM_IS_NONE(unit_datum))
		return nullptr;

	return (BYTE*)object_get_fast_unsafe(unit_datum);
}

void call_give_player_weapon(int playerIndex, datum weaponId, bool resetLoadout)
{
	//LOG_TRACE_GAME("GivePlayerWeapon(PlayerIndex: %08X, WeaponId: %08X)", PlayerIndex, WeaponId);

	datum unit_datum = s_player::GetPlayerUnitDatumIndex(playerIndex);
	if (!DATUM_IS_NONE(unit_datum))
	{
		s_object_placement_data nObject;

		Engine::Objects::create_new_placement_data(&nObject, weaponId, unit_datum, 0);

		datum object_idx = Engine::Objects::object_new(&nObject);

		if (resetLoadout)
			unit_delete_all_weapons(unit_datum);

		unit_add_weapon_to_inventory(unit_datum, object_idx, _weapon_addition_method_one);
	}
}

const wchar_t* H2MOD::get_local_player_name(int local_player_index)
{
	return s_player::GetName(DATUM_INDEX_TO_ABSOLUTE_INDEX(this->get_player_datum_index_from_controller_index(local_player_index)));
}

int H2MOD::get_player_index_from_unit_datum_index(datum unit_datum_index)
{
	PlayerIterator playersIt;
	while (playersIt.get_next_active_player())
	{
		datum unit_datum_index_check = playersIt.get_current_player_data()->unit_index;
		LOG_TRACE_FUNC("Checking datum: {0:x} - index: {1} against datum: {2:x}", unit_datum_index_check, playersIt.get_current_player_index(), unit_datum_index);

		if (unit_datum_index == unit_datum_index_check)
			return playersIt.get_current_player_index();
	}
	return NONE;
}

BYTE H2MOD::get_unit_team_index(datum unit_datum_index)
{
	BYTE team_index = NONE;
	char* unit_object = (char*)object_try_and_get_and_verify_type(unit_datum_index, FLAG(e_object_type::biped));
	if (unit_object != NULL)
	{
		team_index = *(BYTE*)(unit_object + 0x13C);
	}
	return team_index;
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

void H2MOD::set_player_unit_grenades_count(int playerIndex, e_grenades type, BYTE count, bool resetEquipment)
{
	if (type > e_grenades::Plasma)
	{
		LOG_TRACE_GAME("[H2MOD] set_player_unit_grenades_count() Invalid argument: type");
		return;
	}

	static const std::string grenadeEquipamentTagName[2] =
	{
		"objects\\weapons\\grenade\\frag_grenade\\frag_grenade",
		"objects\\weapons\\grenade\\plasma_grenade\\plasma_grenade"
	};

	datum unit_datum_index = s_player::GetPlayerUnitDatumIndex(playerIndex);
	//datum grenade_eqip_tag_datum_index = tags::find_tag(blam_tag::tag_group_type::equipment, grenadeEquipamentTagName[type]);

	char* unit_object = (char*)object_try_and_get_and_verify_type(unit_datum_index, FLAG(e_object_type::biped));
	if (unit_object != NULL)
	{
		// not sure what these flags are, but this is called when picking up grenades
		typedef void(__cdecl* entity_set_unk_flags_t)(datum objectIndex, int flags);
		auto p_simulation_action_object_update = Memory::GetAddress<entity_set_unk_flags_t>(0x1B6685, 0x1B05B5);

		typedef void(__cdecl* unit_add_grenade_to_inventory_send_t)(datum unitDatumIndex, datum equipamentTagIndex);
		auto p_unit_add_grenade_to_inventory_send = Memory::GetAddress<unit_add_grenade_to_inventory_send_t>(0x1B6F12, 0x1B0E42);

		// send simulation update for grenades if we control the simulation
		if (!s_game_globals::game_is_predicted())
		{
			// delete all weapons if required
			if (resetEquipment)
				unit_delete_all_weapons(unit_datum_index);

			// set grenade count
			*(BYTE*)(unit_object + 0x252 + type) = count;

			p_simulation_action_object_update(unit_datum_index, FLAG(22)); // flag 22 seems to be sync entity grenade count (TODO: list all of the update types)
			//p_unit_add_grenade_to_inventory_send(unit_datum_index, grenade_eqip_tag_datum_index);
		}

		LOG_TRACE_GAME("set_player_unit_grenades_count() - sending grenade simulation update, playerIndex={0}, peerIndex={1}", playerIndex, NetworkSession::GetPeerIndex(playerIndex));
	}
}

BYTE H2MOD::get_local_team_index()
{
	return *Memory::GetAddress<BYTE*>(0x51A6B4);
}
#pragma endregion

void H2MOD::disable_sounds(int sound_flags)
{
	static const std::string multiplayerGlobalsTag("multiplayer\\multiplayer_globals");
	if (sound_flags)
	{
		datum multiplayerGlobalsTagIndex = tags::find_tag(blam_tag::tag_group_type::multiplayerglobals, multiplayerGlobalsTag);

		if (!DATUM_IS_NONE(multiplayerGlobalsTagIndex))
		{
			s_multiplayer_globals_group_definition* multiplayerGlobalsTag = tags::get_tag<blam_tag::tag_group_type::multiplayerglobals, s_multiplayer_globals_group_definition>(multiplayerGlobalsTagIndex);

			if (multiplayerGlobalsTag->runtime.size)
			{
				auto* runtime_tag_block_data = multiplayerGlobalsTag->runtime[0];

				if (sound_flags & FLAG(_sound_type_slayer))
				{
					runtime_tag_block_data->slayer_events.size = 0;
					runtime_tag_block_data->slayer_events.data = 0;
				}

				if (sound_flags & ALL_SOUNDS_NO_SLAYER) // check if there is any point in running the code bellow
				{
					for (int i = 0; i < runtime_tag_block_data->general_events.size; i++)
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
								(&general_event->sound)[j].TagIndex = DATUM_INDEX_NONE;
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

update_player_score_t p_update_player_score;

void __fastcall OnPlayerScore(void* thisptr, DWORD _edx, datum playerIdx, int a3, int a4, int a5, char a6)
{
	//LOG_TRACE_GAME("update_player_score_hook ( thisptr: %08X, a2: %08X, a3: %08X, a4: %08X, a5: %08X, a6: %08X )", thisptr, a2, a3, a4, a5, a6);
	//20/10/2018 18:46:51.541 update_player_score_hook ( thisptr: 3000595C, a2: 00000000, a3: 00000002, a4: 00000001, a5: 00000007, a6: 00000001 )
	// / 10 / 2018 18:46 : 51.541 update_player_score_hook(thisptr : 3000595C, a2 : 00000000, a3 : 00000000, a4 : 00000001, a5 : FFFFFFFF, a6 : 00000000)
	//	20 / 10 / 2018 18 : 46 : 51.541 update_player_score_hook(thisptr : 3000595C, a2 : 00000001, a3 : 00000003, a4 : 00000001, a5 : 00000009, a6: 00000001)

	//20 / 10 / 2018 18:48 : 39.756 update_player_score_hook(thisptr : 3000595C, a2 : 00000001, a3 : 00000002, a4 : 00000001, a5 : 00000007, a6 : 00000001)
	//	20 / 10 / 2018 18 : 48 : 39.756 update_player_score_hook(thisptr : 3000595C, a2 : 00000001, a3 : 00000000, a4 : 00000001, a5 : FFFFFFFF, a6 : 00000000)
	//	20 / 10 / 2018 18 : 48 : 39.756 update_player_score_hook(thisptr : 3000595C, a2 : 00000000, a3 : 00000003, a4 : 00000001, a5 : 00000009, a6: 00000001)

	bool handled = CustomVariantHandler::OnPlayerScore(ExecTime::_preEventExec, thisptr, playerIdx, a3, a4, a5, a6);
	if (!handled)
		p_update_player_score(thisptr, playerIdx, a3, a4, a5, a6);
	CustomVariantHandler::OnPlayerScore(ExecTime::_postEventExec, thisptr, playerIdx, a3, a4, a5, a6);
}

// Client Sided Patch
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

void H2MOD::set_local_rank(BYTE rank)
{
	if (Memory::IsDedicatedServer())
		return;

	static bool initialized = false;

	if (!initialized)
	{
		NopFill(Memory::GetAddress(0x1b2c29), 7);
		initialized = true;
	}

	s_player::s_player_properties* local_player_properties = Memory::GetAddress<s_player::s_player_properties*>(0x51A638);

	local_player_properties->player_overall_skill = rank;
	local_player_properties->player_displayed_skill = rank;
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

s_data_array* H2MOD::get_actor_table()
{
	return *Memory::GetAddress<s_data_array**>(0xA965DC, 0x9A1C5C);
}

void H2MOD::toggle_xbox_tickrate(s_game_options* options, bool toggle)
{
	options->tickrate = toggle ? 30 : 60;
	WriteValue<int>(Memory::GetAddress(0x264ABB, 0x1DB8B) + 1, (int)options->tickrate);
	LOG_TRACE_GAME("[h2mod] set game options tickrate to {}", options->tickrate);
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

	EventHandler::MapLoadEventExecute(EventExecutionType::execute_before, options->engine_type);
	CustomVariantHandler::OnMapLoad(ExecTime::_preEventExec, options);

	bool result = p_map_cache_load(options);
	if (result == false) // verify if the game didn't fail to load the map
		return false;

	// set the engine type
	h2mod->SetCurrentEngineType(options->engine_type);
	tags::run_callbacks();

	H2Tweaks::SetScreenRefreshRate();
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
	h2mod->toggle_xbox_tickrate(options, false);
	// reset custom gametypes state
	for (auto& gametype_it : GametypesMap)
		gametype_it.second = false;

	if (h2mod->GetEngineType() == e_engine_type::_main_menu)
	{
		addDebugText("Engine type: Main-Menu");
		UIRankPatch();
		MetaExtender::free_tag_blocks();
	}
	else
	{
		wchar_t* variant_name = NetworkSession::GetGameVariantName();
		LOG_INFO_GAME(L"[h2mod] engine type: {}, game variant name: {}", (int)h2mod->GetEngineType(), variant_name);

		ControllerInput::SetDeadzones();
		ControllerInput::SetSensitiviy(H2Config_controller_sens);
		MouseInput::SetSensitivity(H2Config_mouse_sens);
		hud_patches_on_map_load();
		screens_apply_patches_on_map_load();

		if (h2mod->GetEngineType() == e_engine_type::_multiplayer)
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

			h2mod->toggle_xbox_tickrate(options, xboxTickrateEnabled);
			if (!xboxTickrateEnabled)
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
		else if (h2mod->GetEngineType() == e_engine_type::_single_player)
		{
			//if anyone wants to run code on map load single player
			addDebugText("Engine type: Singleplayer");
			//H2X::Initialize(true);
		}

		resetAfterMatch = true;
	}

	EventHandler::MapLoadEventExecute(EventExecutionType::execute_after, options->engine_type);
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
	s_network_session* session = NetworkSession::GetActiveNetworkSession();

	// prevent team switch in the pregame lobby, when the game already started
	if (session) {
		if ((session->parameters[0].session_mode == 4
			&& get_game_life_cycle() == _life_cycle_pre_game))
			return;
	}
	p_change_local_team(localPlayerIndex, teamIndex);
}

void H2MOD::set_local_team_index(int local_player_index, int team_index)
{
	// we only use player index 0 due to no splitscreen support but whatever
	typedef void(__cdecl* update_player_profile_t)(int local_player_index);
	auto p_update_player_profile = Memory::GetAddress<update_player_profile_t>(0x206A97);

	p_change_local_team(local_player_index, team_index);
	p_update_player_profile(local_player_index); // fixes infection handicap glitch
}

void H2MOD::set_local_clan_tag(int local_player_index, unsigned long long tag)
{
	typedef void(__cdecl* update_player_profile_t)(int local_player_index);
	auto p_update_player_profile = Memory::GetAddress<update_player_profile_t>(0x206A97);
	unsigned long low = tag & 0xFFFFFFFF;
	*(unsigned long*)Memory::GetAddress(0x51A6A8 + (0xB8 * local_player_index)) = low;
	p_update_player_profile(local_player_index);
}

void __cdecl print_to_console(const char* output)
{
	std::string finalOutput("[HSC Print] "); finalOutput += output;
	addDebugText(finalOutput.c_str());
}

DWORD calculate_model_lod;
DWORD calculate_model_lod_detour_end;
__declspec(naked) void calculate_model_lod_detour()
{
	__asm
	{
		// replaced code
		// todo check if this is needed when using a static LOD, might save on some processor time
		call calculate_model_lod

		cmp H2Config_static_lod_state, 0
		jz END_DETOUR

		mov eax, H2Config_static_lod_state
		sub eax, 1 // convert setting to in-game model LOD value (0 - 5, L1 - L6)

		END_DETOUR:
		jmp calculate_model_lod_detour_end
	}
}

bool GrenadeChainReactIsEngineMPCheck() {
	return h2mod->GetEngineType() == e_engine_type::_multiplayer;
}

bool BansheeBombIsEngineMPCheck() {
	return h2mod->GetEngineType() == e_engine_type::_multiplayer;
}

bool FlashlightIsEngineSPCheck() {
	return h2mod->GetEngineType() == e_engine_type::_single_player;
}

void GivePlayerWeaponDatum(datum unit_datum, datum weapon_tag_index)
{
	if (!DATUM_IS_NONE(unit_datum))
	{
		s_object_placement_data object_placement;

		Engine::Objects::create_new_placement_data(&object_placement, weapon_tag_index, unit_datum, 0);

		datum object_idx = Engine::Objects::object_new(&object_placement);
		if (!DATUM_IS_NONE(object_idx))
		{
			unit_delete_all_weapons(unit_datum);
			unit_add_weapon_to_inventory(unit_datum, object_idx, _weapon_addition_method_one);
		}
	}
}

//This is used for maps with 'shops' where the device_acceleration_scale is an indicator that they're using the control device as a 'shop'
float get_device_acceleration_scale(datum device_datum)
{
	DWORD tag_data = (DWORD)tags::get_tag_data();
	DWORD tag_instances = (DWORD)tags::get_tag_instances();

	int device_gamestate_offset = DATUM_INDEX_TO_ABSOLUTE_INDEX(device_datum) + DATUM_INDEX_TO_ABSOLUTE_INDEX(device_datum) * 2;
	DWORD device_gamestate_datum_pointer = *(DWORD*)((BYTE*)get_objects_header()->data + device_gamestate_offset * 4 + 8);
	DWORD device_control_datum = *(DWORD*)((BYTE*)device_gamestate_datum_pointer);

	__int16 device_control_index = device_control_datum & 0xFFFF;
	device_control_index = device_control_index << 4;

	DWORD device_control_tag_offset = *(DWORD*)((BYTE*)device_control_index + tag_instances + 8);
	float acceleration_scale = *(float*)((BYTE*)device_control_tag_offset + tag_data + 0x14);

	return acceleration_scale;
}

typedef int(__cdecl *device_touch_t)(datum device_datum, datum unit_datum);
device_touch_t pdevice_touch;

bool device_active = true;
// This happens whenever a player activates a device control.
int __cdecl device_touch(datum device_datum, datum unit_datum)
{
	if (h2mod->GetEngineType() == e_engine_type::_multiplayer)
	{
		// We check this to see if the device control is a 'shopping' device, if so send a request to buy an item to the DeviceShop.
		if (get_device_acceleration_scale(device_datum) == 999.0f)
		{
			if (deviceShop->BuyItem(device_datum, unit_datum)) // If the purchase was successful we won't execute the original device control action.
			{
				if (device_active == false)
					return pdevice_touch(device_datum, unit_datum);

				device_active = true;
				return 0;
			}
			// If the purchase fails (they don't have enough points), or the device is not a shopping device return normally.
			// In general's map returning normally will turn the point display red indicating the user has no points, we do not indicate that the purchase failed in any other way.
			if (device_active == false)
				return 0;

			device_active = false;
		}
	}

	return pdevice_touch(device_datum, unit_datum);
}

void H2MOD::team_player_indicator_visibility(bool toggle)
{
	this->drawTeamIndicators = toggle;
}

void __cdecl game_mode_engine_draw_team_indicators(int local_user_render_idx)
{
	typedef void(__cdecl* game_mode_engine_draw_team_indicators_t)(int);
	auto p_game_mode_engine_draw_team_indicators = Memory::GetAddress<game_mode_engine_draw_team_indicators_t>(0x6AFA4);

	if (h2mod->drawTeamIndicators)
		p_game_mode_engine_draw_team_indicators(local_user_render_idx);
}

typedef short(__cdecl* get_enabled_teams_flags_t)(s_network_session*);
get_enabled_teams_flags_t p_get_enabled_teams_flags;

short __cdecl get_enabled_team_flags(s_network_session* session)
{
	short default_teams_enabled_flags = p_get_enabled_teams_flags(session);
	short new_teams_enabled_flags = (default_teams_enabled_flags & H2Config_team_bit_flags);
	const short red_versus_blue_teams = FLAG(_object_team_red) | FLAG(_object_team_blue);
	const short infection_teams = FLAG(_object_team_red) | FLAG(_object_team_green);

	std::wstring selected_map_file_name;

	// skip if we're not host, let the host control
	if (!NetworkSession::LocalPeerIsSessionHost())
		return default_teams_enabled_flags;

	if (CustomVariantHandler::ContainsGameVariant(NetworkSession::GetGameVariantName(), _id_infection))
	{
		// infection overrides H2Config
		// TODO get infection_teams through the interface
		new_teams_enabled_flags = infection_teams;
		if ((default_teams_enabled_flags & FLAG(_object_team_red)) == 0
			|| (default_teams_enabled_flags & FLAG(_object_team_green)) == 0)
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
		if ((default_teams_enabled_flags & FLAG(_object_team_red)) == 0
			|| (default_teams_enabled_flags & FLAG(_object_team_blue)) == 0)
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

void H2MOD::ApplyFirefightHooks()
{
	pdevice_touch = (device_touch_t)DetourFunc(Memory::GetAddress<BYTE*>(0x163420, 0x158EE3), (BYTE*)device_touch, 10);
}

int get_active_count_from_bitflags(short teams_bit_flags)
{
	int count = 0;
	for (int i = 0; i < _object_team_end; i++)
	{
		if (TEST_FLAG(teams_bit_flags, i))
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

	if (H2Config_even_shuffle_teams
		&& NetworkSession::IsVariantTeamPlay())
	{
		std::mt19937 mt_rand(rd());
		std::vector<int> activePlayersIndices = NetworkSession::GetActivePlayerIndicesList();
		short activeTeamsFlags = get_enabled_team_flags(NetworkSession::GetActiveNetworkSession());

		int maxTeams = (std::min)((std::max)(get_active_count_from_bitflags(activeTeamsFlags), 2), (int)_object_team_end);

		LOG_INFO_GAME("{} - balancing teams", __FUNCTION__);

		ServerConsole::SendMsg(L"Balancing Teams | Equilibrar equipos", true);
		
		int maxPlayersPerTeam = (std::max)(1, NetworkSession::GetPlayerCount() / maxTeams);

		LOG_DEBUG_GAME("Players Per Team: {}", maxPlayersPerTeam);

		for (int i = 0; i < _object_team_end; i++)
		{
			int currentTeamPlayers = 0;

			if (activePlayersIndices.empty())
				break;

			// check if the team is available for play
			if (!TEST_FLAG(activeTeamsFlags, i))
				continue;

			std::uniform_int_distribution<int> dist(0, activePlayersIndices.size() - 1);

			for (; currentTeamPlayers < maxPlayersPerTeam; currentTeamPlayers++)
			{
				int vecPlayerIdx = dist(mt_rand);
				int playerIndexSelected = activePlayersIndices[vecPlayerIdx];
				// swap the player index with the last one, then just pop the last element
				std::swap(activePlayersIndices[vecPlayerIdx], activePlayersIndices[activePlayersIndices.size() - 1]);
				activePlayersIndices.pop_back();

				NetworkMessage::SendTeamChange(NetworkSession::GetPeerIndex(playerIndexSelected), i);
			}
		}

	}

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
		//ServerConsole::SendCommand2(1, L"vip", L"clear");
		//ServerConsole::SendCommand2(1, L"Privacy", L"Open");
	}
	if(state == _life_cycle_in_game)
	{
		for (int i = 0; i < k_maximum_players; i++)
		{
			if (NetworkSession::PlayerIsActive(i))
				ServerConsole::AddVip(NetworkSession::GetPlayerName(i));
			//ServerConsole::SendCommand2(2, L"vip", L"add", NetworkSession::getPeerPlayerName(i));
		}
		//ServerConsole::SendCommand2(1, L"Privacy", L"VIP");
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

// unlocks all single player maps
int __cdecl get_last_single_player_level_id_unlocked_from_profile()
{
	return 805; // return the id of the last level
}

// sword-flying target clear patch
void __cdecl aim_assist_targeting_clear_hook(int target_data)
{
	if (!s_game_globals::game_is_campaign()
		&& !xboxTickrateEnabled)
	{
		*(DWORD*)(target_data) = -1;
		*(DWORD*)(target_data + 4) = -1;
		*(DWORD*)(target_data + 8) = -1;
		*(WORD*)(target_data + 24) = 0;
		*(DWORD*)(target_data + 28) = 0;
		*(DWORD*)(target_data + 32) = 0;
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

	ApplyFirefightHooks();

	//Guardian Patch
	p_object_cause_damage = Memory::GetAddress<object_cause_damage_t>(0x17AD81, 0x1525E1);
	PatchCall(Memory::GetAddress(0x147DB8, 0x172D55), projectile_collision_object_cause_damage);

	apply_cheat_hooks();

	hud_apply_patches();
	new_hud_apply_patches();
	motion_sensor_apply_patches();
	render_cameras_apply_patches();
	first_person_camera_apply_patches();
	first_person_weapons_apply_patches();

	// server/client detours 
	DETOUR_ATTACH(p_player_spawn, Memory::GetAddress<player_spawn_t>(0x55952, 0x5DE4A), OnPlayerSpawn);
	DETOUR_ATTACH(p_player_died, Memory::GetAddress<player_died_t>(0x5587B, 0x5DD73), OnPlayerDeath);
	DETOUR_ATTACH(p_map_cache_load, Memory::GetAddress<map_cache_load_t>(0x8F62, 0x1F35C), OnMapLoad);
	DETOUR_ATTACH(p_update_player_score, Memory::GetAddress<update_player_score_t>(0xD03ED, 0x8C84C), OnPlayerScore);
	DETOUR_ATTACH(p_object_deplete_body_internal, Memory::GetAddress<object_deplete_body_internal_t>(0x17B674, 0x152ED4), OnObjectDamage);
	DETOUR_ATTACH(p_get_enabled_teams_flags, Memory::GetAddress<get_enabled_teams_flags_t>(0x1B087B, 0x19698B), get_enabled_team_flags);

	// bellow hooks applied to specific executables
	if (!Memory::IsDedicatedServer()) {

		LOG_INFO_GAME("{} - applying client hooks", __FUNCTION__);
		lens_flare_fix();

		/* These hooks are only built for the client, don't enable them on the server! */

		//Shader display hook
		//c_test_hook = Memory::GetAddress<p_test_hook*>(0x1A2AEE);
		//PatchCall(Memory::GetAddress(0x1a10de), test_shader_hook);
		//PatchCall(Memory::GetAddress(0x1a1324), test_hook);
		//PatchCall(Memory::GetAddress(0x1A2FF6), test_shader_hook);
		//PatchCall(Memory::GetAddress(0x1a316B), test_hook);

		// DETOUR_ATTACH(p_load_wgit, Memory::GetAddress<load_wgit_t>(0x2106A2), OnWgitLoad);

		DETOUR_ATTACH(p_show_error_screen, Memory::GetAddress<show_error_screen_t>(0x20E15A), showErrorScreen);

		PatchCall(Memory::GetAddress(0x169E59), aim_assist_targeting_clear_hook);

		//TODO: expensive, use for debugging/searching
		//string_display_hook_method = (string_display_hook)DetourFunc(Memory::GetAddress<BYTE*>(0x287AB5), (BYTE*)stringDisplayHook, 5);

		//pResetRound = (ResetRounds)DetourFunc(Memory::GetAddress<BYTE*>(0x6B1C8), (BYTE*)OnNextRound, 7);

		TEST_N_DEF(PC2);
		
		DETOUR_ATTACH(p_change_local_team, Memory::GetAddress<change_team_t>(0x2068F2), changeTeam);

		// hook the print command to redirect the output to our console
		PatchCall(Memory::GetAddress(0xE9E50), print_to_console);

		calculate_model_lod = Memory::GetAddress(0x19CA3E);
		calculate_model_lod_detour_end = Memory::GetAddress(0x19CDA3 + 5);
		WriteJmpTo(Memory::GetAddress(0x19CDA3), calculate_model_lod_detour);

		// set max model quality to L6
		WriteValue(Memory::GetAddress(0x190B38 + 1), 5);

		PatchCall(Memory::GetAddress(0x182d6d), GrenadeChainReactIsEngineMPCheck);
		PatchCall(Memory::GetAddress(0x92C05), BansheeBombIsEngineMPCheck);
		PatchCall(Memory::GetAddress(0x13ff75), FlashlightIsEngineSPCheck);

		PatchCall(Memory::GetAddress(0x226702), game_mode_engine_draw_team_indicators);

		// Initialise_tag_loader();
		PlayerControl::ApplyHooks();
		
		PatchCall(Memory::GetAddressRelative(0x6422C8), get_last_single_player_level_id_unlocked_from_profile);

		rasterizer_settings_apply_hooks();
		user_interface_text_apply_hooks();
		hud_messaging_apply_hooks();
		font_group_apply_hooks();
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

	PlayerRepresentation::Initialize();
	if (!Memory::IsDedicatedServer())
	{
		MouseInput::Initialize();
		KeyboardInput::Initialize();
		ControllerInput::Initialize();

		Initialise_tag_loader();
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
	CustomVariantHandler::RegisterCustomVariants();
	CustomVariantSettings::Initialize();
	MeleeFix::Initialize();
	TagFixes::Initalize();
	MapSlots::Initialize();
	HaloScript::Initialize();
	KantTesting::Initialize();
	ProjectileFix::ApplyPatches();
	H2X::ApplyPatches();
	H2MOD::ApplyHooks();
	H2MOD::RegisterEvents();

	Engine::Objects::apply_biped_object_definition_patches();
	StatsHandler::Initialize();

	LOG_INFO_GAME("H2MOD - Initialized");
}

void H2MOD::Deinitialize() 
{
}
