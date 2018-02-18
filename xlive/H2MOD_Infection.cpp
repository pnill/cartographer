#include "Globals.h"
#include <h2mod.pb.h>

bool infected_played = false;
bool first_spawn = true;

class ResetZombieHandler : public PlayerIterableHandler {
	void onPlayerIterated(Player* player) {
		player->setIsZombie(false);
	}
};

class SendZombieTeamChangeHandler : public PlayerIterableHandler {
public:
	SendZombieTeamChangeHandler(int zombieIndex) {
		this->zombieIndex = zombieIndex;
	}

	void onPlayerIterated(Player* player) {
		if (player->getPeerIndex() == this->zombieIndex) {
			TRACE_GAME("[H2Mod-Infection] Found Zombie: %ws", player->getPlayerName().c_str());
			player->setIsZombie(true);

			H2ModPacket teampak;
			teampak.set_type(H2ModPacket_Type_set_player_team);

			h2mod_set_team *set_team = teampak.mutable_h2_set_player_team();
			//TODO: add constant for this
			set_team->set_team(3);
			set_team->set_peerindex(player->getPeerIndex());

			char* SendBuf = new char[teampak.ByteSize()];
			teampak.SerializeToArray(SendBuf, teampak.ByteSize());

			network->networkCommand = SendBuf;
			network->sendCustomPacket(player->getPeerIndex());
			network->networkCommand = NULL;

			delete[] SendBuf;

			//stop iterating after we found the player to make a zombie
			this->stopIteration = true;
		}
	}

private:
	int zombieIndex;
};

class ZombiePreSpawnHandler : public PlayerIterableHandler {
public:
	ZombiePreSpawnHandler(int PlayerIndex) {
		this->PlayerIndex = PlayerIndex;
	}

	void onPlayerIterated(Player* player) {
		if (this->PlayerIndex != player->getPlayerIndex()) {
			//skip player indexes that don't match the given player id
			return;
		}
		int teamIndex = player->getPlayerTeam();
		if (teamIndex == 3) {
			h2mod->set_unit_biped(BipedType::Elite, PlayerIndex);
		}
		else {
			h2mod->set_unit_biped(BipedType::MasterChief, PlayerIndex);
		}
	}
private:
	int PlayerIndex;
};

class ZombieSpawnHandler : public PlayerIterableHandler {
public:
	ZombieSpawnHandler(int PlayerIndex) {
		this->PlayerIndex = PlayerIndex;
	}
	void onPlayerIterated(Player* player) {
		if (PlayerIndex != player->getPlayerIndex()) {
			return;
		}

		if (!h2mod->Server)
		{
			if (first_spawn == true)
			{
				std::unique_lock<std::mutex> lck(h2mod->sound_mutex);

				h2mod->SoundMap[L"sounds/infection.wav"] = 1000;

				h2mod->sound_cv.notify_one();


				first_spawn = false;
			}

			if (h2mod->get_local_team_index() == 3 && infected_played == false)
			{
				std::unique_lock<std::mutex> lck(h2mod->sound_mutex);

				h2mod->SoundMap[L"sounds/infected.wav"] = 500;

				h2mod->sound_cv.notify_one();

				infected_played = true;
			}
			if (h2mod->get_local_team_index() == 0)
			{
				h2mod->PatchWeaponsInteraction(true);
				h2mod->IndicatorVisibility(false);
			}
			else if (h2mod->get_local_team_index() == 3) {
				h2mod->PatchWeaponsInteraction(false);
				h2mod->IndicatorVisibility(true);
			}
		}

		if (gameManager->isHost() || h2mod->Server)
		{
			int unit_datum_index = h2mod->get_unit_datum_from_player_index(PlayerIndex);
			int unit_object = call_get_object(unit_datum_index, 3);

			if (unit_object) //if Spawned(alive)
			{

#pragma region HumansPowerUp
				if (h2mod->get_unit_team_index(unit_datum_index) == 0)
				{
					h2mod->set_unit_biped(BipedType::MasterChief, PlayerIndex);
					h2mod->set_unit_speed(1.0f, PlayerIndex);
					GivePlayerWeapon(PlayerIndex, Weapon::shotgun, 1);
					GivePlayerWeapon(PlayerIndex, Weapon::magnum, 0);
				}
#pragma endregion

#pragma region ZombiesPowerUp
				if (h2mod->get_unit_team_index(unit_datum_index) == 3)
				{
					h2mod->set_unit_biped(BipedType::Elite, PlayerIndex);
					h2mod->set_unit_speed(1.1f, PlayerIndex);
					GivePlayerWeapon(PlayerIndex, Weapon::energy_blade, 1);
				}
#pragma endregion

			}
		}
	}

private:
	int PlayerIndex;
};

class ZombieInfectionHandler : public PlayerIterableHandler {
public:
	ZombieInfectionHandler(int unit_datum_index, int unit_object, int pIndex) {
		this->unit_datum_index = unit_datum_index;
		this->unit_object = unit_object;
		this->pIndex = pIndex;
	}

	void onPlayerIterated(Player* player) {
		if (pIndex != player->getPlayerIndex()) {
			return;
		}
		TRACE_GAME("[H2Mod-Infection] PlayerInfected() player died: %ws", player->getPlayerName().c_str());
		if (!h2mod->Server) {
			//if we aren't the dedi, change biped and team
			TRACE_GAME("Setting biped to elite");
			h2mod->set_unit_biped(BipedType::Elite, pIndex);
			if (h2mod->get_unit_team_index(unit_datum_index) != 3)
			{
				TRACE_GAME("Setting team to zombie team (green team)");
				h2mod->set_local_team_index(3);
			}
		}
		if (gameManager->isHost() || h2mod->Server)
		{
			//Add the Dead Player* to The List of Infected	
			TRACE_GAME("[H2Mod-Infection] PlayerInfected() is %ws infected!", player->getPlayerName().c_str());
			player->setIsZombie(true);
			if (h2mod->get_unit_team_index(unit_datum_index) == 3)
			{
				TRACE_GAME("Recording zombie status on host only");
				//TODO: shouldn't we do this for every player, not just host?
				call_unit_reset_equipment(unit_datum_index); //Take away local players weapons
			}
		}
	}
private:
	int unit_object;
	int pIndex;
	int unit_datum_index;
};

void Infection::resetInfectedPlayers() {
	ResetZombieHandler* zombieHandler = new ResetZombieHandler();
	players->iteratePlayers(zombieHandler);
	delete zombieHandler;
}

void Infection::handleLocalHostZombieDetection(int zombiePeerIndex) {
	if (!h2mod->Server)
	{
		TRACE_GAME("[H2Mod-Infection] - Zombie: %i", zombiePeerIndex);

		if (zombiePeerIndex == 0)
		{
			TRACE_GAME("[H2Mod-Infection] - Local host is zombie!");
			h2mod->set_local_team_index(3);
		}
		else
		{
			TRACE_GAME("[H2Mod-Infection] - Local host is a human!");
			h2mod->set_local_team_index(0);
		}
	}
}

void Infection::handlePeerZombieDetection(int zombiePeerIndex) {
	SendZombieTeamChangeHandler* zombieHandler = new SendZombieTeamChangeHandler(zombiePeerIndex);
	players->iteratePlayers(zombieHandler);
	delete zombieHandler;
}

int Infection::calculateZombieIndex() {
	int max = players->getPlayerCount() - 1;
	int min = 0;
	int randNum = rand() % (max - min + 1) + min;
	return randNum; //Random Alpha Zombie Index
}

void Infection::FindZombie()
{
	TRACE_GAME("[H2MOD-Infection] - FindZombie()");
	int zombieIndex = calculateZombieIndex();

	handleLocalHostZombieDetection(zombieIndex);
	if (zombieIndex == 0 && !h2mod->Server) {
		//if this is the peer host and the zombie is localhost, don't send out zombie change packets
		return;
	}
	handlePeerZombieDetection(zombieIndex);
}

void Infection::Initialize()
{
	#pragma region H2v Stuffs
	if (!h2mod->Server)
	{
		h2mod->DisableSound(SoundType::Slayer);
		h2mod->DisableSound(SoundType::GainedTheLead);
		h2mod->DisableSound(SoundType::LostTheLead);
		h2mod->DisableSound(SoundType::TeamChange);

		infected_played = false;
		first_spawn = true;

		//Change Local Player*'s Team to Human if Not in Green
		//(In case player wants to start as Alpha Zombie leave him green)
		if (h2mod->get_local_team_index() != 3)
			h2mod->set_local_team_index(0);
	}
	#pragma endregion

	#pragma region Host/Dedi Work
	if (gameManager->isHost() || h2mod->Server)
	{
		TRACE_GAME("[H2Mod-Infection] - Initializing!");
		//TODO: add logic to get # of infected players on server
		//TRACE_GAME("[H2Mod-Infection] - this->infected_players.size(): %i", this->infected_players.size());

		h2mod->set_unit_speed_patch(true); //Applying SpeedCheck fix
		//h2mod->PatchNewRound(true);	  //New OnRoundPatch
		
		//reset all players to not zombies
		resetInfectedPlayers();
		//Find our Alpha Zombie and Turn rest to Humans
		FindZombie();
	}
#pragma endregion 

}

void Infection::Deinitialize()
{
	h2mod->PatchWeaponsInteraction(true);
	h2mod->IndicatorVisibility(true);

	if (gameManager->isHost())
	{
		TRACE_GAME("[H2Mod-Infection] - Deinitializing!");
		h2mod->set_unit_speed_patch(false);
		//h2mod->PatchNewRound(false); //it's not even called so doesn't worth to deinitialize
	}
}


void Infection::PreSpawn(int PlayerIndex)
{
	TRACE_GAME("[H2Mod-Infection] - Prespawn( %i ) ", PlayerIndex);
	ZombiePreSpawnHandler* zombiePreSpawnHandler = new ZombiePreSpawnHandler(PlayerIndex);
	players->iteratePlayers(zombiePreSpawnHandler);
	delete zombiePreSpawnHandler;
}

void Infection::SpawnPlayer(int PlayerIndex)
{
	TRACE_GAME("[H2Mod-Infection] - SpawnPlayer(%i)", PlayerIndex);
	ZombieSpawnHandler* zombieSpawnHandler = new ZombieSpawnHandler(PlayerIndex);
	players->iteratePlayers(zombieSpawnHandler);
	delete zombieSpawnHandler;
}

void Infection::PlayerInfected(int unit_datum_index)
{
	TRACE_GAME("[H2Mod-Infection] - PlayerInfected()", unit_datum_index);

	int unit_object = call_get_object(unit_datum_index, 3);
	if (unit_object)
	{
		int pIndex = h2mod->get_player_index_from_unit_datum(unit_datum_index);
		ZombieInfectionHandler* zombieHandler = new ZombieInfectionHandler(unit_datum_index, unit_object, pIndex);
		players->iteratePlayers(zombieHandler);
		delete zombieHandler;
	}
}

void Infection::NextRound()
{
	if (!h2mod->Server)
	{
		h2mod->set_local_team_index(0);
		h2mod->PatchWeaponsInteraction(true);
	}

	if (gameManager->isHost())
	{
		TRACE_GAME("[H2Mod-Infection] - Starting New Round!");
		//Reset Infected Players List(For a New Round)
		resetInfectedPlayers();
		//Find our Alpha Zombie and Turn rest to Humans
		FindZombie();
	}
}
