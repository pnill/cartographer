#pragma once
#include "user_interface.h"
#include "user_interface_widget.h"
#include "user_interface_widget_text.h"
#include "signal_slot.h"

/* macro defines */


#define K_HEADER_TEXT_BLOCK_INDEX 0
#define K_BUTTON_KEY_TEXT_BLOCK_INDEX 1
#define K_MINIMUM_NUMBER_OF_DEFAULT_SCREEN_TEXTS 2

#define TEXT_BLOCK_INDEX_TO_WIDGET_INDEX(block_idx) \
			(block_idx + K_MINIMUM_NUMBER_OF_DEFAULT_SCREEN_TEXTS)

/* enums */

// Note : this should be used instead of the one inside tag definitions
enum e_user_interface_screen_id : uint32
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

enum e_special_widgets_type
{
	_special_widget_type_0,
	_special_widget_type_1,
	_special_widget_type_2,
	_special_widget_type_3,
	_special_widget_type_4,
	_special_widget_type_5,
	_special_widget_bitmap_favourites,
	_special_widget_bitmap_arrow_up,
	_special_widget_bitmap_arrow_down,
	_special_widget_type_9,
	_special_widget_type_10,
	_special_widget_type_11,
	_special_widget_type_12,
	_special_widget_type_13,
	_special_widget_type_14,
	_special_widget_type_15,

	k_maximum_number_of_special_widgets = 16
};

/* forward declarations */

class c_button_widget;
class c_tab_view_widget;
class c_player_widget_representation;


/* structures */

// todo : need to verify
struct s_interface_expected_pane
{
	c_button_widget** expected_buttons;
	c_list_widget* expected_list;
	uint32 buttons_count;
	bool list_exists;
};
ASSERT_STRUCT_SIZE(s_interface_expected_pane, 0x10);

// todo : need to verify
struct s_interface_expected_screen_layout
{
	c_tab_view_widget* tab_view;
	int32 panes_count;
	int32 field_8;
	s_interface_expected_pane panes[6];
};
ASSERT_STRUCT_SIZE(s_interface_expected_screen_layout, 0x6C);


/* classes */

class c_screen_widget : public c_user_interface_widget
{
protected:
	e_user_interface_screen_id m_screen_id;
	e_user_interface_channel_type m_channel_type;
	e_user_interface_render_window m_window_index;
	int32 m_child_count;
	c_normal_text_widget m_header_text;
	c_normal_text_widget m_screen_button_key_text;
	int16 m_pane_index;
	bool field_9FA;
	bool field_9FB;
	bool m_disable_overlay_effect;
	bool field_9FD;
	bool field_9FE;
	bool field_9FF;
	bool field_A00;
	bool field_A01;
	c_user_interface_widget* m_special_widgets[k_maximum_number_of_special_widgets];
	c_slot1<c_screen_widget, int32> m_screen_slot;


	void destroy();
	void switch_panes(int32* pane_index_ptr);

public:
	c_screen_widget(e_user_interface_screen_id menu_id, e_user_interface_channel_type channel_type, e_user_interface_render_window window_index, uint16 user_flags);
	
	void verify_and_load_from_layout(datum widget_tag, s_interface_expected_screen_layout* expected_layout);
	void apply_new_representations_to_players(c_player_widget_representation* representations, int32 player_count);
	void* get_screen_definition();
	
	
	// c_screen_widget virtual functions

	virtual c_user_interface_widget* destructor(uint32 flags) override;
	virtual bool handle_event(s_event_record* event) override;
	virtual c_user_interface_text* get_interface() override;
	virtual bool sub_6114B9() override;

	// c_screen_widget additions

	virtual void sub_60E884();
	virtual void initialize(s_screen_parameters* parameters) = 0;
	virtual void post_initialize();
	virtual void setup_special_widgets();
	virtual c_user_interface_widget* sub_6102C5();
	virtual uint8 sub_6103D6();
	virtual int32 sub_60F1F4(s_event_record* a2);
	virtual uint8 sub_60EFC1(s_event_record* event);
	virtual int32 sub_60F081(s_event_record* a2);
	virtual int32 sub_60F151(int32 a2);
	virtual bool sub_40AD53(int32 a2);
	virtual e_user_interface_channel_type  get_channel();
	virtual e_user_interface_render_window  get_render_window();
	virtual int32 sub_60EB92(int32 a2);
	virtual void sub_60EBC2(int32 a1);
	virtual void* sub_60EC5C(s_screen_state* state);
	virtual void sub_60ECC9(s_screen_state* state);
	virtual void* load_proc() = 0;
	virtual bool overlay_effect_is_disabled();
	virtual void sub_60F2A4(uint8 bitmap_index);

private:
	template<typename T>
	static T c_screen_widget_base_vtable_get_func_ptr(DWORD idx)
	{
		return reinterpret_cast<T>(&Memory::GetAddress<void**>(0x3CF2F4)[idx]);
	}
};
ASSERT_STRUCT_SIZE(c_screen_widget, 0xA5C);

// Todo : move to proper location
void user_interface_register_screen_to_channel(c_screen_widget* new_screen, s_screen_parameters* parameters);
