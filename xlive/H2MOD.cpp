#include "stdafx.h"

#include "H2MOD.h"
#include "Blam\Enums\HaloStrings.h"
#include "Blam\Cache\TagGroups\biped_definition.hpp"
#include "Blam\Cache\TagGroups\globals_definition.hpp"
#include "Blam\Cache\TagGroups\model_definition.hpp"
#include "Blam\Engine\Memory\bitstream.h"
#include "Blam\Engine\Game\GameGlobals.h"
#include "Blam\Engine\Game\GameTimeGlobals.h"
#include "Blam\FileSystem\FiloInterface.h"
#include "Blam\Engine\Game\DamageData.h"
#include "Blam\Engine\Networking\NetworkMessageTypeCollection.h"
#include "Blam\Cache\TagGroups\multiplayer_globals_definition.hpp"
#include "Blam\Engine\IceCreamFlavor\IceCreamFlavor.h"
#include "H2MOD\Discord\DiscordInterface.h"
#include "H2MOD\Engine\Engine.h"
#include "H2MOD\EngineHooks\EngineHooks.h"
#include "H2MOD\GUI\GUI.h"
#include "H2MOD\Modules\Shell\Config.h"
#include "H2MOD\Modules\CustomVariantSettings\CustomVariantSettings.h"
#include "H2MOD\Modules\DirectorHooks\DirectorHooks.h"
#include "H2MOD\Modules\EventHandler\EventHandler.hpp"
#include "H2MOD\Modules\GamePhysics\Patches\MeleeFix.h"
#include "H2MOD\Modules\GamePhysics\Patches\ProjectileFix.h"
#include "H2MOD\Modules\HaloScript\HaloScript.h"
#include "H2MOD\Modules\HudElements\HudElements.h"
#include "H2MOD\Modules\Input\ControllerInput.h"
#include "H2MOD\Modules\Input\KeyboardInput.h"
#include "H2MOD\Modules\Input\Mouseinput.h"
#include "H2MOD\Modules\Input\PlayerControl.h"
#include "H2MOD\Modules\KantTesting\KantTesting.h"
#include "H2MOD\Modules\MainMenu\MapSlots.h"
#include "H2MOD\Modules\MainMenu\Ranks.h"
#include "H2MOD\Modules\ObserverMode\ObserverMode.h"
#include "H2MOD\Modules\OnScreenDebug\OnscreenDebug.h"
#include "H2MOD\Modules\PlayerRepresentation\PlayerRepresentation.h"
#include "H2MOD\Modules\PlaylistLoader\PlaylistLoader.h"
#include "H2MOD\Modules\RenderHooks\RenderHooks.h"
#include "H2MOD\Modules\SpecialEvents\SpecialEvents.h"
#include "H2MOD\Modules\Stats\StatsHandler.h"
#include "H2MOD\Modules\TagFixes\TagFixes.h"
#include "H2MOD\Modules\Tweaks\Tweaks.h"
#include "H2MOD\Tags\MetaExtender.h"
#include "H2MOD\Tags\MetaLoader\tag_loader.h"
#include "Util\Hooks\Hook.h"
#include "H2MOD\GUI\imgui_integration\imgui_handler.h"

#include <float.h>

#if (!defined(_M_FP_FAST)) || !_M_FP_FAST
#pragma fenv_access (on)
#endif

H2MOD* h2mod = new H2MOD();

extern int H2GetInstanceId();

bool b_H2X = false;
bool b_XboxTick = false;

// TODO: remove these
s_data_array* game_state_actors = nullptr;

std::unordered_map<const wchar_t*, bool&> GametypesMap
{
	{ L"h2x", b_H2X },
	{ L"ogh2", b_XboxTick },
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
		if (!DATUM_IS_NONE(damage_data->creator_datum) && damage_data->field_10 != -1)
		{
			LOG_TRACE_GAME(
				"{} {} {} {} {} {} {} {}",
				__FUNCTION__,
				damage_data->flags,
				IntToString<int>(damage_data->damage_tag_index, std::hex),
				IntToString<int>(damage_data->creator_datum, std::hex),
				IntToString<int>(damage_data->field_10, std::hex), //TODO reverse what field_10 is
				IntToString<int>(damage_data->field_14, std::hex),
				IntToString<int>(damage_data->field_18, std::hex),
				IntToString<int>(damage_data->field_1C, std::hex),
				IntToString<int>(damage_data->field_24, std::hex),
				IntToString<int>(damage_data->field_28, std::hex)
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

typedef signed int(__cdecl* wcsncpy_s_hook_t)(int a1, unsigned int a2, wchar_t* a3, int a4);
wcsncpy_s_hook_t p_wcsncpy_s_hook;

//lets you follow the call path of any string that is displayed (in a debugger)
signed int __cdecl stringDisplayHook(int a1, unsigned int a2, wchar_t* a3, int a4) {
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

void call_give_player_weapon(int playerIndex, datum weaponId, bool bReset)
{
	//LOG_TRACE_GAME("GivePlayerWeapon(PlayerIndex: %08X, WeaponId: %08X)", PlayerIndex, WeaponId);

	datum unit_datum = s_player::GetPlayerUnitDatumIndex(playerIndex);
	if (!DATUM_IS_NONE(unit_datum))
	{
		s_object_placement_data nObject;

		Engine::Objects::create_new_placement_data(&nObject, weaponId, unit_datum, 0);

		datum object_idx = Engine::Objects::object_new(&nObject);

		if (bReset)
			Engine::Unit::remove_equipment(unit_datum);

		Engine::Unit::assign_equipment_to_unit(unit_datum, object_idx, 1);
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
				Engine::Unit::remove_equipment(unit_datum_index);

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
typedef char(__cdecl* object_deplete_body_internal_t)(datum unit_datum_index, int a2, bool a3, bool a4);
object_deplete_body_internal_t p_object_deplete_body_internal;

char __cdecl OnObjectDamage(datum unit_datum_index, int a2, bool a3, bool a4)
{
	//LOG_TRACE_GAME("OnPlayerDeath(unit_datum_index: %08X, a2: %08X, a3: %08X, a4: %08X)", unit_datum_index,a2,a3,a4);

	/* The first value within a2 ( *(DWORD*)(a2) ) appears to be the datum_index of a player from the gamestate_player_table */

	EventHandler::ObjectDamageEventExecute(EventExecutionType::execute_before, unit_datum_index, *(datum*)(a2));
	CustomVariantHandler::OnObjectDamage(ExecTime::_preEventExec, unit_datum_index, a2, a3, a4);

	bool ret = p_object_deplete_body_internal(unit_datum_index, a2, a3, a4);

	CustomVariantHandler::OnObjectDamage(ExecTime::_postEventExec, unit_datum_index, a2, a3, a4);
	EventHandler::ObjectDamageEventExecute(EventExecutionType::execute_after, unit_datum_index, *(datum*)(a2));

	return ret;
}

typedef void(__thiscall* update_player_score_t)(void* thisptr, unsigned short a2, int a3, int a4, int a5, char a6);
update_player_score_t p_update_player_score;

void __fastcall OnPlayerScore(void* thisptr, BYTE _edx, unsigned short a2, int a3, int a4, int a5, char a6)
{
	//LOG_TRACE_GAME("update_player_score_hook ( thisptr: %08X, a2: %08X, a3: %08X, a4: %08X, a5: %08X, a6: %08X )", thisptr, a2, a3, a4, a5, a6);
	//20/10/2018 18:46:51.541 update_player_score_hook ( thisptr: 3000595C, a2: 00000000, a3: 00000002, a4: 00000001, a5: 00000007, a6: 00000001 )
	// / 10 / 2018 18:46 : 51.541 update_player_score_hook(thisptr : 3000595C, a2 : 00000000, a3 : 00000000, a4 : 00000001, a5 : FFFFFFFF, a6 : 00000000)
	//	20 / 10 / 2018 18 : 46 : 51.541 update_player_score_hook(thisptr : 3000595C, a2 : 00000001, a3 : 00000003, a4 : 00000001, a5 : 00000009, a6: 00000001)

	//20 / 10 / 2018 18:48 : 39.756 update_player_score_hook(thisptr : 3000595C, a2 : 00000001, a3 : 00000002, a4 : 00000001, a5 : 00000007, a6 : 00000001)
	//	20 / 10 / 2018 18 : 48 : 39.756 update_player_score_hook(thisptr : 3000595C, a2 : 00000001, a3 : 00000000, a4 : 00000001, a5 : FFFFFFFF, a6 : 00000000)
	//	20 / 10 / 2018 18 : 48 : 39.756 update_player_score_hook(thisptr : 3000595C, a2 : 00000000, a3 : 00000003, a4 : 00000001, a5 : 00000009, a6: 00000001)

	CustomVariantHandler::OnPlayerScore(ExecTime::_preEventExec, thisptr, a2, a3, a4, a5, a6);
	p_update_player_score(thisptr, a2, a3, a4, a5, a6);
	CustomVariantHandler::OnPlayerScore(ExecTime::_postEventExec, thisptr, a2, a3, a4, a5, a6);
}

// Client Sided Patch
void H2MOD::disable_weapon_pickup(bool bEnable)
{
	static BYTE oldBytes[5];
	static BYTE oldBytesRead = false;
	DWORD address = Memory::GetAddress(0x55EFA);

	if (oldBytesRead == false)
	{
		ReadBytesProtected(address, oldBytes, sizeof(oldBytes));
		oldBytesRead = true;
	}

	if (bEnable) 
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

void get_object_table_memory()
{
	game_state_actors = *Memory::GetAddress<s_data_array**>(0xA965DC, 0x9A1C5C);
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

	EventHandler::MapLoadEventExecute(EventExecutionType::execute_before, options->m_engine_type);
	CustomVariantHandler::OnMapLoad(ExecTime::_preEventExec, options);

	bool result = p_map_cache_load(options);
	if (result == false) // verify if the game didn't fail to load the map
		return false;

	// set the engine type
	h2mod->SetCurrentEngineType(options->m_engine_type);

	tags::run_callbacks();

	get_object_table_memory();

	H2Tweaks::setHz();
	ImGuiHandler::WeaponOffsets::MapLoad();

	// when the game is minimized, the game might skip loading Main menu
	// this is where resetAfterMatch var comes in for help
	if (resetAfterMatch)
	{
		if (b_XboxTick) {
			options->tickrate = XboxTick::setTickRate(false);
			b_XboxTick = false;
		}

		// here deinitialize the custom variant
		CustomVariantHandler::DisposeGameVariant();

		resetAfterMatch = false;
	}

	if (h2mod->GetEngineType() == e_engine_type::_main_menu)
	{
		addDebugText("Engine type: Main-Menu");
		UIRankPatch();
		H2Tweaks::toggleAiMp(false);
		H2Tweaks::toggleUncappedCampaignCinematics(false);
		MetaExtender::free_tag_blocks();
	}
	else
	{
		wchar_t* variant_name = NetworkSession::GetGameVariantName();
		LOG_INFO_GAME(L"[h2mod] engine type: {}, game variant name: {}", (int)h2mod->GetEngineType(), variant_name);

		for (auto& gametype_it : GametypesMap)
		{
			gametype_it.second = false; // reset custom gametypes state
		}

		ControllerInput::SetDeadzones();
		ControllerInput::SetSensitiviy(H2Config_controller_sens);
		MouseInput::SetSensitivity(H2Config_mouse_sens);
		HudElements::OnMapLoad();

		if (h2mod->GetEngineType() == e_engine_type::_multiplayer)
		{
			addDebugText("Engine type: Multiplayer");

			for (auto& gametype_it : GametypesMap)
			{
				if (StrStrIW(variant_name, gametype_it.first)) {
					LOG_INFO_GAME(L"{} - {} custom gametype turned on!", __FUNCTIONW__, gametype_it.first);
					gametype_it.second = true;
					break;
				}
			}

			if (!b_XboxTick)
			{
				H2X::Initialize(b_H2X);
				ProjectileFix::ApplyProjectileVelocity();
				options->tickrate = XboxTick::setTickRate(false);
			}
			else
			{
				options->tickrate = XboxTick::setTickRate(true);
			}

			H2Tweaks::toggleAiMp(true);
			H2Tweaks::toggleUncappedCampaignCinematics(false);

			if (Engine::get_game_life_cycle() == _life_cycle_in_game)
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
			H2Tweaks::toggleUncappedCampaignCinematics(true);
		}

		resetAfterMatch = true;
	}

	EventHandler::MapLoadEventExecute(EventExecutionType::execute_after, options->m_engine_type);
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
	s_network_session* session = NetworkSession::GetCurrentNetworkSession();

	if ((session->parameters[0].session_mode == 4 && Engine::get_game_life_cycle() == _life_cycle_pre_game)
		|| (StrStrIW(NetworkSession::GetGameVariantName(), L"rvb") != NULL && teamIndex > 1)) {
		//rvb mode enabled, don't change teams
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

typedef bool(__cdecl* fn_c000bd114_t)(e_skulls);
fn_c000bd114_t p_fn_c000bd114;

bool __cdecl fn_c000bd114_IsSkullEnabled(e_skulls skull_index)
{
	return ice_cream_flavor_available(skull_index);
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
			Engine::Unit::remove_equipment(unit_datum);
			Engine::Unit::assign_equipment_to_unit(unit_datum, object_idx, 1);
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

typedef int(__cdecl *tdevice_touch)(datum device_datum, datum unit_datum);
tdevice_touch pdevice_touch;

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

void __cdecl game_mode_engine_draw_team_indicators()
{
	typedef void(__cdecl* game_mode_engine_draw_team_indicators_t)();
	auto p_game_mode_engine_draw_team_indicators = Memory::GetAddress<game_mode_engine_draw_team_indicators_t>(0x6AFA4);

	if (h2mod->drawTeamIndicators)
		p_game_mode_engine_draw_team_indicators();
}

typedef short(__cdecl* get_enabled_teams_flags_t)(s_network_session*);
get_enabled_teams_flags_t p_get_enabled_teams_flags;

short __cdecl get_enabled_teams_flags(s_network_session* session)
{
	short default_teams_enabled_flags = p_get_enabled_teams_flags(session);
	short new_teams_enabled_flags = (default_teams_enabled_flags & H2Config_team_bit_flags);
	if (new_teams_enabled_flags && !CustomVariantHandler::VariantEnabled(_id_infection))
		return new_teams_enabled_flags;
	else
		return default_teams_enabled_flags;
}

typedef int(__cdecl* get_next_hill_index_t)(int previousHill);
get_next_hill_index_t p_get_next_hill_index;
signed int __cdecl get_next_hill_index(int previousHill)
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
	pdevice_touch = (tdevice_touch)DetourFunc(Memory::GetAddress<BYTE*>(0x163420, 0x158EE3), (BYTE*)device_touch, 10);
}

typedef void(__cdecl set_screen_bounds_t)(signed int a1, signed int a2, __int16 a3, __int16 a4, __int16 a5, __int16 a6, float a7, float res_scale);
set_screen_bounds_t* p_set_screen_bounds;

void __cdecl set_screen_bounds(signed int a1, signed int a2, __int16 a3, __int16 a4, __int16 a5, __int16 a6, float a7, float res_scale)
{
	p_set_screen_bounds(a1, a2, a3, a4, a5, a6, a7, 1.5f);
}

bool __cdecl should_start_pregame_countdown_hook()
{
	// dedicated server only
	auto p_should_start_pregame_countdown_hook = Memory::GetAddressRelative<decltype(&should_start_pregame_countdown_hook)>(0x0, 0x40BC2A);

	// store what the game thinks about starting the countdown timer
	bool gameCheck = p_should_start_pregame_countdown_hook();

	if (!gameCheck)
		return false; // if the game already thinks the game timer doesn't need to start, return false and skip any processing

	bool teamsAreValidConditionMet = false;
	bool minimumPlayersConditionMet = false;

	//network_session* networkSession = NetworkSession::getCurrentNetworkSession();

	if (H2Config_minimum_player_start > 0)
	{
		if (NetworkSession::GetPlayerCount() >= H2Config_minimum_player_start)
		{
			LOG_INFO_GAME(L"{} - minimum Player count met", __FUNCTIONW__);
			minimumPlayersConditionMet = true;
		}
		else
		{
			ServerConsole::SendMsg(L"Waiting for Players | Esperando a los jugadores", true);
		}
	}
	else
	{
		minimumPlayersConditionMet = true;
	}

	BYTE TeamPlay = *Memory::GetAddress<BYTE*>(0, 0x992880);
	if (H2Config_force_even && TeamPlay == 1 && minimumPlayersConditionMet)
	{
		ServerConsole::SendMsg(L"Balancing Teams | Equilibrar equipos", true);
		LOG_INFO_GAME(L"{} - balancing teams", __FUNCTIONW__);
		std::map<std::string, std::vector<int>> Parties;
		std::vector<int> nonPartyPlayers;
		for (auto i = 0; i < 16; i++) //Detect party members
		{
			if (NetworkSession::PlayerIsActive(i))
			{
				//auto player = NetworkSession::getPlayerInformation(i);
				int calcBaseOffset = 0x530E34 + (i * 0x128);
				auto ClanDescripton = *Memory::GetAddress<unsigned long*>(0, calcBaseOffset + 0x88);
				auto Gamertag = Memory::GetAddress<wchar_t*>(0, calcBaseOffset + 0x18);
				//NetworkSession::getPlayerInformation(i).properties->ClanTag did not appear to have this data.
				auto partyCode = IntToString<unsigned long>(ClanDescripton, std::dec);
				LOG_DEBUG_GAME(L"Checking if {} is part of a party", Gamertag);
				if (ClanDescripton != 0)
				{
					LOG_DEBUG_GAME(L"Party {} adding member {}", std::wstring(partyCode.begin(), partyCode.end()), Gamertag);
					Parties[partyCode].push_back(NetworkSession::GetPlayerIdByName(Gamertag));
				}
				else
					nonPartyPlayers.push_back(NetworkSession::GetPlayerIdByName(Gamertag));
			}
		}
		for (auto i = 0; i < 16; i++) //Detect party leaders
		{
			if (NetworkSession::PlayerIsActive(i))
			{
				int calcBaseOffset = 0x530E34 + (i * 0x128);
				auto playerId = *Memory::GetAddress<unsigned long long*>(0, calcBaseOffset);
				auto Gamertag = Memory::GetAddress<wchar_t*>(0, calcBaseOffset + 0x18);
				auto xuidslug = IntToString<unsigned long>(playerId & 0xFFFFFFFF, std::dec);
				LOG_DEBUG_GAME(L"Checking if {} is leader of party {}", Gamertag, std::wstring(xuidslug.begin(), xuidslug.end()));
				if (Parties.count(xuidslug) == 1) //Party leader found
				{
					LOG_DEBUG_GAME(L"Party {} adding leader {}", std::wstring(xuidslug.begin(), xuidslug.end()), Gamertag);
					Parties[xuidslug].push_back(NetworkSession::GetPlayerIdByName(Gamertag));
					std::vector<int>::iterator position = std::find(nonPartyPlayers.begin(), nonPartyPlayers.end(), NetworkSession::GetPlayerIdByName(Gamertag));
					if (position != nonPartyPlayers.end())
						nonPartyPlayers.erase(position);
				}
			}
		}
		byte playersPerTeam = (*Memory::GetAddress<BYTE*>(0, 0x534858)) / H2Config_team_enabled_count;
		LOG_DEBUG_GAME(L"Players Per Team: {}", playersPerTeam);
		int currentTeam = 0;
		int currentTeamPlayers = 0;
		std::map<int, int> teamPlayers;
		for (auto i = 0; i < 8; i++) //Detect the first enabled team flag, this is so that if red isn't the first enabled team it doesn't place players there
			if (H2Config_team_flag_array[i]) {
				currentTeam = i;
				break;
			}
		LOG_DEBUG_GAME(L"Starting with team {}", IntToWString<int>(currentTeam, std::dec));
		std::vector<int> sortedPlayers;
		for (const auto& party : Parties)
		{
			LOG_DEBUG_GAME("Setting teams for party {}", party.first);
			for (const int &player : party.second)
			{
				LOG_DEBUG_GAME(L"Setting party player Team for {} to {}", IntToWString<int>(player, std::dec), IntToWString<int>(currentTeam, std::dec));
				sortedPlayers.push_back(player);
				NetworkMessage::SendTeamChange(NetworkSession::GetPeerIndex(player), currentTeam);
				teamPlayers[currentTeam] += 1;
				if (teamPlayers[currentTeam] == playersPerTeam)
					for (auto i = 0; i < 8; i++)
						if (H2Config_team_flag_array[i] && i != currentTeam) {
							if (teamPlayers[i] != playersPerTeam) {
								currentTeam = i;
								break;
							}
						}
			}
			for (auto i = 0; i < 8; i++)
				if (H2Config_team_flag_array[i] && i != currentTeam) {
					if (teamPlayers[i] != playersPerTeam) {
						currentTeam = i;
						break;
					}
				}
		}
		for (const auto &team : teamPlayers)
		{
			LOG_DEBUG_GAME(L"Team {} - {} Players", team.first, team.second);
		}
		for (const auto &player : nonPartyPlayers)
		{
			LOG_DEBUG_GAME(L"Setting Non party player Team for {} to {}", IntToWString<int>(player, std::dec), IntToWString<int>(currentTeam, std::dec));
			sortedPlayers.push_back(player);
			NetworkMessage::SendTeamChange(NetworkSession::GetPeerIndex(player), currentTeam);
			teamPlayers[currentTeam] += 1;
			if (teamPlayers[currentTeam] == playersPerTeam)
				for (auto i = 0; i < 8; i++)
					if (H2Config_team_flag_array[i] && i != currentTeam) {
						if (teamPlayers[i] != playersPerTeam) {
							currentTeam = i;
							break;
						}
					}
		}
		for (const auto &team : teamPlayers)
		{
			LOG_DEBUG_GAME(L"Team {} - {} Players", team.first, team.second);
		}
		int validTeams = 0;
		for (auto i = 0; i < 8; i++)
			if (H2Config_team_flag_array[i]) {
				if (teamPlayers[i] == playersPerTeam)
					validTeams++;
			}

		if (validTeams == H2Config_team_enabled_count)
			teamsAreValidConditionMet = true;
	}
	else
	{
		teamsAreValidConditionMet = true;
	}

	if (teamsAreValidConditionMet && minimumPlayersConditionMet)
	{
		EventHandler::CountdownStartEventExecute(EventExecutionType::execute_after);
		return true;
	}
	else
	{
		return false;
	}
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
		for (auto i = 0; i < NetworkSession::GetPeerCount(); i++)
		{
			ServerConsole::AddVip(NetworkSession::GetPeerPlayerName(i));
			//ServerConsole::SendCommand2(2, L"vip", L"add", NetworkSession::getPeerPlayerName(i));
		}
		//ServerConsole::SendCommand2(1, L"Privacy", L"VIP");
		*Memory::GetAddress<byte*>(0, 0x534850) = 2;
	}
}

void H2MOD::RegisterEvents()
{
	if(!Memory::IsDedicatedServer())
	{
		// Client only callbacks	

	}
	else 
	{
		// Server only callbacks
		
		// Setup Events for H2Config_vip_lock
		if (H2Config_vip_lock)
			EventHandler::register_callback(vip_lock, EventType::gamelifecycle_change, EventExecutionType::execute_after);
	}
	//Things that apply to both
#if EVENT_HANDLER_ENABLE_TEST_EVENTS
	EventHandler::TestEvents();
#endif
}

//Shader LOD Bias stuff
//typedef int(__cdecl p_sub_81A676)(int a1, int a2, float a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10);
//p_sub_81A676* c_sub_81A676;
//
//int __cdecl sub_81A676(int a1, int a2, float a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10)
//{
//	return c_sub_81A676(a1, a2, a3, 4, a5, a6, a7, a8, a9, a10);
//}

// unlocks all single player maps
int __cdecl get_last_single_player_level_id_unlocked_from_profile()
{
	return 805; // return the id of the last level
}

// sword-flying target clear patch
void __cdecl aim_assist_targeting_clear_hook(int target_data)
{
	if (!s_game_globals::game_is_campaign()
		&& !b_XboxTick)
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

	ProjectileFix::ApplyPatches();

	//Guardian Patch
	p_object_cause_damage = Memory::GetAddress<object_cause_damage_t>(0x17AD81, 0x1525E1);
	PatchCall(Memory::GetAddress(0x147DB8, 0x172D55), projectile_collision_object_cause_damage);

	// server/client detours 
	DETOUR_ATTACH(p_player_spawn, Memory::GetAddress<player_spawn_t>(0x55952, 0x5DE4A), OnPlayerSpawn);
	DETOUR_ATTACH(p_player_died, Memory::GetAddress<player_died_t>(0x5587B, 0x5DD73), OnPlayerDeath);
	DETOUR_ATTACH(p_map_cache_load, Memory::GetAddress<map_cache_load_t>(0x8F62, 0x1F35C), OnMapLoad);
	DETOUR_ATTACH(p_update_player_score, Memory::GetAddress<update_player_score_t>(0xD03ED, 0x8C84C), OnPlayerScore);
	DETOUR_ATTACH(p_object_deplete_body_internal, Memory::GetAddress<object_deplete_body_internal_t>(0x17B674, 0x152ED4), OnObjectDamage);

	// bellow hooks applied to specific executables
	if (!Memory::IsDedicatedServer()) {

		LOG_INFO_GAME("{} - applying client hooks", __FUNCTION__);
		/* These hooks are only built for the client, don't enable them on the server! */

		//Shader display hook
		//c_test_hook = Memory::GetAddress<p_test_hook*>(0x1A2AEE);
		//PatchCall(Memory::GetAddress(0x1a10de), test_shader_hook);
		//PatchCall(Memory::GetAddress(0x1a1324), test_hook);
		//PatchCall(Memory::GetAddress(0x1A2FF6), test_shader_hook);
		//PatchCall(Memory::GetAddress(0x1a316B), test_hook);

		//Shader LOD Bias stuff
		//c_sub_81A676 = Memory::GetAddress<p_sub_81A676*>(0x19A676);
		//PatchCall(Memory::GetAddress(0x19AD71), sub_81A676);
		//PatchCall(Memory::GetAddress(0x19ADBC), sub_81A676);

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

		DETOUR_ATTACH(p_fn_c000bd114, Memory::GetAddress<fn_c000bd114_t>(0xbd114), fn_c000bd114_IsSkullEnabled);

		PatchCall(Memory::GetAddress(0x182d6d), GrenadeChainReactIsEngineMPCheck);
		PatchCall(Memory::GetAddress(0x92C05), BansheeBombIsEngineMPCheck);
		PatchCall(Memory::GetAddress(0x13ff75), FlashlightIsEngineSPCheck);

		PatchCall(Memory::GetAddress(0x226702), game_mode_engine_draw_team_indicators);

		// Initialise_tag_loader();
		PlayerControl::ApplyHooks();
		p_set_screen_bounds = Memory::GetAddress<set_screen_bounds_t*>(0x264979);
		// PatchCall(GetAddress(0x25E1E5), set_screen_bounds);
		
		PatchCall(Memory::GetAddressRelative(0x6422C8), get_last_single_player_level_id_unlocked_from_profile);
	}
	else {
		LOG_INFO_GAME("{} - applying dedicated server hooks", __FUNCTION__);

		PatchCall(Memory::GetAddress(0x0, 0xBF43), should_start_pregame_countdown_hook);
		ServerConsole::ApplyHooks();

		DETOUR_ATTACH(p_get_enabled_teams_flags, Memory::GetAddress<get_enabled_teams_flags_t>(0, 0x19698B), get_enabled_teams_flags);
	}

	DETOUR_COMMIT();
}

VOID CALLBACK UpdateDiscordStateTimer(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	update_player_count();
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
		SpecialEvents::Initialize();
		ImGuiHandler::WeaponOffsets::Initialize();
#ifndef NDEBUG
		ObserverMode::Initialize();
#endif
		TEST_N_DEF(PC3);
		if (H2Config_discord_enable && H2GetInstanceId() == 1) {
			// Discord init
			DiscordInterface::SetDetails("Startup");
			DiscordInterface::Init();
			SetTimer(NULL, 0, 5000, UpdateDiscordStateTimer);
		}
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
	h2mod->ApplyHooks();
	h2mod->RegisterEvents();

	Engine::Objects::apply_biped_object_definition_patches();
	StatsHandler::Initialize();

	LOG_INFO_GAME("H2MOD - Initialized");
}

void H2MOD::Deinitialize() 
{
}
