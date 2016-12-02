#include <stdafx.h>
#include <windows.h>
#include <iostream>
#include <sstream>
#include "H2MOD.h"
#include "H2MOD_GunGame.h"
#include "H2MOD_Infection.h"
#include "Network.h"
#include "xliveless.h"
#include "CUser.h"
#include <h2mod.pb.h>
#include <Mmsystem.h>
#include <thread>

H2MOD *h2mod = new H2MOD();
GunGame *gg = new GunGame();
Infection *inf = new Infection();

bool b_Infection = false;

extern bool b_GunGame;
extern CUserManagement User;
extern ULONG g_lLANIP;
extern ULONG g_lWANIP;
extern UINT g_port;
extern bool isHost;
extern HANDLE H2MOD_Network;
extern bool NetworkActive;
extern bool Connected;
extern bool ThreadCreated;
extern ULONG broadcast_server;


SOCKET comm_socket = INVALID_SOCKET;
char* NetworkData = new char[255];

HMODULE base;

extern int MasterState;


#pragma region engine calls

int __cdecl call_get_object(signed int object_datum_index, int object_type)
{
	//TRACE_GAME("call_get_object( object_datum_index: %08X, object_type: %08X )", object_datum_index, object_type);

	typedef int(__cdecl *get_object)(signed int object_datum_index, int object_type);
	get_object pget_object = (get_object)((char*)h2mod->GetBase() + 0x1304E3);

	return pget_object(object_datum_index, object_type);
}

int __cdecl call_unit_reset_equipment(int unit_datum_index)
{
	//TRACE_GAME("unit_reset_equipment(unit_datum_index: %08X)", unit_datum_index);

	typedef int(__cdecl *unit_reset_equipment)(int unit_datum_index);
	unit_reset_equipment punit_reset_equipment = (unit_reset_equipment)(((char*)h2mod->GetBase()) + 0x1441E0);

	if (unit_datum_index != -1 && unit_datum_index != 0)
	{
		return punit_reset_equipment(unit_datum_index);
	}

	return 0;
}

int __cdecl call_hs_object_destroy(int object_datum_index)
{
	//TRACE_GAME("hs_object_destory(object_datum_index: %08X)", object_datum_index);
	typedef int(__cdecl *hs_object_destroy)(int object_datum_index);
	hs_object_destroy phs_object_destroy = (hs_object_destroy)(((char*)h2mod->GetBase()) + 0x136005);

	return phs_object_destroy(object_datum_index);
}

signed int __cdecl call_unit_inventory_next_weapon(unsigned short unit)
{
	//TRACE_GAME("unit_inventory_next_weapon(unit: %08X)", unit);
	typedef signed int(__cdecl *unit_inventory_next_weapon)(unsigned short unit);
	unit_inventory_next_weapon punit_inventory_next_weapon = (unit_inventory_next_weapon)(((char*)h2mod->GetBase()) + 0x139E04);

	return punit_inventory_next_weapon(unit);
}

bool __cdecl call_assign_equipment_to_unit(int unit, int object_index, short unk)
{
	//TRACE_GAME("assign_equipment_to_unit(unit: %08X,object_index: %08X,unk: %08X)", unit, object_index, unk);
	typedef bool(__cdecl *assign_equipment_to_unit)(int unit, int object_index, short unk);
	assign_equipment_to_unit passign_equipment_to_unit = (assign_equipment_to_unit)(((char*)h2mod->GetBase()) + 0x1442AA);


	return passign_equipment_to_unit(unit, object_index, unk);
}

int __cdecl call_object_placement_data_new(void* s_object_placement_data, int object_definition_index, int object_owner, int unk)
{

	//TRACE_GAME("object_placement_data_new(s_object_placement_data: %08X,",s_object_placement_data);
	//TRACE_GAME("object_definition_index: %08X, object_owner: %08X, unk: %08X)", object_definition_index, object_owner, unk);

	typedef int(__cdecl *object_placement_data_new)(void*, int, int, int);
	object_placement_data_new pobject_placement_data_new = (object_placement_data_new)(((char*)h2mod->GetBase()) + 0x132163);


	return pobject_placement_data_new(s_object_placement_data, object_definition_index, object_owner, unk);
}

signed int __cdecl call_object_new(void* pObject)
{
	//TRACE_GAME("object_new(pObject: %08X)", pObject);
	typedef int(__cdecl *object_new)(void*);
	object_new pobject_new = (object_new)(((char*)h2mod->GetBase()) + 0x136CA7);
	
	return pobject_new(pObject);
}

#pragma endregion

void GivePlayerWeapon(int PlayerIndex, int WeaponId,bool bReset)
{
	//TRACE_GAME("GivePlayerWeapon(PlayerIndex: %08X, WeaponId: %08X)", PlayerIndex, WeaponId);

	int unit_datum = h2mod->get_unit_datum_from_player_index(PlayerIndex);

	if (unit_datum != -1 && unit_datum != 0)
	{
		char* nObject = new char[0xC4];
		DWORD dwBack;
		VirtualProtect(nObject, 0xC4, PAGE_EXECUTE_READWRITE, &dwBack);

		call_object_placement_data_new(nObject, WeaponId, unit_datum, 0);

		int object_index = call_object_new(nObject);
		
		if(bReset == true)
			call_unit_reset_equipment(unit_datum);

		call_assign_equipment_to_unit(unit_datum, object_index, 1);
	}

}

wchar_t* H2MOD::get_local_player_name()
{
	return (wchar_t*)(((char*)h2mod->GetBase()) + 0x51A638);
}

int H2MOD::get_player_index_from_name(wchar_t* playername)
{
	DWORD player_table_ptr = *(DWORD*)(this->GetBase() + 0x004A8260);
	player_table_ptr += 0x44;

	for (int i = 0; i<=16; i++)
	{
		wchar_t* comparename = (wchar_t*)(*(DWORD*)player_table_ptr + (i * 0x204) + 0x40);
		
		TRACE_GAME("[H2MOD]::get_player_index_from_name( %ws : %ws )", playername, comparename);

		if (wcscmp(comparename, playername))
		{
			return i;
		}
	}
}

wchar_t* H2MOD::get_player_name_from_index(int pIndex)
{
	DWORD player_table_ptr = *(DWORD*)(this->GetBase() + 0x004A8260);
	player_table_ptr += 0x44;

	return	(wchar_t*)(*(DWORD*)player_table_ptr + (pIndex * 0x204) + 0x40);
}

int H2MOD::get_player_index_from_unit_datum(int unit_datum_index)
{
	int pIndex = 0;
	DWORD player_table_ptr = *(DWORD*)(this->GetBase() + 0x004A8260);
	player_table_ptr += 0x44;
	
	for (pIndex = 0; pIndex <= 16; pIndex++)
	{
		if (unit_datum_index == (int)*(int*)(*(DWORD*)player_table_ptr + (pIndex * 0x204) + 0x28))
		{
			return pIndex;
		}
	}

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

BYTE H2MOD::get_unit_team_index(int unit_datum_index)
{
	BYTE tIndex = 0;
	int unit_object = call_get_object(unit_datum_index, 3);
	if (unit_object)
	{
		tIndex = *(BYTE*)((BYTE*)unit_object + 0x13C);
	}
	return tIndex;
}

void H2MOD::set_unit_team_index(int unit_datum_index, BYTE team)
{
	int unit_object = call_get_object(unit_datum_index, 3);
	if (unit_object)
	{	
		*(BYTE*)((BYTE*)unit_object + 0x13C) = team;
	}
}

void H2MOD::set_unit_biped(BYTE biped,int pIndex)
{
	if (pIndex < 17)
		*(BYTE*)(((char*)0x30002BA0 + (pIndex * 0x204))) = biped;
}

void H2MOD::set_local_team_index(BYTE team)
{
	*(BYTE*)(((char*)h2mod->GetBase()) + 0x51A6B4) = team;
}

void H2MOD::set_local_grenades(BYTE type, BYTE count,int pIndex)
{
	int unit_datum_index = h2mod->get_unit_datum_from_player_index(pIndex);
	
	int unit_object = call_get_object(unit_datum_index, 3);

	if (unit_object)
	{
		if(type == GrenadeType::Frag)
			*(BYTE*)((BYTE*)unit_object + 0x252) = count;
		if(type == GrenadeType::Plasma)
			*(BYTE*)((BYTE*)unit_object + 0x253) = count;
	}

}

void H2MOD::set_unit_grenades(BYTE type, BYTE count,int pIndex, bool bReset)
{
	int unit_datum_index = h2mod->get_unit_datum_from_player_index(pIndex);
	wchar_t* pName = h2mod->get_player_name_from_index(pIndex);

	int unit_object = call_get_object(unit_datum_index, 3);
	if (unit_object)
	{
		if (bReset)
			call_unit_reset_equipment(unit_datum_index);

		if (type == GrenadeType::Frag)
		{
			*(BYTE*)((BYTE*)unit_object + 0x252) = count;
			

		}
		if (type == GrenadeType::Plasma)
		{
			*(BYTE*)((BYTE*)unit_object + 0x253) = count;
		}

		H2ModPacket GrenadePak;
		GrenadePak.set_type(H2ModPacket_Type_set_unit_grenades);
		h2mod_set_grenade *gnade = GrenadePak.mutable_set_grenade();
		gnade->set_count(count);
		gnade->set_pindex(pIndex);
		gnade->set_type(type);

		char* GrenadeBuf = new char[GrenadePak.ByteSize()];
		GrenadePak.SerializeToArray(GrenadeBuf, GrenadePak.ByteSize());
		
		for (auto it = NetworkPlayers.begin(); it != NetworkPlayers.end(); ++it)
		{
			if (wcscmp(it->first->PlayerName, pName) == 0)
			{
				it->first->PacketData = GrenadeBuf;
				it->first->PacketSize = GrenadePak.ByteSize();
				it->first->PacketsAvailable = true;
			}
		}


	}
}

BYTE H2MOD::get_local_team_index()
{
	return *(BYTE*)(((char*)h2mod->GetBase() + 0x51A6B4));
}


void H2MOD::DisableSound(int sound)
{
	DWORD AddressOffset = *(DWORD*)((char*)this->GetBase() + 0x47CD54);

	TRACE_GAME("AddressOffset: %08X", AddressOffset);
	switch (sound)
	{
		case SoundType::Slayer:
			TRACE_GAME("AddressOffset+0xd7dfb4:", (DWORD*)(AddressOffset + 0xd7dfb4));
			*(DWORD*)(AddressOffset + 0xd7e05c) = -1;
			*(DWORD*)(AddressOffset + 0xd7dfb4) = -1;
		break;

		case SoundType::GainedTheLead:
			*(DWORD*)(AddressOffset + 0xd7ab34) = -1;
			*(DWORD*)(AddressOffset + 0xd7ac84) = -1;
		break;

		case SoundType::LostTheLead:
			*(DWORD*)(AddressOffset + 0xd7ad2c) = -1;
			*(DWORD*)(AddressOffset + 0xd7add4) = -1;
		break;

		case SoundType::TeamChange:
			*(DWORD*)(AddressOffset + 0xd7b9a4) = -1;
		break;
	}
}

void SoundThread(void)
{

	while (1)
	{	
		if (h2mod->SoundMap.size() > 0)
		{
			auto it = h2mod->SoundMap.begin();
			while (it != h2mod->SoundMap.end())
			{
				TRACE_GAME("[H2MOD-SoundQueue] - attempting to play sound %ws - delaying for %i miliseconds first", it->first, it->second);
				Sleep(it->second);
				PlaySound(it->first, NULL, SND_FILENAME);
				it = h2mod->SoundMap.erase(it);
			}
		}

		Sleep(100);
	}

}


typedef bool(__cdecl *spawn_player)(int a1);
spawn_player pspawn_player;

typedef bool(__cdecl *membership_update_network_decode)(int a1, int a2, int a3);
membership_update_network_decode pmembership_update_network_decode;


typedef int(__cdecl *game_difficulty_get_real_evaluate)(int a1, int a2);
game_difficulty_get_real_evaluate pgame_difficulty_get_real_evaluate;

typedef int(__cdecl *map_intialize)(int a1);
map_intialize pmap_initialize;

typedef char(__cdecl *player_death)(int unit_datum_index, int a2, char a3, char a4);
player_death pplayer_death;


char __cdecl OnPlayerDeath(int unit_datum_index, int a2, char a3, char a4)
{
	
	//TRACE_GAME("OnPlayerDeath(unit_datum_index: %08X, a2: %08X, a3: %08X, a4: %08X)", unit_datum_index,a2,a3,a4);
	//TRACE_GAME("OnPlayerDeath() - Team: %i", h2mod->get_unit_team_index(unit_datum_index));

#pragma region GunGame Handler
	if (b_GunGame && isHost)
		gg->PlayerDied(unit_datum_index);
#pragma endregion

#pragma region Infection Handler
	if(b_Infection)
		inf->PlayerInfected(unit_datum_index);
#pragma endregion

	return pplayer_death(unit_datum_index, a2, a3, a4);
}

typedef void(__stdcall *update_player_score)(void* thisptr, unsigned short a2, int a3, int a4, int a5, char a6);
update_player_score pupdate_player_score;

void __stdcall OnPlayerScore(void* thisptr, unsigned short a2, int a3, int a4, int a5, char a6)
{
	//TRACE_GAME("update_player_score_hook ( thisptr: %08X, a2: %08X, a3: %08X, a4: %08X, a5: %08X, a6: %08X )", thisptr, a2, a3, a4, a5, a6);


#pragma region GunGame Handler
	if (a5 == 7) //player got a kill?
	{
		int PlayerIndex = a2;
		if (b_GunGame && isHost)
			gg->LevelUp(PlayerIndex);
	}

#pragma endregion

	return pupdate_player_score(thisptr, a2, a3, a4, a5, a6);
}

bool first_load = true;
bool bcoop = false;

// This whole hook is called every single time a map loads,
// I've written a PHP script to compile the byte arrays due to the fact comparing unicode is a bitch.
// Basically if we have a single player map we set bcoop = true so that the coop variables are setup.

int __cdecl OnMapLoad(int a1)
{
	
	/*wchar_t mp3[15] = L"new_zombie.mp3";
	h2mod->play_mp3((LPWSTR)&mp3);


	wchar_t mp32[13] = L"infected.mp3";
	h2mod->play_mp3((LPWSTR)&mp32);*/




	b_Infection = false;
	b_GunGame = false;
	
	wchar_t* variant_name = (wchar_t*)(((char*)h2mod->GetBase())+0x97777C);

	TRACE_GAME("[h2mod] OnMapLoad variant name %ws", variant_name);

	if (wcsstr(variant_name, L"zombies") > 0 || wcsstr(variant_name, L"Zombies") > 0 || wcsstr(variant_name, L"Infection") > 0 || wcsstr(variant_name, L"infection") > 0)
	{
		TRACE_GAME("[h2mod] Zombies Turned on!");
		b_Infection = true;
	}

	if (wcsstr(variant_name, L"GunGame") > 0 || wcsstr(variant_name, L"gungame") > 0)
	{
		TRACE_GAME("[h2mod] GunGame Turned on!");
		b_GunGame = true;
	}

#pragma region COOP FIXES
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
		MasterState = 5;

		//Crashfix
		*(int*)(h2mod->GetBase() + 0x464940) = 0;
		*(int*)(h2mod->GetBase() + 0x46494C) = 0;
		*(int*)(h2mod->GetBase() + 0x464958) = 0;
		*(int*)(h2mod->GetBase() + 0x464964) = 0;
	}
	else 
	{
		MasterState = 4;
	}

	if (!memcmp(quarntine_zone, (BYTE*)0x300017E0, 86) && *(engine_mode) == 2 ) // check the map and if we're loading a multiplayer game (We don't want to fuck up normal campaign)
	{
		bcoop = true; // set coop mode to true because we're loading an SP map and we're trying to do so with the multiplayer engine mode set.
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
#pragma endregion

	int ret = pmap_initialize(a1);


	if (MasterState == 4)
	{
		#pragma region Infection
		if(b_Infection)
			inf->Initialize();
		#pragma endregion

		#pragma region GunGame Handler
			if (b_GunGame && isHost)
				gg->Initialize();
		#pragma endregion

		#pragma region Apply Hitfix
			int offset = 0x47CD54;
			if (h2mod->Server)
				offset = 0x4A29BC;

			DWORD AddressOffset = *(DWORD*)((char*)h2mod->GetBase() + offset);

			*(float*)(AddressOffset + 0xA4EC88) = 2400.0f; // battle_rifle_bullet.proj Initial Velocity 
			*(float*)(AddressOffset + 0xA4EC8C) = 2400.0f; //battle_rifle_bullet.proj Final Velocity
			*(float*)(AddressOffset + 0xB7F914) = 5000.0f; //sniper_bullet.proj Initial Velocity
			*(float*)(AddressOffset + 0xB7F918) = 5000.0f; //sniper_bullet.proj Final Velocity
		#pragma endregion
	}


	return ret;

}

bool __cdecl OnPlayerSpawn(int a1)
{
	//TRACE_GAME("OnPlayerSpawn(a1: %08X)", a1);

	int PlayerIndex = a1 & 0x000FFFF;

#pragma region Infection Prespawn Handler
	if(b_Infection)
		inf->PreSpawn(PlayerIndex);
#pragma endregion

#pragma region COOP Fixes
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
#pragma endregion

#pragma region Infection Handler
	if(b_Infection)
		inf->SpawnPlayer(PlayerIndex);
#pragma endregion

#pragma region GunGame Handler
	if (b_GunGame && isHost)
		gg->SpawnPlayer(PlayerIndex);
#pragma endregion


	return ret;
}


/* Really need some hooks here,
	??_7c_simulation_game_engine_player_entity_definition@@6B@ dd offset sub_11D0018
	
	This area will give us tons of info on the game_engine player, including object index and such linked to secure-address, xnaddr, and wether or not they're host.


	; class c_simulation_game_statborg_entity_definition: c_simulation_entity_definition;   (#classinformer)
	We'll want to hook this for infection, it handles updating teams it seems we could possibly call it after updating a team to push it to all people.

	; class c_simulation_damage_section_response_event_definition: c_simulation_event_definition;   (#classinformer)
	Damage handlers... could help for quake sounds and other things like detecting assisnations (We'll probably find this when we track medals in-game as well)

	; class c_simulation_unit_entity_definition: c_simulation_object_entity_definition, c_simulation_entity_definition;   (#classinformer)
	Ofc we want unit handling.

	; class c_simulation_slayer_engine_globals_definition: c_simulation_game_engine_globals_definition, c_simulation_entity_definition;   (#classinformer)
	Should take a look here for extended functions on scoring chances are we're already hooking one of these.

*/

typedef void(__stdcall *tjoin_game)(void* thisptr, int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10, int a11, char a12, int a13, int a14);
tjoin_game pjoin_game;

extern SOCKET game_sock;
extern SOCKET game_sock_1000;
extern CUserManagement User;
XNADDR join_game_xn;

void __stdcall join_game(void* thisptr, int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10, int a11, char a12, int a13, int a14)
{
	Connected = false;
	NetworkActive = false;

	XNADDR* host_xn = (XNADDR*)a6;
	memcpy(&join_game_xn, host_xn, sizeof(XNADDR));

	trace(L"join_game host_xn->ina.s_addr: %08X ",host_xn->ina.s_addr);
	
	sockaddr_in SendStruct;

	if (host_xn->ina.s_addr != g_lWANIP)
		SendStruct.sin_addr.s_addr = host_xn->ina.s_addr;
	else
		SendStruct.sin_addr.s_addr = g_lLANIP; 
	
	short nPort = (ntohs(host_xn->wPortOnline) + 1);

	TRACE("join_game nPort: %i", nPort);

	SendStruct.sin_port = htons(nPort);

	TRACE("join_game SendStruct.sin_port: %i", ntohs(SendStruct.sin_port));
	TRACE("join_game xn_port: %i", ntohs(host_xn->wPortOnline));

	//SendStruct.sin_port = htons(1001); // These kinds of things need to be fixed too cause we would have the port in the XNADDR struct...
	SendStruct.sin_family = AF_INET;

	int securitysend_1001 = sendto(game_sock, (char*)User.SecurityPacket, 8, 0, (SOCKADDR *)&SendStruct, sizeof(SendStruct));

	User.CreateUser(host_xn);

	if (securitysend_1001 == SOCKET_ERROR )
	{
		TRACE("join_game Security Packet - Socket Error True");
		TRACE("join_game Security Packet - WSAGetLastError(): %08X", WSAGetLastError());
	}

	int Data_of_network_Thread = 1;
	H2MOD_Network = CreateThread(NULL, 0, NetworkThread, &Data_of_network_Thread, 0, NULL);

	return pjoin_game(thisptr, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14);
}

typedef int(__cdecl *tconnect_establish_write)(void* a1, int a2, int a3);
tconnect_establish_write pconnect_establish_write;

int __cdecl connect_establish_write(void* a1, int a2, int a3)
{

	TRACE("connect_establish_write(a1: %08X,a2 %08X, a3: %08X)", a1, a2, a3);

	if (!isHost)
	{
		sockaddr_in SendStruct;

		if (join_game_xn.ina.s_addr != g_lWANIP)
			SendStruct.sin_addr.s_addr = join_game_xn.ina.s_addr;
		else
			SendStruct.sin_addr.s_addr = g_lLANIP;

		SendStruct.sin_port = join_game_xn.wPortOnline;
		SendStruct.sin_family = AF_INET;

		int securitysend_1000 = sendto(game_sock_1000, (char*)User.SecurityPacket, 8, 0, (SOCKADDR *)&SendStruct, sizeof(SendStruct));
	}
	

	return pconnect_establish_write(a1, a2, a3);
}


void H2MOD::ApplyHooks()
{

	/* Should store all offsets in a central location and swap the variables based on h2server/halo2.exe*/
	/* We also need added checks to see if someone is the host or not, if they're not they don't need any of this handling. */
	if (this->Server == false)
	{
		TRACE_GAME("Applying client hooks...");
		/* These hooks are only built for the client, don't enable them on the server! */
		DWORD dwBack;

		pjoin_game = (tjoin_game)DetourClassFunc((BYTE*)this->GetBase() + 0x1CDADE, (BYTE*)join_game, 13);
		VirtualProtect(pjoin_game, 4, PAGE_EXECUTE_READWRITE, &dwBack);

		pspawn_player = (spawn_player)DetourFunc((BYTE*)this->GetBase() + 0x55952, (BYTE*)OnPlayerSpawn, 6);
		VirtualProtect(pspawn_player, 4, PAGE_EXECUTE_READWRITE, &dwBack);

		pmap_initialize = (map_intialize)DetourFunc((BYTE*)this->GetBase() + 0x5912D, (BYTE*)OnMapLoad, 10);
		VirtualProtect(pmap_initialize, 4, PAGE_EXECUTE_READWRITE, &dwBack);

		
		pupdate_player_score = (update_player_score)DetourClassFunc((BYTE*)this->GetBase() + 0xD03ED, (BYTE*)OnPlayerScore,12);
		VirtualProtect(pupdate_player_score, 4, PAGE_EXECUTE_READWRITE, &dwBack);

		pplayer_death = (player_death)DetourFunc((BYTE*)this->GetBase() + 0x17B674, (BYTE*)OnPlayerDeath, 9);
		VirtualProtect(pplayer_death, 4, PAGE_EXECUTE_READWRITE, &dwBack);

		pconnect_establish_write = (tconnect_establish_write)DetourFunc((BYTE*)this->GetBase() + 0x1F1A2D, (BYTE*)connect_establish_write, 5);
		VirtualProtect(pconnect_establish_write, 4, PAGE_EXECUTE_READWRITE, &dwBack);

	}
	



}



DWORD WINAPI NetworkThread(LPVOID lParam)
{
	TRACE_GAME("[h2mod-network] NetworkThread Initializing");

	if (comm_socket == INVALID_SOCKET)
	{
		comm_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

		if (comm_socket == INVALID_SOCKET)
		{
			TRACE_GAME("[h2mod-network] Socket is invalid even after socket()");
		}

		SOCKADDR_IN RecvStruct;
		RecvStruct.sin_port = htons(((short)g_port) + 7);
		RecvStruct.sin_addr.s_addr = htonl(INADDR_ANY);

		RecvStruct.sin_family = AF_INET;

		if (bind(comm_socket, (const sockaddr*)&RecvStruct, sizeof RecvStruct) == -1)
		{
			TRACE_GAME("[h2mod-network] Would not bind socket!");
		}

		DWORD dwTime = 20;

		if (setsockopt(comm_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&dwTime, sizeof(dwTime)) < 0)
		{
			TRACE_GAME("[h2mod-network] Socket Error on register request");
		}

		TRACE_GAME("[h2mod-network] Listening on port: %i", ntohs(RecvStruct.sin_port));
	}
	else 
	{
		TRACE_GAME("[h2mod-network] socket already existed continuing without attempting bind...");
	}

	



	
	TRACE_GAME("[h2mod-network] Are we host? %i", isHost);
	
	NetworkActive = true;

	if(isHost)
	{
		TRACE_GAME("[h2mod-network] We're host waiting for a authorization packet from joining clients...");

		while (1)
		{
	
			if (NetworkActive == false)
			{
				isHost = false;
				Connected = false;
				ThreadCreated = false;
				H2MOD_Network = 0;
				TRACE_GAME("[h2mod-network] Killing host thread NetworkActive == false");
				return 0;
			}
			sockaddr_in SenderAddr;
			int SenderAddrSize = sizeof(SenderAddr);

			memset(NetworkData, 0x00, 255);
			int recvresult = recvfrom(comm_socket, NetworkData, 255, 0, (sockaddr*)&SenderAddr, &SenderAddrSize);

			if (h2mod->NetworkPlayers.size() > 0)
			{
				auto it = h2mod->NetworkPlayers.begin();
				while (it != h2mod->NetworkPlayers.end())
				{
					if (it->second == 0)
					{
						TRACE_GAME("[h2mod-network] Deleting player %ws as their value was set to 0", it->first->PlayerName);

						if (it->first->PacketsAvailable == true)
							delete[] it->first->PacketData; // Delete packet data if there is any.


						delete[] it->first; // Clear NetworkPlayer object.

						it = h2mod->NetworkPlayers.erase(it);


					}
					else
					{
						if (it->first->PacketsAvailable == true) // If there's a packet available we set this to true already.
						{
							TRACE_GAME("[h2mod-network] Sending player %ws data", it->first->PlayerName);

							SOCKADDR_IN QueueSock;
							QueueSock.sin_port = it->first->port; // We can grab the port they connected from.
							QueueSock.sin_addr.s_addr = it->first->addr; // Address they connected from.
							QueueSock.sin_family = AF_INET;

							sendto(comm_socket, it->first->PacketData, it->first->PacketSize, 0, (sockaddr*)&QueueSock, sizeof(QueueSock)); // Just send the already serialized data over the socket.

							it->first->PacketsAvailable = false;
							delete[] it->first->PacketData; // Delete packet data we've sent it already.
						}
						it++;
					}
				}
			}
		

			if (recvresult > 0)
			{
				bool already_authed = false;
				H2ModPacket recvpak;
				recvpak.ParseFromArray(NetworkData, recvresult);


				if (recvpak.has_type())
				{
					switch (recvpak.type())
					{
						case H2ModPacket_Type_authorize_client:
							TRACE_GAME("[h2mod-network] Player Connected!");
							if (recvpak.has_h2auth())
							{
								if (recvpak.h2auth().has_name())
								{
									wchar_t* PlayerName = new wchar_t[36];
									memcpy(PlayerName, recvpak.h2auth().name().c_str(), 36);

									for (auto it = h2mod->NetworkPlayers.begin(); it != h2mod->NetworkPlayers.end(); ++it)
									{
										if (wcscmp(it->first->PlayerName, PlayerName) == 0)
										{

											TRACE_GAME("[h2mod-network] This player was already connected, sending them another packet letting them know they're authed already.");

											char* SendBuf = new char[recvpak.ByteSize()];
											recvpak.SerializeToArray(SendBuf, recvpak.ByteSize());

											sendto(comm_socket, SendBuf, recvpak.ByteSize(), 0, (SOCKADDR*)&SenderAddr, sizeof(SenderAddr));


											already_authed = true;

											delete[] SendBuf;
										}
									}

									if (already_authed == false)
									{
										NetworkPlayer *nPlayer = new NetworkPlayer;


										TRACE_GAME("[h2mod-network] PlayerName: %ws", PlayerName);
										TRACE_GAME("[h2mod-network] IP:PORT: %08X:%i", SenderAddr.sin_addr.s_addr, ntohs(SenderAddr.sin_port));

										nPlayer->addr = SenderAddr.sin_addr.s_addr;
										nPlayer->port = SenderAddr.sin_port;
										nPlayer->PlayerName = PlayerName;
										nPlayer->secure = recvpak.h2auth().secureaddr();
										h2mod->NetworkPlayers[nPlayer] = 1;

										char* SendBuf = new char[recvpak.ByteSize()];
										recvpak.SerializeToArray(SendBuf, recvpak.ByteSize());

										sendto(comm_socket, SendBuf, recvpak.ByteSize(), 0, (SOCKADDR*)&SenderAddr, sizeof(SenderAddr));

										delete[] SendBuf;
									}
								}
							}
						break;

						case H2ModPacket_Type_h2mod_ping:
							H2ModPacket pongpak;
							pongpak.set_type(H2ModPacket_Type_h2mod_pong);

							char* pongdata = new char[pongpak.ByteSize()];
							pongpak.SerializeToArray(pongdata, pongpak.ByteSize());
							sendto(comm_socket, pongdata, recvpak.ByteSize(), 0, (SOCKADDR*)&SenderAddr, sizeof(SenderAddr));

							delete[] pongdata;
						break;

					}
				}

				Sleep(500);
			}
		}
	}
	else
	{
		Connected = false;
		TRACE_GAME("[h2mod-network] We're a client connecting to server...");

		
		SOCKADDR_IN SendStruct;
		SendStruct.sin_port = htons(ntohs(join_game_xn.wPortOnline) + 7);
		SendStruct.sin_addr.s_addr = join_game_xn.ina.s_addr;
		SendStruct.sin_family = AF_INET;
		
		TRACE_GAME("[h2mod-network] Connecting to server on %08X:%i", SendStruct.sin_addr.s_addr, ntohs(SendStruct.sin_port));

		H2ModPacket h2pak;
		h2pak.set_type(H2ModPacket_Type_authorize_client);

		h2mod_auth *authpak = h2pak.mutable_h2auth();
		authpak->set_name((char*)h2mod->get_local_player_name(),32);
		authpak->set_secureaddr(User.LocalSec);

		char* SendBuf = new char[h2pak.ByteSize()];
		h2pak.SerializeToArray(SendBuf, h2pak.ByteSize());

		sendto(comm_socket, SendBuf, h2pak.ByteSize(), 0, (SOCKADDR*)&SendStruct, sizeof(SendStruct));

		while (1)
		{
			if (NetworkActive == false)
			{
				isHost = false;
				Connected = false;
				ThreadCreated = false;
				H2MOD_Network = 0;
				TRACE_GAME("[h2mod-network] Networkactive == false ending client thread.");
				return 0;
			}

			if (Connected == false) 
			{
				TRACE_GAME("[h2mod-network] Client - we're not connected re-sending our auth..");
				sendto(comm_socket, SendBuf, h2pak.ByteSize(), 0, (SOCKADDR*)&SendStruct, sizeof(SendStruct));
			}

			sockaddr_in SenderAddr;
			int SenderAddrSize = sizeof(SenderAddr);

			memset(NetworkData, 0x00, 255);
			int recvresult = recvfrom(comm_socket, NetworkData, 255, 0, (sockaddr*)&SenderAddr, &SenderAddrSize);
			
			if (recvresult > 0)
			{
				H2ModPacket recvpak;
				recvpak.ParseFromArray(NetworkData, recvresult);

				if (recvpak.has_type())
				{
					switch (recvpak.type())
					{
						case H2ModPacket_Type_authorize_client:
						
							if (Connected == false)
							{
								TRACE("[h2mod-network] Got the auth packet back!, We're connected!");
								Connected = true;
							}
					
						break;

						case H2ModPacket_Type_set_player_team:
							
							if (recvpak.has_h2_set_player_team())
							{

								BYTE TeamIndex = recvpak.h2_set_player_team().team();

								TRACE_GAME("[h2mod-network] Got a set team request from server! TeamIndex: %i", TeamIndex);
								h2mod->set_local_team_index(TeamIndex);
							}

						break;

						case H2ModPacket_Type_set_unit_grenades:
							if (recvpak.has_set_grenade())
							{
								BYTE type = recvpak.set_grenade().type();
								BYTE count = recvpak.set_grenade().count();
								BYTE pIndex = recvpak.set_grenade().pindex();

								h2mod->set_local_grenades(type, count, pIndex);
							}
						break;
					}
				}

			}

			if (Connected == true)
			{
				H2ModPacket pack;
				pack.set_type(H2ModPacket_Type_h2mod_ping);

				char* SendBuf = new char[pack.ByteSize()];
				pack.SerializeToArray(SendBuf, pack.ByteSize());

				sendto(comm_socket, SendBuf, pack.ByteSize(), 0, (SOCKADDR*)&SendStruct, sizeof(SendStruct));

				delete[] SendBuf;
			}


			Sleep(500);
		}
		
	}

	return 0;
}

DWORD WINAPI Thread1( LPVOID lParam )
{
	char *binarydata = new char[0xAA8 + 1];
	FILE* binarydump = fopen("binarydump.bin", "r");
	fread(binarydata, 0xAA8, 1, binarydump);

	while (1)
	{	

		
		DWORD Base = (DWORD)GetModuleHandleA("halo2.exe");
	
		DWORD *ServerList = (DWORD*)(*(DWORD*)(Base + 0x96743C));
		if (ServerList > 0)
		{
			memcpy(ServerList, binarydata, 0xAA8);
			memcpy(ServerList + 0xAA8, binarydata, 0xAA8);
		}
		
		//fread((ServerList + 0xAA8), 0xAA8, 1, BinaryDump);
		//TRACE("ServerList: %08X\n", ServerList);
		//fwrite(ServerList, 0xAA8, 1, BinaryDump);	
	}
}

void H2MOD::Initialize()
{
	
	if (GetModuleHandleA("H2Server.exe"))
	{
		this->Base = (DWORD)GetModuleHandleA("H2Server.exe");
		this->Server = TRUE;
	}
	else
	{
		this->Base = (DWORD)GetModuleHandleA("halo2.exe");
		this->Server = FALSE;
		//HANDLE Handle_Of_Sound_Thread = 0;
		//int Data_Of_Sound_Thread = 1;
		std::thread SoundT(SoundThread);
		SoundT.detach();
		//Handle_Of_Sound_Thread = CreateThread(NULL, 0, SoundQueue, &Data_Of_Sound_Thread, 0, NULL);
	}

	TRACE_GAME("H2MOD - Initialized v0.1a");
	TRACE_GAME("H2MOD - BASE ADDR %08X", this->Base);

	//TRACE_GAME("H2MOD - Initializing H2MOD Network handlers");

	//Network::Initialize();

	
	
	h2mod->ApplyHooks();
	
}

DWORD H2MOD::GetBase()
{
	return this->Base;
}