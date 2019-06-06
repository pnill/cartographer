#include "Globals.h"
#include "H2MOD\protobuf\h2mod.pb.h"
#include "..\..\Modules\Networking\Networking.h"
#include "..\..\Modules\Networking\CustomPackets\CustomPackets.h"

std::vector<std::wstring> Infection::zombieNames;

const int ZOMBIE_TEAM = 3;
const int HUMAN_TEAM = 0;
const wchar_t* NEW_ZOMBIE_SOUND = L"sounds/new_zombie.wav";
const wchar_t* INFECTION_SOUND = L"sounds/infection.wav";
const wchar_t* INFECTED_SOUND = L"sounds/infected.wav";

bool firstSpawn;
bool infectedPlayed;
signed int zombieIndex = -1;

signed int Infection::calculateZombiePlayerIndex() {
	int rand_index = 0;
	network_session* session = NetworkSession::getCurrentNetworkSession();
	if (session->total_peers > 0)
	{
		rand_index = rand() % session->total_peers;
		if (rand_index == session->session_host_peer_index)
		{
			if (session->dedicated_server[0] == 1)
			{
				if (session->total_peers <= 1)
					return -1;
				else
					return calculateZombiePlayerIndex();
			}
		}
	}
	return rand_index;
}

void Infection::sendTeamChange()
{
	network_session* session = NetworkSession::getCurrentNetworkSession();
	if (session->local_session_state == _network_session_state_session_host)
	{
		if (session->total_peers > 0)
		{
			int index = 0;
			do 
			{
				if (session->session_host_peer_index != index) {
					CustomPackets::sendTeamChange(session, index, zombieIndex == index ? ZOMBIE_TEAM : HUMAN_TEAM);
				} 
				else
				{
					if (!h2mod->Server)
						h2mod->set_local_team_index(0, zombieIndex == index ? ZOMBIE_TEAM : HUMAN_TEAM);
				}

				index++;
			} 
			while (index < session->total_peers);
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
	TRACE_GAME("[h2mod-infection] Triggering sound %s", name);
	h2mod->CustomSoundPlay(name, sleep);
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
	Infection::zombieNames.clear();
}

void Infection::setZombiePlayerStatus(int index)
{
	Infection::zombieNames.push_back(players->getPlayerName(index));
}

void Infection::initHost() {
	TRACE_GAME("[h2mod-infection] Host init setting unit speed patch");
	//Applying SpeedCheck fix
	h2mod->set_unit_speed_patch(true);
	TRACE_GAME("[h2mod-infection] Host init resetting zombie player data status");
	Infection::resetZombiePlayerStatus();
}

void Infection::disableSlayerSounds()
{
	TRACE_GAME("[h2mod-infection] Disabling slayer sounds");
	//disable slayer sounds for the infection game type
	h2mod->DisableSound(SoundType::Slayer);
	h2mod->DisableSound(SoundType::GainedTheLead);
	h2mod->DisableSound(SoundType::LostTheLead);
	h2mod->DisableSound(SoundType::TeamChange);
}

void Infection::resetWeaponInteractionAndEmblems() {
	TRACE_GAME("[h2mod-infection] Resetting weapons interactions and emblem visibility");
	h2mod->DisableWeaponPickup(true);
	h2mod->IndicatorVisibility(true);
}

void Infection::preSpawnServerSetup() {
	int playerCounter = 0;
	do {
		std::wstring playerName = players->getPlayerName(playerCounter);
		BOOL isZombie = std::find(Infection::zombieNames.begin(), Infection::zombieNames.end(), playerName) != Infection::zombieNames.end();
		TRACE_GAME("[h2mod-infection] Zombie pre spawn index=%d, isZombie=%d, playerName=%s", playerCounter, isZombie, playerName.c_str());
		if (isZombie) {
			h2mod->set_unit_biped(BipedType::Elite, playerCounter);
		} else {
			h2mod->set_unit_biped(BipedType::MasterChiefMp, playerCounter);
		}
		playerCounter++;
	} while (playerCounter < players->getPlayerCount());
}

void Infection::setPlayerAsHuman(int index) {
	h2mod->set_unit_biped(BipedType::MasterChiefMp, index);
	h2mod->set_unit_speed(1.0f, index);
	
	GivePlayerWeapon(index, Weapon::shotgun, 1);
	GivePlayerWeapon(index, Weapon::magnum, 0);
}

void Infection::setPlayerAsZombie(int index) {
	h2mod->set_unit_biped(BipedType::Elite, index);
	h2mod->set_unit_speed(1.1f, index);
	
	GivePlayerWeapon(index, Weapon::energy_blade, 1);
}

void Infection::spawnPlayerClientSetup(int index) {
	wchar_t* playername = h2mod->get_player_name_from_index(index);
	//If player being spawned is LocalUser/Player
	if (wcscmp(playername, h2mod->get_local_player_name()) == 0) {

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
			h2mod->DisableWeaponPickup(true);
			h2mod->IndicatorVisibility(false);
		}
		else if (h2mod->get_local_team_index() == ZOMBIE_TEAM) {
			h2mod->set_unit_biped(BipedType::Elite, index);

			h2mod->DisableWeaponPickup(false);
			h2mod->IndicatorVisibility(true);
		}
	}
}

void Infection::spawnServerPlayerSetup(int index) {
	TRACE_GAME("[h2mod-infection] Spawn player server index=%", index);
	int unit_datum_index = h2mod->get_unit_datum_from_player_index(index);
	int unit_object = call_get_object(unit_datum_index, 3);

	if (unit_object && *(BYTE*)(unit_object + 0xAA) == 0) {
		//if the unit_object is not 0, the spawned object is "alive"

		TRACE_GAME("[h2mod-infection] Spawn player server index=%, unit team index=%d", index, h2mod->get_unit_team_index(unit_datum_index));
		if (h2mod->get_unit_team_index(unit_datum_index) == HUMAN_TEAM)	{
			Infection::setPlayerAsHuman(index);
		}

		if (h2mod->get_unit_team_index(unit_datum_index) == ZOMBIE_TEAM) {
			Infection::setPlayerAsZombie(index);
		}
	}
}

void Infection::infectPlayer(int unitDatumIndex, int playerIndex) {
	int unit_object = call_get_object(unitDatumIndex, 3);
	if (unit_object && h2mod->get_unit_team_index(unitDatumIndex) != ZOMBIE_TEAM 
		&& *(BYTE*)(unit_object + 0xAA) == 0) //check if object type is biped
	{
		//if we have a valid object and the object is not on the zombie team 
		wchar_t* playername = h2mod->get_player_name_from_index(h2mod->get_player_index_from_unit_datum(unitDatumIndex));

		TRACE_GAME("[h2mod-infection] Infected player, localName=%s, nameFromUnitDatumIndex=%s", h2mod->get_local_player_name(), playername);

		//If player being infected is LocalUser/Player
		if (wcscmp(playername, h2mod->get_local_player_name()) == 0) {
			TRACE_GAME("[h2mod-infection] Infected player, setting player as zombie");
			h2mod->set_local_team_index(0, ZOMBIE_TEAM); 
			h2mod->set_unit_biped(BipedType::Elite, playerIndex);
		}
		else {
			//if not, then this is a new zombie
			Infection::triggerSound(NEW_ZOMBIE_SOUND, 1000);
		}
	}
}

void Infection::infectPlayers(int unitDatumIndex, int playerIndex) {
	int unit_object = call_get_object(unitDatumIndex, 3);
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
	Infection::infectPlayers(this->getUnitDatumIndex(), this->getPlayerIndex());
}

void ZombieDeathHandler::onClient()
{
	TRACE_GAME("ZombieDeathhandler::OnClient() getUnitDatumIndex: %08X", this->getUnitDatumIndex());

	//infect client
	Infection::infectPlayer(this->getUnitDatumIndex(), this->getPlayerIndex());
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
	wchar_t* playername = h2mod->get_player_name_from_index(this->getPlayerIndex());
	TRACE_GAME("[h2mod-infection] Client pre spawn, playerIndex=%d, playerNameFromIndex=%s, localPlayerName=%s", this->getPlayerIndex(), playername, h2mod->get_local_player_name());
	//If player being spawned is LocalUser/Player
	if (wcscmp(playername, h2mod->get_local_player_name()) == 0)
	{
		TRACE_GAME("[h2mod-infection] Client pre spawn, found local player, current team = %d", h2mod->get_local_team_index());
		//Change biped if LocalUser is in GreenTeam
		if (h2mod->get_local_team_index() == ZOMBIE_TEAM)
		{
			TRACE_GAME("[h2mod-infection] Client is infected! switching bipeds: %i\r\n",this->getPlayerIndex());
			h2mod->set_unit_biped(BipedType::Elite, this->getPlayerIndex());
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
	TRACE_GAME("[h2mod-infection] Client deinit");
	Infection::resetWeaponInteractionAndEmblems();
}

void InfectionInitializer::onPeerHost()
{
	TRACE_GAME("[h2mod-infection] Peer host init");
	Infection::initClient();
	Infection::initHost();
	zombieIndex = -1;
	zombieIndex = Infection::calculateZombiePlayerIndex();
	TRACE_GAME("[h2mod-infection] Peer host calculated zombie index %d", zombieIndex);
	if (zombieIndex == -1) {
		TRACE_GAME("[h2mod-infection] Failed selecting a zombie!");
	} 
	else {
		TRACE_GAME("[h2mod-infection] Peer host setting player as human");
		//send out the team change packets to peers
		Infection::sendTeamChange();

		//mark this player as the zombie in the internal players structure
		Infection::setZombiePlayerStatus(zombieIndex);
	}
}

void InfectionInitializer::onDedi()
{
	TRACE_GAME("[h2mod-infection] Dedicated server init");
	Infection::initHost();
	//figure out who the zombie is
	zombieIndex = Infection::calculateZombiePlayerIndex();
	TRACE_GAME("[h2mod-infection] Dedicated host calculated zombie index %d", zombieIndex);
	if (zombieIndex == -1) {
		TRACE_GAME("[h2mod-infection] Failed selecting a zombie!");
	}
	else {
		TRACE_GAME("[h2mod-infection] Dedicated host setting player as human");
		//send out the team change packets to peers
		Infection::sendTeamChange();

		//mark this player as the zombie in the internal players structure
		Infection::setZombiePlayerStatus(zombieIndex);
	}
}

void InfectionInitializer::onClient()
{
	TRACE_GAME("[h2mod-infection] Client init");
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

void ZombieHandler::setUnitDatumIndex(int unitDatumIndex)
{
	this->unitDatumIndex = unitDatumIndex;
}

int ZombieHandler::getPlayerIndex()
{
	return this->playerIndex;
}

int ZombieHandler::getUnitDatumIndex()
{
	return this->unitDatumIndex;
}
