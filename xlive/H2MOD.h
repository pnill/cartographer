#pragma once

#include "Blam\Engine\Objects\Objects.h"
#include "Blam\Engine\Objects\ObjectPlacementData.h"

#include "Blam\Engine\Game\GameEngine.h"
#include "Blam\Engine\Players\Players.h"
#include "Blam\Common\Common.h"

enum SoundType
{
	TeamChange,
	GainedTheLead,
	LostTheLead,
	TiedLeader,
	Slayer
};
#define ALL_SOUNDS_NO_SLAYER (FLAG(SoundType::TeamChange) | FLAG(SoundType::GainedTheLead) | FLAG(SoundType::LostTheLead) | FLAG(SoundType::TiedLeader))

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

game_life_cycle get_game_life_cycle();
int __cdecl call_get_game_tick_rate();
bool __cdecl call_is_game_minimized();
char* __cdecl call_object_try_and_get_data_with_type(datum object_datum_index, int object_type);
int __cdecl call_unit_reset_equipment(datum unit_datum_index);
bool __cdecl call_add_object_to_sync(datum gamestate_object_datum);
void __cdecl call_hs_object_destroy(datum object_datum_index);
signed int __cdecl call_unit_inventory_next_weapon(unsigned short unit_datum_index);
bool __cdecl call_assign_equipment_to_unit(datum uint, int object_index, short unk);
void __cdecl call_object_placement_data_new(ObjectPlacementData*, datum, datum, int);
signed int __cdecl call_object_new(ObjectPlacementData*);
void call_give_player_weapon(int PlayerIndex, datum WeaponId, bool bReset);

class H2MOD
{
public:
		void Initialize();
		void Deinitialize();
		void ApplyHooks(); 
		void ApplyUnitHooks();

		void team_player_indicator_visibility(bool toggle);
		BYTE* get_player_unit_from_player_index(int playerIndex);
		datum get_player_datum_index_from_controller_index(int controller_index);
		wchar_t* get_local_player_name(int local_player_index);
		real_point3d* get_player_unit_coords(int player_index);
		float get_distance(int, int);
		int get_player_index_from_unit_datum_index(datum unit_datum_index);
		BYTE get_unit_team_index(datum unit_datum_index);
		void set_unit_speed_patch(bool hackit);
		void set_local_team_index(int local_player_index, int team);
		BYTE get_local_team_index();
		void set_player_unit_grenades_count(int playerIndex, Grenades type, BYTE count, bool resetEquipment);
		void disable_sounds(int sound);
		void custom_sound_play(const wchar_t* soundName, int delay);
		void disable_weapon_pickup(bool b_Enable);
		void leave_session();

		scnr_type GetMapType() { return mapType; }
		void SetMapType(scnr_type value) { mapType = value; }

		bool Server;
		std::unordered_map<std::string, bool> AchievementMap;
		std::deque<std::wstring> CustomSounds;
		
		std::mutex sound_mutex;

		std::set<int> hookedObjectDefs;
		bool isChatBoxCommand = false;
		bool drawTeamIndicators = true;

		void SetBase(DWORD base) { Base = base; }

		DWORD GetBase() { return Base; }

		DWORD GetAddress(DWORD client, DWORD server = 0)
		{
			return Base + (Server ? server : client);
		}

		template <typename T = void*>
		T GetAddress(DWORD client, DWORD server = 0)
		{
			return reinterpret_cast<T>(Base + (Server ? server : client));
		}

private:
		DWORD Base;
		scnr_type mapType;
};

extern H2MOD* h2mod;

