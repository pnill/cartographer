#pragma once
#include "Blam/Cache/DataTypes/BlamDataTypes.h"
#include "Blam/Cache/TagGroups.hpp"

#include "Blam/Engine/math/color_math.h"



enum e_screen_widget_flags : int
{
    screen_widget_flag_quarter_screen_dialog = FLAG(0),
    screen_widget_flag_multiple_panes_are_for_list_flavor_items = FLAG(1),
    screen_widget_flag_no_header_text = FLAG(2),
    screen_widget_flag_half_screen_dialog = FLAG(3),
    screen_widget_flag_large_dialog = FLAG(4),
    screen_widget_flag_disable_overlay_effect = FLAG(5)
};

enum e_screen_id : short
{
    screen_id_test1 = 0,
    screen_id_test2 = 1,
    screen_id_test3 = 2,
    screen_id_test4 = 3,
    screen_id_test5 = 4,
    screen_id_game_shell_background = 5,
    screen_id_main_menu = 6,
    screen_id_error_dialog_ok_cancel = 7,
    screen_id_error_dialog_ok = 8,
    screen_id_press_start_intro = 9,
    screen_id_player_profile_select = 10,
    screen_id_sp_level_select = 11,
    screen_id_sp_difficulty_select = 12,
    screen_id_network_squad_browser = 13,
    screen_id_mp_pregame_lobby = 14,
    screen_id_custom_game_menu = 15,
    screen_id_postgame_stats = 16,
    screen_id_mp_map_select = 17,
    screen_id_sp_pause_game = 18,
    screen_id_settings = 19,
    screen_id_gamertag_select = 20,
    screen_id_gamertag_passcode_entry = 21,
    screen_id_multiplayer_protocol = 22,
    screen_id_squad_settings = 23,
    screen_id_squad_game_settings = 24,
    screen_id_squad_privacy_settings = 25,
    screen_id_y_menu_gameshell = 26,
    screen_id_y_menu_gameshell_collapsed = 27,
    screen_id_y_menu_in_game = 28,
    screen_id_y_menu_in_game_collapsed = 29,
    screen_id_4way_join_screen = 30,
    screen_id_y_menu_player_selected_options = 31,
    screen_id_player_selected_options = 32,
    screen_id_confirmation_dialog = 33,
    screen_id_live_feedback_menu_dialog = 34,
    screen_id_live_message_type_dialog = 35,
    screen_id_voice_msg_dialog = 36,
    screen_id_stereo_faceplate = 37,
    screen_id_player_profile_edit_menu = 38,
    screen_id_pp_controller_settings = 39,
    screen_id_pp_button_settings = 40,
    screen_id_pp_thumbstick_settings = 41,
    screen_id_pp_look_sensitivity_settings = 42,
    screen_id_pp_invert_look_settings = 43,
    screen_id_pp_autolevel_settings = 44,
    screen_id_pp_handicap_settings = 45,
    screen_id_pp_high_score_rec_settings = 46,
    screen_id_pp_multiplayer_settings_menu = 47,
    screen_id_pp_profile_delete_confirmation_dlg = 48,
    screen_id_pp_choose_foreground_emblem = 49,
    screen_id_pp_choose_primary_color = 50,
    screen_id_pp_choose_secondary_color = 51,
    screen_id_pp_choose_model = 52,
    screen_id_pp_voice_settings_menu = 53,
    screen_id_pp_choose_voice_mask = 54,
    screen_id_pp_voice_thru_tv = 55,
    screen_id_pp_edit_rotation_list = 56,
    screen_id_pp_xbl_status_menu = 57,
    screen_id_pp_appear_offline = 58,
    screen_id_pp_auto_offline = 59,
    screen_id_game_engine_category_listing = 60,
    screen_id_edit_slayer_menu = 61,
    screen_id_edit_koth_menu = 62,
    screen_id_edit_race_menu = 63,
    screen_id_edit_oddball_menu = 64,
    screen_id_edit_juggernaut_menu = 65,
    screen_id_edit_headhunter_menu = 66,
    screen_id_edit_ctf_menu = 67,
    screen_id_edit_assault_menu = 68,
    screen_id_edi_slayer_score_to_win = 69,
    screen_id_edit_slayer_time_limit = 70,
    screen_id_edit_slayer_teams = 71,
    screen_id_edit_slayer_score_4_killing = 72,
    screen_id_edit_slayer_kill_in_order = 73,
    screen_id_edit_slayer_death_pt_loss = 74,
    screen_id_edit_slayer_suicide_pt_loss = 75,
    screen_id_edit_slayer_dmg_after_kill = 76,
    screen_id_edit_slayer_dmg_after_death = 77,
    screen_id_edit_slayer_speed_after_kill = 78,
    screen_id_edit_slayer_speed_after_death = 79,
    screen_id_edit_koth_score_to_win = 80,
    screen_id_edit_koth_time_limit = 81,
    screen_id_edit_koth_teams = 82,
    screen_id_edit_koth_moving_hills = 83,
    screen_id_edit_koth_uncontesed_control = 84,
    screen_id_edit_koth_xtra_dmg = 85,
    screen_id_edit_race_laps_to_win = 86,
    screen_id_edit_race_time_limit = 87,
    screen_id_edit_race_teams = 88,
    screen_id_edit_race_team_scoring = 89,
    screen_id_edit_race_type = 90,
    screen_id_edit_race_flag_order = 91,
    screen_id_edit_race_game_end_condition = 92,
    screen_id_edit_race_dmg_with_laps = 93,
    screen_id_edit_race_speed_with_laps = 94,
    screen_id_edit_oddball_time_to_win = 95,
    screen_id_edit_oddball_time_limit = 96,
    screen_id_edit_oddball_teams = 97,
    screen_id_edit_oddball_ball_spawn_count = 98,
    screen_id_edit_oddball_ball_waypoints = 99,
    screen_id_edit_oddball_damage_with_ball = 100,
    screen_id_edit_oddball_speed_with_ball = 101,
    screen_id_edit_oddball_invisibility_with_ball = 102,
    screen_id_edit_jug_score_to_win = 103,
    screen_id_edit_jug_time_limit = 104,
    screen_id_edit_jug_pts_for_killing_jugger = 105,
    screen_id_edit_jug_count = 106,
    screen_id_edit_jug_species = 107,
    screen_id_edit_jug_starting_equip = 108,
    screen_id_edit_jug_damage = 109,
    screen_id_edit_jug_health = 110,
    screen_id_edit_jug_speed = 111,
    screen_id_edit_jug_regeneration = 112,
    screen_id_edit_jug_waypoints = 113,
    screen_id_edit_hh_score_to_win = 114,
    screen_id_edit_hh_time_limit = 115,
    screen_id_edit_hh_teams = 116,
    screen_id_edit_hh_death_pt_loss = 117,
    screen_id_edit_hh_suicide_pt_loss = 118,
    screen_id_edit_hh_speed_with_token = 119,
    screen_id_edit_hh_dropped_token_lifetime = 120,
    screen_id_edit_hh_score_multiplier = 121,
    screen_id_edit_ctf_score_to_win = 122,
    screen_id_edit_ctf_time_limit = 123,
    screen_id_edit_ctf_tie_resolution = 124,
    screen_id_edit_ctf_single_flag = 125,
    screen_id_edit_ctf_role_swapping = 126,
    screen_id_edit_ctf_flag_at_home_to_score = 127,
    screen_id_edit_ctf_flag_must_reset = 128,
    screen_id_edit_ctf_dmg_with_flag = 129,
    screen_id_edit_ctf_speed_with_flag = 130,
    screen_id_edit_assault_score_to_win = 131,
    screen_id_edit_assault_time_limit = 132,
    screen_id_edit_assault_tie_resolution = 133,
    screen_id_edit_assault_single_flag = 134,
    screen_id_edit_assault_role_swapping = 135,
    screen_id_edit_assault_enemy_flag_at_home_to_score = 136,
    screen_id_edit_assault_flag_must_reset = 137,
    screen_id_edit_assault_dmg_with_flag = 138,
    screen_id_edit_assault_speed_with_flag = 139,
    screen_id_edit_player_number_of_lives = 140,
    screen_id_edit_player_max_health = 141,
    screen_id_edit_player_shields = 142,
    screen_id_edit_player_respawn_time = 143,
    screen_id_edit_player_count = 144,
    screen_id_edit_player_invisibility = 145,
    screen_id_edit_player_suicide_penalty = 146,
    screen_id_edit_player_friendly_fire = 147,
    screen_id_edit_item_respawn_grenades = 148,
    screen_id_edit_item_powerups = 149,
    screen_id_edit_item_weapon_set = 150,
    screen_id_edit_item_starting_equipment = 151,
    screen_id_edit_item_warthogs = 152,
    screen_id_edit_item_ghosts = 153,
    screen_id_edit_item_scorpions = 154,
    screen_id_edit_item_banshees = 155,
    screen_id_edit_item_mongeese = 156,
    screen_id_edit_item_shadows = 157,
    screen_id_edit_item_wraiths = 158,
    screen_id_edit_indicator_objectives = 159,
    screen_id_edit_indicator_players_on_motion_sensor = 160,
    screen_id_edit_indicator_invisible_players_on_motion_sensor = 161,
    screen_id_edit_indicator_friends = 162,
    screen_id_edit_indicator_enemies = 163,
    screen_id_edit_player_options = 164,
    screen_id_edit_item_options = 165,
    screen_id_edit_indicator_options = 166,
    screen_id_virtual_keyboard = 167,
    screen_id_custom_game_menu_168 = 168,
    screen_id_slayer_quick_options = 169,
    screen_id_koth_quick_options = 170,
    screen_id_race_quick_options = 171,
    screen_id_oddball_quick_options = 172,
    screen_id_jugger_quick_options = 173,
    screen_id_hh_quick_options = 174,
    screen_id_ctf_quick_options = 175,
    screen_id_assault_quick_options = 176,
    screen_id_pick_new_squad_leader = 177,
    screen_id_variant_editing_options_menu = 178,
    screen_id_playlist_list_settings = 179,
    screen_id_playlist_contents = 180,
    screen_id_playlist_selected_options = 181,
    screen_id_xbox_live_task_progress = 182,
    screen_id_pp_vibration_settings = 183,
    screen_id_boot_player_dialog = 184,
    screen_id_postgame_stats_lobby = 185,
    screen_id_xbox_live_main_menu = 186,
    screen_id_edit_terries_menu = 187,
    screen_id_edit_terries_score_to_win = 188,
    screen_id_edit_terries_time_limit = 189,
    screen_id_edit_terries_teams = 190,
    screen_id_terries_quick_options = 191,
    screen_id_xbox_live_notification_beeper = 192,
    screen_id_player_profile_select_fancy = 193,
    screen_id_saved_game_file_actions_dialog = 194,
    screen_id_mp_start_menu = 195,
    screen_id_mp_start_player_settings = 196,
    screen_id_mp_start_handicap_settings = 197,
    screen_id_mp_start_change_teams = 198,
    screen_id_mp_start_admin_settings = 199,
    screen_id_mp_start_controller_settings = 200,
    screen_id_mp_start_voice_settings = 201,
    screen_id_mp_start_online_status = 202,
    screen_id_mp_alpha_legal_warning = 203,
    screen_id_squad_join_progress_dialog = 204,
    screen_id_mp_alpha_postgame_legal_warning = 205,
    screen_id_mp_map_select_lobby = 206,
    screen_id_mp_variant_type_lobby = 207,
    screen_id_mp_variant_list_lobby = 208,
    screen_id_loading_progress = 209,
    screen_id_matchmaking_progress = 210,
    screen_id_live_message_display = 211,
    screen_id_fadein_from_black = 212,
    screen_id_live_player_profile = 213,
    screen_id_live_clan_profile = 214,
    screen_id_live_message_send = 215,
    screen_id_friends_options_dialog = 216,
    screen_id_clan_options_dialog = 217,
    screen_id_campaign_options_dialog = 218,
    screen_id_optimatch_hoppers_fullscreen = 219,
    screen_id_playlist_listdialog = 220,
    screen_id_variant_editing_format = 221,
    screen_id_variant_quick_options_format = 222,
    screen_id_variant_param_setting_format = 223,
    screen_id_vehicle_options = 224,
    screen_id_match_options = 225,
    screen_id_player_options = 226,
    screen_id_team_options = 227,
    screen_id_game_options = 228,
    screen_id_equipment_options = 229,
    screen_id_multiple_choice_dialog = 230,
    screen_id_network_transition_progress = 231,
    screen_id_xbox_live_stats = 232,
    screen_id_pp_choose_background_emblem = 233,
    screen_id_pp_buttons_qtr = 234,
    screen_id_pp_stix_qtr = 235,
    screen_id_clan_member_privs = 236,
    screen_id_optimatch_hoppers_lobby = 237,
    screen_id_saved_game_file_dialog = 238,
    screen_id_xyzzy = 239,
    screen_id_error_ok_cancel_large = 240,
    screen_id_yzzyx = 241,
    screen_id_subtitle_display = 242,
    screen_id_pp_keyboard_settings = 243,
    screen_id_pp_keyboard_settings_qtr = 244,
    screen_id_pp_invert_dual_wield = 245,
    screen_id_system_settings = 246,
    screen_id_bungie_news = 247,
    screen_id_filter_select = 248,
    screen_id_live_game_browser = 249,
    screen_id_game_details = 250,
    screen_id_mp_custom_map_select = 251,
    screen_id_mp_all_maps_select = 252,
    screen_id_pp_advanced_keyboard_settings = 253,
    screen_id_pp_advanced_keyboard_settings_qtr = 254,
    screen_id_network_adapter_settings = 255
};

enum e_button_key_type : short
{
    button_key_type_none = 0,
    button_key_type_a_select_b_back = 1,
    button_key_type_a_select_b_Cancel = 2,
    button_key_type_a_enter_b_cancel = 3,
    button_key_type_y_xbl_players = 4,
    button_key_type_x_friend_options = 5,
    button_key_type_x_clan_options = 6,
    button_key_type_x_recent_players_options = 7,
    button_key_type_x_options = 8,
    button_key_type_a_select = 9,
    button_key_type_x_settings_a_select_b_back = 10,
    button_key_type_x_delete_a_select_b_done = 11,
    button_key_type_a_accept = 12,
    button_key_type_b_cancel = 13,
    button_key_type_y_xbox_live_players_a_select_b_back = 14,
    button_key_type_y_xbox_live_players_a_select_b_cancel = 15,
    button_key_type_y_xbox_live_players_a_enter_b_cancel = 16,
    button_key_type_y_xbox_live_players_a_select = 17,
    button_key_type_y_xbox_live_players_a_select_b_done = 18,
    button_key_type_y_xbox_live_players_a_accept = 19,
    button_key_type_y_xbox_live_players_b_cancel = 20,
    button_key_type_x_delete_a_select_b_back = 21,
    button_key_type_a_ok = 22
};

enum e_animation_index : short
{
    none = 0,
    animation_index_00 = 1,
    animation_index_01 = 2,
    animation_index_02 = 3,
    animation_index_03 = 4,
    animation_index_04 = 5,
    animation_index_05 = 6,
    animation_index_06 = 7,
    animation_index_07 = 8,
    animation_index_08 = 9,
    animation_index_09 = 10,
    animation_index_10 = 11,
    animation_index_11 = 12,
    animation_index_12 = 13,
    animation_index_13 = 14,
    animation_index_14 = 15,
    animation_index_15 = 16,
    animation_index_16 = 17,
    animation_index_17 = 18,
    animation_index_18 = 19,
    animation_index_19 = 20,
    animation_index_20 = 21,
    animation_index_21 = 22,
    animation_index_22 = 23,
    animation_index_23 = 24,
    animation_index_24 = 25,
    animation_index_25 = 26,
    animation_index_26 = 27,
    animation_index_27 = 28,
    animation_index_28 = 29,
    animation_index_29 = 30,
    animation_index_30 = 31,
    animation_index_31 = 32,
    animation_index_32 = 33,
    animation_index_33 = 34,
    animation_index_34 = 35,
    animation_index_35 = 36,
    animation_index_36 = 37,
    animation_index_37 = 38,
    animation_index_38 = 39,
    animation_index_39 = 40,
    animation_index_40 = 41,
    animation_index_41 = 42,
    animation_index_42 = 43,
    animation_index_43 = 44,
    animation_index_44 = 45,
    animation_index_45 = 46,
    animation_index_46 = 47,
    animation_index_47 = 48,
    animation_index_48 = 49,
    animation_index_49 = 50,
    animation_index_50 = 51,
    animation_index_51 = 52,
    animation_index_52 = 53,
    animation_index_53 = 54,
    animation_index_54 = 55,
    animation_index_55 = 56,
    animation_index_56 = 57,
    animation_index_57 = 58,
    animation_index_58 = 59,
    animation_index_59 = 60,
    animation_index_60 = 61,
    animation_index_61 = 62,
    animation_index_62 = 63,
    animation_index_63 = 64
};

enum e_text_flags : int
{
    text_flag_left_justify_text = FLAG(0),
    text_flag_right_justify_text = FLAG(1),
    text_flag_pulsating_text = FLAG(2),
    text_flag_callout_text = FLAG(3),
    text_flag_small_31_char_buffer = FLAG(4)
};

enum e_custom_font : short
{
    custom_font_terminal = 0,
    custom_font_body_text = 1,
    custom_font_title = 2,
    custom_font_super_large_font = 3,
    custom_font_large_body_text = 4,
    custom_font_split_screen_hud_message = 5,
    custom_font_full_screen_hud_message = 6,
    custom_font_english_body_text = 7,
    custom_font_hud_number_text = 8,
    custom_font_subtitle_font = 9,
    custom_font_main_menu_font = 10,
    custom_font_text_chat_font = 11
};

enum e_button_flags : int
{
    button_flag_doesnt_ta_vertically = FLAG(0),
    button_flag_doesnt_tab_horizontally = FLAG(1)
};

#define k_maximum_number_of_button_widget_blocks 64
struct s_button_widget_reference
{
    e_text_flags text_flags;
    e_animation_index animation_index;
    short intro_animation_delay_milliseconds;
    short pad;

    e_custom_font custom_font;
    real_argb_color text_color;
    rectangle2d bounds;

    // bitm
    tag_reference bitmap;
    /// from top-left
    point2d bitmap_offset;
    string_id string_id;
    short render_depth_bias;
    short mouse_region_top_offset;
    
    e_button_flags button_flags;
};
TAG_BLOCK_SIZE_ASSERT(s_button_widget_reference, 0x3C);

enum e_list_reference_flags : int
{
    list_reference_flag_list_wraps = FLAG(0),
    list_reference_flag_interactive = FLAG(1)
};

enum e_skin_index : short
{
    skin_index_default = 0,
    skin_index_squad_lobby_player_list = 1,
    skin_index_settings_list = 2,
    skin_index_playlist_entry_list = 3,
    skin_index_variants = 4,
    skin_index_game_browser = 5,
    skin_index_online_player_menu = 6,
    skin_index_game_setup_menu = 7,
    skin_index_playlist_contents_display = 8,
    skin_index_player_profile_picker = 9,
    skin_index_mp_map_selection = 10,
    skin_index_main_menu_list = 11,
    skin_index_color_picker = 12,
    skin_index_profile_picker = 13,
    skin_index_y_menu_recent_list = 14,
    skin_index_pcr_team_stats = 15,
    skin_index_pcr_player_stats = 16,
    skin_index_pcr_kill_stats = 17,
    skin_index_pcr_pvp_stats = 18,
    skin_index_pcr_medal_stats = 19,
    skin_index_matchmaking_progress = 20,
    skin_index_default_5 = 21,
    skin_index_default_6 = 22,
    skin_index_advanced_settings_list = 23,
    skin_index_live_game_browser = 24,
    skin_index_default_wide = 25,
    skin_index_unused26 = 26,
    skin_index_unused27 = 27,
    skin_index_unused28 = 28,
    skin_index_unused29 = 29,
    skin_index_unused30 = 30,
    skin_index_unused31 = 31,
};

// This is a stupid enum but it's sized as a short in the original game so this is how things have to be
enum e_boolean_value : short
{
    boolean_value_false = 0,
    boolean_value_true = 1
};

enum e_value_type : short
{
    value_type_integer_number = 0,
    value_type_floating_point_number = 1,
    value_type_boolean = 2,
    value_type_text_string = 3
};

#define k_maximum_text_value_pairs_per_block 100
struct s_text_value_pair_reference_UNUSED
{
    // Explaination("OBSOLETE", "this is all obsolete")

    e_value_type value_type;

    // Explaination("Value", "Enter the value in the box corresponding to the value type you specified above")    
    e_boolean_value boolean_value;

    int integer_value;
    float fp_value;
    string_id text_value_stringid;
    /* Explaination("Text Label", "This is text string associated with data when it has the value specified above.
    The string comes from the screen's string list tag.")*/
    string_id text_label_stringid;
};
TAG_BLOCK_SIZE_ASSERT(s_text_value_pair_reference_UNUSED, 20);

#define k_max_list_reference_block_count 1
struct s_list_reference
{
    e_list_reference_flags flags;
    e_skin_index skin_index;
    short num_visible_items;
    point2d bottom_left;

    e_animation_index animation_index;
    short intro_animation_delay_milliseconds;

    // Explaination("UNUSED", "This is unused")
    tag_block<s_text_value_pair_reference_UNUSED> uNUSED;
};
TAG_BLOCK_SIZE_ASSERT(s_list_reference, 24);

enum e_table_view_list_reference_flags : int
{
    table_view_list_reference_flag_unused = FLAG(0),
};

#define k_maximum_number_of_table_view_list_cell_blocks 8
struct table_view_list_item_reference_block
{
    e_text_flags text_flags;
    short cell_width;
    short pad;
    point2d bitmap_topleft_if_there_is_a_bitmap;

    // bitm
    tag_reference bitmap_tag;
    string_id string;
    short renderDepthBias;
    short pad2;
};
TAG_BLOCK_SIZE_ASSERT(table_view_list_item_reference_block, 0x1C);

enum e_table_view_list_row_reference_flags : int
{
    table_view_list_row_reference_flag_unused = FLAG(0),
};

#define k_maximum_number_of_table_view_list_row_blocks 16
struct s_table_view_list_row_reference_OBSOLETE
{
    e_table_view_list_row_reference_flags flags;
    short row_height;
    short pad;
    tag_block<table_view_list_item_reference_block> rowCells;
};
TAG_BLOCK_SIZE_ASSERT(s_table_view_list_row_reference_OBSOLETE, 16);

#define k_maximum_number_of_table_view_list_blocks 1
struct s_table_view_list_reference_OBSOLETE
{
    e_table_view_list_reference_flags flags;
    e_animation_index animation_index;
    short intro_animation_delay_milliseconds;

    e_custom_font custom_font;
    short pad;
    real_argb_color text_color;
    point2d topleft;
    tag_block<s_table_view_list_row_reference_OBSOLETE> table_rows;
};
TAG_BLOCK_SIZE_ASSERT(s_table_view_list_reference_OBSOLETE, 40);

#define k_maximum_number_of_text_blocks 64
struct s_text_block_reference
{
    e_text_flags text_flags;
    e_animation_index animation_index;
    short intro_animation_delay_milliseconds;
    short pad0;

    e_custom_font custom_font;
    real_argb_color text_color;
    rectangle2d text_bounds;
    string_id string;
    short render_depth_bias;
    short pad1;
};
TAG_BLOCK_SIZE_ASSERT(s_text_block_reference, 44);

enum e_bitmap_block_reference_flags : int
{
    bitmap_block_reference_flag_ignore_for_list_skin_size_calculation = FLAG(0),
    bitmap_block_reference_flag_swap_on_relative_list_position = FLAG(1),
    bitmap_block_reference_flag_render_as_progress_bar = FLAG(2)
};

enum e_bitmap_blend_method : short
{
    bitmap_blend_method_standard = 0,
    bitmap_blend_method_multiply = 1,
    bitmap_blend_method_unused = 2,
};

#define k_maximum_number_of_bitmap_blocks 64
struct s_bitmap_block_reference
{
    e_bitmap_block_reference_flags flags;
    e_animation_index animation_index;
    short intro_animation_delay_milliseconds;
    e_bitmap_blend_method bitmap_blend_method;
    short initial_sprite_frame;
    point2d topleft;
    float horiz_texture_wrapssecond;
    float vert_texture_wrapssecond;

    // bitm
    tag_reference bitmap_tag;
    short render_depth_bias;
    short pad;
    float sprite_animation_speed_fps;
    point2d progress_bottomleft;
    string_id string_identifier;
    real_vector2d progress_scale;
};
TAG_BLOCK_SIZE_ASSERT(s_bitmap_block_reference, 56);

enum e_ui_model_scene_reference_flags : int
{
    ui_model_scene_reference_flag_unused = FLAG(0)
};

#define k_maximum_lights_per_ui_scene 8
struct s_ui_light_reference
{
    static_string32 name;
};
TAG_BLOCK_SIZE_ASSERT(s_ui_light_reference, 32);

#define k_maximum_objects_per_ui_scene 32
struct s_ui_object_reference
{
    static_string32 name;
};
TAG_BLOCK_SIZE_ASSERT(s_ui_object_reference, 32);

#define k_maximum_object_scenes_per_screen 32
struct s_ui_model_scene_reference
{
    /* Explaination("NOTE on coordinate systems", "Halo y-axis=ui z-axis, and Halo z-axis=ui y-axis.
    As a convention, let's always place objects in the ui scenario such that
    they are facing in the '-y' direction, and the camera such that is is
    facing the '+y' direction.This way the ui animation for models(which
    gets applied to the camera) will always be consisitent.")*/

    e_ui_model_scene_reference_flags flags;
    e_animation_index animation_index;

    short intro_animation_delay_milliseconds;
    short render_depth_bias;
    short pad;

    tag_block<s_ui_object_reference> objects;

    tag_block<s_ui_light_reference> lights;

    real_vector3d animation_scale_factor;
    real_point3d camera_position;
    float fov_degress;
    rectangle2d ui_viewport;
    string_id unused_intro_anim;
    string_id unused_outro_anim;
    string_id unused_ambient_anim;
};
TAG_BLOCK_SIZE_ASSERT(s_ui_model_scene_reference, 76);

#define k_maximum_text_value_pairs_per_block 100
struct s_text_value_pair_block_UNUSED
{
    // Explaination("OBSOLETE", "this is all obsolete")
    static_string32 name;
    tag_block<s_text_value_pair_reference_UNUSED> text_value_pairs;
};
TAG_BLOCK_SIZE_ASSERT(s_text_value_pair_block_UNUSED, 40);

enum e_hud_block_reference_flags : int
{
    hud_block_reference_flag_ignore_for_list_skin_size = FLAG(0),
    hud_block_reference_flag_needs_valid_rank = FLAG(1)
};

#define k_maximum_number_of_hud_blocks 64
struct s_hud_block_reference
{
    e_hud_block_reference_flags flags;

    e_animation_index animation_index;
    short intro_animation_delay_milliseconds;
    short render_depth_bias;
    short starting_bitmap_sequence_index;

    // bitm
    tag_reference bitmap;
    // shad
    tag_reference shader;

    rectangle2d bounds;
};
TAG_BLOCK_SIZE_ASSERT(s_hud_block_reference, 36);

enum class e_table_order : char
{
    table_order_row_major = 0,
    table_order_column_major = 1,
};

#define k_maximum_number_of_player_blocks 64
struct s_player_block_reference
{
    int pad;

    // skin
    tag_reference skin;
    point2d bottomleft;
    e_table_order table_order;
    byte maximum_player_count;
    byte row_count;
    byte column_count;
    short row_height;
    short column_width;
}; 
TAG_BLOCK_SIZE_ASSERT(s_player_block_reference, 24);

#define k_maximum_number_of_window_pane_tag_blocks 16
struct s_window_pane_reference
{
    short pad;
    e_animation_index animation_index;

    // Explaination("Button Definitions", "If the pane contains buttons, define them here")
    tag_block<s_button_widget_reference> buttons;

    // Explaination("List Definition", "If the pane contains a list, define it here")
    tag_block<s_list_reference> list_block;

    // Explaination("OBSOLETE Table View Definition", "If the pane contains a table-view, define it here")
    tag_block<s_table_view_list_reference_OBSOLETE> table_view;

    // Explaination("Flavor Item Blocks", "Define additional flavor items here")
    tag_block<s_text_block_reference> text_blocks;

    tag_block<s_bitmap_block_reference> bitmap_blocks;

    tag_block<s_ui_model_scene_reference> model_scene_blocks;

    // Explaination("UNUSED", "these are all OBSOLETE")
    tag_block<s_text_value_pair_block_UNUSED> textvalue_blocks;

    tag_block<s_hud_block_reference> hud_blocks;

    tag_block<s_player_block_reference> player_blocks;
};
TAG_BLOCK_SIZE_ASSERT(s_window_pane_reference, 0x4C);

struct s_user_interface_screen_widget_definition : TagGroup<'wgit'>
{
    /* Explaination("Notes on screen widgets:", 
    "- the widget coordinate system is a left-handed system (+x to the right, +y up, +z into the screen)
    with the origin centered in the display(regardless of display size)
    - for widget component placement, all coordinates you define in the tag specifiy the object's
    placement prior to the application of any animation
    - all coordinates you define are local to that object
    - all text specific to objects in the screen comes from the screen's string list tag
    all of the string indices you may need to specify will refer to the screen's string list tag
    - a pane may contain either buttons OR a list OR a table - view, but never a combination of those
    (widget won't function correctly if you try that)
    - all text is centered unless you specify otherwise")*/

    // Explaination("Flags", "Set misc. screen behavior here")

    e_screen_widget_flags flags;
    e_screen_id screen_id;

    // Explaination("Button Key", 
    // "The labels here are just a guide; the actual string used comes from the Nth position of this button key entry as found in the ui globals button key string list tag")
    e_button_key_type button_key_type;

    // Explaination("Default Text Color", "Any ui elements that don't explicitly set a text color will use this color")
    real_argb_color text_color;
    // Explaination("Screen Text", "All text specific to this screen")
    // TagReference("unic")
    tag_reference string_list_tag;
    // Explaination("Panes", "Define the screen's panes here (normal screens have 1 pane, tab-view screens have 2+ panes)")
    tag_block<s_window_pane_reference> panes;
};