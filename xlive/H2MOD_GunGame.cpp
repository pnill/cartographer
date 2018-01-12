#include <windows.h>
#include "H2MOD_GunGame.h"
#include "H2MOD.h"
#include "xliveless.h"

extern int weapon_one;
extern int weapon_two;
extern int weapon_three;
extern int weapon_four;
extern int weapon_five;
extern int weapon_six;
extern int weapon_seven;
extern int weapon_eight;
extern int weapon_nine;
extern int weapon_ten;
extern int weapon_eleven;
extern int weapon_tweleve;
extern int weapon_thirteen;
extern int weapon_fourteen;
extern int weapon_fiffteen;
extern int weapon_sixteen;

unsigned int Weapon_ID[36] = { 0xE53D2AD8, 0xE5F02B8B, 0xE6322BCD, 0xE6AF2C4A,
0xE79B2D36,0xE8172DB2,0xE8382DD3,0xE8742E0F,
0xE8D32E6E,0xE9062EA1,0xE90C2EA7,0xE90C2EA7,
0xE9732F0E,0xE9F62F91,0xEAD83073,0xEB4230DD,
0xEB9E3139,0xEC3131CC,0xEC673202,0xEC9E3239,
0xECD63271,0xED3F32DA,0xED753310,0xEDA2333D,
0xEDD4336F,0xEE0933A4,0xEE3433CF,0xEE5233ED,
0xEE5F33FA,0xEE7B3416,0xEE993434,0xEE9E3439,
0xEED3346E,0xEEF1348C,0xEF1B34B6,0xF33838D2 };

void GunGame::Initialize()
{
	TRACE_GAME("[H2Mod-GunGame] : Initialize()");
	
	if (weapon_one != 0)
	{
		TRACE_GAME("[H2Mod-GunGame] : Intialize() - weapon_one: %i", weapon_one);
		TRACE_GAME("[H2Mod-GunGame] : Intialize() - weapon_one enum:  %08X", Weapon_ID[weapon_one]);
		TRACE_GAME("[H2Mod-GunGame] : Intialize() - weapon_two enum:  %08X", Weapon_ID[weapon_two]);
		this->level_weapon[0] = Weapon_ID[weapon_one];
		this->level_weapon[1] = Weapon_ID[weapon_two];
		this->level_weapon[2] = Weapon_ID[weapon_three];
		this->level_weapon[3] = Weapon_ID[weapon_four];
		this->level_weapon[4] = Weapon_ID[weapon_five];
		this->level_weapon[5] = Weapon_ID[weapon_six];
		this->level_weapon[6] = Weapon_ID[weapon_seven];
		this->level_weapon[7] = Weapon_ID[weapon_eight];
		this->level_weapon[8] = Weapon_ID[weapon_nine];
		this->level_weapon[9] = Weapon_ID[weapon_ten];
		this->level_weapon[10] = Weapon_ID[weapon_eleven];
		this->level_weapon[11] = Weapon_ID[weapon_tweleve];
		this->level_weapon[12] = Weapon_ID[weapon_thirteen];
		this->level_weapon[13] = Weapon_ID[weapon_fourteen];
		this->level_weapon[14] = Weapon_ID[weapon_fiffteen];
	
	
	}
	else
	{
		this->level_weapon[0] = Weapon::energy_blade_useless;
		this->level_weapon[1] = Weapon::needler;
		this->level_weapon[2] = Weapon::plasma_pistol;
		this->level_weapon[3] = Weapon::magnum;
		this->level_weapon[4] = Weapon::smg;
		this->level_weapon[5] = Weapon::plasma_rifle;
		this->level_weapon[6] = Weapon::brute_plasma_rifle;
		this->level_weapon[7] = Weapon::juggernaut_powerup;
		this->level_weapon[8] = Weapon::shotgun;
		this->level_weapon[9] = Weapon::brute_shot;
		this->level_weapon[10] = Weapon::covenant_carbine;
		this->level_weapon[11] = Weapon::battle_rifle;
		this->level_weapon[12] = Weapon::beam_rifle;
		this->level_weapon[13] = Weapon::sniper_rifle;
		this->level_weapon[14] = Weapon::rocket_launcher;
	}

	if (this->GunGamePlayers.size() > 0)
	{
		for (auto it = this->GunGamePlayers.begin(); it != this->GunGamePlayers.end(); ++it)
		{
			delete[] it->first;
		}
		this->GunGamePlayers.clear();
	}


	if (h2mod->NetworkPlayers.size() > 0)
	{
		for (auto it = h2mod->NetworkPlayers.begin(); it != h2mod->NetworkPlayers.end(); ++it)
		{
			GunGamePlayer* nPlayer = new GunGamePlayer;
			wcscpy(&nPlayer->PlayerName[0], it->first->PlayerName);
			nPlayer->level = 0;
			this->GunGamePlayers[nPlayer] = true;
		}
	}

	GunGamePlayer* host = new GunGamePlayer;
	wcscpy(&host->PlayerName[0], h2mod->get_local_player_name());
	host->level = 0;
	this->GunGamePlayers[host] = true;

}

void GunGame::AddPlayer(NetworkPlayer* netPlayer)
{
	GunGamePlayer *nPlayer = new GunGamePlayer;
	wcscpy(&nPlayer->PlayerName[0], netPlayer->PlayerName);
	nPlayer->level = 0;
	this->GunGamePlayers[nPlayer] = true;
}

void GunGame::PlayerDied(int unit_datum_index) // we need to start passing player index her for sanity.
{
	int unit_object = call_get_object(unit_datum_index, 3);
	if (unit_object)
	{
		call_unit_reset_equipment(unit_datum_index);

		int pIndex = h2mod->get_player_index_from_unit_datum(unit_datum_index);
		h2mod->set_unit_grenades(GrenadeType::Frag, 0, pIndex,1);
		h2mod->set_unit_grenades(GrenadeType::Plasma, 0, pIndex,1);
	}

}

void GunGame::NextRound()
{
}

void GunGame::SpawnPlayer(int PlayerIndex)
{
	wchar_t* pName = h2mod->get_player_name_from_index(PlayerIndex);

	TRACE_GAME("[H2Mod-GunGame]: SpawnPlayer(%i) : %ws ", PlayerIndex,pName);

	int unit_datum_index = h2mod->get_unit_datum_from_player_index(PlayerIndex);

	int unit_object = call_get_object(unit_datum_index, 3);

	if (unit_object)
	{
		int level = 0;
		for (auto it = this->GunGamePlayers.begin(); it != this->GunGamePlayers.end(); ++it)
		{
			TRACE_GAME("[H2Mod-GunGame]: SpawnPlayer comparing %ws to %ws to get level...", pName, it->first->PlayerName);

			if (wcscmp(pName, it->first->PlayerName) == 0)
			{
				level = it->first->level;
				TRACE_GAME("[H2Mod-GunGame]: SpawnPlayer found : %ws level: %i", pName, level);
			}
		}

		TRACE_GAME("[H2Mod-GunGame]: SpawnPlayer(%i) %ws - Level: %i", PlayerIndex, pName, level);

		int CurrentWeapon = this->level_weapon[level];

		if (level == 15)
		{
			TRACE_GAME("[H2Mod-GunGame]: %ws on frag grenade level!", pName);
			h2mod->set_unit_grenades(GrenadeType::Frag, 99, PlayerIndex, 1);
		}
		
		if (level == 16)
		{
			TRACE_GAME("[H2Mod-GunGame]: %ws on plasma grenade level!", pName);
			h2mod->set_unit_grenades(GrenadeType::Plasma, 99, PlayerIndex, 1);
		}

		if (level < 15)
		{
			GivePlayerWeapon(PlayerIndex, CurrentWeapon, 1);
		}
		
	}
}

int GunGame::GetCurrentWeapon(int PlayerIndex)
{
	TRACE("[H2Mod-GunGame]: GetCurrentWeapon(%i)", PlayerIndex);
	
	int level = 0;

	wchar_t* PlayerName = h2mod->get_player_name_from_index(PlayerIndex);
	for (auto it = this->GunGamePlayers.begin(); it != this->GunGamePlayers.end(); ++it)
	{
		if (wcscmp(PlayerName, it->first->PlayerName) == 0)
		{
			level = it->first->level;
		}
	}

	TRACE("[H2Mod-GunGame]: GetCurrentWeapon - player_level: %i", level);
	TRACE("[H2Mod-GunGame]: level_weapon-> %08X", this->level_weapon[level]);

	return this->level_weapon[level];
}

void GunGame::LevelUp(int PlayerIndex)
{
	wchar_t* PlayerName = h2mod->get_player_name_from_index(PlayerIndex);
	TRACE_GAME("[H2Mod-GunGame]: LevelUp(%i) : %ws", PlayerIndex,PlayerName);

	int level = 0;
	
	for (auto it = this->GunGamePlayers.begin(); it != this->GunGamePlayers.end(); ++it)
	{
		TRACE_GAME("[H2Mod-GunGame]: LevelUp() -  Searching for %ws against %ws in GunGamePlayers",PlayerName,it->first->PlayerName);

		if (wcscmp(PlayerName, it->first->PlayerName) == 0)
		{
			
			level = it->first->level;
			TRACE_GAME("[H2Mod-GunGame]: LevelUp() - Found %ws in GunGamePlayers map level: %i", it->first->PlayerName,level);
			it->first->level = level + 1;
		}
	}
	
	level++;

	TRACE_GAME("[H2Mod-GunGame]: LevelUp(%i) - new level: %i ", PlayerIndex, level);

	if (level == 15)
	{
		TRACE_GAME("[H2Mod-GunGame]: %ws Level 15 - Frag Grenades!",PlayerName);
		h2mod->set_unit_grenades(GrenadeType::Frag, 99, PlayerIndex,1);
	}

	if (level == 16)
	{
		TRACE_GAME("[H2Mod-GunGame]: %ws Level 16 - Plasma Grenades!", PlayerName);
		h2mod->set_unit_grenades(GrenadeType::Plasma, 99, PlayerIndex,1);
	}

	if (level < 15)
	{
		TRACE_GAME("[H2Mod-GunGame]: %ws on level %i giving them weapon...", PlayerName, level);

		int LevelWeapon = this->level_weapon[level];
		h2mod->set_unit_grenades(GrenadeType::Frag, 0, PlayerIndex,1);
		h2mod->set_unit_grenades(GrenadeType::Plasma, 0, PlayerIndex,1);
		GivePlayerWeapon(PlayerIndex, LevelWeapon, 1);
	}

}
