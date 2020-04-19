#include "stdafx.h"

#include "Globals.h"
#include "Blam\Enums\Enums.h"
#include "..\..\Modules\Networking\Networking.h"
#include "..\..\Modules\Networking\CustomPackets\CustomPackets.h"

using namespace Blam::Enums;
using namespace NetworkSession;

std::vector<XUID> Infection::zombieIdentifiers;

const int ZOMBIE_TEAM = 3;
const int HUMAN_TEAM = 0;
const wchar_t* NEW_ZOMBIE_SOUND = L"sounds/new_zombie.wav";
const wchar_t* INFECTION_SOUND = L"sounds/infection.wav";
const wchar_t* INFECTED_SOUND = L"sounds/infected.wav";

bool firstSpawn;
bool infectedPlayed;
signed int zombiePlayerIndex = -1;

signed int Infection::calculateZombiePlayerIndex() 
{
	if (getPlayerCount() > 0)
	{
		std::mt19937 mt_rand(rd());
		std::vector<int> vecPlayersActiveIndexes;

		int playerIndex = 0;
		do 
		{
			if (playerIsActive(playerIndex))
				vecPlayersActiveIndexes.push_back(playerIndex);

			playerIndex++;
		} while (playerIndex < 16);
	
		if (vecPlayersActiveIndexes.empty())
			return -1;

		std::uniform_int_distribution<int> dist(0, getPlayerCount() - 1);

		int infectedPlayerIndex = vecPlayersActiveIndexes[dist(mt_rand)];
		LOG_TRACE_GAME(L"[h2mod-infection] random infection player index: {}, with name: {}", infectedPlayerIndex, getPlayerName(infectedPlayerIndex));

		return infectedPlayerIndex;
	}
	
	return -1;
}

void Infection::sendTeamChange()
{
	if (localPeerIsSessionHost())
	{
		if (getPlayerCount() > 0)
		{
			int playerIndex = 0;
			do
			{
				if (playerIsActive(playerIndex))
				{
					if (getLocalPeerIndex() != getPeerIndex(playerIndex)) {
						CustomPackets::sendTeamChange(getPeerIndex(playerIndex), zombiePlayerIndex == playerIndex ? ZOMBIE_TEAM : HUMAN_TEAM);
						LOG_TRACE_GAME(L"[h2mod-infection] sent team change packet to player index: {}, with name: {}, infected?: {}", playerIndex, getPlayerName(playerIndex), zombiePlayerIndex == playerIndex ? true : false);
					}
					else if (!h2mod->Server) {
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

void Infection::triggerSound(const wchar_t* name, int sleep) {
	LOG_TRACE_GAME(L"[h2mod-infection] Triggering sound {}", name);
	h2mod->custom_sound_play(name, sleep);
}

void Infection::initClient()
{
	Infection::disableSlayerSounds();
	infectedPlayed = false;
	firstSpawn = true;

	//Change Local Player's Team to Human if Not in Green
	//(In case player wants to start as Alpha Zombie leave him green)
	if (h2mod->get_local_team_index() != ZOMBIE_TEAM) {
		h2mod->set_local_team_index(0, HUMAN_TEAM);
	}
}

void Infection::resetZombiePlayerStatus() {
	Infection::zombieIdentifiers.clear();
}

void Infection::setZombiePlayerStatus(int playerIndex)
{
	Infection::zombieIdentifiers.push_back(getPlayerXuid(playerIndex));
}

void Infection::initHost() {
	LOG_TRACE_GAME("[h2mod-infection] Host init setting unit speed patch");
	//Applying SpeedCheck fix
	h2mod->set_unit_speed_patch(true);
	LOG_TRACE_GAME("[h2mod-infection] Host init resetting zombie player data status");
	Infection::resetZombiePlayerStatus();
}

void Infection::disableSlayerSounds()
{
	LOG_TRACE_GAME("[h2mod-infection] Disabling slayer sounds");
	//disable slayer sounds for the infection game type
	h2mod->disable_sound(SoundType::Slayer);
	h2mod->disable_sound(SoundType::GainedTheLead);
	h2mod->disable_sound(SoundType::LostTheLead);
	h2mod->disable_sound(SoundType::TeamChange);
}

void Infection::resetWeaponInteractionAndEmblems() {
	LOG_TRACE_GAME("[h2mod-infection] Resetting weapons interactions and emblem visibility");
	h2mod->disable_weapon_pickup(true);
	h2mod->team_player_indicator_visibility(true);
}

void Infection::preSpawnServerSetup() {
	int humanCount = 0;
	int playerIndex = 0;
	do {
		if (playerIsActive(playerIndex)) {
			XUID playerIdentifier = getPlayerXuid(playerIndex);
			BOOL isZombie = std::find(Infection::zombieIdentifiers.begin(), Infection::zombieIdentifiers.end(), playerIdentifier) != Infection::zombieIdentifiers.end();

			LOG_TRACE_GAME(L"[h2mod-infection] Zombie pre spawn index={0}, isZombie={1}, playerIdentifier={2}, playerName:{3}", playerIndex, isZombie, playerIdentifier, getPlayerName(playerIndex));
			if (isZombie) {
				h2mod->set_unit_biped(Player::Biped::Elite, playerIndex);
				if (localPeerIsSessionHost() 
					&& getPlayerTeam(playerIndex) != ZOMBIE_TEAM) // prevent *toxic* kids from switching to humans in the pre-game lobby after joining
					CustomPackets::sendTeamChange(getPeerIndex(playerIndex), ZOMBIE_TEAM);
			}
			else {
				h2mod->set_unit_biped(Player::Biped::Spartan, playerIndex);
				humanCount++;
			}
		}
		playerIndex++;
	} while (playerIndex < 16);

	// end the game if all humans are dead
	if (humanCount == 0 && getPlayerCount() > 1)
		endGame();
}

void Infection::setPlayerAsHuman(int playerIndex) {
	h2mod->set_unit_biped(Player::Biped::Spartan, playerIndex);
	h2mod->set_unit_speed(1.0f, playerIndex);

	call_give_player_weapon(playerIndex, Weapon::shotgun, 1);
	call_give_player_weapon(playerIndex, Weapon::magnum, 0);
}

void Infection::setPlayerAsZombie(int playerIndex) {
	h2mod->set_unit_biped(Player::Biped::Elite, playerIndex);
	h2mod->set_unit_speed(1.1f, playerIndex);

	call_give_player_weapon(playerIndex, Weapon::energy_blade, 1);
}

void Infection::spawnPlayerClientSetup(int playerIndex) {
	XUID playerIdentifier = getPlayerXuid(playerIndex);
	//If player being spawned is LocalUser/Player
	if (playerIdentifier == getPlayerXuid(h2mod->get_player_datum_index_from_controller_index(0).ToAbsoluteIndex())) {
		if (firstSpawn == true) {
			//start of zombie match
			Infection::triggerSound(INFECTION_SOUND, 1000);
			firstSpawn = false;
		}

		if (h2mod->get_local_team_index() == ZOMBIE_TEAM && infectedPlayed == false) {
			Infection::triggerSound(INFECTED_SOUND, 500);
			infectedPlayed = true;
		}

		if (h2mod->get_local_team_index() == HUMAN_TEAM) {
			h2mod->disable_weapon_pickup(true);
			h2mod->team_player_indicator_visibility(false);
		}
		else if (h2mod->get_local_team_index() == ZOMBIE_TEAM) {
			h2mod->set_unit_biped(Player::Biped::Elite, playerIndex);

			h2mod->disable_weapon_pickup(false);
			h2mod->team_player_indicator_visibility(true);
		}
	}
}

void Infection::spawnServerPlayerSetup(int playerIndex) {
	LOG_TRACE_GAME("[h2mod-infection] Spawn player server index={}", playerIndex);
	datum unit_datum_index = h2mod->get_unit_datum_from_player_index(playerIndex);
	int unit_object = call_object_try_and_get_with_type(unit_datum_index, 3);

	if (unit_object && *(BYTE*)(unit_object + 0xAA) == 0) {
		//if the unit_object is not 0, the spawned object is "alive"

		LOG_TRACE_GAME("[h2mod-infection] Spawn player server index={0}, unit team index={1}", playerIndex, h2mod->get_unit_team_index(unit_datum_index));
		if (h2mod->get_unit_team_index(unit_datum_index) == HUMAN_TEAM) {
			Infection::setPlayerAsHuman(playerIndex);
		}

		if (h2mod->get_unit_team_index(unit_datum_index) == ZOMBIE_TEAM) {
			Infection::setPlayerAsZombie(playerIndex);
		}
	}
}

void Infection::infectPlayer(datum unitDatumIndex, int playerIndex) {
	int unit_object = call_object_try_and_get_with_type(unitDatumIndex, 3);
	if (unit_object && h2mod->get_unit_team_index(unitDatumIndex) != ZOMBIE_TEAM
		&& *(BYTE*)(unit_object + 0xAA) == 0) //check if object type is biped
	{
		//if we have a valid object and the object is not on the zombie team
		XUID playerIdentifier = getPlayerXuid(h2mod->get_player_index_from_unit_datum(unitDatumIndex));

		LOG_TRACE_GAME(L"[h2mod-infection] Infected player, localName={0}, nameFromUnitDatumIndex={1}", h2mod->get_local_player_name(0), playerIdentifier);

		//If player being infected is LocalUser/Player
		if (playerIdentifier == getPlayerXuid(h2mod->get_player_datum_index_from_controller_index(0).ToAbsoluteIndex())) {
			LOG_TRACE_GAME("[h2mod-infection] Setting player as zombie");
			h2mod->set_local_team_index(0, ZOMBIE_TEAM);
			h2mod->set_unit_biped(Player::Biped::Elite, playerIndex);
		}
		else {
			//if not, then this is a new zombie
			Infection::triggerSound(NEW_ZOMBIE_SOUND, 1000);
		}
	}
}

void Infection::infectPlayers(datum unitDatumIndex, int playerIndex) {
	int unit_object = call_object_try_and_get_with_type(unitDatumIndex, 3);
	if (unit_object && *(BYTE*)(unit_object + 0xAA) == 0) {
		Infection::setZombiePlayerStatus(playerIndex);

		if (h2mod->get_unit_team_index(unitDatumIndex) == ZOMBIE_TEAM) {
			//don't drop swords after zombie death
			call_unit_reset_equipment(unitDatumIndex); //Take away zombie's weapons
		}
	}
}

void ZombieDeathHandler::onPeerHost()
{
	int PlayerIndex = h2mod->get_player_index_from_unit_datum(this->getUnitDatumIndex());
	//infect peer host
	Infection::infectPlayer(this->getUnitDatumIndex(), PlayerIndex);

	//infect other players if applicable
	Infection::infectPlayers(this->getUnitDatumIndex(), PlayerIndex);
}

void ZombieDeathHandler::onDedi()
{
	//infect other players if applicable
	Infection::infectPlayers(this->getUnitDatumIndex(), h2mod->get_player_index_from_unit_datum(this->getUnitDatumIndex()));
}

void ZombieDeathHandler::onClient()
{
	LOG_TRACE_GAME("ZombieDeathhandler::OnClient() getUnitDatumIndex: {:x}", this->getUnitDatumIndex().ToInt());

	//infect client
	Infection::infectPlayer(this->getUnitDatumIndex(), h2mod->get_player_index_from_unit_datum(this->getUnitDatumIndex()));
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
	XUID playerIdentifier = getPlayerXuid(this->getPlayerIndex());
	LOG_TRACE_GAME(L"[h2mod-infection] Client pre spawn, playerIndex={0}, playerIdentifier={1}, localPlayerName={2}", this->getPlayerIndex(), playerIdentifier, h2mod->get_local_player_name(0));
	//If player being spawned is LocalUser/Player
	if (playerIdentifier == getPlayerXuid(h2mod->get_player_datum_index_from_controller_index(0).ToAbsoluteIndex()))
	{
		LOG_TRACE_GAME("[h2mod-infection] Client pre spawn, found local player, current team = {}", h2mod->get_local_team_index());
		//Change biped if LocalUser is in GreenTeam
		if (h2mod->get_local_team_index() == ZOMBIE_TEAM)
		{
			LOG_TRACE_GAME("[h2mod-infection] Client is infected! switching bipeds: {}", this->getPlayerIndex());
			h2mod->set_unit_biped(Player::Biped::Elite, this->getPlayerIndex());
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
	if (zombiePlayerIndex == -1) {
		LOG_TRACE_GAME("[h2mod-infection] Failed selecting a zombie!");
	}
	else {
		LOG_TRACE_GAME("[h2mod-infection] Peer host setting player as human");
		//send out the team change packets to peers
		Infection::sendTeamChange();

		//mark this player as the zombie in the internal players structure
		Infection::setZombiePlayerStatus(zombiePlayerIndex);
	}
}

void InfectionInitializer::onDedi()
{
	LOG_TRACE_GAME("[h2mod-infection] Dedicated server init");
	Infection::initHost();
	//figure out who the zombie is
	zombiePlayerIndex = Infection::calculateZombiePlayerIndex();
	LOG_TRACE_GAME("[h2mod-infection] Dedicated host calculated zombie index {}", zombiePlayerIndex);
	if (zombiePlayerIndex == -1) {
		LOG_TRACE_GAME("[h2mod-infection] Failed selecting a zombie!");
	}
	else {
		LOG_TRACE_GAME("[h2mod-infection] Dedicated host setting player as human");
		//send out the team change packets to peers
		Infection::sendTeamChange();

		//mark this player as the zombie in the internal players structure
		Infection::setZombiePlayerStatus(zombiePlayerIndex);
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