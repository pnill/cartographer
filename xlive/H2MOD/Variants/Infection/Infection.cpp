#include "Infection.h"

#include "H2MOD/Modules/Config/Config.h"
#include "H2MOD/Modules/CustomMenu/CustomLanguage.h"
#include "Blam/Cache/TagGroups/scenario_definition.hpp"
#include "Blam/Cache/TagGroups/item_collection_defenition.hpp"
#include "Blam/Cache/TagGroups/vehicle_collection_defenition.hpp"
#include "H2MOD/Engine/Engine.h"

#include "H2MOD\Modules\Networking\Networking.h"
#include "H2MOD/Tags/TagInterface.h"

std::vector<XUID> Infection::zombieIdentifiers;

const e_object_team HUMAN_TEAM = e_object_team::Red;
const e_object_team ZOMBIE_TEAM = e_object_team::Green;

std::map<int, std::map<e_infection_sounds, const wchar_t*>> i_SoundsTable;

bool firstSpawn;
bool infectedPlayed;
int zombiePlayerIndex = NONE;

int Infection::calculateZombiePlayerIndex() 
{
	if (NetworkSession::getPlayerCount() > 0)
	{
		std::mt19937 mt_rand(rd());
		std::vector<int> vecPlayersActiveIndexes;

		int playerIndex = 0;
		do 
		{
			if (NetworkSession::playerIsActive(playerIndex))
				vecPlayersActiveIndexes.push_back(playerIndex);

			playerIndex++;
		} while (playerIndex < 16);
	
		if (vecPlayersActiveIndexes.empty())
			return NONE;

		std::uniform_int_distribution<int> dist(0, NetworkSession::getPlayerCount() - 1);

		int infectedPlayerIndex = vecPlayersActiveIndexes[dist(mt_rand)];
		LOG_TRACE_GAME(L"[h2mod-infection] random infection player index: {}, with name: {}", infectedPlayerIndex, NetworkSession::getPlayerName(infectedPlayerIndex));

		return infectedPlayerIndex;
	}
	
	return NONE;
}

void Infection::sendTeamChange()
{
	if (NetworkSession::localPeerIsSessionHost())
	{
		if (NetworkSession::getPlayerCount() > 0)
		{
			int playerIndex = 0;
			do
			{
				if (NetworkSession::playerIsActive(playerIndex))
				{
					if (NetworkSession::getLocalPeerIndex() != NetworkSession::getPeerIndex(playerIndex)) {
						CustomPackets::sendTeamChange(NetworkSession::getPeerIndex(playerIndex), zombiePlayerIndex == playerIndex ? ZOMBIE_TEAM : HUMAN_TEAM);
						LOG_TRACE_GAME(L"[h2mod-infection] sent team change packet to player index: {}, with name: {}, infected?: {}", playerIndex, NetworkSession::getPlayerName(playerIndex), zombiePlayerIndex == playerIndex ? true : false);
					}
					else if (!Memory::isDedicatedServer()) {
						h2mod->set_local_team_index(0, zombiePlayerIndex == playerIndex ? ZOMBIE_TEAM : HUMAN_TEAM);
						LOG_TRACE_GAME(L"[h2mod-infection] setting local player team index, infected?: {}", zombiePlayerIndex == playerIndex ? true : false);
					}
				}

				playerIndex++;
			} while (playerIndex < 16);
		}
	}
}

Infection::Infection()
{
	this->deinitializer = new InfectionDeinitializer();
	this->initializer = new InfectionInitializer();
	this->preSpawnPlayer = new ZombiePreSpawnHandler();
	this->spawnPlayer = new ZombieSpawnHandler();
	this->playerDeath = new ZombieDeathHandler();
	this->playerKill = new KillZombieHandler();
}

void Infection::triggerSound(e_infection_sounds sound, int sleep)
{
	if (i_SoundsTable.count(H2Config_language.code_main))
	{
		LOG_TRACE_GAME(L"[h2mod-infection] Triggering sound {}", i_SoundsTable[H2Config_language.code_main][sound]);
		h2mod->custom_sound_play(i_SoundsTable[H2Config_language.code_main][sound], sleep);
	}
	else
	{
		LOG_TRACE_GAME(L"[h2mod-infection] Triggering sound {}", i_SoundsTable[0][sound]);
		h2mod->custom_sound_play(i_SoundsTable[0][sound], sleep);
	}
}

void Infection::initClient()
{
	Infection::disableSlayerSounds();
	infectedPlayed = false;
	firstSpawn = true;

	i_SoundsTable[english][e_infection_sounds::infected] = L"sounds/en/infected.wav";
	i_SoundsTable[english][e_infection_sounds::infection] = L"sounds/en/infection.wav";
	i_SoundsTable[english][e_infection_sounds::new_zombie] = L"sounds/en/new_zombie.wav";
	i_SoundsTable[spanish][e_infection_sounds::infected] = L"sounds/es/infected.wav";
	i_SoundsTable[spanish][e_infection_sounds::infection] = L"sounds/es/infection.wav";
	i_SoundsTable[spanish][e_infection_sounds::new_zombie] = L"sounds/es/new_zombie.wav";
	//Change Local Player's Team to Human if Not in Green
	//(In case player wants to start as Alpha Zombie leave him green)
	if (h2mod->get_local_team_index() != ZOMBIE_TEAM) {
		h2mod->set_local_team_index(0, HUMAN_TEAM);
	}
}

void Infection::resetZombiePlayerStatus() {
	zombieIdentifiers.clear();
}

void Infection::setZombiePlayerStatus(XUID identifier)
{
	zombieIdentifiers.push_back(identifier);
}

void Infection::initHost() {
	LOG_TRACE_GAME("[h2mod-infection] Host init setting unit speed patch");
	//Applying SpeedCheck fix
	h2mod->set_unit_speed_patch(true);
	//Fixing broken equipment and replace powerups/vehicles with weapons
	auto scenarios = tags::find_tags(blam_tag::tag_group_type::scenario);
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
	h2mod->disable_sounds(FLAG(SoundType::Slayer) | ALL_SOUNDS_NO_SLAYER);
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
		if (Player::getTeam(currentPlayerIndex) == ZOMBIE_TEAM && isZombie == false) {
			// if the player just joined the and he doesn't have zombie status, and his team is green, add him in the array
			setZombiePlayerStatus(playerIdentifier);
			isZombie = true;
		}

		LOG_TRACE_GAME(L"[h2mod-infection] Zombie pre spawn index={}, isZombie={}, playerIdentifier={}, playerName:{}", currentPlayerIndex, isZombie, playerIdentifier, Player::getName(currentPlayerIndex));
		if (isZombie) {
			Player::setUnitBipedType(currentPlayerIndex, Player::Biped::Elite);
			if (Player::getTeam(currentPlayerIndex) != ZOMBIE_TEAM)  {
				if (NetworkSession::localPeerIsSessionHost())
					CustomPackets::sendTeamChange(NetworkSession::getPeerIndex(currentPlayerIndex), ZOMBIE_TEAM); // prevent *toxic* kids from switching to humans in the pre-game lobby after joining
			}
		}
		else {
			Player::setUnitBipedType(currentPlayerIndex, Player::Biped::Spartan);
			humanCount++;
		}
		playerCount++;
	}

	// end the game if all humans are dead
	if (humanCount == 0 && playerCount > 1)
		NetworkSession::endGame();
}

void Infection::setPlayerAsHuman(int playerIndex) {
	Player::setUnitBipedType(playerIndex, Player::Biped::Spartan);
	Player::setBipedSpeed(playerIndex, 1.0f);
}

void Infection::setPlayerAsZombie(int playerIndex) {
	Player::setUnitBipedType(playerIndex, Player::Biped::Elite);
	Player::setBipedSpeed(playerIndex, 1.1f);

	call_give_player_weapon(playerIndex, e_weapons_datum_index::energy_blade, 1);
}

void Infection::spawnPlayerClientSetup(int playerIndex) {
	if (playerIndex != NONE) {
		XUID playerIdentifier = Player::getIdentifier(playerIndex);
		//If player being spawned is LocalUser/Player
		if (playerIdentifier == Player::getIdentifier(DATUM_ABSOLUTE_INDEX(h2mod->get_player_datum_index_from_controller_index(0)))) {
			if (firstSpawn == true) {
				//start of zombie match
				Infection::triggerSound(e_infection_sounds::infection, 1000);
				firstSpawn = false;
			}

			if (h2mod->get_local_team_index() == ZOMBIE_TEAM && infectedPlayed == false) {
				Infection::triggerSound(infected, 500);
				infectedPlayed = true;
			}

			if (h2mod->get_local_team_index() == HUMAN_TEAM) {
				h2mod->disable_weapon_pickup(true);
				h2mod->team_player_indicator_visibility(false);
			}
			else if (h2mod->get_local_team_index() == ZOMBIE_TEAM) {
				Player::setUnitBipedType(playerIndex, Player::Biped::Elite);

				h2mod->disable_weapon_pickup(false);
				h2mod->team_player_indicator_visibility(true);
			}
		}
	}
}

void Infection::spawnServerPlayerSetup(int playerIndex) {
	if (playerIndex != NONE) {
		LOG_TRACE_GAME("[h2mod-infection] Spawn player server index={}", playerIndex);
		datum unit_datum_index = Player::getPlayerUnitDatumIndex(playerIndex);
		char* unit_object = (char*)object_try_and_get_and_verify_type(unit_datum_index, FLAG(e_object_type::biped));
		if (unit_object) {
			//if the unit_object data pointer is not nullptr, the spawned object is "alive"

			LOG_TRACE_GAME("[h2mod-infection] Spawn player server index={0}, unit team index={1}", playerIndex, h2mod->get_unit_team_index(unit_datum_index));
			if (h2mod->get_unit_team_index(unit_datum_index) == HUMAN_TEAM) {
				Infection::setPlayerAsHuman(playerIndex);
			}

			if (h2mod->get_unit_team_index(unit_datum_index) == ZOMBIE_TEAM) {
				Infection::setPlayerAsZombie(playerIndex);
			}
		}
	}
}

void Infection::infectPlayer(int playerIndex, datum unitDatumIndex) {
	if (playerIndex != NONE) {
		char* unit_object = (char*)object_try_and_get_and_verify_type(unitDatumIndex, FLAG(e_object_type::biped));
		if (unit_object && Player::getTeam(playerIndex) != ZOMBIE_TEAM)
		{
			//if we have a valid object and the object is not on the zombie team
			XUID playerIdentifier = Player::getIdentifier(playerIndex);

			LOG_TRACE_GAME(L"[h2mod-infection] Infected local player, Name={}, identifier={}", h2mod->get_local_player_name(0), playerIdentifier);

			//If player being infected is LocalUser/Player
			if (playerIdentifier == Player::getIdentifier(DATUM_ABSOLUTE_INDEX(h2mod->get_player_datum_index_from_controller_index(0)))) {
				LOG_TRACE_GAME("[h2mod-infection] Setting player as zombie");
				h2mod->set_local_team_index(0, ZOMBIE_TEAM);
				Player::setUnitBipedType(playerIndex, Player::Biped::Elite);
			}
			else {
				//if not, then this is a new zombie
				LOG_TRACE_GAME(L"[h2mod-infection] Player died, name={}, identifer={}", Player::getName(playerIndex), playerIdentifier);
				Infection::triggerSound(new_zombie, 1000);
			}
		}
	}
}

void Infection::infectPlayers(int playerIndex, datum unitDatumIndex) {
	if (playerIndex != NONE) {
		char* unit_object = (char*)object_try_and_get_and_verify_type(unitDatumIndex, FLAG(e_object_type::biped));
		if (unit_object) {
			if (h2mod->get_unit_team_index(unitDatumIndex) == ZOMBIE_TEAM) {
				//don't drop swords after zombie death
				Engine::Unit::remove_equipment(unitDatumIndex); //Take away zombie's weapons
			}
			else {
				Infection::setZombiePlayerStatus(Player::getIdentifier(playerIndex));
			}
		}
	}
}

void ZombieDeathHandler::onPeerHost()
{
	int PlayerIndex = h2mod->get_player_index_from_unit_datum_index(this->getUnitDatumIndex());
	//infect peer host
	Infection::infectPlayer(PlayerIndex, this->getUnitDatumIndex());

	//infect other players if applicable
	Infection::infectPlayers(PlayerIndex, this->getUnitDatumIndex());
}

void ZombieDeathHandler::onDedi()
{
	//infect other players if applicable
	Infection::infectPlayers(h2mod->get_player_index_from_unit_datum_index(this->getUnitDatumIndex()), this->getUnitDatumIndex());
}

void ZombieDeathHandler::onClient()
{
	LOG_TRACE_GAME("ZombieDeathhandler::OnClient() getUnitDatumIndex: {:x}", this->getUnitDatumIndex());

	//infect client
	Infection::infectPlayer(h2mod->get_player_index_from_unit_datum_index(this->getUnitDatumIndex()), this->getUnitDatumIndex());
}

void ZombiePreSpawnHandler::onPeerHost()
{
	this->onClient();
	Infection::preSpawnServerSetup();
}

void ZombiePreSpawnHandler::onDedi()
{
	Infection::preSpawnServerSetup();
}

void ZombiePreSpawnHandler::onClient()
{
	XUID playerIdentifier = Player::getIdentifier(this->getPlayerIndex());
	LOG_TRACE_GAME(L"[h2mod-infection] Client pre spawn, playerIndex={0}, playerIdentifier={1}, localPlayerName={2}", this->getPlayerIndex(), playerIdentifier, h2mod->get_local_player_name(0));
	//If player being spawned is LocalUser/Player
	if (playerIdentifier == Player::getIdentifier(DATUM_ABSOLUTE_INDEX(h2mod->get_player_datum_index_from_controller_index(0))))
	{
		LOG_TRACE_GAME("[h2mod-infection] Client pre spawn, found local player, current team = {}", h2mod->get_local_team_index());
		//Change biped if LocalUser is in GreenTeam
		if (h2mod->get_local_team_index() == ZOMBIE_TEAM)
		{
			LOG_TRACE_GAME("[h2mod-infection] Client is infected! switching bipeds: {}", this->getPlayerIndex());
			Player::setUnitBipedType(this->getPlayerIndex(), Player::Biped::Elite);
		}
	}
}

void ZombieSpawnHandler::onPeerHost()
{
	Infection::spawnPlayerClientSetup(this->getPlayerIndex());
	Infection::spawnServerPlayerSetup(this->getPlayerIndex());
}

void ZombieSpawnHandler::onDedi()
{
	Infection::spawnServerPlayerSetup(this->getPlayerIndex());
}

void ZombieSpawnHandler::onClient()
{
	Infection::spawnPlayerClientSetup(this->getPlayerIndex());
}

void InfectionDeinitializer::onPeerHost()
{
	Infection::resetWeaponInteractionAndEmblems();
	//reset unit speeds on server
	h2mod->set_unit_speed_patch(false);
}

void InfectionDeinitializer::onDedi()
{
	Infection::resetWeaponInteractionAndEmblems();
	//reset unit speeds on server
	h2mod->set_unit_speed_patch(false);
}

void InfectionDeinitializer::onClient()
{
	LOG_TRACE_GAME("[h2mod-infection] Client deinit");
	Infection::resetWeaponInteractionAndEmblems();
}

void InfectionInitializer::onPeerHost()
{
	LOG_TRACE_GAME("[h2mod-infection] Peer host init");
	Infection::initClient();
	Infection::initHost();
	zombiePlayerIndex = Infection::calculateZombiePlayerIndex();
	LOG_TRACE_GAME("[h2mod-infection] Peer host calculated zombie index {}", zombiePlayerIndex);
	if (zombiePlayerIndex == NONE) {
		LOG_TRACE_GAME("[h2mod-infection] Failed selecting a zombie!");
	}
	else {
		LOG_TRACE_GAME("[h2mod-infection] Peer host setting player as human");
		//send out the team change packets to peers
		Infection::sendTeamChange();
		Infection::setZombiePlayerStatus(NetworkSession::getPlayerXuid(zombiePlayerIndex));
	}
}

void InfectionInitializer::onDedi()
{
	LOG_TRACE_GAME("[h2mod-infection] Dedicated server init");
	Infection::initHost();
	//figure out who the zombie is
	zombiePlayerIndex = Infection::calculateZombiePlayerIndex();
	LOG_TRACE_GAME("[h2mod-infection] Dedicated host calculated zombie index {}", zombiePlayerIndex);
	if (zombiePlayerIndex == NONE) {
		LOG_TRACE_GAME("[h2mod-infection] Failed selecting a zombie!");
	}
	else {
		LOG_TRACE_GAME("[h2mod-infection] Dedicated host setting player as human");
		//send out the team change packets to peers
		Infection::sendTeamChange();
		Infection::setZombiePlayerStatus(NetworkSession::getPlayerXuid(zombiePlayerIndex));
	}
}

void InfectionInitializer::onClient()
{
	LOG_TRACE_GAME("[h2mod-infection] Client init");
	Infection::initClient();
}

void KillZombieHandler::onClient() {
	//implement on kill zombie
}

void KillZombieHandler::onDedi() {
	//implement on kill zombie
}

void KillZombieHandler::onPeerHost() {
	//implement on kill zombie
}

void ZombieHandler::setPlayerIndex(int playerIndex)
{
	this->playerIndex = playerIndex;
}

void ZombieHandler::setUnitDatumIndex(datum unitDatumIndex)
{
	this->unitDatumIndex = unitDatumIndex;
}

int ZombieHandler::getPlayerIndex()
{
	return this->playerIndex;
}

datum ZombieHandler::getUnitDatumIndex()
{
	return this->unitDatumIndex;
}