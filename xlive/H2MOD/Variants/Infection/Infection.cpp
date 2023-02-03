#include "stdafx.h"

#include "Infection.h"
#include "Blam/Engine/Game/GameGlobals.h"
#include "Blam/Engine/Networking/NetworkMessageTypeCollection.h"
#include "Blam/Cache/TagGroups/item_collection_definition.hpp"
#include "Blam/Cache/TagGroups/scenario_definition.hpp"
#include "Blam/Cache/TagGroups/vehicle_collection_definition.hpp"
#include "Blam/Engine/Game/GameTimeGlobals.h"
#include "H2MOD/Engine/Engine.h"
#include "H2MOD/Modules/SpecialEvents/SpecialEvents.h"
#include "H2MOD/Modules/Shell/Config.h"
#include "H2MOD/Modules/CustomMenu/CustomLanguage.h"
#include "H2MOD/Modules/EventHandler/EventHandler.hpp"
#include "H2MOD/Modules/PlayerRepresentation/PlayerRepresentation.h"
#include "H2MOD/Tags/MetaLoader/tag_loader.h"
#include "H2MOD/Tags/TagInterface.h"

std::vector<unsigned long long> Infection::zombieIdentifiers;

#define HUMAN_TEAM _object_team_red
#define ZOMBIE_TEAM _object_team_green

bool initialSpawn;
bool infectedPlayed;
int zombiePlayerIndex = NONE;
int ticks_without_any_humans = 0;
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
		std::vector<int> activePlayersIndices = NetworkSession::GetActivePlayerIndicesList();
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
			for (int playerIndex = 0; playerIndex < ENGINE_MAX_PLAYERS; playerIndex++)
			{
				if (NetworkSession::PlayerIsActive(playerIndex))
				{
					if (!NetworkSession::PeerIndexLocal(NetworkSession::GetPeerIndex(playerIndex))) {
						NetworkMessage::SendTeamChange(NetworkSession::GetPeerIndex(playerIndex), zombiePlayerIndex == playerIndex ? ZOMBIE_TEAM : HUMAN_TEAM);
						LOG_TRACE_GAME(L"[h2mod-infection] sent team change packet to player index: {}, with name: {}, infected?: {}", playerIndex, NetworkSession::GetPlayerName(playerIndex), zombiePlayerIndex == playerIndex ? true : false);
					}
					else if (!Memory::IsDedicatedServer()) {
						h2mod->set_local_team_index(0, zombiePlayerIndex == playerIndex ? ZOMBIE_TEAM : HUMAN_TEAM);
						LOG_TRACE_GAME(L"[h2mod-infection] setting local player team index, infected?: {}", zombiePlayerIndex == playerIndex ? true : false);
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
	h2mod->disable_sounds(FLAG(_sound_type_slayer) | ALL_SOUNDS_NO_SLAYER);

	infectedPlayed = false;
	initialSpawn = true;

	//Change Local Player's Team to Human if Not in Green
	//(In case player wants to start as Alpha Zombie leave him green)
	if (h2mod->get_local_team_index() != ZOMBIE_TEAM) {
		h2mod->set_local_team_index(0, HUMAN_TEAM);
	}
}

void Infection::resetZombiePlayerStatus() {
	zombieIdentifiers.clear();
}

void Infection::setZombiePlayerStatus(unsigned long long identifier)
{
	zombieIdentifiers.push_back(identifier);
}

void Infection::InitHost() {
	LOG_TRACE_GAME("[h2mod-infection] Host init setting unit speed patch");
	//Applying SpeedCheck fix
	h2mod->set_unit_speed_patch(true);

	// Remove unwanted items in infection
	auto itemcollections = tags::find_tags(blam_tag::tag_group_type::itemcollection);
	for each (auto itemcollection in itemcollections)
	{
		std::string item_name = tags::get_tag_name(itemcollection.first);
		if (item_name.find("multiplayer\\powerups") != std::string::npos ||
			item_name == "multiplayer\\single_weapons\\frag_grenades" ||
			item_name == "multiplayer\\single_weapons\\plasma_grenades")
		{
			auto itmc = tags::get_tag_fast<s_item_collection_group_definition>(itemcollection.first);

			datum shotgun_ammo_equip_datum = tags::find_tag(blam_tag::tag_group_type::equipment, "objects\\powerups\\shotgun_ammo\\shotgun_ammo");
			for (int i = 0; i < itmc->item_permutations.size; i++)
			{
				itmc->item_permutations[i]->item.TagIndex = shotgun_ammo_equip_datum;
				itmc->item_permutations[i]->item.TagGroup = blam_tag::tag_group_type::equipment;
			}
		}
	}
	
	//Replace vehicles with shotguns
	auto scenarios = tags::find_tags(blam_tag::tag_group_type::scenario);
	for(auto &scenario_ : scenarios)
	{
		auto scenario = tags::get_tag<blam_tag::tag_group_type::scenario, s_scenario_group_definition>(scenario_.first);
		for(auto i = 0; i < scenario->netgame_equipment.size; i++)
		{
			auto equipment = scenario->netgame_equipment[i];
			if(equipment->itemvehicle_collection.TagGroup.tag_type == blam_tag::tag_group_type::vehiclecollection)
			{
				equipment->itemvehicle_collection.TagGroup = blam_tag::tag_group_type::itemcollection;
				equipment->itemvehicle_collection.TagIndex = DATUM_INDEX_NONE;
			}
		}
	}

	LOG_TRACE_GAME("[h2mod-infection] Host init resetting zombie player data status");
	Infection::resetZombiePlayerStatus();
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
	int humanCount = 0;
	int playerCount = 0;
	PlayerIterator playerIt;
	while (playerIt.get_next_active_player())
	{
		int currentPlayerIndex = playerIt.get_current_player_index();
		unsigned long long playerIdentifier = playerIt.get_current_player_id();
		bool isZombie = std::find(Infection::zombieIdentifiers.begin(), Infection::zombieIdentifiers.end(), playerIdentifier) != Infection::zombieIdentifiers.end();
		if (s_player::GetTeam(currentPlayerIndex) == ZOMBIE_TEAM && isZombie == false) {
			// if the player just joined the and he doesn't have zombie status, and his team is green, add him in the array
			setZombiePlayerStatus(playerIdentifier);
			isZombie = true;
		}

		LOG_TRACE_GAME(L"[h2mod-infection] Zombie pre spawn index={}, isZombie={}, playerIdentifier={}, playerName:{}", currentPlayerIndex, isZombie, playerIdentifier, s_player::GetName(currentPlayerIndex));
		if (isZombie) {
			s_player::SetUnitBipedType(currentPlayerIndex, s_player::e_character_type::Flood);
			if (s_player::GetTeam(currentPlayerIndex) != ZOMBIE_TEAM) {
				if (NetworkSession::LocalPeerIsSessionHost())
					NetworkMessage::SendTeamChange(NetworkSession::GetPeerIndex(currentPlayerIndex), ZOMBIE_TEAM); // prevent *toxic* kids from switching to humans in the pre-game lobby after joining
			}
		}
		else {
			if (SpecialEvents::getCurrentEvent() == SpecialEvents::_halloween && H2Config_spooky_boy)
				s_player::SetUnitBipedType(currentPlayerIndex, s_player::e_character_type::Skeleton);
			else
				s_player::SetUnitBipedType(currentPlayerIndex, s_player::e_character_type::Spartan);
			humanCount++;
		}
		playerCount++;
	}

	// end the game if all humans are dead
	if (humanCount == 0 && playerCount > 1)
		NetworkSession::EndGame();
}

void Infection::setPlayerAsHuman(int playerIndex) {
	if (SpecialEvents::getCurrentEvent() == SpecialEvents::_halloween && H2Config_spooky_boy)
		s_player::SetUnitBipedType(playerIndex, s_player::e_character_type::Skeleton);
	else
		s_player::SetUnitBipedType(playerIndex, s_player::e_character_type::Spartan);
	s_player::SetBipedSpeed(playerIndex, 1.0f);
}

void Infection::setPlayerAsZombie(int playerIndex) {
	s_player::SetUnitBipedType(playerIndex, s_player::e_character_type::Flood);
	s_player::SetBipedSpeed(playerIndex, 1.1f);

	call_give_player_weapon(playerIndex, e_weapons_datum_index::energy_blade, 1);
}

void Infection::onGameTick()
{
	if(Engine::get_game_life_cycle() == _life_cycle_in_game && NetworkSession::LocalPeerIsSessionHost())
	{
		PlayerIterator playerIt;
		int human_count = 0;
		int zombie_count = 0;
		while (playerIt.get_next_active_player())
		{
			XUID playerIdentifier = playerIt.get_current_player_id();
			bool isZombie = std::find(Infection::zombieIdentifiers.begin(), Infection::zombieIdentifiers.end(), playerIdentifier) != Infection::zombieIdentifiers.end();

			if (isZombie)
				zombie_count++;
			else
				human_count++;

		}
		if(human_count == 0 && zombie_count >= 1)
		{
			ticks_without_any_humans++;
		}
		else
		{
			ticks_without_any_humans = 0;
		}

		if(time_globals::ticks_to_seconds(ticks_without_any_humans) > 5.0f)
		{
			NetworkSession::EndGame();
		}
	}
}


void Infection::Initialize()
{
	LOG_TRACE_GAME("{} - infection initialization!");

	if (!Memory::IsDedicatedServer())
		Infection::InitClient();

	if (NetworkSession::LocalPeerIsSessionHost()) {
		Infection::InitHost();

		ticks_without_any_humans = 0;
		zombiePlayerIndex = Infection::calculateZombiePlayerIndex();
		EventHandler::register_callback(onGameTick, EventType::game_loop, EventExecutionType::execute_before);

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
		EventHandler::remove_callback(onGameTick, EventType::game_loop, EventExecutionType::execute_before);
	}

	Infection::resetWeaponInteractionAndEmblems();
	if (!s_game_globals::game_is_predicted()) {
		h2mod->set_unit_speed_patch(false);
	}
}

CustomVariantId Infection::GetVariantId()
{
	return CustomVariantId::_id_infection;
}

void Infection::OnMapLoad(ExecTime execTime, s_game_options* gameOptions)
{
	switch (execTime)
	{
	case ExecTime::_preEventExec:
		break;

	case ExecTime::_postEventExec:
		switch (h2mod->GetEngineType())
		{
			// cleanup when loading main menu
		case _multiplayer:
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

void Infection::OnPlayerDeath(ExecTime execTime, datum playerIdx)
{
	int absPlayerIdx = DATUM_INDEX_TO_ABSOLUTE_INDEX(playerIdx);
	datum playerUnitDatum = s_player::GetPlayerUnitDatumIndex(absPlayerIdx);

	switch (execTime)
	{
	case ExecTime::_preEventExec:
		// to note after the original function executes, the controlled unit by this player is set to NONE

		if (!Memory::IsDedicatedServer())
		{
			if (s_player::GetTeam(absPlayerIdx) != ZOMBIE_TEAM) {
				//if we have a valid object and the object is not on the zombie team
				unsigned long long playerIdentifier = s_player::GetId(absPlayerIdx);

				LOG_TRACE_GAME(L"[h2mod-infection] Infected local player, Name={}, identifier={}", h2mod->get_local_player_name(0), playerIdentifier);

				// check if the player being infected is local
				if (playerIdentifier == s_player::GetId(DATUM_INDEX_TO_ABSOLUTE_INDEX(h2mod->get_player_datum_index_from_controller_index(0)))) {
					LOG_TRACE_GAME("[h2mod-infection] Setting player as zombie");
					h2mod->set_local_team_index(0, ZOMBIE_TEAM);
					s_player::SetUnitBipedType(absPlayerIdx, s_player::e_character_type::Flood);
				}
				else {
					//if not, then this is a new zombie
					LOG_TRACE_GAME(L"[h2mod-infection] Player died, name={}, identifer={}", s_player::GetName(absPlayerIdx), playerIdentifier);
					Infection::triggerSound(_snd_new_zombie, 1000);
				}
			}
		}

		// host code
		if (!s_game_globals::game_is_predicted())
		{
			char* unit_object = (char*)object_try_and_get_and_verify_type(playerUnitDatum, FLAG(e_object_type::biped));
			if (unit_object) {
				if (h2mod->get_unit_team_index(playerUnitDatum) != ZOMBIE_TEAM) {
					Infection::setZombiePlayerStatus(s_player::GetId(absPlayerIdx));
				}
				else {
					// take away zombie's weapons
					Engine::Unit::remove_equipment(playerUnitDatum);
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
	datum playerUnitDatum = s_player::GetPlayerUnitDatumIndex(absPlayerIdx);

	switch (execTime)
	{
		// prespawn handler
	case ExecTime::_preEventExec:
		// to note after the original function executes, the controlled unit by this player is set to NONE

		if (!Memory::IsDedicatedServer())
		{
			unsigned long long playerIdentifier = s_player::GetId(absPlayerIdx);
			LOG_TRACE_GAME(L"[h2mod-infection] Client pre spawn, playerIndex={}, playerIdentifier={}, localPlayerName={}", absPlayerIdx, playerIdentifier, h2mod->get_local_player_name(0));
			//If player being spawned is LocalUser/Player
			if (playerIdentifier == s_player::GetId(DATUM_INDEX_TO_ABSOLUTE_INDEX(h2mod->get_player_datum_index_from_controller_index(0))))
			{
				LOG_TRACE_GAME("[h2mod-infection] Client pre spawn, found local player, current team = {}", h2mod->get_local_team_index());
				//Change biped if LocalUser is in GreenTeam
				if (h2mod->get_local_team_index() == ZOMBIE_TEAM)
				{
					LOG_TRACE_GAME("[h2mod-infection] Client is infected! switching bipeds: {}", absPlayerIdx);
					s_player::SetUnitBipedType(absPlayerIdx, s_player::e_character_type::Flood);
				}
			}
		}

		if (!s_game_globals::game_is_predicted())
			Infection::preSpawnServerSetup();
		break;

		// postspawn handler
	case ExecTime::_postEventExec:

		// client only
		if (!Memory::IsDedicatedServer())
		{
			unsigned long long playerIdentifier = s_player::GetId(absPlayerIdx);
			// if player being spawned is local
			if (playerIdentifier == s_player::GetId(DATUM_INDEX_TO_ABSOLUTE_INDEX(h2mod->get_player_datum_index_from_controller_index(0)))) {
				if (initialSpawn == true) {
					//start of zombie match
					Infection::triggerSound(e_infection_sounds::_snd_infection, 1000);
					initialSpawn = false;
				}

				if (h2mod->get_local_team_index() == ZOMBIE_TEAM && infectedPlayed == false) {
					Infection::triggerSound(_snd_infected, 500);
					infectedPlayed = true;
				}

				if (h2mod->get_local_team_index() == HUMAN_TEAM) {
					h2mod->disable_weapon_pickup(true);
					h2mod->team_player_indicator_visibility(false);
				}

				else if (h2mod->get_local_team_index() == ZOMBIE_TEAM) {
					s_player::SetUnitBipedType(absPlayerIdx, s_player::e_character_type::Flood);
					h2mod->disable_weapon_pickup(false);
					h2mod->team_player_indicator_visibility(true);
				}
			}
		}

		// host only (both client/dedicated server)
		if (!s_game_globals::game_is_predicted())
		{
			LOG_TRACE_GAME("[h2mod-infection] Spawn player server index={}", absPlayerIdx);
			char* unit_object = (char*)object_try_and_get_and_verify_type(playerUnitDatum, FLAG(e_object_type::biped));
			if (unit_object) {
				//if the unit_object data pointer is not nullptr, the spawned object is "alive"

				LOG_TRACE_GAME("[h2mod-infection] Spawn player server index={}, unit team index={}", absPlayerIdx, h2mod->get_unit_team_index(playerUnitDatum));
				if (h2mod->get_unit_team_index(playerUnitDatum) == HUMAN_TEAM) {
					Infection::setPlayerAsHuman(absPlayerIdx);
				}

				if (h2mod->get_unit_team_index(playerUnitDatum) == ZOMBIE_TEAM) {
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
