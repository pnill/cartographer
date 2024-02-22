#include "stdafx.h"
#include "Infection.h"

#include "Blam/Cache/TagGroups/item_collection_definition.hpp"
#include "Blam/Cache/TagGroups/vehicle_collection_definition.hpp"
#include "Blam/Engine/game/game.h"
#include "Blam/Engine/game/game_time.h"
#include "Blam/Engine/scenario/scenario.h"
#include "Blam/Engine/Networking/logic/life_cycle_manager.h"
#include "Blam/Engine/Networking/NetworkMessageTypeCollection.h"
#include "Blam/Engine/units/units.h"

#include "H2MOD/Modules/SpecialEvents/SpecialEvents.h"
#include "H2MOD/Modules/Shell/Config.h"
#include "H2MOD/Modules/CustomMenu/CustomLanguage.h"
#include "H2MOD/Modules/EventHandler/EventHandler.hpp"
#include "H2MOD/Tags/MetaLoader/tag_loader.h"
#include "H2MOD/Tags/TagInterface.h"

std::vector<uint64> Infection::zombieIdentifiers;

#define k_humans_team _game_team_red
#define k_zombie_team _game_team_green

bool initialSpawn;
bool infectedPlayed;
int zombiePlayerIndex = NONE;
int last_time_at_game_should_not_end = 0;
const wchar_t* infectionSoundTable[e_language_ids::_lang_id_end][e_infection_sounds::_infection_end]
{
	{SND_INFECTION_EN, SND_INFECTED_EN, SND_NEW_ZOMBIE_EN },
	{SND_INFECTION_JP, SND_INFECTED_JP, SND_NEW_ZOMBIE_JP },
	{SND_INFECTION_GE, SND_INFECTED_GE, SND_NEW_ZOMBIE_GE },
	{SND_INFECTION_FR, SND_INFECTED_FR, SND_NEW_ZOMBIE_FR },
	{SND_INFECTION_ES, SND_INFECTED_ES, SND_NEW_ZOMBIE_ES },
	{SND_INFECTION_IT, SND_INFECTED_IT, SND_NEW_ZOMBIE_IT },
	{SND_INFECTION_KO, SND_INFECTED_KO, SND_NEW_ZOMBIE_KO },
	{SND_INFECTION_CH, SND_INFECTED_CH, SND_NEW_ZOMBIE_CH },
};


int Infection::calculateZombiePlayerIndex()
{
	if (NetworkSession::GetPlayerCount() > 0)
	{
		std::mt19937 mt_rand(rd());
		std::vector<int32> activePlayersIndices = NetworkSession::GetActivePlayerIndicesList();
		std::uniform_int_distribution<int> dist(0, NetworkSession::GetPlayerCount() - 1);
	
		if (activePlayersIndices.empty())
			return NONE;

		int infectedPlayerIndex = activePlayersIndices[dist(mt_rand)];
		LOG_TRACE_GAME(L"[h2mod-infection] random infection player index: {}, with name: {}", infectedPlayerIndex, NetworkSession::GetPlayerName(infectedPlayerIndex));

		return infectedPlayerIndex;
	}

	return NONE;
}

void Infection::sendTeamChange()
{
	if (NetworkSession::LocalPeerIsSessionHost())
	{
		if (NetworkSession::GetPlayerCount() > 0)
		{
			for (int playerIndex = 0; playerIndex < k_maximum_players; playerIndex++)
		{
			if (NetworkSession::PlayerIsActive(playerIndex))
			{
				if (!NetworkSession::IsPeerIndexLocal(NetworkSession::GetPeerIndex(playerIndex))) {
					NetworkMessage::SendTeamChange(NetworkSession::GetPeerIndex(playerIndex), zombiePlayerIndex == playerIndex ? k_zombie_team : k_humans_team);
					LOG_TRACE_GAME(L"[h2mod-infection] sent team change packet to player index: {}, with name: {}, infected?: {}", playerIndex, NetworkSession::GetPlayerName(playerIndex), zombiePlayerIndex == playerIndex ? true : false);
				}
				else if (!Memory::IsDedicatedServer()) {
					bool is_player_zombie = playerIndex == zombiePlayerIndex;
					h2mod->set_local_team_index(0, is_player_zombie ? k_zombie_team : k_humans_team);
					LOG_TRACE_GAME(L"[h2mod-infection] setting local player team index, infected?: {}", is_player_zombie);
				}
			}
			}
		}
	}
}

void Infection::triggerSound(e_infection_sounds sound, int sleep)
{
	const int language_id = *Memory::GetAddress<int*>(0x412818);

	if (infectionSoundTable[language_id][sound] != nullptr)
	{
		LOG_TRACE_GAME(L"[h2mod-infection] Triggering sound {}", infectionSoundTable[language_id][sound]);
		h2mod->custom_sound_play(infectionSoundTable[language_id][sound], sleep);
	}
}

void Infection::InitClient()
{
	LOG_TRACE_GAME("[h2mod-infection] Disabling slayer sounds");
	h2mod->disable_score_announcer_sounds(FLAG(_sound_type_slayer) | ALL_SOUNDS_NO_SLAYER);

	infectedPlayed = false;
	initialSpawn = true;

	//Change Local Player's Team to Human if Not in Green
	//(In case player wants to start as Alpha Zombie leave him green)
	e_game_team team = (e_game_team)s_session_interface_globals::get()->users[0].properties.team_index;
	if (team != k_zombie_team) {
		h2mod->set_local_team_index(0, k_humans_team);
	}
}

void Infection::resetZombiePlayerStatus() {
	zombieIdentifiers.clear();
}

void Infection::setZombiePlayerStatus(uint64 identifier)
{
	zombieIdentifiers.push_back(identifier);
}

void Infection::InitHost() {
	LOG_TRACE_GAME("[h2mod-infection] Host init setting unit speed patch");
	//Applying SpeedCheck fix
	h2mod->set_unit_speed_patch(true);

	LOG_TRACE_GAME("[h2mod-infection] Host init resetting zombie player data status");
	Infection::resetZombiePlayerStatus();
}

bool Infection::shouldEndGame()
{
	int humanCount, zombieCount, playerCount;
	humanCount = zombieCount = playerCount = 0;
	player_iterator player_it;
	while (player_it.get_next_active_player())
	{
		int currentPlayerIndex = player_it.get_current_player_index();
		uint64 playerIdentifier = player_it.get_current_player_id();
		bool isZombie = std::find(Infection::zombieIdentifiers.begin(), Infection::zombieIdentifiers.end(), playerIdentifier) != Infection::zombieIdentifiers.end();

		if (isZombie)
			zombieCount++;
		else
			humanCount++;

		playerCount++;
	}

	return playerCount > 1 && (zombieCount == 0 || humanCount == 0);
}

void Infection::resetWeaponInteractionAndEmblems() {
	LOG_TRACE_GAME("[h2mod-infection] Resetting weapons interactions and emblem visibility");
	h2mod->disable_weapon_pickup(true);
	h2mod->team_player_indicator_visibility(true);
}

void Infection::preSpawnServerSetup() {

	/*
		Game state players should be initialized when we are about to spawn a player
	*/
	
	player_iterator player_it;
	while (player_it.get_next_active_player())
	{
		int currentPlayerIndex = player_it.get_current_player_index();
		uint64 playerIdentifier = player_it.get_current_player_id();
		bool isZombie = std::find(Infection::zombieIdentifiers.begin(), Infection::zombieIdentifiers.end(), playerIdentifier) != Infection::zombieIdentifiers.end();

		bool zombie_team_status_human = isZombie == false && s_player::get_team(currentPlayerIndex) == k_zombie_team;
		if (zombie_team_status_human) {
			// if the player just joined the and he doesn't have zombie status, and his team is green, add him in the array
			setZombiePlayerStatus(playerIdentifier);
			isZombie = true;
		}

		LOG_TRACE_GAME(L"[h2mod-infection] Zombie pre spawn index={}, isZombie={}, playerIdentifier={}, playerName:{}", currentPlayerIndex, isZombie, playerIdentifier, s_player::get_name(currentPlayerIndex));
		if (isZombie) {
			s_player::set_unit_character_type(currentPlayerIndex, _character_type_flood);
			if (s_player::get_team(currentPlayerIndex) != k_zombie_team) {
				if (NetworkSession::LocalPeerIsSessionHost())
					NetworkMessage::SendTeamChange(NetworkSession::GetPeerIndex(currentPlayerIndex), k_zombie_team); // prevent *toxic* kids from switching to humans in the pre-game lobby after joining
			}
		}
		else {
			if (get_current_special_event() == _special_event_halloween && H2Config_spooky_boy)
				s_player::set_unit_character_type(currentPlayerIndex, _character_type_skeleton);
			else
				s_player::set_unit_character_type(currentPlayerIndex, _character_type_spartan);
		}
	}
}

void Infection::setPlayerAsHuman(int playerIndex) {
	if (get_current_special_event() == _special_event_halloween && H2Config_spooky_boy)
		s_player::set_unit_character_type(playerIndex, _character_type_skeleton);
	else
		s_player::set_unit_character_type(playerIndex, _character_type_spartan);
	s_player::set_unit_speed(playerIndex, 1.0f);
}

void Infection::setPlayerAsZombie(int playerIndex) {
	s_player::set_unit_character_type(playerIndex, _character_type_flood);
	s_player::set_unit_speed(playerIndex, 1.1f);

	call_give_player_weapon(playerIndex, e_weapons_datum_index::energy_blade, 1);
}

void Infection::onGameTick()
{
	if(get_game_life_cycle() == _life_cycle_in_game && NetworkSession::LocalPeerIsSessionHost())
	{
		if (time_globals::get_game_time() > 0)
		{
			bool should_end_game = shouldEndGame();

			// check if the current game should be ended
			if (!should_end_game)
			{
				// and update the time if it shouldn't be
				last_time_at_game_should_not_end = time_globals::get_game_time();
			}

			// check the difference between game time now
			// if the time wasn't updated for more than 5 seconds, end the game
			if (game_ticks_to_seconds(time_globals::get_game_time() - last_time_at_game_should_not_end) > 5.0f)
			{
				NetworkSession::EndGame();
			}
		}
	}
}

void Infection::removeUnwantedItems()
{
	const datum shotgun_ammo_equip_datum = tags::find_tag(_tag_group_equipment, "objects\\powerups\\shotgun_ammo\\shotgun_ammo");

	auto itemcollections = tags::find_tags(_tag_group_item_collection);
	for (auto it = itemcollections.begin(); it != itemcollections.end(); it++)
	{
		std::string item_name = tags::get_tag_name(it->first);
		if (item_name.find("multiplayer\\powerups") != std::string::npos ||
			item_name == "multiplayer\\single_weapons\\frag_grenades" ||
			item_name == "multiplayer\\single_weapons\\plasma_grenades")
		{
			auto itmc = tags::get_tag_fast<s_item_collection_group_definition>(it->first);

			for (int i = 0; i < itmc->item_permutations.count; i++)
			{
				itmc->item_permutations[i]->item.group.group = _tag_group_equipment;
				itmc->item_permutations[i]->item.index = shotgun_ammo_equip_datum;
			}
		}
	}

	//Replace vehicles with shotgun ammo
	scenario* scenario_definition = get_global_scenario();
	for (DWORD i = 0; i < scenario_definition->netgame_equipment.count; i++)
	{
		scenario_netgame_equipment* netgame_equipment = scenario_definition->netgame_equipment[i];
		if (netgame_equipment->item_vehicle_collection.group.group == _tag_group_vehicle_collection)
		{
			netgame_equipment->classification = netgame_item_classification_powerup;
			netgame_equipment->item_vehicle_collection.group.group = _tag_group_item_collection;
			netgame_equipment->item_vehicle_collection.index = NONE;
		}
	}
}

void Infection::Initialize()
{
	LOG_TRACE_GAME("{} - infection initialization!");
	removeUnwantedItems();

	if (!Memory::IsDedicatedServer())
		Infection::InitClient();

	if (NetworkSession::LocalPeerIsSessionHost()) {
		Infection::InitHost();

		last_time_at_game_should_not_end = 0;
		zombiePlayerIndex = Infection::calculateZombiePlayerIndex();
		EventHandler::register_callback(onGameTick, EventType::game_loop, EventExecutionType::execute_after);

		LOG_TRACE_GAME("[h2mod-infection] Peer host calculated zombie index {}", zombiePlayerIndex);
		if (zombiePlayerIndex == NONE) {
			LOG_TRACE_GAME("[h2mod-infection] Failed selecting a zombie!");
		}
		else {
			LOG_TRACE_GAME("[h2mod-infection] Peer host setting player as human");
			//send out the team change packets to peers
			Infection::sendTeamChange();
			Infection::setZombiePlayerStatus(NetworkSession::GetPlayerId(zombiePlayerIndex));
		}
	}
}

void Infection::Dispose()
{
	LOG_TRACE_GAME("{} - infection dispose!");

	if(NetworkSession::LocalPeerIsSessionHost())
	{
		EventHandler::remove_callback(onGameTick, EventType::game_loop, EventExecutionType::execute_after);
	}

	Infection::resetWeaponInteractionAndEmblems();
	if (!game_is_predicted()) {
		h2mod->set_unit_speed_patch(false);
	}
}

CustomVariantId Infection::GetVariantId()
{
	return CustomVariantId::_id_infection;
}

void Infection::OnMapLoad(ExecTime execTime, s_game_options* options)
{
	switch (execTime)
	{
	case ExecTime::_preEventExec:
		break;

	case ExecTime::_postEventExec:
		switch (options->game_mode)
		{
			// cleanup when loading main menu
		case _game_mode_multiplayer:
			this->Initialize();
			break;
			/*case _main_menu:
				this->Dispose();
				break;*/
		default:
			break;
		}

		break;

	case ExecTime::_ExecTimeUnknown:
	default:
		LOG_TRACE_GAME("{} - unknown execTime", __FUNCTION__);
		break;
	}
}

void Infection::OnPlayerDeath(ExecTime execTime, datum player_index)
{
	int absPlayerIdx = DATUM_INDEX_TO_ABSOLUTE_INDEX(player_index);
	datum playerUnitDatum = s_player::get_unit_index(player_index);

	switch (execTime)
	{
	case ExecTime::_preEventExec:
		// to note after the original function executes, the controlled unit by this player is set to NONE

		if (!Memory::IsDedicatedServer())
		{
			if (s_player::get_team(player_index) != k_zombie_team) {
				//if we have a valid object and the object is not on the zombie team
				uint64 player_id = s_player::get_id(player_index_from_user_index(0));

				LOG_TRACE_GAME(L"[h2mod-infection] Infected local player, Name={}, identifier={}", s_player::get_name(player_index_from_user_index(0)), player_id);

				// check if the player being infected is local
				if (player_id == s_player::get_id(player_index_from_user_index(0))) {
					LOG_TRACE_GAME("[h2mod-infection] Setting player as zombie");
					h2mod->set_local_team_index(0, k_zombie_team);
					s_player::set_unit_character_type(player_index, _character_type_flood);
				}
				else {
					//if not, then this is a new zombie
					LOG_TRACE_GAME(L"[h2mod-infection] Player died, name={}, identifer={}", s_player::get_name(player_index), player_id);
					Infection::triggerSound(_snd_new_zombie, 1000);
				}
			}
		}

		// host code
		if (!game_is_predicted())
		{
			void* unit_object = object_try_and_get_and_verify_type(playerUnitDatum, FLAG(_object_type_biped));
			if (unit_object) {
				if (h2mod->get_unit_team_index(playerUnitDatum) != k_zombie_team) {
					Infection::setZombiePlayerStatus(s_player::get_id(player_index));
				}
				else {
					// take away zombie's weapons
					unit_delete_all_weapons(playerUnitDatum);
				}
			}
		}
		break;

	case ExecTime::_postEventExec:
		break;

	case ExecTime::_ExecTimeUnknown:
	default:
		LOG_TRACE_GAME("{} - unknown execTime", __FUNCTION__);
		break;
	}
}

void Infection::OnPlayerSpawn(ExecTime execTime, datum playerIdx)
{
	int absPlayerIdx = DATUM_INDEX_TO_ABSOLUTE_INDEX(playerIdx);
	datum playerUnitDatum = s_player::get_unit_index(playerIdx);

	switch (execTime)
	{
		// prespawn handler
	case ExecTime::_preEventExec:
		// to note after the original function executes, the controlled unit by this player is set to NONE

		if (!Memory::IsDedicatedServer())
		{
			uint64 playerIdentifier = s_player::get_id(playerIdx);
			LOG_TRACE_GAME(L"[h2mod-infection] Client pre spawn, playerIndex={}, playerIdentifier={}, localPlayerName={}", absPlayerIdx, playerIdentifier, s_player::get_name(player_index_from_user_index(0)));
			//If player being spawned is LocalUser/Player
			if (playerIdentifier == s_player::get_id(player_index_from_user_index(0)))
			{
				e_game_team team = (e_game_team)s_session_interface_globals::get()->users[0].properties.team_index;

				LOG_TRACE_GAME("[h2mod-infection] Client pre spawn, found local player, current team = {}", (int16)team);
				//Change biped if LocalUser is in GreenTeam
				if (team == k_zombie_team)
				{
					LOG_TRACE_GAME("[h2mod-infection] Client is infected! switching bipeds: {}", absPlayerIdx);
					s_player::set_unit_character_type(playerIdx, _character_type_flood);
				}
			}
		}

		if (!game_is_predicted())
			Infection::preSpawnServerSetup();
		break;

		// postspawn handler
	case ExecTime::_postEventExec:

		// client only
		if (!Memory::IsDedicatedServer())
		{
			uint64 playerIdentifier = s_player::get_id(playerIdx);
			// if player being spawned is local
			if (playerIdentifier == s_player::get_id(player_index_from_user_index(0))) {
				if (initialSpawn == true) {
					//start of zombie match
					Infection::triggerSound(e_infection_sounds::_snd_infection, 1000);
					initialSpawn = false;
				}

				e_game_team team = (e_game_team)s_session_interface_globals::get()->users[0].properties.team_index;

				if (team == k_zombie_team && infectedPlayed == false) {
					Infection::triggerSound(_snd_infected, 500);
					infectedPlayed = true;
				}

				if (team == k_humans_team) {
					h2mod->disable_weapon_pickup(true);
					h2mod->team_player_indicator_visibility(false);
				}
				else if (team == k_zombie_team) {
					s_player::set_unit_character_type(playerIdx, _character_type_flood);
					h2mod->disable_weapon_pickup(false);
					h2mod->team_player_indicator_visibility(true);
				}
			}
		}

		// host only (both client/dedicated server)
		if (!game_is_predicted())
		{
			LOG_TRACE_GAME("[h2mod-infection] Spawn player server index={}", absPlayerIdx);
			void* unit_object = object_try_and_get_and_verify_type(playerUnitDatum, FLAG(_object_type_biped));
			if (unit_object) {
				//if the unit_object data pointer is not nullptr, the spawned object is "alive"

				LOG_TRACE_GAME("[h2mod-infection] Spawn player server index={}, unit team index={}", absPlayerIdx, h2mod->get_unit_team_index(playerUnitDatum));
				if (h2mod->get_unit_team_index(playerUnitDatum) == k_humans_team) {
					Infection::setPlayerAsHuman(absPlayerIdx);
				}

				if (h2mod->get_unit_team_index(playerUnitDatum) == k_zombie_team) {
					Infection::setPlayerAsZombie(absPlayerIdx);
				}
			}
		}

		break;

	case ExecTime::_ExecTimeUnknown:
	default:
		LOG_TRACE_GAME("{} - unknown execTime", __FUNCTION__);
		break;
	}
}
