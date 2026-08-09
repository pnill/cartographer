#pragma once
#include "input/controllers.h"

/* constants */

enum
{
	k_maximum_number_of_active_screens = 36,
};

/* enums */

enum e_user_interface_render_window
{
	_first_render_window,
	_window_0 = _first_render_window,
	_window_1,
	_window_2,
	_window_3,
	_window_4,
	k_number_of_render_windows,
	k_no_window = NONE,
	k_any_window = 255
};

enum e_event_type
{
	_user_interface_event_type0 = 0,
	_user_interface_event_type_tab_up,
	_user_interface_event_type_tab_left,
	_user_interface_event_type_tab_down,
	_user_interface_event_type_tab_right,
	_user_interface_event_type_gamepad_button_pressed,
	_user_interface_event_type_mouse_button_left_click,
	_user_interface_event_type_keyboard_button_pressed,
	_user_interface_event_type8,
	_user_interface_event_type9,
	_user_interface_event_type_mouse_scroll_up,
	_user_interface_event_type_mouse_scroll_down,
	_user_interface_event_type_automation,
	_user_interface_event_type13,
	_user_interface_event_type14,
	_user_interface_event_type15,
};

// TODO : recheck these
enum e_user_interface_controller_component
{
	_controller_component_button_a = 0,
	_controller_component_button_b,
	_controller_component_button_x,
	_controller_component_button_y,
	_controller_component_button_black,
	_controller_component_button_white,
	_controller_component_button_left_trigger,
	_controller_component_button_right_trigger,
	_controller_component_button_dpad_up,
	_controller_component_button_dpad_down,
	_controller_component_button_dpad_left,
	_controller_component_button_dpad_right,
	_controller_component_button_start,
	_controller_component_button_back,
	_controller_component_button_left_thumb,
	_controller_component_button_right_thumb,
	_controller_component_any_stick,



	// keyboard_component are similar to Virtual KeyCodes (VK_KEY)
	_user_interface_keyboard_component_button_backspace = 0x8,
	_user_interface_keyboard_component_button_tab = 0x9,
	_user_interface_keyboard_component_button_enter = 0xD,
	_user_interface_keyboard_component_button_pause_break = 0x13,
	_user_interface_keyboard_component_button_capslock = 0x14,
	_user_interface_keyboard_component_button_escape = 0x1B,
	_user_interface_keyboard_component_button_space = 0x20,
	_user_interface_keyboard_component_button_page_up = 0x21,
	_user_interface_keyboard_component_button_page_down = 0x22,
	_user_interface_keyboard_component_button_end = 0x23,

	_user_interface_keyboard_component_button_left = 0x25,
	_user_interface_keyboard_component_button_up = 0x26,
	_user_interface_keyboard_component_button_right = 0x27,
	_user_interface_keyboard_component_button_down = 0x28,
	_user_interface_keyboard_component_button_insert = 0x2D,
	_user_interface_keyboard_component_button_delete = 0x2E,


	_user_interface_keyboard_component_button_numeric_zero = 0x30,
	_user_interface_keyboard_component_button_numeric_two = 0x32,
	// ....so on
	_user_interface_keyboard_component_button_numeric_nine = 0x39,


	_user_interface_keyboard_component_button_letter_a = 0x41,
	_user_interface_keyboard_component_button_letter_b = 0x42,
	// ....so on
	_user_interface_keyboard_component_button_letter_z = 0x5A,


	_user_interface_keyboard_component_button_f1 = 0x70,
	_user_interface_keyboard_component_button_f2 = 0x71,
	// ....so on
	_user_interface_keyboard_component_button_f12 = 0x7B,



	// ....special keys
	_user_interface_keyboard_component_button_numlock = 0x90,
	_user_interface_keyboard_component_button_left_shift = 0xA0,
	_user_interface_keyboard_component_button_right_shift = 0xA1,
	_user_interface_keyboard_component_button_left_control = 0xA2,
	_user_interface_keyboard_component_button_right_control = 0xA3,
	_user_interface_keyboard_component_button_left_alt = 0xA4,
	_user_interface_keyboard_component_button_right_alt = 0xA5,
	_user_interface_keyboard_component_button_tilde = 0x107,
	_user_interface_keyboard_component_button_minus = 0x108,
	_user_interface_keyboard_component_button_comma = 0x10E,
	_user_interface_keyboard_component_button_period = 0x10E,
	_user_interface_keyboard_component_button_backslash = 0x110,
	_user_interface_keyboard_component_button_equals = 0x112,
	_user_interface_keyboard_component_button_multiple = 0x115,
	_user_interface_keyboard_component_button_numeric_one = 0x117,
};

//TODO : figure out proper place to keep this enum
enum e_ui_error_types
{
	_ui_error_unknown = 0,
	_ui_error_generic,
	_ui_error_generic_networking,
	_ui_error_system_link_generic_join_failure,
	_ui_error_system_link_no_network_connection,
	_ui_error_system_link_connection_lost,
	_ui_error_network_game_oos,
	_ui_error_xbox_live_sign_out_confirmation,
	_ui_error_confirm_revert_to_last_save,
	_ui_error_confirm_quit_without_save,
	_ui_error_confirm_delete_player_profile,
	_ui_error_confirm_delete_variant,
	_ui_error_player_profile_creation_failed,
	_ui_error_variant_profile_creation_failed,
	_ui_error_playlist_creation_failed,
	_ui_error_core_file_load_failed,
	_ui_error_mu_removed_during_player_profile_save,
	_ui_error_mu_removed_during_variant_save,
	_ui_error_mu_removed_during_playlist_save,
	_ui_error_message_saving_to_mu,
	_ui_error_message_saving_file,
	_ui_error_message_creating_player_profile,
	_ui_error_message_creating_variant_profile,
	_ui_error_message_saving_checkpoint,
	_ui_error_failed_to_load_player_profile,
	_ui_error_failed_to_load_variant,
	_ui_error_failed_to_load_playlist,
	_ui_error_failed_to_load_save_game,
	_ui_error_controller1_removed,
	_ui_error_controller2_removed,
	_ui_error_controller3_removed,
	_ui_error_controller4_removed,
	_ui_error_need_more_free_blocks_to_save,
	_ui_error_maximum_saved_game_files_already_exist,
	_ui_error_dirty_disk,
	_ui_error_xblive_cannot_access_service,
	_ui_error_xblive_title_update_required,
	_ui_error_xblive_servers_too_busy,
	_ui_error_xblive_duplicate_logon,
	_ui_error_xblive_account_management_required,
	_ui_error_warning_xblive_recommended_messages_available,
	_ui_error_xblive_invalid_match_session,
	_ui_error_warning_xblive_poor_network_performance,
	_ui_error_not_enough_open_slots_to_join_match_session,
	_ui_error_xblive_corrupt_download_content,
	_ui_error_confirm_xblive_corrupt_saved_game_file_removal,
	_ui_error_xblive_invalid_user_account,
	_ui_error_confirm_boot_clan_member,
	_ui_error_confirm_controller_sign_out,
	_ui_error_beta_xblive_service_qos_report,
	_ui_error_beta_feature_disabled,
	_ui_error_beta_network_connection_required,
	_ui_error_confirm_friend_removal,
	_ui_error_confirm_boot_to_dash,
	_ui_error_confirm_launch_xdemos,
	_ui_error_confirm_exit_game_session,
	_ui_error_xblive_connection_to_xbox_live_lost,
	_ui_error_xblive_message_send_failure,
	_ui_error_network_link_lost,
	_ui_error_network_link_required,
	_ui_error_xblive_invalid_passcode,
	_ui_error_join_aborted,
	_ui_error_join_session_not_found,
	_ui_error_join_qos_failure,
	_ui_error_join_data_decode_failure,
	_ui_error_join_game_full,
	_ui_error_join_game_closed,
	_ui_error_join_version_mismatch,
	_ui_error_join_failed_unknown_reason,
	_ui_error_join_failed_friend_in_matchmade_game,
	_ui_error_player_profile_name_must_be_unique,
	_ui_error_variant_name_must_be_unique,
	_ui_error_playlist_name_must_be_unique,
	_ui_error_saved_film_name_must_be_unique,
	_ui_error_no_free_slots_player_profile,
	_ui_error_no_free_slots_variant,
	_ui_error_no_free_slots_playlist,
	_ui_error_no_free_slots_saved_film,
	_ui_error_need_more_space_for_player_profile,
	_ui_error_need_more_space_for_variant,
	_ui_error_need_more_space_for_playlist,
	_ui_error_need_more_space_for_saved_film,
	_ui_error_cannot_set_privileges_on_member_whose_data_not_known,
	_ui_error_cant_delete_default_profile,
	_ui_error_cant_delete_default_variant,
	_ui_error_cant_delete_default_playlist,
	_ui_error_cant_delete_default_saved_film,
	_ui_error_cant_delete_profile_in_use,
	_ui_error_player_profile_name_must_have_alphanumeric_characters,
	_ui_error_variant_name_must_have_alphanumeric_characters,
	_ui_error_playlist_name_must_have_alphanumeric_characters,
	_ui_error_saved_film_name_must_have_alphanumeric_characters,
	_ui_error_teams_not_a_member,
	_ui_error_teams_insufficient_privileges,
	_ui_error_teams_server_busy,
	_ui_error_teams_team_full,
	_ui_error_teams_member_pending,
	_ui_error_teams_too_many_requests,
	_ui_error_teams_user_already_exists,
	_ui_error_teams_user_not_found,
	_ui_error_teams_user_teams_full,
	_ui_error_teams_no_task,
	_ui_error_teams_too_many_teams,
	_ui_error_teams_team_already_exists,
	_ui_error_teams_team_not_found,
	_ui_error_teams_name_contains_bad_words,
	_ui_error_teams_description_contains_bad_words,
	_ui_error_teams_motto_contains_bad_words,
	_ui_error_teams_url_contains_bad_words,
	_ui_error_teams_no_admin,
	_ui_error_teams_cannot_set_privileges_on_member_whose_data_not_known,
	_ui_error_live_unknown,
	_ui_error_confirm_delete_profile,
	_ui_error_confirm_delete_playlist,
	_ui_error_confirm_delete_saved_film,
	_ui_error_confirm_live_sign_out,
	_ui_error_confirm_confirm_friend_removal,
	_ui_error_confirm_promotion_to_superuser,
	_ui_error_warn_no_more_clan_superusers,
	_ui_error_confirm_corrupt_profile,
	_ui_error_confirm_xbox_live_sign_out,
	_ui_error_confirm_corrupt_game_variant,
	_ui_error_confirm_leave_clan,
	_ui_error_confirm_corrupt_playlist,
	_ui_error_cant_join_gameinvite_without_signon,
	_ui_confirm_proceed_to_crossgame_invite,
	_ui_confirm_decline_crossgame_invite,
	_ui_warn_insert_cd_for_crossgame_invite,
	_ui_error_need_more_space_for_saved_game,
	_ui_error_saved_game_cannot_be_loaded,
	_ui_error_confirm_controller_signout_with_guests,
	_ui_error_warning_party_closed,
	_ui_error_warning_party_required,
	_ui_error_warning_party_full,
	_ui_error_warning_player_in_mm_game,
	_ui_error_xblive_failed_to_sign_in,
	_ui_error_cant_sign_out_master_with_guests,
	_ui_error_this_dot_command_is_obsolete,
	_ui_error_this_has_not_been_unlocked,
	_ui_error_confirm_leave_lobby,
	_ui_error_confirm_party_leader_leave_matchmaking,
	_ui_error_confirm_single_box_leave_matchmaking,
	_ui_error_clan_name_not_valid,
	_ui_error_player_list_full,
	_ui_error_recipient_has_blocked_you,
	_ui_error_friend_pending,
	_ui_error_too_many_requests,
	_ui_error_player_already_in_list,
	_ui_error_gamertag_not_found,
	_ui_error_cannot_message_self,
	_ui_error_warning_last_overlord_cant_leave_clan,
	_ui_error_confirm_boot_player,
	_ui_error_confirm_party_member_leave_pcr,
	_ui_error_cannot_sign_in_during_countdown,
	_ui_error_xblive_invalid_user,
	_ui_error_xblive_user_not_authorized,
	_ui_error_OBSOLETE,
	_ui_error_OBSOLETE2,
	_ui_error_xblive_banned_xbox,
	_ui_error_xblive_banned_user,
	_ui_error_xblive_banned_title,
	_ui_error_confirm_exit_game_session_leader,
	_ui_error_message_objectionable_content,
	_ui_error_confirm_enter_downloader,
	_ui_error_confirm_block_user,
	_ui_error_confirm_negative_feedback,
	_ui_error_confirm_change_clan_member_level,
	_ui_error_blank_gamertag,
	_ui_error_confirm_save_and_exit_campaign,
	_ui_error_cant_join_during_matchmaking,
	_ui_error_confirm_restart_level,
	_ui_error_matchmaking_failure_generic,
	_ui_error_matchmaking_failure_missing_content,
	_ui_error_matchmaking_failure_aborted,
	_ui_error_matchmaking_failure_membership_changed,
	_ui_error_confirm_end_game_session,
	_ui_error_confirm_exit_game_session_only_player,
	_ui_error_confirm_exit_game_session_xbox_live_ranked_leader,
	_ui_error_confirm_exit_game_session_xbox_live_ranked,
	_ui_error_confirm_exit_game_session_xbox_live_leader,
	_ui_error_confirm_exit_game_session_xbox_live_only_player,
	_ui_error_confirm_exit_game_session_xbox_live,
	_ui_error_recipient_list_full,
	_ui_error_confirm_exit_campaign,
	_ui_error_xblive_connection_to_xbox_live_lost_save_and_quit,
	_ui_error_booted_from_session,
	_ui_error_confirm_exit_game_session_xbox_live_guest,
	_ui_error_confirm_exit_game_session_xbox_live_ranked_only_player,
	_ui_error_confirm_exit_game_session_xbox_live_unranked_only_player,
	_ui_error_confirm_exit_game_session_xbox_live_unranked_leader,
	_ui_error_confirm_exit_game_session_xbox_live_unranked,
	_ui_error_cant_join_friend_while_in_matchmade_game,
	_ui_error_map_load_failure,
	_ui_error_confirm_campaign_without_achievements,
	_ui_error_no_live_menu_branch_without_signin,
	_ui_error_map_out_of_hard_disk_space,
	_ui_error_device_not_supported,
	_ui_error_achievements_interrupted,
	_ui_error_confirm_lose_progress,
	_ui_error_beta_achievements_disabled,
	_ui_error_cannot_connect_versions_wrong,
	_ui_error_confirm_booted_from_session,
	_ui_error_confirm_boot_player_from_squad,
	_ui_error_confirm_leave_system_link_lobby,
	_ui_error_confirm_party_member_leave_matchmaking,
	_ui_error_confirm_quit_single_player,
	_ui_error_controller_removed,
	_ui_error_download_in_progress,
	_ui_error_download_fail,
	_ui_error_failed_to_load_map,
	_ui_error_feature_requires_gold,
	_ui_error_keyboard_mapping,
	_ui_error_keyboard_removed,
	_ui_error_live_game_unavailable,
	_ui_error_map_missing,
	_ui_error_matchmaking_failed_generic,
	_ui_error_matchmaking_failed_missing_content,
	_ui_error_mouse_removed,
	_ui_error_party_not_all_on_live,
	_ui_error_party_subnet_not_shared,
	_ui_error_required_game_update,
	_ui_error_saved_game_cannot_be_saved,
	_ui_error_sound_microphone_not_supported,
	_ui_error_system_link_direct_IP,
	_ui_error_text_chat_muted,
	_ui_error_text_chat_parental_controls,
	_ui_error_update_start,
	_ui_error_update_fail,
	_ui_error_update_fail_blocks,
	_ui_error_update_exists,
	_ui_error_insert_original,
	_ui_error_update_fail_network_lost,
	_ui_error_update_mp_out_of_sync,
	_ui_error_update_must_upgrade,
	_ui_error_voice_gold_required,
	_ui_error_voice_parental_controls,
	_ui_error_warning_xblive_poor_network_perofrmance,
	_ui_error_you_missing_map,
	_ui_error_someone_missing_map,
	_ui_error_tnp_no_source,
	_ui_error_tnp_disk_read,
	_ui_error_tnp_no_engine_running,
	_ui_error_tnp_signature_verification,
	_ui_error_tnp_drive_removed,
	_ui_error_tnp_disk_full,
	_ui_error_tnp_permissions,
	_ui_error_tnp_unknown,
	_ui_error_continue_install,
	_ui_error_cancel_install,
	_ui_error_confirm_upsell_gold,
	_ui_error_add_to_favorites,
	_ui_error_remove_from_favorites,
	_ui_error_updating_favorites,
	_ui_error_choose_exisiting_checkpoint_location,
	_ui_error_choose_new_checkpoint_location_checkpoints_exist_on_live_and_locally,
	_ui_error_choose_new_checkpoint_location_checkpoints_exist_on_live,
	_ui_error_choose_new_checkpoint_location_checkpoints_exist_locally,
	_ui_error_download_map,
	_ui_error_want_to_download_map,
	_ui_error_ok_download_map,
	_ui_error_cancel_download_map,
	_ui_error_not_gold_no_map_download,
	_ui_error_map_download_connection_lost,
	_ui_error_map_download_collision,
	_ui_error_map_download_disk_write_error,
	_ui_error_matchmaking_failed_no_games,
	_ui_error_matchmaking_failed_timeout,
	_ui_error_live_checkpoint_connection_dropped,
	_ui_error_live_checkpoint_hash_mismatch,
	_ui_error_join_gold_game_not_allowed,
	_ui_error_join_locked_game_not_allowed,
	_ui_error_system_link_port_in_use,
	_ui_error_invite_requires_signin,
	_ui_error_overwrite_custom_keyboard_mappings,
	_ui_error_profile_version_mismatch,
	_ui_error_profane_map_name,
	_ui_error_profane_variant_name,
	_ui_error_demo_version_no_more_for_you,
	_ui_error_no_fullscreen_res,
	_ui_error_install_not_complete,
	_ui_error_lan_fail_download_map,
	_ui_error_locater_service_failed,
	_ui_error_double_mapping_actions,
	_ui_error_no_multiplayer_achievements_for_silver,
	_ui_error_map_download_in_game,
	_ui_error_locator_service_timed_out,
	_ui_error_connection_to_host_lost,
	_ui_error_map_download_profane_name,
	
	k_last_ui_error_code = _ui_error_map_download_profane_name,
	k_ui_error_count,
};

enum e_user_interface_channel_type
{
	_user_interface_channel_type_hardware_error = 0,
	_user_interface_channel_type_game_error,
	_user_interface_channel_type_virtual_keyboard,
	_user_interface_channel_type_gameshell_dialog,
	/*_user_interface_channel_type_online_menu,*/
	// got replaced with dialog_history_channel in h2v
	_user_interface_channel_type_gameshell_dialog_history,
	_user_interface_channel_type_gameshell_screen,
	_user_interface_channel_type_gameshell_background,
	k_number_of_user_interface_channels
};

/* typedefs */

typedef void* (__cdecl* proc_ui_screen_load_cb_t)(class c_screen_parameters*);

/* structures */

struct s_event_record
{
	e_event_type type;
	e_controller_index controller;
	e_user_interface_controller_component component;
	int16 event_value; // holds e_user_interface_automation_mode during _user_interface_event_type_automation
};
ASSERT_STRUCT_SIZE(s_event_record, 16);

struct s_screen_state
{
	int32 field_0;
	int32 m_last_focused_item_order;
	int32 m_last_focused_item_index;
};

class c_screen_parameters
{
public:
	c_screen_parameters(void);

	void initialize_default_user(
		uint16 user_flags,
		e_user_interface_channel_type channel_type,
		e_user_interface_render_window window_index,
		proc_ui_screen_load_cb_t load_cb);

	void initialize_internal(
		uint16 flags,
		uint16 user_flags,
		e_user_interface_channel_type channel_type,
		e_user_interface_render_window window_index,
		s_screen_state* screen_state,
		proc_ui_screen_load_cb_t load_cb);

	void* execute_load_function()
	{
		return m_load_function(this);
	}

	int16 get_user_flags(
		void) const
	{
		return m_user_flags;
	}

	void set_user_flag(
		uint8 bit)
	{
		SET_BIT(m_user_flags, bit, true);
		return;
	}

	void set_user_flag_unsafe(
		int16 flags)
	{
		m_user_flags = flags;
		return;
	}

	e_user_interface_channel_type get_channel_type(
		void) const
	{
		return m_channel_type;
	}

	e_user_interface_render_window get_window_index(
		void) const
	{
		return m_window_index;
	}

private:
	uint16 m_flags;
	int16 m_user_flags;
	e_user_interface_channel_type m_channel_type;
	e_user_interface_render_window m_window_index;
	void* m_context;
	s_screen_state m_screen_state;
	proc_ui_screen_load_cb_t m_load_function;
};
ASSERT_STRUCT_SIZE(c_screen_parameters, 0x20);

/* prototypes */

bool __cdecl user_interface_automation_is_active(void);
uint32 __cdecl user_interface_milliseconds(void);

class c_user_interface_channel* __fastcall user_interface_get_channel(e_user_interface_render_window window_index, e_user_interface_channel_type channel_index);

bool __cdecl user_interface_error_display_allowed(void);
bool __cdecl user_interface_has_responding_controller(int32 user_index);
bool __cdecl user_interface_channel_is_busy(e_user_interface_channel_type channel_type);
bool __cdecl user_interface_back_out_from_channel_by_id(e_user_interface_channel_type channel_type, e_user_interface_render_window window_index, enum e_user_interface_screen_id id);

int32 __cdecl user_interface_get_screen_tag_index_by_id(enum e_user_interface_screen_id id);

bool __cdecl user_interface_in_screen(e_user_interface_channel_type channel_type, e_user_interface_render_window window_index, enum e_user_interface_screen_id screen_id);
bool __cdecl user_interface_error_screen_is_active(e_user_interface_channel_type channel_index, e_user_interface_render_window window_index, e_ui_error_types error_code);

void __cdecl screen_error_ok_dialog_show(e_user_interface_channel_type channel_type, e_ui_error_types ui_error_index, e_user_interface_render_window window_index, int16 user_flags, void* ok_callback, void* fallback);
void __cdecl screen_error_ok_dialog_with_custom_text(e_user_interface_channel_type channel_type, e_ui_error_types ui_error_index, e_user_interface_render_window window_index, int16 user_flags, void* ok_callback, void* fallback, const wchar_t* custom_title, const wchar_t* custom_body);

void __cdecl user_interface_error_ok_cancel_dialog_show_confirmation(e_user_interface_channel_type channel_type, e_user_interface_render_window window_index, int16 user_flags, void* ok_callback_handle, e_ui_error_types error_type);
void __cdecl user_interface_back_out_from_channel(e_user_interface_channel_type channel_type, e_user_interface_render_window window_index);
void __cdecl user_interface_enter_game_shell(int32 context);

void __cdecl render_menu_user_interface(int32 controller_index, e_user_interface_render_window render_window, rectangle2d* out_rect2d);

void __cdecl user_interface_return_to_mainmenu(bool a1);

void __cdecl user_interface_update(real32 dt);

uint32 user_interface_set_context_presence(uint32 game_mode);

#ifdef UI_DEBUG
void user_interface_debug_load_main_menu(void);

void user_interface_debug_text_bounds(bool value);

void debug_render_title_safe_bounds(bool value);

void set_debug_frame_element_bounds(bool value);

void user_interface_test_screen(const char* screen);

void user_interface_test_transition_out_console_screen(void);

void set_debug_show_screen_tag_path(bool value);

void user_interface_show_current_screen_tag(const char* path);

void debug_set_ui_beta(bool value);

void user_interface_test_error_ok(int16 id);

void user_interface_test_error_ok_cancel(int16 id);

void user_interface_test_confirmation(int16 id);
#endif

bool __cdecl user_interface_globals_is_beta_build(void);
int32 __cdecl user_interface_globals_get_game_difficulty(void);
int32 __cdecl user_interface_globals_get_edit_player_profile_index(void);
struct s_saved_game_player_profile* user_interface_globals_get_edit_player_profile(void);
enum e_scenario_type __cdecl user_interface_globals_get_map_type(void);
void __cdecl user_interface_globals_set_game_difficulty_real(int32 difficulty);
void __cdecl user_interface_globals_set_loading_from_persistent_storage(bool a1);
void __cdecl user_interface_globals_commit_edit_profile_changes(void);
void __cdecl user_interface_globals_save_profile_changes_to_disk(void);
void __cdecl user_interface_globals_finish_saving_profile_changes(void);
void __cdecl user_interface_globals_set_edit_player_profile(e_controller_index controller_index, uint32 profile_index, struct s_saved_game_player_profile* profile);
