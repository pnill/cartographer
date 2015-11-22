#pragma once
#ifndef H2MOD_H
#define H2MOD_H
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
	//void GivePlayerWeapon(int PlayerIndex, int WeaponTagIndex);
	//int GetWeapon(int PlayerIndex);

	std::unordered_map<int, int> level_weapon;
	std::unordered_map<int, int> player_level;
};

class H2MOD
{
public:
		void Initialize();
		int SetGravity(float);
		int unit_kill(int);
		int get_unit_from_player_index(int);
		int get_unit_datum_from_player_index(int);
		int unit_set_active_camo(int);
		int camera_set_fov(float, short);
		int object_placement_data_new(char*, int, signed int, int);
		signed int object_new(int);
		bool assign_equipment_to_unit(int unit, int weapon_id, short a3);
		void give_player_weapon(int unit,int weapon_id);
		void ApplyHooks();
		DWORD GetBase();
		
		BOOL Server;
private:
		DWORD Base;
};


extern H2MOD* h2mod;


#endif