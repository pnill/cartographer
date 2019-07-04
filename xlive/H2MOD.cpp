#include "stdafx.h"
#include "H2MOD.h"

#include "Blam\Engine\FileSystem\FiloInterface.h"
#include "H2MOD\Discord\DiscordInterface.h"
#include "H2MOD\Modules\OnScreenDebug\OnscreenDebug.h"
#include "H2MOD\Modules\HitFix\Hitfix.h"
#include "H2MOD\Modules\MapFix\SPMapfix.h"
#include "H2MOD\Modules\MapFix\MPMapfix.h"
#include "H2MOD\Modules\Input\Mouseinput.h"
#include "H2MOD\Modules\Tweaks\Tweaks.h"
#include "H2MOD\Modules\Config\Config.h"
#include "H2MOD\Modules\MainMenu\Ranks.h"
#include "H2MOD\Variants\H2X\H2X.h"
#include "H2MOD\Variants\GunGame\GunGame.h"
#include "H2MOD\Modules\Networking\Memory\bitstream.h"
#include "H2MOD\Modules\Networking\CustomPackets\CustomPackets.h"
#include <Shlwapi.h>
#include "H2MOD\Tags\global_tags_interface.h"
#include "MetaLoader\tag_loader.h"

H2MOD *h2mod = new H2MOD();
GunGame* gunGame = new GunGame();
Halo2Final *h2f = new Halo2Final();
XboxTick* xboxTickHandler = new XboxTick();
Infection* infectionHandler = new Infection();
FireFight* fireFightHandler = new FireFight();
HeadHunter* headHunterHandler = new HeadHunter();
DeviceShop* device_shop = new DeviceShop();
VariantPlayer* variant_player = new VariantPlayer();

bool b_H2X = false;
bool b_GunGame = false;
bool b_FireFight = false;
bool b_XboxTick = false;
bool b_Infection = false;
bool b_Halo2Final = false;
bool b_HeadHunter = false;

extern int H2GetInstanceId();
extern XUID xFakeXuid[4];
std::unordered_map<int, int> object_to_variant;


/*constants*/
const wchar_t* ZOMBIES = L"zombies";
const wchar_t* INFECTION = L"infection";
const wchar_t* GUNGAME = L"gungame";
const wchar_t* H2F = L"h2f";
const wchar_t* H2X = L"h2x";
const wchar_t* OGH2 = L"ogh2";
const wchar_t* GRAVEROBBER = L"graverobber";
const wchar_t* HEADHUNTER = L"headhunter";
const wchar_t* WARECONOMY = L"wareconomy";
const wchar_t* RVB = L"rvb";
/*         */
using namespace Blam::Cache::DataTypes;

int GAME_BUILD = 11122;
int EXECUTABLE_VERSION = 4;

int character_datum_from_index(BYTE index)
{
	DWORD global_scnr = *(DWORD*)((*(DWORD*)(char*)(h2mod->GetBase() + 0x479E74)) + 0x17C);
	DWORD var2 = *(DWORD*)((char*)h2mod->GetBase() + 0x482290);
	DWORD var3 = var2 + global_scnr;
	DWORD var4 = var3 + (8 * index) + 4;
	DWORD char_tag_datum = *(DWORD*)var4;

	return char_tag_datum;
}

int get_player_index_from_datum(DatumIndex unit_datum)
{
	ObjectEntityDefinition *unit_object = (&game_state_objects_header->object_header[unit_datum.ToAbsoluteIndex()])->object;
	return unit_object->PlayerDatum.ToAbsoluteIndex();
}

enum game_lobby_states : int
{
	not_in_lobby,
	in_lobby,
	unk1,
	in_game,
	unk2,
	joining_lobby
};

game_lobby_states call_get_lobby_state()
{
	typedef game_lobby_states(__cdecl* get_lobby_state)();
	auto p_get_lobby_state = reinterpret_cast<get_lobby_state>(h2mod->GetBase() + 0x1AD660);

	return p_get_lobby_state();
}

#pragma region engine calls

int __cdecl call_get_object(signed int object_datum_index, int object_type)
{
	//LOG_TRACE_GAME("call_get_object( object_datum_index: %08X, object_type: %08X )", object_datum_index, object_type);

	typedef int(__cdecl *get_object)(signed int object_datum_index, int object_type);
	get_object pget_object = (get_object)((char*)h2mod->GetBase() + ((h2mod->Server) ? 0x11F3A6 : 0x1304E3));

	return pget_object(object_datum_index, object_type);
}

int __cdecl call_unit_reset_equipment_datum(DatumIndex unit_datum_index)
{
	//LOG_TRACE_GAME("unit_reset_equipment(unit_datum_index: %08X)", unit_datum_index);
	typedef int(__cdecl *unit_reset_equipment)(DatumIndex unit_datum_index);
	unit_reset_equipment punit_reset_equipment = (unit_reset_equipment)(((char*)h2mod->GetBase()) + ((h2mod->Server) ? 0x133030 : 0x1441E0));
	if (unit_datum_index != -1 && unit_datum_index != 0)
	{
		return punit_reset_equipment(unit_datum_index);
	}

	return 0;
}

int __cdecl call_unit_reset_equipment(int unit_datum_index)
{
	//LOG_TRACE_GAME("unit_reset_equipment(unit_datum_index: %08X)", unit_datum_index);
	typedef int(__cdecl *unit_reset_equipment)(int unit_datum_index);
	unit_reset_equipment punit_reset_equipment = (unit_reset_equipment)(((char*)h2mod->GetBase()) + ((h2mod->Server) ? 0x133030 : 0x1441E0));
	if (unit_datum_index != -1 && unit_datum_index != 0)
	{
		return punit_reset_equipment(unit_datum_index);
	}

	return 0;
}

void __cdecl call_hs_object_destroy_datum(DatumIndex object_datum_index)
{
	//LOG_TRACE_GAME("hs_object_destory(object_datum_index: %08X)", object_datum_index);
	typedef void(__cdecl *hs_object_destroy)(DatumIndex object_datum_index);
	hs_object_destroy phs_object_destroy = (hs_object_destroy)(((char*)h2mod->GetBase()) + ((h2mod->Server) ? 0x124ED5 : 0xFDCFD)); // update on dedis

	return phs_object_destroy(object_datum_index);
}


void __cdecl call_hs_object_destroy(int object_datum_index)
{
	//LOG_TRACE_GAME("hs_object_destory(object_datum_index: %08X)", object_datum_index);
	typedef void(__cdecl *hs_object_destroy)(int object_datum_index);
	hs_object_destroy phs_object_destroy = (hs_object_destroy)(((char*)h2mod->GetBase()) + ((h2mod->Server) ? 0x124ED5 : 0xFDCFD)); // update dedi

	return phs_object_destroy(object_datum_index);
}

signed int __cdecl call_unit_inventory_next_weapon(unsigned short unit)
{
	//LOG_TRACE_GAME("unit_inventory_next_weapon(unit: %08X)", unit);
	typedef signed int(__cdecl *unit_inventory_next_weapon)(unsigned short unit);
	unit_inventory_next_weapon punit_inventory_next_weapon = (unit_inventory_next_weapon)(((char*)h2mod->GetBase()) + 0x139E04);

	return punit_inventory_next_weapon(unit);
}

bool __cdecl call_assign_equipment_to_unit_datum(DatumIndex unit, int object_index, short unk)
{
	//LOG_TRACE_GAME("assign_equipment_to_unit(unit: %08X,object_index: %08X,unk: %08X)", unit, object_index, unk);
	typedef bool(__cdecl *assign_equipment_to_unit)(DatumIndex unit, int object_index, short unk);
	assign_equipment_to_unit passign_equipment_to_unit = (assign_equipment_to_unit)(((char*)h2mod->GetBase()) + ((h2mod->Server) ? 0x1330FA : 0x1442AA));

	return passign_equipment_to_unit(unit, object_index, unk);
}


bool __cdecl call_assign_equipment_to_unit(int unit, int object_index, short unk)
{
	//LOG_TRACE_GAME("assign_equipment_to_unit(unit: %08X,object_index: %08X,unk: %08X)", unit, object_index, unk);
	typedef bool(__cdecl *assign_equipment_to_unit)(int unit, int object_index, short unk);
	assign_equipment_to_unit passign_equipment_to_unit = (assign_equipment_to_unit)(((char*)h2mod->GetBase()) + ((h2mod->Server) ? 0x1330FA : 0x1442AA));

	return passign_equipment_to_unit(unit, object_index, unk);
}

int __cdecl call_object_placement_data_new_datum(void* s_object_placement_data, DatumIndex object_definition_index, DatumIndex object_owner, int unk)
{

	//LOG_TRACE_GAME("object_placement_data_new(s_object_placement_data: %08X,",s_object_placement_data);
	//LOG_TRACE_GAME("object_definition_index: %08X, object_owner: %08X, unk: %08X)", object_definition_index, object_owner, unk);

	typedef int(__cdecl *object_placement_data_new)(void*, DatumIndex, DatumIndex, int);
	object_placement_data_new pobject_placement_data_new = (object_placement_data_new)(((char*)h2mod->GetBase()) + ((h2mod->Server) ? 0x121033 : 0x132163));

	return pobject_placement_data_new(s_object_placement_data, object_definition_index, object_owner, unk);


}
int __cdecl call_object_placement_data_new(void* s_object_placement_data, int object_definition_index, int object_owner, int unk)
{
	//LOG_TRACE_GAME("object_placement_data_new(s_object_placement_data: %08X,",s_object_placement_data);
	//LOG_TRACE_GAME("object_definition_index: %08X, object_owner: %08X, unk: %08X)", object_definition_index, object_owner, unk);

	typedef int(__cdecl *object_placement_data_new)(void*, int, int, int);
	object_placement_data_new pobject_placement_data_new = (object_placement_data_new)(((char*)h2mod->GetBase()) + ((h2mod->Server) ? 0x121033 : 0x132163));

	return pobject_placement_data_new(s_object_placement_data, object_definition_index, object_owner, unk);
}

/*int __cdecl call_entity_datum_to_gamestate_datum(int entity_datum)
{
	typedef int(__cdecl *entity_datum_to_gamestate_datum)(int entity_datum);
	entity_datum_to_gamestate_datum pentity_datum_to_gamestate_datum = (entity_datum_to_gamestate_datum)((BYTE*)h2mod->GetBase() + 0x1F2211);

	return pentity_datum_to_gamestate_datum(entity_datum);
}*/

signed int __cdecl call_object_new(void* pObject)
{
	//LOG_TRACE_GAME("object_new(pObject: %08X)", pObject);

	typedef int(__cdecl *object_new)(void*);
	object_new pobject_new = (object_new)(((char*)h2mod->GetBase()) + ((h2mod->Server) ? 0x125B77 : 0x136CA7));

	return pobject_new(pObject);

}

bool __cdecl call_add_object_to_sync(int gamestate_object_datum)
{
	typedef int(__cdecl *add_object_to_sync)(int gamestate_object_datum);
	add_object_to_sync p_add_object_to_sync = (add_object_to_sync)((char*)h2mod->GetBase() + ((h2mod->Server) ? 0x1B2C44 : 0x1B8D14));

	return p_add_object_to_sync(gamestate_object_datum);
}

/* We should really make this stuff into a struct/class, and access it that way it'd be much cleaner... */
int get_actor_datum_from_unit_datum(int unit_datum)
{
	int unit_ptr = call_get_object(unit_datum, 3);
	if (unit_ptr)
	{
		return *(int*)((BYTE*)unit_ptr + 0x130);
	}

	return -1;
}

/* This looks at the actors table to get the character datum which is assigned to the specific actor. */
int get_char_datum_from_actor(int actor_datum)
{

	__int16 actor_index = actor_datum & 0xFFFF;
	DWORD actor_table_ptr = *(DWORD*)((BYTE*)h2mod->GetBase() + 0xA965DC);
	DWORD actor_table = *(DWORD*)((BYTE*)actor_table_ptr + 0x44);
	DWORD actor = (DWORD)((BYTE*)actor_table + (actor_index * 0x898));
	int character_datum = *(int*)((BYTE*)actor+0x54);

	return character_datum;
}

/*This is to get the datum of the last player who damaged the datum/unit provided */
int get_damage_owner(int damaged_unit_index)
{
	int damaged_player_ptr = call_get_object(damaged_unit_index, 3);
	if (damaged_player_ptr)
	{
		return *(int*)((BYTE*)damaged_player_ptr + 0xC8); // player_ptr/unit_ptr + 0xC8 = damaging player this works on vehicles/AI and such too.
	}

	return -1;
}

int __cdecl call_get_character_sid_from_datum(int char_datum)
{
	typedef int(__cdecl *get_character_sid_from_datum)(int datum);
	get_character_sid_from_datum pget_character_sid_from_datum = (get_character_sid_from_datum)((char*)h2mod->GetBase() + 0x31796C);

	return pget_character_sid_from_datum(char_datum);
}

int __cdecl call_fill_creation_data_from_object_index(int object_index, void* creation_data)
{
	typedef int(__cdecl *fill_creation_data_from_object_index)(int datum,void* creation_data);
	fill_creation_data_from_object_index pfill_creation_data_from_object_index = (fill_creation_data_from_object_index)((char*)h2mod->GetBase() + 0x1F24ED);


	return pfill_creation_data_from_object_index(object_index, creation_data);
}

signed int __cdecl object_new_hook(void *pObject)
{
	int variant_index = *(int*)((char*)pObject + 0xC);
	signed int result = call_object_new(pObject);

	//unsigned __int16 object_index = result & 0xFFFF;

	object_to_variant[result] = variant_index;

	//wchar_t DebugText[255] = { 0 };
	//ZeroMemory(DebugText, sizeof(DebugText));
	//wsprintf(DebugText, L"AI object_new hook - object_index: %08X - variant_index: %08X - datum: %08X", result, variant_index);

	//LOG_TRACE_GAME_N("AI object_new hook - object_index: %08X - variant_index: %08X - datum: %08X", result, variant_index);

	//addDebugText(DebugText);

	return result;
}

typedef int(__stdcall *tc_simulation_unit_entity_definition_creation_encode)(void* thisptr, int creation_data_size, void* creation_data, int a3, void* packet);
tc_simulation_unit_entity_definition_creation_encode pc_simulation_unit_entity_definition_encode;

int __stdcall c_simulation_unit_entity_definition_creation_encode(void *thisptr, int creation_data_size, void* creation_data, int a3, void* packet)
{
	//LOG_TRACE_GAME_N("c_simulation_unit_entity_definition_creation_encode()\r\nthisptr: %08X, creation_data_size: %i, creation_data: %08X, a3: %i, packet: %08X", thisptr, creation_data_size, creation_data, a3, packet);
	int object_permutation_index = *(int*)((char*)creation_data + 0x24);
	if (object_permutation_index != -1)
	{
		//LOG_TRACE_GAME_N("creation_data+0x24: %08X", object_permutation_index);

		bitstream::p_data_encode_bool()(packet, "object-permutation-exists", 1);
		bitstream::p_data_encode_integer()(packet, "object-permutation-index", object_permutation_index, 32);
		//LOG_TRACE_GAME_N("c_simulation_unit_entity_encode - object-permutation-exists packet: %08X, *packet: %08X", packet, *(int*)packet);

	}
	else
		bitstream::p_data_encode_bool()(packet, "object-permutation-exists", 0);

	int ret = pc_simulation_unit_entity_definition_encode(thisptr, creation_data_size, creation_data, a3, packet);
	return ret;
}


typedef int(__stdcall *tc_simulation_unit_entity_definition_creation_decode)(void* thisptr, int creation_data_size, void* creation_data,void* packet);
tc_simulation_unit_entity_definition_creation_decode pc_simulation_unit_entity_definition_decode;

int __stdcall c_simulation_unit_entity_definition_creation_decode(void *thisptr, int creation_data_size, void* creation_data, void* packet)
{
	//LOG_TRACE_GAME_N("c_simulation_unit_entity_definition_creation_decode()\r\nthisptr: %08X, creation_data_size: %i, creation_data: %08X, packet: %08X", thisptr, creation_data_size, creation_data, packet);

	if (bitstream::p_data_decode_bool()(packet, "object-permutation-exists"))
	{
		//LOG_TRACE_GAME_N("c_simulation_unit_entity_decode - object-permutation-exists packet: %08X, *packet: %08X", packet, *(int*)packet);
		int object_permutation_index = bitstream::p_data_decode_integer()(packet, "object-permutation-index", 32);
		*(int*)((char*)creation_data + 0x24) = object_permutation_index;

		//LOG_TRACE_GAME_N("object_permutation_index: %08X", object_permutation_index);
	}
	else
		*(int*)((char*)creation_data + 0x24) = -1;

	int ret = pc_simulation_unit_entity_definition_decode(thisptr, creation_data_size, creation_data, packet);
	return ret;
}

typedef int(__stdcall *tset_unit_creation_data)(unsigned int object_index, void* object_creation_data);
tset_unit_creation_data pset_unit_creation_data;

int __stdcall set_unit_creation_data_hook(unsigned int object_index, void* object_creation_data)
{
	int result = pset_unit_creation_data(object_index, object_creation_data);

	if (object_to_variant[object_index] != 0)
	{
		//We should have allocated an additional 4 bytes above 0x24 so we'll write our in between 0x24 and 0x28
		*(int*)((char*)object_creation_data + 0x24) = object_to_variant[object_index];

		/*wchar_t DebugText[255] = { 0 };
		ZeroMemory(DebugText, sizeof(DebugText));
		wsprintf(DebugText, L"AI unit_creation_data_setup hook - object_index: %08X - variant_index: %08X", object_index, object_to_variant[object_index]);

		LOG_TRACE_GAME_N("set_unit_creation_data_hook - object_index: %08X, variant_index: %08X", object_index, object_to_variant[object_index]);
		addDebugText(DebugText);*/
	}
	else
		*(int*)((char*)object_creation_data + 0x24) = -1;



	return result;
}

typedef bool(__cdecl *tset_unit_color_data)(int, unsigned __int16, int a3);
tset_unit_color_data pset_unit_color_data;

bool __cdecl set_unit_color_data_hook(int a1, unsigned __int16 a2, int a3)
{
	int object_creation_data = a1 - 0x10;
	int object_permutation_index = *(int*)((char*)object_creation_data + 0x24);

	LOG_TRACE_GAME("set_unit_color_data_hook - {:x}", object_permutation_index);

	if ( object_permutation_index == -1)
		return pset_unit_color_data(a1, a2, a3);

	return 0;
}

typedef bool(__stdcall *tcreate_unit_hook)(void*, int, int, void*);
tcreate_unit_hook pcreate_unit_hook;

bool __stdcall create_unit_hook(void* pCreationData, int a2, int a3, void* pObject)
{
	if (*(int*)((char*)pCreationData + 0x24) != -1)
	{
		//wchar_t DebugText[255] = { 0 };
		//ZeroMemory(DebugText, sizeof(DebugText));
		//wsprintf(DebugText, L"create_unit_hook - variant type: %08X - ", *(int*)((char*)pCreationData + 0x24));

		//addDebugText(DebugText);

		*(int*)((char*)pObject + 0xC) = *(int*)((char*)pCreationData+0x24);
	}

	return pcreate_unit_hook(pCreationData, a2, a3, pObject);
}

EngineType H2MOD::GetEngineType()
{
	DWORD GameGlobals = *(DWORD*)(h2mod->GetBase() + ((h2mod->Server) ? 0x4CB520 : 0x482D3C));

	switch (*(BYTE*)(GameGlobals + 0x8))
	{
	case 1:
		return EngineType::SINGLE_PLAYER_ENGINE;
	case 2:
		return EngineType::MULTIPLAYER_ENGINE;
	case 3:
		return EngineType::MAIN_MENU_ENGINE;
	default:
		return EngineType::INVALID_ENGINE_TYPE; // if everything ok shouldn't ever get here
	}
}

inline wchar_t* H2MOD::GetLobbyGameVariantName()
{
	return (wchar_t*)(h2mod->GetBase() + ((h2mod->Server) ? 0x534A18 : 0x97777C));
}

void H2MOD::exit_game()
{
	if (h2mod->Server)
		return;

	if (GetEngineType() != EngineType::MAIN_MENU_ENGINE)
	{
		// request_squad_browser
		WriteValue<BYTE>(H2BaseAddr + 0x978BAC, 1);

		typedef void(__cdecl *load_main_menu_with_context)(int context);
		auto load_main_menu_with_context_impl = reinterpret_cast<load_main_menu_with_context>(H2BaseAddr + 0x08EAF);
		load_main_menu_with_context_impl(0);
	}

	typedef int(__cdecl *leave_game_type)(int a1);
	leave_game_type leave_game = (leave_game_type)(h2mod->GetBase() + 0x216388);
	leave_game(0);
}

#pragma endregion

void H2MOD::kick_player(int peerIndex) {
	typedef void(__thiscall* game_session_boot)(void* a1, int peer_index, bool a3);
	auto p_game_session_boot = (game_session_boot)(h2mod->GetBase() + 0x1CCE9B);

	DWORD* ptr = (DWORD*)(((char*)h2mod->GetBase()) + 0x420FE8);
	LOG_TRACE_GAME("about to kick player index={}", peerIndex);
	p_game_session_boot((DWORD*)(*ptr), peerIndex, true);
}

typedef int(__cdecl *show_error_screen)(int a1, signed int a2, int a3, __int16 a4, int a5, int a6);
show_error_screen show_error_screen_method;

int __cdecl showErrorScreen(int a1, signed int a2, int a3, __int16 a4, int a5, int a6) {
	if (a2 == 0x117)
	{
		LOG_TRACE_FUNC("Ignoring need to reinstall maps");
		return 0;
	}
	return show_error_screen_method(a1, a2, a3, a4, a5, a6);
}

typedef signed int(__cdecl *string_display_hook)(int a3, unsigned int a4, int a5, int a6);
string_display_hook string_display_hook_method;


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

void H2MOD::logToDedicatedServerConsole(wchar_t* message) {

	if (!h2mod->Server)
		return;

	typedef signed int(*dedi_print)(const char* a1, ...);
	dedi_print dedi_print_method = (dedi_print)(h2mod->GetBase() + 0x2354C8);

	dedi_print_method((const char*)(message));
}

typedef int(__cdecl *dedi_command_hook)(wchar_t** a1, int a2, char a3);
dedi_command_hook dedi_command_hook_method;

int __cdecl dediCommandHook(wchar_t** command_line_args, int a2, int a3) {

	wchar_t* command = command_line_args[0];
	std::wstring wsCommand(command);
	if (command[0] == L'$') {
		h2mod->logToDedicatedServerConsole(L"Running custom command\n");
		//run the chatbox commands
		h2mod->handle_command(wsCommand);
	}

	return dedi_command_hook_method(command_line_args, a2, a3);
}

bool H2MOD::is_team_play() {
	//0x971A90 only works in lobby (not in game)
	//0x978CB4 works in both
	DWORD ptr = *((DWORD*)(this->GetBase() + 0x978CB4));
	ptr += 0x1C68;
	return *(BYTE*)ptr;
}

#pragma region PlayerFunctions

float H2MOD::get_distance(int playerIndex1, int playerIndex2) {
	float player1X, player1Y, player1Z;
	float player2X, player2Y, player2Z;

	player1X = h2mod->get_player_x(playerIndex1, false);
	player1Y = h2mod->get_player_y(playerIndex1, false);
	player1Z = h2mod->get_player_z(playerIndex1, false);

	player2X = h2mod->get_player_x(playerIndex2, false);
	player2Y = h2mod->get_player_y(playerIndex2, false);
	player2Z = h2mod->get_player_z(playerIndex2, false);

	float dx = abs(player1X - player2X);
	float dy = abs(player1Y - player2Y);
	float dz = abs(player1Z - player2Z);

	return sqrt(pow(dx, 2) + pow(dy, 2) + pow(dz, 2));
}

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
	int DynamicObjBase = (int)game_state_objects_header;
	if (playerdatum != -1)
		dyanamic = *(DWORD *)(*(DWORD *)(DynamicObjBase + 68) + 12 * (unsigned __int16)playerdatum + 8);
	return dyanamic;
}



void GivePlayerWeapon2(int PlayerIndex, int WeaponId, short Unk)
{
	//LOG_TRACE_GAME("GivePlayerWeapon(PlayerIndex: %08X, WeaponId: %08X)", PlayerIndex, WeaponId);

	int unit_datum = h2mod->get_unit_datum_from_player_index(PlayerIndex);

	if (unit_datum != -1 && unit_datum != 0)
	{
		char* nObject = new char[0xC4];

		call_object_placement_data_new(nObject, WeaponId, unit_datum, 0);

		int object_index = call_object_new(nObject);

		//if (bReset == true)
		//	call_unit_reset_equipment(unit_datum);

		call_assign_equipment_to_unit(unit_datum, object_index, Unk);
	}

}

void GivePlayerWeapon(int PlayerIndex, int WeaponId, bool bReset)
{
	//LOG_TRACE_GAME("GivePlayerWeapon(PlayerIndex: %08X, WeaponId: %08X)", PlayerIndex, WeaponId);

	int unit_datum = h2mod->get_unit_datum_from_player_index(PlayerIndex);

	if (unit_datum != -1 && unit_datum != 0)
	{
		char* nObject = new char[0xC4];

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

		LOG_TRACE_GAME(L"[H2MOD]::get_player_index_from_name( {0} : {1} )", playername, comparename);

		if (wcscmp(comparename, playername))
		{
			return i;
		}
	}
	return 0xFFFFFFFF;
}

wchar_t* H2MOD::get_player_name_from_index(int pIndex)
{
	DWORD player_table_ptr;
	if (!h2mod->Server)
		player_table_ptr = *(DWORD*)(this->GetBase() + 0x004A8260);
	else
		player_table_ptr = *(DWORD*)(this->GetBase() + 0x004D64C4);
	player_table_ptr += 0x44;

	return (wchar_t*)(*(DWORD*)player_table_ptr + (pIndex * 0x204) + 0x40);
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
		int unit_datum_index_check = (int)*(int*)(*(DWORD*)player_table_ptr + (pIndex * 0x204) + 0x28);
		LOG_TRACE_FUNC("Checking datum: {0:x} - index: {1} against datum: {2:x}", unit_datum_index_check, pIndex, unit_datum_index);

		if (unit_datum_index == unit_datum_index_check)
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
	{
		BYTE orig_val = *(BYTE*)(((char*)((h2mod->Server) ? 0x3000274C : 0x30002BA0) + (pIndex * 0x204)));
		BYTE *orig_addr = (BYTE*)(((char*)((h2mod->Server) ? 0x3000274C : 0x30002BA0) + (pIndex * 0x204)));

		LOG_TRACE_GAME("set_unit_biped(biped: {0}, index: {1}), orig_val: {2}, address: {3:p}", biped, pIndex, orig_val, (void*)orig_addr);
		*(BYTE*)(((char*)((h2mod->Server) ? 0x3000274C : 0x30002BA0) + (pIndex * 0x204))) = biped;
	}
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

	LOG_TRACE_GAME("AddressOffset: {:x}", AddressOffset);
	switch (sound)
	{
	case SoundType::Slayer:
		LOG_TRACE_GAME("AddressOffset + 0xd7dfb4 = {:p}", (void*)(AddressOffset + 0xd7dfb4));
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

void H2MOD::CustomSoundPlay(const wchar_t* soundName, int delay)
{
	auto playSound = [=]()
	{
		//std::unique_lock<std::mutex> lck(h2mod->sound_mutex);
		std::chrono::system_clock::time_point timePoint = std::chrono::system_clock::now() + std::chrono::milliseconds(delay);

		LOG_TRACE_GAME(L"[H2MOD-SoundQueue] - attempting to play sound {0} - delaying {1} miliseconds first", soundName, delay);

		if (delay > 0)
			std::this_thread::sleep_until(timePoint);

		PlaySound(soundName, NULL, SND_FILENAME);
	};

	if (!h2mod->Server)
		std::thread(playSound).detach();
}

typedef char(__cdecl *player_death)(int unit_datum_index, int a2, char a3, char a4);
player_death pplayer_death;

typedef void(__stdcall *update_player_score)(void* thisptr, unsigned short a2, int a3, int a4, int a5, char a6);
update_player_score pupdate_player_score;

/* This is technically closer to object death than player-death as it impacts anything with health at all. */

char __cdecl OnPlayerDeath(int unit_datum_index, int a2, char a3, char a4)
{

	//LOG_TRACE_GAME("OnPlayerDeath(unit_datum_index: %08X, a2: %08X, a3: %08X, a4: %08X)", unit_datum_index,a2,a3,a4);

	/* The first value within a2 ( *(DWORD*)(a2) ) appears to be the datum_index of a player from the gamestate_player_table */

	/* This is the unit of the player who last damaged the object*/
	int damaging_player_unit = get_damage_owner(unit_datum_index);

	if (b_HeadHunter)
	{
		DatumIndex dead_player = DatumIndex(unit_datum_index);
		headHunterHandler->playerDeath->SetDeadPlayer(dead_player); // set this so we can spawn a skull on their position.
		headHunterHandler->playerDeath->execute();
	}

	if (b_FireFight)
	{
		/* Hack until rest of code is changed to support DatumIndex vs int*/
		DatumIndex ai_datum = DatumIndex(unit_datum_index);

		/*
			In firefight we want to track AI deaths and execute on them to grant points.
		*/
		fireFightHandler->playerDeath->SetPlayerIndex(*(DatumIndex*)(a2)); // this is the player datum of player who killed the datum.
		fireFightHandler->playerDeath->SetKilledDatum(ai_datum);
		fireFightHandler->playerDeath->execute();
	}

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
	//LOG_TRACE_GAME("update_player_score_hook ( thisptr: %08X, a2: %08X, a3: %08X, a4: %08X, a5: %08X, a6: %08X )", thisptr, a2, a3, a4, a5, a6);
	//20/10/2018 18:46:51.541 update_player_score_hook ( thisptr: 3000595C, a2: 00000000, a3: 00000002, a4: 00000001, a5: 00000007, a6: 00000001 )
	// / 10 / 2018 18:46 : 51.541 update_player_score_hook(thisptr : 3000595C, a2 : 00000000, a3 : 00000000, a4 : 00000001, a5 : FFFFFFFF, a6 : 00000000)
	//	20 / 10 / 2018 18 : 46 : 51.541 update_player_score_hook(thisptr : 3000595C, a2 : 00000001, a3 : 00000003, a4 : 00000001, a5 : 00000009, a6: 00000001)

	//20 / 10 / 2018 18:48 : 39.756 update_player_score_hook(thisptr : 3000595C, a2 : 00000001, a3 : 00000002, a4 : 00000001, a5 : 00000007, a6 : 00000001)
	//	20 / 10 / 2018 18 : 48 : 39.756 update_player_score_hook(thisptr : 3000595C, a2 : 00000001, a3 : 00000000, a4 : 00000001, a5 : FFFFFFFF, a6 : 00000000)
	//	20 / 10 / 2018 18 : 48 : 39.756 update_player_score_hook(thisptr : 3000595C, a2 : 00000000, a3 : 00000003, a4 : 00000001, a5 : 00000009, a6: 00000001)

	if (a5 == -1)
	{
		if(b_HeadHunter)
			return;
	}

	if (a5 == 7) //player got a kill?
	{
		int PlayerIndex = a2;

		if (b_GunGame) {
			gunGame->playerKill->setPlayerIndex(PlayerIndex);
			gunGame->playerKill->execute();
		}

		if (b_HeadHunter)
		{
			return;
		}
	}

	pupdate_player_score(thisptr, a2, a3, a4, a5, a6);
}

void PatchGameDetailsCheck()
{
	NopFill<2>(h2mod->GetBase() + 0x219D6D);
}

void H2MOD::DisableWeaponPickup(bool b_Enable)
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

int OnAutoPickUpHandler(DatumIndex player_datum, DatumIndex object_datum)
{
	int(_cdecl*AutoHandler)(DatumIndex, DatumIndex);
	AutoHandler = (int(_cdecl*)(DatumIndex, DatumIndex))((char*)h2mod->GetBase() + ((!h2mod->Server) ? 0x57AA5 : 0x5FF9D));

	if (b_HeadHunter)
	{
		headHunterHandler->itemInteraction->SetPlayerIndex(player_datum);
		bool handled = headHunterHandler->itemInteraction->SetInteractedObject(object_datum);
		headHunterHandler->itemInteraction->execute();

		if (handled)
			return 0;
	}

	return AutoHandler(player_datum, object_datum);
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

void get_object_table_memory()
{
	game_state_players = (GameStatePlayerTable*)(*(DWORD*)(((BYTE*)h2mod->GetBase() + (h2mod->Server ? 0x4D64C4 : 0x4A8260))));
	game_state_objects_header = (GameStateObjectHeaderTable*)(*(DWORD*)(((BYTE*)h2mod->GetBase() + (h2mod->Server ? 0x50C8EC : 0x4E461C))));
	game_state_actors = (GameStateActorTable*)((*(DWORD*)((BYTE*)h2mod->GetBase() + (h2mod->Server ? 0x9A1C5C : 0xA965DC))));
}

// this gets called after game globals are updated fyi (which includes game engine type)
typedef void(__cdecl *set_random_number)(int a1);
set_random_number p_set_random_number;

void __cdecl OnMapLoad(int a1)
{
	overrideUnicodeMessage = false;
	isLobby = true;

	//based on what onGameEngineChange has changed
	//we do our stuff bellow

	if (h2mod->GetEngineType() == EngineType::MAIN_MENU_ENGINE)
	{
		addDebugText("GameEngine: Main-Menu");
		object_to_variant.clear();

		if (!NetworkSession::localPeerIsSessionHost()) {
			advLobbySettings->resetLobbySettings();
		}

		if (b_Halo2Final && !h2mod->Server)
			h2f->Dispose();

		if (b_Infection) {
			infectionHandler->deinitializer->execute();
		}

		if (b_GunGame) {
			gunGame->deinitializer->execute();
		}

		H2Tweaks::disableAI_MP();
		UIRankPatch();
		H2Tweaks::disable60FPSCutscenes();
		H2Tweaks::setHz();

		p_set_random_number(a1);
		return;
	}

	b_H2X = false;
	b_GunGame = false;
	b_XboxTick = false;
	b_FireFight = false;
	b_Infection = false;
	b_HeadHunter = false;
	b_Halo2Final = false;

	wchar_t* variant_name = h2mod->GetLobbyGameVariantName();
	LOG_TRACE_GAME(L"[h2mod] OnMapLoad engine mode {0}, variant name {1}", h2mod->GetEngineType(), variant_name);
	BYTE GameState = *(BYTE*)(h2mod->GetBase() + ((h2mod->Server) ? 0x3C40AC : 0x420FC4));

	if (h2mod->GetEngineType() == EngineType::MULTIPLAYER_ENGINE)
	{
		addDebugText("GameEngine: Multiplayer");

		// StrStrIW returns pointer to first substring occurrence, NULL othewise
		if (StrStrIW(variant_name, ZOMBIES) || StrStrIW(variant_name, INFECTION))
		{
			LOG_TRACE_GAME("[h2mod] Zombies Turned on!");
			b_Infection = true;
		}

		if (StrStrIW(variant_name, GUNGAME))
		{
			LOG_TRACE_GAME("[h2mod] GunGame Turned on!");
			b_GunGame = true;
		}

		if (StrStrIW(variant_name, H2F))
		{
			LOG_TRACE_GAME("[h2mod] Halo2Final Turned on!");
			b_Halo2Final = true;
		}

		if (StrStrIW(variant_name, H2X))
		{
			LOG_TRACE_GAME("[h2mod] Halo 2 Xbox Rebalance Turned on!");
			b_H2X = true;
		}
		else if (StrStrIW(variant_name, OGH2))
		{
			LOG_TRACE_GAME("[h2mod] 30 Tick Mod Activated!");
			b_XboxTick = true;
		}

		if (StrStrIW(variant_name, GRAVEROBBER) || StrStrIW(variant_name, HEADHUNTER))
		{
			LOG_TRACE_GAME("[h2mod] GraveRobber (Headhunter) Turned on!");
			b_HeadHunter = true;
		}

		if (StrStrIW(variant_name, WARECONOMY))
		{
			LOG_TRACE_GAME("[h2mod] Fire Fight Turned on!");
			b_FireFight = true;
		}

		get_object_table_memory();

		if (!b_XboxTick) 
		{
			HitFix::Initialize();
			MPMapFix::Initialize();
		}
		else
		{
			xboxTickHandler->preSpawnPlayer->execute();
		}
		
		H2Tweaks::enableAI_MP();
		H2Tweaks::setCrosshairPos(H2Config_crosshair_offset);

		H2Tweaks::setCrosshairSize(0, false);
		H2Tweaks::disable60FPSCutscenes();
		H2Tweaks::setSavedSens();
		//H2Tweaks::applyShaderTweaks(); 

		if (GameState == 3)
		{
			// send server map checksums to client
			//MapChecksumSync::SendState();
			//inform players of the current advanced lobby settings
			// TODO: issue #232
			//advLobbySettings->sendLobbySettingsPacket();

			if (b_Infection) {
				infectionHandler->initializer->execute();
			}

			if (b_GunGame) {
				gunGame->initializer->execute();
			}

			if (b_H2X)
				H2X::Initialize();
			else
				H2X::Deinitialize();

			if (b_Halo2Final)
				h2f->Initialize();
		}
	}

	else if (h2mod->GetEngineType() == EngineType::SINGLE_PLAYER_ENGINE)
	{
		//if anyone wants to run code on map load single player
		addDebugText("GameEngine: Singleplayer");

		H2Tweaks::setCrosshairPos(H2Config_crosshair_offset);
		H2Tweaks::enable60FPSCutscenes();
		H2Tweaks::setSavedSens();
		SPMapFix::Initialize();
	}

	p_set_random_number(a1);
}

typedef bool(__cdecl *spawn_player)(int a1);
spawn_player pspawn_player;

bool __cdecl OnPlayerSpawn(int a1)
{
	//I cant find somewhere to put this where it actually works (only needs to be done once on map load). It's only a few instructions so it shouldn't take long to execute.
	H2Tweaks::toggleKillVolumes(!AdvLobbySettings_disable_kill_volumes);

	overrideUnicodeMessage = false;
	//once players spawn we aren't in lobby anymore ;)
	isLobby = false;
	//LOG_TRACE_GAME("OnPlayerSpawn(a1: %08X)", a1);
	int PlayerIndex = a1;


	if (b_Infection) {
		infectionHandler->preSpawnPlayer->setPlayerIndex(PlayerIndex & 0x0000FFFF);
		infectionHandler->preSpawnPlayer->execute();
	}

	if (b_GunGame) {
		gunGame->preSpawnPlayer->setPlayerIndex(PlayerIndex & 0x0000FFFF);
		gunGame->preSpawnPlayer->execute();
	}

	if (b_XboxTick) {
		xboxTickHandler->preSpawnPlayer->execute();
	}

	bool ret = pspawn_player(a1);

	if (b_Infection) {
		infectionHandler->spawnPlayer->setPlayerIndex(PlayerIndex & 0x0000FFFF);
		infectionHandler->spawnPlayer->execute();
	}

	if (b_GunGame) {
		gunGame->spawnPlayer->setPlayerIndex(PlayerIndex & 0x0000FFFF);
		gunGame->spawnPlayer->execute();
	}

	return ret;
}

typedef int(__cdecl *object_p_hook)(int s_object_placement_data, int a2, signed int a3, int a4);
object_p_hook object_p_hook_method;

int __cdecl objectPHook(int s_object_placement_data, int object_definition_index, int object_owner, int unk) {
	if (h2mod->hookedObjectDefs.find(object_definition_index) == h2mod->hookedObjectDefs.end()) {
		//ingame only
		wchar_t* mapName = (wchar_t*)(h2mod->GetBase() + 0x97737C);
		LOG_TRACE_GAME(L"MapName={0}, object_definition_index: {1:x}", mapName, object_definition_index);
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

	pload_wgit(thisptr, wgit, a3, a4, a5);
	return thisptr;
}

typedef void(__cdecl *change_team)(int a1, int a2);
change_team p_change_local_team;

void __cdecl changeTeam(int localPlayerIndex, int teamIndex) {
	wchar_t* variant_name = h2mod->GetLobbyGameVariantName();
	if (StrStrIW(variant_name, RVB) != NULL && teamIndex != 0 && teamIndex != 1) {
		//rvb mode enabled, don't change teams
		return;
	}
	p_change_local_team(localPlayerIndex, teamIndex);
}

void H2MOD::set_local_team_index(int local_player_index, int team_index)
{
	// we only use player index 0 due to no splitscreen support but whatever
	typedef void(__cdecl* update_player_profile)(int local_player_index);
	auto p_update_player_profile = reinterpret_cast<update_player_profile>(h2mod->GetAddress(0x206A97, 0x0));

	p_change_local_team(local_player_index, team_index);
	p_update_player_profile(local_player_index); // fixes infection handicap glitch
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

typedef void(__cdecl *on_custom_map_change)(const void* a1);
on_custom_map_change on_custom_map_change_method;

void __cdecl onCustomMapChange(const void* a1) {
	on_custom_map_change_method(a1);
	//map changed, send update to all players
}

typedef char(__stdcall *intercept_map_load)(LPCRITICAL_SECTION* thisx, const void *a2);
intercept_map_load intercept_map_load_method;

char __stdcall interceptMapLoad(LPCRITICAL_SECTION* thisx, const void *a2) {
	LPCRITICAL_SECTION *v2; // ebx@1
	struct _RTL_CRITICAL_SECTION *v3; // ebp@1
	char result; // al@2

	LOG_TRACE_GAME("[h2mod] Intercepted map load - crash function");

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

	LOG_TRACE_GAME("[h2mod] Intercepted map load - about to enter critical section");
	EnterCriticalSection(*thisx);
	LOG_TRACE_GAME("[h2mod] Intercepted map load - in critical section");
	if (a2
		&& *((WORD *)v2 + 74008) < 0x32u
		&& map_filetime_check_method(v2, (int)a2, 0xB90u)
		&& !map_touch_method(v2, (int)a2))
	{
		if (!unknown_function_method())
			unknown_function_method2((int)a2);

		map_limit_touch_method((int)v2, (int)a2);
		LOG_TRACE_GAME("[h2mod] Intercepted map load - memcpy");
		memcpy(&v2[740 * (*((WORD *)v2 + 0x12118))++ + 4], a2, 0xB90u);
		LeaveCriticalSection(v3);
		LOG_TRACE_GAME("[h2mod] Intercepted map load - left critical section");
		result = 1;
	}
	else
	{
		LeaveCriticalSection(v3);
		LOG_TRACE_GAME("[h2mod] Intercepted map load - left critical section");
		result = 0;
	}
	return result;
}

typedef bool(__cdecl *tfn_c000bd114)(int);
tfn_c000bd114 pfn_c000bd114;
bool __cdecl fn_c000bd114_IsSkullEnabled(int skull_index)
{
	//bool result = pfn_c000bd114(skull_index);
	//return result;

	bool* var_c004d8320 = (bool*)(H2BaseAddr + 0x004d8320);
	bool(*fn_c00049833_IsEngineModeCampaign)() = (bool(*)())(H2BaseAddr + 0x00049833);

	bool result = false;
	if (skull_index <= 0xE && fn_c00049833_IsEngineModeCampaign())
		result = var_c004d8320[skull_index];
	if (skull_index == 0xA && AdvLobbySettings_mp_sputnik)
		result = true;
	else if (skull_index == 0x1 && AdvLobbySettings_mp_grunt_bday_party)
		result = true;
	else if (skull_index == 0x6 && AdvLobbySettings_mp_blind)
		result = true;
	return result;
}

bool GrenadeChainReactIsEngineMPCheck() {
	if (AdvLobbySettings_grenade_chain_react)
		return false;
	return h2mod->GetEngineType() == EngineType::MULTIPLAYER_ENGINE;
}

bool BansheeBombIsEngineMPCheck() {
	if (AdvLobbySettings_banshee_bomb)
		return false;
	return h2mod->GetEngineType() == EngineType::MULTIPLAYER_ENGINE;
}

bool FlashlightIsEngineSPCheck() {
	if (AdvLobbySettings_flashlight)
		return true;
	return h2mod->GetEngineType() == EngineType::SINGLE_PLAYER_ENGINE;
}

typedef bool(__cdecl* verify_game_version_on_join)(int executable_version, int build_version, int build_version2);
verify_game_version_on_join p_verify_game_version_on_join;

bool __cdecl VerifyGameVersionOnJoin(int executable_version, int build_version, int build_version2)
{
	return executable_version == EXECUTABLE_VERSION && build_version >= GAME_BUILD && build_version2 <= GAME_BUILD;
}

typedef bool(__cdecl* verify_executable_version)(int executable_version);
verify_executable_version p_verify_executable_version;

bool __cdecl VerifyExecutableVersion(int executable_version)
{
	return executable_version == EXECUTABLE_VERSION; // will not display servers that don't match this in server list
}

typedef void(__cdecl *get_game_version)(DWORD *executable_version, DWORD *build_version, DWORD *build_version2);
get_game_version p_get_game_version;

void __cdecl GetGameVersion(DWORD *executable_version, DWORD *build_version, DWORD *build_version2)
{
	*executable_version = EXECUTABLE_VERSION;
	*build_version = GAME_BUILD;
	*build_version2 = GAME_BUILD;
}

void GivePlayerWeaponDatum(DatumIndex unit_datum,DatumIndex weapon_datum)
{
	if (unit_datum != -1 && unit_datum != 0)
	{
		char* nObject = new char[0xC4];

		call_object_placement_data_new_datum(nObject, weapon_datum, unit_datum, 0);

		int object_index = call_object_new(nObject);

		if (object_index != -1)
		{
			call_unit_reset_equipment_datum(unit_datum);
			call_assign_equipment_to_unit_datum(unit_datum, object_index, 1);
		}
		else
			delete[] nObject;
	}
}

//This is used for maps with 'shops' where the device_acceleration_scale is an indicator that they're using the control device as a 'shop'
float get_device_acceleration_scale(DatumIndex device_datum)
{
	DWORD tag_header = *(DWORD*)((BYTE*)h2mod->GetBase() + (h2mod->Server ? 0x4A29BC : 0x47CD54));
	DWORD global_tag_instances = *(DWORD*)((BYTE*)h2mod->GetBase() + (h2mod->Server ? 0x4A29B8 : 0x47CD50));
	DWORD game_state_objects_header_table = *(DWORD*)((BYTE*)game_state_objects_header + 0x44);

	int device_gamestate_offset = device_datum.Index + device_datum.Index * 2;
	DWORD device_gamestate_datum_pointer = *(DWORD*)((BYTE*)game_state_objects_header_table + device_gamestate_offset * 4 + 8);
	DWORD device_control_datum = *(DWORD*)((BYTE*)device_gamestate_datum_pointer);

	__int16 device_control_index = device_control_datum & 0xFFFF;
	device_control_index = device_control_index << 4;

	DWORD device_control_tag_offset = *(DWORD*)((BYTE*)device_control_index + global_tag_instances + 8);
	float acceleration_scale = *(float*)((BYTE*)device_control_tag_offset + tag_header + 0x14);

	return acceleration_scale;

}

typedef int(__cdecl *tdevice_touch)(DatumIndex device_datum, DatumIndex unit_datum);
tdevice_touch pdevice_touch;

bool device_active = true;
//This happens whenever a player activates a device control.
int __cdecl device_touch(DatumIndex device_datum, DatumIndex unit_datum)
{
	if (h2mod->GetEngineType() == EngineType::MULTIPLAYER_ENGINE)
	{
		//We check this to see if the device control is a 'shopping' device, if so send a request to buy an item to the DeviceShop.
		if (get_device_acceleration_scale(device_datum) == 999.0f)
		{
			if (device_shop->BuyItem(device_datum, unit_datum)) // If the purchase was successful we won't execute the original device control action.
			{
				if (device_active == false)
					return pdevice_touch(device_datum, unit_datum);

				device_active = true;
				return 0;
			}
			// If the purchase fails (they don't have enough points), or the device is not a shopping device return normally.
			// In general's map returning normally will turn the point display red indicating the user has no points, we do not indicate that the purchase failed in any other way.
			if (device_active == false)
				return 0;

			device_active = false;
		}
	}

	return pdevice_touch(device_datum, unit_datum);
}

void H2MOD::ApplyUnitHooks()
{
	DWORD dwBack;

	BYTE packet_sz = 0x30;
	WriteBytes(h2mod->GetBase() + (h2mod->Server ? 0x1E1D8F : 0x1F8029), &packet_sz, 1);

	//This encodes the unit creation packet, only gets executed on host.
	pc_simulation_unit_entity_definition_encode = (tc_simulation_unit_entity_definition_creation_encode)DetourClassFunc((BYTE*)this->GetBase() + (h2mod->Server ? 0x1E2269 : 0x1F8503), (BYTE*)c_simulation_unit_entity_definition_creation_encode, 10);
	VirtualProtect(pc_simulation_unit_entity_definition_encode, 4, PAGE_EXECUTE_READWRITE, &dwBack);

	//This decodes the unit creation packet, only gets executed on client.
	pc_simulation_unit_entity_definition_decode = (tc_simulation_unit_entity_definition_creation_decode)DetourClassFunc((BYTE*)this->GetBase() + (h2mod->Server ? 0x1E22BD : 0x1F8557), (BYTE*)c_simulation_unit_entity_definition_creation_decode, 11);
	VirtualProtect(pc_simulation_unit_entity_definition_decode, 4, PAGE_EXECUTE_READWRITE, &dwBack);

	pdevice_touch = (tdevice_touch)DetourFunc((BYTE*)this->GetBase() + (h2mod->Server ? 0x158EE3 : 0x163420), (BYTE*)device_touch, 10);
	VirtualProtect(pdevice_touch, 4, PAGE_EXECUTE_READWRITE, &dwBack);

	//Only patch the object_new call on host during AI_Place function, no reason to hook all object_new calls.
	PatchCall(GetBase() + (h2mod->Server ? 0x2C3B56 : 0x318DEC), object_new_hook);

	//We update creation data here which is used later on to add data to the packet
	PatchCall(GetBase() + (h2mod->Server ? 0x1E1DE0 : 0x1F807A), set_unit_creation_data_hook);
	pset_unit_creation_data = (tset_unit_creation_data)(GetBase() + (h2mod->Server ? 0x1DD586 : 0x1F24ED));

	// Hooks a call within the creat_unit property on the client side in order to set their permutation index before spawn.
	PatchCall(GetBase() + (h2mod->Server ? 0x1E3BD4 : 0x1F9E6C), create_unit_hook);
	pcreate_unit_hook = (tcreate_unit_hook)(GetBase() + (h2mod->Server ? 0x1DE374 : 0x1F32DB));

	// Hooks the part of the unit spawn from simulation that handles setting their color data in order to ensure AI do not have their color overridden
	PatchCall(GetBase() + (h2mod->Server ? 0x1E3B9C : 0x1F9E34), set_unit_color_data_hook);
	pset_unit_color_data = (tset_unit_color_data)(GetBase() + (h2mod->Server ? 0x6D1BF : 0x6E5C3));
}

void H2MOD::ApplyHooks() {
	/* Should store all offsets in a central location and swap the variables based on h2server/halo2.exe*/
	/* We also need added checks to see if someone is the host or not, if they're not they don't need any of this handling. */
	DWORD dwBack;
	if (this->Server == false) {
		LOG_TRACE_GAME("Applying client hooks...");
		/* These hooks are only built for the client, don't enable them on the server! */

		p_verify_game_version_on_join = (verify_game_version_on_join)DetourFunc((BYTE*)this->GetBase() + 0x1B4C14, (BYTE*)VerifyGameVersionOnJoin, 5);
		VirtualProtect(p_verify_game_version_on_join, 4, PAGE_EXECUTE_READWRITE, &dwBack);

		p_get_game_version = (get_game_version)DetourFunc((BYTE*)this->GetBase() + 0x1B4BF5, (BYTE*)GetGameVersion, 8);
		VirtualProtect(p_get_game_version, 4, PAGE_EXECUTE_READWRITE, &dwBack);

		p_verify_executable_version = (verify_executable_version)DetourFunc((BYTE*)this->GetBase() + 0x1B4C32, (BYTE*)VerifyExecutableVersion, 8);
		VirtualProtect(p_verify_executable_version, 4, PAGE_EXECUTE_READWRITE, &dwBack);

		//pload_wgit = (tload_wgit)DetourClassFunc((BYTE*)this->GetBase() + 0x2106A2, (BYTE*)OnWgitLoad, 13);
		//VirtualProtect(pload_wgit, 4, PAGE_EXECUTE_READWRITE, &dwBack);

		intercept_map_load_method = (intercept_map_load)DetourClassFunc((BYTE*)this->GetBase() + 0xC259B, (BYTE*)interceptMapLoad, 13);
		VirtualProtect(intercept_map_load_method, 4, PAGE_EXECUTE_READWRITE, &dwBack);

		//on_custom_map_change_method = (on_custom_map_change)DetourFunc((BYTE*)this->GetBase() + 0x32176, (BYTE*)onCustomMapChange, 5);
		//VirtualProtect(on_custom_map_change_method, 4, PAGE_EXECUTE_READWRITE, &dwBack);

		// disable part of custom map tag verification
		NopFill<6>(GetBase() + 0x4FA0A);

		pspawn_player = (spawn_player)DetourFunc((BYTE*)this->GetBase() + 0x55952, (BYTE*)OnPlayerSpawn, 6);
		VirtualProtect(pspawn_player, 4, PAGE_EXECUTE_READWRITE, &dwBack);

		PatchCall(GetBase() + 0x49E95, OnMapLoad);
		p_set_random_number = (set_random_number)(GetBase() + 0x5912D);

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

		//pResetRound=(ResetRounds)DetourFunc((BYTE*)this->GetBase() + 0x6B1C8, (BYTE*)OnNextRound, 7);
		//VirtualProtect(pResetRound, 4, PAGE_EXECUTE_READWRITE, &dwBack);

		/*
		WritePointer(GetBase() + 0x1F0B3A, player_add_packet_handler);
		WritePointer(GetBase() + 0x1F0B80, player_remove_packet_handler);
		*/

		// Patch out the code that displays the "Invalid Checkpoint" error
		// Start
		NopFill<0x41>(GetBase() + 0x30857);
		// Respawn
		NopFill<0x2b>(GetBase() + 0x8BB98);

		p_change_local_team = (change_team)DetourFunc((BYTE*)this->GetBase() + 0x2068F2, (BYTE*)changeTeam, 8);
		VirtualProtect(p_change_local_team, 4, PAGE_EXECUTE_READWRITE, &dwBack);

		// hook the print command to redirect the output to our console
		PatchCall(GetBase() + 0xE9E50, print_to_console);

		PatchCall(GetBase() + 0x9B09F, filo_write__encrypted_data_hook);
		PatchWinAPICall(GetBase() + 0x9AF9E, CryptUnprotectDataHook);
		PatchWinAPICall(GetBase() + 0x9B08A, CryptProtectDataHook);

		calculate_model_lod = GetBase() + 0x19CA3E;
		calculate_model_lod_detour_end = GetBase() + 0x19CDA3 + 5;
		WriteJmpTo(GetBase() + 0x19CDA3, calculate_model_lod_detour);

		// set max model quality to L6
		WriteValue(GetBase() + 0x190B38 + 1, 5);

		pfn_c000bd114 = (tfn_c000bd114)DetourFunc((BYTE*)H2BaseAddr + 0x000bd114, (BYTE*)fn_c000bd114_IsSkullEnabled, 5);
		PatchCall(GetBase() + 0x00182d6d, GrenadeChainReactIsEngineMPCheck);
		PatchCall(GetBase() + 0x00092C05, BansheeBombIsEngineMPCheck);
		PatchCall(GetBase() + 0x0013ff75, FlashlightIsEngineSPCheck);

		Initialise_tag_loader();
		TagInterface::GlobalTagInterface.Init();
	}
	else {

		p_get_game_version = (get_game_version)DetourFunc((BYTE*)this->GetBase() + 0x1B0043, (BYTE*)GetGameVersion, 8);
		VirtualProtect(p_get_game_version, 4, PAGE_EXECUTE_READWRITE, &dwBack);

		on_custom_map_change_method = (on_custom_map_change)DetourFunc((BYTE*)this->GetBase() + 0x25738, (BYTE*)onCustomMapChange, 5);
		VirtualProtect(on_custom_map_change_method, 4, PAGE_EXECUTE_READWRITE, &dwBack);

		dedi_command_hook_method = (dedi_command_hook)DetourFunc((BYTE*)this->GetBase() + 0x1CCFC, (BYTE*)dediCommandHook, 7);
		VirtualProtect(dedi_command_hook_method, 4, PAGE_EXECUTE_READWRITE, &dwBack);

		//TODO: turn on later
		//std::thread t1(&MapManager::startListeningForClients, mapManager);
		//t1.detach();

		pspawn_player = (spawn_player)DetourFunc((BYTE*)this->GetBase() + 0x5DE4A, (BYTE*)OnPlayerSpawn, 6);
		VirtualProtect(pspawn_player, 4, PAGE_EXECUTE_READWRITE, &dwBack);

		PatchCall(GetBase() + 0x43113, OnMapLoad);
		p_set_random_number = (set_random_number)(GetBase() + 0x4E43C);

		pupdate_player_score = (update_player_score)DetourClassFunc((BYTE*)this->GetBase() + 0x8C84C, (BYTE*)OnPlayerScore, 12);
		VirtualProtect(pupdate_player_score, 4, PAGE_EXECUTE_READWRITE, &dwBack);//

		// disable part of custom map tag verification
		NopFill<6>(GetBase() + 0x56C0A);

		pplayer_death = (player_death)DetourFunc((BYTE*)this->GetBase() + 0x152ED4, (BYTE*)OnPlayerDeath, 9);
		VirtualProtect(pplayer_death, 4, PAGE_EXECUTE_READWRITE, &dwBack);

		PatchWinAPICall(GetBase() + 0x85F5E, CryptProtectDataHook);
		PatchWinAPICall(GetBase() + 0x352538, CryptUnprotectDataHook);
		PatchCall(GetBase() + 0x85F73, filo_write__encrypted_data_hook);
	}

	/* Labeled "AutoPickup" handler may be proximity to vehicles and such as well */
	PatchCall(h2mod->GetBase() + ((!h2mod->Server) ? 0x58789 : 0x60C81), OnAutoPickUpHandler);

	mapManager->applyGamePatches();
	ApplyUnitHooks();
}

VOID CALLBACK UpdateDiscordStateTimer(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	update_player_count();
}

void H2MOD::Initialize()
{
	if (!h2mod->Server)
	{
		this->Server = FALSE;

		H2Tweaks::setFOV(H2Config_field_of_view);
		H2Tweaks::setVehicleFOV(H2Config_vehicle_field_of_view);
		if (H2Config_raw_input)
			Mouseinput::Initialize();

		PatchGameDetailsCheck();

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

	LOG_TRACE_GAME("H2MOD - Initialized v0.5a");
	LOG_TRACE_GAME("H2MOD - BASE ADDR {:x}", this->Base);

	h2mod->ApplyHooks();
	tags::apply_patches();	
}

void H2MOD::Deinitialize() {

}

void H2MOD::IndicatorVisibility(bool toggle)
{
	// makes toggles between jz and jmp
	WriteValue<BYTE>(h2mod->GetBase() + 0x111197, toggle ? 0x74 : 0xEB);
}
