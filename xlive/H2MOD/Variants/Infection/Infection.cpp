#include "stdafx.h"

#include "Infection.h"
#include "Blam\Engine\Game\GameGlobals.h"
#include "Blam\Engine\Networking\NetworkMessageTypeCollection.h"
#include "Blam\Cache\TagGroups\item_collection_definition.hpp"
#include "Blam\Cache\TagGroups\scenario_definition.hpp"
#include "Blam\Cache\TagGroups\vehicle_collection_definition.hpp"
#include "H2MOD\Engine\Engine.h"
#include "H2MOD/Modules/SpecialEvents/SpecialEvents.h"
#include "H2MOD\Modules\Shell\Config.h"
#include "H2MOD\Modules\CustomMenu\CustomLanguage.h"
#include "H2MOD\Modules\PlayerRepresentation\PlayerRepresentation.h"
#include "H2MOD\Tags\MetaLoader\tag_loader.h"
#include "H2MOD\Tags\TagInterface.h"

std::vector<unsigned long long> Infection::zombieIdentifiers;

const e_object_team HUMAN_TEAM = e_object_team::Red;
const e_object_team ZOMBIE_TEAM = e_object_team::Green;

std::map<int, std::map<e_infection_sounds, const wchar_t*>> infectionSoundTable;

bool initialSpawn;
bool infectedPlayed;
int zombiePlayerIndex = NONE;

Infection::Infection()
{
}

int Infection::calculateZombiePlayerIndex() 
{
	if (NetworkSession::GetPlayerCount() > 0)
	{
		std::mt19937 mt_rand(rd());
		std::vector<int> vecPlayersActiveIndexes;

		int playerIndex = 0;
		do 
		{
			if (NetworkSession::PlayerIsActive(playerIndex))
				vecPlayersActiveIndexes.push_back(playerIndex);

			playerIndex++;
		} while (playerIndex < 16);
	
		if (vecPlayersActiveIndexes.empty())
			return NONE;

		std::uniform_int_distribution<int> dist(0, NetworkSession::GetPlayerCount() - 1);

		int infectedPlayerIndex = vecPlayersActiveIndexes[dist(mt_rand)];
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
			int playerIndex = 0;
			do
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

				playerIndex++;
			} while (playerIndex < ENGINE_PLAYER_MAX);
		}
	}
}

void Infection::triggerSound(e_infection_sounds sound, int sleep)
{
	if (infectionSoundTable.count(H2Config_language.code_main))
	{
		LOG_TRACE_GAME(L"[h2mod-infection] Triggering sound {}", infectionSoundTable[H2Config_language.code_main][sound]);
		h2mod->custom_sound_play(infectionSoundTable[H2Config_language.code_main][sound], sleep);
	}
	else
	{
		LOG_TRACE_GAME(L"[h2mod-infection] Triggering sound {}", infectionSoundTable[0][sound]);
		h2mod->custom_sound_play(infectionSoundTable[0][sound], sleep);
	}
}

void Infection::InitClient()
{
	Infection::disableSlayerSounds();
	infectedPlayed = false;
	initialSpawn = true;

	infectionSoundTable[_lang_id_english][e_infection_sounds::_snd_infected] = L"sounds/en/infected.wav";
	infectionSoundTable[_lang_id_english][e_infection_sounds::_snd_infection] = L"sounds/en/infection.wav";
	infectionSoundTable[_lang_id_english][e_infection_sounds::_snd_new_zombie] = L"sounds/en/new_zombie.wav";
	infectionSoundTable[_lang_id_spanish][e_infection_sounds::_snd_infected] = L"sounds/es/infected.wav";
	infectionSoundTable[_lang_id_spanish][e_infection_sounds::_snd_infection] = L"sounds/es/infection.wav";
	infectionSoundTable[_lang_id_spanish][e_infection_sounds::_snd_new_zombie] = L"sounds/es/new_zombie.wav";

	
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
	//Fixing broken equipment and replace powerups/vehicles with weapons
	//auto scenarios = tags::find_tags(blam_tag::tag_group_type::scenario);
	//auto magnum_datum = tags::find_tag(blam_tag::tag_group_type::weapon, "objects\\weapons\\pistol\\magnum\\magnum");
	//auto shotgun_datum = tags::find_tag(blam_tag::tag_group_type::weapon, "objects\\weapons\\rifle\\shotgun\\shotgun");
	//for(auto &scenario_ : scenarios)
	//{
	//	auto scenario = tags::get_tag<blam_tag::tag_group_type::scenario, s_scenario_group_definition>(scenario_.first);
	//	//Using this to make all vehicle spawns be shotguns
	//	datum itmc_override;
	//	bool flop = false;
	//	for(auto i = 0; i < scenario->netgame_equipment.size; i++)
	//	{
	//		auto equipment = scenario->netgame_equipment[i];
	//		equipment->classification = s_scenario_group_definition::s_netgame_equipment_block::e_classification::powerup;
	//		s_item_collection_group_definition* i_collection;
	//		s_vehicle_collection_group_definition* v_collection;
	//		switch(equipment->itemvehicle_collection.TagGroup.tag_type)
	//		{
	//			case blam_tag::tag_group_type::itemcollection:
	//				if (!flop)
	//					itmc_override = equipment->itemvehicle_collection.TagIndex;
	//				i_collection = tags::get_tag<blam_tag::tag_group_type::itemcollection, s_item_collection_group_definition>(equipment->itemvehicle_collection.TagIndex);
	//				for(auto j = 0; j < i_collection->item_permutations.size; j++)
	//				{
	//					i_collection->item_permutations[j]->item.TagGroup = blam_tag::tag_group_type::weapon;
	//					if (flop) {
	//						i_collection->item_permutations[j]->item.TagIndex = magnum_datum;
	//					}
	//					else
	//					{
	//						flop = true;
	//						i_collection->item_permutations[j]->item.TagIndex = shotgun_datum;
	//					}
	//				}
	//				
	//				break;
	//			case blam_tag::tag_group_type::vehiclecollection:
	//				equipment->itemvehicle_collection.TagGroup = blam_tag::tag_group_type::itemcollection;
	//				equipment->itemvehicle_collection.TagIndex = itmc_override;
	//				break;
	//		}

	//	}
	//}
	LOG_TRACE_GAME("[h2mod-infection] Host init resetting zombie player data status");
	Infection::resetZombiePlayerStatus();
}

void Infection::disableSlayerSounds()
{
	LOG_TRACE_GAME("[h2mod-infection] Disabling slayer sounds");
	//disable slayer sounds for the infection game type
	h2mod->disable_sounds(FLAG(_sound_type_slayer) | ALL_SOUNDS_NO_SLAYER);
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
		XUID playerIdentifier = playerIt.get_current_player_id();
		bool isZombie = std::find(Infection::zombieIdentifiers.begin(), Infection::zombieIdentifiers.end(), playerIdentifier) != Infection::zombieIdentifiers.end();
		if (s_player::GetTeam(currentPlayerIndex) == ZOMBIE_TEAM && isZombie == false) {
			// if the player just joined the and he doesn't have zombie status, and his team is green, add him in the array
			setZombiePlayerStatus(playerIdentifier);
			isZombie = true;
		}

		LOG_TRACE_GAME(L"[h2mod-infection] Zombie pre spawn index={}, isZombie={}, playerIdentifier={}, playerName:{}", currentPlayerIndex, isZombie, playerIdentifier, s_player::GetName(currentPlayerIndex));
		if (isZombie) {
			s_player::SetUnitBipedType(currentPlayerIndex, s_player::e_character_type::Flood);
			if (s_player::GetTeam(currentPlayerIndex) != ZOMBIE_TEAM)  {
				if (NetworkSession::LocalPeerIsSessionHost())
					NetworkMessage::SendTeamChange(NetworkSession::GetPeerIndex(currentPlayerIndex), ZOMBIE_TEAM); // prevent *toxic* kids from switching to humans in the pre-game lobby after joining
			}
		}
		else {
			if(SpecialEvents::getCurrentEvent() == SpecialEvents::_halloween && H2Config_spooky_boy)
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
			//send out the team change packets to peers
			Infection::sendTeamChange();
			Infection::setZombiePlayerStatus(NetworkSession::GetPlayerXuid(zombiePlayerIndex));
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
				XUID playerIdentifier = s_player::GetIdentifier(absPlayerIdx);

				LOG_TRACE_GAME(L"[h2mod-infection] Infected local player, Name={}, identifier={}", h2mod->get_local_player_name(0), playerIdentifier);

				// check if the player being infected is local
				if (playerIdentifier == s_player::GetIdentifier(DATUM_INDEX_TO_ABSOLUTE_INDEX(h2mod->get_player_datum_index_from_controller_index(0)))) {
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
					Infection::setZombiePlayerStatus(s_player::GetIdentifier(absPlayerIdx));
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
			XUID playerIdentifier = s_player::GetIdentifier(absPlayerIdx);
			LOG_TRACE_GAME(L"[h2mod-infection] Client pre spawn, playerIndex={}, playerIdentifier={}, localPlayerName={}", absPlayerIdx, playerIdentifier, h2mod->get_local_player_name(0));
			//If player being spawned is LocalUser/Player
			if (playerIdentifier == s_player::GetIdentifier(DATUM_INDEX_TO_ABSOLUTE_INDEX(h2mod->get_player_datum_index_from_controller_index(0))))
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
			XUID playerIdentifier = s_player::GetIdentifier(absPlayerIdx);
			// if player being spawned is local
			if (playerIdentifier == s_player::GetIdentifier(DATUM_INDEX_TO_ABSOLUTE_INDEX(h2mod->get_player_datum_index_from_controller_index(0)))) {
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
