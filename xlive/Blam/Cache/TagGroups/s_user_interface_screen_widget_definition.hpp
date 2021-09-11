#pragma once
#include "Blam\Cache\DataTypes\BlamDataTypes.h"
#include "Blam/Cache/TagGroups.hpp"
#include "Blam/Math/BlamMath.h"
#pragma pack(push, 1)

struct s_user_interface_screen_widget_definition : TagGroup<'wigt'>
{
	enum e_flags : __int32
	{
		NUM_14_screen_dialog = FLAG(0),
		multiple_panes_are_for_list_flavor_items = FLAG(1),
		no_header_text = FLAG(2),
		NUM_12_screen_dialog = FLAG(3),
		large_dialog = FLAG(4),
		disable_overlay_effect = FLAG(5),
	};
	e_flags flags;
	enum e_screen_id : __int16
	{
		test_1 = 0,
		test_2 = 1,
		test_3 = 2,
		test_4 = 3,
		test_5 = 4,
		game_shell_background = 5,
		main_menu = 6,
		error_dialog_ok_cancel = 7,
		error_dialog_ok = 8,
		press_start_intro = 9,
		player_profile_select = 10,
		sp_level_select = 11,
		sp_difficulty_select = 12,
		network_squad_browser = 13,
		mp_pregame_lobby = 14,
		custom_game_menu = 15,
		postgame_stats = 16,
		mp_map_select = 17,
		sp_pause_game = 18,
		settings = 19,
		gamertag_select = 20,
		gamertag_passcode_entry = 21,
		multiplayer_protocol = 22,
		squad_settings = 23,
		squad_game_settings = 24,
		squad_privacy_settings = 25,
		y_menugameshell = 26,
		y_menugameshell_collapsed = 27,
		y_menuingame = 28,
		y_menuingame_collapsed = 29,
		NUM_4way_join_screen = 30,
		y_menuplayer_selected_options = 31,
		player_selected_options = 32,
		confirmation_dialog = 33,
		live_feedback_menu_dialog = 34,
		live_message_type_dialog = 35,
		voice_msg_dialog = 36,
		stereo_faceplate = 37,
		player_profile_edit_menu = 38,
		pp_controller_settings = 39,
		pp_button_settings = 40,
		pp_thumbstick_settings = 41,
		pp_look_sensitivity_settings = 42,
		pp_invert_look_settings = 43,
		pp_autolevel_settings = 44,
		pp_handicap_settings = 45,
		pp_high_score_rec_settings = 46,
		pp_multiplayer_settings_menu = 47,
		pp_profile_delete_confirmation_dlg = 48,
		pp_choose_foreground_emblem = 49,
		pp_choose_primary_color = 50,
		pp_choose_secondary_color = 51,
		pp_choose_model = 52,
		pp_voice_settings_menu = 53,
		pp_choose_voice_mask = 54,
		pp_voice_thru_tv = 55,
		pp_edit_rotation_list = 56,
		pp_xbl_status_menu = 57,
		pp_appear_offline = 58,
		pp_autooffline = 59,
		game_engine_category_listing = 60,
		edit_slayer_menu = 61,
		edit_koth_menu = 62,
		edit_race_menu = 63,
		edit_oddball_menu = 64,
		edit_juggernaut_menu = 65,
		edit_headhunter_menu = 66,
		edit_ctf_menu = 67,
		edit_assault_menu = 68,
		edit_slayer_score_to_win = 69,
		edit_slayer_time_limit = 70,
		edit_slayer_teams = 71,
		edit_slayer_score_4_killing = 72,
		edit_slayer_kill_in_order = 73,
		edit_slayer_death_pt_loss = 74,
		edit_slayer_suicide_pt_loss = 75,
		edit_slayer_dmg_after_kill = 76,
		edit_slayer_dmg_after_death = 77,
		edit_slayer_speed_after_kill = 78,
		edit_slayer_speed_after_death = 79,
		edit_koth_score_to_win = 80,
		edit_koth_time_limit = 81,
		edit_koth_teams = 82,
		edit_koth_moving_hills = 83,
		edit_koth_uncontesed_control = 84,
		edit_koth_xtra_dmg = 85,
		edit_race_laps_to_win = 86,
		edit_race_time_limit = 87,
		edit_race_teams = 88,
		edit_race_team_scoring = 89,
		edit_race_type = 90,
		edit_race_flag_order = 91,
		edit_race_game_end_condition = 92,
		edit_race_dmg_with_laps = 93,
		edit_race_speed_with_laps = 94,
		edit_oddball_time_to_win = 95,
		edit_oddball_time_limit = 96,
		edit_oddball_teams = 97,
		edit_oddball_ball_spawn_count = 98,
		edit_oddball_ball_waypoints = 99,
		edit_oddball_damage_with_ball = 100,
		edit_oddball_speed_with_ball = 101,
		edit_oddball_invisibility_with_ball = 102,
		edit_jug_score_to_win = 103,
		edit_jug_time_limit = 104,
		edit_jug_pts_for_killing_jugger = 105,
		edit_jug_count = 106,
		edit_jug_species = 107,
		edit_jug_starting_equip = 108,
		edit_jug_damage = 109,
		edit_jug_health = 110,
		edit_jug_speed = 111,
		edit_jug_regeneration = 112,
		edit_jug_waypoints = 113,
		edit_hh_score_to_win = 114,
		edit_hh_time_limit = 115,
		edit_hh_teams = 116,
		edit_hh_death_pt_loss = 117,
		edit_hh_suicide_pt_loss = 118,
		edit_hh_speed_with_token = 119,
		edit_hh_dropped_token_lifetime = 120,
		edit_hh_score_multiplier = 121,
		edit_ctf_score_to_win = 122,
		edit_ctf_time_limit = 123,
		edit_ctf_tie_resolution = 124,
		edit_ctf_single_flag = 125,
		edit_ctf_role_swapping = 126,
		edit_ctf_flag_at_home_to_score = 127,
		edit_ctf_flag_must_reset = 128,
		edit_ctf_dmg_with_flag = 129,
		edit_ctf_speed_with_flag = 130,
		edit_assault_score_to_win = 131,
		edit_assault_time_limit = 132,
		edit_assault_tie_resolution = 133,
		edit_assault_single_flag = 134,
		edit_assault_role_swapping = 135,
		edit_assault_enemy_flag_at_home_to_score = 136,
		edit_assault_flag_must_reset = 137,
		edit_assault_dmg_with_flag = 138,
		edit_assault_speed_with_flag = 139,
		edit_player_number_of_lives = 140,
		edit_player_max_health = 141,
		edit_player_shields = 142,
		edit_player_respawn_time = 143,
		edit_player_count = 144,
		edit_player_invisibility = 145,
		edit_player_suicide_penalty = 146,
		edit_player_friendly_fire = 147,
		edit_item_respawn_grenades = 148,
		edit_item_powerups = 149,
		edit_item_weapon_set = 150,
		edit_item_starting_equipment = 151,
		edit_item_warthogs = 152,
		edit_item_ghosts = 153,
		edit_item_scorpions = 154,
		edit_item_banshees = 155,
		edit_item_mongeese = 156,
		edit_item_shadows = 157,
		edit_item_wraiths = 158,
		edit_indicator_objectives = 159,
		edit_indicator_players_on_motion_sensor = 160,
		edit_indicator_invisible_players_on_motion_sensor = 161,
		edit_indicator_friends = 162,
		edit_indicator_enemies = 163,
		edit_player_options = 164,
		edit_item_options = 165,
		edit_indicator_options = 166,
		virtual_keyboard = 167,
		custom_game_menu_2 = 168,
		slayer_quick_options = 169,
		koth_quick_options = 170,
		race_quick_options = 171,
		oddball_quick_options = 172,
		jugger_quick_options = 173,
		hh_quick_options = 174,
		ctf_quick_options = 175,
		assault_quick_options = 176,
		pick_new_squad_leader = 177,
		variant_editing_options_menu = 178,
		playlist_listsettings = 179,
		playlist_contents = 180,
		playlist_selected_options = 181,
		xbox_live_task_progress = 182,
		pp_vibration_settings = 183,
		boot_player_dialog = 184,
		postgame_statslobby = 185,
		xbox_live_main_menu = 186,
		edit_terries_menu = 187,
		edit_terries_score_to_win = 188,
		edit_terries_time_limit = 189,
		edit_terries_teams = 190,
		terries_quick_options = 191,
		xbox_live_notification_beeper = 192,
		player_profile_selectfancy = 193,
		saved_game_file_actions_dialog = 194,
		mp_start_menu = 195,
		mp_start_player_settings = 196,
		mp_start_handicap_settings = 197,
		mp_start_change_teams = 198,
		mp_start_admin_settings = 199,
		mp_start_controller_settings = 200,
		mp_start_voice_settings = 201,
		mp_start_online_status = 202,
		mpalpha_legal_warning = 203,
		squad_join_progress_dialog = 204,
		mp_alpha_postgame_legal_warning = 205,
		mp_map_selectlobby = 206,
		mp_variant_typelobby = 207,
		mp_variant_listlobby = 208,
		loading_progress = 209,
		matchmaking_progress = 210,
		live_message_display = 211,
		fadein_from_black = 212,
		live_player_profile = 213,
		live_clan_profile = 214,
		live_message_send = 215,
		friends_options_dialog = 216,
		clan_options_dialog = 217,
		campaign_options_dialog = 218,
		optimatch_hoppers_fullscreen = 219,
		playlist_listdialog = 220,
		variant_editing_format = 221,
		variant_quick_options_format = 222,
		variant_param_setting_format = 223,
		vehicle_options = 224,
		match_options = 225,
		player_options = 226,
		team_options = 227,
		game_options = 228,
		equipment_options = 229,
		multiple_choice_dialog = 230,
		network_transition_progress = 231,
		xbox_live_stats = 232,
		pp_choose_background_emblem = 233,
		pp_buttons_qtr = 234,
		pp_stix_qtr = 235,
		clan_member_privs = 236,
		optimatch_hoppers_lobby = 237,
		saved_game_file_dialog = 238,
		xyzzy = 239,
		error_ok_cancel_large = 240,
		yzzyx = 241,
		subtitle_display = 242,
		pp_keyboard_settings = 243,
		pp_keyboard_settings_qtr = 244,
		pp_invert_dual_wield = 245,
		system_settings = 246,
		bungie_news = 247,
		filter_select = 248,
		live_game_browser = 249,
		game_details = 250,
		mp_custom_map_select = 251,
		mp_all_maps_select = 252,
		pp_advanced_keyboard_settings = 253,
		pp_advanced_keyboard_settings_qtr = 254,
		network_adapter_settings = 255,
	};
	e_screen_id screen_id;//0x4
	enum e_button_key_type : __int16
	{
		none = 0,
		A_Select_B_Back= 1,
		A_Select_B_Cancel = 2,
		A_Enter_B_Cancel = 3,
		Y_XBL_Players = 4,
		X_Friend_options = 5,
		X_Clan_Options = 6,
		x_recent_players_options = 7,
		x_options = 8,
		a_select = 9,
		x_settings_a_select_b_back = 10,
		x_delete_a_select_b_done = 11,
		a_accept = 12,
		b_cancel = 13,
		y_xbox_live_players_a_select_b_back = 14,
		y_xbox_live_players_a_select_b_cancel = 15,
		y_xbox_live_players_a_enter_b_cancel = 16,
		y_xbox_live_players_a_select = 17,
		y_xbox_live_players_a_select_b_done = 18,
		y_xbox_live_players_a_accept = 19,
		y_xbox_live_players_b_cancel = 20,
		x_delete_a_select_b_back = 21,
		a_ok = 22,
	};
	e_button_key_type button_key_type;//0x6
	real_color_argb text_color;//0x8
	tag_reference string_list_tag;//0x18
	struct s_panes_block
	{
		PAD(0x2);//0x0
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
		e_animation_index animation_index;//0x2
		struct s_buttons_block
		{
			enum class e_text_flags : __int32
			{
				left_justify_text = FLAG(0),
				right_justify_text = FLAG(1),
				pulsating_text = FLAG(2),
				callout_text = FLAG(3),
				small_31_char_buffer = FLAG(4),
			};
			e_text_flags text_flags;//0x0
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
			e_animation_index animation_index;//0x4
			__int16 intro_animation_delay_milliseconds;//0x6
			PAD(0x2);//0x8
			enum class e_custom_font : __int16
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
			e_custom_font custom_font;//0xA
			real_color_argb text_color;//0xC
			__int16 boundstop;//0x1C
			__int16 boundsleft;//0x1E
			__int16 boundsbottom;//0x20
			__int16 boundsright;//0x22
			tag_reference bitmap;//0x24
			__int16 bitmap_offset_from_topleft_x;//0x2C
			__int16 bitmap_offset_from_topleft_y;//0x2E
			string_id string_id;//0x30
			__int16 render_depth_bias;//0x34
			__int16 mouse_region_top_offset;//0x36
			enum class e_button_flags : __int32
			{
				doesnt_tab_vertically = FLAG(0),
				doesnt_tab_horizontally = FLAG(1),
			};
			e_button_flags button_flags;//0x38
		};
		TAG_BLOCK_SIZE_ASSERT(s_buttons_block, 0x3C);
		tag_block<s_buttons_block> buttons;//0x4
		struct s_list_block_block
		{
			enum class e_flags : __int32
			{
				list_wraps = FLAG(0),
				interactive = FLAG(1),
			};
			e_flags flags;//0x0
			enum class e_skin_index : __int16
			{
				default = 0,
				squad_lobby_player_list = 1,
				settings_list = 2,
				playlist_entry_list = 3,
				variants = 4,
				game_browser = 5,
				online_player_menu = 6,
				game_setup_menu = 7,
				playlist_contents_display = 8,
				player_profile_picker = 9,
				mp_map_selection = 10,
				main_menu_list = 11,
				color_picker = 12,
				profile_picker = 13,
				y_menu_recent_list = 14,
				pcr_team_stats = 15,
				pcr_player_stats = 16,
				pcr_kill_stats = 17,
				pcr_pvp_stats = 18,
				pcr_medal_stats = 19,
				matchmaking_progress = 20,
				default_5 = 21,
				default_6 = 22,
				advanced_settings_list = 23,
				live_game_browser = 24,
				default_wide = 25,
				unused26 = 26,
				unused27 = 27,
				unused28 = 28,
				unused29 = 29,
				unused30 = 30,
				unused31 = 31,
			};
			e_skin_index skin_index;//0x4
			__int16 num_visible_items;//0x6
			__int16 bottom_left_x;//0x8
			__int16 bottom_left_y;//0xA
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
			e_animation_index animation_index;//0xC
			__int16 intro_animation_delay_milliseconds;//0xE
			struct s_unused_block
			{
				enum class e_value_type : __int16
				{
					integer_number = 0,
					floating_point_number = 1,
					boolean = 2,
					text_string = 3,
				};
				e_value_type value_type;//0x0
				enum class e_boolean_value : __int16
				{
					e_false = 0,
					e_true = 1,
				};
				e_boolean_value boolean_value;//0x2
				__int32 integer_value;//0x4
				float fp_value;//0x8
				string_id text_value_string_id;//0xC
				string_id text_label_string_id;//0x10
			};
			TAG_BLOCK_SIZE_ASSERT(s_unused_block, 0x14);
			tag_block<s_unused_block> unused;//0x10
		};
		TAG_BLOCK_SIZE_ASSERT(s_list_block_block, 0x18);
		tag_block<s_list_block_block> list_block;//0xC
		struct s_table_view_block
		{
			enum class e_flags : __int32
			{
				unused = FLAG(0),
			};
			e_flags flags;//0x0
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
			e_animation_index animation_index;//0x4
			__int16 intro_animation_delay_milliseconds;//0x6
			enum class e_custom_font : __int16
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
			e_custom_font custom_font;//0x8
			PAD(0x2);//0xA
			real_color_argb text_color;//0xC
			__int16 topleft_x;//0x1C
			__int16 topleft_y;//0x1E
			struct s_table_rows_block
			{
				enum class e_flags : __int32
				{
					unused = FLAG(0),
				};
				e_flags flags;//0x0
				__int16 row_height;//0x4
				PAD(0x2);//0x6
				struct s_row_cells_block
				{
					enum class e_text_flags : __int32
					{
						left_justify_text = FLAG(0),
						right_justify_text = FLAG(1),
						pulsating_text = FLAG(2),
						callout_text = FLAG(3),
						small_31_char_buffer = FLAG(4),
					};
					e_text_flags text_flags;//0x0
					__int16 cell_width;//0x4
					PAD(0x2);//0x6
					__int16 bitmap_topleft_x;//0x8
					__int16 bitmap_topleft_y;//0xA
					tag_reference bitmap_tag;//0xC
					string_id string_id;//0x14
					__int16 render_depth_bias;//0x18
					PAD(0x2);//0x1A
				};
				TAG_BLOCK_SIZE_ASSERT(s_row_cells_block, 0x1C);
				tag_block<s_row_cells_block> row_cells;//0x8
			};
			TAG_BLOCK_SIZE_ASSERT(s_table_rows_block, 0x10);
			tag_block<s_table_rows_block> table_rows;//0x20
		};
		TAG_BLOCK_SIZE_ASSERT(s_table_view_block, 0x28);
		tag_block<s_table_view_block> table_view;//0x14
		struct s_text_blocks_block
		{
			enum class e_text_flags : __int32
			{
				left_justify_text = FLAG(0),
				right_justify_text = FLAG(1),
				pulsating_text = FLAG(2),
				callout_text = FLAG(3),
				small_31_char_buffer = FLAG(4),
			};
			e_text_flags text_flags;//0x0
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
			e_animation_index animation_index;//0x4
			__int16 intro_animation_delay_milliseconds;//0x6
			PAD(0x2);//0x8
			enum class e_custom_font : __int16
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
			e_custom_font custom_font;//0xA
			real_color_argb text_color;//0xC
			__int16 text_boundstop;//0x1C
			__int16 text_boundsleft;//0x1E
			__int16 text_boundsbottom;//0x20
			__int16 text_boundsright;//0x22
			string_id string_id;//0x24
			__int16 render_depth_bias;//0x28
			PAD(0x2);//0x2A
		};
		TAG_BLOCK_SIZE_ASSERT(s_text_blocks_block, 0x2C);
		tag_block<s_text_blocks_block> text_blocks;//0x1C
		struct s_bitmap_blocks_block
		{
			enum class e_flags : __int32
			{
				ignore_for_list_skin_size_calculation = FLAG(0),
				swap_on_relative_list_position = FLAG(1),
				render_as_progress_bar = FLAG(2),
			};
			e_flags flags;//0x0
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
			e_animation_index animation_index;//0x4
			__int16 intro_animation_delay_milliseconds;//0x6
			enum class e_bitmap_blend_method : __int16
			{
				standard = 0,
				multiply = 1,
				unused = 2,
			};
			e_bitmap_blend_method bitmap_blend_method;//0x8
			__int16 initial_sprite_frame;//0xA
			__int16 topleft_x;//0xC
			__int16 topleft_y;//0xE
			float horiz_texture_wrapssecond;//0x10
			float vert_texture_wrapssecond;//0x14
			tag_reference bitmap_tag;//0x18
			__int16 render_depth_bias;//0x20
			PAD(0x2);//0x22
			float sprite_animation_speed_fps;//0x24
			__int16 progress_bottomleft_x;//0x28
			__int16 progress_bottomleft_y;//0x2A
			string_id string_identifier;//0x2C
			float progress_scale_i;//0x30
			float progress_scale_j;//0x34
		};
		TAG_BLOCK_SIZE_ASSERT(s_bitmap_blocks_block, 0x38);
		tag_block<s_bitmap_blocks_block> bitmap_blocks;//0x24
		struct s_model_scene_blocks_block
		{
			enum class e_flags : __int32
			{
				unused = FLAG(0),
			};
			e_flags flags;//0x0
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
			e_animation_index animation_index;//0x4
			__int16 intro_animation_delay_milliseconds;//0x6
			__int16 render_depth_bias;//0x8
			PAD(0x2);//0xA
			struct s_objects_block
			{
				tag_string32 name;//0x0
			};
			TAG_BLOCK_SIZE_ASSERT(s_objects_block, 0x20);
			tag_block<s_objects_block> objects;//0xC
			struct s_lights_block
			{
				tag_string32 name;//0x0
			};
			TAG_BLOCK_SIZE_ASSERT(s_lights_block, 0x20);
			tag_block<s_lights_block> lights;//0x14
			PAD(0xC);//0x1C
			float camera_position_x;//0x28
			float camera_position_y;//0x2C
			float camera_position_z;//0x30
			float fov_degress;//0x34
			__int16 ui_viewporttop;//0x38
			__int16 ui_viewportleft;//0x3A
			__int16 ui_viewportbottom;//0x3C
			__int16 ui_viewportright;//0x3E
			string_id unused_intro_anim;//0x40
			string_id unused_outro_anim;//0x44
			string_id unused_ambient_anim;//0x48
		};
		TAG_BLOCK_SIZE_ASSERT(s_model_scene_blocks_block, 0x4C);
		tag_block<s_model_scene_blocks_block> model_scene_blocks;//0x2C
		struct s_textvalue_blocks_block
		{
			tag_string32 name;//0x0
			struct s_text_value_pairs_block
			{
				enum class e_value_type : __int16
				{
					integer_number = 0,
					floating_point_number = 1,
					boolean = 2,
					text_string = 3,
				};
				e_value_type value_type;//0x0
				enum class e_boolean_value : __int16
				{
					e_false = 0,
					e_true = 1,
				};
				e_boolean_value boolean_value;//0x2
				__int32 integer_value;//0x4
				float fp_value;//0x8
				string_id text_value_string_id;//0xC
				string_id text_label_string_id;//0x10
			};
			TAG_BLOCK_SIZE_ASSERT(s_text_value_pairs_block, 0x14);
			tag_block<s_text_value_pairs_block> text_value_pairs;//0x20
		};
		TAG_BLOCK_SIZE_ASSERT(s_textvalue_blocks_block, 0x28);
		tag_block<s_textvalue_blocks_block> textvalue_blocks;//0x34
		struct s_hud_blocks_block
		{
			enum class e_flags : __int32
			{
				ignore_for_list_skin_size = FLAG(0),
				needs_valid_rank = FLAG(1),
			};
			e_flags flags;//0x0
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
			e_animation_index animation_index;//0x4
			__int16 intro_animation_delay_milliseconds;//0x6
			__int16 render_depth_bias;//0x8
			__int16 starting_bitmap_sequence_index;//0xA
			tag_reference bitmap;//0xC
			tag_reference shader;//0x14
			__int16 boundstop;//0x1C
			__int16 boundsleft;//0x1E
			__int16 boundsbottom;//0x20
			__int16 boundsright;//0x22
		};
		TAG_BLOCK_SIZE_ASSERT(s_hud_blocks_block, 0x24);
		tag_block<s_hud_blocks_block> hud_blocks;//0x3C
		struct s_player_blocks_block
		{
			PAD(0x4);//0x0
			tag_reference skin;//0x4
			__int16 bottomleft_x;//0xC
			__int16 bottomleft_y;//0xE
			enum class e_table_order : __int8
			{
				row_major = 0,
				column_major = 1,
			};
			e_table_order table_order;//0x10
			__int8 maximum_player_count;//0x11
			__int8 row_count;//0x12
			__int8 column_count;//0x13
			__int16 row_height;//0x14
			__int16 column_width;//0x16
		};
		TAG_BLOCK_SIZE_ASSERT(s_player_blocks_block, 0x18);
		tag_block<s_player_blocks_block> player_blocks;//0x44
	};
	TAG_BLOCK_SIZE_ASSERT(s_panes_block, 0x4C);
	tag_block<s_panes_block> panes;//0x20
	enum class e_shape_group : __int16
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
	};
	e_shape_group shape_group;//0x28
	PAD(0x2);//0x2A
	string_id header_string_id;//0x2C
	struct s_local_strings_block
	{
		string_id section_name;//0x0
		struct s_local_string_section_references_block
		{
			string_id string_id;//0x0
		};
		TAG_BLOCK_SIZE_ASSERT(s_local_string_section_references_block, 0x4);
		tag_block<s_local_string_section_references_block> local_string_section_references;//0x4
	};
	TAG_BLOCK_SIZE_ASSERT(s_local_strings_block, 0xC);
	tag_block<s_local_strings_block> local_strings;//0x30
	struct s_local_bitmaps_block
	{
		tag_reference bitmap;//0x0
	};
	TAG_BLOCK_SIZE_ASSERT(s_local_bitmaps_block, 0x8);
	tag_block<s_local_bitmaps_block> local_bitmaps;//0x38
	real_color_rgb source_color;//0x40
	real_color_rgb destination_color;//0x4C
	float accumulate_zoom_scale_x;//0x58
	float accumulate_zoom_scale_y;//0x5C
	float refraction_scale_x;//0x60
	float refraction_scale_y;//0x64
	tag_reference mouse_cursor_definition;//0x68
};
TAG_GROUP_SIZE_ASSERT(s_user_interface_screen_widget_definition, 0x70);

