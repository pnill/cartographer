#pragma once

#include "Hook.h"
#include <unordered_map>
#include <set>
#include <mutex>

#define SINGLE_PLAYER_ENGINE 1
#define MULTIPLAYER_ENGINE 2
#define MAIN_MENU_ENGINE 3

enum GrenadeType
{
	Frag = 0,
	Plasma = 1
};

enum SoundType
{
	TeamChange = 1,
	GainedTheLead = 2,
	LostTheLead = 3,
	Slayer = 4
};

enum BipedType
{
	MasterChief = 0,
	Arbiter = 1,
	Spartan = 2,
	Elite = 3
};

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

enum static_lod : DWORD
{
	disable = 0,
	super_low,
	low,
	medium,
	high,
	super_high,
	cinematic
};

int __cdecl call_get_object(signed int object_datum_index, int object_type);
int __cdecl call_unit_reset_equipment(int unit_datum_index);
bool __cdecl call_add_object_to_sync(int gamestate_object_datum);
int __cdecl call_hs_object_destroy(int object_datum_index);
signed int __cdecl call_unit_inventory_next_weapon(unsigned short unit_datum_index);
bool __cdecl call_assign_equipment_to_unit(int uint, int object_index, short unk);
int __cdecl call_object_placement_data_new(void*, int, int, int);
signed int __cdecl call_object_new(void*);
void GivePlayerWeapon(int PlayerIndex, int WeaponId, bool bReset);

class H2MOD
{
public:
		void Initialize();
		void Deinitialize();
		void IndicatorVisibility(bool toggle);
		int get_unit_from_player_index(int);
		int get_unit_datum_from_player_index(int);
		void ApplyHooks();
		DWORD GetBase(); 
		void handle_command(std::string);
		void handle_command(std::wstring);
		void logToDedicatedServerConsole(wchar_t* message);
		wchar_t* get_local_player_name();		
		int get_dynamic_player_base(int playerIndex, bool resetDynamicBase);
		float get_player_x(int, bool);
		float get_player_y(int, bool);
		float get_player_z(int, bool);
		wchar_t* get_player_name_from_index(int pIndex);
		int get_player_index_from_name(wchar_t* playername);
		int get_player_index_from_unit_datum(int unit_datum_index);
		BYTE get_unit_team_index(int unit_datum_index);
		void set_unit_team_index(int unit_datum_index, BYTE team);
		void set_unit_biped(BYTE biped, int pIndex);
		void set_unit_speed_patch(bool hackit);
		void kick_player(int peerIndex);
		void set_unit_speed(float speed, int pIndex);
		void set_local_team_index(BYTE team);
		BYTE get_local_team_index();
		void set_local_grenades(BYTE type, BYTE count, int pIndex);
		void DisableSound(int sound);
		void PatchWeaponsInteraction(bool b_Enable);		
		void securityPacketProcessing();
		BOOL Server;
		std::unordered_map<wchar_t*, int> SoundMap;
		std::unordered_map<std::string, bool> AchievementMap;
	

		std::mutex sound_mutex;
		std::condition_variable sound_cv;

		std::set<int> hookedObjectDefs;
		bool isChatBoxCommand = false;

private:
		DWORD Base;
		std::unordered_map<int, int> playerIndexToDynamicBase;

};


extern H2MOD* h2mod;

