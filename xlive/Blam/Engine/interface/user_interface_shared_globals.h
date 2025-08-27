#pragma once

#include "math/color_math.h"
#include "math/integer_math.h"
#include "tag_files/string_id.h"
#include "tag_files/tag_block.h"
#include "tag_files/tag_reference.h"

/* constants */

enum
{
	k_maximum_error_category_count = 100,
	k_ui_maximum_error_per_category_count = 100,
	k_maximum_number_of_widget_animations_per_screen = 64,
	k_maximum_number_of_animation_keyframe_blocks = 64,
	k_maximum_number_of_shapes_per_group = 64,
	k_maximum_number_of_shape_blocks = 32,
	k_maximum_points_per_shape = 16,
	k_maximum_object_scenes_per_screen = 32,
	k_maximum_objects_per_ui_scene = 32,
	k_maximum_lights_per_ui_scene = 8,
	k_maximum_number_of_bitmap_blocks = 64,
	k_maximum_number_of_persisant_animations = 100,
	k_maximum_number_of_list_item_skins = 32,
};

/* enums */

enum e_user_interface_global_sound
{
	_user_interface_global_sound_cursor,
	_user_interface_global_sound_selection,
	_user_interface_global_sound_error,
	_user_interface_global_sound_advancing,
	_user_interface_global_sound_retreating,
	_user_interface_global_sound_initial_login,
	_user_interface_global_sound_vkbd_cursor,
	_user_interface_global_sound_vkbd_character_insertion,
	_user_interface_global_sound_online_notification,
	_user_interface_global_sound_tabbed_view_pane_tabbing,
	_user_interface_global_sound_pregame_countdown_timer,
	_user_interface_global_sound_unused0,
	_user_interface_global_sound_matchmaking_advance,
	_user_interface_global_sound_unused1,
	_user_interface_global_sound_unused2,
	_user_interface_global_sound_unused3,
	k_user_interface_global_sound_count
};

enum e_ui_error_category_flags : int16
{
	_ui_error_category_use_large_dialog_bit = 0
};

enum e_ui_error_default_button : uint8
{
	_ui_error_default_button_no_default,
	_ui_error_default_button_default_ok,
	_ui_error_default_button_default_cancel,
};

enum e_ui_error_types : uint32
{
	_ui_error_unknown = 0x0,
	_ui_error_generic = 0x1,
	_ui_error_generic_networking = 0x2,
	_ui_error_system_link_generic_join_failure = 0x3,
	_ui_error_system_link_no_network_connection = 0x4,
	_ui_error_system_link_connection_lost = 0x5,
	_ui_error_network_game_oos = 0x6,
	_ui_error_xbox_live_sign_out_confirmation = 0x7,
	_ui_error_confirm_revert_to_last_save = 0x8,
	_ui_error_confirm_quit_without_save = 0x9,
	_ui_error_confirm_delete_player_profile = 0xA,
	_ui_error_confirm_delete_variant = 0xB,
	_ui_error_player_profile_creation_failed = 0xC,
	_ui_error_variant_profile_creation_failed = 0xD,
	_ui_error_playlist_creation_failed = 0xE,
	_ui_error_core_file_load_failed = 0xF,
	_ui_error_mu_removed_during_player_profile_save = 0x10,
	_ui_error_mu_removed_during_variant_save = 0x11,
	_ui_error_mu_removed_during_playlist_save = 0x12,
	_ui_error_message_saving_to_mu = 0x13,
	_ui_error_message_saving_file = 0x14,
	_ui_error_message_creating_player_profile = 0x15,
	_ui_error_message_creating_variant_profile = 0x16,
	_ui_error_message_saving_checkpoint = 0x17,
	_ui_error_failed_to_load_player_profile = 0x18,
	_ui_error_failed_to_load_variant = 0x19,
	_ui_error_failed_to_load_playlist = 0x1A,
	_ui_error_failed_to_load_save_game = 0x1B,
	_ui_error_controller1_removed = 0x1C,
	_ui_error_controller2_removed = 0x1D,
	_ui_error_controller3_removed = 0x1E,
	_ui_error_controller4_removed = 0x1F,
	_ui_error_need_more_free_blocks_to_save = 0x20,
	_ui_error_maximum_saved_game_files_already_exist = 0x21,
	_ui_error_dirty_disk = 0x22,
	_ui_error_xblive_cannot_access_service = 0x23,
	_ui_error_xblive_title_update_required = 0x24,
	_ui_error_xblive_servers_too_busy = 0x25,
	_ui_error_xblive_duplicate_logon = 0x26,
	_ui_error_xblive_account_management_required = 0x27,
	_ui_error_warning_xblive_recommended_messages_available = 0x28,
	_ui_error_xblive_invalid_match_session = 0x29,
	_ui_error_warning_xblive_poor_network_performance = 0x2A,
	_ui_error_not_enough_open_slots_to_join_match_session = 0x2B,
	_ui_error_xblive_corrupt_download_content = 0x2C,
	_ui_error_confirm_xblive_corrupt_saved_game_file_removal = 0x2D,
	_ui_error_xblive_invalid_user_account = 0x2E,
	_ui_error_confirm_boot_clan_member = 0x2F,
	_ui_error_confirm_controller_sign_out = 0x30,
	_ui_error_beta_xblive_service_qos_report = 0x31,
	_ui_error_beta_feature_disabled = 0x32,
	_ui_error_beta_network_connection_required = 0x33,
	_ui_error_confirm_friend_removal = 0x34,
	_ui_error_confirm_boot_to_dash = 0x35,
	_ui_error_confirm_launch_xdemos = 0x36,
	_ui_error_confirm_exit_game_session = 0x37,
	_ui_error_xblive_connection_to_xbox_live_lost = 0x38,
	_ui_error_xblive_message_send_failure = 0x39,
	_ui_error_network_link_lost = 0x3A,
	_ui_error_network_link_required = 0x3B,
	_ui_error_xblive_invalid_passcode = 0x3C,
	_ui_error_join_aborted = 0x3D,
	_ui_error_join_session_not_found = 0x3E,
	_ui_error_join_qos_failure = 0x3F,
	_ui_error_join_data_decode_failure = 0x40,
	_ui_error_join_game_full = 0x41,
	_ui_error_join_game_closed = 0x42,
	_ui_error_join_version_mismatch = 0x43,
	_ui_error_join_failed_unknown_reason = 0x44,
	_ui_error_join_failed_friend_in_matchmade_game = 0x45,
	_ui_error_player_profile_name_must_be_unique = 0x46,
	_ui_error_variant_name_must_be_unique = 0x47,
	_ui_error_playlist_name_must_be_unique = 0x48,
	_ui_error_saved_film_name_must_be_unique = 0x49,
	_ui_error_no_free_slots_player_profile = 0x4A,
	_ui_error_no_free_slots_variant = 0x4B,
	_ui_error_no_free_slots_playlist = 0x4C,
	_ui_error_no_free_slots_saved_film = 0x4D,
	_ui_error_need_more_space_for_player_profile = 0x4E,
	_ui_error_need_more_space_for_variant = 0x4F,
	_ui_error_need_more_space_for_playlist = 0x50,
	_ui_error_need_more_space_for_saved_film = 0x51,
	_ui_error_cannot_set_privileges_on_member_whose_data_not_known = 0x52,
	_ui_error_cant_delete_default_profile = 0x53,
	_ui_error_cant_delete_default_variant = 0x54,
	_ui_error_cant_delete_default_playlist = 0x55,
	_ui_error_cant_delete_default_saved_film = 0x56,
	_ui_error_cant_delete_profile_in_use = 0x57,
	_ui_error_player_profile_name_must_have_alphanumeric_characters = 0x58,
	_ui_error_variant_name_must_have_alphanumeric_characters = 0x59,
	_ui_error_playlist_name_must_have_alphanumeric_characters = 0x5A,
	_ui_error_saved_film_name_must_have_alphanumeric_characters = 0x5B,
	_ui_error_teams_not_a_member = 0x5C,
	_ui_error_teams_insufficient_privileges = 0x5D,
	_ui_error_teams_server_busy = 0x5E,
	_ui_error_teams_team_full = 0x5F,
	_ui_error_teams_member_pending = 0x60,
	_ui_error_teams_too_many_requests = 0x61,
	_ui_error_teams_user_already_exists = 0x62,
	_ui_error_teams_user_not_found = 0x63,
	_ui_error_teams_user_teams_full = 0x64,
	_ui_error_teams_no_task = 0x65,
	_ui_error_teams_too_many_teams = 0x66,
	_ui_error_teams_team_already_exists = 0x67,
	_ui_error_teams_team_not_found = 0x68,
	_ui_error_teams_name_contains_bad_words = 0x69,
	_ui_error_teams_description_contains_bad_words = 0x6A,
	_ui_error_teams_motto_contains_bad_words = 0x6B,
	_ui_error_teams_url_contains_bad_words = 0x6C,
	_ui_error_teams_no_admin = 0x6D,
	_ui_error_teams_cannot_set_privileges_on_member_whose_data_not_known = 0x6E,
	_ui_error_live_unknown = 0x6F,
	_ui_error_confirm_delete_profile = 0x70,
	_ui_error_confirm_delete_playlist = 0x71,
	_ui_error_confirm_delete_saved_film = 0x72,
	_ui_error_confirm_live_sign_out = 0x73,
	_ui_error_confirm_confirm_friend_removal = 0x74,
	_ui_error_confirm_promotion_to_superuser = 0x75,
	_ui_error_warn_no_more_clan_superusers = 0x76,
	_ui_error_confirm_corrupt_profile = 0x77,
	_ui_error_confirm_xbox_live_sign_out = 0x78,
	_ui_error_confirm_corrupt_game_variant = 0x79,
	_ui_error_confirm_leave_clan = 0x7A,
	_ui_error_confirm_corrupt_playlist = 0x7B,
	_ui_error_cant_join_gameinvite_without_signon = 0x7C,
	_ui_confirm_proceed_to_crossgame_invite = 0x7D,
	_ui_confirm_decline_crossgame_invite = 0x7E,
	_ui_warn_insert_cd_for_crossgame_invite = 0x7F,
	_ui_error_need_more_space_for_saved_game = 0x80,
	_ui_error_saved_game_cannot_be_loaded = 0x81,
	_ui_error_confirm_controller_signout_with_guests = 0x82,
	_ui_error_warning_party_closed = 0x83,
	_ui_error_warning_party_required = 0x84,
	_ui_error_warning_party_full = 0x85,
	_ui_error_warning_player_in_mm_game = 0x86,
	_ui_error_xblive_failed_to_sign_in = 0x87,
	_ui_error_cant_sign_out_master_with_guests = 0x88,
	_ui_error_this_dot_command_is_obsolete = 0x89,
	_ui_error_this_has_not_been_unlocked = 0x8A,
	_ui_error_confirm_leave_lobby = 0x8B,
	_ui_error_confirm_party_leader_leave_matchmaking = 0x8C,
	_ui_error_confirm_single_box_leave_matchmaking = 0x8D,
	_ui_error_clan_name_not_valid = 0x8E,
	_ui_error_player_list_full = 0x8F,
	_ui_error_recipient_has_blocked_you = 0x90,
	_ui_error_friend_pending = 0x91,
	_ui_error_too_many_requests = 0x92,
	_ui_error_player_already_in_list = 0x93,
	_ui_error_gamertag_not_found = 0x94,
	_ui_error_cannot_message_self = 0x95,
	_ui_error_warning_last_overlord_cant_leave_clan = 0x96,
	_ui_error_confirm_boot_player = 0x97,
	_ui_error_confirm_party_member_leave_pcr = 0x98,
	_ui_error_cannot_sign_in_during_countdown = 0x99,
	_ui_error_xblive_invalid_user = 0x9A,
	_ui_error_xblive_user_not_authorized = 0x9B,
	_ui_error_OBSOLETE = 0x9C,
	_ui_error_OBSOLETE2 = 0x9D,
	_ui_error_xblive_banned_xbox = 0x9E,
	_ui_error_xblive_banned_user = 0x9F,
	_ui_error_xblive_banned_title = 0xA0,
	_ui_error_confirm_exit_game_session_leader = 0xA1,
	_ui_error_message_objectionable_content = 0xA2,
	_ui_error_confirm_enter_downloader = 0xA3,
	_ui_error_confirm_block_user = 0xA4,
	_ui_error_confirm_negative_feedback = 0xA5,
	_ui_error_confirm_change_clan_member_level = 0xA6,
	_ui_error_blank_gamertag = 0xA7,
	_ui_error_confirm_save_and_exit_campaign = 0xA8,
	_ui_error_cant_join_during_matchmaking = 0xA9,
	_ui_error_confirm_restart_level = 0xAA,
	_ui_error_matchmaking_failure_generic = 0xAB,
	_ui_error_matchmaking_failure_missing_content = 0xAC,
	_ui_error_matchmaking_failure_aborted = 0xAD,
	_ui_error_matchmaking_failure_membership_changed = 0xAE,
	_ui_error_confirm_end_game_session = 0xAF,
	_ui_error_confirm_exit_game_session_only_player = 0xB0,
	_ui_error_confirm_exit_game_session_xbox_live_ranked_leader = 0xB1,
	_ui_error_confirm_exit_game_session_xbox_live_ranked = 0xB2,
	_ui_error_confirm_exit_game_session_xbox_live_leader = 0xB3,
	_ui_error_confirm_exit_game_session_xbox_live_only_player = 0xB4,
	_ui_error_confirm_exit_game_session_xbox_live = 0xB5,
	_ui_error_recipient_list_full = 0xB6,
	_ui_error_confirm_exit_campaign = 0xB7,
	_ui_error_xblive_connection_to_xbox_live_lost_save_and_quit = 0xB8,
	_ui_error_booted_from_session = 0xB9,
	_ui_error_confirm_exit_game_session_xbox_live_guest = 0xBA,
	_ui_error_confirm_exit_game_session_xbox_live_ranked_only_player = 0xBB,
	_ui_error_confirm_exit_game_session_xbox_live_unranked_only_player = 0xBC,
	_ui_error_confirm_exit_game_session_xbox_live_unranked_leader = 0xBD,
	_ui_error_confirm_exit_game_session_xbox_live_unranked = 0xBE,
	_ui_error_cant_join_friend_while_in_matchmade_game = 0xBF,
	_ui_error_map_load_failure = 0xC0,
	_ui_error_confirm_campaign_without_achievements = 0xC1,
	_ui_error_no_live_menu_branch_without_signin = 0xC2,
	_ui_error_map_out_of_hard_disk_space = 0xC3,
	_ui_error_device_not_supported = 0xC4,
	_ui_error_achievements_interrupted = 0xC5,
	_confirm_lose_progress = 0xC6,
	_ui_error_beta_achievements_disabled = 0xC7,
	_ui_error_cannot_connect_versions_wrong = 0xC8,
	_ui_error_confirm_booted_from_session = 0xC9,
	_ui_error_confirm_boot_player_from_squad = 0xCA,
	_ui_error_confirm_leave_system_link_lobby = 0xCB,
	_ui_error_confirm_party_member_leave_matchmaking = 0xCC,
	_ui_error_confirm_quit_single_player = 0xCD,
	_ui_error_controller_removed = 0xCE,
	_ui_error_download_in_progress = 0xCF,
	_ui_error_download_fail = 0xD0,
	_ui_error_failed_to_load_map = 0xD1,
	_ui_error_feature_requires_gold = 0xD2,
	_ui_error_keyboard_mapping = 0xD3,
	_ui_error_keyboard_removed = 0xD4,
	_ui_error_live_game_unavailable = 0xD5,
	_ui_error_map_missing = 0xD6,
	_ui_error_matchmaking_failed_generic = 0xD7,
	_ui_error_matchmaking_failed_missing_content = 0xD8,
	_ui_error_mouse_removed = 0xD9,
	_ui_error_party_not_all_on_live = 0xDA,
	_ui_error_party_subnet_not_shared = 0xDB,
	_ui_error_required_game_update = 0xDC,
	_ui_error_saved_game_cannot_be_saved = 0xDD,
	_ui_error_sound_microphone_not_supported = 0xDE,
	_ui_error_system_link_direct_IP = 0xDF,
	_ui_error_text_chat_muted = 0xE0,
	_ui_error_text_chat_parental_controls = 0xE1,
	_ui_error_update_start = 0xE2,
	_ui_error_update_fail = 0xE3,
	_ui_error_update_fail_blocks = 0xE4,
	_ui_error_update_exists = 0xE5,
	_ui_error_insert_original = 0xE6,
	_ui_error_update_fail_network_lost = 0xE7,
	_ui_error_update_mp_out_of_sync = 0xE8,
	_ui_error_update_must_upgrade = 0xE9,
	_ui_error_voice_gold_required = 0xEA,
	_ui_error_voice_parental_controls = 0xEB,
	_ui_error_warning_xblive_poor_network_perofrmance = 0xEC,
	_ui_error_you_missing_map = 0xED,
	_ui_error_someone_missing_map = 0xEE,
	_ui_error_tnp_no_source = 0xEF,
	_ui_error_tnp_disk_read = 0xF0,
	_ui_error_tnp_no_engine_running = 0xF1,
	_ui_error_tnp_signature_verification = 0xF2,
	_ui_error_tnp_drive_removed = 0xF3,
	_ui_error_tnp_disk_full = 0xF4,
	_ui_error_tnp_permissions = 0xF5,
	_ui_error_tnp_unknown = 0xF6,
	_ui_error_continue_install = 0xF7,
	_ui_error_cancel_install = 0xF8,
	_ui_error_confirm_upsell_gold = 0xF9,
	_ui_error_add_to_favorites = 0xFA,
	_ui_error_remove_from_favorites = 0xFB,
	_ui_error_updating_favorites = 0xFC,
	_ui_error_choose_exisiting_checkpoint_location = 0xFD,
	_ui_error_choose_new_checkpoint_location_checkpoints_exist_on_live_and_locally = 0xFE,
	_ui_error_choose_new_checkpoint_location_checkpoints_exist_on_live = 0xFF,
	_ui_error_choose_new_checkpoint_location_checkpoints_exist_locally = 0x100,
	_ui_error_download_map = 0x101,
	_ui_error_want_to_download_map = 0x102,
	_ui_error_ok_download_map = 0x103,
	_ui_error_cancel_download_map = 0x104,
	_ui_error_not_gold_no_map_download = 0x105,
	_ui_error_map_download_connection_lost = 0x106,
	_ui_error_map_download_collision = 0x107,
	_ui_error_map_download_disk_write_error = 0x108,
	_ui_error_matchmaking_failed_no_games = 0x109,
	_ui_error_matchmaking_failed_timeout = 0x10A,
	_ui_error_live_checkpoint_connection_dropped = 0x10B,
	_ui_error_live_checkpoint_hash_mismatch = 0x10C,
	_ui_error_join_gold_game_not_allowed = 0x10D,
	_ui_error_join_locked_game_not_allowed = 0x10E,
	_ui_error_system_link_port_in_use = 0x10F,
	_ui_error_invite_requires_signin = 0x110,
	_ui_error_overwrite_custom_keyboard_mappings = 0x111,
	_ui_error_profile_version_mismatch = 0x112,
	_ui_error_profane_map_name = 0x113,
	_ui_error_profane_variant_name = 0x114,
	_ui_error_demo_version_no_more_for_you = 0x115,
	_ui_error_no_fullscreen_res = 0x116,
	_ui_error_install_not_complete = 0x117,
	_ui_error_lan_fail_download_map = 0x118,
	_ui_error_locater_service_failed = 0x119,
	_ui_error_double_mapping_actions = 0x11A,
	_ui_error_no_multiplayer_achievements_for_silver = 0x11B,
	_ui_error_map_download_in_game = 0x11C,
	_ui_error_locator_service_timed_out = 0x11D,
	_ui_error_connection_to_host_lost = 0x11E,
	_ui_error_map_download_profane_name = 0x11F,
	k_last_ui_error_code = _ui_error_map_download_profane_name
};

enum e_header_text_type
{
	_header_text_type_fullscreen,
	_header_text_type_large,
	_header_text_type_half,
	_header_text_type_quarter,
	k_header_text_type_count
};

enum e_header_text_font : int16
{
	_header_text_font_terminal = 0,
	_header_text_font_body_text = 1,
	_header_text_font_title = 2,
	_header_text_font_super_large = 3,
	_header_text_font_large_body_text = 4,
	_header_text_font_split_screen_hud_message = 5,
	_header_text_font_full_screen_hud_message = 6,
	_header_text_font_english_body_text = 7,
	_header_text_font_hud_number_text = 8,
	_header_text_font_subtitle_font = 9,
	_header_text_font_main_menu = 10,
	_header_text_font_text_chat = 11
};

enum e_ui_animation_index : int16
{
	_ui_animation_index_none = 0,
	_ui_animation_index_00 = 1,
	_ui_animation_index_01 = 2,
	_ui_animation_index_02 = 3,
	_ui_animation_index_03 = 4,
	_ui_animation_index_04 = 5,
	_ui_animation_index_05 = 6,
	_ui_animation_index_06 = 7,
	_ui_animation_index_07 = 8,
	_ui_animation_index_08 = 9,
	_ui_animation_index_09 = 10,
	_ui_animation_index_10 = 11,
	_ui_animation_index_11 = 12,
	_ui_animation_index_12 = 13,
	_ui_animation_index_13 = 14,
	_ui_animation_index_14 = 15,
	_ui_animation_index_15 = 16,
	_ui_animation_index_16 = 17,
	_ui_animation_index_17 = 18,
	_ui_animation_index_18 = 19,
	_ui_animation_index_19 = 20,
	_ui_animation_index_20 = 21,
	_ui_animation_index_21 = 22,
	_ui_animation_index_22 = 23,
	_ui_animation_index_23 = 24,
	_ui_animation_index_24 = 25,
	_ui_animation_index_25 = 26,
	_ui_animation_index_26 = 27,
	_ui_animation_index_27 = 28,
	_ui_animation_index_28 = 29,
	_ui_animation_index_29 = 30,
	_ui_animation_index_30 = 31,
	_ui_animation_index_31 = 32,
	_ui_animation_index_32 = 33,
	_ui_animation_index_33 = 34,
	_ui_animation_index_34 = 35,
	_ui_animation_index_35 = 36,
	_ui_animation_index_36 = 37,
	_ui_animation_index_37 = 38,
	_ui_animation_index_38 = 39,
	_ui_animation_index_39 = 40,
	_ui_animation_index_40 = 41,
	_ui_animation_index_41 = 42,
	_ui_animation_index_42 = 43,
	_ui_animation_index_43 = 44,
	_ui_animation_index_44 = 45,
	_ui_animation_index_45 = 46,
	_ui_animation_index_46 = 47,
	_ui_animation_index_47 = 48,
	_ui_animation_index_48 = 49,
	_ui_animation_index_49 = 50,
	_ui_animation_index_50 = 51,
	_ui_animation_index_51 = 52,
	_ui_animation_index_52 = 53,
	_ui_animation_index_53 = 54,
	_ui_animation_index_54 = 55,
	_ui_animation_index_55 = 56,
	_ui_animation_index_56 = 57,
	_ui_animation_index_57 = 58,
	_ui_animation_index_58 = 59,
	_ui_animation_index_59 = 60,
	_ui_animation_index_60 = 61,
	_ui_animation_index_61 = 62,
	_ui_animation_index_62 = 63,
	_ui_animation_index_63 = 64
};

enum e_animation_reference_flags : uint32
{
	_animation_reference_bit_0	// unused
};

enum e_animation_looping_style : int16
{
	_animation_looping_style_none = 0,
	_animation_looping_style_reverse_loop = 1,
	_animation_looping_style_loop = 2,
	_animation_looping_style_dont_loop = 3
};

enum e_shape_block_reference_flags : uint32
{
	_shape_block_reference_bit_0	// unused
};

enum e_ui_model_scene_reference_flags : uint32
{
	_ui_model_scene_reference_bit_0	// unused
};

enum e_bitmap_block_reference_flags : uint32
{
	_bitmap_block_reference_bit_ignore_for_list_skin_size_calculation_bit,
	_bitmap_block_reference_bit_swap_on_relative_list_position_bit,
	_bitmap_block_reference_bit_render_as_progress_bar_bit,
};

enum e_bitmap_blend_method : int16
{
	_bitmap_blend_method_standard = 0,
	_bitmap_blend_method_multiply = 1,
	_bitmap_blend_method_unused = 2
};

/* structures */


struct s_ui_error_information
{
	e_ui_error_types error;
	e_ui_error_category_flags flags;
	e_ui_error_default_button default_button;
	int8 pad;
};

struct s_ui_error_string_data
{
	string_id title;
	string_id message;
	string_id ok;
	string_id cancel;
};

// max count: k_ui_maximum_error_per_category_count
struct s_ui_error
{
	s_ui_error_information info;
	s_ui_error_string_data strings;
};
ASSERT_STRUCT_SIZE(s_ui_error, 24);

// max count: k_maximum_error_category_count
struct s_ui_error_category
{
	s_ui_error_information info;
	tag_reference string_tag;	// unic
	s_ui_error_string_data default_strings;

	tag_block<s_ui_error> error_block;
};
ASSERT_STRUCT_SIZE(s_ui_error_category, 40);

// max count: k_maximum_number_of_animation_keyframe_blocks
struct s_animation_keyframe_reference
{
	int32 pad;
	real32 alpha;
	real_point3d position;
};
ASSERT_STRUCT_SIZE(s_animation_keyframe_reference, 20);

// max count: k_maximum_number_of_widget_animations_per_screen
struct s_animation_reference
{
	e_animation_reference_flags flags;

	// Explaination("Primary Intro Transition", "Defines the primary intro transitional animation")

	int32 intro_animation_period;	// Milliseconds
	tag_block<s_animation_keyframe_reference> keyframes;

	// Explaination("Primary Outro Transition", "Defines the primary outro transitional animation")

	int32 outro_animation_period;	// Milliseconds
	tag_block<s_animation_keyframe_reference> outro_keyframes;

	// Explaination("Ambient Animation", "Defines the ambient animation")

	int32 ambient_animation_period;	// Milliseconds

	e_animation_looping_style ambient_animation_looping_style;
	int16 pad;
	tag_block<s_animation_keyframe_reference> ambient_keyframes;
};
ASSERT_STRUCT_SIZE(s_animation_reference, 44);

// max count: k_maximum_points_per_shape
struct s_point_block_reference
{
	point2d coordinates;
};
ASSERT_STRUCT_SIZE(s_point_block_reference, 4);

// max count: k_maximum_number_of_shape_blocks
struct s_shape_block_reference
{
	e_shape_block_reference_flags flags;
	e_ui_animation_index animation_index;
	int16 intro_animation_delay;	// Milliseconds
	real_argb_color color;
	tag_block<s_point_block_reference> points;

	int16 render_depth_bias;
	int8 pad[14];
};
ASSERT_STRUCT_SIZE(s_shape_block_reference, 48);

// max count: k_maximum_objects_per_ui_scene
struct s_ui_object_reference
{
	char name[32];
};
ASSERT_STRUCT_SIZE(s_ui_object_reference, 32);

// max count: k_maximum_lights_per_ui_scene
struct s_ui_light_reference
{
	char name[32];
};
ASSERT_STRUCT_SIZE(s_ui_light_reference, 32);

// max count: k_maximum_object_scenes_per_screen
struct s_ui_model_scene_reference
{
	/* Explaination("NOTE on coordinate systems", "Halo y-axis=ui z-axis, and Halo z-axis=ui y-axis.
	As a convention, let's always place objects in the ui scenario such that
	they are facing in the '-y' direction, and the camera such that is is
	facing the '+y' direction.This way the ui animation for models(which
	gets applied to the camera) will always be consisitent.") */

	e_ui_model_scene_reference_flags flags;

	e_ui_animation_index animation_index;
	int16 intro_animation_delay;	// Milliseconds
	int16 render_depth_bias;
	int16 pad;

	tag_block<s_ui_object_reference> objects;
	tag_block<s_ui_light_reference> lights;

	real_vector3d animation_scale_factor;
	real_point3d camera_position;
	real32 fov;
	rectangle2d ui_viewport;
	string_id intro_anim;	// unused
	string_id outro_anim;	// unused
	string_id ambient_anim;	// unused
};
ASSERT_STRUCT_SIZE(s_ui_model_scene_reference, 76);

// max count: k_maximum_number_of_bitmap_blocks
struct s_bitmap_block_reference
{
	e_bitmap_block_reference_flags flags;
	e_ui_animation_index animation_index;
	int16 intro_animation_delay;	// Milliseconds

	e_bitmap_blend_method bitmap_blend_method;
	int16 initial_sprite_frame;
	point2d topleft;
	real32 horiz_texture_wrapssecond;
	real32 vert_texture_wraps_second;

	tag_reference bitmap_tag;	// bitm

	int16 render_depth_bias;
	int16 pad;
	real32 sprite_animation_speed_fps;
	point2d progress_bottomleft;
	string_id string_identifier;
	real_vector2d progress_scale;
};
ASSERT_STRUCT_SIZE(s_bitmap_block_reference, 56);

// max count: k_maximum_number_of_shapes_per_group
struct s_shape_group_reference
{
	// Explaination("Unused Debug Geometry Shapes", "This is the old way")

	tag_block<s_shape_block_reference> shapes;

	// Explaination("Model-Light Groups", "Specify commonly used model/light groups here")

	tag_block<s_ui_model_scene_reference> model_scene_blocks;

	// Explaination("Bitmaps", "Specify more flavor bitmaps here")

	tag_block<s_bitmap_block_reference> bitmap_blocks;
};
ASSERT_STRUCT_SIZE(s_shape_group_reference, 24);

// max count: k_maximum_number_of_persisant_animations
struct s_persistant_animation_reference
{
	int32 pad;
	int32 animation_period;	// Milliseconds
	tag_block<s_animation_keyframe_reference> interpolatedKeyframes;
};
ASSERT_STRUCT_SIZE(s_persistant_animation_reference, 16);

// max count: k_maximum_number_of_list_item_skins
struct s_user_interface_list_skin_reference
{
	tag_reference list_item_skins;	// skin
};
ASSERT_STRUCT_SIZE(s_user_interface_list_skin_reference, 8);

// max count: UNSIGNED_SHORT_MAX
struct s_skill_to_rank_mapping
{
	short_bounds skill_bounds;
};
ASSERT_STRUCT_SIZE(s_skill_to_rank_mapping, 4);

struct s_user_interface_dialog_bounds
{
	rectangle2d dialog_header_text_bounds;
	rectangle2d dialog_button_key_text_bounds;
};

struct s_user_interface_shared_globals
{
	real32 gap0;
	int16 auto_typing_text_time;
	int16 auto_typing_text_words_per_line;
	real32 auto_typing_text_size;
	real_argb_color auto_typing_text_color;
	real32 gap1;
	real_vector2d blur_blending_speed;
	real_argb_color blur_color;
	real32 gap2[3];

	// Explaination("UI Rendering Globals", "miscellaneous rendering globals, more below...")

	real32 overlayed_screen_alpha_mod;
	int16 inc_text_update_period_milliseconds;
	int16 inc_text_block_character;				// ASCII Code
	real32 callout_text_scale;
	real_argb_color progress_bar_color;
	real32 near_clip_plane_distance;	// ObjectsCloserThanThisAreNotDrawn
	real32 projection_plane_distance;	// DistanceAtWhichObjectsAreRenderedWhenZ0normalSize
	real32 far_clip_plane_distance;		// ObjectsFartherThanThisAreNotDrawn

	// Explaination("Overlayed UI Color", "This is the color of the overlayed ui effect; the alpha component is the maximum opacity")

	real_argb_color overlayed_interface_color;
	int8 pad0[12];

	// Explaination("Displayed Errors", "For each error condition displayed in the UI, set the title and description string ids here")

	tag_block<s_ui_error_category> errors;

	tag_reference sounds[k_user_interface_global_sound_count]; // snd!

	// Explaination("Global Bitmaps", "Sprite sequences for global ui bitmaps, as follows: 1) vkeyboard cursor")

	tag_reference global_bitmaps_tag; // bitm

	// Explaination("Global Text Strings", "Global UI Text goes here")

	tag_reference unicode_string_list_tag;	// unic

	// Explaination("Screen Animations", "Animations used by screen definitions for transitions and ambient animating")

	tag_block<s_animation_reference> screen_animations;

	// Explaination("Polygonal Shape Groups", "Define the various groups of shape-objects for use on any ui screens here")

	tag_block<s_shape_group_reference> shape_gsroups;

	// Explaination("Persistant Background Animations", "These are the animations used by elements that live in the persistant background")

	tag_block<s_persistant_animation_reference> animations;

	/* Explaination("List Skins", "These define the visual appearances (skins) available for UI lists
	They are expected to be entered in the following order :
		0) default
		1) squad lobby player list
		2) settings list
		3) playlist entry list
		4) variants list
		5) game browser list
		6) online player menu
		7) game setup menu
		8) playlist contents display
		9) profile picker
		10) mp map list
		11) main menu
		")
	*/

	tag_block<s_user_interface_list_skin_reference> list_item_skins;

	// Explaination("Additional UI Strings", "These are for specific purposes as noted")

	tag_reference button_key_type_strings;	// unic
	tag_reference gametype_strings;	// unic
	tag_reference unknown_tag_reference;

	// Explaination("Skill to rank mapping table", "")

	tag_block<s_skill_to_rank_mapping> skill_mappings;

	// Explaination("WINDOW PARAMETERS", "Various settings for different sized UI windows")

	e_header_text_font header_text_font[k_header_text_type_count];

	real_argb_color default_text_color;

	s_user_interface_dialog_bounds dialog_bounds[k_header_text_type_count];

	// Explaination("Main menu music", "Looping sound that plays while the main menu is active")

	tag_reference main_menu_music;	// lsnd
	int32 music_fade_time_milliseconds;
};
ASSERT_STRUCT_SIZE(s_user_interface_shared_globals, 452);

/* prototypes */

s_user_interface_shared_globals* __cdecl user_interface_shared_globals_get(void);