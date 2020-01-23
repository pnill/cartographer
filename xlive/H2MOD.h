#pragma once

#include "Blam\Enums\Game\GameEngine.h"
#include "Blam\Engine\EngineDefinitions.h"

using namespace Blam::Enums;
using namespace Blam::Maths;
using namespace Blam::Cache::DataTypes;
using namespace Blam::EngineDefinitions::Objects;
constexpr signed int NONE = -1;

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

int __cdecl call_get_object(DatumIndex object_datum_index, int object_type);
int __cdecl call_unit_reset_equipment(DatumIndex unit_datum_index);
bool __cdecl call_add_object_to_sync(DatumIndex gamestate_object_datum);
void __cdecl call_hs_object_destroy(DatumIndex object_datum_index);
signed int __cdecl call_unit_inventory_next_weapon(unsigned short unit_datum_index);
bool __cdecl call_assign_equipment_to_unit(DatumIndex uint, int object_index, short unk);
void __cdecl call_object_placement_data_new(ObjectPlacementData*, DatumIndex, DatumIndex, int);
signed int __cdecl call_object_new(ObjectPlacementData*);
void call_give_player_weapon(int PlayerIndex, DatumIndex WeaponId, bool bReset);

class H2MOD
{
public:
		void Initialize();
		void Deinitialize();
		void TeamPlayerIndicatorVisibility(bool toggle);
		int get_unit_index_from_player_index(int);
		DatumIndex get_unit_datum_from_player_index(int);
		BYTE* get_player_unit_from_player_index(int playerIndex);
		void ApplyHooks(); 
		void handle_command(std::string);
		void handle_command(std::wstring);
		DatumIndex get_player_datum_index_from_controller_index(int controller_index);
		wchar_t* get_local_player_name();
		Real::Point3D* get_player_coords(int playerIndex);
		float get_distance(int, int);
		wchar_t* GetLobbyGameVariantName();
		wchar_t* get_player_name_from_player_index(int playerIndex);
		int get_player_index_from_name(wchar_t* playername);
		int get_player_index_from_unit_datum(DatumIndex unit_datum_index);
		BYTE get_unit_team_index(DatumIndex unit_datum_index);
		void set_unit_team_index(int unit_datum_index, BYTE team);
		void set_unit_biped(Player::Biped biped_type, int playerIndex);
		void set_unit_speed_patch(bool hackit);
		void set_unit_speed(float speed, int playerIndex);
		void set_local_team_index(int local_player_index, int team);
		BYTE get_local_team_index();
		void set_local_grenades(BYTE type, BYTE count, int playerIndex);
		void DisableSound(int sound);
		void CustomSoundPlay(const wchar_t* soundName, int delay);
		void DisableWeaponPickup(bool b_Enable);
		void ApplyUnitHooks();

		MapType GetMapType() { return mapType; }
		void SetMapType(MapType value) { mapType = value; }

		void exit_game();
		bool Server;
		std::unordered_map<std::string, bool> AchievementMap;
		std::deque<std::wstring> CustomSounds;
		
		std::mutex sound_mutex;

		std::set<int> hookedObjectDefs;
		bool isChatBoxCommand = false;

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
		MapType mapType;
};

extern H2MOD* h2mod;

