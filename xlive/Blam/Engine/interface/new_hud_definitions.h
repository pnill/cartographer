#pragma once
#include "new_hud.h"

#include "math/function_definitions.h"

/* constants */

enum
{
	NEW_HUD_TAG = 'nhdt'
};

enum
{
	k_hud_dashlight_count = 9,
	k_hud_waypoint_arrow_type_count = 4,
	k_hud_waypoint_type_count = 8,
	k_maximum_hud_bitmap_widgets_per_tag = 256,
	k_maximum_hud_text_widgets_per_tag = 256,
	k_maximum_hud_screen_effect_widgets_per_tag = 4,
	k_maximum_hud_sounds = 6,
};

/* macros */

#define new_hud_definition_get(index)	((struct s_new_hud_definition*)tag_get(NEW_HUD_TAG, (index)))

/* enums */

enum e_new_hud_dashlight_flags
{
	_new_hud_dashlight_flag_dont_scale_when_pulsing_bit = 0,
	k_new_hud_dashlight_flag_count
};

enum e_sound_latched_to
{
	_sound_latched_to_shield_recharging = 0,
	_sound_latched_shield_damaged_bit,
	_sound_latched_shield_low_bit,
	_sound_latched_shield_empty_bit,
	_sound_latched_health_low_bit,
	_sound_latched_health_empty_bit,
	_sound_latched_health_minor_damage_bit,
	_sound_latched_health_major_damage_bit,
	_sound_latched_rocket_locking_bit,
	_sound_latched_rocket_locked_bit,
	k_sound_latched_to_count
};

enum e_bitmap_widget_flags
{
	_bitmap_widget_flag_flip_horizontally = 0,
	_bitmap_widget_flag_flip_vertically,
	_bitmap_widget_flag_scope_mirror_horizontally,
	_bitmap_widget_flag_scope_mirror_vertically,
	_bitmap_widget_flag_scope_stretch,
	k_bitmap_widget_flag_count
};

enum e_special_hud_type
{
	_special_hud_type_unspecial = 0,

	// sb means scoreboard
	_special_hud_type_sb_player_emblem,
	_special_hud_type_sb_other_player_emblem,
	_special_hud_type_sb_player_score_meter,
	_special_hud_type_sb_other_player_score_meter,

	_special_hud_type_unit_shield_meter,
	_special_hud_type_motion_sensor,
	_special_hud_type_territory_meter,
	k_special_hud_type_count,
};

enum e_hud_input_type
{
	_hud_input_type_basic_zero = 0,
	_hud_input_type_basic_one,
	_hud_input_type_basic_time,
	_hud_input_type_basic_global__hud_fade,
	_hud_input_type_4,
	_hud_input_type_5,
	_hud_input_type_6,
	_hud_input_type_7,
	_hud_input_type_8,
	_hud_input_type_9,
	_hud_input_type_10,
	_hud_input_type_11,
	_hud_input_type_12,
	_hud_input_type_13,
	_hud_input_type_14,
	_hud_input_type_15,
	_hud_input_type_unit_shield,
	_hud_input_type_unit_body,
	_hud_input_type_unit_autoaimed,
	_hud_input_type_unit_has_no_grenades,
	_hud_input_type_unit_frag_gren_cnt,
	_hud_input_type_unit_plasma_gren_cnt,
	_hud_input_type_unit_time_on_dpl_shld,
	_hud_input_type_unit_zoom_fraction,
	_hud_input_type_unit_camo_value,
	_hud_input_type_25,
	_hud_input_type_26,
	_hud_input_type_27,
	_hud_input_type_28,
	_hud_input_type_29,
	_hud_input_type_30,
	_hud_input_type_31,
	_hud_input_type_parent_shield,
	_hud_input_type_parent_body,
	_hud_input_type_34,
	_hud_input_type_35,
	_hud_input_type_36,
	_hud_input_type_37,
	_hud_input_type_38,
	_hud_input_type_39,
	_hud_input_type_40,
	_hud_input_type_41,
	_hud_input_type_42,
	_hud_input_type_43,
	_hud_input_type_44,
	_hud_input_type_45,
	_hud_input_type_46,
	_hud_input_type_47,
	_hud_input_type_weapon_clip_ammo,
	_hud_input_type_weapon_heat,
	_hud_input_type_weapon_battery,
	_hud_input_type_weapon_total_ammo,
	_hud_input_type_weapon_barrel_spin,
	_hud_input_type_weapon_overheated,
	_hud_input_type_weapon_clip_ammo_fraction,
	_hud_input_type_weapon_time_on_overheat,
	_hud_input_type_weapon_battery_fraction,
	_hud_input_type_weapon_locking_fraction,
	_hud_input_type_58,
	_hud_input_type_59,
	_hud_input_type_60,
	_hud_input_type_61,
	_hud_input_type_62,
	_hud_input_type_63,
	_hud_input_type_64,
	_hud_input_type_user_score_fraction,
	_hud_input_type_other_user_score_fraction,
	_hud_input_type_user_winning,
	_hud_input_type_bomb_arming_amount,
	_hud_input_type_69,
	_hud_input_type_70,
	_hud_input_type_71,
	_hud_input_type_72,
	_hud_input_type_73,
	_hud_input_type_74,
	_hud_input_type_75,
	_hud_input_type_76,
	_hud_input_type_77,
	_hud_input_type_78,
	_hud_input_type_79,
	_hud_input_type_80,
	k_hud_input_type_count
};

enum e_widget_state_unit_flags
{
	_widget_state_unit_flag_default_bit = 0,
	_widget_state_unit_flag_grenade_type_is_none_bit,
	_widget_state_unit_flag_grenade_type_is_frag_bit,
	_widget_state_unit_flag_grenade_type_is_plasma_bit,
	_widget_state_unit_flag_unit_is_single_wielding_bit,
	_widget_state_unit_flag_unit_is_dual_wielding_bit,
	_widget_state_unit_flag_unit_is_unzoomed_bit,
	_widget_state_unit_flag_unit_is_zoomed_level1_bit,
	_widget_state_unit_flag_unit_is_zoomed_level2_bit,
	_widget_state_unit_flag_grenades_disabled_bit,
	_widget_state_unit_flag_binoculars_enabled_bit,
	_widget_state_unit_flag_motion_sensor_enabled_bit,
	_widget_state_unit_flag_shield_enabled_bit,
	_widget_state_unit_flag_dervish_bit,
	k_widget_state_unit_flag_count
};

enum e_widget_state_extra_flags
{
	_widget_state_extra_flag_autoaimfriendly_bit = 0,
	_widget_state_extra_flag_autoaimplasma_bit,
	_widget_state_extra_flag_autoaimheadshot_bit,
	_widget_state_extra_flag_autoaimvulnerable_bit,
	_widget_state_extra_flag_autoaiminvincible_bit,
	k_widget_state_weapon_extra_flag_count
};

enum e_widget_state_weapon_flags
{
	_widget_state_weapon_flag_primary_weapon = 0,
	_widget_state_weapon_flag_secondary_weapon,
	_widget_state_weapon_flag_backpack_weapon,
	_widget_state_weapon_flag_age_below_cutoff,
	_widget_state_weapon_flag_clip_below_cutoff,
	_widget_state_weapon_flag_total_below_cutoff,
	_widget_state_weapon_flag_overheated,
	_widget_state_weapon_flag_out_of_ammo,
	_widget_state_weapon_flag_lock_target_available,
	_widget_state_weapon_flag_locking,
	_widget_state_weapon_flag_locked,
	k_widget_state_weapon_flag_count
};

enum e_widget_state_game_engine_state_flags
{
	_widget_state_game_engine_state_flag_campaign_solo_bit = 0,
	_widget_state_game_engine_state_flag_campaign_coop_bit,
	_widget_state_game_engine_state_flag_freeforall_bit,
	_widget_state_game_engine_state_flag_team_game_bit,
	_widget_state_game_engine_state_flag_user_leading_bit,
	_widget_state_game_engine_state_flag_user_not_leading_bit,
	_widget_state_game_engine_state_flag_timed_game_bit,
	_widget_state_game_engine_state_flag_untimed_game_bit,
	_widget_state_game_engine_state_flag_other_score_valid_bit,
	_widget_state_game_engine_state_flag_other_score_invalid_bit,
	_widget_state_game_engine_state_flag_player_is_arming_bomb_bit,
	_widget_state_game_engine_state_flag_player_talking_bit,
	k_widget_state_game_engine_state_flag_count
};

enum e_hud_widget_effect_flags
{
	_hud_widget_effect_flag_apply_scale = 0,
	_hud_widget_effect_flag_apply_theta,
	_hud_widget_effect_flag_apply_offset,
	k_hud_widget_effect_flag_count
};

enum e_text_widget_flags
{
	_text_widget_flag_string_is_a_number = 0,
	_text_widget_flag_force_2digit_number,
	_text_widget_flag_force_3digit_number,
	_text_widget_flag_talking_player_hack,
	k_text_widget_flag_count
};

enum e_screen_effect_widget_flags
{
	_screen_effect_widget_flag_unused_bit = 0,
	k_screen_effect_widget_flag_count
};

/* structures */

struct s_new_hud_dashlight_definition
{
	tag_reference bitmap;       // bitm
	tag_reference shader;       // shad
	int16 sequence_index;
	int16 flags;				// e_new_hud_dashlight_flags
	tag_reference sound;        // snd!
};
ASSERT_STRUCT_SIZE(s_new_hud_dashlight_definition, 28);

struct s_new_hud_waypoint_arrow_definition
{
	tag_reference bitmap;           // bitm
	tag_reference shader;           // shad
	int16 sequence_index;
	int16 pad;
	real32 smallest_size;
	real32 smallest_distance;
	tag_reference border_bitmap;    // bitm
};
ASSERT_STRUCT_SIZE(s_new_hud_waypoint_arrow_definition, 36);

struct s_new_hud_waypoint_definition
{
	tag_reference bitmap;    // bitm
	tag_reference shader;    // shad
	int16 onscreen_sequence_index;
	int16 occluded_sequence_index;
	int16 offscreen_sequence_index;
	int16 pad;
};
ASSERT_STRUCT_SIZE(s_new_hud_waypoint_definition, 24);

struct new_hud_sound_element_definition
{
	tag_reference chief_sound;
	e_sound_latched_to latched_to;
	real32 scale;
	tag_reference dervish_sound;
};

struct s_new_hud_globals_constants
{
	tag_reference primary_message_sound;
	tag_reference secondary_message_sound;
	string_id boot_griefer_string;
	string_id cannot_boot_griefer_string;

	tag_reference training_shader;              // shad
	tag_reference human_training_top_right;     // bitm
	tag_reference human_training_top_center;    // bitm
	tag_reference human_training_top_left;      // bitm
	tag_reference human_training_middle;        // bitm
	tag_reference elite_training_top_right;     // bitm
	tag_reference elite_training_top_center;    // bitm
	tag_reference elite_training_top_left;      // bitm
	tag_reference elite_training_middle;        // bitm
};
ASSERT_STRUCT_SIZE(s_new_hud_globals_constants, 96);

struct s_new_hud_globals  
{
	tag_reference hud_text;    // unic

	s_tag_block dashlights;				// s_new_hud_dashlight_definition
	s_tag_block waypoint_arrows;		// s_new_hud_waypoint_arrow_definition
	s_tag_block waypoints;				// s_new_hud_waypoint_definition
	s_tag_block hud_sounds;				// new_hud_sound_element_definition
	s_tag_block player_training_data;	// s_player_training_entry_data
	s_new_hud_globals_constants constants;
};
ASSERT_STRUCT_SIZE(s_new_hud_globals, 144);

struct s_hud_widget_inputs_definition
{
	// Explaination("widget inputs", "")

	uint8 inputs[4];	// e_hud_input_type
};
ASSERT_STRUCT_SIZE(s_hud_widget_inputs_definition, 4);

struct s_hud_widget_state_definition
{
	/*Explaination("widget state", "this section is split up into YES and NO flags.
	a widget will draw if any of it's YES flags are true,
	but it will NOT draw if any of it's NO flags are true.")*/

	c_flags_no_init<e_widget_state_unit_flags, uint16, k_widget_state_unit_flag_count> yes_unit_flags;
	c_flags_no_init<e_widget_state_extra_flags, uint16, k_widget_state_weapon_extra_flag_count> yes_extra_flags;
	c_flags_no_init<e_widget_state_weapon_flags, uint16, k_widget_state_weapon_flag_count> yes_weapon_flags;
	c_flags_no_init<e_widget_state_game_engine_state_flags, uint16, k_widget_state_game_engine_state_flag_count> yes_game_engine_state_flags;

	c_flags_no_init<e_widget_state_unit_flags, uint16, k_widget_state_unit_flag_count>  no_unit_flags;
	c_flags_no_init<e_widget_state_extra_flags, uint16, k_widget_state_weapon_extra_flag_count> no_extra_flags;
	c_flags_no_init<e_widget_state_weapon_flags, uint16, k_widget_state_weapon_flag_count> no_weapon_flags;
	c_flags_no_init<e_widget_state_game_engine_state_flags, uint16, k_widget_state_game_engine_state_flag_count>  no_game_engine_state_flags;

	uint8 age_cutoff;
	uint8 clip_cutoff;
	uint8 total_cutoff;
	uint8 pad;
};
ASSERT_STRUCT_SIZE(s_hud_widget_inputs_definition, 4);

struct s_hud_widget_effect_function
{
	string_id input_name;
	string_id range_name;
	real32 time_period_in_seconds;
	c_function_definition function;
};
ASSERT_STRUCT_SIZE(s_hud_widget_effect_function, 20);

struct s_hud_widget_effect_definition
{
	// Explaination("WIDGET EFFECTS", "allow the scaling, rotation, and offsetting of widgets")

	c_flags_no_init<e_hud_widget_effect_flags, uint16, k_hud_widget_effect_flag_count> flags;
	int16 pad;

	// Your mom below (according to bungie)

	// Explaination("horizontal and vertical scale", "")
	s_hud_widget_effect_function horizontal_scale;
	s_hud_widget_effect_function vertical_scale;

	// Explaination("theta", "")
	s_hud_widget_effect_function theta;

	// Explaination("horizontal and vertical offset", "")
	s_hud_widget_effect_function horizontal_offset;
	s_hud_widget_effect_function vertical_offset;
};
ASSERT_STRUCT_SIZE(s_hud_widget_effect_definition, 104);

struct s_hud_bitmap_widget_definition
{
	string_id name;
	s_hud_widget_inputs_definition widget_inputs;
	s_hud_widget_state_definition widget_state;

	e_hud_anchor anchor;
	c_flags_no_init<e_bitmap_widget_flags, uint16, k_bitmap_widget_flag_count> flags;

	tag_reference bitmap;    // bitm
	tag_reference shader;    // shad

	int8 screen_sequence_indices[k_split_screen_type_count];
	int8 pad;

	point2d screen_offsets[k_split_screen_type_count];

	real_point2d registration_points[k_split_screen_type_count];
	s_tag_block effect;		// s_hud_widget_effect_definition


	int16 special_hud_type;	// e_special_hud_type
	int16 pad1;
};
ASSERT_STRUCT_SIZE(s_hud_bitmap_widget_definition, 100);

struct s_hud_text_widget_definition
{
	string_id name;
	s_hud_widget_inputs_definition widget_inputs;
	s_hud_widget_state_definition widget_state;

	e_hud_anchor anchor;
	
	/*Explaination("FLAGS", "string is a number: treats the inputted string id as a function name, not a string name
	force 2 - digit number : when used in combination with above, forces output to be a 2 - digit numberwith leading zeros if necessary
	force 3 - digit number : same as above, but with 3 digits instead of 2")*/
	c_flags_no_init<e_text_widget_flags, uint16, k_text_widget_flag_count> flags;

	tag_reference shader;   // shad
	string_id string;
	int16 justification;	// e_text_justification
	int16 pad;

	int8 screen_font_indices[k_split_screen_type_count];	// e_font_index
	int8 pad1;

	real32 screen_scales[k_split_screen_type_count];
	point2d screen_offsets[k_split_screen_type_count];
	s_tag_block effect;		// s_hud_widget_effect_definition
};
ASSERT_STRUCT_SIZE(s_hud_text_widget_definition, 84);

struct s_new_hud_dashlight_data
{
	// Explaination("dashlight data", "only relevant to new hud tags for weapons")

	int16 low_clip_cutoff;  // the cutoff for showing the reload dashlight
	int16 low_ammo_cutoff;  // the cutoff for showing the low ammo dashlight
	real32 age_cutoff;       // the age cutoff for showing the low battery dashlight
};
ASSERT_STRUCT_SIZE(s_new_hud_dashlight_data, 8);

struct s_screen_effect_bonus_struct_block
{
	tag_reference halfscreen_screen_effect;     // egor
	tag_reference quarterscreen_screen_effect;  // egor
};
ASSERT_STRUCT_SIZE(s_screen_effect_bonus_struct_block, 16);

struct s_hud_screen_effect_widget_definition
{
	string_id name;
	s_hud_widget_inputs_definition widget_inputs;
	s_hud_widget_state_definition widget_state;

	e_hud_anchor anchor;
	uint16 flags;							// e_screen_effect_widget_flags

	tag_reference bitmap;					// bitm
	tag_reference fullscreen_screen_effect;	// egor
	s_screen_effect_bonus_struct_block splitscreen_screen_effects;  // waa

	int8 screen_sequence_indices[k_split_screen_type_count];
	int8 pad;

	point2d screen_offsets[k_split_screen_type_count];
};
ASSERT_STRUCT_SIZE(s_hud_screen_effect_widget_definition, 80);

struct s_new_hud_definition
{
	tag_reference donotuse;				// this isnt used anymore
	s_tag_block bitmap_widgets;			// s_hud_bitmap_widget_definition
	s_tag_block text_widgets;			// s_hud_text_widget_definition
	s_new_hud_dashlight_data dashlight_data;
	s_tag_block screen_effect_widgets;	// s_hud_screen_effect_widget_definition
};
ASSERT_STRUCT_SIZE(s_new_hud_definition, 40);
