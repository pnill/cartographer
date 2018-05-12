#include <stdafx.h>
#include <windows.h>
#include <Wincrypt.h>
#include <iostream>
#include <sstream>
#include <codecvt>
#include "H2MOD.h"
#include "H2MOD_Mouseinput.h"
#include "H2MOD_H2X.h"
#include "H2MOD_GunGame.h"
#include "H2MOD_Infection.h"
#include "H2MOD_Halo2Final.h"
#include "Network.h"
#include "xliveless.h"
#include "CUser.h"
#include <Mmsystem.h>
#include <thread>
#include "Globals.h"
#include "H2OnscreenDebugLog.h"
#include "GSUtils.h"
#include <Mmsystem.h>
#include "discord/DiscordInterface.h"
#include "H2Config.h"
#include "H2Tweaks.h"
#include "Blam\Engine\FileSystem\FiloInterface.h"

H2MOD *h2mod = new H2MOD();
GunGame* gunGame = new GunGame();
Infection* infectionHandler = new Infection();
Halo2Final *h2f = new Halo2Final();
Mouseinput *mouse = new Mouseinput();

bool b_Infection = false;
bool b_Halo2Final = false;
bool b_H2X = false;

extern bool b_GunGame;
extern CUserManagement User;
extern HANDLE H2MOD_Network;
extern bool NetworkActive;
extern bool Connected;
extern bool ThreadCreated;
extern int H2GetInstanceId();

SOCKET comm_socket = INVALID_SOCKET;
char* NetworkData = new char[255];

HMODULE base;


#pragma region engine calls

int __cdecl call_get_object(signed int object_datum_index, int object_type)
{
	//TRACE_GAME("call_get_object( object_datum_index: %08X, object_type: %08X )", object_datum_index, object_type);

	typedef int(__cdecl *get_object)(signed int object_datum_index, int object_type);
	get_object pget_object = (get_object)((char*)h2mod->GetBase() + ((h2mod->Server) ? 0x11F3A6 : 0x1304E3));

	return pget_object(object_datum_index, object_type);
}

int __cdecl call_unit_reset_equipment(int unit_datum_index)
{
	//TRACE_GAME("unit_reset_equipment(unit_datum_index: %08X)", unit_datum_index);
	typedef int(__cdecl *unit_reset_equipment)(int unit_datum_index);
	unit_reset_equipment punit_reset_equipment = (unit_reset_equipment)(((char*)h2mod->GetBase()) + ((h2mod->Server) ? 0x133030 : 0x1441E0));
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
	hs_object_destroy phs_object_destroy = (hs_object_destroy)(((char*)h2mod->GetBase()) + ((h2mod->Server) ? 0x124ED5 : 0x136005));

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
	assign_equipment_to_unit passign_equipment_to_unit = (assign_equipment_to_unit)(((char*)h2mod->GetBase()) + ((h2mod->Server) ? 0x1330FA : 0x1442AA));

	return passign_equipment_to_unit(unit, object_index, unk);
}

int __cdecl call_object_placement_data_new(void* s_object_placement_data, int object_definition_index, int object_owner, int unk)
{

	//TRACE_GAME("object_placement_data_new(s_object_placement_data: %08X,",s_object_placement_data);
	//TRACE_GAME("object_definition_index: %08X, object_owner: %08X, unk: %08X)", object_definition_index, object_owner, unk);

	typedef int(__cdecl *object_placement_data_new)(void*, int, int, int);
	object_placement_data_new pobject_placement_data_new = (object_placement_data_new)(((char*)h2mod->GetBase()) + ((h2mod->Server) ? 0x121033 : 0x132163));

	return pobject_placement_data_new(s_object_placement_data, object_definition_index, object_owner, unk);


}

signed int __cdecl call_object_new(void* pObject)
{
	//TRACE_GAME("object_new(pObject: %08X)", pObject);

	typedef int(__cdecl *object_new)(void*);
	object_new pobject_new = (object_new)(((char*)h2mod->GetBase()) + ((h2mod->Server) ? 0x125B77 : 0x136CA7));

	return pobject_new(pObject);

}

bool __cdecl call_add_object_to_sync(int gamestate_object_datum)
{
	typedef int(__cdecl  *add_object_to_sync)(int gamestate_object_datum);
	add_object_to_sync p_add_object_to_sync = (add_object_to_sync)((char*)h2mod->GetBase() + ((h2mod->Server) ? 0x1B2C44 : 0x1B8D14));

	return p_add_object_to_sync(gamestate_object_datum);
}

#pragma endregion

//sub_1cce9b
typedef int(__stdcall *calls_session_boot)(void*, int, char);
calls_session_boot calls_session_boot_method;

int __stdcall calls_session_boot_sub_1cce9b(void* thisx, int a2, char a3) {
	TRACE_GAME_N("session boot - this=%d,a2=%d,a3=%d", thisx, a2, a3);
	return calls_session_boot_method(thisx, a2, a3);
}

void H2MOD::kick_player(int peerIndex) {
	DWORD* ptr = (DWORD*)(((char*)h2mod->GetBase()) + 0x420FE8);
	TRACE_GAME_N("about to kick player index=%d", peerIndex);
	calls_session_boot_method((DWORD*)(*ptr), peerIndex, (char)0x01);
}

//0x1BA418
typedef bool(*live_check)();
live_check live_check_method;

bool clientXboxLiveCheck() {
	//lets you access live menu
	return true;
}

//0x1B1643
typedef signed int(*live_check2)();
live_check2 live_check_method2;

signed int clientXboxLiveCheck2() {
	//1 = turns off live? 
	//2 = either not live or can't download maps
	return 2;
}

typedef int(__cdecl *show_error_screen)(int a1, signed int a2, int a3, __int16 a4, int a5, int a6);
show_error_screen show_error_screen_method;

int __cdecl showErrorScreen(int a1, signed int a2, int a3, __int16 a4, int a5, int a6) {
	if (a2 == 280) {
		//280 is special here, the constant is used when a custom map cannot be loaded for clients
		return 0;
	}
	return show_error_screen_method(a1, a2, a3, a4, a5, a6);
}

typedef signed int(__cdecl *string_display_hook)(int a3, unsigned int a4, int a5, int a6);
string_display_hook string_display_hook_method;

std::wstring YOU_FAILED_TO_LOAD_MAP_ORG = L"You failed to load the map.";

//lets you follow the call path of any string that is displayed (in a debugger)
signed int __cdecl stringDisplayHook(int a3, unsigned int a4, int a5, int a6) {
	/*
	if (overrideUnicodeMessage) {
	wchar_t* temp = (wchar_t*)a5;
	if (temp[0] != L' ') {
	//const wchar_t* lobbyMessage = mapManager->getCustomLobbyMessage();
	if (lobbyMessage != NULL && wcscmp(temp, yftlm) == 0) {
	//if we detect that we failed to load the map, we display different strings only for the duration of
	//this specific string being displayed
	return string_display_hook_method(a3, a4, (int)(lobbyMessage), a6);
	}

	/*
	if (wcscmp(OCTAGON.c_str(), temp) == 0) {
	__debugbreak();
	}
	if (temp != NULL && temp[0] == L'T' && temp[1] == L'i' && temp[2] == L'e' && temp[3] == L'd') {
	__debugbreak();
	}*/
	//}
	//}*/
	return string_display_hook_method(a3, a4, a5, a6);
}

void H2MOD::handle_command(std::string command) {
	commands->handle_command(command);
}

void H2MOD::handle_command(std::wstring command) {
	commands->handle_command(std::string(command.begin(), command.end()));
}

typedef int(__cdecl *dedi_command_hook)(int a1, int a2, char a3);
dedi_command_hook dedi_command_hook_method;

typedef signed int(*dedi_print)(const char* a1, ...);

void H2MOD::logToDedicatedServerConsole(wchar_t* message) {
	dedi_print dedi_print_method = (dedi_print)(h2mod->GetBase() + 0x2354C8);
	dedi_print_method((const char*)(message));
}

int __cdecl dediCommandHook(int a1, int a2, int a3) {
	h2mod->logToDedicatedServerConsole(L"Dedicated command\n");
	unsigned __int16* ptr = *(unsigned __int16 **)a1;
	const wchar_t* text = (wchar_t*)ptr;
	wchar_t c = text[0];
	if (c == L'$') {
		h2mod->logToDedicatedServerConsole(L"Running chatbox command\n");
		//run the chatbox commands
		h2mod->handle_command(std::wstring(text));
	}

	return dedi_command_hook_method(a1, a2, a3);
}

#pragma region PlayerFunctions

float H2MOD::get_player_x(int playerIndex, bool resetDynamicBase) {
	int base = get_dynamic_player_base(playerIndex, resetDynamicBase);
	if (base != -1) {
		float buffer;
		ReadProcessMemory(GetCurrentProcess(), (LPVOID)(base + 0x64), &buffer, sizeof(buffer), NULL);
		return buffer;
	}
	return 0.0f;
}

float H2MOD::get_player_y(int playerIndex, bool resetDynamicBase) {
	int base = get_dynamic_player_base(playerIndex, resetDynamicBase);
	if (base != -1) {
		float buffer;
		ReadProcessMemory(GetCurrentProcess(), (LPVOID)(base + 0x68), &buffer, sizeof(buffer), NULL);
		return buffer;
	}
	return 0.0f;
}

float H2MOD::get_player_z(int playerIndex, bool resetDynamicBase) {
	int base = get_dynamic_player_base(playerIndex, resetDynamicBase);
	if (base != -1) {
		float buffer;
		ReadProcessMemory(GetCurrentProcess(), (LPVOID)(base + 0x6C), &buffer, sizeof(buffer), NULL);
		return buffer;
	}
	return 0.0f;
}

int H2MOD::get_dynamic_player_base(int playerIndex, bool resetDynamicBase) {
	/*int tempSight = get_unit_datum_from_player_index(playerIndex);
	int cachedDynamicBase = playerIndexToDynamicBase[playerIndex];
	if (tempSight != -1 && tempSight != 0 && (!cachedDynamicBase || resetDynamicBase)) {
	//TODo: this is based on some weird implementation in HaloObjects.cs, need just to find real offsets to dynamic player pointer
	//instead of this garbage
	for (int i = 0; i < 2048; i++) {
	int possiblyDynamicBase = *(DWORD*)(0x3003CF3C + (i * 12) + 8);
	DWORD* possiblyDynamicBasePtr = (DWORD*)(possiblyDynamicBase + 0x3F8);
	if (possiblyDynamicBasePtr) {
	BYTE buffer[4];
	//use readprocess memory since it will set the page memory and read/write
	ReadProcessMemory(GetCurrentProcess(), (LPVOID)(possiblyDynamicBase + 0x3F8), &buffer, sizeof(buffer), NULL);
	//little endian
	int dynamicS2 = buffer[0] + (buffer[1] << 8) + (buffer[2] << 16) + (buffer[3] << 24);
	if (tempSight == dynamicS2) {
	playerIndexToDynamicBase[playerIndex] = possiblyDynamicBase;
	return possiblyDynamicBase;
	}
	}
	}
	}
	return cachedDynamicBase;*/
	int dyanamic = -1;
	int playerdatum = h2mod->get_unit_datum_from_player_index(playerIndex);
	int DynamicObjBase = *(DWORD *)(((char*)h2mod->GetBase()) + ((h2mod->Server) ? 0x50C8EC : 0x4E461C));
	if (playerdatum != -1)
		dyanamic = *(DWORD *)(*(DWORD *)(DynamicObjBase + 68) + 12 * (unsigned __int16)playerdatum + 8);
	return dyanamic;
}


void GivePlayerWeapon(int PlayerIndex, int WeaponId, bool bReset)
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

		if (bReset == true)
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
	DWORD player_table_ptr;
	if (!h2mod->Server)
		player_table_ptr = *(DWORD*)(this->GetBase() + 0x004A8260);
	else
		player_table_ptr = *(DWORD*)(this->GetBase() + 0x004D64C4);
	player_table_ptr += 0x44;

	for (int i = 0; i <= 16; i++)
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
	DWORD player_table_ptr;
	if (!h2mod->Server)
		player_table_ptr = *(DWORD*)(this->GetBase() + 0x004A8260);
	else
		player_table_ptr = *(DWORD*)(this->GetBase() + 0x004D64C4);
	player_table_ptr += 0x44;

	return	(wchar_t*)(*(DWORD*)player_table_ptr + (pIndex * 0x204) + 0x40);
}

int H2MOD::get_player_index_from_unit_datum(int unit_datum_index)
{
	int pIndex = 0;
	DWORD player_table_ptr;
	if (!h2mod->Server)
		player_table_ptr = *(DWORD*)(this->GetBase() + 0x004A8260);
	else
		player_table_ptr = *(DWORD*)(this->GetBase() + 0x004D64C4);
	player_table_ptr += 0x44;

	for (pIndex = 0; pIndex <= 16; pIndex++)
	{
		if (unit_datum_index == (int)*(int*)(*(DWORD*)player_table_ptr + (pIndex * 0x204) + 0x28))
		{
			return pIndex;
		}
	}
	return -1;
}


int H2MOD::get_unit_datum_from_player_index(int pIndex)
{
	int unit = 0;
	DWORD player_table_ptr;
	if (!h2mod->Server)
		player_table_ptr = *(DWORD*)(this->GetBase() + 0x004A8260);
	else
		player_table_ptr = *(DWORD*)(this->GetBase() + 0x004D64C4);
	player_table_ptr += 0x44;

	unit = (int)*(int*)(*(DWORD*)player_table_ptr + (pIndex * 0x204) + 0x28);

	return unit;
}

int H2MOD::get_unit_from_player_index(int pIndex)
{
	int unit = 0;
	DWORD player_table_ptr;
	if (!h2mod->Server)
		player_table_ptr = *(DWORD*)(this->GetBase() + 0x004A8260);
	else
		player_table_ptr = *(DWORD*)(this->GetBase() + 0x004D64C4);
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

//can be used on clients and server
void H2MOD::set_unit_biped(BYTE biped, int pIndex)
{
	if (pIndex >= 0 && pIndex < 16)
		*(BYTE*)(((char*)((h2mod->Server) ? 0x3000274C : 0x30002BA0) + (pIndex * 0x204))) = biped;
}

void H2MOD::set_unit_speed_patch(bool hackit) {
	//TODO: create a way to undo the patch in the case when more than just infection relies on this.
	//Enable Speed Hacks

	BYTE assmPatchSpeed[8];
	memset(assmPatchSpeed, 0x90, 8);
	WriteBytes(h2mod->GetBase() + ((!h2mod->Server) ? 0x6AB7f : 0x6A3BA), assmPatchSpeed, 8);
}

void H2MOD::set_unit_speed(float speed, int pIndex)
{
	if (pIndex >= 0 && pIndex < 16)
		*(float*)(((char*)((h2mod->Server) ? 0x30002848 : 0x30002C9C) + (pIndex * 0x204))) = speed;
}

void H2MOD::set_local_team_index(BYTE team)
{
	*(BYTE*)(((char*)h2mod->GetBase()) + 0x51A6B4) = team;
}

void H2MOD::set_local_grenades(BYTE type, BYTE count, int pIndex)
{
	int unit_datum_index = h2mod->get_unit_datum_from_player_index(pIndex);

	int unit_object = call_get_object(unit_datum_index, 3);

	if (unit_object)
	{
		if (type == GrenadeType::Frag)
			*(BYTE*)((BYTE*)unit_object + 0x252) = count;
		if (type == GrenadeType::Plasma)
			*(BYTE*)((BYTE*)unit_object + 0x253) = count;
	}

}

BYTE H2MOD::get_local_team_index()
{
	return *(BYTE*)(((char*)h2mod->GetBase() + 0x51A6B4));
}
#pragma endregion

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
		std::unique_lock<std::mutex> lck(h2mod->sound_mutex);
		h2mod->sound_cv.wait(lck);

		if (h2mod->SoundMap.size() > 0)
		{
			std::unordered_map<wchar_t*, int> tempSoundMap;
			tempSoundMap.insert(h2mod->SoundMap.begin(), h2mod->SoundMap.end());
			h2mod->SoundMap.clear();
			//unlock immediately after reading everything from sound map
			lck.unlock();

			auto it = tempSoundMap.begin();
			while (it != tempSoundMap.end())
			{
				TRACE_GAME("[H2MOD-SoundQueue] - attempting to play sound %ws - delaying for %i miliseconds first", it->first, it->second);
				Sleep(it->second);
				PlaySound(it->first, NULL, SND_FILENAME);
				it = tempSoundMap.erase(it);
			}
		}
	}

}

typedef char(__cdecl *tsub_4F17A)(void*, int, int);
tsub_4F17A psub_4F17A;

char __cdecl sub_4F17A(void* thisptr, int a2, int a3) //allows people to load custom maps with custom tags/huds 
{
	int result = psub_4F17A(thisptr, a2, a3);
	return 1;
}

typedef bool(__cdecl *spawn_player)(int a1);
spawn_player pspawn_player;

typedef bool(__cdecl *membership_update_network_decode)(int a1, int a2, int a3);
membership_update_network_decode pmembership_update_network_decode;

typedef int(__cdecl *game_difficulty_get_real_evaluate)(int a1, int a2);
game_difficulty_get_real_evaluate pgame_difficulty_get_real_evaluate;

typedef char(__cdecl *player_death)(int unit_datum_index, int a2, char a3, char a4);
player_death pplayer_death;

typedef void(__stdcall *update_player_score)(void* thisptr, unsigned short a2, int a3, int a4, int a5, char a6);
update_player_score pupdate_player_score;

typedef void(__stdcall *tjoin_game)(void* thisptr, int a2, int a3, int a4, int a5, XNADDR* host_xn, int a7, int a8, int a9, int a10, int a11, char a12, int a13, int a14);
tjoin_game pjoin_game;

extern SOCKET game_sock;
extern SOCKET game_sock_1000;
extern CUserManagement User;
XNADDR join_game_xn;

typedef int(__cdecl *tconnect_establish_write)(void* a1, int a2, int a3);
tconnect_establish_write pconnect_establish_write;


char __cdecl OnPlayerDeath(int unit_datum_index, int a2, char a3, char a4)
{

	//TRACE_GAME("OnPlayerDeath(unit_datum_index: %08X, a2: %08X, a3: %08X, a4: %08X)", unit_datum_index,a2,a3,a4);
	//TRACE_GAME("OnPlayerDeath() - Team: %i", h2mod->get_unit_team_index(unit_datum_index));

	if (b_GunGame) {
		gunGame->playerDeath->setUnitDatumIndex(unit_datum_index);
		gunGame->playerDeath->execute();
	}

	if (b_Infection) {
		infectionHandler->playerDeath->setUnitDatumIndex(unit_datum_index);
		infectionHandler->playerDeath->execute();
	}

	return pplayer_death(unit_datum_index, a2, a3, a4);
}


void __stdcall OnPlayerScore(void* thisptr, unsigned short a2, int a3, int a4, int a5, char a6)
{
	//TRACE_GAME("update_player_score_hook ( thisptr: %08X, a2: %08X, a3: %08X, a4: %08X, a5: %08X, a6: %08X )", thisptr, a2, a3, a4, a5, a6);


#pragma region GunGame Handler
	if (a5 == 7) //player got a kill?
	{
		int PlayerIndex = a2;
		if (b_GunGame) {
			gunGame->playerKill->setPlayerIndex(PlayerIndex);
			gunGame->playerKill->execute();
		}
	}

#pragma endregion

	pupdate_player_score(thisptr, a2, a3, a4, a5, a6);
}

void PatchGameDetailsCheck()
{
	NopFill(h2mod->GetBase() + 0x219D6D, 2);
}

void H2MOD::PatchWeaponsInteraction(bool b_Enable)
{
	//Client Sided Patch
	DWORD offset = h2mod->GetBase() + 0x55EFA;
	BYTE assm[5] = { 0xE8, 0x18, 0xE0,0xFF, 0xFF };
	if (!b_Enable)
	{
		memset(assm, 0x90, 5);
	}
	WriteBytes(offset, assm, 5);
}

/*
TODO: might be useful for updating player count while in-lobby
typedef bool(__cdecl *PacketHandler)(void *packet, int size, void *data);

int __cdecl player_add_packet_handler(void *packet, int size, void *data)
{
	update_player_count();
	PacketHandler game_player_add_handler = reinterpret_cast<PacketHandler>(h2mod->GetBase() + 0x1F06B6);
	return game_player_add_handler(packet, size, data);
}

bool __cdecl player_remove_packet_handler(void *packet, int size, void *data)
{
	update_player_count();
	PacketHandler game_player_remove_handler = reinterpret_cast<PacketHandler>(h2mod->GetBase() + 0x1F08BC);
	return game_player_remove_handler(packet, size, data);
}
*/

typedef void(__cdecl *onGameEngineChange)(int a1);
onGameEngineChange ponGameEngineChange;

void __cdecl onGameEngineChange_hook(int a1)
{
	ponGameEngineChange(a1); //sets game engine
    //NOTE: the AI globals initializers get called after this method returns

	overrideUnicodeMessage = false;
	isLobby = true;

	DWORD GameGlobals = *(DWORD*)(h2mod->GetBase() + ((h2mod->Server) ? 0x4CB520 : 0x482D3C));
	BYTE GameState = *(BYTE*)(h2mod->GetBase() + ((h2mod->Server) ? 0x3C40AC : 0x420FC4));
	BYTE GameEngine = *(BYTE*)(GameGlobals + 0x8);

	//based on what onGameEngineChange has changed
	//we do our stuff bellow

	if (GameEngine == MAIN_MENU_ENGINE)
	{
		addDebugText("GameEngine: Main-Menu, apply patches");

		if (b_Halo2Final && !h2mod->Server)
			h2f->Dispose();

		if (b_Infection) {
			infectionHandler->deinitializer->execute();
		}

		if (b_GunGame) {
			gunGame->deinitializer->execute();
		}

		H2Tweaks::disableAI_MP(); //TODO: get dedi offset
		H2Tweaks::FixRanksIcons();
		H2Tweaks::disable60FPSCutscenes();

		return;
	}

	b_Infection = false;
	b_GunGame = false;
	b_Halo2Final = false;
	b_H2X = false;

	wchar_t* variant_name = (wchar_t*)(h2mod->GetBase() + ((h2mod->Server) ? 0x534A18 : 0x97777C));
	TRACE_GAME("[h2mod] OnMapLoad engine mode %d, variant name %ws", GameEngine, variant_name);

	if (GameEngine == MULTIPLAYER_ENGINE)
	{
		addDebugText("GameEngine: Multi-player, apply patches");

		if (wcsstr(variant_name, L"zombies") > 0 || wcsstr(variant_name, L"Zombies") > 0 || wcsstr(variant_name, L"Infection") > 0 || wcsstr(variant_name, L"infection") > 0)
		{
			TRACE_GAME("[h2mod] Zombies Turned on!");
			b_Infection = true;
		}

		if (wcsstr(variant_name, L"GunGame") > 0 || wcsstr(variant_name, L"gungame") > 0 || wcsstr(variant_name, L"Gungame") > 0)
		{
			TRACE_GAME("[h2mod] GunGame Turned on!");
			b_GunGame = true;
		}

		if (wcsstr(variant_name, L"H2F") > 0 || wcsstr(variant_name, L"h2f") > 0 || wcsstr(variant_name, L"Halo2Final") > 0 || wcsstr(variant_name, L"halo2final") > 0)
		{
			TRACE_GAME("[h2mod] Halo2Final Turned on!");
			b_Halo2Final = true;
		}

		if (wcsstr(variant_name, L"H2X") > 0 || wcsstr(variant_name, L"h2x") > 0)
		{
			TRACE_GAME("[h2mod] Halo 2 Xbox Rebalance Turned on!");
			b_H2X = true;
		}

		H2Tweaks::enableAI_MP(); //TODO: get dedi offset
		H2Tweaks::applyHitfix(); // "fix hit registration"
		H2Tweaks::setCrosshairPos(H2Config_crosshair_offset);
		//H2Tweaks::applyShaderTweaks(); 

	}

	if (GameEngine != 3 && GameState == 3)
	{
		if (b_Infection) {
			infectionHandler->initializer->execute();
		}

		if (b_GunGame) {
			gunGame->initializer->execute();
		}
	}
#pragma region H2V Stuff
	if (!h2mod->Server)
	{
		//Crashfix
		//*(int*)(h2mod->GetBase() + 0x464940) = 0;
		//*(int*)(h2mod->GetBase() + 0x46494C) = 0;
		//*(int*)(h2mod->GetBase() + 0x464958) = 0;
		//*(int*)(h2mod->GetBase() + 0x464964) = 0;
		
		if (!h2mod->Server) //h2v stuff
		{
			//Crashfix
			//*(int*)(h2mod->GetBase() + 0x464940) = 0;
			//*(int*)(h2mod->GetBase() + 0x46494C) = 0;
			//*(int*)(h2mod->GetBase() + 0x464958) = 0;
			//*(int*)(h2mod->GetBase() + 0x464964) = 0;

			if (GameState == 3) {
				if (b_H2X)
					H2X::Initialize();
				else
					H2X::Deinitialize();
			}	
		}

	}
	else {
#pragma endregion

#pragma region H2Server Stuff
		if (GameEngine != 3 && GameState == 3)
		{
			if (b_H2X)
				H2X::Initialize();
			else
				H2X::Deinitialize();
		}

		else if (GameEngine == SINGLE_PLAYER_ENGINE) { //if anyone wants to run code on map load single player
			addDebugText("GameEngine: Single-player, apply patches");

			H2Tweaks::setCrosshairPos(H2Config_crosshair_offset);
			H2Tweaks::enable60FPSCutscenes();
		}
	}
}

bool __cdecl OnPlayerSpawn(int a1)
{
	overrideUnicodeMessage = false;
	//once players spawn we aren't in lobby anymore ;)
	isLobby = false;
	//TRACE_GAME("OnPlayerSpawn(a1: %08X)", a1);
	int PlayerIndex = a1 & 0x000FFFF;

	if (b_Infection) {
		infectionHandler->preSpawnPlayer->setPlayerIndex(PlayerIndex);
		infectionHandler->preSpawnPlayer->execute();
	}

	if (b_GunGame) {
		gunGame->preSpawnPlayer->setPlayerIndex(PlayerIndex);
		gunGame->preSpawnPlayer->execute();
	}
	bool ret = pspawn_player(a1);	

	if (b_Infection) {
		infectionHandler->spawnPlayer->setPlayerIndex(PlayerIndex);
		infectionHandler->spawnPlayer->execute();
	}

	if (b_GunGame) {
		gunGame->spawnPlayer->setPlayerIndex(PlayerIndex);
		gunGame->spawnPlayer->execute();
	}

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

void __stdcall join_game(void* thisptr, int a2, int a3, int a4, int a5, XNADDR* host_xn, int a7, int a8, int a9, int a10, int a11, char a12, int a13, int a14)
{
	Connected = false;
	NetworkActive = false;

	memcpy(&join_game_xn, host_xn, sizeof(XNADDR));

	trace(L"join_game host_xn->ina.s_addr: %08X ", host_xn->ina.s_addr);

	sockaddr_in SendStruct;

	if (host_xn->ina.s_addr != H2Config_ip_wan)
	{
		TRACE("XN is not equal to the WAN address, assigning external XN");
		SendStruct.sin_addr.s_addr = host_xn->ina.s_addr;
	}
	else
	{ 
		SendStruct.sin_addr.s_addr = H2Config_ip_lan;
	}
	short nPort = (ntohs(host_xn->wPortOnline) + 1);

	TRACE("join_game nPort: %i", nPort);

	SendStruct.sin_port = htons(nPort);

	TRACE("join_game SendStruct.sin_port: %i", ntohs(SendStruct.sin_port));
	TRACE("join_game xn_port: %i", ntohs(host_xn->wPortOnline));

	//SendStruct.sin_port = htons(1001); // These kinds of things need to be fixed too cause we would have the port in the XNADDR struct...
	SendStruct.sin_family = AF_INET;

	int securitysend_1001 = sendto(game_sock, (char*)User.SecurityPacket, 8 + sizeof(XNADDR), 0, (SOCKADDR *)&SendStruct, sizeof(SendStruct));

	if (securitysend_1001 != (8 + sizeof(XNADDR)))
		TRACE("join_game Security Packet Send had return different than len: %i", securitysend_1001);

	User.CreateUser(host_xn, FALSE);


	if (securitysend_1001 == SOCKET_ERROR )
	{
		TRACE("join_game Security Packet - Socket Error True");
		TRACE("join_game Security Packet - WSAGetLastError(): %08X", WSAGetLastError());
	}

	return pjoin_game(thisptr, a2, a3, a4, a5, host_xn, a7, a8, a9, a10, a11, a12, a13, a14);
}

int __cdecl connect_establish_write(void* a1, int a2, int a3)
{
	TRACE("connect_establish_write(a1: %08X,a2 %08X, a3: %08X)", a1, a2, a3);
	h2mod->securityPacketProcessing();

	return pconnect_establish_write(a1, a2, a3);
}

typedef int(__cdecl *object_p_hook)(int s_object_placement_data, int a2, signed int a3, int a4);
object_p_hook object_p_hook_method;

int __cdecl objectPHook(int s_object_placement_data, int object_definition_index, int object_owner, int unk) {
	if (h2mod->hookedObjectDefs.find(object_definition_index) == h2mod->hookedObjectDefs.end()) {
		//ingame only
		wchar_t* mapName = (wchar_t*)(h2mod->GetBase() + 0x97737C);
		TRACE_GAME("MapName=%s, object_definition_index: %08X", mapName, object_definition_index);
		h2mod->hookedObjectDefs.insert(object_definition_index);
	}
	return object_p_hook_method(s_object_placement_data, object_definition_index, object_owner, unk);
}

typedef BOOL(__stdcall *is_debugger_present)();
is_debugger_present is_debugger_present_method;

BOOL __stdcall isDebuggerPresent() {
	return false;
}

typedef void*(__stdcall *tload_wgit)(void* thisptr, int a2, int a3, int a4, unsigned short a5);
tload_wgit pload_wgit;

void* __stdcall OnWgitLoad(void* thisptr, int a2, int a3, int a4, unsigned short a5) {
	int wgit = a2;

	//char NotificationPlayerText[20];
	//sprintf(NotificationPlayerText, "WGIT ID: %d", a2);
	//addDebugText(NotificationPlayerText);
	//MessageBoxA(NULL, NotificationPlayerText, "WGITness", MB_OK);

	//void* thisptr = 
	pload_wgit(thisptr, wgit, a3, a4, a5);
	return thisptr;
}

typedef int(__cdecl *build_gui_list)(int a1, int a2, int a3);
build_gui_list build_gui_list_method;

int __cdecl buildGuiList(int a1, int a2, int a3) {
	if (b_Infection && a1 == (DWORD)(h2mod->GetBase() + 0x3d3620) && !gameManager->isHost()) {
		a2 = 1;
	}
	return build_gui_list_method(a1, a2, a3);
}

typedef int(__cdecl *change_team)(int a1, int a2);
change_team change_team_method;

int __cdecl changeTeam(int a1, int a2) {
	wchar_t* variant_name = (wchar_t*)(((char*)h2mod->GetBase()) + ((h2mod->Server) ? 0x534A18 : 0x97777C));
	if (wcsstr(variant_name, L"RvB") > 0 && a2 != 0 && a2 != 1) {
		//rvb mode enabled, don't change teams
		return 4732 * a1;
	}
	return change_team_method(a1, a2);
}

void __cdecl print_to_console(char *output)
{
	const static std::string prefix = "[HSC Print] ";
	commands->display(prefix + output);
}

void DuplicateDataBlob(DATA_BLOB  *pDataIn, DATA_BLOB  *pDataOut)
{
	pDataOut->cbData = pDataIn->cbData;
	pDataOut->pbData = static_cast<BYTE*>(LocalAlloc(LMEM_FIXED, pDataIn->cbData));
	CopyMemory(pDataOut->pbData, pDataIn->pbData, pDataIn->cbData);
}

BOOL WINAPI CryptProtectDataHook(
	_In_       DATA_BLOB                 *pDataIn,
	_In_opt_   LPCWSTR                   szDataDescr,
	_In_opt_   DATA_BLOB                 *pOptionalEntropy,
	_Reserved_ PVOID                     pvReserved,
	_In_opt_   CRYPTPROTECT_PROMPTSTRUCT *pPromptStruct,
	_In_       DWORD                     dwFlags,
	_Out_      DATA_BLOB                 *pDataOut
)
{
	DuplicateDataBlob(pDataIn, pDataOut);

	return TRUE;
}

BOOL WINAPI CryptUnprotectDataHook(
	_In_       DATA_BLOB                 *pDataIn,
	_Out_opt_  LPWSTR                    *ppszDataDescr,
	_In_opt_   DATA_BLOB                 *pOptionalEntropy,
	_Reserved_ PVOID                     pvReserved,
	_In_opt_   CRYPTPROTECT_PROMPTSTRUCT *pPromptStruct,
	_In_       DWORD                     dwFlags,
	_Out_      DATA_BLOB                 *pDataOut
)
{
	if (CryptUnprotectData(pDataIn, ppszDataDescr, pOptionalEntropy, pvReserved, pPromptStruct, dwFlags, pDataOut) == FALSE) {
		DuplicateDataBlob(pDataIn, pDataOut); // if decrypting the data fails just assume it's unencrypted
	}

	return TRUE;
}

char filo_write__encrypted_data_hook(filo *file_ptr, DWORD nNumberOfBytesToWrite, LPVOID lpBuffer)
{
	DWORD file_size = GetFileSize(file_ptr->handle, NULL);

	if (file_size > nNumberOfBytesToWrite) // clear the file as unencrypted data is shorter then encrypted data.
		FiloInterface::change_size(file_ptr, 0);
	return FiloInterface::write(file_ptr, lpBuffer, nNumberOfBytesToWrite);
}

//int static_lod_state = static_lod::cinematic;
DWORD calculate_model_lod;
DWORD calculate_model_lod_detour_end;
__declspec(naked) void calculate_model_lod_detour()
{
	__asm
	{
		// replaced code
		// todo check if this is needed when using a static LOD, might save on some processor time
		call calculate_model_lod

		cmp H2Config_static_lod_state, 0
		jz END_DETOUR

		mov eax, H2Config_static_lod_state
		sub eax, 1 // convert setting to in-game model LOD value (0 - 5, L1 - L6)

		END_DETOUR:
		jmp calculate_model_lod_detour_end
	}
}
void H2MOD::securityPacketProcessing()
{

	if (!gameManager->isHost())
	{
		sockaddr_in SendStruct;

		if (join_game_xn.ina.s_addr != H2Config_ip_wan)
			SendStruct.sin_addr.s_addr = join_game_xn.ina.s_addr;
		else
			SendStruct.sin_addr.s_addr = H2Config_ip_lan;

		SendStruct.sin_port = join_game_xn.wPortOnline;
		SendStruct.sin_family = AF_INET;

		int securitysend_1000 = sendto(game_sock_1000, (char*)User.SecurityPacket, 8 + sizeof(XNADDR), 0, (SOCKADDR *)&SendStruct, sizeof(SendStruct));
	}
}

typedef void(__cdecl *on_custom_map_change)(const void* a1);
on_custom_map_change on_custom_map_change_method;

void __cdecl onCustomMapChange(const void* a1) {
	on_custom_map_change_method(a1);
	//map changed, send update
	mapManager->sendMapInfoPacket();
}

typedef char(__stdcall *intercept_map_load)(LPCRITICAL_SECTION* thisx, const void *a2);
intercept_map_load intercept_map_load_method;

char __stdcall interceptMapLoad(LPCRITICAL_SECTION* thisx, const void *a2) {
	LPCRITICAL_SECTION *v2; // ebx@1
	struct _RTL_CRITICAL_SECTION *v3; // ebp@1
	char result; // al@2

	TRACE_GAME("[h2mod] Intercepted map load - crash function");

	typedef char(__thiscall *map_filetime_check)(LPCRITICAL_SECTION* thisx, int a2, unsigned int a3);
	map_filetime_check map_filetime_check_method = (map_filetime_check)(h2mod->GetBase() + 0xC1E01);

	typedef char(__thiscall *map_touch)(LPCRITICAL_SECTION* thisx, int a2);
	map_touch map_touch_method = (map_touch)(h2mod->GetBase() + 0xC2541);

	typedef char(*unknown_function)();
	unknown_function unknown_function_method = (unknown_function)(h2mod->GetBase() + 0x4541);

	typedef char(__stdcall *unknown_function2)(int a1);
	unknown_function2 unknown_function_method2 = (unknown_function2)(h2mod->GetBase() + 0xC2069);

	typedef int(__thiscall *map_limit_touch)(int thisx, int a2);
	map_limit_touch map_limit_touch_method = (map_limit_touch)(h2mod->GetBase() + 0xC1FA6);

	v2 = thisx;
	v3 = *thisx;

	TRACE_GAME("[h2mod] Intercepted map load - about to enter critical section");
	EnterCriticalSection(*thisx);
	TRACE_GAME("[h2mod] Intercepted map load - in critical section");
	if (a2
		&& *((WORD *)v2 + 74008) < 0x32u
		&& map_filetime_check_method(v2, (int)a2, 0xB90u)
		&& !map_touch_method(v2, (int)a2))
	{
		if (!unknown_function_method())
			unknown_function_method2((int)a2);

		map_limit_touch_method((int)v2, (int)a2);
		TRACE_GAME("[h2mod] Intercepted map load - memcpy");
		memcpy(&v2[740 * (*((WORD *)v2 + 0x12118))++ + 4], a2, 0xB90u);
		LeaveCriticalSection(v3);
		TRACE_GAME("[h2mod] Intercepted map load - left critical section");
		result = 1;
	}
	else
	{
		LeaveCriticalSection(v3);
		TRACE_GAME("[h2mod] Intercepted map load - left critical section");
		result = 0;
	}
	return result;
}

void H2MOD::ApplyHooks() {
	/* Should store all offsets in a central location and swap the variables based on h2server/halo2.exe*/
	/* We also need added checks to see if someone is the host or not, if they're not they don't need any of this handling. */
	if (this->Server == false) {
		TRACE_GAME("Applying client hooks...");
		/* These hooks are only built for the client, don't enable them on the server! */

#pragma region H2V Hooks
		DWORD dwBack;

		//pload_wgit = (tload_wgit)DetourClassFunc((BYTE*)this->GetBase() + 0x2106A2, (BYTE*)OnWgitLoad, 13);
		//VirtualProtect(pload_wgit, 4, PAGE_EXECUTE_READWRITE, &dwBack);

		intercept_map_load_method = (intercept_map_load)DetourClassFunc((BYTE*)this->GetBase() + 0xC259B, (BYTE*)interceptMapLoad, 13);
		VirtualProtect(intercept_map_load_method, 4, PAGE_EXECUTE_READWRITE, &dwBack);

		//on_custom_map_change_method = (on_custom_map_change)DetourFunc((BYTE*)this->GetBase() + 0x32176, (BYTE*)onCustomMapChange, 5);
		//VirtualProtect(on_custom_map_change_method, 4, PAGE_EXECUTE_READWRITE, &dwBack);

		//boot method
		calls_session_boot_method = (calls_session_boot)DetourClassFunc((BYTE*)this->GetBase() + 0x1CCE9B, (BYTE*)calls_session_boot_sub_1cce9b, 8);
		VirtualProtect(calls_session_boot_method, 4, PAGE_EXECUTE_READWRITE, &dwBack);

		psub_4F17A = (tsub_4F17A)DetourFunc((BYTE*)this->GetBase() + 0x4F17A, (BYTE*)sub_4F17A, 7);
		VirtualProtect(psub_4F17A, 4, PAGE_EXECUTE_READWRITE, &dwBack);

		pjoin_game = (tjoin_game)DetourClassFunc((BYTE*)this->GetBase() + 0x1CDADE, (BYTE*)join_game, 13);
		VirtualProtect(pjoin_game, 4, PAGE_EXECUTE_READWRITE, &dwBack);

		pspawn_player = (spawn_player)DetourFunc((BYTE*)this->GetBase() + 0x55952, (BYTE*)OnPlayerSpawn, 6);
		VirtualProtect(pspawn_player, 4, PAGE_EXECUTE_READWRITE, &dwBack);

		ponGameEngineChange = (onGameEngineChange)DetourFunc((BYTE*)this->GetBase() + 0x49E4C, (BYTE*)onGameEngineChange_hook, 8);
		VirtualProtect(ponGameEngineChange, 4, PAGE_EXECUTE_READWRITE, &dwBack);

		pupdate_player_score = (update_player_score)DetourClassFunc((BYTE*)this->GetBase() + 0xD03ED, (BYTE*)OnPlayerScore, 12);
		VirtualProtect(pupdate_player_score, 4, PAGE_EXECUTE_READWRITE, &dwBack);

		pplayer_death = (player_death)DetourFunc((BYTE*)this->GetBase() + 0x17B674, (BYTE*)OnPlayerDeath, 9);
		VirtualProtect(pplayer_death, 4, PAGE_EXECUTE_READWRITE, &dwBack);

		//0x20E15A
		show_error_screen_method = (show_error_screen)DetourFunc((BYTE*)h2mod->GetBase() + 0x20E15A, (BYTE*)showErrorScreen, 8);
		VirtualProtect(show_error_screen_method, 4, PAGE_EXECUTE_READWRITE, &dwBack);

		//TODO: turn on if you want to debug halo2.exe from start of process
		//is_debugger_present_method = (is_debugger_present)DetourFunc((BYTE*)h2mod->GetBase() + 0x39B394, (BYTE*)isDebuggerPresent, 5);
		//VirtualProtect(is_debugger_present_method, 4, PAGE_EXECUTE_READWRITE, &dwBack);

		//TODO: use for object spawn hooking
		//0x132163
		//object_p_hook_method = (object_p_hook)DetourFunc((BYTE*)this->GetBase() + 0x132163, (BYTE*)objectPHook, 6);
		//VirtualProtect(object_p_hook_method, 4, PAGE_EXECUTE_READWRITE, &dwBack);

		//TODO: expensive, use for debugging/searching
		//string_display_hook_method = (string_display_hook)DetourFunc((BYTE*)h2mod->GetBase() + 0x287AB5, (BYTE*)stringDisplayHook, 5);
		//VirtualProtect(string_display_hook_method, 4, PAGE_EXECUTE_READWRITE, &dwBack);

		//TODO: for when live list is ready
		//live checks removed will make users exit to live menu instead of network browser :(
		//live_check_method = (live_check)DetourFunc((BYTE*)this->GetBase() + 0x1BA418, (BYTE*)clientXboxLiveCheck, 9);
		//VirtualProtect(live_check_method, 4, PAGE_EXECUTE_READWRITE, &dwBack);

		//live_check_method2 = (live_check2)DetourFunc((BYTE*)this->GetBase() + 0x1B1643, (BYTE*)clientXboxLiveCheck2, 9);
		//VirtualProtect(live_check_method2, 4, PAGE_EXECUTE_READWRITE, &dwBack);

		//pResetRound=(ResetRounds)DetourFunc((BYTE*)this->GetBase() + 0x6B1C8, (BYTE*)OnNextRound, 7);
		//VirtualProtect(pResetRound, 4, PAGE_EXECUTE_READWRITE, &dwBack);

		build_gui_list_method = (build_gui_list)DetourFunc((BYTE*)this->GetBase() + 0x20D1FD, (BYTE*)buildGuiList, 8);
		VirtualProtect(build_gui_list_method, 4, PAGE_EXECUTE_READWRITE, &dwBack);

		/*
		WritePointer(GetBase() + 0x1F0B3A, player_add_packet_handler);
		WritePointer(GetBase() + 0x1F0B80, player_remove_packet_handler);
		*/

		// Patch out the code that displays the "Invalid Checkpoint" error
		// Start
		NopFill(GetBase() + 0x30857, 0x41);
		// Respawn
		NopFill(GetBase() + 0x8BB98, 0x2b);

		change_team_method = (change_team)DetourFunc((BYTE*)this->GetBase() + 0x2068F2, (BYTE*)changeTeam, 8);
		VirtualProtect(change_team_method, 4, PAGE_EXECUTE_READWRITE, &dwBack);

		// hook the print command to redirect the output to our console
		PatchCall(Base + 0xE9E50, print_to_console);

		PatchCall(GetBase() + 0x9B09F, filo_write__encrypted_data_hook);
		PatchWinAPICall(GetBase() + 0x9AF9E, CryptUnprotectDataHook);
		PatchWinAPICall(GetBase() + 0x9B08A, CryptProtectDataHook);

		calculate_model_lod = GetBase() + 0x19CA3E;
		calculate_model_lod_detour_end = GetBase() + 0x19CDA3 + 5;
		WriteJmpTo(GetBase() + 0x19CDA3, calculate_model_lod_detour);

		// set max model qaulity to L6
		WriteValue(GetBase() + 0x190B38 + 1, 5);
	}
#pragma endregion

#pragma region H2ServerHooks
	else {

		DWORD dwBack;

		on_custom_map_change_method = (on_custom_map_change)DetourFunc((BYTE*)this->GetBase() + 0x25738, (BYTE*)onCustomMapChange, 5);
		VirtualProtect(on_custom_map_change_method, 4, PAGE_EXECUTE_READWRITE, &dwBack);

		dedi_command_hook_method = (dedi_command_hook)DetourFunc((BYTE*)this->GetBase() + 0x1CCFC, (BYTE*)dediCommandHook, 7);
		VirtualProtect(dedi_command_hook_method, 4, PAGE_EXECUTE_READWRITE, &dwBack);

		//TODO: turn on later
		//std::thread t1(&MapManager::startListeningForClients, mapManager);
		//t1.detach();

		pspawn_player = (spawn_player)DetourFunc((BYTE*)this->GetBase() + 0x5DE4A, (BYTE*)OnPlayerSpawn, 6);
		VirtualProtect(pspawn_player, 4, PAGE_EXECUTE_READWRITE, &dwBack);//

		ponGameEngineChange = (onGameEngineChange)DetourFunc((BYTE*)this->GetBase() + 0x430CA, (BYTE*)onGameEngineChange_hook, 8);
		VirtualProtect(ponGameEngineChange, 4, PAGE_EXECUTE_READWRITE, &dwBack);

		pupdate_player_score = (update_player_score)DetourClassFunc((BYTE*)this->GetBase() + 0x8C84C, (BYTE*)OnPlayerScore, 12);
		VirtualProtect(pupdate_player_score, 4, PAGE_EXECUTE_READWRITE, &dwBack);//
		
		psub_4F17A = (tsub_4F17A)DetourFunc((BYTE*)this->GetBase() + 0x5637A, (BYTE*)sub_4F17A, 7);
		VirtualProtect(psub_4F17A, 4, PAGE_EXECUTE_READWRITE, &dwBack);

		pplayer_death = (player_death)DetourFunc((BYTE*)this->GetBase() + 0x152ED4, (BYTE*)OnPlayerDeath, 9);
		VirtualProtect(pplayer_death, 4, PAGE_EXECUTE_READWRITE, &dwBack);

		PatchWinAPICall(GetBase() + 0x85F5E, CryptProtectDataHook);

		PatchWinAPICall(GetBase() + 0x352538, CryptUnprotectDataHook);

		PatchCall(GetBase() + 0x85F73, filo_write__encrypted_data_hook);
	}

	//apply any network hooks
	network->applyNetworkHooks();
#pragma endregion
}

VOID CALLBACK UpdateDiscordStateTimer(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	update_player_count();
}

void H2MOD::Initialize()
{
	//HANDLE hThread = CreateThread(NULL, 0, Thread1, NULL, 0, NULL);

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
		H2Tweaks::setFOV(H2Config_field_of_view);
		//setSens(CONTROLLER, H2Config_sens_controller);
		//setSens(MOUSE, H2Config_sens_mouse);
		if (H2Config_raw_input)
			Mouseinput::Initialize();

		PatchGameDetailsCheck();
		//H2Tweaks::PatchPingMeterCheck();
		*(bool*)((char*)h2mod->GetBase() + 0x422450) = 1; //allows for all live menus to be accessed

		if (H2Config_discord_enable && H2GetInstanceId() == 1) {
			// Discord init
			DiscordInterface::SetDetails("Startup");
			DiscordInterface::Init();
			SetTimer(NULL, 0, 5000, UpdateDiscordStateTimer);
		}
	}

	//effects can vary (good or bad) depending on different software configurations.
	//if someone wants it they should set it manually or if it's really sought after make it a config setting.
	//and it should have been set Above_Normal not High imo -Glitchy Scripts.
	//if (!SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS)) {
	//	addDebugText("Error setting the process priority");
	//}
	
	TRACE_GAME("H2MOD - Initialized v0.4a");
	TRACE_GAME("H2MOD - BASE ADDR %08X", this->Base);

	//Network::Initialize();
	h2mod->ApplyHooks();
}

void H2MOD::Deinitialize() {

}

DWORD H2MOD::GetBase()
{
	return this->Base;
}

void H2MOD::IndicatorVisibility(bool toggle)
{
	BYTE* indicatorPointer = ((BYTE*)h2mod->GetBase() + 0x111197);
	DWORD dwback;
	VirtualProtect(indicatorPointer, 1, PAGE_EXECUTE_READWRITE, &dwback);

	if (toggle)
		*indicatorPointer = 0x74;
	else
		*indicatorPointer = 0xEB;

	VirtualProtect(indicatorPointer, 1, dwback, &dwback);
}
