#pragma once

#include "Hook.h"
#include "H2mod.h"
#include <unordered_map>

class GunGamePlayer
{
public:
	wchar_t PlayerName[16];
	int level = 0;
};

class GunGame
{
public:
	void Initialize();
	void LevelUp(int PlayerIndex);
	void SpawnPlayer(int PlayerIndex);
	int GetCurrentWeapon(int PlayerIndex);
	void PlayerDied(int unit_datum_index); // We need to start using PlayerIndex here for sanity.
	void NextRound();
	void AddPlayer(NetworkPlayer* netPlayer);

	std::unordered_map<int, int> level_weapon;
	std::unordered_map<int, int> player_level;
	std::unordered_map<GunGamePlayer*, bool> GunGamePlayers;

};
