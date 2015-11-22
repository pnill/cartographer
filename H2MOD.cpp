#include <windows.h>
#include <iostream>
#include <sstream>
#include "H2MOD.h"
#include "Network.h"
#include "xliveless.h"

H2MOD *h2mod = new H2MOD();
GunGame *gg = new GunGame();
HMODULE base;

//int(__cdecl *kill_player)(int) = (int(__cdecl*)(int))GetModuleHandle(L"halo2.exe")+0x13B514;
int(__cdecl* set_gravity)(float);
int(__cdecl* engine_unit_kill)(int unit);
int(__cdecl* engine_set_camera_fov)(float fov, short index);
int(__cdecl* engine_set_active_camo)(int unit, bool state, float time);
//int(__cdecl* engine_object_placement_data_new)(char*, int, signed int, int);
signed int(__cdecl* engine_object_new)(int);
bool(__cdecl* engine_assign_equipment_to_unit)(int, int, short);


int __cdecl call_get_object(signed int object_datum_index, int object_type);

int __cdecl call_get_object(signed int object_datum_index, int object_type)
{
	TRACE("call_get_object( %08X,%08X )", object_datum_index, object_type);

	typedef int(__cdecl *get_object)(signed int object_datum_index, int object_type);
	get_object pget_object = (get_object)((char*)h2mod->GetBase() + 0x1304E3);

	return pget_object(object_datum_index, object_type);
}

signed int __cdecl call_unit_doesnt_drop_items(int unit_datum_index);

signed int __cdecl call_unit_doesnt_drop_items(int unit_datum_index)
{
	typedef int(__cdecl *unit_doesnt_drop_items)(int unit_datum_index);
	unit_doesnt_drop_items punit_doesnt_drop_items = (unit_doesnt_drop_items)((char*)h2mod->GetBase() + 0x184DD1);
	
	return punit_doesnt_drop_items(unit_datum_index);
}

int __cdecl call_unit_reset_equipment(int unit);

int __cdecl call_unit_reset_equipment(int unit)
{
	typedef int(__cdecl *unit_reset_equipment)(int unit);
	unit_reset_equipment punit_reset_equipment = (unit_reset_equipment)(((char*)h2mod->GetBase()) + 0x1441E0);

	if (unit != -1 && unit != 0)
	{
		return punit_reset_equipment(unit);
	}

	return 0;
}

int __cdecl call_hs_object_destroy(int object_index);

int __cdecl call_hs_object_destroy(int object_index)
{
	typedef int(__cdecl *hs_object_destroy)(int object_index);
	hs_object_destroy phs_object_destroy = (hs_object_destroy)(((char*)h2mod->GetBase()) + 0x136005);

	return phs_object_destroy(object_index);
}

signed int __cdecl call_unit_inventory_next_weapon(unsigned short unit);

signed int __cdecl call_unit_inventory_next_weapon(unsigned short unit)
{
	typedef signed int(__cdecl *unit_inventory_next_weapon)(unsigned short unit);
	unit_inventory_next_weapon punit_inventory_next_weapon = (unit_inventory_next_weapon)(((char*)h2mod->GetBase()) + 0x139E04);

	return punit_inventory_next_weapon(unit);
}

bool __cdecl call_assign_equipment_to_unit(int uint, int object_index, short unk);

bool __cdecl call_assign_equipment_to_unit(int unit, int object_index, short unk)
{
	typedef bool(__cdecl *assign_equipment_to_unit)(int unit, int object_index, short unk);
	assign_equipment_to_unit passign_equipment_to_unit = (assign_equipment_to_unit)(((char*)h2mod->GetBase()) + 0x1442AA);


	return passign_equipment_to_unit(unit, object_index, unk);
}

int __cdecl call_object_placement_data_new(void*, int, int, int);

int __cdecl call_object_placement_data_new(void* s_object_placement_data, int object_definition_index, int object_owner, int unk)
{

	typedef int(__cdecl *object_placement_data_new)(void*, int, int, int);
	object_placement_data_new pobject_placement_data_new = (object_placement_data_new)(((char*)h2mod->GetBase()) + 0x132163);


	return pobject_placement_data_new(s_object_placement_data, object_definition_index, object_owner, unk);
}

signed int __cdecl call_object_new(void*);

signed int __cdecl call_object_new(void* pObject)
{
	typedef int(__cdecl *object_new)(void*);
	object_new pobject_new = (object_new)(((char*)h2mod->GetBase()) + 0x136CA7);

	return pobject_new(pObject);
}



void GivePlayerWeapon(int PlayerIndex, int WeaponId)
{
	TRACE("[GunGame]: GivePlayerWeapon(%i,%08X)", PlayerIndex, WeaponId);

	if (base != nullptr)
	{
		int unit_datum = h2mod->get_unit_datum_from_player_index(PlayerIndex);

		TRACE("GivePlayerWeapon: %08X", unit_datum);

		if (unit_datum != -1 && unit_datum != 0)
		{
			char* nObject = new char[0xC4];
			DWORD dwBack;
			VirtualProtect(nObject, 0xC4, PAGE_EXECUTE_READWRITE, &dwBack);

			call_object_placement_data_new(nObject, WeaponId, unit_datum, 0);

			int object_index = call_object_new(nObject);

			call_unit_reset_equipment(unit_datum);

			call_assign_equipment_to_unit(unit_datum, object_index, 1);
		}
	}
}


enum Weapon
{
	//Unknown = 0x2,//36
	//None = 0x1,//37
	chaingun_turret = 0xE53D2AD8,//0
	gauss_turret = 0xE5F02B8B,//1
	warthog_horn = 0xE6322BCD,//2
	banshee_gun = 0xE6AF2C4A,//3
	ghost_gun = 0xE75F2CFA,//4
	h_turret_ap = 0xE79B2D36,//5
	cannon_turret = 0xE8172DB2,//6
	cannon_turret_mp = 0xE8382DD3,//7
	big_needler = 0xE8742E0F,//8
	mortar_turret = 0xE8D32E6E,//9
	minigun_turret = 0xE9062EA1,//10
	mortar_turret_mp = 0xE90C2EA7,//11
	plasma_turret = 0xE9732F0E,//12
	plasma_cannon = 0xE9F62F91,//13
	magnum = 0xEAD83073,//14
	needler = 0xEB4230DD,//15
	plasma_pistol = 0xEB9E3139,//16
	battle_rifle = 0xEC3131CC,//17
	covenant_carbine = 0xEC673202,//18
	plasma_rifle = 0xEC9E3239,//19
	shotgun = 0xECD63271,//20
	smg = 0xED3F32DA,//21
	sniper_rifle = 0xED753310,//22
	flak_cannon = 0xEDA2333D,//23
	rocket_launcher = 0xEDD4336F,//24
	brute_shot = 0xEE0933A4,//25
	assault_bomb = 0xEE3433CF,//26
	ball = 0xEE5233ED, //27
	flag = 0xEE5F33FA,//28
	energy_blade = 0xEE7B3416, //29
	energy_blade_useless = 0xEE993434, //30
	beam_rifle = 0xEE9E3439, //31
	brute_plasma_rifle = 0xEED3346E,//32
	sentinel_aggressor_beam = 0xEEF1348C,//33
	smg_silenced = 0xEF1B34B6,//34
	juggernaut_powerup = 0xF33838D2 //35
};

void GunGame::Initialize()
{
	base = (HMODULE)h2mod->GetBase();
	
	this->level_weapon[0] = Weapon::energy_blade_useless;
	this->level_weapon[1] = Weapon::needler;
	this->level_weapon[2] = Weapon::plasma_pistol;
	this->level_weapon[3] = Weapon::magnum;
	this->level_weapon[4] = Weapon::smg;
	this->level_weapon[5] = Weapon::plasma_rifle;
	this->level_weapon[6] = Weapon::brute_plasma_rifle;
	//this->level_weapon[7] = Weapon::sentinel_aggressor_beam;
	this->level_weapon[7] = Weapon::ball;
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
		
			//GivePlayerWeapon(PlayerIndex, Weapon::assault_bomb);

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

				*(BYTE*)((BYTE*)unit_object + 0x10A) = 0x40;

				//GivePlayerWeapon(PlayerIndex, Weapon::assault_bomb);
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

			*(BYTE*)((BYTE*)unit_object + 0x10A) = 0x40;

			//GivePlayerWeapon(PlayerIndex, Weapon::assault_bomb);
		}
	}
	else
	{
		GivePlayerWeapon(PlayerIndex, this->level_weapon[level]);
	}
}

void H2MOD::give_player_weapon(int unit, int weapon_id)
{
	char* nObject = new char[0xC4];
	DWORD dwBack;
	VirtualProtect(nObject, 0xC4, PAGE_EXECUTE_READWRITE, &dwBack);
	TRACE("nObject: %08X", nObject);

	this->object_placement_data_new(nObject, weapon_id, -1, 0);

	this->object_new((int)nObject);

	this->assign_equipment_to_unit(unit, weapon_id, 1);

}

bool H2MOD::assign_equipment_to_unit(int unit, int weapon_id, short a3)
{
	engine_assign_equipment_to_unit = (bool(__cdecl*)(int, int, short))(this->GetBase() + 0x5D729);

	return engine_assign_equipment_to_unit(unit, weapon_id, a3);
}
int H2MOD::SetGravity(float Gravity)
{
	set_gravity = (int(__cdecl *)(float))(this->GetBase() + 0xB3C00);

	return set_gravity(Gravity);
}

int H2MOD::object_placement_data_new(char* objptr, int objtype, signed int unk1, int unk2)
{
	//	engine_object_placement_data_new = (int(__cdecl*)(char*,int,signed int,int))(this->GetBase() + 0x5D6DD);

	return 0;
	//	return engine_object_placement_data_new(objptr, objtype, unk1, unk2);
}

signed int H2MOD::object_new(int objptr)
{
	engine_object_new = (signed int(__cdecl*)(int))(this->GetBase() + 0x136CA7);

	return engine_object_new(objptr);
}

int H2MOD::get_unit_datum_from_player_index(int pIndex)
{
	int unit = 0;
	DWORD player_table_ptr = *(DWORD*)(this->GetBase() + 0x004A8260);
	player_table_ptr += 0x44;

	unit = (int)*(int*)(*(DWORD*)player_table_ptr + (pIndex * 0x204) + 0x28);
	
	return unit;
}

int H2MOD::get_unit_from_player_index(int pIndex)
{
	int unit = 0;
	DWORD player_table_ptr = *(DWORD*)(this->GetBase() + 0x004A8260);
	player_table_ptr += 0x44;

	unit = (int)*(short*)(*(DWORD*)player_table_ptr + (pIndex * 0x204) + 0x28);

	return unit;
}

int H2MOD::unit_set_active_camo(int unit)
{
	engine_set_active_camo = (int(__cdecl*)(int, bool, float))(this->GetBase() + 0x184555);

	return engine_set_active_camo(unit, true, 90000.0f);
}

int H2MOD::unit_kill(int unit)
{
	engine_unit_kill = (int(__cdecl*)(int))(this->GetBase() + 0x13B514);


	return engine_unit_kill(unit);
}

int H2MOD::camera_set_fov(float fov, short index)
{
	engine_set_camera_fov = (int(__cdecl *)(float, short))(this->GetBase() + 0x97CC9);

	return engine_set_camera_fov(fov, index);
}


typedef bool(__cdecl *spawn_player)(int a1);
spawn_player pspawn_player;

typedef int(__cdecl *game_difficulty_get_real_evaluate)(int a1, int a2);
game_difficulty_get_real_evaluate pgame_difficulty_get_real_evaluate;

typedef int(__cdecl *map_intialize)(int a1);
map_intialize pmap_initialize;

typedef char(__cdecl *player_death)(int unit_datum_index, int a2, char a3, char a4);
player_death pplayer_death;

char __cdecl player_death_hook(int unit_datum_index, int a2, char a3, char a4)
{
	TRACE("player_death_hook(%08X)", unit_datum_index);
	
	int unit_object = call_get_object(unit_datum_index, 3);
	if (unit_object)
	{
		call_unit_reset_equipment(unit_datum_index);

		*(BYTE*)((BYTE*)unit_object + 0x252) = 0; // frag grenades
		*(BYTE*)((BYTE*)unit_object + 0x253) = 0; // plasma grenades
	}

	return pplayer_death(unit_datum_index, a2, a3, a4);
}

typedef void(__stdcall *update_player_score)(void* thisptr, unsigned short a2, int a3, int a4, int a5, char a6);
update_player_score pupdate_player_score;

void __stdcall update_player_score_hook(void* thisptr, unsigned short a2, int a3, int a4, int a5, char a6)
{
	TRACE("update_player_score_hook ( thisptr: %08X, a2: %i, a3: %i, a4: %i, a5: %i, a6: %i )", thisptr, a2, a3, a4, a5, a6);
	if (a5 == 7) //player got a kill?
	{
		int PlayerIndex = a2;
		gg->LevelUp(PlayerIndex);
	}

	if (a5 == -1 && a4 == -1)
	{
		int PlayerIndex = a2;
		gg->LevelDown(PlayerIndex);
	}
	return pupdate_player_score(thisptr, a2, a3, a4, a5, a6);
}

bool first_load = true;
bool bcoop = false;

// This whole hook is called every single time a map loads,
// I've written a PHP script to compile the byte arrays due to the fact comparing unicode is a bitch.
// Basically if we have a singleplayer map we set bcoop = true so that the coop variables are setup.

int __cdecl map_initialize_hook(int a1)
{
	bcoop = false;
	
	DWORD game_globals = *(DWORD*)(((char*)h2mod->GetBase()) + 0x482D3C);
	BYTE* engine_mode = (BYTE*)(game_globals + 8);

	BYTE quarntine_zone[86] = { 0x73, 0x00, 0x63, 0x00, 0x65, 0x00, 0x6E, 0x00, 0x61, 0x00, 0x72, 
								0x00, 0x69, 0x00, 0x6F, 0x00, 0x73, 0x00, 0x5C, 0x00, 0x6D, 0x00, 
								0x75, 0x00, 0x6C, 0x00, 0x74, 0x00, 0x69, 0x00, 0x5C, 0x00, 0x30, 
								0x00, 0x36, 0x00, 0x62, 0x00, 0x5F, 0x00, 0x66, 0x00, 0x6C, 0x00, 
								0x6F, 0x00, 0x6F, 0x00, 0x64, 0x00, 0x7A, 0x00, 0x6F, 0x00, 0x6E, 
								0x00, 0x65, 0x00, 0x5C, 0x00, 0x30, 0x00, 0x36, 0x00, 0x62, 0x00, 
								0x5F, 0x00, 0x66, 0x00, 0x6C, 0x00, 0x6F, 0x00, 0x6F, 0x00, 0x64, 
								0x00, 0x7A, 0x00, 0x6F, 0x00, 0x6E, 0x00, 0x65, 0x00 };

	BYTE main_menu[60] = { 0x73, 0x00, 0x63, 0x00, 0x65, 0x00, 0x6E, 0x00, 0x61, 0x00, 0x72, 
						   0x00, 0x69, 0x00, 0x6F, 0x00, 0x73, 0x00, 0x5C, 0x00, 0x75, 0x00, 0x69, 0x00, 
						   0x5C, 0x00, 0x6D, 0x00, 0x61, 0x00, 0x69, 0x00, 0x6E, 0x00, 0x6D, 0x00, 0x65, 
						   0x00, 0x6E, 0x00, 0x75, 0x00, 0x5C, 0x00, 0x6D, 0x00, 0x61, 0x00, 0x69, 0x00, 
						   0x6E, 0x00, 0x6D, 0x00, 0x65, 0x00, 0x6E, 0x00, 0x75, 0x00 };

	if (!memcmp(main_menu, (BYTE*)0x300017E0, 60))
	{
		DWORD game_globals = *(DWORD*)(((char*)h2mod->GetBase()) + 0x482D3C);
		BYTE* garbage_collect = (BYTE*)(game_globals + 0xC);
		*(garbage_collect) = 1;
		TRACE("Loading mainmenu");
	}

	if (!memcmp(quarntine_zone, (BYTE*)0x300017E0, 86) && *(engine_mode) == 2 ) // check the map and if we're loading a multiplayer game (We don't want to fuck up normal campaign)
	{
		bcoop = true; // set coop mode to true because we're loading an SP map and we're trying to do so with the multiplayer engine mode set.
		TRACE("Loading quarantine zone");
	}

	if (bcoop == true)
	{
	

		DWORD game_globals = *(DWORD*)(((char*)h2mod->GetBase()) + 0x482D3C);
		BYTE* coop_mode = (BYTE*)(game_globals + 0x2a4);
		BYTE* engine_mode = (BYTE*)(game_globals + 8);
		BYTE* garbage_collect = (BYTE*)(game_globals + 0xC);
		*(engine_mode) = 1;
		bcoop = true;

		if (first_load == true)
		{
			*(garbage_collect) = 4; // This is utterly broken and causes weird issues when other players start joining.
		}
		else
		{
			*(garbage_collect) = 1; // This has to be left at 5 for it to work, for some reason after the first time the host loads it seems to resolve some issues with weapons creation.
			first_load = false;
		}

	}

	TRACE("map being loaded: %s", (wchar_t*)0x300017E0);
	gg->Initialize();
	return pmap_initialize(a1);
}

bool __cdecl spawn_player_hook(int a1)
{
	TRACE("Player spawn a1: %08X", a1);
	


	/* hacky coop fixes*/
	DWORD game_globals = *(DWORD*)(((char*)h2mod->GetBase()) + 0x482D3C);
	BYTE* garbage_collect = (BYTE*)(game_globals + 0xC);
	BYTE* coop_mode = (BYTE*)(game_globals + 0x2a4);
	BYTE* engine_mode = (BYTE*)(game_globals + 8);

	if (bcoop == true)
	{
		*(coop_mode) = 1; // Turn coop mode on before spawning the player, maybe this fixes their weapon and biped or something idk?
						  // Going to have to reverse the engine simulation function for weapon creation further.
	}
	
	int ret =  pspawn_player(a1); // This handles player spawning for both multiplayer and sinlgeplayer/coop careful with it.

	/* More hacky coop fixes*/
	if (bcoop == true)
	{
		*(coop_mode) = 0; // Turn it back off, sometimes it causes crashes if it's self on we only need it when we're spawning players.
	}

	int PlayerIndex = a1 & 0x000FFFF;
	gg->SpawnPlayer(PlayerIndex);
	
	//TRACE("unit_object: %08X", unit_object);
	//if (unit_object)
	//	*(short*)((short*)unit_object + 0x138) |= 0x10000;
	return ret;
}

void H2MOD::ApplyHooks()
{

	if (this->Server == false)
	{
		/* These hooks are only built for the client, don't enable them on the server! */
		DWORD dwBack;


		pspawn_player = (spawn_player)DetourFunc((BYTE*)this->GetBase() + 0x55952, (BYTE*)spawn_player_hook, 6);
		VirtualProtect(pspawn_player, 4, PAGE_EXECUTE_READWRITE, &dwBack);

		pmap_initialize = (map_intialize)DetourFunc((BYTE*)this->GetBase() + 0x5912D, (BYTE*)map_initialize_hook, 10);
		VirtualProtect(pmap_initialize, 4, PAGE_EXECUTE_READWRITE, &dwBack);

		
		pupdate_player_score = (update_player_score)DetourClassFunc((BYTE*)this->GetBase() + 0xD03ED, (BYTE*)update_player_score_hook,12);
		VirtualProtect(pupdate_player_score, 4, PAGE_EXECUTE_READWRITE, &dwBack);

		pplayer_death = (player_death)DetourFunc((BYTE*)this->GetBase() + 0x17B674, (BYTE*)player_death_hook, 9);
		VirtualProtect(pplayer_death, 4, PAGE_EXECUTE_READWRITE, &dwBack);

	}

}

void H2MOD::Initialize()
{
	if (GetModuleHandleW(L"H2Server.exe"))
	{
		this->Base = (DWORD)GetModuleHandleW(L"H2Server.exe");
		this->Server = TRUE;
	}
	else
	{
		this->Base = (DWORD)GetModuleHandleW(L"halo2.exe");
		this->Server = FALSE;
	}

	TRACE_GAME("H2MOD - Initialized v0.1a");
	TRACE_GAME("H2MOD - BASE ADDR %08X", this->Base);
	TRACE_GAME("H2MOD - Initializing H2MOD Network handlers");

	Network::Initialize();
	gg->Initialize();
	h2mod->ApplyHooks();
	
}

DWORD H2MOD::GetBase()
{
	return this->Base;
}