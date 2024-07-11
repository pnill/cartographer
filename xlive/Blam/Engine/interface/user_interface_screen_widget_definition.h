#pragma once
#include "cseries/cseries_strings.h"
#include "math/color_math.h"
#include "tag_files/string_id.h"
#include "tag_files/tag_block.h"
#include "tag_files/tag_reference.h"

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
    _screen_none = 0xFFFFFFFF,
    _screen_test_1 = 0x0,
    _screen_test_2 = 0x1,
    _screen_test_3 = 0x2,
    _screen_test_4 = 0x3,
    _screen_test_5 = 0x4,
    _screen_game_shell_background = 0x5,
    _screen_main_menu = 0x6,
    _screen_error_dialog_ok_cancel = 0x7,
    _screen_error_dialog_ok = 0x8,
    _screen_press_start_intro = 0x9,
    _screen_player_profile_select = 0xA,
    _screen_single_player_level_select = 0xB,
    _screen_single_player_difficulty_select = 0xC,
    _screen_network_squad_browser = 0xD,
    _screen_multiplayer_pregame_lobby = 0xE,
    _screen_custom_game_menu = 0xF,
    _screen_postgame_stats = 0x10,
    _screen_multiplayer_map_select = 0x11,
    _screen_single_player_pause_game = 0x12,
    _screen_settings = 0x13,
    _screen_gamertag_select = 0x14,
    _screen_gamertag_passcode_entry = 0x15,
    _screen_multiplayer_protocol = 0x16,
    _screen_squad_settings = 0x17,
    _screen_squad_game_settings = 0x18,
    _screen_squad_privacy_settings = 0x19,
    _screen_y_menugameshell = 0x1A,
    _screen_y_menugameshell_collapsed = 0x1B,
    _screen_y_menuingame = 0x1C,
    _screen_y_menuingame_collapsed = 0x1D,
    _screen_4way_join_screen = 0x1E,
    _screen_y_menuplayer_selected_options = 0x1F,
    _screen_player_selected_options = 0x20,
    _screen_confirmation_dialog = 0x21,
    _screen_live_feedback_menu_dialog = 0x22,
    _screen_live_message_type_dialog = 0x23,
    _screen_voice_msg_dialog = 0x24,
    _screen_stereo_faceplate = 0x25,
    _screen_player_profile_edit_menu = 0x26,
    _screen_pp_controller_settings = 0x27,
    _screen_pp_button_settings = 0x28,
    _screen_pp_thumbstick_settings = 0x29,
    _screen_pp_look_sensitivity_settings = 0x2A,
    _screen_pp_invert_look_settings = 0x2B,
    _screen_pp_autolevel_settings = 0x2C,
    _screen_pp_handicap_settings = 0x2D,
    _screen_pp_high_score_rec_settings = 0x2E,
    _screen_pp_multiplayer_settings_menu = 0x2F,
    _screen_pp_profile_delete_confirmation_dlg = 0x30,
    _screen_pp_choose_foreground_emblem = 0x31,
    _screen_pp_choose_primary_color = 0x32,
    _screen_pp_choose_secondary_color = 0x33,
    _screen_pp_choose_model = 0x34,
    _screen_pp_voice_settings_menu = 0x35,
    _screen_pp_choose_voice_mask = 0x36,
    _screen_pp_voice_thru_tv = 0x37,
    _screen_pp_edit_rotation_list = 0x38,
    _screen_pp_xbl_status_menu = 0x39,
    _screen_pp_appear_offline = 0x3A,
    _screen_pp_autooffline = 0x3B,
    _screen_game_engine_category_listing = 0x3C,
    _screen_edit_slayer_menu = 0x3D,
    _screen_edit_koth_menu = 0x3E,
    _screen_edit_race_menu = 0x3F,
    _screen_edit_oddball_menu = 0x40,
    _screen_edit_juggernaut_menu = 0x41,
    _screen_edit_headhunter_menu = 0x42,
    _screen_edit_ctf_menu = 0x43,
    _screen_edit_assualt_menu = 0x44,
    _screen_edit_slayer_score_to_win = 0x45,
    _screen_edit_slayer_time_limit = 0x46,
    _screen_edit_slayer_teams = 0x47,
    _screen_edit_slayer_score_4_killing = 0x48,
    _screen_edit_slayer_kill_in_order = 0x49,
    _screen_edit_slayer_death_pt_loss = 0x4A,
    _screen_edit_slayer_suicide_pt_loss = 0x4B,
    _screen_edit_slayer_dmg_after_kill = 0x4C,
    _screen_edit_slayer_dmg_after_death = 0x4D,
    _screen_edit_slayer_speed_after_kill = 0x4E,
    _screen_edit_slayer_speed_after_death = 0x4F,
    _screen_edit_koth_score_to_win = 0x50,
    _screen_edit_koth_time_limit = 0x51,
    _screen_edit_koth_teams = 0x52,
    _screen_edit_koth_moving_hills = 0x53,
    _screen_edit_koth_uncontesed_control = 0x54,
    _screen_edit_koth_xtra_dmg = 0x55,
    _screen_edit_race_laps_to_win = 0x56,
    _screen_edit_race_time_limit = 0x57,
    _screen_edit_race_teams = 0x58,
    _screen_edit_race_team_scoring = 0x59,
    _screen_edit_race_type = 0x5A,
    _screen_edit_race_flag_order = 0x5B,
    _screen_edit_race_game_end_condition = 0x5C,
    _screen_edit_race_dmg_with_laps = 0x5D,
    _screen_edit_race_speed_with_laps = 0x5E,
    _screen_edit_oddball_time_to_win = 0x5F,
    _screen_edit_oddball_time_limit = 0x60,
    _screen_edit_oddball_teams = 0x61,
    _screen_edit_oddball_ball_spawn_count = 0x62,
    _screen_edit_oddball_ball_waypoints = 0x63,
    _screen_edit_oddball_damage_with_ball = 0x64,
    _screen_edit_oddball_speed_with_ball = 0x65,
    _screen_edit_oddball_invisibility_with_ball = 0x66,
    _screen_edit_jug_score_to_win = 0x67,
    _screen_edit_jug_time_limit = 0x68,
    _screen_edit_jug_pts_for_killing_jugger = 0x69,
    _screen_edit_jug_count = 0x6A,
    _screen_edit_jug_species = 0x6B,
    _screen_edit_jug_starting_equip = 0x6C,
    _screen_edit_jug_dmg = 0x6D,
    _screen_edit_jug_health = 0x6E,
    _screen_edit_jug_speed = 0x6F,
    _screen_edit_jug_regeneration = 0x70,
    _screen_edit_jug_waypoints = 0x71,
    _screen_edit_hh_score_to_win = 0x72,
    _screen_edit_hh_time_limit = 0x73,
    _screen_edit_hh_teams = 0x74,
    _screen_edit_hh_death_pts_loss = 0x75,
    _screen_edit_hh_suicide_pt_loss = 0x76,
    _screen_edit_hh_speed_with_token = 0x77,
    _screen_edit_hh_dropped_token_lifetime = 0x78,
    _screen_edit_hh_score_multiplier = 0x79,
    _screen_edit_ctf_score_to_win = 0x7A,
    _screen_edit_ctf_time_limit = 0x7B,
    _screen_edit_ctf_tie_resolution = 0x7C,
    _screen_edit_ctf_single_flag = 0x7D,
    _screen_edit_ctf_role_swapping = 0x7E,
    _screen_edit_ctf_flag_at_home_to_score = 0x7F,
    _screen_edit_ctf_flag_multi_reset = 0x80,
    _screen_edit_ctf_dmg_with_flag = 0x81,
    _screen_edit_ctf_speed_with_flag = 0x82,
    _screen_edit_assault_score_to_win = 0x83,
    _screen_edit_assault_time_limit = 0x84,
    _screen_edit_assault_tie_resolution = 0x85,
    _screen_edit_assault_single_flag = 0x86,
    _screen_edit_assault_role_swapping = 0x87,
    _screen_edit_assault_enemy_flag_at_home_to_score = 0x88,
    _screen_edit_assault_flag_must_reset = 0x89,
    _screen_edit_assault_dmg_with_flag = 0x8A,
    _screen_edit_assault_speed_with_flag = 0x8B,
    _screen_edit_player_number_of_lives = 0x8C,
    _screen_edit_player_max_health = 0x8D,
    _screen_edit_player_shields = 0x8E,
    _screen_edit_player_respawn_times = 0x8F,
    _screen_edit_player_count = 0x90,
    _screen_edit_player_invisibility = 0x91,
    _screen_edit_player_suicide_penalty = 0x92,
    _screen_edit_player_friendly_fire = 0x93,
    _screen_edit_item_respawn_grenades = 0x94,
    _screen_edit_item_powerups = 0x95,
    _screen_edit_item_weapon_set = 0x96,
    _screen_edit_item_starting_equipment = 0x97,
    _screen_edit_item_warthogs = 0x98,
    _screen_edit_item_ghosts = 0x99,
    _screen_edit_item_scorpions = 0x9A,
    _screen_edit_item_banshees = 0x9B,
    _screen_edit_item_mongeese = 0x9C,
    _screen_edit_item_shadows = 0x9D,
    _screen_edit_item_wraiths = 0x9E,
    _screen_edit_indicator_objectives = 0x9F,
    _screen_edit_indicator_players_on_motion_sensor = 0xA0,
    _screen_edit_indicator_invisible_players_on_motion_sensor = 0xA1,
    _screen_edit_indicator_friends = 0xA2,
    _screen_edit_indicator_enemies = 0xA3,
    _screen_edit_players_options = 0xA4,
    _screen_edit_item_options = 0xA5,
    _screen_edit_indicator_options = 0xA6,
    _screen_virtual_keyboard = 0xA7,
    _screen_custom_game_menu2 = 0xA8,
    _screen_slayer_quick_options = 0xA9,
    _screen_koth_quick_options = 0xAA,
    _screen_race_quick_options = 0xAB,
    _screen_oddball_quick_options = 0xAC,
    _screen_jugger_quick_options = 0xAD,
    _screen_hh_quick_options = 0xAE,
    _screen_ctf_quick_options = 0xAF,
    _screen_assault_quick_options = 0xB0,
    _screen_pick_new_squad_leader = 0xB1,
    _screen_variant_editing_options_menu = 0xB2,
    _screen_playlist_list_settings = 0xB3,
    _screen_playlist_contents = 0xB4,
    _screen_playlist_selected_options = 0xB5,
    _screen_xbox_live_task_progress_dialog = 0xB6,
    _screen_pp_vibration_settings = 0xB7,
    _screen_boot_player_dialog = 0xB8,
    _screen_postgame_statslobby = 0xB9,
    _screen_xbox_live_main_menu = 0xBA,
    _screen_edit_terries_menu = 0xBB,
    _screen_edit_terries_score_to_win = 0xBC,
    _screen_edit_terries_time_limit = 0xBD,
    _screen_edit_terries_teams = 0xBE,
    _screen_terries_quick_options = 0xBF,
    _screen_xbox_live_notification_beeper = 0xC0,
    _screen_player_profile_select_fancy = 0xC1,
    _screen_saved_game_file_actions_dialog = 0xC2,
    _screen_multiplayer_start_menu = 0xC3,
    _screen_multiplayer_start_player_settings = 0xC4,
    _screen_multiplayer_start_handicap_settings = 0xC5,
    _screen_multiplayer_start_change_teams = 0xC6,
    _screen_multiplayer_start_admin_settings = 0xC7,
    _screen_multiplayer_start_controller_settings = 0xC8,
    _screen_multiplayer_start_voice_settings = 0xC9,
    _screen_multiplayer_start_online_status = 0xCA,
    _screen_mpalpha_legal_warning = 0xCB,
    _screen_squad_join_progress_dialog = 0xCC,
    _screen_multiplayer_alpha_postgame_legal_warning = 0xCD,
    _screen_multiplayer_map_selectlobby = 0xCE,
    _screen_multiplayer_variant_typelobby = 0xCF,
    _screen_multiplayer_variant_listlobby = 0xD0,
    _screen_loading_progress = 0xD1,
    _screen_matchmaking_progress = 0xD2,
    _screen_live_message_display = 0xD3,
    _screen_fadein_from_black = 0xD4,
    _screen_live_player_profile = 0xD5,
    _screen_live_clan_profile = 0xD6,
    _screen_live_message_send = 0xD7,
    _screen_friends_options_dialog = 0xD8,
    _screen_clan_options_dialog = 0xD9,
    _screen_campaign_options_dialog = 0xDA,
    _screen_optimatch_hoppers_fullscreen = 0xDB,
    _screen_playlist_listdialog = 0xDC,
    _screen_variant_editing_format = 0xDD,
    _screen_variant_quick_options_format = 0xDE,
    _screen_variant_param_setting_format = 0xDF,
    _screen_vehicle_options = 0xE0,
    _screen_match_options = 0xE1,
    _screen_player_options = 0xE2,
    _screen_team_options = 0xE3,
    _screen_game_options = 0xE4,
    _screen_equipment_options = 0xE5,
    _screen_multiple_choice_dialog = 0xE6,
    _screen_network_transition_progress = 0xE7,
    _screen_xbox_live_stats = 0xE8,
    _screen_pp_choose_background_emblem = 0xE9,
    _screen_pp_buttons_qtr = 0xEA,
    _screen_pp_stix_qtr = 0xEB,
    _screen_clan_memeber_privs = 0xEC,
    _screen_optimatch_hoppers_lobby = 0xED,
    _screen_saved_game_file_dialog = 0xEE,
    _screen_extra_settings = 0xEF,
    _screen_error_ok_cancel_large = 0xF0,
    _screen_extras_enabled_dialog = 0xF1,
    _screen_subtitle_display = 0xF2,
    _screen_pp_keyboard_settings = 0xF3,
    _screen_pp_keyboard_settings_qtr = 0xF4,
    _screen_pp_invert_dual_wield = 0xF5,
    _screen_system_setings = 0xF6,
    _screen_bungie_news = 0xF7,
    _screen_filter_select = 0xF8,
    _screen_live_game_browser = 0xF9,
    _screen_game_details = 0xFA,
    _screen_multiplayer_custom_map_select = 0xFB,
    _screen_multiplayer_all_maps_select = 0xFC,
    _screen_pp_advanced_keyboard_settings = 0xFD,
    _screen_pp_advanced_keyboard_settings_qtr = 0xFE,
    _screen_restore_controller_defaults = 0xFF,
    _screen_video_settings = 0x100,
    _screen_audio_settings = 0x101,
    _screen_keyboard_settings_menu = 0x102,
    _screen_pause_settings = 0x103,
    _screen_keyboard_settings = 0x104,
    _screen_video_settings2 = 0x105,
    _screen_audio_settings2 = 0x106,
    _screen_volume_settings = 0x107,
    _screen_sound_quality = 0x108,
    _screen_eax = 0x109,
    _screen_audio_hardware_3d = 0x10A,
    _screen_speaker_config = 0x10B,
    _screen_restore_audio_defaults = 0x10C,
    _screen_resolution = 0x10D,
    _screen_aspect_ratio = 0x10E,
    _screen_display_mode = 0x10F,
    _screen_brightness_level = 0x110,
    _screen_gamma_setting = 0x111,
    _screen_antialiasing = 0x112,
    _screen_resize_hud = 0x113,
    _screen_restore_video_defaults = 0x114,
    _screen_search_option_maps = 0x115,
    _screen_search_option_gametype = 0x116,
    _screen_search_option_variant = 0x117,
    _screen_search_option_gold_only = 0x118,
    _screen_search_option_dedicated_servers = 0x119,
    _screen_search_option_max_players = 0x11A,
    _screen_search_option_favorites = 0x11B,
    _screen_search_option_show_full_games = 0x11C,
    _screen_safe_area = 0x11D,
    _screen_find_game_menu = 0x11E,
    _screen_search_options = 0x11F,
    _screen_unused = 0x120,
    _screen_unused2 = 0x121,
    _screen_lod_setting = 0x122,
    _screen_refresh = 0x123,
    _screen_esrb_warning = 0x124,
    _screen_resolution_confirmation = 0x125,
    _screen_invert_kb_look = 0x126,
    _screen_restore_default_keyboard_settings = 0x127,
    _screen_network_adapter = 0x128,
    _screen_about_dialog = 0x129,
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
ASSERT_STRUCT_SIZE(s_button_widget_reference, 0x3C);

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
ASSERT_STRUCT_SIZE(s_text_value_pair_reference_UNUSED, 20);

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
ASSERT_STRUCT_SIZE(s_list_reference, 24);

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
ASSERT_STRUCT_SIZE(table_view_list_item_reference_block, 0x1C);

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
ASSERT_STRUCT_SIZE(s_table_view_list_row_reference_OBSOLETE, 16);

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
ASSERT_STRUCT_SIZE(s_table_view_list_reference_OBSOLETE, 40);

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
ASSERT_STRUCT_SIZE(s_text_block_reference, 44);

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
ASSERT_STRUCT_SIZE(s_bitmap_block_reference, 56);

enum e_ui_model_scene_reference_flags : int
{
    ui_model_scene_reference_flag_unused = FLAG(0)
};

#define k_maximum_lights_per_ui_scene 8
struct s_ui_light_reference
{
    static_string32 name;
};
ASSERT_STRUCT_SIZE(s_ui_light_reference, 32);

#define k_maximum_objects_per_ui_scene 32
struct s_ui_object_reference
{
    static_string32 name;
};
ASSERT_STRUCT_SIZE(s_ui_object_reference, 32);

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
ASSERT_STRUCT_SIZE(s_ui_model_scene_reference, 76);

#define k_maximum_text_value_pairs_per_block 100
struct s_text_value_pair_block_UNUSED
{
    // Explaination("OBSOLETE", "this is all obsolete")
    static_string32 name;
    tag_block<s_text_value_pair_reference_UNUSED> text_value_pairs;
};
ASSERT_STRUCT_SIZE(s_text_value_pair_block_UNUSED, 40);

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
ASSERT_STRUCT_SIZE(s_hud_block_reference, 36);

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
ASSERT_STRUCT_SIZE(s_player_block_reference, 24);

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
ASSERT_STRUCT_SIZE(s_window_pane_reference, 0x4C);

// ### TODO Finish this
struct s_user_interface_screen_widget_definition
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