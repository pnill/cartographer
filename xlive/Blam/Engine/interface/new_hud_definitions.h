#pragma once
#include "math/function_definitions.h"
#include "tag_files/string_id.h"
#include "tag_files/tag_reference.h"
#include "memory/static_arrays.h"
#include "text/text.h"

enum e_new_hud_dashlight_flags : short
{
    new_hud_dashlight_flag_dont_scale_when_pulsing = FLAG(0),
};

#define k_hud_dashlight_count 9
struct s_new_hud_dashlight_definition
{
    tag_reference bitmap;       // bitm
    tag_reference shader;       // shad
    short sequence_index;
    e_new_hud_dashlight_flags flags;
    tag_reference sound;        // snd!
};
ASSERT_STRUCT_SIZE(s_new_hud_dashlight_definition, 28);

#define k_hud_waypoint_arrow_type_count 4
struct s_new_hud_waypoint_arrow_definition
{
    tag_reference bitmap;           // bitm
    tag_reference shader;           // shad
    short sequence_index;
    short pad;
    float smallest_size;
    float smallest_distance;
    tag_reference border_bitmap;    // bitm
};
ASSERT_STRUCT_SIZE(s_new_hud_waypoint_arrow_definition, 36);

#define k_hud_waypoint_type_count 8
struct s_new_hud_waypoint_definition
{
    tag_reference bitmap;    // bitm
    tag_reference shader;    // shad
    short onscreen_sequence_index;
    short occluded_sequence_index;
    short offscreen_sequence_index;
    short pad;
};
ASSERT_STRUCT_SIZE(s_new_hud_waypoint_definition, 24);

enum e_sound_latched_to : int
{
    sound_latched_to_shield_recharging = FLAG(0),
    sound_latched_shield_damaged = FLAG(1),
    sound_latched_shield_low = FLAG(2),
    sound_latched_shield_empty = FLAG(3),
    sound_latched_health_low = FLAG(4),
    sound_latched_health_empty = FLAG(5),
    sound_latched_health_minor_damage = FLAG(6),
    sound_latched_health_major_damage = FLAG(7),
    sound_latched_rocket_locking = FLAG(8),
    sound_latched_rocket_locked = FLAG(9)
};

#define k_maximum_hud_sounds 6
struct new_hud_sound_element_definition
{
    tag_reference chief_sound;
    e_sound_latched_to latched_to;
    float scale;
    tag_reference dervish_sound;
};

enum e_player_training_flags : short
{
    player_training_flag_not_in_multiplayer = FLAG(0),
};

#define k_player_training_count 32
struct s_player_training_entry_data
{
    string_id display_string;       // comes out of the HUD text globals
    string_id display_string2;      // comes out of the HUD text globals, used for grouped prompt
    string_id display_string3;      // comes out of the HUD text globals, used for ungrouped prompt
    short max_display_time;         // how long the message can be on screen before being hidden
    short display_count;            // how many times a training message will get displayed (0-3 only!)
    short dissapear_delay;          // how long a displayed but untriggered message stays up
    short redisplay_delay;          // how long after display this message will stay hidden
    float display_delays;           // how long the event can be triggered before it's displayed

    e_player_training_flags flags;
    short pad;
};
ASSERT_STRUCT_SIZE(s_player_training_entry_data, 28);


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

    tag_block<s_new_hud_dashlight_definition> dashlights;
    tag_block<s_new_hud_waypoint_arrow_definition> waypoint_arrows;
    tag_block<s_new_hud_waypoint_definition> waypoints;
    tag_block<new_hud_sound_element_definition> hud_sounds;
    tag_block<s_player_training_entry_data> player_training_data;
    s_new_hud_globals_constants constants;
};
ASSERT_STRUCT_SIZE(s_new_hud_globals, 144);

enum e_hud_anchor : int16
{
    _hud_anchor_health_and_shield = 0,
    _hud_anchor_weapon_hud = 1,
    _hud_anchor_motion_sensor = 2,
    _hud_anchor_scoreboard = 3,
    _hud_anchor_crosshair = 4,
    _hud_anchor_lockon_target = 5
};

enum e_bitmap_widget_flags : uint16
{
    bitmap_widget_flag_flip_horizontally = 0,
    bitmap_widget_flag_flip_vertically = 1,
    bitmap_widget_flag_scope_mirror_horizontally = 2,
    bitmap_widget_flag_scope_mirror_vertically = 3,
    bitmap_widget_flag_scope_stretch = 4,
    k_bitmap_widget_flag_count
};

enum e_special_hud_type : short
{
    special_hud_type_unspecial = 0,

    // sb means scoreboard
    special_hud_type_sb_player_emblem = 1,
    special_hud_type_sb_other_player_emblem = 2,
    special_hud_type_sb_player_score_meter = 3,
    special_hud_type_sb_other_player_score_meter = 4,

    special_hud_type_unit_shield_meter = 5,
    special_hud_type_motion_sensor = 6,
    special_hud_type_territory_meter = 7
};

enum e_hud_input_type : byte
{
    hud_input_type_basic_zero = 0,
    hud_input_type_basic_one = 1,
    hud_input_type_basic_time = 2,
    hud_input_type_basic_global_hud_fade = 3,
    hud_input_type_4 = 4,
    hud_input_type_5 = 5,
    hud_input_type_6 = 6,
    hud_input_type_7 = 7,
    hud_input_type_8 = 8,
    hud_input_type_9 = 9,
    hud_input_type_10 = 10,
    hud_input_type_11 = 11,
    hud_input_type_12 = 12,
    hud_input_type_13 = 13,
    hud_input_type_14 = 14,
    hud_input_type_15 = 15,
    hud_input_type_unit_shield = 16,
    hud_input_type_unit_body = 17,
    hud_input_type_unit_autoaimed = 18,
    hud_input_type_unit_has_no_grenades = 19,
    hud_input_type_unit_frag_gren_cnt = 20,
    hud_input_type_unit_plasma_gren_cnt = 21,
    hud_input_type_unit_time_on_dpl_shld = 22,
    hud_input_type_unit_zoom_fraction = 23,
    hud_input_type_unit_camo_value = 24,
    hud_input_type_25 = 25,
    hud_input_type_26 = 26,
    hud_input_type_27 = 27,
    hud_input_type_28 = 28,
    hud_input_type_29 = 29,
    hud_input_type_30 = 30,
    hud_input_type_31 = 31,
    hud_input_type_parent_shield = 32,
    hud_input_type_parent_body = 33,
    hud_input_type_34 = 34,
    hud_input_type_35 = 35,
    hud_input_type_36 = 36,
    hud_input_type_37 = 37,
    hud_input_type_38 = 38,
    hud_input_type_39 = 39,
    hud_input_type_40 = 40,
    hud_input_type_41 = 41,
    hud_input_type_42 = 42,
    hud_input_type_43 = 43,
    hud_input_type_44 = 44,
    hud_input_type_45 = 45,
    hud_input_type_46 = 46,
    hud_input_type_47 = 47,
    hud_input_type_weapon_clip_ammo = 48,
    hud_input_type_weapon_heat = 49,
    hud_input_type_weapon_battery = 50,
    hud_input_type_weapon_total_ammo = 51,
    hud_input_type_weapon_barrel_spin = 52,
    hud_input_type_weapon_overheated = 53,
    hud_input_type_weapon_clip_ammo_fraction = 54,
    hud_input_type_weapon_time_on_overheat = 55,
    hud_input_type_weapon_battery_fraction = 56,
    hud_input_type_weapon_locking_fraction = 57,
    hud_input_type_58 = 58,
    hud_input_type_59 = 59,
    hud_input_type_60 = 60,
    hud_input_type_61 = 61,
    hud_input_type_62 = 62,
    hud_input_type_63 = 63,
    hud_input_type_64 = 64,
    hud_input_type_user_score_fraction = 65,
    hud_input_type_other_user_score_fraction = 66,
    hud_input_type_user_winning = 67,
    hud_input_type_bomb_arming_amount = 68,
    hud_input_type_69 = 69,
    hud_input_type_70 = 70,
    hud_input_type_71 = 71,
    hud_input_type_72 = 72,
    hud_input_type_73 = 73,
    hud_input_type_74 = 74,
    hud_input_type_75 = 75,
    hud_input_type_76 = 76,
    hud_input_type_77 = 77,
    hud_input_type_78 = 78,
    hud_input_type_79 = 79,
    hud_input_type_80 = 80
};

struct s_hud_widget_inputs_definition
{
    // Explaination("widget inputs", "")

    e_hud_input_type input_1;
    e_hud_input_type input_2;
    e_hud_input_type input_3;
    e_hud_input_type input_4;
};
ASSERT_STRUCT_SIZE(s_hud_widget_inputs_definition, 4);

enum e_widget_state_unit_flags : short
{
    widget_state_unit_flag_default = FLAG(0),
    widget_state_unit_flag_grenade_type_is_none = FLAG(1),
    widget_state_unit_flag_grenade_type_is_frag = FLAG(2),
    widget_state_unit_flag_grenade_type_is_plasma = FLAG(3),
    widget_state_unit_flag_unit_is_single_wielding = FLAG(4),
    widget_state_unit_flag_unit_is_dual_wielding = FLAG(5),
    widget_state_unit_flag_unit_is_unzoomed = FLAG(6),
    widget_state_unit_flag_unit_is_zoomed_level1 = FLAG(7),
    widget_state_unit_flag_unit_is_zoomed_level2 = FLAG(8),
    widget_state_unit_flag_grenades_disabled = FLAG(9),
    widget_state_unit_flag_binoculars_enabled = FLAG(10),
    widget_state_unit_flag_motion_sensor_enabled = FLAG(11),
    widget_state_unit_flag_shield_enabled = FLAG(12),
    widget_state_unit_flag_dervish = FLAG(13)
};

enum e_widget_state_extra_flags : short
{
    widget_state_extra_flag_autoaimfriendly = FLAG(0),
    widget_state_extra_flag_autoaimplasma = FLAG(1),
    widget_state_extra_flag_autoaimheadshot = FLAG(2),
    widget_state_extra_flag_autoaimvulnerable = FLAG(3),
    widget_state_extra_flag_autoaiminvincible = FLAG(4)
};

enum e_widget_state_weapon_flags : uint16
{
    widget_state_weapon_flag_primary_weapon = 0,
    widget_state_weapon_flag_secondary_weapon = 1,
    widget_state_weapon_flag_backpack_weapon = 2,
    widget_state_weapon_flag_age_below_cutoff = 3,
    widget_state_weapon_flag_clip_below_cutoff = 4,
    widget_state_weapon_flag_total_below_cutoff = 5,
    widget_state_weapon_flag_overheated = 6,
    widget_state_weapon_flag_out_of_ammo = 7,
    widget_state_weapon_flag_lock_target_available = 8,
    widget_state_weapon_flag_locking = 9,
    widget_state_weapon_flag_locked = 10,
    k_widget_state_weapon_flag_count
};

enum e_widget_state_game_engine_state_flags : short
{
    widget_state_game_engine_state_flag_campaign_solo = FLAG(0),
    widget_state_game_engine_state_flag_campaign_coop = FLAG(1),
    widget_state_game_engine_state_flag_freeforall = FLAG(2),
    widget_state_game_engine_state_flag_team_game = FLAG(3),
    widget_state_game_engine_state_flag_user_leading = FLAG(4),
    widget_state_game_engine_state_flag_user_not_leading = FLAG(5),
    widget_state_game_engine_state_flag_timed_game = FLAG(6),
    widget_state_game_engine_state_flag_untimed_game = FLAG(7),
    widget_state_game_engine_state_flag_other_score_valid = FLAG(8),
    widget_state_game_engine_state_flag_other_score_invalid = FLAG(9),
    widget_state_game_engine_state_flag_player_is_arming_bomb = FLAG(10),
    widget_state_game_engine_state_flag_player_talking = FLAG(11)
};

struct s_hud_widget_state_definition
{
    /*Explaination("widget state", "this section is split up into YES and NO flags.
    a widget will draw if any of it's YES flags are true,
    but it will NOT draw if any of it's NO flags are true.")*/

    e_widget_state_unit_flags yes_unit_flags;
    e_widget_state_extra_flags yes_extra_flags;
    c_flags_no_init<e_widget_state_weapon_flags, uint16, k_widget_state_weapon_flag_count> yes_weapon_flags;
    e_widget_state_game_engine_state_flags yes_game_engine_state_flags;

    e_widget_state_unit_flags no_unit_flags;
    e_widget_state_extra_flags no_extra_flags;
    e_widget_state_weapon_flags no_weapon_flags;
    e_widget_state_game_engine_state_flags no_game_engine_state_flags;

    byte age_cutoff;
    byte clip_cutoff;
    byte total_cutoff;
    byte pad;
};
ASSERT_STRUCT_SIZE(s_hud_widget_inputs_definition, 4);

enum e_hud_widget_effect_flags : uint16
{
    hud_widget_effect_flag_apply_scale = 0,
    hud_widget_effect_flag_apply_theta = 1,
    hud_widget_effect_flag_apply_offset = 2,
    k_hud_widget_effect_flag_count
};

struct s_hud_widget_effect_function
{
    string_id input_name;
    string_id range_name;
    float time_period_in_seconds;
    c_function_definition function;
};
ASSERT_STRUCT_SIZE(s_hud_widget_effect_function, 20);

#define k_maximum_hud_bitmap_widgets_per_tag 256
struct s_hud_widget_effect_definition
{
    // Explaination("WIDGET EFFECTS", "allow the scaling, rotation, and offsetting of widgets")

    c_flags_no_init<e_hud_widget_effect_flags, uint16, k_hud_widget_effect_flag_count> flags;
    short pad;

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

#define k_maximum_hud_bitmap_widgets_per_tag 256
struct s_hud_bitmap_widget_definition
{
    string_id name;
    s_hud_widget_inputs_definition widget_inputs;
    s_hud_widget_state_definition widget_state;

    e_hud_anchor anchor;
    c_flags_no_init<e_bitmap_widget_flags, uint16, k_bitmap_widget_flag_count> flags;

    tag_reference bitmap;    // bitm
    tag_reference shader;    // shad

    char fullscreen_sequence_index;
    char halfscreen_sequence_index;
    char quarterscreen_sequence_index;
    char pad;
    point2d fullscreen_offset;
    point2d halfscreen_offset;
    point2d quarterscreen_offset;

    real_point2d fullscreen_registration_point;
    real_point2d halfscreen_registration_point;
    real_point2d quarterscreen_registration_point;
    tag_block<s_hud_widget_effect_definition> effect;


    e_special_hud_type special_hud_type;
    short pad1;
};
ASSERT_STRUCT_SIZE(s_hud_bitmap_widget_definition, 100);

enum e_text_widget_flags : uint16
{
    text_widget_flag_string_is_a_number = 0,
    text_widget_flag_force_2digit_number = 1,
    text_widget_flag_force_3digit_number = 2,
    text_widget_flag_talking_player_hack = 3,
    k_text_widget_flag_count
};

#define k_maximum_hud_text_widgets_per_tag 256
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
    e_text_justification justification;
    short pad;

    e_font_index fullscreen_font_index;
    e_font_index halfscreen_font_index;
    e_font_index quarterscreen_font_index;
    byte pad1;
    float fullscreen_scale;
    float halfscreen_scale;
    float quarterscreen_scale;
    point2d fullscreen_offset;
    point2d halfscreen_offset;
    point2d quarterscreen_offset;
    tag_block<s_hud_widget_effect_definition> effect;
};
ASSERT_STRUCT_SIZE(s_hud_text_widget_definition, 84);

struct s_new_hud_dashlight_data
{
    // Explaination("dashlight data", "only relevant to new hud tags for weapons")

    short low_clip_cutoff;  // the cutoff for showing the reload dashlight
    short low_ammo_cutoff;  // the cutoff for showing the low ammo dashlight
    float age_cutoff;       // the age cutoff for showing the low battery dashlight
};
ASSERT_STRUCT_SIZE(s_new_hud_dashlight_data, 8);

enum e_screen_effect_widget_flags : short
{
    screen_effect_widget_flag_unused = FLAG(0)
};

struct s_screen_effect_bonus_struct_block
{
    tag_reference halfscreen_screen_effect;     // egor
    tag_reference quarterscreen_screen_effect;  // egor
};
ASSERT_STRUCT_SIZE(s_screen_effect_bonus_struct_block, 16);

#define k_maximum_hud_screen_effect_widgets_per_tag 4
struct s_hud_screen_effect_widget_definition
{
    string_id name;
    s_hud_widget_inputs_definition widget_inputs;
    s_hud_widget_state_definition widget_state;

    e_hud_anchor anchor;
    e_screen_effect_widget_flags flags;

    tag_reference bitmap;                   // bitm
    tag_reference fullscreen_screen_effect; // egor
    s_screen_effect_bonus_struct_block splitscreen_screen_effects;  // waa

    char fullscreen_sequence_index;
    char halfscreen_sequence_index;
    char quarterscreen_sequence_index;
    char pad;
    point2d fullscreen_offset;
    point2d halfscreen_offset;
    point2d quarterscreen_offset;
};
ASSERT_STRUCT_SIZE(s_hud_screen_effect_widget_definition, 80);

struct s_new_hud_definition
{
    tag_reference donotuse;         // this isnt used anymore
    tag_block<s_hud_bitmap_widget_definition> bitmap_widgets;
    tag_block<s_hud_text_widget_definition> text_widgets;
    s_new_hud_dashlight_data dashlight_data;
    tag_block<s_hud_screen_effect_widget_definition> screen_effect_widgets;
};
ASSERT_STRUCT_SIZE(s_new_hud_definition, 40);