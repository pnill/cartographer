#pragma once
#include "Blam/Engine/memory/data.h"
#include "Blam/Engine/objects/damage_reporting.h"
#include "Blam/Engine/objects/emblems.h"
#include "Blam/Engine/objects/objects.h"
#include "Blam/Engine/Simulation/MachineID.h"

#define k_maximum_players 16

enum e_player_color : byte
{
	player_color_white,
	player_color_steel,
	player_color_red,
	player_color_orange,
	player_color_gold,
	player_color_olive,
	player_color_green,
	player_color_sage,
	player_color_cyan,
	player_color_teal,
	player_color_colbat,
	player_color_blue,
	player_color_violet,
	player_color_purple,
	player_color_pink,
	player_color_crimson,
	player_color_brown,
	player_color_tan
};

enum e_character_type : int8
{
	_character_type_masterchief = 0,
	_character_type_dervish = 1,
	_character_type_spartan = 2,
	_character_type_elite = 3,

	// cartographer added characters
	_character_type_skeleton = 4,
	_character_type_flood = 5,
	_character_type_lmao = 6,
	k_player_character_type_count
};

enum e_handicap : int8
{
	_handicap_none = 0,
	_handicap_minor = 1,
	_handicap_moderate = 2,
	_handicap_severe = 3
};

enum e_player_flags : int
{
	// player_flag_player_active = 0, // not entirely sure about this one, but the code uses the flag bellow
	_player_left_game_bit = 1,
	player_flag_player_first_spawn = 3,
};

struct s_player_profile
{
	e_player_color primary_color;
	e_player_color secondary_color;
	e_player_color tertiary_color;
	e_player_color quaternary_color;
	e_character_type player_character_type;
	s_emblem_info emblem_info;
};
CHECK_STRUCT_SIZE(s_player_profile, 8);

struct s_player_profile_traits
{
	s_player_profile profile;
	int unk_48;
	int unk_4C;
};
CHECK_STRUCT_SIZE(s_player_profile_traits, 16);

struct s_clan_identifiers
{
	DWORD ID_1;
	DWORD ID_2;
	DWORD ID_3;
};
CHECK_STRUCT_SIZE(s_clan_identifiers, 12);

struct s_player_properties
{
	wchar_t player_name[16];
	int32 spawn_protection_time;
	char unk[28];

	s_player_profile_traits profile_traits;
	wchar_t clan_name[16];
	s_clan_identifiers clan_identifiers;

	int8 team_index;
	e_handicap player_handicap_level;
	int8 player_displayed_skill;
	int8 player_overall_skill;
	int8 player_is_griefer;
	int8 bungie_user_role;
	int8 achievement_flags;
	int8 unk2;
};
CHECK_STRUCT_SIZE(s_player_properties, 132);

#pragma pack(push, 1)
struct s_player
{
	WORD datum_salt;
	WORD flags;
	unsigned long long identifier;
	DWORD player_creation_tick;
	s_machine_identifier machine_identifier; // also known as abEnet
	__int16 machine_index;
	int machine_user_index;
	int machine_controller_index;
	int controller_index;
	__int16 user_index;
	__int16 player_bsp_location_index;
	datum unit_index;
	datum dead_unit_index;
	datum possibly_datum;
	DWORD InputFlags;
	DWORD InputFlags2;
	char field_3C[4];
	s_player_properties properties[2];
	DWORD field_148;
	DWORD respawn_penalty;
	DWORD teleporter_blocked_tick_count;
	int respawn_time;
	int unk_12;
	BYTE gap_15C[4];
	DWORD field_160;
	WORD field_164;
	BYTE gap_166[14];
	DWORD betrayal_encountered_tick;
	int spawn_protection_time;
	WORD field_17C[2];
	float unit_speed;
	DWORD field_184;
	BYTE gap_188[2];
	WORD field_18A;
	BYTE gap_18C[2];
	WORD field_18E;
	WORD player_lives_count;
	BYTE gap_192[2];
	DWORD betraying_player_index;
	BYTE gap198[2];
	int field_19C;
	BYTE gap_19E[30];
	DWORD field_1BC;
	BYTE gap_1C0[60];
	WORD random_index;
	BYTE gap_1FE[2];
	DWORD is_chatting;

	/*
	- TO NOTE: 
	- These functions work only after game has started, if you need to do something in the pregame lobby, use the functions available in Network Session (Blam/Engine/Networking/Session)
	*/

	static s_data_array* GetArray();
	static bool IndexValid(int playerIndex);
	static s_player* GetPlayer(int playerIndex);
	static e_game_team GetTeam(int playerIndex);
	static void SetTeam(int playerIndex, e_game_team team);
	static void SetUnitBipedType(int playerIndex, e_character_type bipedType);
	static void SetBipedSpeed(int playerIndex, float speed);
	static const wchar_t* GetName(int playerIndex);
	static datum GetPlayerUnitDatumIndex(int playerIndex);
	static unsigned long long GetId(int playerIndex);
};
CHECK_STRUCT_SIZE(s_player, 516);
#pragma pack(pop)

class PlayerIterator : private s_data_iterator<s_player>
{
public:

	PlayerIterator();

	bool get_next_active_player();

	s_player* get_current_player_data();

	int get_current_player_index();

	wchar_t* get_current_player_name();

	unsigned long long get_current_player_id();

private:
	s_player* m_current_player = nullptr;
};

struct s_persistent_weapon_data
{
	e_damage_reporting_type damage_reporting_type_0;
	int8 pad0;
	e_damage_reporting_type damage_reporting_type_1;
	int8 pad1;
	e_damage_reporting_type damage_reporting_type_2;
	int8 pad2;
	int16 field_6;
};

struct s_persistent_campaign_player
{
	bool initialized;
	s_persistent_weapon_data weapon_0;
	s_persistent_weapon_data weapon_1;
	s_persistent_weapon_data weapon_2;
	WORD grenade_counts_mask;
};
CHECK_STRUCT_SIZE(s_persistent_campaign_player, 28);

struct s_players_globals
{
	int players_in_game_count;
	bool all_players_dead;
	bool any_players_dead;
	bool input_disabled;
	bool disable_movement;
	short player_user_count;
	short player_controller_count;
	datum player_user_mapping[4];
	datum player_controller_mapping[4];
	int machine_valid_mask;
	s_machine_identifier machine_identifier[17];
	bool local_machine_exists;
	s_machine_identifier local_machine_identifier;
	byte unk_A5[3];
	int local_machine_index;
	short coop_respawn_hud_message_type;
	bool display_coop_respawn_message;
	byte display_fail_respawn_message;
	int respawn_time;
	short bsp_switch_trigger_volume_index;
	short unk_AE;
	int player_datum_that_triggered_bsp_switch;
	int teleported_unit_datum;
	byte end_padding[128];
};
CHECK_STRUCT_SIZE(s_players_globals, 312);

s_players_globals* get_players_globals();

datum __cdecl player_index_from_user_index(int32 user_index);
bool __cdecl players_user_is_active(int32 user_index);

// gets required bits for player_appearance
uint32 player_appearance_required_bits(void);

// Validate player configuration
void __cdecl player_validate_configuration(datum player_index, s_player_properties* configuration_data);

void players_apply_patches(void);
