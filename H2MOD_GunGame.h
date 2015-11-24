#pragma once
#ifndef H2GUNGAME_H
#define H2GUNGAME_H
#include "Hook.h"
#include <unordered_map>

class GunGame
{
public:
	void Initialize();
	void LevelUp(int PlayerIndex);
	void SpawnPlayer(int PlayerIndex);
	int GetCurrentWeapon(int PlayerIndex);
	void LevelDown(int PlayerIndex);
	void PlayerDied(int unit_datum_index); // We need to start using PlayerIndex here for sanity.

	std::unordered_map<int, int> level_weapon;
	std::unordered_map<int, int> player_level;
};
#endif