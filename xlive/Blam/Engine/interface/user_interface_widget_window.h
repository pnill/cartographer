#pragma once
#include "user_interface_widget_text.h"
#include "signal_slot.h"

/* constants */

enum
{
	k_header_text_block_index = 0,
	k_button_key_text_block_index = 1,
	k_sub_header_text_block_index = 2,
	k_minimum_number_of_default_screen_texts = 2,
	k_maximum_number_of_button_keys = 4
};

/* macros */

#define TEXT_BLOCK_INDEX_TO_WIDGET_INDEX(block_idx) ((block_idx) + k_minimum_number_of_default_screen_texts)

/* enums */

enum e_user_interface_screen_id
{
	_screen_test_1 = 0,
	_screen_test_2,
	_screen_test_3,
	_screen_test_4,
	_screen_test_5,
	_screen_game_shell_background,
	_screen_main_menu,
	_screen_error_dialog_ok_cancel,
	_screen_error_dialog_ok,
	_screen_press_start_intro,
	_screen_player_profile_select,
	_screen_single_player_level_select,
	_screen_single_player_difficulty_select,
	_screen_network_squad_browser,
	_screen_multiplayer_pregame_lobby,
	_screen_custom_game_menu,
	_screen_postgame_stats,
	_screen_multiplayer_map_select,
	_screen_single_player_pause_game,
	_screen_settings,
	_screen_gamertag_select,
	_screen_gamertag_passcode_entry,
	_screen_multiplayer_protocol,
	_screen_squad_settings,
	_screen_squad_game_settings,
	_screen_squad_privacy_settings,
	_screen_y_menugameshell,
	_screen_y_menugameshell_collapsed,
	_screen_y_menuingame,
	_screen_y_menuingame_collapsed,
	_screen_4way_join_screen,
	_screen_y_menuplayer_selected_options,
	_screen_player_selected_options,
	_screen_confirmation_dialog,
	_screen_live_feedback_menu_dialog,
	_screen_live_message_type_dialog,
	_screen_voice_msg_dialog,
	_screen_stereo_faceplate,
	_screen_player_profile_edit_menu,
	_screen_pp_controller_settings,
	_screen_pp_button_settings,
	_screen_pp_thumbstick_settings,
	_screen_pp_look_sensitivity_settings,
	_screen_pp_invert_look_settings,
	_screen_pp_autolevel_settings,
	_screen_pp_handicap_settings,
	_screen_pp_high_score_rec_settings,
	_screen_pp_multiplayer_settings_menu,
	_screen_pp_profile_delete_confirmation_dlg,
	_screen_pp_choose_foreground_emblem,
	_screen_pp_choose_primary_color,
	_screen_pp_choose_secondary_color,
	_screen_pp_choose_model,
	_screen_pp_voice_settings_menu,
	_screen_pp_choose_voice_mask,
	_screen_pp_voice_thru_tv,
	_screen_pp_edit_rotation_list,
	_screen_pp_xbl_status_menu,
	_screen_pp_appear_offline,
	_screen_pp_autooffline,
	_screen_game_engine_category_listing,
	_screen_edit_slayer_menu,
	_screen_edit_koth_menu,
	_screen_edit_race_menu,
	_screen_edit_oddball_menu,
	_screen_edit_juggernaut_menu,
	_screen_edit_headhunter_menu,
	_screen_edit_ctf_menu,
	_screen_edit_assualt_menu,
	_screen_edit_slayer_score_to_win,
	_screen_edit_slayer_time_limit,
	_screen_edit_slayer_teams,
	_screen_edit_slayer_score_4_killing,
	_screen_edit_slayer_kill_in_order,
	_screen_edit_slayer_death_pt_loss,
	_screen_edit_slayer_suicide_pt_loss,
	_screen_edit_slayer_dmg_after_kill,
	_screen_edit_slayer_dmg_after_death,
	_screen_edit_slayer_speed_after_kill,
	_screen_edit_slayer_speed_after_death,
	_screen_edit_koth_score_to_win,
	_screen_edit_koth_time_limit,
	_screen_edit_koth_teams,
	_screen_edit_koth_moving_hills,
	_screen_edit_koth_uncontesed_control,
	_screen_edit_koth_xtra_dmg,
	_screen_edit_race_laps_to_win,
	_screen_edit_race_time_limit,
	_screen_edit_race_teams,
	_screen_edit_race_team_scoring,
	_screen_edit_race_type,
	_screen_edit_race_flag_order,
	_screen_edit_race_game_end_condition,
	_screen_edit_race_dmg_with_laps,
	_screen_edit_race_speed_with_laps,
	_screen_edit_oddball_time_to_win,
	_screen_edit_oddball_time_limit,
	_screen_edit_oddball_teams,
	_screen_edit_oddball_ball_spawn_count,
	_screen_edit_oddball_ball_waypoints,
	_screen_edit_oddball_damage_with_ball,
	_screen_edit_oddball_speed_with_ball,
	_screen_edit_oddball_invisibility_with_ball,
	_screen_edit_jug_score_to_win,
	_screen_edit_jug_time_limit,
	_screen_edit_jug_pts_for_killing_jugger,
	_screen_edit_jug_count,
	_screen_edit_jug_species,
	_screen_edit_jug_starting_equip,
	_screen_edit_jug_dmg,
	_screen_edit_jug_health,
	_screen_edit_jug_speed,
	_screen_edit_jug_regeneration,
	_screen_edit_jug_waypoints,
	_screen_edit_hh_score_to_win,
	_screen_edit_hh_time_limit,
	_screen_edit_hh_teams,
	_screen_edit_hh_death_pts_loss,
	_screen_edit_hh_suicide_pt_loss,
	_screen_edit_hh_speed_with_token,
	_screen_edit_hh_dropped_token_lifetime,
	_screen_edit_hh_score_multiplier,
	_screen_edit_ctf_score_to_win,
	_screen_edit_ctf_time_limit,
	_screen_edit_ctf_tie_resolution,
	_screen_edit_ctf_single_flag,
	_screen_edit_ctf_role_swapping,
	_screen_edit_ctf_flag_at_home_to_score,
	_screen_edit_ctf_flag_multi_reset,
	_screen_edit_ctf_dmg_with_flag,
	_screen_edit_ctf_speed_with_flag,
	_screen_edit_assault_score_to_win,
	_screen_edit_assault_time_limit,
	_screen_edit_assault_tie_resolution,
	_screen_edit_assault_single_flag,
	_screen_edit_assault_role_swapping,
	_screen_edit_assault_enemy_flag_at_home_to_score,
	_screen_edit_assault_flag_must_reset,
	_screen_edit_assault_dmg_with_flag,
	_screen_edit_assault_speed_with_flag,
	_screen_edit_player_number_of_lives,
	_screen_edit_player_max_health,
	_screen_edit_player_shields,
	_screen_edit_player_respawn_times,
	_screen_edit_player_count,
	_screen_edit_player_invisibility,
	_screen_edit_player_suicide_penalty,
	_screen_edit_player_friendly_fire,
	_screen_edit_item_respawn_grenades,
	_screen_edit_item_powerups,
	_screen_edit_item_weapon_set,
	_screen_edit_item_starting_equipment,
	_screen_edit_item_warthogs,
	_screen_edit_item_ghosts,
	_screen_edit_item_scorpions,
	_screen_edit_item_banshees,
	_screen_edit_item_mongeese,
	_screen_edit_item_shadows,
	_screen_edit_item_wraiths,
	_screen_edit_indicator_objectives,
	_screen_edit_indicator_players_on_motion_sensor,
	_screen_edit_indicator_invisible_players_on_motion_sensor,
	_screen_edit_indicator_friends,
	_screen_edit_indicator_enemies,
	_screen_edit_players_options,
	_screen_edit_item_options,
	_screen_edit_indicator_options,
	_screen_virtual_keyboard,
	_screen_custom_game_menu2,
	_screen_slayer_quick_options,
	_screen_koth_quick_options,
	_screen_race_quick_options,
	_screen_oddball_quick_options,
	_screen_jugger_quick_options,
	_screen_hh_quick_options,
	_screen_ctf_quick_options,
	_screen_assault_quick_options,
	_screen_pick_new_squad_leader,
	_screen_variant_editing_options_menu,
	_screen_playlist_list_settings,
	_screen_playlist_contents,
	_screen_playlist_selected_options,
	_screen_xbox_live_task_progress_dialog,
	_screen_pp_vibration_settings,
	_screen_boot_player_dialog,
	_screen_postgame_statslobby,
	_screen_xbox_live_main_menu,
	_screen_edit_terries_menu,
	_screen_edit_terries_score_to_win,
	_screen_edit_terries_time_limit,
	_screen_edit_terries_teams,
	_screen_terries_quick_options,
	_screen_xbox_live_notification_beeper,
	_screen_player_profile_select_fancy,
	_screen_saved_game_file_actions_dialog,
	_screen_multiplayer_start_menu,
	_screen_multiplayer_start_player_settings,
	_screen_multiplayer_start_handicap_settings,
	_screen_multiplayer_start_change_teams,
	_screen_multiplayer_start_admin_settings,
	_screen_multiplayer_start_controller_settings,
	_screen_multiplayer_start_voice_settings,
	_screen_multiplayer_start_online_status,
	_screen_mpalpha_legal_warning,
	_screen_squad_join_progress_dialog,
	_screen_multiplayer_alpha_postgame_legal_warning,
	_screen_multiplayer_map_selectlobby,
	_screen_multiplayer_variant_typelobby,
	_screen_multiplayer_variant_listlobby,
	_screen_loading_progress,
	_screen_matchmaking_progress,
	_screen_live_message_display,
	_screen_fadein_from_black,
	_screen_live_player_profile,
	_screen_live_clan_profile,
	_screen_live_message_send,
	_screen_friends_options_dialog,
	_screen_clan_options_dialog,
	_screen_campaign_options_dialog,
	_screen_optimatch_hoppers_fullscreen,
	_screen_playlist_listdialog,
	_screen_variant_editing_format,
	_screen_variant_quick_options_format,
	_screen_variant_param_setting_format,
	_screen_vehicle_options,
	_screen_match_options,
	_screen_player_options,
	_screen_team_options,
	_screen_game_options,
	_screen_equipment_options,
	_screen_multiple_choice_dialog,
	_screen_network_transition_progress,
	_screen_xbox_live_stats,
	_screen_pp_choose_background_emblem,
	_screen_pp_buttons_qtr,
	_screen_pp_stix_qtr,
	_screen_clan_memeber_privs,
	_screen_optimatch_hoppers_lobby,
	_screen_saved_game_file_dialog,
	_screen_extra_settings,
	_screen_error_ok_cancel_large,
	_screen_extras_enabled_dialog,
	_screen_subtitle_display,
	_screen_pp_keyboard_settings,
	_screen_pp_keyboard_settings_qtr,
	_screen_pp_invert_dual_wield,
	_screen_system_setings,
	_screen_bungie_news,
	_screen_filter_select,
	_screen_live_game_browser,
	_screen_game_details,
	_screen_multiplayer_custom_map_select,
	_screen_multiplayer_all_maps_select,
	_screen_pp_advanced_keyboard_settings,
	_screen_pp_advanced_keyboard_settings_qtr,
	_screen_restore_controller_defaults,
	_screen_video_settings,
	_screen_audio_settings,
	_screen_keyboard_settings_menu,
	_screen_pause_settings,
	_screen_keyboard_settings,
	_screen_video_settings_mp,
	_screen_audio_settings2,
	_screen_volume_settings,
	_screen_sound_quality,
	_screen_eax,
	_screen_audio_hardware_3d,
	_screen_speaker_config,
	_screen_restore_audio_defaults,
	_screen_resolution,
	_screen_aspect_ratio,
	_screen_display_mode,
	_screen_brightness_level,
	_screen_gamma_setting,
	_screen_antialiasing,
	_screen_resize_hud,
	_screen_restore_video_defaults,
	_screen_search_option_maps,
	_screen_search_option_gametype,
	_screen_search_option_variant,
	_screen_search_option_gold_only,
	_screen_search_option_dedicated_servers,
	_screen_search_option_max_players,
	_screen_search_option_favorites,
	_screen_search_option_show_full_games,
	_screen_safe_area,
	_screen_find_game_menu,
	_screen_search_options,
	_screen_unused,
	_screen_unused2,
	_screen_lod_setting,
	_screen_refresh,
	_screen_esrb_warning,
	_screen_resolution_confirmation,
	_screen_invert_kb_look,
	_screen_restore_default_keyboard_settings,
	_screen_network_adapter,
	_screen_about_dialog,
	
	k_user_interface_screen_count,
	
	_screen_none = NONE,
};

enum e_special_widgets_type
{
	_special_widget_type_0 = 0,
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

	k_maximum_number_of_special_widgets
};

/* structures */

// todo : need to verify
struct s_interface_expected_pane
{
	class c_button_widget** expected_buttons;
	class c_list_widget* expected_list;
	uint32 buttons_count;
	bool list_exists;
};
ASSERT_STRUCT_SIZE(s_interface_expected_pane, 0x10);

// todo : need to verify
struct s_interface_expected_screen_layout
{
	class c_tab_view_widget* tab_view;
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
	enum e_user_interface_channel_type m_channel_type;
	enum e_user_interface_render_window m_window_index;
	int32 m_child_count;
	c_normal_text_widget m_header_text;
	c_normal_text_widget m_screen_button_key_text;
	int16 m_pane_index;
	bool field_9FA;
	int8 field_9FB;
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
	

	e_user_interface_screen_id get_id(void) const;
	class c_text_widget* get_screen_header_text(void);
	class c_text_widget* get_screen_button_key_text(void);
	class c_text_widget* try_find_screen_text(uint32 idx);
	void verify_and_load_from_layout(datum widget_tag, s_interface_expected_screen_layout* expected_layout);
	void apply_new_representations_to_players(class c_player_widget_representation* representations, int32 player_count);
	void initialize_button_keys_text(bool add_new_child);
	void set_list_arrows_widget(c_bitmap_widget* up_arrow, c_bitmap_widget* down_arrow);
	void* get_screen_definition(void);
	void get_string_by_string_id(string_id sid, c_maximum_interface_text* out_string);
	
	
	// c_screen_widget virtual functions

	virtual ~c_screen_widget(void) = default;
	virtual bool handle_event(struct s_event_record* event) override;
	virtual c_user_interface_text* get_interface(void) override;
	virtual bool sub_6114B9(void) override;

	// c_screen_widget additions

	virtual void sub_60E884(void);
	virtual void initialize(class c_screen_parameters const* parameters) = 0;
	virtual void post_initialize(void);
	virtual void post_initialize_button_keys(void);
	virtual c_user_interface_widget* sub_6102C5(void);
	virtual uint8 sub_6103D6(void);
	virtual int32 sub_60F1F4(struct s_event_record* a2);
	virtual uint8 sub_60EFC1(struct s_event_record* event);
	virtual int32 sub_60F081(struct s_event_record* a2);
	virtual enum e_user_interface_controller_component get_component_from_button_key(int32 special_widget_index);
	virtual bool sub_40AD53(int32 a2);
	virtual e_user_interface_channel_type get_channel();
	virtual e_user_interface_render_window get_render_window();
	virtual int32 sub_60EB92(int32 a2);
	virtual void sub_60EBC2(int32 a1);
	virtual void* sub_60EC5C(struct s_screen_state* state);
	virtual void sub_60ECC9(struct s_screen_state* state);
	virtual const void* load_proc(void) const = 0;
	virtual bool overlay_effect_is_disabled(void);
	virtual void set_favourites_bitmap_visible(bool show_icon);

	e_user_interface_screen_id screen_id_get(void) const
	{
		return m_screen_id;
	}

private:
	template<typename T>
	static T c_screen_widget_base_vtable_get_func_ptr(uint32 idx)
	{
		return reinterpret_cast<T>(&Memory::GetAddress<void**>(0x3CF2F4)[idx]);
	}
};
ASSERT_STRUCT_SIZE(c_screen_widget, 0xA5C);


class c_screen_with_menu : public c_screen_widget
{
public:
	c_screen_with_menu(e_user_interface_screen_id menu_id, e_user_interface_channel_type channel_type, e_user_interface_render_window window_index, uint16 user_flags , class c_list_widget* list);
	class c_text_widget* get_screen_subheader_text();


	// c_screen_with_menu virtual functions

	// base interface overrides
	virtual ~c_screen_with_menu(void) = default;
	virtual bool handle_event(s_event_record* event) override;
	virtual class c_user_interface_widget* sub_6121F6(rectangle2d* point) override;
	virtual void initialize(c_screen_parameters const* parameters) override;

protected:
	class c_list_widget* m_child_list;

private:
	typedef c_screen_with_menu class_type;

	template<typename T>
	static T _get_vfptr_table(uint32 idx)
	{
		return reinterpret_cast<T>(&Memory::GetAddress<void**>(0x3CF3A4)[idx]);
	}
};
ASSERT_STRUCT_SIZE(c_screen_with_menu, 0xA60);

/* prototypes */

// Todo : move to proper location (user_interface.cpp)
void user_interface_register_screen_to_channel(class c_screen_widget* new_screen, class c_screen_parameters const* parameters);
