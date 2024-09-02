#pragma once

#include "input/controllers.h"
#include "memory/data.h"
#include "objects/damage_reporting.h"
#include "objects/emblems.h"
#include "objects/objects.h"
#include "simulation/machine_id.h"
#include "units/units.h"

#define k_maximum_players 16
#define k_player_index_bit_count 4	// 4 because 4 bits can store 16 players in k_maximum_players

enum e_player_color : int8
{
	_player_color_white,
	_player_color_steel,
	_player_color_red,
	_player_color_orange,
	_player_color_gold,
	_player_color_olive,
	_player_color_green,
	_player_color_sage,
	_player_color_cyan,
	_player_color_teal,
	_player_color_colbat,
	_player_color_blue,
	_player_color_violet,
	_player_color_purple,
	_player_color_pink,
	_player_color_crimson,
	_player_color_brown,
	_player_color_tan,
	k_player_color_count
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
	_handicap_severe = 3,

	k_handicap_count
};

enum e_player_flags : int16
{
	_player_active_in_game_bit = 0, // not entirely sure about this one, but the code uses the bit below
	_player_left_game_bit = 1,
	_player_first_spawn_bit = 3,
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
ASSERT_STRUCT_SIZE(s_player_profile, 8);

struct s_player_profile_traits
{
	s_player_profile profile;
	int32 gap_48;
	int32 gap_4C;
};
ASSERT_STRUCT_SIZE(s_player_profile_traits, 16);

// practically the xuid, but aligned to 4 (hence the split in 2 32 bit variables)
struct s_player_identifier
{
	uint32 id_low;
	uint32 id_high;
};
ASSERT_STRUCT_SIZE(s_player_identifier, 8);

struct s_clan_identifiers
{
	uint32 ID_1;
	uint32 ID_2;
	uint32 ID_3;
};
ASSERT_STRUCT_SIZE(s_clan_identifiers, 12);

struct s_player_properties
{
	wchar_t player_name[16];
	int32 spawn_protection_time;
	int8 gap_24[28];

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
ASSERT_STRUCT_SIZE(s_player_properties, 132);

#pragma pack(push, 1)
struct s_player
{
	uint16 datum_salt;
	uint16 flags;
	uint64 identifier;
	int32 player_creation_tick;
	s_machine_identifier machine_identifier;
	int16 machine_index;
	int32 machine_user_index;
	int32 machine_controller_index;
	e_controller_index controller_index;
	int16 user_index;
	int16 player_bsp_location_index;
	datum unit_index;
	datum dead_unit_index;
	datum possibly_datum;
	int32 InputFlags;
	int32 InputFlags2;
	int8 field_3C[4];
	s_player_properties properties[2];
	int32 field_148;
	int32 respawn_penalty;
	int32 teleporter_blocked_tick_count;
	int32 respawn_time;
	int32 unk_12;
	int8 gap_15C[4];
	int32 field_160;
	int16 field_164;
	int8 gap_166[14];
	int32 betrayal_encountered_tick;
	int32 spawn_protection_time;
	int16 field_17C[2];
	real32 unit_speed;
	int32 field_184;
	int8 gap_188[2];
	int16 field_18A;
	int8 gap_18C[2];
	int16 field_18E;
	int16 player_lives_count;
	int8 gap_192[2];
	int32 betraying_player_index;
	int8 gap_198[2];
	int32 field_19C;
	int8 gap_19E[30];
	int32 field_1BC;
	int8 gap_1C0[60];
	int16 random_index;
	int8 gap_1FE[2];
	int32 is_chatting;

	/*
	- TO NOTE: 
	- These functions work only after game has started, if you need to do something in the pregame lobby, use the functions available in Network Session (Blam/Engine/Networking/Session)
	*/

	static s_data_array* get_data();
	static bool is_index_valid(datum player_index);
	static s_player* get(datum player_index);
	static e_game_team get_team(datum player_index);
	static s_player* get_from_unit_index(datum unit_index);
	static void set_team(datum player_index, e_game_team team);
	static void set_unit_character_type(datum player_index, e_character_type character_type);
	static void set_player_unit_grenade_count(datum player_index, e_unit_grenade_type type, int8 count, bool reset_equipment);
	static void set_unit_speed(datum player_index, float speed);
	static const wchar_t* get_name(datum player_index);
	static datum get_unit_index(datum player_index);
	static unit_datum* get_player_unit_data(datum player_index);
	static real_point3d* get_unit_coords(datum player_index);
	static uint64 get_id(datum player_index);
};
ASSERT_STRUCT_SIZE(s_player, 516);
#pragma pack(pop)

class player_iterator 
{
public:
	player_iterator();

	bool get_next_active_player();

	s_player* get_current_player_data() const;
	int32 get_current_player_index() const;
	datum get_current_player_datum_index() const;
	wchar_t* get_current_player_name() const;
	uint64 get_current_player_id() const;

private:
	s_player* m_current_player;
	s_data_iterator<s_player> m_data_iterator;
};

struct s_persistent_weapon_data
{
	e_damage_reporting_type damage_reporting_type_0;
	int8 gap_1;
	e_damage_reporting_type damage_reporting_type_1;
	int8 gap_3;
	e_damage_reporting_type damage_reporting_type_2;
	int8 gap_5;
	int16 field_6;
};

struct s_persistent_campaign_player
{
	bool initialized;
	int8 pad;
	s_persistent_weapon_data weapon_0;
	s_persistent_weapon_data weapon_1;
	s_persistent_weapon_data weapon_2;
	uint16 grenade_counts_mask;
};
ASSERT_STRUCT_SIZE(s_persistent_campaign_player, 28);

struct s_players_globals
{
	int32 players_in_game_count;
	bool all_players_dead;
	bool any_players_dead;
	bool input_disabled;
	bool disable_movement;
	int16 player_user_count;
	int16 player_controller_count;
	datum player_user_mapping[4];
	datum player_controller_mapping[4];
	int32 machine_valid_mask;
	s_machine_identifier machine_identifier[17];
	bool local_machine_exists;
	s_machine_identifier local_machine_identifier;
	int8 gap_A5[3];
	int32 local_machine_index;
	int16 coop_respawn_hud_message_type;
	bool display_coop_respawn_message;
	int8 display_fail_respawn_message;
	int32 respawn_time;
	int16 bsp_switch_trigger_volume_index;
	int16 unk_AE;
	int32 player_datum_that_triggered_bsp_switch;
	int32 teleported_unit_datum;
	int8 gap_B8[128];
};
ASSERT_STRUCT_SIZE(s_players_globals, 312);

s_players_globals* get_players_globals();

datum __cdecl player_index_from_user_index(int32 user_index);

int8 __cdecl player_unk_84_from_user_index(int32 user_index);

bool __cdecl players_user_is_active(int32 user_index);

datum __cdecl player_index_from_absolute_player_index(uint16 abs_player_index);

void __cdecl players_set_machines(uint32 new_machine_valid_mask, const s_machine_identifier* new_machine_identifiers);

// gets required bits for player_appearance
uint32 player_appearance_required_bits(void);

// Validate player configuration
void __cdecl player_validate_configuration(datum player_index, s_player_properties* configuration_data);

void players_apply_patches(void);
