#pragma once
#include "game_variant.h"
#include "input/input_abstraction.h"
#include "objects/damage_reporting.h"
#include "objects/emblems.h"

/* constants */

enum
{
	k_default_profiles_count = 1
};

/* enums */

enum e_saved_game_profile_input_preference_flags : uint32
{
	_saved_game_profile_input_preference_bit_controller_look_inversion = 0x0,
	_saved_game_profile_input_preference_bit_mouse_look_inversion = 0x1,
	_saved_game_profile_input_preference_bit_vibration_disabled = 0x2,
	_saved_game_profile_input_preference_bit_4 = 0x3,
	_saved_game_profile_input_preference_bit_controller_auto_look_centering = 0x4,
	_saved_game_profile_input_preference_bit_mouse_auto_look_centering = 0x5,
	_saved_game_profile_input_preference_bit_mouse_dual_wield_inversion = 0x6,
	k_saved_game_profile_input_preferences_count
};

/*
 *	New Player Representations Read Me
 *
 *	adding a new representation to the project requires adding a new value to the enum below
 *  once you have done that add a new function in game_globals.cpp to prepare the s_game_globals_custom_representation_result
 *  the function should load/create all necessary data required to create the custom representation (fp, body, biped, variant, etc)
 *	then add the function into the k_game_globals_custom_representation_function_table constant
 */
enum e_character_type : int8
{
	_character_type_masterchief = 0,
	_character_type_dervish = 1,
	_character_type_spartan = 2,
	_character_type_elite = 3,

	k_player_character_type_count_original,	// Original count of characters that shipped with the game

	// cartographer added characters
	_character_type_skeleton = 4,
	_character_type_flood = 5,
	_character_type_lmao = 6,
	k_player_character_type_count,

	k_cartographer_custom_representation_count = k_player_character_type_count - k_player_character_type_count_original
};

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

/* structures */

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

struct s_saved_game_profile_variant_info
{
	int8 data[192];
	s_game_variant variant;
};

struct s_saved_game_profile_input_binds
{
	uint32 button_count;
	s_input_button buttons[5];
};

struct s_saved_game_profile_input_preferences
{
	c_flags_no_init<e_saved_game_profile_input_preference_flags, uint32, k_saved_game_profile_input_preferences_count> flags;
	e_button_preset_types controller_button_layout;
	e_joystick_preset_types controller_thumbstick_layout;
	int8 controller_sensitivity;
	int8 mouse_sensitivity;
	e_custom_keyboard_preset_types keyboard_preset_type;
	int8 pad[3];
	s_saved_game_profile_input_binds input_binds[NUMBER_OF_EXTENDED_CONTROL_BUTTONS];
};
ASSERT_STRUCT_SIZE(s_saved_game_profile_input_preferences, 3660);

struct s_saved_game_player_profile
{
	int32 valid_maybe;
	int32 unk;
	wchar_t name[16];
	int8 data[193];
	bool last_selected_variant_set;
	int8 data2[6];
	enumerated_file_index last_selected_file_index;
	int8 data3[420];
	wchar_t last_selected_variant_name[32];
	int8 data4[20];
	s_saved_game_profile_input_preferences input_preferences;
	int8 data5[16];
	s_player_profile_traits profile_traits;
	int8 gap2[176];
};
ASSERT_STRUCT_SIZE(s_saved_game_player_profile, 4616);

/* prototypes */

void __cdecl saved_game_player_profile_set_default_training_data(void* saved_game_variant);

void saved_game_player_profile_default_new(s_saved_game_player_profile* profile, int32 default_profile_type);

bool saved_game_player_profile_read_file(enumerated_file_index enumerated_file_index, s_saved_game_player_profile* profile);

bool __cdecl saved_game_player_profile_read_post_verify_profile_traits(s_player_profile_traits* profile);

bool saved_game_player_profile_load(enumerated_file_index enumerated_file_index, s_saved_game_player_profile* profile);
