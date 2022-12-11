#include "stdafx.h"
#include "Infection.h"

#include "H2MOD.h"
#include "Blam/Cache/TagGroups/item_collection_definition.hpp"
#include "Blam/Cache/TagGroups/scenario_definition.hpp"
#include "Blam/Cache/TagGroups/vehicle_collection_definition.hpp"
#include "Blam/Engine/Game/GameGlobals.h"
#include "Blam/Engine/Networking/NetworkMessageTypeCollection.h"
#include "Blam/Engine/Players/LocalPlayers.h"
#include "H2MOD/Engine/Engine.h"
#include "H2MOD/Modules/SpecialEvents/SpecialEvents.h"
#include "H2MOD/Modules/Shell/Config.h"
#include "H2MOD/Modules/PlayerRepresentation/PlayerRepresentation.h"
#include "H2MOD/Tags/MetaLoader/tag_loader.h"
#include "H2MOD/Tags/TagInterface.h"

#define HUMAN_TEAM e_object_team::Red
#define ZOMBIE_TEAM e_object_team::Green

bool b_initialSpawn;
bool b_infectedPlayed;
int zombiePlayerIndex = NONE;
unsigned long long zombieIdentifiers[ENGINE_MAX_PLAYERS] = { 0 };
const wchar_t* infectionSoundTable[e_language_ids::_end][e_infection_sounds::_infection_end]
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
	std::vector<int> activePlayersIndices = NetworkSession::GetActivePlayerIndicesList();
	if (NetworkSession::GetPlayerCount() <= 0 || activePlayersIndices.empty()) { return NONE; }
	
	std::mt19937 mt_rand(rd());
	std::uniform_int_distribution<int> dist(0, NetworkSession::GetPlayerCount() - 1);
	
	int infectedPlayerIndex = activePlayersIndices[dist(mt_rand)];
	LOG_TRACE_GAME(L"[h2mod-infection] random infection player index: {}, with name: {}", infectedPlayerIndex, NetworkSession::GetPlayerName(infectedPlayerIndex));

	return infectedPlayerIndex;
}

void Infection::sendTeamChange()
{
	if (!NetworkSession::LocalPeerIsSessionHost() || NetworkSession::GetPlayerCount() <= 0) { return; }

	for (int absPlayerIdx = 0; absPlayerIdx < ENGINE_MAX_PLAYERS; absPlayerIdx++)
	{
		if (NetworkSession::PlayerIsActive(absPlayerIdx))
		{
			if (!NetworkSession::PeerIndexLocal(NetworkSession::GetPeerIndex(absPlayerIdx))) 
			{
				NetworkMessage::SendTeamChange(NetworkSession::GetPeerIndex(absPlayerIdx), zombiePlayerIndex == absPlayerIdx ? ZOMBIE_TEAM : HUMAN_TEAM);
				LOG_TRACE_GAME(L"[h2mod-infection] sent team change packet to player index: {}, with name: {}, infected?: {}", 
					absPlayerIdx, NetworkSession::GetPlayerName(absPlayerIdx), zombiePlayerIndex == absPlayerIdx ? true : false);
			}
			else if (!Memory::IsDedicatedServer())
			{
				for (byte i = 0; i < ENGINE_MAX_LOCAL_PLAYERS; i++)
				{
					if (absPlayerIdx == DATUM_INDEX_TO_ABSOLUTE_INDEX(h2mod->get_player_datum_index_from_controller_index(i)))
					{
						h2mod->set_local_team_index(i, zombiePlayerIndex == absPlayerIdx ? ZOMBIE_TEAM : HUMAN_TEAM);
						LOG_TRACE_GAME(L"[h2mod-infection] setting local player team index, infected?: {}", zombiePlayerIndex == absPlayerIdx ? true : false);
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
	b_infectedPlayed = false;
	b_initialSpawn = true;
	const int language_id = *Memory::GetAddress<int*>(0x412818);

	LOG_TRACE_GAME("[h2mod-infection] Disabling slayer sounds");
	h2mod->disable_sounds(FLAG(_sound_type_slayer) | ALL_SOUNDS_NO_SLAYER);

	//Change Local Player's Team to Human if Not in Green
	//(In case player wants to start as Alpha Zombie leave him green)
	for (byte i = 0; i < ENGINE_MAX_LOCAL_PLAYERS; i++)
	{
		if (h2mod->get_local_team_index(i) != ZOMBIE_TEAM) { h2mod->set_local_team_index(i, HUMAN_TEAM); }
	}
}

void Infection::resetZombiePlayerStatus() 
{
	for (byte i = 0; i < ENGINE_MAX_PLAYERS; i++)
	{
		zombieIdentifiers[i] = 0;
	}
}

void Infection::setZombiePlayerStatus(unsigned long long identifier, unsigned long absPlayerIdx)
{
	zombieIdentifiers[absPlayerIdx] = identifier;
}

void Infection::InitHost() {
	LOG_TRACE_GAME("[h2mod-infection] Host init setting unit speed patch");
	// Applying SpeedCheck fix
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

void Infection::preSpawnServerSetup() 
{
	// Game state players should be initialized when we are about to spawn a player
	int humanCount = 0;
	int playerCount = 0;
	PlayerIterator playerIt;
	while (playerIt.get_next_active_player())
	{
		int absPlayerIdx = playerIt.get_current_player_index();
		unsigned long long playerIdentifier = playerIt.get_current_player_id();

		bool isZombie = (zombieIdentifiers[absPlayerIdx] == playerIdentifier);

		// if the player just joined the game and he doesn't have zombie status, and his team is green, add him in the array
		if (s_player::GetTeam(absPlayerIdx) == ZOMBIE_TEAM && isZombie == false)
		{
			setZombiePlayerStatus(playerIdentifier, absPlayerIdx);
			isZombie = true;
		}

		LOG_TRACE_GAME(L"[h2mod-infection] Zombie pre spawn index={}, isZombie={}, playerIdentifier={}, playerName:{}", 
			absPlayerIdx, isZombie, playerIdentifier, s_player::GetName(absPlayerIdx));
		if (isZombie) 
		{
			s_player::SetUnitBipedType(absPlayerIdx, s_player::e_character_type::Flood);
			// prevent *toxic* kids from switching to humans in the pre-game lobby after joining
			if (s_player::GetTeam(absPlayerIdx) != ZOMBIE_TEAM && NetworkSession::LocalPeerIsSessionHost())
			{
				NetworkMessage::SendTeamChange(NetworkSession::GetPeerIndex(absPlayerIdx), ZOMBIE_TEAM);
			}
		}
		else 
		{
			setPlayerAsHuman(absPlayerIdx);
			humanCount++;
		}
		playerCount++;
	}

	// end the game if all humans are dead
	if (humanCount == 0 && playerCount > 1)
		NetworkSession::EndGame();
}

void Infection::setPlayerAsHuman(int playerIndex) 
{
	(SpecialEvents::getCurrentEvent() == SpecialEvents::_halloween && H2Config_spooky_boy
		? s_player::SetUnitBipedType(playerIndex, s_player::e_character_type::Skeleton) 
		: s_player::SetUnitBipedType(playerIndex, s_player::e_character_type::Spartan));
	
	s_player::SetBipedSpeed(playerIndex, 1.0f);
}

void Infection::setPlayerAsZombie(int playerIndex) 
{
	s_player::SetUnitBipedType(playerIndex, s_player::e_character_type::Flood);
	s_player::SetBipedSpeed(playerIndex, 1.1f);

	call_give_player_weapon(playerIndex, e_weapons_datum_index::energy_blade, true);
}

void Infection::Initialize()
{
	LOG_TRACE_GAME("{} - infection initialization!");

	if (!Memory::IsDedicatedServer())
		Infection::InitClient();

	if (NetworkSession::LocalPeerIsSessionHost()) {
		Infection::InitHost();

		zombiePlayerIndex = Infection::calculateZombiePlayerIndex();
		LOG_TRACE_GAME("[h2mod-infection] Peer host calculated zombie index {}", zombiePlayerIndex);
		if (zombiePlayerIndex == NONE) {
			LOG_TRACE_GAME("[h2mod-infection] Failed selecting a zombie!");
		}
		else {
			LOG_TRACE_GAME("[h2mod-infection] Peer host setting player as human");
			// send out the team change packets to peers
			Infection::sendTeamChange();
			Infection::setZombiePlayerStatus(NetworkSession::GetPlayerId(zombiePlayerIndex), zombiePlayerIndex);
		}
	}
}

void Infection::Dispose()
{
	LOG_TRACE_GAME("{} - infection dispose!");

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
		case _multiplayer:
			this->Initialize();
			break;
		case _main_menu:
			break;
		default:
			break;
		}
		break;

	default:
		LOG_TRACE_GAME("{} - unknown execTime", __FUNCTION__);
		break;
	}
}

void Infection::PreOnPlayerDeathClientCode(const unsigned long absPlayerIdx)
{
	bool b_local_infection = false;
	if (s_player::GetTeam(absPlayerIdx) != ZOMBIE_TEAM) 
	{
		//if we have a valid object and the object is not on the zombie team
		LOG_TRACE_GAME(L"[h2mod-infection] Infected player, Name={}, identifier={}", s_player::GetName(absPlayerIdx));

		for (byte i = 0; i < ENGINE_MAX_LOCAL_PLAYERS; i++)
		{
			// check if the player being infected is local
			if (absPlayerIdx == DATUM_INDEX_TO_ABSOLUTE_INDEX(h2mod->get_player_datum_index_from_controller_index(i)))
			{
				LOG_TRACE_GAME("[h2mod-infection] Setting player as zombie");
				h2mod->set_local_team_index(i, ZOMBIE_TEAM);
				s_player::SetTeam(absPlayerIdx, ZOMBIE_TEAM);
				s_player::SetUnitBipedType(absPlayerIdx, s_player::e_character_type::Flood);
				b_local_infection = true;
				break;
			}
		}
		if (b_local_infection) 
		{
			//if not, then this is a new zombie
			LOG_TRACE_GAME(L"[h2mod-infection] Player died, name={}", s_player::GetName(absPlayerIdx));
			Infection::triggerSound(_snd_new_zombie, 1000);
		}
	}
}

void Infection::PreOnPlayerDeathHostCode(const unsigned long absPlayerIdx)
{
	datum playerUnitDatum = s_player::GetPlayerUnitDatumIndex(absPlayerIdx);
	if (playerUnitDatum != DATUM_INDEX_NONE) 
	{
		if (s_player::GetTeam(playerUnitDatum) != ZOMBIE_TEAM) 
		{
			Infection::setZombiePlayerStatus(s_player::GetId(absPlayerIdx), absPlayerIdx);
		}
		else 
		{
			// take away zombie's weapons
			Engine::Unit::remove_equipment(playerUnitDatum);
		}
	}
}

void Infection::OnPlayerDeath(ExecTime execTime, datum playerIdx)
{
	const unsigned long absPlayerIdx = DATUM_INDEX_TO_ABSOLUTE_INDEX(playerIdx);

	switch (execTime)
	{
	case ExecTime::_preEventExec:
		// to note after the original function executes, the controlled unit by this player is set to NONE
		if (!Memory::IsDedicatedServer())
		{
			PreOnPlayerDeathClientCode(absPlayerIdx);
		}

		if (!s_game_globals::game_is_predicted())
		{
			PreOnPlayerDeathHostCode(absPlayerIdx);
		}
		break;

	case ExecTime::_postEventExec:
		break;

	default:
		LOG_TRACE_GAME("{} - unknown execTime", __FUNCTION__);
		break;
	}
}

void Infection::PreOnPlayerSpawnClientCode(const unsigned long absPlayerIdx)
{
	LOG_TRACE_GAME(L"[h2mod-infection] Client pre spawn, playerIndex={}, localPlayerName={}", absPlayerIdx, s_player::GetName(absPlayerIdx));

	if (s_player::GetTeam(absPlayerIdx) != ZOMBIE_TEAM) { return; }

	//If player being spawned is LocalUser/Player
	for (byte i = 0; i < ENGINE_MAX_LOCAL_PLAYERS; i++)
	{
		LOG_TRACE_GAME("[h2mod-infection] Client pre spawn, found local player, current team = {}", h2mod->get_local_team_index(i));
		if (absPlayerIdx == DATUM_INDEX_TO_ABSOLUTE_INDEX(h2mod->get_player_datum_index_from_controller_index(i)))
		{
			//Change biped if LocalUser is in GreenTeam
			LOG_TRACE_GAME("[h2mod-infection] Client is infected! switching bipeds: {}", absPlayerIdx);
			s_player::SetUnitBipedType(absPlayerIdx, s_player::e_character_type::Flood);
			break;
		}
	}
}

void Infection::PostOnPlayerSpawnClientCode(const unsigned long absPlayerIdx)
{
	// if player being spawned is local
	for (byte i = 0; i < ENGINE_MAX_LOCAL_PLAYERS; i++)
	{
		if (absPlayerIdx == DATUM_INDEX_TO_ABSOLUTE_INDEX(h2mod->get_player_datum_index_from_controller_index(i))) 
		{
			if (b_initialSpawn == true) {
				//start of zombie match
				Infection::triggerSound(e_infection_sounds::_snd_infection, 1000);
				b_initialSpawn = false;
			}

			if (h2mod->get_local_team_index(i) == ZOMBIE_TEAM && b_infectedPlayed == false) {
				Infection::triggerSound(_snd_infected, 500);
				b_infectedPlayed = true;
			}

			if (h2mod->get_local_team_index(i) == HUMAN_TEAM) {
				h2mod->disable_weapon_pickup(true);
				h2mod->team_player_indicator_visibility(false);
			}

			else if (h2mod->get_local_team_index(i) == ZOMBIE_TEAM) {
				s_player::SetUnitBipedType(absPlayerIdx, s_player::e_character_type::Flood);
				h2mod->disable_weapon_pickup(false);
				h2mod->team_player_indicator_visibility(true);
			}
			break;
		}
	}
}

void Infection::PostOnPlayerSpawnHostCode(const unsigned long absPlayerIdx)
{
	LOG_TRACE_GAME("[h2mod-infection] Spawn player server index={}", absPlayerIdx);

	LOG_TRACE_GAME("[h2mod-infection] Spawn player server index={}, unit team index={}", absPlayerIdx, s_player::GetTeam(absPlayerIdx));
	if (s_player::GetTeam(absPlayerIdx) == HUMAN_TEAM)
	{
		Infection::setPlayerAsHuman(absPlayerIdx);
	}

	if (s_player::GetTeam(absPlayerIdx) == ZOMBIE_TEAM)
	{
		Infection::setPlayerAsZombie(absPlayerIdx);
	}
}

void Infection::OnPlayerSpawn(ExecTime execTime, datum playerIdx)
{
	const int absPlayerIdx = DATUM_INDEX_TO_ABSOLUTE_INDEX(playerIdx);

	switch (execTime)
	{
	// prespawn handler
	// to note after the original function executes, the controlled unit by this player is set to NONE
	case ExecTime::_preEventExec:
		if (!Memory::IsDedicatedServer())
		{
			PreOnPlayerSpawnClientCode(absPlayerIdx);
		}

		if (!s_game_globals::game_is_predicted()) 
		{ 
			Infection::preSpawnServerSetup(); 
		}
		break;

	// postspawn handler 
	case ExecTime::_postEventExec:
		//if the unit_object data pointer is not nullptr, the spawned object is "alive"
		LOG_TRACE_GAME("[h2mod-infection] Spawn player server index={}, player team index={}", absPlayerIdx, s_player::GetTeam(absPlayerIdx));

		if (!Memory::IsDedicatedServer()) 
		{ 
			PostOnPlayerSpawnClientCode(absPlayerIdx);
		}
		
		if (!s_game_globals::game_is_predicted())
		{
			PostOnPlayerSpawnHostCode(absPlayerIdx);
		}
		
		break;

	default:
		LOG_TRACE_GAME("{} - unknown execTime", __FUNCTION__);
		break;
	}
}