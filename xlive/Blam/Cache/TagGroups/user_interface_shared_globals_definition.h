#pragma once
#include "Blam/Common/Common.h"
#include "Blam/Cache/DataTypes/BlamDataTypes.h"
#include "Blam/Cache/TagGroups.h"
#include "Blam/Math/BlamMath.h"
#include "Util/Memory.h"
#include <wtypes.h>

#pragma pack(push,1)
struct user_interface_shared_globals :TagGroup<'wigl'>
{
	DWORD unk0[19];
	float overlayed_screen_alpha_mod;
	__int16 inc_text_update_period;
	__int16 inc_text_block_character;
	float callout_text_scale;
	real_color_argb progress_bar_color;
	float near_clip_plane_distance;
	float projection_plane_distance;
	float far_clip_plane_distance;
	real_color_argb overlayed_interface_color;
	DWORD unk7C[3];
	struct s_errors_block
	{
		string_id category_name;
		enum e_flags : short
		{
			use_large_dialog = FLAG(0),
		};
		e_flags flags;
		enum e_default_button : byte
		{
			no_default = 0,
			default_ok = 1,
			default_cancel = 2,
		};
		e_default_button default_button;
		byte pad;
		tag_reference string_tag;
		string_id default_title;
		string_id default_message;
		string_id default_ok;
		string_id default_cancel;
		struct s_ui_error
		{
			enum e_error : int
			{
				error_unknown = 0,
				error_generic = 1,
				error_generic_networking = 2,
				error_system_link_generic_join_failure = 3,
				error_system_link_no_network_connection = 4,
				error_system_link_connection_lost = 5,
				error_network_game_oos = 6,
				error_xbox_live_sign_out_confirmation = 7,
				error_confirm_revert_to_last_save = 8,
				error_confirm_quit_without_save = 9,
				error_confirm_delete_player_profile = 10,
				error_confirm_delete_variant_file = 11,
				error_player_profile_creation_failed = 12,
				error_variant_profile_creation_failed = 13,
				error_playlist_creation_failed = 14,
				error_core_file_load_failed = 15,
				error_mu_removed_during_player_profile_save = 16,
				error_mu_removed_during_variant_save = 17,
				error_mu_removed_during_playlist_save = 18,
				error_message_saving_to_mu = 19,
				error_message_saving_file = 20,
				error_message_creating_player_profile = 21,
				error_message_creating_variant_profile = 22,
				error_message_saving_checkpoint = 23,
				error_failed_to_load_player_profile = 24,
				error_failed_to_load_variant = 25,
				error_failed_to_load_playlist = 26,
				error_failed_to_load_save_game = 27,
				error_controller1_removed = 28,
				error_controller2_removed = 29,
				error_controller3_removed = 30,
				error_controller4_removed = 31,
				error_need_more_free_blocks_to_save = 32,
				error_maximum_saved_game_files_already_exist = 33,
				error_dirty_disk = 34,
				error_xblive_cannot_access_service = 35,
				error_xblive_title_update_required = 36,
				error_xblive_servers_too_busy = 37,
				error_xblive_duplicate_logon = 38,
				error_xblive_account_management_required = 39,
				error_warning_xblive_recommended_messages_available = 40,
				error_xblive_invalid_match_session = 41,
				error_warning_xblive_poor_network_performance = 42,
				error_not_enough_open_slots_to_join_match_session = 43,
				error_xblive_corrupt_download_content = 44,
				error_confirm_xblive_corrupt_saved_game_file_removal = 45,
				error_xblive_invalid_user_account = 46,
				error_confirm_boot_clan_member = 47,
				error_confirm_controller_sign_out = 48,
				error_beta_xblive_service_qos_report = 49,
				error_beta_feature_disabled = 50,
				error_beta_network_connection_required = 51,
				error_confirm_friend_removal = 52,
				error_confirm_boot_to_dash = 53,
				error_confirm_launch_xdemos = 54,
				error_confirm_exit_game_session = 55,
				error_xblive_connection_to_xbox_live_lost = 56,
				error_xblive_message_send_failure = 57,
				error_network_link_lost = 58,
				error_network_link_required = 59,
				error_xblive_invalid_passcode = 60,
				error_join_aborted = 61,
				error_join_session_not_found = 62,
				error_join_qos_failure = 63,
				error_join_data_decode_failure = 64,
				error_join_game_full = 65,
				error_join_game_closed = 66,
				error_join_version_mismatch = 67,
				error_join_failed_unknown_reason = 68,
				error_join_failed_friend_in_matchmade_game = 69,
				error_player_profile_name_must_be_unique = 70,
				error_variant_name_must_be_unique = 71,
				error_playlist_name_must_be_unique = 72,
				error_saved_film_name_must_be_unique = 73,
				error_no_free_slots_player_profile = 74,
				error_no_free_slots_variant = 75,
				error_no_free_slots_playlist = 76,
				error_no_free_slots_saved_film = 77,
				error_need_more_space_for_player_profile = 78,
				error_need_more_space_for_variant = 79,
				error_need_more_space_for_playlist = 80,
				error_need_more_space_for_saved_film = 81,
				error_cannot_set_privileges_on_member_whose_data_not_known = 82,
				error_cant_delete_default_profile = 83,
				error_cant_delete_default_variant = 84,
				error_cant_delete_default_playlist = 85,
				error_cant_delete_default_saved_film = 86,
				error_cant_delete_profile_in_use = 87,
				error_player_profile_name_must_have_alphanumeric_characters = 88,
				error_variant_name_must_have_alphanumeric_characters = 89,
				error_playlist_name_must_have_alphanumeric_characters = 90,
				error_saved_film_name_must_have_alphanumeric_characters = 91,
				error_teams_not_a_member = 92,
				error_teams_insufficient_privileges = 93,
				error_teams_server_busy = 94,
				error_teams_team_full = 95,
				error_teams_member_pending = 96,
				error_teams_too_many_requests = 97,
				error_teams_user_already_exists = 98,
				error_teams_user_not_found = 99,
				error_teams_user_teams_full = 100,
				error_teams_no_task = 101,
				error_teams_too_many_teams = 102,
				error_teams_team_already_exists = 103,
				error_teams_team_not_found = 104,
				error_teams_name_contains_bad_words = 105,
				error_teams_description_contains_bad_words = 106,
				error_teams_motto_contains_bad_words = 107,
				error_teams_url_contains_bad_words = 108,
				error_teams_no_admin = 109,
				error_teams_cannot_set_privileges_on_member_whose_data_not_known = 110,
				error_live_unknown = 111,
				error_confirm_delete_profile = 112,
				error_confirm_delete_playlist = 113,
				error_confirm_delete_saved_film = 114,
				error_confirm_live_sign_out = 115,
				error_confirm_confirm_friend_removal = 116,
				error_confirm_promotion_to_superuser = 117,
				error_warn_no_more_clan_superusers = 118,
				error_confirm_corrupt_profile = 119,
				error_confirm_xbox_live_sign_out = 120,
				error_confirm_corrupt_game_variant = 121,
				error_confirm_leave_clan = 122,
				error_confirm_corrupt_playlist = 123,
				error_cant_join_gameinvite_without_signon = 124,
				error_confirm_proceed_to_crossgame_invite = 125,
				error_confirm_decline_crossgame_invite = 126,
				error_warn_insert_cd_for_crossgame_invite = 127,
				error_need_more_space_for_saved_game = 128,
				error_saved_game_cannot_be_loaded = 129,
				error_confirm_controller_signout_with_guests = 130,
				error_warning_party_closed = 131,
				error_warning_party_required = 132,
				error_warning_party_full = 133,
				error_warning_player_in_mm_game = 134,
				error_xblive_failed_to_sign_in = 135,
				error_cant_sign_out_master_with_guests = 136,
				error_obsolete_dot_command = 137,
				error_not_unlocked = 138,
				confirm_leave_lobby = 139,
				error_confirm_party_leader_leave_matchmaking = 140,
				error_confirm_single_box_leave_matchmaking = 141,
				error_invalid_clan_name = 142,
				error_player_list_full = 143,
				error_blocked_by_player = 144,
				error_friend_pending = 145,
				error_too_many_requests = 146,
				error_player_already_in_list = 147,
				error_gamertag_not_found = 148,
				error_cannot_message_self = 149,
				error_warning_last_overlord_cant_leave_clan = 150,
				error_confirm_boot_player = 151,
				error_confirm_party_member_leave_pcr = 152,
				error_cannot_sign_in_during_countdown = 153,
				error_xbl_invalid_user = 154,
				error_xbl_user_not_authorized = 155,
				obsolete = 156,
				obsolete2 = 157,
				error_xbl_banned_xbox = 158,
				error_xbl_banned_user = 159,
				error_xbl_banned_title = 160,
				error_confirm_exit_game_session_leader = 161,
				error_message_objectionable_content = 162,
				error_confirm_enter_downloader = 163,
				error_confirm_block_user = 164,
				error_confirm_negative_feedback = 165,
				error_confirm_change_clan_member_level = 166,
				error_blank_gamertag = 167,
				confirm_save_and_quit_game = 168,
				error_cant_join_during_matchmaking = 169,
				error_confirm_restart_level = 170,
				matchmaking_failure_generic = 171,
				matchmaking_failure_missing_content = 172,
				matchmaking_failure_aborted = 173,
				matchmaking_failure_membership_changed = 174,
				confirm_end_game_session = 175,
				confirm_exit_game_session_only_player = 176,
				confirm_exit_game_session_xbox_live_ranked_leader = 177,
				confirm_exit_game_session_xbox_live_ranked = 178,
				confirm_exit_game_session_xbox_live_leader = 179,
				confirm_exit_game_session_xbox_live_only_player = 180,
				confirm_exit_game_session_xbox_live = 181,
				recipients_list_full = 182,
				confirm_quit_campaign_no_save = 183,
				xblive_connection_to_xbox_live_lost_save_and_quit = 184,
				booted_from_session = 185,
				confirm_exit_game_session_xbox_live_guest = 186,
				confirm_exit_game_session_xbox_live_ranked_only_player = 187,
				confirm_exit_game_session_xbox_live_unranked_only_player = 188,
				confirm_exit_game_session_xbox_live_unranked_leader = 189,
				confirm_exit_game_session_xbox_live_unranked = 190,
				cant_join_friend_while_in_matchmade_game = 191,
				map_load_failure = 192,
				error_achievements_interrupted = 193,
				confirm_lose_progress = 194,
				error_beta_achievements_disabled = 195,
				error_cannot_connect_versions_wrong = 196,
				confirm_booted_from_session = 197,
				confirm_boot_player_from_squad = 198,
				confirm_leave_system_link_lobby = 199,
				confirm_party_member_leave_matchmaking = 200,
				confirm_quit_single_player = 201,
				error_controller_removed = 202,
				error_download_in_progress = 203,
				error_download_fail = 204,
				error_failed_to_load_map = 205,
				error_feature_requires_gold = 206,
				error_keyboard_mapping = 207,
				error_keyboard_removed = 208,
				error_live_game_unavailable = 209,
				error_map_missing = 210,
				error_matchmaking_failed_generic = 211,
				error_matchmaking_failed_missing_content = 212,
				error_mouse_removed = 213,
				error_party_not_all_on_live = 214,
				error_party_subnet_not_shared = 215,
				error_required_game_update = 216,
				error_saved_game_cannot_be_saved = 217,
				error_sound_microphone_not_supported = 218,
				error_system_link_direct_ip = 219,
				error_text_chat_muted = 220,
				error_text_chat_parental_controls = 221,
				error_update_start = 222,
				error_update_fail = 223,
				error_update_fail_blocks = 224,
				error_update_exists = 225,
				error_insert_original = 226,
				error_update_fail_network_lost = 227,
				error_update_mp_out_of_sync = 228,
				error_update_must_upgrade = 229,
				error_voice_gold_required = 230,
				error_voice_parental_controls = 231,
				error_warning_xblive_poor_network_perofrmance = 232,
				error_you_missing_map = 233,
				error_someone_missing_map = 234,
				error_tnp_no_source = 235,
				error_tnp_disk_read = 236,
				error_tnp_no_engine_running = 237,
				error_tnp_signature_verification = 238,
				error_tnp_drive_removed = 239,
				error_tnp_disk_full = 240,
				error_tnp_permissions = 241,
				error_tnp_unknown = 242,
				continue_install = 243,
				cancel_install = 244,
				error_confirm_upsell_gold = 245,
				error_add_to_favorites = 246,
				error_remove_from_favorites = 247,
				error_updating_favorites = 248,
				choose_existing_checkpoint_location = 249,
				choose_new_checkpoint_location_checkpoints_exist_on_live_and_local = 250,
				choose_new_checkpoint_location_checkpoints_exist_on_live = 251,
				choose_new_checkpoint_location_checkpoints_exist_locally = 252,
				xxx = 253,
			};
			e_error error;
			enum e_flags : short
			{
				use_large_dialog = FLAG(0),
			};
			e_flags flags;
			enum e_default_button : byte
			{
				no_default = 0,
				default_ok = 1,
				default_cancel = 2,
			};
			e_default_button default_button;
			byte unk_7[1];
			string_id title;
			string_id message;
			string_id ok;
			string_id cancel;
		};
		TAG_BLOCK_SIZE_ASSERT(s_ui_error, 24);
		tag_block<s_ui_error> error_block;
	};
	TAG_BLOCK_SIZE_ASSERT(s_errors_block, 0x28);
	tag_block<s_errors_block> errors;
	tag_reference cursor_sound_tag;
	tag_reference selection_sound_tag;
	tag_reference error_sound_tag;
	tag_reference advancing_sound_tag;
	tag_reference retreating_sound_tag;
	tag_reference initial_login_sound_tag;
	tag_reference vkbd_sound_tag;
	tag_reference vkbd_char_insertion_sound_tag;
	tag_reference online_notification_sound_tag;
	tag_reference tabbed_view_pane_tabbing_sound_tag;
	tag_reference pregame_count_down_sound_tag;
	tag_reference unkE8;
	tag_reference matchmaking_advance_sound_tag;
	tag_reference unkF8;
	tag_reference unk100;
	tag_reference unk108;
	tag_reference global_bitmaps_tag;
	tag_reference unicode_string_list_tag;
	struct s_animation_reference
	{
		enum e_flags : __int32
		{
			unused = FLAG(0),
		};
		e_flags flags;

		struct s_keyframes_block
		{
			DWORD pad;
			float alpha;
			real_point3d position;
		};
		TAG_BLOCK_SIZE_ASSERT(s_keyframes_block, 0x14);

		int intro_animationPeriodMilliseconds;
		tag_block<s_keyframes_block> primary_intro_transition;

		DWORD pad[4];

		int outro_animationPeriodMilliseconds;
		tag_block<s_keyframes_block> primary_intro_transition;

		DWORD pad[4];

		struct s_ambient_animation
		{
			int animation_period_miliseconds;
			enum e_ambient_animation_looping_style : short
			{
				_ambient_animation_looping_style_none = 0,
				_ambient_animation_looping_style_reverse_loop = 1,
				_ambient_animation_looping_style_loop = 2,
				_ambient_animation_looping_style_dont_loop = 3,
			};
			e_ambient_animation_looping_style ambient_animation_looping_style;
			WORD unk22;
			tag_block<s_keyframes_block> keyframes;
		};
		s_ambient_animation ambient_animation;

		DWORD pad[4];
	};
	TAG_BLOCK_SIZE_ASSERT(s_animation_reference, 0x38);
	tag_block<s_animation_reference> screen_animations;
	struct s_shape_group_reference
	{
		struct s_shapes_block
		{
			enum class e_flags : int
			{
				unused = FLAG(0),
			};
			e_flags flags;
			enum class e_animation_index : __int16
			{
				none = 0,
				NUM_00 = 1,
				NUM_01 = 2,
				NUM_02 = 3,
				NUM_03 = 4,
				NUM_04 = 5,
				NUM_05 = 6,
				NUM_06 = 7,
				NUM_07 = 8,
				NUM_08 = 9,
				NUM_09 = 10,
				NUM_10 = 11,
				NUM_11 = 12,
				NUM_12 = 13,
				NUM_13 = 14,
				NUM_14 = 15,
				NUM_15 = 16,
				NUM_16 = 17,
				NUM_17 = 18,
				NUM_18 = 19,
				NUM_19 = 20,
				NUM_20 = 21,
				NUM_21 = 22,
				NUM_22 = 23,
				NUM_23 = 24,
				NUM_24 = 25,
				NUM_25 = 26,
				NUM_26 = 27,
				NUM_27 = 28,
				NUM_28 = 29,
				NUM_29 = 30,
				NUM_30 = 31,
				NUM_31 = 32,
				NUM_32 = 33,
				NUM_33 = 34,
				NUM_34 = 35,
				NUM_35 = 36,
				NUM_36 = 37,
				NUM_37 = 38,
				NUM_38 = 39,
				NUM_39 = 40,
				NUM_40 = 41,
				NUM_41 = 42,
				NUM_42 = 43,
				NUM_43 = 44,
				NUM_44 = 45,
				NUM_45 = 46,
				NUM_46 = 47,
				NUM_47 = 48,
				NUM_48 = 49,
				NUM_49 = 50,
				NUM_50 = 51,
				NUM_51 = 52,
				NUM_52 = 53,
				NUM_53 = 54,
				NUM_54 = 55,
				NUM_55 = 56,
				NUM_56 = 57,
				NUM_57 = 58,
				NUM_58 = 59,
				NUM_59 = 60,
				NUM_60 = 61,
				NUM_61 = 62,
				NUM_62 = 63,
				NUM_63 = 64,
			};
			e_animation_index animation_index;
			__int16 intro_animation_delay_milliseconds;
			real_color_argb color;
			struct s_points_block
			{
				__int16 coordinates_x;
				__int16 coordinates_y;
			};
			TAG_BLOCK_SIZE_ASSERT(s_points_block, 0x4);
			tag_block<s_points_block> points;
			__int16 render_depth_bias;
			WORD pad22[7];
		};
		TAG_BLOCK_SIZE_ASSERT(s_shapes_block, 0x30);
		tag_block<s_shapes_block> shapes;

		struct s_model_scene_blocks_block
		{
			enum e_flags : int
			{
				unused = FLAG(0),
			};
			e_flags flags;
			enum e_animation_index : short
			{
				none = 0,
				NUM_00 = 1,
				NUM_01 = 2,
				NUM_02 = 3,
				NUM_03 = 4,
				NUM_04 = 5,
				NUM_05 = 6,
				NUM_06 = 7,
				NUM_07 = 8,
				NUM_08 = 9,
				NUM_09 = 10,
				NUM_10 = 11,
				NUM_11 = 12,
				NUM_12 = 13,
				NUM_13 = 14,
				NUM_14 = 15,
				NUM_15 = 16,
				NUM_16 = 17,
				NUM_17 = 18,
				NUM_18 = 19,
				NUM_19 = 20,
				NUM_20 = 21,
				NUM_21 = 22,
				NUM_22 = 23,
				NUM_23 = 24,
				NUM_24 = 25,
				NUM_25 = 26,
				NUM_26 = 27,
				NUM_27 = 28,
				NUM_28 = 29,
				NUM_29 = 30,
				NUM_30 = 31,
				NUM_31 = 32,
				NUM_32 = 33,
				NUM_33 = 34,
				NUM_34 = 35,
				NUM_35 = 36,
				NUM_36 = 37,
				NUM_37 = 38,
				NUM_38 = 39,
				NUM_39 = 40,
				NUM_40 = 41,
				NUM_41 = 42,
				NUM_42 = 43,
				NUM_43 = 44,
				NUM_44 = 45,
				NUM_45 = 46,
				NUM_46 = 47,
				NUM_47 = 48,
				NUM_48 = 49,
				NUM_49 = 50,
				NUM_50 = 51,
				NUM_51 = 52,
				NUM_52 = 53,
				NUM_53 = 54,
				NUM_54 = 55,
				NUM_55 = 56,
				NUM_56 = 57,
				NUM_57 = 58,
				NUM_58 = 59,
				NUM_59 = 60,
				NUM_60 = 61,
				NUM_61 = 62,
				NUM_62 = 63,
				NUM_63 = 64,
			};
			e_animation_index animation_index;
			__int16 intro_animation_delay_milliseconds;
			__int16 render_depth_bias;
			WORD unkA;
			struct s_objects_block
			{
				tag_string32 name;
			};
			TAG_BLOCK_SIZE_ASSERT(s_objects_block, 0x20);
			tag_block<s_objects_block> objects;
			struct s_lights_block
			{
				tag_string32 name;
			};
			TAG_BLOCK_SIZE_ASSERT(s_lights_block, 0x20);
			tag_block<s_lights_block> lights;
			real_vector3d animation_scale_factor;
			real_point3d camera_position;		
			float fov_degress;
			rect2d ui_viewport;
			string_id unused_intro_anim;
			string_id unused_outro_anim;
			string_id unused_ambient_anim;
		};
		TAG_BLOCK_SIZE_ASSERT(s_model_scene_blocks_block, 0x4C);

		tag_block<s_model_scene_blocks_block> model_scene_blocks;
		struct s_bitmap_blocks_block
		{
			enum e_flags : __int32
			{
				ignore_for_list_skin_size_calculation = FLAG(0),
				swap_on_relative_list_position = FLAG(1),
				render_as_progress_bar = FLAG(2),
			};
			e_flags flags;
			enum class e_animation_index : short
			{
				none = 0,
				NUM_00 = 1,
				NUM_01 = 2,
				NUM_02 = 3,
				NUM_03 = 4,
				NUM_04 = 5,
				NUM_05 = 6,
				NUM_06 = 7,
				NUM_07 = 8,
				NUM_08 = 9,
				NUM_09 = 10,
				NUM_10 = 11,
				NUM_11 = 12,
				NUM_12 = 13,
				NUM_13 = 14,
				NUM_14 = 15,
				NUM_15 = 16,
				NUM_16 = 17,
				NUM_17 = 18,
				NUM_18 = 19,
				NUM_19 = 20,
				NUM_20 = 21,
				NUM_21 = 22,
				NUM_22 = 23,
				NUM_23 = 24,
				NUM_24 = 25,
				NUM_25 = 26,
				NUM_26 = 27,
				NUM_27 = 28,
				NUM_28 = 29,
				NUM_29 = 30,
				NUM_30 = 31,
				NUM_31 = 32,
				NUM_32 = 33,
				NUM_33 = 34,
				NUM_34 = 35,
				NUM_35 = 36,
				NUM_36 = 37,
				NUM_37 = 38,
				NUM_38 = 39,
				NUM_39 = 40,
				NUM_40 = 41,
				NUM_41 = 42,
				NUM_42 = 43,
				NUM_43 = 44,
				NUM_44 = 45,
				NUM_45 = 46,
				NUM_46 = 47,
				NUM_47 = 48,
				NUM_48 = 49,
				NUM_49 = 50,
				NUM_50 = 51,
				NUM_51 = 52,
				NUM_52 = 53,
				NUM_53 = 54,
				NUM_54 = 55,
				NUM_55 = 56,
				NUM_56 = 57,
				NUM_57 = 58,
				NUM_58 = 59,
				NUM_59 = 60,
				NUM_60 = 61,
				NUM_61 = 62,
				NUM_62 = 63,
				NUM_63 = 64,
			};
			e_animation_index animation_index;
			__int16 intro_animation_delay_milliseconds;
			enum e_bitmap_blend_method : short
			{
				standard = 0,
				multiply = 1,
				unused = 2,
			};
			e_bitmap_blend_method bitmap_blend_method;
			__int16 initial_sprite_frame;
			__int16 topleft_x;
			__int16 topleft_y;
			float horiz_texture_wrapssecond;
			float vert_texture_wrapssecond;
			tag_reference bitmap_tag;
			__int16 render_depth_bias;
			WORD unk22;
			float sprite_animation_speed_fps;
			__int16 progress_bottomleft_x;
			__int16 progress_bottomleft_y;
			string_id string_identifier;
			float progress_scale_i;
			float progress_scale_j;
		};
		TAG_BLOCK_SIZE_ASSERT(s_bitmap_blocks_block, 0x38);
		tag_block<s_bitmap_blocks_block> bitmap_blocks;
	};
	TAG_BLOCK_SIZE_ASSERT(s_shape_group_reference, 0x18);
	tag_block<s_shape_group_reference> shape_groups;
	struct s_animations_block
	{
		DWORD unk0;
		int animation_period_miliseconds;
		struct s_interpolated_keyframes_block
		{
			__int32 start_transition_index;
			float alpha;
			real_point3d position;
		};
		TAG_BLOCK_SIZE_ASSERT(s_interpolated_keyframes_block, 0x14);
		tag_block<s_interpolated_keyframes_block> interpolated_keyframes;
	};
	TAG_BLOCK_SIZE_ASSERT(s_animations_block, 0x10);
	tag_block<s_animations_block> animations;
	struct s_list_item_skins_block
	{
		tag_reference list_item_skins;
	};
	TAG_BLOCK_SIZE_ASSERT(s_list_item_skins_block, 0x8);
	tag_block<s_list_item_skins_block> list_item_skins;
	tag_reference button_key_type_strings;
	tag_reference game_type_strings;
	tag_reference unk150;
	struct s_skill_mappings_block
	{
		__int16 skill_bounds_lower;
		__int16 skill_bounds_upper;
	};
	TAG_BLOCK_SIZE_ASSERT(s_skill_mappings_block, 0x4);
	tag_block<s_skill_mappings_block> skill_mappings;
	enum e_full_screen_header_text_font : short
	{
		terminal = 0,
		body_text = 1,
		title = 2,
		super_large_font = 3,
		large_body_text = 4,
		split_screen_hud_message = 5,
		full_screen_hud_message = 6,
		english_body_text = 7,
		hud_number_text = 8,
		subtitle_font = 9,
		main_menu_font = 10,
		text_chat_font = 11,
	};
	e_full_screen_header_text_font full_screen_header_text_font;
	enum e_large_dialog_header_text_font : short
	{
		terminal = 0,
		body_text = 1,
		title = 2,
		super_large_font = 3,
		large_body_text = 4,
		split_screen_hud_message = 5,
		full_screen_hud_message = 6,
		english_body_text = 7,
		hud_number_text = 8,
		subtitle_font = 9,
		main_menu_font = 10,
		text_chat_font = 11,
	};
	e_large_dialog_header_text_font large_dialog_header_text_font;
	enum e_half_dialog_header_text_font : short
	{
		terminal = 0,
		body_text = 1,
		title = 2,
		super_large_font = 3,
		large_body_text = 4,
		split_screen_hud_message = 5,
		full_screen_hud_message = 6,
		english_body_text = 7,
		hud_number_text = 8,
		subtitle_font = 9,
		main_menu_font = 10,
		text_chat_font = 11,
	};
	e_half_dialog_header_text_font half_dialog_header_text_font;
	enum  e_qtr_dialog_header_text_font : short
	{
		terminal = 0,
		body_text = 1,
		title = 2,
		super_large_font = 3,
		large_body_text = 4,
		split_screen_hud_message = 5,
		full_screen_hud_message = 6,
		english_body_text = 7,
		hud_number_text = 8,
		subtitle_font = 9,
		main_menu_font = 10,
		text_chat_font = 11,
	};
	e_qtr_dialog_header_text_font qtr_dialog_header_text_font;
	real_color_argb default_text_color;
	rect2d full_screen_header_text_bounds;

	rect2d full_screen_button_key_text_bounds;

	rect2d large_dialog_header_text_bounds;

	rect2d large_dialog_button_key_text_bounds;

	rect2d half_dialog_header_text_bounds;

	rect2d half_dialog_button_key_text_bounds;

	rect2d qtr_dialog_header_text_bounds;

	rect2d qtr_dialog_button_key_text_bounds;

	tag_reference main_menu_music;
	int music_fade_time;
};
TAG_GROUP_SIZE_ASSERT(user_interface_shared_globals, 0x1C4);
#pragma pack(pop)

/* gets the current loaded user_interface_shared_globals tag*/
user_interface_shared_globals *get_user_interface_shared_globals_ptr()
{
	//dives into globals/globals tag and gets the block "Interface Tags"
	//from there checks current scnr type and itself returns a pointer for the correct one
	typedef user_interface_shared_globals*(_cdecl* get_user_interface_shared_globals_t)();
	get_user_interface_shared_globals_t pwgtz_get_user_interface_shared_globals = Memory::GetAddress<get_user_interface_shared_globals_t>(0x20BB89, 0x1F2CC6);
	return pwgtz_get_user_interface_shared_globals();
}