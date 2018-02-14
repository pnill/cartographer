#pragma once

class Infection
{
public:
	void PlayerInfected(int);
	void PreSpawn(int);
	void SpawnPlayer(int);
	void Initialize();
	void Deinitialize();
	void FindZombie();
	void NextRound();
private:
	void resetInfectedPlayers();
	void handleLocalHostZombieDetection(int zombiePeerIndex);
	void handlePeerZombieDetection(int zombiePeerIndex);
	int calculateZombieIndex();
};

