#include <windows.h>
#include "H2MOD_GunGame.h"
#include "H2MOD.h"
#include "xliveless.h"


void GunGame::Initialize()
{
	TRACE("[GunGame] : Initialize()");

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

			//*(BYTE*)((BYTE*)unit_object + 0x10A) = 0x40;
		}
	}
	else
	{
		GivePlayerWeapon(PlayerIndex, this->level_weapon[level]);
	}
}
