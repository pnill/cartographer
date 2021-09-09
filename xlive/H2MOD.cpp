
#include "H2MOD.h"

#include "H2MOD/Discord/DiscordInterface.h"
#include "H2MOD/Modules/GamePhysics/Patches/ProjectileFix.h"
#include "H2MOD/Modules/Input/Mouseinput.h"
#include "H2MOD/Modules/MainMenu/Ranks.h"
#include "H2MOD/Modules/Networking/Memory/bitstream.h"
#include "H2MOD/Modules/OnScreenDebug/OnscreenDebug.h"
#include "H2MOD/Modules/Tweaks/Tweaks.h"
#include "H2MOD/Tags/MetaLoader/tag_loader.h"
#include "H2MOD/Modules/MapManager/MapManager.h"
#include "H2MOD/Modules/Stats/StatsHandler.h"
#include "H2MOD/Modules/EventHandler/EventHandler.h"
#include "H2MOD/Modules/HudElements/HudElements.h"
#include "H2MOD/Modules/Input/PlayerControl.h"
#include "H2MOD/Modules/Input/KeyboardInput.h"
#include "H2MOD/Tags/MetaExtender.h"
#include "H2MOD/Modules/TagFixes/TagFixes.h"
#include "H2MOD/Modules/RenderHooks/RenderHooks.h"
#include "H2MOD/Modules/HaloScript/HaloScript.h"
#include "H2MOD/Modules/DirectorHooks/DirectorHooks.h"
#include "H2MOD/Modules/MainMenu/MapSlots.h"
#include "H2MOD/Modules/GamePhysics/Patches/MeleeFix.h"
#include "H2MOD/Modules/SpecialEvents/SpecialEvents.h"
#include "H2MOD/Modules/AdvLobbySettings/AdvLobbySettings.h"
#include "Util/Hooks/Hook.h"

#include "H2MOD/Engine/Engine.h"
#include "H2MOD/Modules/Config/Config.h"
#include "H2MOD/Modules/Input/ControllerInput.h"
#include "H2MOD/Modules/Console/ConsoleCommands.h"
#include "H2MOD/Modules/Networking/CustomPackets/CustomPackets.h"

#include "Blam/Engine/Game/DamageData.h"
#include "Blam/Engine/FileSystem/FiloInterface.h"
#include "Blam/Engine/Objects/GameStateObjects.h"
#include "Blam/Cache/TagGroups/multiplayer_globals_definition.hpp"

#include <float.h>
#pragma fenv_access (on)

H2MOD* h2mod = new H2MOD();
GunGame* gunGame = new GunGame();
DeviceShop* device_shop = new DeviceShop();
Infection* infectionHandler = new Infection();
FireFight* fireFightHandler = new FireFight();
HeadHunter* headHunterHandler = new HeadHunter();
VariantPlayer* variant_player = new VariantPlayer();
AdvLobbySettings* advLobbySettings = new AdvLobbySettings();
StatsHandler* stats_handler; 

extern int H2GetInstanceId();
std::unordered_map<int, int> object_to_variant;

bool b_H2X = false;
bool b_GunGame = false;
bool b_FireFight = false;
bool b_XboxTick = false;
bool b_Infection = false;
bool b_HeadHunter = false;

// TODO: remove these
s_datum_array* game_state_actors = nullptr;

std::unordered_map<wchar_t*, bool&> GametypesMap
{
	{ L"h2x", b_H2X },
	{ L"ogh2", b_XboxTick },
	{ L"gungame", b_GunGame },
	{ L"zombies", b_Infection },
	{ L"infection", b_Infection },
	{ L"wareconomy", b_FireFight },
	{ L"headhunter", b_HeadHunter },
	{ L"graverobber", b_HeadHunter }
};

int GAME_BUILD = 11122;
int EXECUTABLE_VERSION = 4;

//Currently not used in code base
int get_player_index_from_datum(datum unit_datum)
{
	return ((s_biped_object_definition*)s_game_state_objects::getObject(unit_datum))->PlayerDatum.ToAbsoluteIndex();
}

#pragma region engine calls

// Used to get damage on any object
typedef void(__cdecl p_object_cause_damage)(s_damage_data* damage_data, int damaged_object_indexes, __int16 a4, __int16 a5, __int16 a6, int a7);
p_object_cause_damage* c_object_cause_damage;

// Engine call to set damage applied on an object by a projectile
void __cdecl projectile_collision_object_cause_damage(s_damage_data* damage_data, int damaged_object_indexes, __int16 a4, __int16 a5, __int16 a6, int a7)
{
	// Hook on call to prevent guardian glitching on Infection gametype
	if (b_Infection) {
		if (damage_data->creator_datum != -1 && damage_data->field_10 != -1)
		{
			LOG_TRACE_GAME(
				"{} {} {} {} {} {} {} {}",
				damage_data->flags,
				IntToString<int>(damage_data->damage_tag_index, std::hex),
				IntToString<int>(damage_data->creator_datum.ToInt(), std::hex),
				IntToString<int>(damage_data->field_10, std::hex), //TODO reverse what field_10 is
				IntToString<int>(damage_data->field_14, std::hex),
				IntToString<int>(damage_data->field_18, std::hex),
				IntToString<int>(damage_data->field_1C, std::hex),
				IntToString<int>(damage_data->field_24, std::hex),
				IntToString<int>(damage_data->field_28, std::hex)
			);
			c_object_cause_damage(damage_data, damaged_object_indexes, a4, a5, a6, a7);
		}
		else
		{
			LOG_TRACE_GAME("GUARDIAN GLITCH PREVENTED");
		}
	}
	else
	{
		//Calls basic engine function when not in zombies game
		c_object_cause_damage(damage_data, damaged_object_indexes, a4, a5, a6, a7);
	}
}

//TODO: Document what entity datum is used for
int __cdecl call_entity_datum_to_gamestate_datum(int entity_datum)
{
	typedef int(__cdecl *entity_datum_to_gamestate_datum)(int entity_datum);
	entity_datum_to_gamestate_datum pentity_datum_to_gamestate_datum = (entity_datum_to_gamestate_datum)Memory::GetAddress(0x1F2211);
	return pentity_datum_to_gamestate_datum(entity_datum);
}

bool __cdecl call_add_object_to_sync(datum gamestate_object_datum)
{
	typedef int(__cdecl add_object_to_sync)(datum gamestate_object_datum);
	auto p_add_object_to_sync = Memory::GetAddress<add_object_to_sync*>(0x1B8D14, 0x1B2C44);

	return p_add_object_to_sync(gamestate_object_datum);
}

/* We should really make this stuff into a struct/class, and access it that way it'd be much cleaner... */
int get_actor_datum_from_unit_datum(int unit_datum)
{
	char* unit_ptr = Engine::Objects::try_and_get_data_with_type(unit_datum, FLAG(e_object_type::biped));
	if (unit_ptr)
	{
		return *(int*)(unit_ptr + 0x130);
	}

	return NONE;
}

/* This looks at the actors table to get the character datum which is assigned to the specific actor. */
int get_char_datum_from_actor(int actor_datum)
{
	__int16 actor_index = actor_datum & 0xFFFF;
	DWORD actor_table_ptr = *Memory::GetAddress<DWORD*>(0xA965DC);
	DWORD actor_table = *(DWORD*)((BYTE*)actor_table_ptr + 0x44);
	DWORD actor = (DWORD)((BYTE*)actor_table + (actor_index * 0x898));
	int character_datum = *(int*)((BYTE*)actor+0x54);

	return character_datum;
}

/*This is to get the datum of the last player who damaged the datum/unit provided */
int get_damage_owner(int damaged_unit_index)
{
	char* damaged_player_ptr = Engine::Objects::try_and_get_data_with_type(damaged_unit_index, FLAG(e_object_type::biped) | FLAG(e_object_type::vehicle));
	if (damaged_player_ptr)
	{
		return *(int*)(damaged_player_ptr + 0xC8); // player_ptr/unit_ptr + 0xC8 = damaging player this works on vehicles/AI and such too.
	}

	return NONE;
}

int __cdecl call_get_character_sid_from_datum(int char_datum)
{
	typedef int(__cdecl get_character_sid_from_datum)(int datum);
	auto p_get_character_sid_from_datum = Memory::GetAddress<get_character_sid_from_datum*>(0x31796C);

	return p_get_character_sid_from_datum(char_datum);
}

int __cdecl call_fill_creation_data_from_object_index(int object_index, void* creation_data)
{
	typedef int(__cdecl fill_creation_data_from_object_index)(int datum,void* creation_data);
	auto p_fill_creation_data_from_object_index = Memory::GetAddress<fill_creation_data_from_object_index*>(0x1F24ED);

	return p_fill_creation_data_from_object_index(object_index, creation_data);
}

signed int __cdecl object_new_hook(s_object_placement_data* new_object)
{
	int variant_index = *(int*)((char*)new_object + 0xC);
	int result = Engine::Objects::call_object_new(new_object);

	//unsigned __int16 object_index = result & 0xFFFF;

	if (result != NONE)
		object_to_variant[result] = variant_index;

	//wchar_t DebugText[255] = { 0 };
	//SecureZeroMemory(DebugText, sizeof(DebugText));
	//wsprintf(DebugText, L"AI object_new hook - object_index: %08X - variant_index: %08X - datum: %08X", result, variant_index);

	//LOG_TRACE_GAME_N("AI object_new hook - object_index: %08X - variant_index: %08X - datum: %08X", result, variant_index);

	//addDebugText(DebugText);

	return result;
}

typedef void(__stdcall *tc_simulation_unit_entity_definition_creation_encode)(void* thisptr, int creation_data_size, void* creation_data, int a3, bitstream* stream);
tc_simulation_unit_entity_definition_creation_encode pc_simulation_unit_entity_definition_encode;

void __stdcall c_simulation_unit_entity_definition_creation_encode(void *thisptr, int creation_data_size, void* creation_data, int a3, bitstream* stream)
{
	//LOG_TRACE_GAME_N("c_simulation_unit_entity_definition_creation_encode()\r\nthisptr: %08X, creation_data_size: %i, creation_data: %08X, a3: %i, packet: %08X", thisptr, creation_data_size, creation_data, a3, packet);
	int object_permutation_index = *(int*)((char*)creation_data + 0x24);
	if (object_permutation_index != NONE)
	{
		//LOG_TRACE_GAME_N("creation_data+0x24: %08X", object_permutation_index);

		stream->data_encode_bool("object-permutation-exists", 1);
		stream->data_encode_integer("object-permutation-index", object_permutation_index, 32);
		//LOG_TRACE_GAME_N("c_simulation_unit_entity_encode - object-permutation-exists packet: %08X, *packet: %08X", packet, *(int*)packet);

	}
	else
		stream->data_encode_bool("object-permutation-exists", 0);

	pc_simulation_unit_entity_definition_encode(thisptr, creation_data_size, creation_data, a3, stream);
}


typedef bool(__stdcall *tc_simulation_unit_entity_definition_creation_decode)(void* thisptr, int creation_data_size, void* creation_data, bitstream* stream);
tc_simulation_unit_entity_definition_creation_decode pc_simulation_unit_entity_definition_decode;

bool __stdcall c_simulation_unit_entity_definition_creation_decode(void *thisptr, int creation_data_size, void* creation_data, bitstream* stream)
{
	//LOG_TRACE_GAME_N("c_simulation_unit_entity_definition_creation_decode()\r\nthisptr: %08X, creation_data_size: %i, creation_data: %08X, packet: %08X", thisptr, creation_data_size, creation_data, packet);

	if (stream->data_decode_bool("object-permutation-exists"))
	{
		//LOG_TRACE_GAME_N("c_simulation_unit_entity_decode - object-permutation-exists packet: %08X, *packet: %08X", packet, *(int*)packet);
		int object_permutation_index = stream->data_decode_integer("object-permutation-index", 32);
		*(int*)((char*)creation_data + 0x24) = object_permutation_index;

		//LOG_TRACE_GAME_N("object_permutation_index: %08X", object_permutation_index);
	}
	else
		*(int*)((char*)creation_data + 0x24) = NONE;

	return pc_simulation_unit_entity_definition_decode(thisptr, creation_data_size, creation_data, stream);
}

void __stdcall set_unit_creation_data_hook(unsigned int object_index, void* object_creation_data)
{
	typedef void(__stdcall *tset_unit_creation_data)(unsigned int object_index, void* object_creation_data);
	Memory::GetAddress<tset_unit_creation_data>(0x1F24ED, 0x1DD586)(object_index, object_creation_data);

	if (object_to_variant.find(object_index) != object_to_variant.end())
	{
		//We should have allocated an additional 4 bytes above 0x24 so we'll write our in between 0x24 and 0x28
		*(int*)((char*)object_creation_data + 0x24) = object_to_variant[object_index];

		/*wchar_t DebugText[255] = { 0 };
		SecureZeroMemory(DebugText, sizeof(DebugText));
		wsprintf(DebugText, L"AI unit_creation_data_setup hook - object_index: %08X - variant_index: %08X", object_index, object_to_variant[object_index]);

		LOG_TRACE_GAME_N("set_unit_creation_data_hook - object_index: %08X, variant_index: %08X", object_index, object_to_variant[object_index]);
		addDebugText(DebugText);*/
	}
	else
		*(int*)((char*)object_creation_data + 0x24) = NONE;
}

typedef bool(__cdecl *tset_unit_color_data)(int, unsigned __int16, int a3);
tset_unit_color_data pset_unit_color_data;

bool __cdecl set_unit_color_data_hook(int a1, unsigned __int16 a2, int a3)
{
	int object_creation_data = a1 - 0x10;
	int object_permutation_index = *(int*)((char*)object_creation_data + 0x24);

	LOG_TRACE_GAME("set_unit_color_data_hook - {:x}", object_permutation_index);

	if (object_permutation_index == NONE)
		return pset_unit_color_data(a1, a2, a3);

	return 0;
}

bool __stdcall create_unit_hook(void* pCreationData, int a2, int a3, void* pObject)
{
	typedef bool(__stdcall *tcreate_unit_hook)(void*, int, int, void*);

	if (*(int*)((char*)pCreationData + 0x24) != NONE)
	{
		//wchar_t DebugText[255] = { 0 };
		//SecureZeroMemory(DebugText, sizeof(DebugText));
		//wsprintf(DebugText, L"create_unit_hook - variant type: %08X - ", *(int*)((char*)pCreationData + 0x24));

		//addDebugText(DebugText);

		*(int*)((char*)pObject + 0xC) = *(int*)((char*)pCreationData + 0x24);
	}

	return Memory::GetAddress<tcreate_unit_hook>(0x1F32DB, 0x1DE374)(pCreationData, a2, a3, pObject);
}


void H2MOD::leave_session()
{
	if (Memory::isDedicatedServer())
		return;

	if (GetEngineType() != e_engine_type::MainMenu)
	{
		// request_squad_browser
		WriteValue<BYTE>(Memory::GetAddress(0x978BAC), 1);

		typedef void(__cdecl load_main_menu_with_context)(int context);
		auto load_main_menu_with_context_impl = Memory::GetAddress<load_main_menu_with_context*>(0x08EAF);
		load_main_menu_with_context_impl(0);
	}

	typedef int(__cdecl leave_game_type)(int a1);
	auto leave_session = Memory::GetAddress<leave_game_type*>(0x216388);
	leave_session(0);
}

#pragma endregion

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

typedef signed int(__cdecl *wcsncpy_s_hook)(int a1, unsigned int a2, wchar_t* a3, int a4);
wcsncpy_s_hook p_wcsncpy_s_hook;

//lets you follow the call path of any string that is displayed (in a debugger)
signed int __cdecl stringDisplayHook(int a1, unsigned int a2, wchar_t* a3, int a4) {
	return p_wcsncpy_s_hook(a1, a2, a3, a4);
}

/* controller index aka local player index -> player index */
datum H2MOD::get_player_datum_index_from_controller_index(int controller_index) 
{
	typedef int(__cdecl* get_local_player_index)(int controller_index); 
	auto p_get_local_player_index = reinterpret_cast<get_local_player_index>(Memory::GetAddress(0x5141D));
	return p_get_local_player_index(controller_index); 
}

#pragma region PlayerFunctions

float H2MOD::get_distance(int playerIndex1, int playerIndex2) {
	real_point3d points_distance;
	real_point3d* player1 = nullptr;
	real_point3d* player2 = nullptr;

	player1 = h2mod->get_player_unit_coords(playerIndex1);
	player2 = h2mod->get_player_unit_coords(playerIndex2);
	
	points_distance.x = abs(player1->x - player2->x);
	points_distance.y = abs(player1->y - player2->y);
	points_distance.z = abs(player1->z - player2->z);

	return sqrt(pow(points_distance.x, 2) + pow(points_distance.y, 2) + pow(points_distance.z, 2));
}

real_point3d* H2MOD::get_player_unit_coords(int playerIndex) {
	BYTE* player_unit = get_player_unit_from_player_index(playerIndex);
	if (player_unit != nullptr)
		return reinterpret_cast<real_point3d*>(player_unit + 0x64);

	return nullptr;
}

BYTE* H2MOD::get_player_unit_from_player_index(int playerIndex) {
	datum unit_datum = Player::getPlayerUnitDatumIndex(playerIndex);
	if (unit_datum.IsNull())
		return nullptr;

	DatumIterator<s_object_header> objectsIt(get_objects_header());
	return (BYTE*)objectsIt.get_data_at_index(unit_datum.ToAbsoluteIndex())->object;
}

void call_give_player_weapon(int playerIndex, datum weaponId, bool bReset)
{
	//LOG_TRACE_GAME("GivePlayerWeapon(PlayerIndex: %08X, WeaponId: %08X)", PlayerIndex, WeaponId);

	datum unit_datum = Player::getPlayerUnitDatumIndex(playerIndex);
	if (unit_datum != NONE)
	{
		s_object_placement_data nObject;

		Engine::Objects::create_new_placement_data(&nObject, weaponId, unit_datum, 0);

		int object_index = Engine::Objects::call_object_new(&nObject);

		if (bReset == true)
			Engine::Unit::remove_equipment(unit_datum);

		Engine::Unit::assign_equipment_to_unit(unit_datum, object_index, 1);
	}
}

wchar_t* H2MOD::get_local_player_name(int local_player_index)
{
	return Player::getName(this->get_player_datum_index_from_controller_index(local_player_index).ToAbsoluteIndex());
}

int H2MOD::get_player_index_from_unit_datum_index(datum unit_datum_index)
{
	PlayerIterator playersIt;
	while (playersIt.get_next_active_player())
	{
		datum unit_datum_index_check = playersIt.get_current_player_data()->controlled_unit_index;
		LOG_TRACE_FUNC("Checking datum: {0:x} - index: {1} against datum: {2:x}", unit_datum_index_check.ToInt(), playersIt.get_current_player_index(), unit_datum_index.ToInt());

		if (unit_datum_index == unit_datum_index_check)
			return playersIt.get_current_player_index();
	}
	return NONE;
}

BYTE H2MOD::get_unit_team_index(datum unit_datum_index)
{
	BYTE team_index = NONE;
	char* unit_object = Engine::Objects::try_and_get_data_with_type(unit_datum_index, FLAG(e_object_type::biped));
	if (unit_object)
	{
		team_index = *(BYTE*)(unit_object + 0x13C);
	}
	return team_index;
}

void H2MOD::set_unit_speed_patch(bool hackit) {
	static BYTE oldBytes[8];
	static bool oldBytesRead = false;
	DWORD address = Memory::GetAddress(0x6AB7F, 0x6A3BA);

	if (oldBytesRead == false)
	{
		ReadBytesProtected(address, oldBytes, sizeof(oldBytes));
		oldBytesRead = true;
	}

	if (hackit)
	{
		NopFill(address, sizeof(oldBytes));
	}
	else
	{
		WriteBytes(address, oldBytes, sizeof(oldBytes));
	}
}

void H2MOD::set_player_unit_grenades_count(int playerIndex, e_grenades type, BYTE count, bool resetEquipment)
{
	if (type > e_grenades::Plasma)
	{
		LOG_TRACE_GAME("[H2MOD] set_player_unit_grenades_count() Invalid argument: type");
		return;
	}

	static std::string grenadeEquipamentTagName[2] =
	{
		"objects\\weapons\\grenade\\frag_grenade\\frag_grenade"
		"objects\\weapons\\grenade\\plasma_grenade\\plasma_grenade"
	};

	datum unit_datum_index = Player::getPlayerUnitDatumIndex(playerIndex);
	datum grenade_eqip_tag_datum_index = tags::find_tag(blam_tag::tag_group_type::equipment, grenadeEquipamentTagName[type]);

	char* unit_object = Engine::Objects::try_and_get_data_with_type(unit_datum_index, FLAG(e_object_type::biped));
	if (unit_object)
	{
		if (resetEquipment)
			Engine::Unit::remove_equipment(unit_datum_index);

		typedef bool(__cdecl* simulation_is_predicted)();
		auto p_simulation_is_predicted = Memory::GetAddress<simulation_is_predicted>(0x498B7, 0x42B54);

		// not sure what these flags are, but this is called when picking up grenades
		typedef void(__cdecl* entity_set_unk_flags)(datum objectIndex, int flags);
		auto p_entity_set_unk_flags = Memory::GetAddress<entity_set_unk_flags>(0x1B6685, 0x1B05B5);

		typedef void(__cdecl* unit_add_grenade_to_inventory_send)(datum unitDatumIndex, datum equipamentTagIndex);
		auto p_unit_add_grenade_to_inventory_send = Memory::GetAddress<unit_add_grenade_to_inventory_send>(0x1B6F12, 0x1B0E42);

		// send simulation update for grenades if we control the simulation
		if (!p_simulation_is_predicted())
		{
			// set grenade count
			*(BYTE*)(unit_object + 0x252 + type) = count;

			p_entity_set_unk_flags(unit_datum_index, FLAG(22)); // flag 22 seems to be sync entity grenade count (TODO: list all of the update types)
			p_unit_add_grenade_to_inventory_send(unit_datum_index, grenade_eqip_tag_datum_index);
		}

		LOG_TRACE_GAME("[H2Mod-GunGame] set_player_unit_grenades_count - sending grenade simulation update, playerIndex={0}, peerIndex={1}", playerIndex, NetworkSession::getPeerIndex(playerIndex));
	}

}

BYTE H2MOD::get_local_team_index()
{
	return *Memory::GetAddress<BYTE*>(0x51A6B4);
}
#pragma endregion

void H2MOD::disable_sounds(int sound_flags)
{
	static const std::string multiplayerGlobalsTag("multiplayer\\multiplayer_globals");
	if (sound_flags)
	{
		datum multiplayerGlobalsTagIndex = tags::find_tag(blam_tag::tag_group_type::multiplayerglobals, multiplayerGlobalsTag);

		if (!multiplayerGlobalsTagIndex.IsNull())
		{
			s_multiplayer_globals_group_definition* multiplayerGlobalsTag = tags::get_tag<blam_tag::tag_group_type::multiplayerglobals, s_multiplayer_globals_group_definition>(multiplayerGlobalsTagIndex);

			if (multiplayerGlobalsTag->runtime.size)
			{
				auto* runtime_tag_block_data = multiplayerGlobalsTag->runtime[0];

				if (sound_flags & FLAG(SoundType::Slayer))
				{
					runtime_tag_block_data->slayer_events.size = 0;
					runtime_tag_block_data->slayer_events.data = 0;
				}

				if (sound_flags & ALL_SOUNDS_NO_SLAYER) // check if there is any point in running the code bellow
				{
					for (int i = 0; i < runtime_tag_block_data->general_events.size; i++)
					{
						using sound_events = s_multiplayer_globals_group_definition::s_runtime_block::s_general_events_block::e_event;
						auto* general_event = runtime_tag_block_data->general_events[i];
						auto event = general_event->event;
						if (
							(sound_flags & FLAG(SoundType::GainedTheLead) && (event == sound_events::gained_lead || event == sound_events::gained_team_lead))
							|| (sound_flags & FLAG(SoundType::TeamChange) && event == sound_events::player_changed_team)
							|| (sound_flags & FLAG(SoundType::LostTheLead) && (event == sound_events::lost_lead))
							|| (sound_flags & FLAG(SoundType::TiedLeader) && (event == sound_events::tied_leader || event == sound_events::tied_team_leader))
							)
						{
							// disable all sounds from english to chinese
							for (int j = 0; j < 8; j++)
							{
								(&general_event->sound)[j].TagIndex = NONE;
							}
						}
					}
				}
			}
		}
	}
}

void H2MOD::custom_sound_play(const wchar_t* soundName, int delay)
{
	auto playSound = [=]()
	{
		//std::unique_lock<std::mutex> lck(h2mod->sound_mutex);
		std::chrono::high_resolution_clock::time_point timePoint = std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(delay);

		LOG_TRACE_GAME(L"[H2MOD-SoundQueue] - attempting to play sound {0} - delaying {1} miliseconds first", soundName, delay);

		if (delay > 0)
			std::this_thread::sleep_until(timePoint);

		PlaySound(soundName, NULL, SND_FILENAME | SND_NODEFAULT);
	};

	if (!Memory::isDedicatedServer())
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
		datum dead_player = datum(unit_datum_index);
		headHunterHandler->playerDeath->SetDeadPlayer(dead_player); // set this so we can spawn a skull on their position.
		headHunterHandler->playerDeath->execute();
	}

	if (b_FireFight)
	{
		/* Hack until rest of code is changed to support datum vs int*/
		datum ai_datum = datum(unit_datum_index);

		/*
			In firefight we want to track AI deaths and execute on them to grant points.
		*/
		fireFightHandler->playerDeath->SetPlayerIndex(*(datum*)(a2)); // this is the player datum of player who killed the datum.
		fireFightHandler->playerDeath->SetKilledDatum(ai_datum);
		fireFightHandler->playerDeath->execute();
	}

	if (b_GunGame) 
	{
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

// Client Sided Patch
void H2MOD::disable_weapon_pickup(bool b_Enable)
{
	static BYTE oldBytes[5];
	static BYTE oldBytesRead = false;
	DWORD address = Memory::GetAddress(0x55EFA);

	if (oldBytesRead == false)
	{
		ReadBytesProtected(address, oldBytes, sizeof(oldBytes));
		oldBytesRead = true;
	}

	if (b_Enable) 
	{
		WriteBytes(address, oldBytes, sizeof(oldBytes));
	}
	else
	{
		NopFill(address, sizeof(oldBytes));
	}
}

void H2MOD::set_local_rank(BYTE rank)
{
	if (Memory::isDedicatedServer())
		return;

	static bool initialized = false;

	if (!initialized)
	{
		NopFill(Memory::GetAddress(0x1b2c29), 7);
		initialized = true;
	}

	Player::Properties* local_player_properties = Memory::GetAddress<Player::Properties*>(0x51A638);

	local_player_properties->player_overall_skill = rank;
	local_player_properties->player_displayed_skill = rank;
}

int OnAutoPickUpHandler(datum player_datum, datum object_datum)
{
	int(_cdecl* AutoHandler)(datum, datum);
	AutoHandler = (int(_cdecl*)(datum, datum))Memory::GetAddress(0x57AA5, 0x5FF9D);

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

void get_object_table_memory()
{
	game_state_actors = *Memory::GetAddress<s_datum_array**>(0xA965DC, 0x9A1C5C);
}

typedef bool(__cdecl *map_cache_load)(s_game_engine_settings* map_load_settings);
map_cache_load p_map_cache_load;

bool __cdecl OnMapLoad(s_game_engine_settings* engine_settings)
{
	static bool resetAfterMatch = false;

	bool result = p_map_cache_load(engine_settings);
	if (result == false) // verify if the game didn't fail to load the map
		return false;
	// clear all the object variant data
	object_to_variant.clear();

	// set the engine type
	h2mod->SetCurrentEngineType(engine_settings->map_type);

	tags::run_callbacks();

	get_object_table_memory();

	H2Tweaks::setHz();
	// when the game is minimized, the game might skip loading Main menu
	// this is where resetAfterMatch var comes in for help
	if (resetAfterMatch)
	{
		//TODO: issue #232
		/*if (!NetworkSession::localPeerIsSessionHost()) {
			advLobbySettings->resetLobbySettings();
		}*/

		if (b_Infection) {
			infectionHandler->deinitializer->execute();
			b_Infection = false;
		}

		if (b_GunGame) {
			gunGame->deinitializer->execute();
			b_GunGame = false;
		}

		if (b_XboxTick) {
			engine_settings->tickrate = XboxTick::setTickRate(false);
			b_XboxTick = false;
		}

		if(b_HeadHunter)
		{
			headHunterHandler->deinitializer->execute();
			b_HeadHunter = false;
		}

		resetAfterMatch = false;
	}

	if (h2mod->GetEngineType() == e_engine_type::MainMenu)
	{
		addDebugText("Engine type: Main-Menu");
		UIRankPatch();
		H2Tweaks::toggleAiMp(false);
		H2Tweaks::toggleUncappedCampaignCinematics(false);
		MetaExtender::free_tag_blocks();
		return result;
	}


	wchar_t* variant_name = NetworkSession::getGameVariantName();
	LOG_INFO_GAME(L"[h2mod] OnMapLoad engine type {}, variant name {}", (int)h2mod->GetEngineType(), variant_name);

	for (auto gametype_it : GametypesMap)
		gametype_it.second = false; // reset custom gametypes state

	ControllerInput::SetSensitiviy(H2Config_controller_sens);
	MouseInput::SetSensitivity(H2Config_mouse_sens);
	HudElements::OnMapLoad();
	if (h2mod->GetEngineType() == e_engine_type::Multiplayer)
	{
		addDebugText("Engine type: Multiplayer");
		
		for (auto gametype_it : GametypesMap)
		{
			if (StrStrIW(variant_name, gametype_it.first)) {
				LOG_INFO_GAME(L"[h2mod] {} custom gametype turned on!", gametype_it.first);
				gametype_it.second = true; // enable a gametype if substring is found
			}
		}
		
		if (!b_XboxTick) 
		{
			H2X::Initialize(b_H2X);
			ProjectileFix::ApplyProjectileVelocity();
			engine_settings->tickrate = XboxTick::setTickRate(false);
		}
		else
		{
			engine_settings->tickrate = XboxTick::setTickRate(true);
		}

		H2Tweaks::toggleAiMp(true);
		H2Tweaks::toggleUncappedCampaignCinematics(false);
		EventHandler::executeMapLoadCallback(e_engine_type::Multiplayer);

		if (Engine::get_game_life_cycle() == life_cycle_in_game)
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

			if(b_HeadHunter)
			{
				headHunterHandler->initializer->execute();
			}
		}
	}

	else if (h2mod->GetEngineType() == e_engine_type::SinglePlayer)
	{
		//if anyone wants to run code on map load single player
		addDebugText("Engine type: Singleplayer");
		//H2X::Initialize(true);
		H2Tweaks::toggleUncappedCampaignCinematics(true);
		EventHandler::executeMapLoadCallback(e_engine_type::SinglePlayer);
	}

	// if we got this far, it means map is MP or SP, and if map load is called again, it should reset/deinitialize any custom gametypes
	resetAfterMatch = true;

	return result;
}

typedef bool(__cdecl *player_spawn)(datum playerDatumIndex);
player_spawn p_player_spawn;

bool __cdecl OnPlayerSpawn(datum playerDatumIndex)
{
	//I cant find somewhere to put this where it actually works (only needs to be done once on map load). It's only a few instructions so it shouldn't take long to execute.
	H2Tweaks::toggleKillVolumes(!AdvLobbySettings_disable_kill_volumes);

	//LOG_TRACE_GAME("OnPlayerSpawn(a1: %08X)", a1);

	if(b_HeadHunter)
	{
		headHunterHandler->preSpawnPlayer->SetPlayerIndex(playerDatumIndex.ToAbsoluteIndex());
		headHunterHandler->preSpawnPlayer->execute();
	}

	if (b_Infection) {
		infectionHandler->preSpawnPlayer->setPlayerIndex(playerDatumIndex.ToAbsoluteIndex());
		infectionHandler->preSpawnPlayer->execute();
	}

	if (b_GunGame) {
		gunGame->preSpawnPlayer->setPlayerIndex(playerDatumIndex.ToAbsoluteIndex());
		gunGame->preSpawnPlayer->execute();
	}

	bool ret = p_player_spawn(playerDatumIndex);

	if(b_HeadHunter)
	{
		headHunterHandler->spawnPlayer->SetPlayerIndex(playerDatumIndex.ToAbsoluteIndex());
		headHunterHandler->spawnPlayer->execute();
	}

	if (b_Infection) {
		infectionHandler->spawnPlayer->setPlayerIndex(playerDatumIndex.ToAbsoluteIndex());
		infectionHandler->spawnPlayer->execute();
	}

	if (b_GunGame) {
		gunGame->spawnPlayer->setPlayerIndex(playerDatumIndex.ToAbsoluteIndex());
		gunGame->spawnPlayer->execute();
	}

	return ret;
}

typedef int(__cdecl *object_p_hook)(int s_object_placement_data, int a2, signed int a3, int a4);
object_p_hook object_p_hook_method;

int __cdecl objectPHook(int s_object_placement_data, int object_definition_index, int object_owner, int unk) {
	if (h2mod->hookedObjectDefs.find(object_definition_index) == h2mod->hookedObjectDefs.end()) {
		//ingame only
		wchar_t* mapName = Memory::GetAddress<wchar_t*>(0x97737C);
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

void __cdecl changeTeam(int localPlayerIndex, int teamIndex) 
{
	network_session* session = NetworkSession::getCurrentNetworkSession();
	if ((session->parameters.session_mode == 4 && Engine::get_game_life_cycle() == life_cycle_pre_game)
		|| (StrStrIW(NetworkSession::getGameVariantName(), L"rvb") != NULL && teamIndex > 1)) {
		//rvb mode enabled, don't change teams
		return;
	}
	p_change_local_team(localPlayerIndex, teamIndex);
}

void H2MOD::set_local_team_index(int local_player_index, int team_index)
{
	// we only use player index 0 due to no splitscreen support but whatever
	typedef void(__cdecl update_player_profile)(int local_player_index);
	auto p_update_player_profile = Memory::GetAddress<update_player_profile*>(0x206A97);

	p_change_local_team(local_player_index, team_index);
	p_update_player_profile(local_player_index); // fixes infection handicap glitch
}

void H2MOD::set_local_team_match_xuid(XUID xuid)
{
	network_session* session = NetworkSession::getCurrentNetworkSession();
	if ((Engine::get_game_life_cycle() == life_cycle_pre_game))
		for(auto i = 0; i < 16; i++)
			if(session->membership.player_info[i].identifier == xuid)
			{
				changeTeam(0, session->membership.player_info[i].properties.player_team);
				break;
			}
}
void H2MOD::set_local_clan_tag(int local_player_index, XUID tag)
{
	typedef void(__cdecl update_player_profile)(int local_player_index);
	auto p_update_player_profile = Memory::GetAddress<update_player_profile*>(0x206A97);
	unsigned long low = tag & 0xFFFFFFFF;
	*(unsigned long*)Memory::GetAddress(0x51A6A8 + (0xB8 * local_player_index)) = low;
	p_update_player_profile(local_player_index);
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
}

typedef bool(__cdecl *tfn_c000bd114)(int);
tfn_c000bd114 pfn_c000bd114;
bool __cdecl fn_c000bd114_IsSkullEnabled(int skull_index)
{
	//bool result = pfn_c000bd114(skull_index);
	//return result;

	bool* var_c004d8320 = Memory::GetAddress<bool*>(0x4d8320);
	bool(*fn_c00049833_IsEngineModeCampaign)() = (bool(*)())Memory::GetAddress(0x49833);

	bool result = false;
	if (skull_index <= 14 && fn_c00049833_IsEngineModeCampaign())
		result = var_c004d8320[skull_index];
	if (skull_index == 10 && AdvLobbySettings_mp_sputnik)
		result = true;
	else if (skull_index == 1 && AdvLobbySettings_mp_grunt_bday_party)
		result = true;
	else if (skull_index == 6 && AdvLobbySettings_mp_blind)
		result = true;
	return result;
}

bool GrenadeChainReactIsEngineMPCheck() {
	if (AdvLobbySettings_grenade_chain_react)
		return false;
	return h2mod->GetEngineType() == e_engine_type::Multiplayer;
}

bool BansheeBombIsEngineMPCheck() {
	if (AdvLobbySettings_banshee_bomb)
		return false;
	return h2mod->GetEngineType() == e_engine_type::Multiplayer;
}

bool FlashlightIsEngineSPCheck() {
	if (AdvLobbySettings_flashlight)
		return true;
	return h2mod->GetEngineType() == e_engine_type::SinglePlayer;
}

#pragma region Game Version hooks
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
#pragma endregion

void GivePlayerWeaponDatum(datum unit_datum, datum weapon_datum)
{
	if (unit_datum != NONE)
	{
		s_object_placement_data nObject;

		Engine::Objects::create_new_placement_data(&nObject, weapon_datum, unit_datum, 0);

		int object_index = Engine::Objects::call_object_new(&nObject);
		if (object_index != NONE)
		{
			Engine::Unit::remove_equipment(unit_datum);
			Engine::Unit::assign_equipment_to_unit(unit_datum, object_index, 1);
		}
	}
}

//This is used for maps with 'shops' where the device_acceleration_scale is an indicator that they're using the control device as a 'shop'
float get_device_acceleration_scale(datum device_datum)
{
	DWORD tag_data = (DWORD)tags::get_tag_data();
	DWORD tag_instances = (DWORD)tags::get_tag_instances();

	int device_gamestate_offset = device_datum.Index + device_datum.Index * 2;
	DWORD device_gamestate_datum_pointer = *(DWORD*)((BYTE*)get_objects_header()->datum + device_gamestate_offset * 4 + 8);
	DWORD device_control_datum = *(DWORD*)((BYTE*)device_gamestate_datum_pointer);

	__int16 device_control_index = device_control_datum & 0xFFFF;
	device_control_index = device_control_index << 4;

	DWORD device_control_tag_offset = *(DWORD*)((BYTE*)device_control_index + tag_instances + 8);
	float acceleration_scale = *(float*)((BYTE*)device_control_tag_offset + tag_data + 0x14);

	return acceleration_scale;
}

typedef int(__cdecl *tdevice_touch)(datum device_datum, datum unit_datum);
tdevice_touch pdevice_touch;

bool device_active = true;
//This happens whenever a player activates a device control.
int __cdecl device_touch(datum device_datum, datum unit_datum)
{
	if (h2mod->GetEngineType() == e_engine_type::Multiplayer)
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



void H2MOD::team_player_indicator_visibility(bool toggle)
{
	this->drawTeamIndicators = toggle;
}

void __cdecl game_mode_engine_draw_team_indicators()
{
	typedef void(__cdecl* game_mode_engine_draw_team_indicators_def)();
	auto p_game_mode_engine_draw_team_indicators = Memory::GetAddress<game_mode_engine_draw_team_indicators_def>(0x6AFA4);

	if (h2mod->drawTeamIndicators)
		p_game_mode_engine_draw_team_indicators();
}

typedef short(__cdecl* get_enabled_teams_flags_def)(network_session*);
get_enabled_teams_flags_def p_get_enabled_teams_flags;

short __cdecl get_enabled_teams_flags(network_session* session)
{
	short default_teams_enabled_flags = p_get_enabled_teams_flags(session);
	short new_teams_enabled_flags = (default_teams_enabled_flags & H2Config_team_bit_flags);
	if (new_teams_enabled_flags && !b_Infection)
		return new_teams_enabled_flags;
	else
		return default_teams_enabled_flags;
}

typedef int(__cdecl* getnexthillindex)(int previousHill);
getnexthillindex p_get_next_hill_index;
signed int __cdecl get_next_hill_index(int previousHill)
{
	int hillCount = *Memory::GetAddress<int*>(0x4dd0a8, 0x5008e8);
	if (previousHill + 1 >= hillCount) 
	{
		//LOG_TRACE_GAME("[KoTH Behavior] Hill Count: {} Current Hill: {} Next Hill: {}", hillCount, previousHill, 0);
		return 0;
	}
	//LOG_TRACE_GAME("[KoTH Behavior] Hill Count: {} Current Hill: {} Next Hill: {}", hillCount, previousHill, previousHill + 1);
	return previousHill + 1;
}

void H2MOD::ApplyUnitHooks()
{
	// increase the size of the unit entity creation definition packet
	WriteValue<DWORD>(Memory::GetAddress(0x1F8028, 0x1E1D8E) + 1, 48);

	//This encodes the unit creation packet, only gets executed on host.
	pc_simulation_unit_entity_definition_encode = (tc_simulation_unit_entity_definition_creation_encode)DetourClassFunc(Memory::GetAddress<BYTE*>(0x1F8503, 0x1E2269), (BYTE*)c_simulation_unit_entity_definition_creation_encode, 10);

	//This decodes the unit creation packet, only gets executed on client.
	pc_simulation_unit_entity_definition_decode = (tc_simulation_unit_entity_definition_creation_decode)DetourClassFunc(Memory::GetAddress<BYTE*>(0x1F8557, 0x1E22BD), (BYTE*)c_simulation_unit_entity_definition_creation_decode, 11);

	pdevice_touch = (tdevice_touch)DetourFunc(Memory::GetAddress<BYTE*>(0x163420, 0x158EE3), (BYTE*)device_touch, 10);

	//Only patch the object_new call on host during AI_Place function, no reason to hook all object_new calls.
	PatchCall(Memory::GetAddress(0x318DEC, 0x2C3B56), object_new_hook);

	//We update creation data here which is used later on to add data to the packet
	PatchCall(Memory::GetAddress(0x1F807A, 0x1E1DE0), set_unit_creation_data_hook);

	// Hooks a call within the creat_unit property on the client side in order to set their permutation index before spawn.
	PatchCall(Memory::GetAddress(0x1F9E6C, 0x1E3BD4), create_unit_hook);

	// Hooks the part of the unit spawn from simulation that handles setting their color data in order to ensure AI do not have their color overridden
	PatchCall(Memory::GetAddress(0x1F9E34, 0x1E3B9C), set_unit_color_data_hook);
	pset_unit_color_data = Memory::GetAddress<tset_unit_color_data>(0x6E5C3, 0x6D1BF);
}


static BYTE previousGamestate = 0;
typedef int(__thiscall* ChangeGameState)(BYTE* this_);
ChangeGameState p_EvaulateGameState;
void EvaluateGameState()
{
	p_EvaulateGameState(Memory::GetAddress<BYTE*>(0x420FC4, 0x3C40AC));
	BYTE GameState = *Memory::GetAddress<BYTE*>(0x420FC4, 0x3C40AC);
	if (previousGamestate != GameState) {
		previousGamestate = GameState;
		EventHandler::executeGameStateCallbacks(GameState);
	}
}

typedef void(__cdecl p_set_screen_bounds)(signed int a1, signed int a2, __int16 a3, __int16 a4, __int16 a5, __int16 a6, float a7, float res_scale);
p_set_screen_bounds* c_set_screen_bounds;

void __cdecl set_screen_bounds(signed int a1, signed int a2, __int16 a3, __int16 a4, __int16 a5, __int16 a6, float a7, float res_scale)
{
	c_set_screen_bounds(a1, a2, a3, a4, a5, a6, a7, 1.5f);
}

bool __cdecl should_start_pregame_countdown_hook()
{
	// dedicated server only
	auto p_should_start_pregame_countdown_hook = Memory::GetAddressRelative<decltype(&should_start_pregame_countdown_hook)>(0x0, 0x40BC2A);

	// store what the game thinks about starting the countdown timer
	bool gameCheck = p_should_start_pregame_countdown_hook();

	if (!gameCheck)
		return false; // if the game already thinks the game timer doesn't need to start, return false and skip any processing

	bool teamsAreValidConditionMet = false;
	bool minimumPlayersConditionMet = false;

	//network_session* networkSession = NetworkSession::getCurrentNetworkSession();

	if (H2Config_minimum_player_start > 0)
	{
		if (NetworkSession::getPlayerCount() >= H2Config_minimum_player_start)
		{
			LOG_DEBUG_GAME(L"Minimum Player count met.");
			minimumPlayersConditionMet = true;
		}
		else
		{
			ServerConsole::SendMsg(L"Waiting for Players | Esperando a los jugadores", true);
		}
	}
	else
	{
		minimumPlayersConditionMet = true;
	}

	BYTE TeamPlay = *Memory::GetAddress<BYTE*>(0, 0x992880);
	if (H2Config_force_even && TeamPlay == 1 && minimumPlayersConditionMet)
	{
		ServerConsole::SendMsg(L"Balancing Teams | Equilibrar equipos", true);
		LOG_DEBUG_GAME(L"Balancing teams");
		std::map<std::string, std::vector<int>> Parties;
		std::vector<int> nonPartyPlayers;
		for (auto i = 0; i < 16; i++) //Detect party members
		{
			if (NetworkSession::playerIsActive(i))
			{
				//auto player = NetworkSession::getPlayerInformation(i);
				int calcBaseOffset = 0x530E34 + (i * 0x128);
				auto ClanDescripton = *Memory::GetAddress<unsigned long*>(0, calcBaseOffset + 0x88);
				auto Gamertag = Memory::GetAddress<wchar_t*>(0, calcBaseOffset + 0x18);
				//NetworkSession::getPlayerInformation(i).properties->ClanTag did not appear to have this data.
				auto partyCode = IntToString<unsigned long>(ClanDescripton, std::dec);
				LOG_DEBUG_GAME(L"Checking if {} is part of a party", Gamertag);
				if (ClanDescripton != 0)
				{
					LOG_DEBUG_GAME(L"Party {} adding member {}", std::wstring(partyCode.begin(), partyCode.end()), Gamertag);
					Parties[partyCode].push_back(NetworkSession::getPlayerIdByName(Gamertag));
				}
				else
					nonPartyPlayers.push_back(NetworkSession::getPlayerIdByName(Gamertag));
			}
		}
		for (auto i = 0; i < 16; i++) //Detect party leaders
		{
			if (NetworkSession::playerIsActive(i))
			{


				int calcBaseOffset = 0x530E34 + (i * 0x128);
				auto XUID = *Memory::GetAddress<::XUID*>(0, calcBaseOffset);
				auto Gamertag = Memory::GetAddress<wchar_t*>(0, calcBaseOffset + 0x18);
				auto xuidslug = IntToString<unsigned long>(XUID & 0xFFFFFFFF, std::dec);
				LOG_DEBUG_GAME(L"Checking if {} is leader of party {}", Gamertag, std::wstring(xuidslug.begin(), xuidslug.end()));
				if (Parties.count(xuidslug) == 1) //Party leader found
				{
					LOG_DEBUG_GAME(L"Party {} adding leader {}", std::wstring(xuidslug.begin(), xuidslug.end()), Gamertag);
					Parties[xuidslug].push_back(NetworkSession::getPlayerIdByName(Gamertag));
					std::vector<int>::iterator position = std::find(nonPartyPlayers.begin(), nonPartyPlayers.end(), NetworkSession::getPlayerIdByName(Gamertag));
					if (position != nonPartyPlayers.end())
						nonPartyPlayers.erase(position);
				}
			}
		}
		byte playersPerTeam = (*Memory::GetAddress<BYTE*>(0, 0x534858)) / H2Config_team_enabled_count;
		LOG_DEBUG_GAME(L"Players Per Team: {}", playersPerTeam);
		int currentTeam = 0;
		int currentTeamPlayers = 0;
		std::map<int, int> teamPlayers;
		for (auto i = 0; i < 8; i++) //Detect the first enabled team flag, this is so that if red isn't the first enabled team it doesn't place players there
			if (H2Config_team_flag_array[i]) {
				currentTeam = i;
				break;
			}
		LOG_DEBUG_GAME(L"Starting with team {}", IntToWString<int>(currentTeam, std::dec));
		std::vector<int> sortedPlayers;
		for (const auto& party : Parties)
		{
			LOG_DEBUG_GAME("Setting teams for party {}", party.first);
			for (const int &player : party.second)
			{
				LOG_DEBUG_GAME(L"Setting party player Team for {} to {}", IntToWString<int>(player, std::dec), IntToWString<int>(currentTeam, std::dec));
				sortedPlayers.push_back(player);
				CustomPackets::sendTeamChange(NetworkSession::getPeerIndex(player), currentTeam);
				teamPlayers[currentTeam] += 1;
				if (teamPlayers[currentTeam] == playersPerTeam)
					for (auto i = 0; i < 8; i++)
						if (H2Config_team_flag_array[i] && i != currentTeam) {
							if (teamPlayers[i] != playersPerTeam) {
								currentTeam = i;
								break;
							}
						}
			}
			for (auto i = 0; i < 8; i++)
				if (H2Config_team_flag_array[i] && i != currentTeam) {
					if (teamPlayers[i] != playersPerTeam) {
						currentTeam = i;
						break;
					}
				}
		}
		for (const auto &team : teamPlayers)
		{
			LOG_DEBUG_GAME(L"Team {} - {} Players", team.first, team.second);
		}
		for (const auto &player : nonPartyPlayers)
		{
			LOG_DEBUG_GAME(L"Setting Non party player Team for {} to {}", IntToWString<int>(player, std::dec), IntToWString<int>(currentTeam, std::dec));
			sortedPlayers.push_back(player);
			CustomPackets::sendTeamChange(NetworkSession::getPeerIndex(player), currentTeam);
			teamPlayers[currentTeam] += 1;
			if (teamPlayers[currentTeam] == playersPerTeam)
				for (auto i = 0; i < 8; i++)
					if (H2Config_team_flag_array[i] && i != currentTeam) {
						if (teamPlayers[i] != playersPerTeam) {
							currentTeam = i;
							break;
						}
					}
		}
		for (const auto &team : teamPlayers)
		{
			LOG_DEBUG_GAME(L"Team {} - {} Players", team.first, team.second);
		}
		int validTeams = 0;
		for (auto i = 0; i < 8; i++)
			if (H2Config_team_flag_array[i]) {
				if (teamPlayers[i] == playersPerTeam)
					validTeams++;
			}

		if (validTeams == H2Config_team_enabled_count)
			teamsAreValidConditionMet = true;
	}
	else
	{
		teamsAreValidConditionMet = true;
	}


	if (teamsAreValidConditionMet && minimumPlayersConditionMet)
		return true; // if we got this far, the game already thinks the countdown should start, therefore just return true
	else
		return false;
}

void H2MOD::RegisterEvents()
{

	if(!Memory::isDedicatedServer())//Client only callbacks	
	{
		

	}
	else //Server only callbacks
	{
		//Setup Events for H2Config_vip_lock
		if(H2Config_vip_lock)
		{
			EventHandler::registerGameStateCallback({
				"VIPLockClear",
				life_cycle_post_game,
				[]()
				{
					ServerConsole::ClearVip();
					*Memory::GetAddress<byte*>(0, 0x534850) = 0;
					//ServerConsole::SendCommand2(1, L"vip", L"clear");
					//ServerConsole::SendCommand2(1, L"Privacy", L"Open");
				}}, true);
			EventHandler::registerGameStateCallback({
				"VIPLockAdd",
				life_cycle_in_game,
				[]()
				{
					for (auto i = 0; i < NetworkSession::getPeerCount(); i++)
					{
						ServerConsole::AddVip(NetworkSession::getPeerPlayerName(i));
						//ServerConsole::SendCommand2(2, L"vip", L"add", NetworkSession::getPeerPlayerName(i));
					}
					//ServerConsole::SendCommand2(1, L"Privacy", L"VIP");
					*Memory::GetAddress<byte*>(0, 0x534850) = 2;
				}}, true);
		}
	}
	//Things that apply to both
	
}

//Shader LOD Bias stuff
//typedef int(__cdecl p_sub_81A676)(int a1, int a2, float a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10);
//p_sub_81A676* c_sub_81A676;
//
//int __cdecl sub_81A676(int a1, int a2, float a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10)
//{
//	return c_sub_81A676(a1, a2, a3, 4, a5, a6, a7, a8, a9, a10);
//}


void H2MOD::ApplyHooks() {
	/* Should store all offsets in a central location and swap the variables based on h2server/halo2.exe*/
	/* We also need added checks to see if someone is the host or not, if they're not they don't need any of this handling. */

	LOG_TRACE_GAME("Applying hooks...");

	/* Labeled "AutoPickup" handler may be proximity to vehicles and such as well */
	PatchCall(Memory::GetAddress(0x58789, 0x60C81), OnAutoPickUpHandler);

	//Hook to do stuff after Game State Change
	p_EvaulateGameState = Memory::GetAddress<ChangeGameState>(0x1d7738, 0x1BCDA8);
	PatchCall(Memory::GetAddress(0x1AD84D, 0x1A67CA), EvaluateGameState);

	// hook to initialize stuff before game start
	p_map_cache_load = (map_cache_load)DetourFunc(Memory::GetAddress<BYTE*>(0x8F62, 0x1F35C), (BYTE*)OnMapLoad, 11);

	//get next hill index hook
	if(!H2Config_koth_random)
		p_get_next_hill_index = (getnexthillindex)DetourFunc(Memory::GetAddress<BYTE*>(0x10DF1E, 0xDA4CE), (BYTE*)get_next_hill_index, 9);

	// player spawn hook
	p_player_spawn = (player_spawn)DetourFunc(Memory::GetAddress<BYTE*>(0x55952, 0x5DE4A), (BYTE*)OnPlayerSpawn, 6);

	// game version hook
	p_get_game_version = (get_game_version)DetourFunc(Memory::GetAddress<BYTE*>(0x1B4BF5, 0x1B0043), (BYTE*)GetGameVersion, 8);

	pplayer_death = (player_death)DetourFunc(Memory::GetAddress<BYTE*>(0x17B674, 0x152ED4), (BYTE*)OnPlayerDeath, 9);

	pupdate_player_score = (update_player_score)DetourClassFunc(Memory::GetAddress<BYTE*>(0xD03ED, 0x8C84C), (BYTE*)OnPlayerScore, 12);

	//on_custom_map_change_method = (on_custom_map_change)DetourFunc(Memory::GetAddress<BYTE*>(0x32176, 0x25738), (BYTE*)onCustomMapChange, 5);

	// disable part of custom map tag verification
	NopFill(Memory::GetAddress(0x4FA0A, 0x56C0A), 6);

	// disables profiles/game saves encryption
	PatchWinAPICall(Memory::GetAddress(0x9B08A, 0x85F5E), CryptProtectDataHook);
	PatchWinAPICall(Memory::GetAddress(0x9AF9E, 0x352538), CryptUnprotectDataHook);
	PatchCall(Memory::GetAddress(0x9B09F, 0x85F73), filo_write__encrypted_data_hook);

	ApplyUnitHooks();
	mapManager->applyHooks();

	ProjectileFix::ApplyPatches();

	//Guardian Patch
	c_object_cause_damage = Memory::GetAddress<p_object_cause_damage*>(0x17AD81, 0x1525E1);
	PatchCall(Memory::GetAddress(0x147DB8, 0x172D55), projectile_collision_object_cause_damage);

	// bellow hooks applied to specific executables
	if (Memory::isDedicatedServer() == false) {

		LOG_TRACE_GAME("Applying client hooks...");
		/* These hooks are only built for the client, don't enable them on the server! */


		//Shader LOD Bias stuff
		//c_sub_81A676 = Memory::GetAddress<p_sub_81A676*>(0x19A676);
		//PatchCall(Memory::GetAddress(0x19AD71), sub_81A676);
		//PatchCall(Memory::GetAddress(0x19ADBC), sub_81A676);

		p_verify_game_version_on_join = (verify_game_version_on_join)DetourFunc(Memory::GetAddress<BYTE*>(0x1B4C14), (BYTE*)VerifyGameVersionOnJoin, 5);

		p_verify_executable_version = (verify_executable_version)DetourFunc(Memory::GetAddress<BYTE*>(0x1B4C32), (BYTE*)VerifyExecutableVersion, 8);

		//pload_wgit = (tload_wgit)DetourClassFunc(Memory::GetAddress<BYTE*>(0x2106A2), (BYTE*)OnWgitLoad, 13);

		//intercept_map_load_method = (intercept_map_load)DetourClassFunc(Memory::GetAddress<BYTE*>(0xC259B), (BYTE*)interceptMapLoad, 13);

		show_error_screen_method = (show_error_screen)DetourFunc(Memory::GetAddress<BYTE*>(0x20E15A), (BYTE*)showErrorScreen, 8);

		//TODO: turn on if you want to debug halo2.exe from start of process
		is_debugger_present_method = (is_debugger_present)DetourFunc(Memory::GetAddress<BYTE*>(0x39B394), (BYTE*)isDebuggerPresent, 5);

		//TODO: use for object spawn hooking
		//0x132163
		//object_p_hook_method = (object_p_hook)DetourFunc(Memory::GetAddress<BYTE*>(0x132163), (BYTE*)objectPHook, 6);

		//TODO: expensive, use for debugging/searching
		//string_display_hook_method = (string_display_hook)DetourFunc(Memory::GetAddress<BYTE*>(0x287AB5), (BYTE*)stringDisplayHook, 5);

		//pResetRound=(ResetRounds)DetourFunc(Memory::GetAddress<BYTE*>(0x6B1C8), (BYTE*)OnNextRound, 7);

		/*
		WritePointer(Memory::GetAddress<BYTE*>(0x1F0B3A), player_add_packet_handler);
		WritePointer(Memory::GetAddress<BYTE*>(0x1F0B80), player_remove_packet_handler);
		*/

		p_change_local_team = (change_team)DetourFunc(Memory::GetAddress<BYTE*>(0x2068F2), (BYTE*)changeTeam, 8);

		// hook the print command to redirect the output to our console
		PatchCall(Memory::GetAddress(0xE9E50), print_to_console);

		calculate_model_lod = Memory::GetAddress(0x19CA3E);
		calculate_model_lod_detour_end = Memory::GetAddress(0x19CDA3 + 5);
		WriteJmpTo(Memory::GetAddress(0x19CDA3), calculate_model_lod_detour);

		// set max model quality to L6
		WriteValue(Memory::GetAddress(0x190B38 + 1), 5);

		pfn_c000bd114 = (tfn_c000bd114)DetourFunc(Memory::GetAddress<BYTE*>(0xbd114), (BYTE*)fn_c000bd114_IsSkullEnabled, 5);

		PatchCall(Memory::GetAddress(0x182d6d), GrenadeChainReactIsEngineMPCheck);
		PatchCall(Memory::GetAddress(0x92C05), BansheeBombIsEngineMPCheck);
		PatchCall(Memory::GetAddress(0x13ff75), FlashlightIsEngineSPCheck);

		PatchCall(Memory::GetAddress(0x226702), game_mode_engine_draw_team_indicators);

		//Initialise_tag_loader();
		PlayerControl::ApplyHooks();
		c_set_screen_bounds = Memory::GetAddress<p_set_screen_bounds*>(0x264979);
		//PatchCall(GetAddress(0x25E1E5), set_screen_bounds);
		
	}
	else {

		LOG_TRACE_GAME("Applying dedicated server hooks...");
		PatchCall(Memory::GetAddressRelative(0, 0x40BF43), should_start_pregame_countdown_hook);
		ServerConsole::ApplyHooks();

		p_get_enabled_teams_flags = (get_enabled_teams_flags_def)DetourFunc(Memory::GetAddress<BYTE*>(0, 0x19698B), (BYTE*)get_enabled_teams_flags, 6);
	}
}

VOID CALLBACK UpdateDiscordStateTimer(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	update_player_count();
}

void H2MOD::Initialize()
{
	stats_handler = new StatsHandler();
	if (!Memory::isDedicatedServer())
	{
		MouseInput::Initialize();
		KeyboardInput::Initialize();
		ControllerInput::Initialize();
		
		Initialise_tag_loader();
		RenderHooks::Initialize();
		DirectorHooks::Initialize();
		SpecialEvents::Initialize();
		//ObserverMode::Initialize();
		if (H2Config_discord_enable && H2GetInstanceId() == 1) {
			// Discord init
			DiscordInterface::SetDetails("Startup");
			DiscordInterface::Init();
			SetTimer(NULL, 0, 5000, UpdateDiscordStateTimer);
		}
		
		
	}
	MeleeFix::Initialize();
	TagFixes::Initalize();
	MapSlots::Initialize();
	HaloScript::Initialize();
	LOG_TRACE_GAME("H2MOD - Initialized {}", DLL_VERSION_STR);
	LOG_TRACE_GAME("H2MOD - Image base address: 0x{:X}", Memory::baseAddress);
	//WriteValue(GetAddress(0xC25EA + 8), 100);
	h2mod->ApplyHooks();
	h2mod->RegisterEvents();
}

void H2MOD::Deinitialize() {

}
