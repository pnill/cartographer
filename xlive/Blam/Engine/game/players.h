#pragma once
#include "Blam/Engine/memory/data.h"
#include "Blam/Engine/objects/damage_reporting.h"
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

enum e_character_type : byte
{
	character_type_masterchief = 0,
	character_type_dervish = 1,
	character_type_spartan = 2,
	character_type_elite = 3,
	character_type_skeleton = 4,
	character_type_flood = 5,
	character_type_lmao = 6
};

enum e_emblem_foreground : byte
{
	emblem_foreground_seventh_column = 0,
	emblem_foreground_bullseye = 1,
	emblem_foreground_vortex = 2,
	emblem_foreground_halt = 3,
	emblem_foreground_spartan = 4,
	emblem_foreground_da_bomb = 5,
	emblem_foreground_trinity = 6,
	emblem_foreground_delta = 7,
	emblem_foreground_rampancy = 8,
	emblem_foreground_sergeant = 9,
	emblem_foreground_phenoix = 10,
	emblem_foreground_champion = 11,
	emblem_foreground_jolly_roger = 12,
	emblem_foreground_marathon = 13,
	emblem_foreground_cube = 14,
	emblem_foreground_radioactive = 15,
	emblem_foreground_smiley = 16,
	emblem_foreground_frowney = 17,
	emblem_foreground_spearhead = 18,
	emblem_foreground_sol = 19,
	emblem_foreground_waypoint = 20,
	emblem_foreground_ying_yang = 21,
	emblem_foreground_helmet = 22,
	emblem_foreground_triad = 23,
	emblem_foreground_grunt_symbol = 24,
	emblem_foreground_cleave = 25,
	emblem_foreground_thor = 26,
	emblem_foreground_skull_king = 27,
	emblem_foreground_triplicate = 28,
	emblem_foreground_subnova = 29,
	emblem_foreground_flaming_ninja = 30,
	emblem_foreground_doubleCresent = 31,
	emblem_foreground_spades = 32,
	emblem_foreground_clubs = 33,
	emblem_foreground_diamonds = 34,
	emblem_foreground_hearts = 35,
	emblem_foreground_wasp = 36,
	emblem_foreground_mark_of_shame = 37,
	emblem_foreground_snake = 38,
	emblem_foreground_hawk = 39,
	emblem_foreground_lips = 40,
	emblem_foreground_capsule = 41,
	emblem_foreground_cancel = 42,
	emblem_foreground_gas_mask = 43,
	emblem_foreground_grenade = 44,
	emblem_foreground_tsanta = 45,
	emblem_foreground_race = 46,
	emblem_foreground_valkyire = 47,
	emblem_foreground_drone = 48,
	emblem_foreground_grunt = 49,
	emblem_foreground_grunt_head = 50,
	emblem_foreground_brute_head = 51,
	emblem_foreground_runes = 52,
	emblem_foreground_trident = 53,
	emblem_foreground_number0 = 54,
	emblem_foreground_number1 = 55,
	emblem_foreground_number2 = 56,
	emblem_foreground_number3 = 57,
	emblem_foreground_number4 = 58,
	emblem_foreground_number5 = 59,
	emblem_foreground_number6 = 60,
	emblem_foreground_number7 = 61,
	emblem_foreground_number8 = 62,
	emblem_foreground_number9 = 63
};

enum e_emblem_background : BYTE
{
	emblem_background_solid = 0,
	emblem_background_vertical_split = 1,
	emblem_background_horizontal_split1 = 2,
	emblem_background_horizontal_split2 = 3,
	emblem_background_vertical_gradient = 4,
	emblem_background_horizontal_gradient = 5,
	emblem_background_triple_column = 6,
	emblem_background_triple_row = 7,
	emblem_background_quadrants1 = 8,
	emblem_background_quadrants2 = 9,
	emblem_background_diagonal_slice = 10,
	emblem_background_cleft = 11,
	emblem_background_x1 = 12,
	emblem_background_x2 = 13,
	emblem_background_dircle = 14,
	emblem_background_diamond = 15,
	emblem_background_cross = 16,
	emblem_background_square = 17,
	emblem_background_dual_half_circle = 18,
	emblem_background_triangle = 19,
	emblem_background_diagonal_quadrant = 20,
	emblem_background_three_quaters = 21,
	emblem_background_quarter = 22,
	emblem_background_four_rows1 = 23,
	emblem_background_four_rows2 = 24,
	emblem_background_split_circle = 25,
	emblem_background_one_third = 26,
	emblem_background_two_thirds = 27,
	emblem_background_upper_field = 28,
	emblem_background_top_and_bottom = 29,
	emblem_background_center_stripe = 30,
	emblem_background_left_and_right = 31
};

enum e_emblem_toggle : byte
{
	emblem_toggle_off = 0,
	emblem_toggle_on
};

enum e_handicap : byte
{
	handicap_none = 0,
	handicap_minor = 1,
	handicap_moderate = 2,
	handicap_severe = 3
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
	e_emblem_foreground foreground_emblem;
	e_emblem_background background_emblem;
	char emblem_flags;
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
	char unk[32];

	s_player_profile_traits profile_traits;
	wchar_t clan_name[16];
	s_clan_identifiers clan_identifiers;

	e_object_team player_team;
	e_handicap player_handicap_level;
	byte player_displayed_skill;
	byte player_overall_skill;
	char player_is_griefer;
	char bungie_user_role;
	char achievement_flags;
	byte unk2;
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
	static e_object_team GetTeam(int playerIndex);
	static void SetTeam(int playerIndex, e_object_team team);
	static void SetUnitBipedType(int playerIndex, e_character_type bipedType);
	static void SetUnitBipedType(int playerIndex, byte representationIndex);
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
	e_damage_reporting_type damage_reporting_type_1;
	short field_4;
	short field_6;
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
