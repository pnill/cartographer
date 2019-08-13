#pragma once

#include "Blam\Enums\Enums.h"
#include "Blam\Enums\Game\GameEngine.h"

using namespace Blam::Enums;
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

int __cdecl call_get_object(signed int object_datum_index, int object_type);
int __cdecl call_unit_reset_equipment(int unit_datum_index);
bool __cdecl call_add_object_to_sync(int gamestate_object_datum);
void __cdecl call_hs_object_destroy(int object_datum_index);
signed int __cdecl call_unit_inventory_next_weapon(unsigned short unit_datum_index);
bool __cdecl call_assign_equipment_to_unit(int uint, int object_index, short unk);
int __cdecl call_object_placement_data_new(void*, int, int, int);
signed int __cdecl call_object_new(void*);
void GivePlayerWeapon(int PlayerIndex, int WeaponId, bool bReset);
void GivePlayerWeapon2(int PlayerIndex, int WeaponId, short Unk);

class H2MOD
{
public:
		void Initialize();
		void Deinitialize();
		void TeamPlayerIndicatorVisibility(bool toggle);
		int get_unit_from_player_index(int);
		int get_unit_datum_from_player_index(int);
		void ApplyHooks(); 
		void handle_command(std::string);
		void handle_command(std::wstring);
		void logToDedicatedServerConsole(const wchar_t* message, ...);
		wchar_t* get_local_player_name();		
		int get_player_unit_from_player_index(int playerIndex);
		float get_player_x(int);
		float get_player_y(int);
		float get_player_z(int);
		float get_distance(int, int);
		bool is_team_play();
		wchar_t* get_player_name_from_player_index(int playerIndex);
		int get_player_index_from_name(wchar_t* playername);
		int get_player_index_from_unit_datum(int unit_datum_index);
		BYTE get_unit_team_index(int unit_datum_index);
		void set_unit_team_index(int unit_datum_index, BYTE team);
		void set_unit_biped(Player::Biped biped, int playerIndex);
		void set_unit_speed_patch(bool hackit);
		void set_unit_speed(float speed, int playerIndex);
		void set_local_team_index(int local_player_index, int team);
		BYTE get_local_team_index();
		void set_local_grenades(BYTE type, BYTE count, int playerIndex);
		void DisableSound(int sound);
		void CustomSoundPlay(const wchar_t* soundName, int delay);
		void DisableWeaponPickup(bool b_Enable);
		void ApplyUnitHooks();
		EngineType GetEngineType();
		wchar_t* GetLobbyGameVariantName();
		void exit_game();
		BOOL Server;
		std::unordered_map<std::string, bool> AchievementMap;
		std::deque<std::wstring> CustomSounds;
		
		std::mutex sound_mutex;

		std::set<int> hookedObjectDefs;
		bool isChatBoxCommand = false;

		void SetBase(DWORD base) { Base = base; }

		DWORD GetBase() { return Base; }
		DWORD GetAddress(unsigned long client, DWORD server = 0)
		{
			return Base + (Server ? server : client);
		}

		template <typename T = void*>
		inline T GetPointer(DWORD client, DWORD server = 0)
		{
			return reinterpret_cast<T>(GetAddress(client, server));
		}

private:
		DWORD Base;
		std::unordered_map<int, int> playerIndexToDynamicBase;

};

extern H2MOD* h2mod;

