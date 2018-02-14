#pragma once

#include "Hook.h"
#include "H2mod.h"
#include <unordered_map>

class GunGame
{
public:
	void resetPlayerLevels();
	void Initialize();
	void LevelUp(int PlayerIndex);
	void SpawnPlayer(int PlayerIndex);
	int GetCurrentWeapon(int PlayerIndex);
	void PlayerDied(int unit_datum_index); // We need to start using PlayerIndex here for sanity.
	void NextRound();
	void set_unit_grenades(BYTE type, BYTE count, int pIndex, bool bReset);

	std::unordered_map<int, int> level_weapon;
	std::unordered_map<int, int> player_level;
};
