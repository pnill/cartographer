#include "stdafx.h"
#include "user_interface.h"

#include "dialog_channel.h"
#include "game_error_dialog_channel.h"
#include "gameshell_background_channel.h"
#include "gameshell_channel.h"
#include "hardware_error_dialog_channel.h"
#include "user_interface_channel.h"
#include "user_interface_errors.h"
#include "user_interface_main_menu_music.h"
#include "user_interface_text_chat_receiver.h"
#include "user_interface_text_chat_sender.h"
#include "user_interface_widget_window.h"

#include "cache/cache_files.h"
#include "cutscene/cinematics.h"
#include "game/game.h"
#include "interface/screens/screen_error_dialog.h"
#include "main/console.h"
#include "saved_games/game_variant.h"
#include "saved_games/player_profile.h"
#include "scenario/scenario_definitions.h"

#include <XLive/xbox/xbox.h>

#include "cartographer_variant_settings.h"
#include "user_interface_networking.h"

/* macros */

#define ERROR_STRING_CREATE(_value)\
				_value,STRINGIFY(_value)

/* structures */

#pragma pack(push,1)
struct s_user_interface_globals
{
	int32 field_0;
	bool game_shell_active;
	bool render_title_safe_bounds;
	bool render_screen_tag_path;
	bool render_element_bounds;
	bool build_is_beta;
	int8 gap_9[3];
	int32 field_C;
	/*e_scenario_type*/int8 map_type;
	int8 gap_11[3];
	real32 near_clip_distance;
	real32 projection_plane_distance;
	real32 far_clip_distance;
	int32 field_20;
	int32 field_24;
	int32 field_28;
	c_gameshell_background_channel gameshell_background_channel;
	c_gameshell_channel gameshell_channel[k_number_of_render_windows];
	c_user_interface_channel dialog_channel[k_number_of_render_windows];
	c_game_error_dialog_channel game_error_channel[k_number_of_render_windows];
	c_dialog_channel dialog_history_channel[k_number_of_render_windows];
	c_hardware_error_dialog_channel hardware_errror_channel;
	c_user_interface_channel virtual_keyboard_channel;
	c_screen_widget* screen_collection[k_maximum_number_of_active_screens];
	int8 field_6A8[104];
	int32 online_task_datum;
	int32 field_714;
	int8 field_718[1694];
	int8 field_DB6[142];
	int32 field_E44;
	int32 field_E48;
	int8 gap_E4C[8];
	int32 edit_saved_game_variant_index;
	s_game_variant edit_saved_game_variant;
	int32 m_controller_index;
	int32 edit_player_profile_index;
	s_saved_game_player_profile m_player_profile;
	int8 field_2198;
	int8 gap_2199[3];
	int32 field_219C;
	int32 m_game_campaign_id;
	int32 m_game_map_id;
	int32 m_game_difficulty;
	bool load_from_persistent_storage;
	int8 field_21AD;
	int32 current_year;
	int32 field_21B2;
	int32 time_in_hours;
	int32 field_21BA;
	int8 gap_21BE[2];
	c_user_interface_main_menu_music main_menu_music;
	int32 field_21D8;
	bool xbox_live_active;
	int8 gap_21DD[3];
	c_user_interface_text_chat_sender text_chat_sender;
	c_user_interface_text_chat_receiver text_chat_receiver;
};
#pragma pack(pop)
ASSERT_STRUCT_SIZE(s_user_interface_globals, 0x3FED);

struct error_code_string_mapping
{
	e_ui_error_types error_code;
	const char* string;
};

/* constants */

static error_code_string_mapping table[k_last_ui_error_code + 1] =
{
	{ERROR_STRING_CREATE(_ui_error_unknown)},
	{ERROR_STRING_CREATE(_ui_error_generic)},
	{ERROR_STRING_CREATE(_ui_error_generic_networking)},
	{ERROR_STRING_CREATE(_ui_error_system_link_generic_join_failure)},
	{ERROR_STRING_CREATE(_ui_error_system_link_no_network_connection)},
	{ERROR_STRING_CREATE(_ui_error_system_link_connection_lost)},
	{ERROR_STRING_CREATE(_ui_error_network_game_oos)},
	{ERROR_STRING_CREATE(_ui_error_xbox_live_sign_out_confirmation)},
	{ERROR_STRING_CREATE(_ui_error_confirm_revert_to_last_save)},
	{ERROR_STRING_CREATE(_ui_error_confirm_quit_without_save)},
	{ERROR_STRING_CREATE(_ui_error_confirm_delete_player_profile)},
	{ERROR_STRING_CREATE(_ui_error_confirm_delete_variant)},
	{ERROR_STRING_CREATE(_ui_error_player_profile_creation_failed)},
	{ERROR_STRING_CREATE(_ui_error_variant_profile_creation_failed)},
	{ERROR_STRING_CREATE(_ui_error_playlist_creation_failed)},
	{ERROR_STRING_CREATE(_ui_error_core_file_load_failed)},
	{ERROR_STRING_CREATE(_ui_error_mu_removed_during_player_profile_save)},
	{ERROR_STRING_CREATE(_ui_error_mu_removed_during_variant_save)},
	{ERROR_STRING_CREATE(_ui_error_mu_removed_during_playlist_save)},
	{ERROR_STRING_CREATE(_ui_error_message_saving_to_mu)},
	{ERROR_STRING_CREATE(_ui_error_message_saving_file)},
	{ERROR_STRING_CREATE(_ui_error_message_creating_player_profile)},
	{ERROR_STRING_CREATE(_ui_error_message_creating_variant_profile)},
	{ERROR_STRING_CREATE(_ui_error_message_saving_checkpoint)},
	{ERROR_STRING_CREATE(_ui_error_failed_to_load_player_profile)},
	{ERROR_STRING_CREATE(_ui_error_failed_to_load_variant)},
	{ERROR_STRING_CREATE(_ui_error_failed_to_load_playlist)},
	{ERROR_STRING_CREATE(_ui_error_failed_to_load_save_game)},
	{ERROR_STRING_CREATE(_ui_error_controller1_removed)},
	{ERROR_STRING_CREATE(_ui_error_controller2_removed)},
	{ERROR_STRING_CREATE(_ui_error_controller3_removed)},
	{ERROR_STRING_CREATE(_ui_error_controller4_removed)},
	{ERROR_STRING_CREATE(_ui_error_need_more_free_blocks_to_save)},
	{ERROR_STRING_CREATE(_ui_error_maximum_saved_game_files_already_exist)},
	{ERROR_STRING_CREATE(_ui_error_dirty_disk)},
	{ERROR_STRING_CREATE(_ui_error_xblive_cannot_access_service)},
	{ERROR_STRING_CREATE(_ui_error_xblive_title_update_required)},
	{ERROR_STRING_CREATE(_ui_error_xblive_servers_too_busy)},
	{ERROR_STRING_CREATE(_ui_error_xblive_duplicate_logon)},
	{ERROR_STRING_CREATE(_ui_error_xblive_account_management_required)},
	{ERROR_STRING_CREATE(_ui_error_warning_xblive_recommended_messages_available)},
	{ERROR_STRING_CREATE(_ui_error_xblive_invalid_match_session)},
	{ERROR_STRING_CREATE(_ui_error_warning_xblive_poor_network_performance)},
	{ERROR_STRING_CREATE(_ui_error_not_enough_open_slots_to_join_match_session)},
	{ERROR_STRING_CREATE(_ui_error_xblive_corrupt_download_content)},
	{ERROR_STRING_CREATE(_ui_error_confirm_xblive_corrupt_saved_game_file_removal)},
	{ERROR_STRING_CREATE(_ui_error_xblive_invalid_user_account)},
	{ERROR_STRING_CREATE(_ui_error_confirm_boot_clan_member)},
	{ERROR_STRING_CREATE(_ui_error_confirm_controller_sign_out)},
	{ERROR_STRING_CREATE(_ui_error_beta_xblive_service_qos_report)},
	{ERROR_STRING_CREATE(_ui_error_beta_feature_disabled)},
	{ERROR_STRING_CREATE(_ui_error_beta_network_connection_required)},
	{ERROR_STRING_CREATE(_ui_error_confirm_friend_removal)},
	{ERROR_STRING_CREATE(_ui_error_confirm_boot_to_dash)},
	{ERROR_STRING_CREATE(_ui_error_confirm_launch_xdemos)},
	{ERROR_STRING_CREATE(_ui_error_confirm_exit_game_session)},
	{ERROR_STRING_CREATE(_ui_error_xblive_connection_to_xbox_live_lost)},
	{ERROR_STRING_CREATE(_ui_error_xblive_message_send_failure)},
	{ERROR_STRING_CREATE(_ui_error_network_link_lost)},
	{ERROR_STRING_CREATE(_ui_error_network_link_required)},
	{ERROR_STRING_CREATE(_ui_error_xblive_invalid_passcode)},
	{ERROR_STRING_CREATE(_ui_error_join_aborted)},
	{ERROR_STRING_CREATE(_ui_error_join_session_not_found)},
	{ERROR_STRING_CREATE(_ui_error_join_qos_failure)},
	{ERROR_STRING_CREATE(_ui_error_join_data_decode_failure)},
	{ERROR_STRING_CREATE(_ui_error_join_game_full)},
	{ERROR_STRING_CREATE(_ui_error_join_game_closed)},
	{ERROR_STRING_CREATE(_ui_error_join_version_mismatch)},
	{ERROR_STRING_CREATE(_ui_error_join_failed_unknown_reason)},
	{ERROR_STRING_CREATE(_ui_error_join_failed_friend_in_matchmade_game)},
	{ERROR_STRING_CREATE(_ui_error_player_profile_name_must_be_unique)},
	{ERROR_STRING_CREATE(_ui_error_variant_name_must_be_unique)},
	{ERROR_STRING_CREATE(_ui_error_playlist_name_must_be_unique)},
	{ERROR_STRING_CREATE(_ui_error_saved_film_name_must_be_unique)},
	{ERROR_STRING_CREATE(_ui_error_no_free_slots_player_profile)},
	{ERROR_STRING_CREATE(_ui_error_no_free_slots_variant)},
	{ERROR_STRING_CREATE(_ui_error_no_free_slots_playlist)},
	{ERROR_STRING_CREATE(_ui_error_no_free_slots_saved_film)},
	{ERROR_STRING_CREATE(_ui_error_need_more_space_for_player_profile)},
	{ERROR_STRING_CREATE(_ui_error_need_more_space_for_variant)},
	{ERROR_STRING_CREATE(_ui_error_need_more_space_for_playlist)},
	{ERROR_STRING_CREATE(_ui_error_need_more_space_for_saved_film)},
	{ERROR_STRING_CREATE(_ui_error_cannot_set_privileges_on_member_whose_data_not_known)},
	{ERROR_STRING_CREATE(_ui_error_cant_delete_default_profile)},
	{ERROR_STRING_CREATE(_ui_error_cant_delete_default_variant)},
	{ERROR_STRING_CREATE(_ui_error_cant_delete_default_playlist)},
	{ERROR_STRING_CREATE(_ui_error_cant_delete_default_saved_film)},
	{ERROR_STRING_CREATE(_ui_error_cant_delete_profile_in_use)},
	{ERROR_STRING_CREATE(_ui_error_player_profile_name_must_have_alphanumeric_characters)},
	{ERROR_STRING_CREATE(_ui_error_variant_name_must_have_alphanumeric_characters)},
	{ERROR_STRING_CREATE(_ui_error_playlist_name_must_have_alphanumeric_characters)},
	{ERROR_STRING_CREATE(_ui_error_saved_film_name_must_have_alphanumeric_characters)},
	{ERROR_STRING_CREATE(_ui_error_teams_not_a_member)},
	{ERROR_STRING_CREATE(_ui_error_teams_insufficient_privileges)},
	{ERROR_STRING_CREATE(_ui_error_teams_server_busy)},
	{ERROR_STRING_CREATE(_ui_error_teams_team_full)},
	{ERROR_STRING_CREATE(_ui_error_teams_member_pending)},
	{ERROR_STRING_CREATE(_ui_error_teams_too_many_requests)},
	{ERROR_STRING_CREATE(_ui_error_teams_user_already_exists)},
	{ERROR_STRING_CREATE(_ui_error_teams_user_not_found)},
	{ERROR_STRING_CREATE(_ui_error_teams_user_teams_full)},
	{ERROR_STRING_CREATE(_ui_error_teams_no_task)},
	{ERROR_STRING_CREATE(_ui_error_teams_too_many_teams)},
	{ERROR_STRING_CREATE(_ui_error_teams_team_already_exists)},
	{ERROR_STRING_CREATE(_ui_error_teams_team_not_found)},
	{ERROR_STRING_CREATE(_ui_error_teams_name_contains_bad_words)},
	{ERROR_STRING_CREATE(_ui_error_teams_description_contains_bad_words)},
	{ERROR_STRING_CREATE(_ui_error_teams_motto_contains_bad_words)},
	{ERROR_STRING_CREATE(_ui_error_teams_url_contains_bad_words)},
	{ERROR_STRING_CREATE(_ui_error_teams_no_admin)},
	{ERROR_STRING_CREATE(_ui_error_teams_cannot_set_privileges_on_member_whose_data_not_known)},
	{ERROR_STRING_CREATE(_ui_error_live_unknown)},
	{ERROR_STRING_CREATE(_ui_error_confirm_delete_profile)},
	{ERROR_STRING_CREATE(_ui_error_confirm_delete_playlist)},
	{ERROR_STRING_CREATE(_ui_error_confirm_delete_saved_film)},
	{ERROR_STRING_CREATE(_ui_error_confirm_live_sign_out)},
	{ERROR_STRING_CREATE(_ui_error_confirm_confirm_friend_removal)},
	{ERROR_STRING_CREATE(_ui_error_confirm_promotion_to_superuser)},
	{ERROR_STRING_CREATE(_ui_error_warn_no_more_clan_superusers)},
	{ERROR_STRING_CREATE(_ui_error_confirm_corrupt_profile)},
	{ERROR_STRING_CREATE(_ui_error_confirm_xbox_live_sign_out)},
	{ERROR_STRING_CREATE(_ui_error_confirm_corrupt_game_variant)},
	{ERROR_STRING_CREATE(_ui_error_confirm_leave_clan)},
	{ERROR_STRING_CREATE(_ui_error_confirm_corrupt_playlist)},
	{ERROR_STRING_CREATE(_ui_error_cant_join_gameinvite_without_signon)},
	{ERROR_STRING_CREATE(_ui_confirm_proceed_to_crossgame_invite)},
	{ERROR_STRING_CREATE(_ui_confirm_decline_crossgame_invite)},
	{ERROR_STRING_CREATE(_ui_warn_insert_cd_for_crossgame_invite)},
	{ERROR_STRING_CREATE(_ui_error_need_more_space_for_saved_game)},
	{ERROR_STRING_CREATE(_ui_error_saved_game_cannot_be_loaded)},
	{ERROR_STRING_CREATE(_ui_error_confirm_controller_signout_with_guests)},
	{ERROR_STRING_CREATE(_ui_error_warning_party_closed)},
	{ERROR_STRING_CREATE(_ui_error_warning_party_required)},
	{ERROR_STRING_CREATE(_ui_error_warning_party_full)},
	{ERROR_STRING_CREATE(_ui_error_warning_player_in_mm_game)},
	{ERROR_STRING_CREATE(_ui_error_xblive_failed_to_sign_in)},
	{ERROR_STRING_CREATE(_ui_error_cant_sign_out_master_with_guests)},
	{ERROR_STRING_CREATE(_ui_error_this_dot_command_is_obsolete)},
	{ERROR_STRING_CREATE(_ui_error_this_has_not_been_unlocked)},
	{ERROR_STRING_CREATE(_ui_error_confirm_leave_lobby)},
	{ERROR_STRING_CREATE(_ui_error_confirm_party_leader_leave_matchmaking)},
	{ERROR_STRING_CREATE(_ui_error_confirm_single_box_leave_matchmaking)},
	{ERROR_STRING_CREATE(_ui_error_clan_name_not_valid)},
	{ERROR_STRING_CREATE(_ui_error_player_list_full)},
	{ERROR_STRING_CREATE(_ui_error_recipient_has_blocked_you)},
	{ERROR_STRING_CREATE(_ui_error_friend_pending)},
	{ERROR_STRING_CREATE(_ui_error_too_many_requests)},
	{ERROR_STRING_CREATE(_ui_error_player_already_in_list)},
	{ERROR_STRING_CREATE(_ui_error_gamertag_not_found)},
	{ERROR_STRING_CREATE(_ui_error_cannot_message_self)},
	{ERROR_STRING_CREATE(_ui_error_warning_last_overlord_cant_leave_clan)},
	{ERROR_STRING_CREATE(_ui_error_confirm_boot_player)},
	{ERROR_STRING_CREATE(_ui_error_confirm_party_member_leave_pcr)},
	{ERROR_STRING_CREATE(_ui_error_cannot_sign_in_during_countdown)},
	{ERROR_STRING_CREATE(_ui_error_xblive_invalid_user)},
	{ERROR_STRING_CREATE(_ui_error_xblive_user_not_authorized)},
	{ERROR_STRING_CREATE(_ui_error_OBSOLETE)},
	{ERROR_STRING_CREATE(_ui_error_OBSOLETE2)},
	{ERROR_STRING_CREATE(_ui_error_xblive_banned_xbox)},
	{ERROR_STRING_CREATE(_ui_error_xblive_banned_user)},
	{ERROR_STRING_CREATE(_ui_error_xblive_banned_title)},
	{ERROR_STRING_CREATE(_ui_error_confirm_exit_game_session_leader)},
	{ERROR_STRING_CREATE(_ui_error_message_objectionable_content)},
	{ERROR_STRING_CREATE(_ui_error_confirm_enter_downloader)},
	{ERROR_STRING_CREATE(_ui_error_confirm_block_user)},
	{ERROR_STRING_CREATE(_ui_error_confirm_negative_feedback)},
	{ERROR_STRING_CREATE(_ui_error_confirm_change_clan_member_level)},
	{ERROR_STRING_CREATE(_ui_error_blank_gamertag)},
	{ERROR_STRING_CREATE(_ui_error_confirm_save_and_exit_campaign)},
	{ERROR_STRING_CREATE(_ui_error_cant_join_during_matchmaking)},
	{ERROR_STRING_CREATE(_ui_error_confirm_restart_level)},
	{ERROR_STRING_CREATE(_ui_error_matchmaking_failure_generic)},
	{ERROR_STRING_CREATE(_ui_error_matchmaking_failure_missing_content)},
	{ERROR_STRING_CREATE(_ui_error_matchmaking_failure_aborted)},
	{ERROR_STRING_CREATE(_ui_error_matchmaking_failure_membership_changed)},
	{ERROR_STRING_CREATE(_ui_error_confirm_end_game_session)},
	{ERROR_STRING_CREATE(_ui_error_confirm_exit_game_session_only_player)},
	{ERROR_STRING_CREATE(_ui_error_confirm_exit_game_session_xbox_live_ranked_leader)},
	{ERROR_STRING_CREATE(_ui_error_confirm_exit_game_session_xbox_live_ranked)},
	{ERROR_STRING_CREATE(_ui_error_confirm_exit_game_session_xbox_live_leader)},
	{ERROR_STRING_CREATE(_ui_error_confirm_exit_game_session_xbox_live_only_player)},
	{ERROR_STRING_CREATE(_ui_error_confirm_exit_game_session_xbox_live)},
	{ERROR_STRING_CREATE(_ui_error_recipient_list_full)},
	{ERROR_STRING_CREATE(_ui_error_confirm_exit_campaign)},
	{ERROR_STRING_CREATE(_ui_error_xblive_connection_to_xbox_live_lost_save_and_quit)},
	{ERROR_STRING_CREATE(_ui_error_booted_from_session)},
	{ERROR_STRING_CREATE(_ui_error_confirm_exit_game_session_xbox_live_guest)},
	{ERROR_STRING_CREATE(_ui_error_confirm_exit_game_session_xbox_live_ranked_only_player)},
	{ERROR_STRING_CREATE(_ui_error_confirm_exit_game_session_xbox_live_unranked_only_player)},
	{ERROR_STRING_CREATE(_ui_error_confirm_exit_game_session_xbox_live_unranked_leader)},
	{ERROR_STRING_CREATE(_ui_error_confirm_exit_game_session_xbox_live_unranked)},
	{ERROR_STRING_CREATE(_ui_error_cant_join_friend_while_in_matchmade_game)},
	{ERROR_STRING_CREATE(_ui_error_map_load_failure)},
	{ERROR_STRING_CREATE(_ui_error_confirm_campaign_without_achievements)},
	{ERROR_STRING_CREATE(_ui_error_no_live_menu_branch_without_signin)},
	{ERROR_STRING_CREATE(_ui_error_map_out_of_hard_disk_space)},
	{ERROR_STRING_CREATE(_ui_error_device_not_supported)},
	{ERROR_STRING_CREATE(_ui_error_achievements_interrupted)},
	{ERROR_STRING_CREATE(_ui_error_confirm_lose_progress)},
	{ERROR_STRING_CREATE(_ui_error_beta_achievements_disabled)},
	{ERROR_STRING_CREATE(_ui_error_cannot_connect_versions_wrong)},
	{ERROR_STRING_CREATE(_ui_error_confirm_booted_from_session)},
	{ERROR_STRING_CREATE(_ui_error_confirm_boot_player_from_squad)},
	{ERROR_STRING_CREATE(_ui_error_confirm_leave_system_link_lobby)},
	{ERROR_STRING_CREATE(_ui_error_confirm_party_member_leave_matchmaking)},
	{ERROR_STRING_CREATE(_ui_error_confirm_quit_single_player)},
	{ERROR_STRING_CREATE(_ui_error_controller_removed)},
	{ERROR_STRING_CREATE(_ui_error_download_in_progress)},
	{ERROR_STRING_CREATE(_ui_error_download_fail)},
	{ERROR_STRING_CREATE(_ui_error_failed_to_load_map)},
	{ERROR_STRING_CREATE(_ui_error_feature_requires_gold)},
	{ERROR_STRING_CREATE(_ui_error_keyboard_mapping)},
	{ERROR_STRING_CREATE(_ui_error_keyboard_removed)},
	{ERROR_STRING_CREATE(_ui_error_live_game_unavailable)},
	{ERROR_STRING_CREATE(_ui_error_map_missing)},
	{ERROR_STRING_CREATE(_ui_error_matchmaking_failed_generic)},
	{ERROR_STRING_CREATE(_ui_error_matchmaking_failed_missing_content)},
	{ERROR_STRING_CREATE(_ui_error_mouse_removed)},
	{ERROR_STRING_CREATE(_ui_error_party_not_all_on_live)},
	{ERROR_STRING_CREATE(_ui_error_party_subnet_not_shared)},
	{ERROR_STRING_CREATE(_ui_error_required_game_update)},
	{ERROR_STRING_CREATE(_ui_error_saved_game_cannot_be_saved)},
	{ERROR_STRING_CREATE(_ui_error_sound_microphone_not_supported)},
	{ERROR_STRING_CREATE(_ui_error_system_link_direct_IP)},
	{ERROR_STRING_CREATE(_ui_error_text_chat_muted)},
	{ERROR_STRING_CREATE(_ui_error_text_chat_parental_controls)},
	{ERROR_STRING_CREATE(_ui_error_update_start)},
	{ERROR_STRING_CREATE(_ui_error_update_fail)},
	{ERROR_STRING_CREATE(_ui_error_update_fail_blocks)},
	{ERROR_STRING_CREATE(_ui_error_update_exists)},
	{ERROR_STRING_CREATE(_ui_error_insert_original)},
	{ERROR_STRING_CREATE(_ui_error_update_fail_network_lost)},
	{ERROR_STRING_CREATE(_ui_error_update_mp_out_of_sync)},
	{ERROR_STRING_CREATE(_ui_error_update_must_upgrade)},
	{ERROR_STRING_CREATE(_ui_error_voice_gold_required)},
	{ERROR_STRING_CREATE(_ui_error_voice_parental_controls)},
	{ERROR_STRING_CREATE(_ui_error_warning_xblive_poor_network_perofrmance)},
	{ERROR_STRING_CREATE(_ui_error_you_missing_map)},
	{ERROR_STRING_CREATE(_ui_error_someone_missing_map)},
	{ERROR_STRING_CREATE(_ui_error_tnp_no_source)},
	{ERROR_STRING_CREATE(_ui_error_tnp_disk_read)},
	{ERROR_STRING_CREATE(_ui_error_tnp_no_engine_running)},
	{ERROR_STRING_CREATE(_ui_error_tnp_signature_verification)},
	{ERROR_STRING_CREATE(_ui_error_tnp_drive_removed)},
	{ERROR_STRING_CREATE(_ui_error_tnp_disk_full)},
	{ERROR_STRING_CREATE(_ui_error_tnp_permissions)},
	{ERROR_STRING_CREATE(_ui_error_tnp_unknown)},
	{ERROR_STRING_CREATE(_ui_error_continue_install)},
	{ERROR_STRING_CREATE(_ui_error_cancel_install)},
	{ERROR_STRING_CREATE(_ui_error_confirm_upsell_gold)},
	{ERROR_STRING_CREATE(_ui_error_add_to_favorites)},
	{ERROR_STRING_CREATE(_ui_error_remove_from_favorites)},
	{ERROR_STRING_CREATE(_ui_error_updating_favorites)},
	{ERROR_STRING_CREATE(_ui_error_choose_exisiting_checkpoint_location)},
	{ERROR_STRING_CREATE(_ui_error_choose_new_checkpoint_location_checkpoints_exist_on_live_and_locally)},
	{ERROR_STRING_CREATE(_ui_error_choose_new_checkpoint_location_checkpoints_exist_on_live)},
	{ERROR_STRING_CREATE(_ui_error_choose_new_checkpoint_location_checkpoints_exist_locally)},
	{ERROR_STRING_CREATE(_ui_error_download_map)},
	{ERROR_STRING_CREATE(_ui_error_want_to_download_map)},
	{ERROR_STRING_CREATE(_ui_error_ok_download_map)},
	{ERROR_STRING_CREATE(_ui_error_cancel_download_map)},
	{ERROR_STRING_CREATE(_ui_error_not_gold_no_map_download)},
	{ERROR_STRING_CREATE(_ui_error_map_download_connection_lost)},
	{ERROR_STRING_CREATE(_ui_error_map_download_collision)},
	{ERROR_STRING_CREATE(_ui_error_map_download_disk_write_error)},
	{ERROR_STRING_CREATE(_ui_error_matchmaking_failed_no_games)},
	{ERROR_STRING_CREATE(_ui_error_matchmaking_failed_timeout)},
	{ERROR_STRING_CREATE(_ui_error_live_checkpoint_connection_dropped)},
	{ERROR_STRING_CREATE(_ui_error_live_checkpoint_hash_mismatch)},
	{ERROR_STRING_CREATE(_ui_error_join_gold_game_not_allowed)},
	{ERROR_STRING_CREATE(_ui_error_join_locked_game_not_allowed)},
	{ERROR_STRING_CREATE(_ui_error_system_link_port_in_use)},
	{ERROR_STRING_CREATE(_ui_error_invite_requires_signin)},
	{ERROR_STRING_CREATE(_ui_error_overwrite_custom_keyboard_mappings)},
	{ERROR_STRING_CREATE(_ui_error_profile_version_mismatch)},
	{ERROR_STRING_CREATE(_ui_error_profane_map_name)},
	{ERROR_STRING_CREATE(_ui_error_profane_variant_name)},
	{ERROR_STRING_CREATE(_ui_error_demo_version_no_more_for_you)},
	{ERROR_STRING_CREATE(_ui_error_no_fullscreen_res)},
	{ERROR_STRING_CREATE(_ui_error_install_not_complete)},
	{ERROR_STRING_CREATE(_ui_error_lan_fail_download_map)},
	{ERROR_STRING_CREATE(_ui_error_locater_service_failed)},
	{ERROR_STRING_CREATE(_ui_error_double_mapping_actions)},
	{ERROR_STRING_CREATE(_ui_error_no_multiplayer_achievements_for_silver)},
	{ERROR_STRING_CREATE(_ui_error_map_download_in_game)},
	{ERROR_STRING_CREATE(_ui_error_locator_service_timed_out)},
	{ERROR_STRING_CREATE(_ui_error_connection_to_host_lost)},
	{ERROR_STRING_CREATE(_ui_error_map_download_profane_name) },

};


/* prototypes */

static s_user_interface_globals* user_interface_globals_get(void);

static const char* user_interface_error_codes_get_name(e_ui_error_types error_code);

static void ui_test_error_code(e_ui_error_types error_id, bool use_cancel, bool confirmation);

/* public code */

c_screen_parameters::c_screen_parameters(void)
{
	m_context = NULL;
	return;
}

void c_screen_parameters::initialize_default_user(
	uint16 user_flags,
	e_user_interface_channel_type channel_type,
	e_user_interface_render_window window_index,
	proc_ui_screen_load_cb_t load_cb)
{
	initialize_internal(0, user_flags, channel_type, window_index, NULL, load_cb);
	return;
}

void c_screen_parameters::initialize_internal(
	uint16 flags,
	uint16 user_flags,
	e_user_interface_channel_type channel_type,
	e_user_interface_render_window window_index,
	s_screen_state* screen_state,
	proc_ui_screen_load_cb_t load_cb)
{
	m_flags = flags;
	m_user_flags = user_flags;
	m_channel_type = channel_type;
	m_window_index = window_index;
	
	if (screen_state)
	{
		csmemcpy(&m_screen_state, screen_state, sizeof(m_screen_state));
	}
	else
	{
		csmemset(&m_screen_state, NONE, sizeof(m_screen_state));
	}
	
	m_load_function = load_cb;

	return;
}

// TODO: move to user_interface_automation.h
bool user_interface_automation_is_active(void)
{
	return false;
}

uint32 __cdecl user_interface_milliseconds(void)
{
	return INVOKE(0x2096AE, 0x0, user_interface_milliseconds);
}

c_user_interface_channel* __fastcall user_interface_get_channel(
	e_user_interface_render_window window_index,
	e_user_interface_channel_type channel_index)
{
	return INVOKE(0x209957, 0x0, user_interface_get_channel, window_index, channel_index);
}

bool __cdecl user_interface_error_display_allowed(void)
{
	//return INVOKE(0x21D441, 0x0, user_interface_error_display_allowed);

	bool result = !cinematic_in_progress();
	if (game_in_progress() && game_is_ui_shell())
		result = true;
	return result;
}

bool __cdecl user_interface_has_responding_controller(
	int32 user_index)
{
	return INVOKE(0x20A20C, 0x0, user_interface_has_responding_controller, user_index);
}

bool __cdecl user_interface_channel_is_busy(
	e_user_interface_channel_type channel_type)
{
	return INVOKE(0x20A540, 0x0, user_interface_channel_is_busy, channel_type);
}

bool __cdecl user_interface_in_screen(
	e_user_interface_channel_type channel_type,
	e_user_interface_render_window window_index,
	e_user_interface_screen_id screen_id)
{
	return INVOKE(0x20BAAA, 0x0, user_interface_in_screen, channel_type, window_index, screen_id);
}

bool __cdecl user_interface_error_screen_is_active(
	e_user_interface_channel_type channel_index,
	e_user_interface_render_window window_index,
	e_ui_error_types error_code)
{
	return INVOKE(0x20A77D, 0x0, user_interface_error_screen_is_active, channel_index, window_index, error_code);
}

void __cdecl screen_error_ok_dialog_show(
	e_user_interface_channel_type channel_type,
	e_ui_error_types ui_error_index,
	e_user_interface_render_window window_index,
	int16 user_flags,
	void* ok_callback,
	void* fallback)
{
	INVOKE(0x20E1D8, 0x0, screen_error_ok_dialog_show, channel_type, ui_error_index, window_index, user_flags, ok_callback, fallback);
	return;
}

void __cdecl screen_error_ok_dialog_with_custom_text(
	e_user_interface_channel_type channel_type,
	e_ui_error_types ui_error_index,
	e_user_interface_render_window window_index,
	int16 user_flags,
	void* ok_callback,
	void* fallback,
	const wchar_t* custom_title,
	const wchar_t* custom_body)
{
	return INVOKE(0x20E1DA, 0x0, screen_error_ok_dialog_with_custom_text, channel_type, ui_error_index, window_index, user_flags, ok_callback, fallback, custom_title, custom_body);
}

void __cdecl user_interface_error_ok_cancel_dialog_show_confirmation(
	e_user_interface_channel_type channel_type,
	e_user_interface_render_window window_index,
	int16 user_flags,
	void* ok_callback_handle,
	e_ui_error_types error_type)
{
	INVOKE(0x20E3BB, 0x0, user_interface_error_ok_cancel_dialog_show_confirmation, channel_type, window_index, user_flags, ok_callback_handle, error_type);
	return;
}

void __cdecl user_interface_back_out_from_channel(
	e_user_interface_channel_type channel_type,
	e_user_interface_render_window window_index)
{
	INVOKE(0x2096DA, 0x0, user_interface_back_out_from_channel, channel_type, window_index);
	return;
}

bool __cdecl user_interface_back_out_from_channel_by_id(
	e_user_interface_channel_type channel_type,
	e_user_interface_render_window window_index,
	e_user_interface_screen_id id)
{
	return INVOKE(0x209850, 0x0, user_interface_back_out_from_channel_by_id, channel_type, window_index, id);
}

int32 __cdecl user_interface_get_screen_tag_index_by_id(
	e_user_interface_screen_id id)
{
	return INVOKE(0x20C701, 0x0, user_interface_get_screen_tag_index_by_id, id);
}

void __cdecl user_interface_enter_game_shell(
	int32 context)
{
	INVOKE(0x20CE70, 0x0, user_interface_enter_game_shell, context);
	return;
}

void __cdecl render_menu_user_interface(
	int32 controller_index,
	e_user_interface_render_window render_window,
	rectangle2d* out_rect2d)
{
	INVOKE(0x20B697, 0x0, render_menu_user_interface, controller_index, render_window, out_rect2d);
	return;
}

void __cdecl user_interface_return_to_mainmenu(
	bool a1)
{
	INVOKE(0x20A87E, 0x0, user_interface_return_to_mainmenu, a1);
	return;
}

void __cdecl user_interface_update(
	real32 dt)
{
	static real32 user_interface_precise_accumulator_msec = 0.f;
	user_interface_precise_accumulator_msec += (dt * 1000.f) - floor(dt * 1000.f);
	if (user_interface_precise_accumulator_msec >= 1.f)
	{
		*Memory::GetAddress<int32*>(0x971900) += (int32)user_interface_precise_accumulator_msec;
		user_interface_precise_accumulator_msec -= (int32)user_interface_precise_accumulator_msec;
	}

	INVOKE(0x20CA7D, 0x0, user_interface_update, dt);

	// move to user_interface_networking_update when rewritten
	user_interface_networking_update_auto_join();
	cartographer_variant_settings_interface_update(dt);
	return;
}

uint32 user_interface_set_context_presence(
	uint32 game_mode)
{
	return XUserSetContext(0, X_CONTEXT_PRESENCE, game_mode);
}

#ifdef UI_DEBUG
void user_interface_debug_load_main_menu(void)
{
	user_interface_enter_game_shell(0);
	return;
}

void user_interface_debug_text_bounds(
	bool value)
{
	*Memory::GetAddress<bool*>(0x977370) = value;
	return;
}

void debug_render_title_safe_bounds(
	bool value)
{
	user_interface_globals_get()->render_title_safe_bounds = value;
	return;
}

void set_debug_frame_element_bounds(
	bool value)
{
	user_interface_globals_get()->render_element_bounds = value;
	return;
}

void user_interface_test_screen(
	const char* screen)
{
	// TODO: implement
	return;
}

void user_interface_test_transition_out_console_screen(void)
{
	if (user_interface_globals_get()->gameshell_channel[_window_4].active_or_incoming_screen_exists())
	{
		user_interface_globals_get()->gameshell_channel[_window_4].transition_out();
	}

	if (user_interface_globals_get()->gameshell_background_channel.active_or_incoming_screen_exists())
	{
		user_interface_globals_get()->gameshell_background_channel.transition_out();
	}
	
	return;
}

void set_debug_show_screen_tag_path(
	bool value)
{
	user_interface_globals_get()->render_screen_tag_path = value;
	return;
}

void user_interface_show_current_screen_tag(
	const char* path)
{
	char destination[128];

	e_user_interface_channel_type ui_channel = (e_user_interface_channel_type)NONE;

	csstrncpy(destination, path, NUMBEROF(destination));
	
	if (!csstrcmp(destination, "hardware") || !csstrcmp(destination, "hardware_error") || !csstrcmp(destination, ""))
	{
		ui_channel = _user_interface_channel_type_hardware_error;
	}
	else if (!csstrcmp(destination, "error") || !csstrcmp(destination, "game_error") || !csstrcmp(destination, "1"))
	{
		ui_channel = _user_interface_channel_type_game_error;
	}
	else if (!csstrcmp(destination, "keyboard") || !csstrcmp(destination, "virtual_keyboard") || !csstrcmp(destination, "2"))
	{
		ui_channel = _user_interface_channel_type_virtual_keyboard;
	}
	else if (!csstrcmp(destination, "dialog") || !csstrcmp(destination, "gameshell_dialog") || !csstrcmp(destination, "3"))
	{
		ui_channel = _user_interface_channel_type_gameshell_dialog;
	}
	else if (!csstrcmp(destination, "screen") || !csstrcmp(destination, "gameshell_screen") || !csstrcmp(destination, "5"))
	{
		ui_channel = _user_interface_channel_type_gameshell_screen;
	}
	else if (!csstrcmp(destination, "background") || !csstrcmp(destination, "gameshell_background") || !csstrcmp(destination, "6"))
	{
		ui_channel = _user_interface_channel_type_gameshell_background;
	}

	if (ui_channel != NONE)
	{
		c_user_interface_channel* channel = user_interface_get_channel(_window_4, ui_channel);
		if (channel)
		{
			c_screen_widget* screen = channel->incoming_screen_get();
			if (screen)
			{
				const e_user_interface_screen_id screen_id = screen->screen_id_get();
				if (screen_id == _screen_none)
				{
					console_printf("%d: current_id: %d", ui_channel, NONE);
				}
				else
				{
					const int32 tag_index = user_interface_get_screen_tag_index_by_id(screen_id);
					if (tag_index == NONE)
					{
						console_printf("%d: tag_index: %d");
					}
					else
					{
						console_printf("%d: %s", ui_channel, tag_get_name(tag_index));
					}
				}
			}
			else
			{
				console_printf("%d: no active screen");
			}
		}
		else
		{
			console_printf("invalid channel");
		}
	}

	return;
}

void debug_set_ui_beta(
	bool value)
{
	user_interface_globals_get()->build_is_beta = value;
	return;
}

void user_interface_test_error_ok(
	int16 id)
{
	ui_test_error_code((e_ui_error_types)id, false, false);
	return;
}

void user_interface_test_error_ok_cancel(
	int16 id)
{
	ui_test_error_code((e_ui_error_types)id, true, false);
	return;
}

void user_interface_test_confirmation(int16 id)
{
	ui_test_error_code((e_ui_error_types)id, false, true);
	return;
}
#endif


bool __cdecl user_interface_globals_is_beta_build(void)
{
	return INVOKE(0x209ED8, 0x0, user_interface_globals_is_beta_build);
}

int32 __cdecl user_interface_globals_get_game_difficulty(void)
{
	return INVOKE(0x209E98, 0x0, user_interface_globals_get_game_difficulty);
}

int32 __cdecl user_interface_globals_get_edit_player_profile_index(void)
{
	return INVOKE(0x209BA3, 0, user_interface_globals_get_edit_player_profile_index);
}

s_saved_game_player_profile* __cdecl user_interface_globals_get_edit_player_profile(void)
{
	return INVOKE(0x209B9D, 0, user_interface_globals_get_edit_player_profile);
}

e_scenario_type __cdecl user_interface_globals_get_map_type(void)
{
	return INVOKE(0x20B8BB, 0x0, user_interface_globals_get_map_type);
}

void __cdecl user_interface_globals_set_game_difficulty_real(
	int32 difficulty)
{
	INVOKE(0x209E44, 0x0, user_interface_globals_set_game_difficulty_real, difficulty);
	return;
}

void __cdecl user_interface_globals_set_loading_from_persistent_storage(
	bool a1)
{
	INVOKE(0x209E6C, 0x0, user_interface_globals_set_loading_from_persistent_storage, a1);
	return;
}

void __cdecl user_interface_globals_commit_edit_profile_changes(void)
{
	INVOKE(0x209A98, 0x0, user_interface_globals_commit_edit_profile_changes);
	return;
}

void __cdecl user_interface_globals_save_profile_changes_to_disk(void)
{
	INVOKE(0x209C3E, 0x0, user_interface_globals_save_profile_changes_to_disk);
	return;
}

void __cdecl user_interface_globals_finish_saving_profile_changes(void)
{
	INVOKE(0x209D08, 0x0, user_interface_globals_finish_saving_profile_changes);
	return;
}

void __cdecl user_interface_globals_set_edit_player_profile(
	e_controller_index controller_index,
	uint32 profile_index,
	s_saved_game_player_profile* profile)
{
	INVOKE(0x209B72, 0x0, user_interface_globals_set_edit_player_profile, controller_index, profile_index, profile);
	return;
}

/* private code */

static s_user_interface_globals* user_interface_globals_get(void)
{
	return Memory::GetAddress<s_user_interface_globals*>(0x9718E0);
}

static const char* user_interface_error_codes_get_name(
	e_ui_error_types error_code)
{
	ASSERT(VALID_INDEX(error_code, k_last_ui_error_code));
	ASSERT(table[error_code].error_code == error_code);
	return table[error_code].string;
}

static void ui_test_error_code(
	e_ui_error_types error_id,
	bool use_cancel,
	bool confirmation)
{
	if (error_id > k_last_ui_error_code)
	{
		error(_error_delayed, "error code must be between 0 & %d", (int32)k_last_ui_error_code);
	}
	else
	{
#ifdef TERMINAL_ENABLED
		console_printf("error code #%d= '%s'", error_id, user_interface_error_codes_get_name(error_id));
#endif

		if (confirmation)
		{
			user_interface_error_ok_cancel_dialog_show_confirmation(
				_user_interface_channel_type_game_error,
				_window_4,
				NONE,
				nullptr,
				error_id);
		}
		else if (use_cancel)
		{
			c_screen_error_dialog_ok_cancel::show_dialog(
				_user_interface_channel_type_game_error,
				error_id,
				_window_4,
				(uint16)NONE,
				nullptr,
				nullptr,
				0,
				0);
		}
		else
		{
			screen_error_ok_dialog_show(_user_interface_channel_type_game_error, error_id, _window_4, NONE, nullptr, nullptr);
		}
	}

	return;
}
