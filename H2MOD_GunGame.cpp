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

int Weapon_ID[36] = { 0xE53D2AD8, 0xE5F02B8B, 0xE6322BCD, 0xE6AF2C4A,
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
	TRACE("[GunGame] : Initialize()");
	
	if (weapon_one != 0)
	{
		TRACE("[GunGame] : Intialize() - weapon_one: %i", weapon_one);
		TRACE("[GunGame] : Intialize() - weapon_one enum:  %08X", Weapon_ID[weapon_one]);
		TRACE("[GunGame] : Intialize() - weapon_two enum:  %08X", Weapon_ID[weapon_two]);
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
		this->level_weapon[15] = Weapon_ID[weapon_sixteen];
	
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
		this->level_weapon[15] = Weapon::energy_blade;
	}

	this->player_level[0] = 0;
	this->player_level[1] = 0;
	this->player_level[2] = 0;
	this->player_level[3] = 0;
	this->player_level[4] = 0;
	this->player_level[5] = 0;
	this->player_level[6] = 0;
	this->player_level[7] = 0;
	this->player_level[8] = 0;
	this->player_level[9] = 0;
	this->player_level[10] = 0;
	this->player_level[11] = 0;
	this->player_level[12] = 0;
	this->player_level[13] = 0;
	this->player_level[14] = 0;
	this->player_level[15] = 0;

}

void GunGame::PlayerDied(int unit_datum_index) // we need to start passing player index her for sanity.
{
	int unit_object = call_get_object(unit_datum_index, 3);
	if (unit_object)
	{
		call_unit_reset_equipment(unit_datum_index);

		*(BYTE*)((BYTE*)unit_object + 0x252) = 0; // frag grenades
		*(BYTE*)((BYTE*)unit_object + 0x253) = 0; // plasma grenades
	}

}

void GunGame::SpawnPlayer(int PlayerIndex)
{
	TRACE("[GunGame]: SpawnPlayer(%i)", PlayerIndex);

	int unit_datum_index = h2mod->get_unit_datum_from_player_index(PlayerIndex);

	int unit_object = call_get_object(unit_datum_index, 3);



	TRACE("[GunGame]: SpawnPlayer - unit_object: %08X", unit_object);

	if (unit_object)
	{
		*(BYTE*)((BYTE*)unit_object + 0x13C) = 0;
		int level = this->player_level[PlayerIndex];
		TRACE("[GunGame]: SpawnPlayer - Level: %i", level);

		if (level > 15)
		{
			TRACE("[GunGame]: SpawnPlayer - Level > 15");

			call_unit_reset_equipment(unit_datum_index);

			if (level == 16)
			{
				TRACE("[GunGame]: SpawnPlayer - Level == 16");
				*(BYTE*)((BYTE*)unit_object + 0x252) = 50; // frag grenades
			}
			if (level == 17)
			{
				TRACE("[GunGame]: SpawnPlayer - Level == 17");
				*(BYTE*)((BYTE*)unit_object + 0x253) = 50; // plasma grenades	
			}

		}
		else
		{
			*(BYTE*)((BYTE*)unit_object + 0x252) = 0; // frag grenades
			*(BYTE*)((BYTE*)unit_object + 0x253) = 0; // plasma grenades

			int CurrentWeapon = GetCurrentWeapon(PlayerIndex);
			GivePlayerWeapon(PlayerIndex, CurrentWeapon);

			if (level == 0 || level == 1)
			{
				*(float*)((float*)unit_object + 0x2CC) = 900.0f; // set camo timer 9000.0f;
				*(BYTE*)((BYTE*)unit_object + 0x138) = 0x0B; // set camo on

			}

		}

	}
}

int GunGame::GetCurrentWeapon(int PlayerIndex)
{
	TRACE("[GunGame]: GetCurrentWeapon(%i)", PlayerIndex);
	int level = this->player_level[PlayerIndex];
	TRACE("[GunGame]: GetCurrentWeapon - player_level: %i", level);
	TRACE("[GunGame]: level_weapon-> %08X", this->level_weapon[level]);

	return this->level_weapon[level];
}

void GunGame::LevelDown(int PlayerIndex)
{
	TRACE("[GunGame]: LevelDown( %i )", PlayerIndex);
	int level = this->player_level[PlayerIndex];
	if (level > 0)
	{
		this->player_level[PlayerIndex]--;


		if (level > 15)
		{

			TRACE("[GunGame]: LevelDown - level: %i", level);

			int unit_datum_index = h2mod->get_unit_datum_from_player_index(PlayerIndex);


			call_unit_reset_equipment(unit_datum_index);



			int unit_object = call_get_object(unit_datum_index, 3);
			if (unit_object)
			{

				call_unit_reset_equipment(unit_datum_index);

				if (level == 0 || level == 1)
				{
					*(float*)((float*)unit_object + 0x2CC) = 900.0f; // set camo timer 9000.0f;
					*(BYTE*)((BYTE*)unit_object + 0x138) = 0x0B; // set camo on
					*(float*)((float*)unit_object + 0x2CC) = 900.0f; // set camo timer 9000.0f;

				}

				if (level == 16)
				{
					TRACE("[GunGame]: LevelUp level == 16, unit_object: %08X", unit_object);
					*(BYTE*)((BYTE*)unit_object + 0x252) = 50; // frag grenades
				}
				if (level == 17)
				{
					TRACE("[GunGame]: LevelUp level == 17");
					*(BYTE*)((BYTE*)unit_object + 0x253) = 50; // plasma grenades
				}

				//*(BYTE*)((BYTE*)unit_object + 0x10A) = 0x40;

			}
		}
		else
		{
			GivePlayerWeapon(PlayerIndex, this->level_weapon[level]);
		}
	}
}

void GunGame::LevelUp(int PlayerIndex)
{
	TRACE("[GunGame]: LevelUp( %i )", PlayerIndex);
	this->player_level[PlayerIndex]++;

	int level = this->player_level[PlayerIndex];
	if (level > 15)
	{

		TRACE("[GunGame]: LevelUp - level: %i", level);

		int unit_datum_index = h2mod->get_unit_datum_from_player_index(PlayerIndex);


		call_unit_reset_equipment(unit_datum_index);



		int unit_object = call_get_object(unit_datum_index, 3);
		if (unit_object)
		{

			call_unit_reset_equipment(unit_datum_index);

			if (level == 0 || level == 1)
			{
				*(float*)((float*)unit_object + 0x2CC) = 900.0f; // set camo timer 9000.0f;
				*(BYTE*)((BYTE*)unit_object + 0x138) = 0x0B; // set camo on
				*(float*)((float*)unit_object + 0x2CC) = 900.0f; // set camo timer 9000.0f;

			}

			if (level == 16)
			{
				TRACE("[GunGame]: LevelUp level == 16, unit_object: %08X", unit_object);
				
				*(BYTE*)((BYTE*)unit_object + 0x252) = 50; // frag grenades
			}
			if (level == 17)
			{
				TRACE("[GunGame]: LevelUp level == 17");
				*(BYTE*)((BYTE*)unit_object + 0x253) = 50; // plasma grenades
			}
			
			if (level == 18)
			{
				TRACE("[GunGame]: LevelUp level == 18");
				this->player_level[PlayerIndex] = 0;
				GivePlayerWeapon(PlayerIndex, this->level_weapon[0]);
			}

			//*(BYTE*)((BYTE*)unit_object + 0x10A) = 0x40;
		}
	}
	else
	{
		GivePlayerWeapon(PlayerIndex, this->level_weapon[level]);
	}
}
