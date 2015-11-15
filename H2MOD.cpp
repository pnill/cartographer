#include <windows.h>
#include <iostream>
#include <sstream>
#include "H2MOD.h"
#include "Network.h"
#include "xliveless.h"

H2MOD *h2mod = new H2MOD();

//int(__cdecl *kill_player)(int) = (int(__cdecl*)(int))GetModuleHandle(L"halo2.exe")+0x13B514;
int(__cdecl* set_gravity)(float);
int(__cdecl* engine_unit_kill)(int unit);
int(__cdecl* engine_set_camera_fov)(float fov, short index);
int(__cdecl* engine_set_active_camo)(int unit, bool state, float time);
//int(__cdecl* engine_object_placement_data_new)(char*, int, signed int, int);
signed int(__cdecl* engine_object_new)(int);
bool(__cdecl* engine_assign_equipment_to_unit)(int, int, short);

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
	engine_set_active_camo = (int(__cdecl*)(int,bool,float))(this->GetBase() + 0x184555);

	return engine_set_active_camo(unit,true,90000.0f);
}

int H2MOD::unit_kill(int unit)
{
	engine_unit_kill = (int(__cdecl*)(int))(this->GetBase() + 0x13B514);
	

	return engine_unit_kill(unit);
}

int H2MOD::camera_set_fov(float fov, short index)
{
	engine_set_camera_fov = (int(__cdecl *)(float,short))(this->GetBase() + 0x97CC9);

	return engine_set_camera_fov(fov, index);
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
}

DWORD H2MOD::GetBase()
{
	return this->Base;
}