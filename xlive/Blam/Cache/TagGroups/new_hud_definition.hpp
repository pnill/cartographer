#pragma once
#include "Blam\Cache\DataTypes\BlamDataTypes.h"

/*********************************************************************
* name: new_hud_definition
* group_tag : nhdt
* header size : 40
* *********************************************************************/
struct s_new_hud_definition_group_definition
{
	tag_reference do_not_use;//0x0
	enum class e_input : __int8
	{
		basic_zero = 0,
		basic_one = 1,
		basic_time = 2,
		basic_global_hud_fade = 3,
		NUM_1 = 4,
		NUM_2 = 5,
		NUM_3 = 6,
		NUM_4 = 7,
		NUM_5 = 8,
		NUM_6 = 9,
		NUM_7 = 10,
		NUM_8 = 11,
		NUM_9 = 12,
		NUM_10 = 13,
		NUM_11 = 14,
		NUM_12 = 15,
		unit_shield = 16,
		unit_body = 17,
		unit_autoaimed = 18,
		unit_has_no_grenades = 19,
		unit_frag_gren_cnt = 20,
		unit_plasma_gren_cnt = 21,
		unit_time_on_dpl_shld = 22,
		unit_zoom_fraction = 23,
		unit_camo_value = 24,
		NUM_13 = 25,
		NUM_14 = 26,
		NUM_15 = 27,
		NUM_16 = 28,
		NUM_17 = 29,
		NUM_18 = 30,
		NUM_19 = 31,
		parent_shield = 32,
		parent_body = 33,
		NUM_20 = 34,
		NUM_21 = 35,
		NUM_22 = 36,
		NUM_23 = 37,
		NUM_24 = 38,
		NUM_25 = 39,
		NUM_26 = 40,
		NUM_27 = 41,
		NUM_28 = 42,
		NUM_29 = 43,
		NUM_30 = 44,
		NUM_31 = 45,
		NUM_32 = 46,
		NUM_33 = 47,
		weapon_clip_ammo = 48,
		weapon_heat = 49,
		weapon_battery = 50,
		weapon_total_ammo = 51,
		weapon_barrel_spin = 52,
		weapon_overheated = 53,
		weapon_clip_ammo_fraction = 54,
		weapon_time_on_overheat = 55,
		weapon_battery_fraction = 56,
		weapon_locking_fraction = 57,
		NUM_34 = 58,
		NUM_35 = 59,
		NUM_36 = 60,
		NUM_37 = 61,
		NUM_38 = 62,
		NUM_39 = 63,
		NUM_40 = 64,
		user_score_fraction = 65,
		other_user_score_fraction = 66,
		user_winning = 67,
		bomb_arming_amount = 68,
		NUM_41 = 69,
		NUM_42 = 70,
		NUM_43 = 71,
		NUM_44 = 72,
		NUM_45 = 73,
		NUM_46 = 74,
		NUM_47 = 75,
		NUM_48 = 76,
		NUM_49 = 77,
		NUM_50 = 78,
		NUM_51 = 79,
		NUM_52 = 80,
	};
	struct s_bitmap_widgets_block
	{
		string_id name;//0x0
		
		e_input input_1;//0x4
		e_input input_2;//0x5
		e_input input_3;//0x6
		e_input input_4;//0x7
		enum class enable_unit_flags : __int16
		{
			default = FLAG(0),
			grenade_type_is_none = FLAG(1),
			grenade_type_is_frag = FLAG(2),
			grenade_type_is_plasma = FLAG(3),
			unit_is_single_wielding = FLAG(4),
			unit_is_dual_wielding = FLAG(5),
			unit_is_unzoomed = FLAG(6),
			unit_is_zoomed_level_1 = FLAG(7),
			unit_is_zoomed_level_2 = FLAG(8),
			grenades_disabled = FLAG(9),
			binoculars_enabled = FLAG(10),
			motion_sensor_enabled = FLAG(11),
			shield_enabled = FLAG(12),
			dervish = FLAG(13),
		};
		enable_unit_flags enable_unit_flags;//0x8
		enum class enable_extra_flags : __int16
		{
			autoaimfriendly = FLAG(0),
			autoaimplasma = FLAG(1),
			autoaimheadshot = FLAG(2),
			autoaimvulnerable = FLAG(3),
			autoaiminvincible = FLAG(4),
		};
		enable_extra_flags enable_extra_flags;//0xA
		enum class enable_weapon_flags : __int16
		{
			primary_weapon = FLAG(0),
			secondary_weapon = FLAG(1),
			backpack_weapon = FLAG(2),
			age_below_cutoff = FLAG(3),
			clip_below_cutoff = FLAG(4),
			total_below_cutoff = FLAG(5),
			overheated = FLAG(6),
			out_of_ammo = FLAG(7),
			lock_target_available = FLAG(8),
			locking = FLAG(9),
			locked = FLAG(10),
		};
		enable_weapon_flags enable_weapon_flags;//0xC
		enum class enable_game_engine_state_flags : __int16
		{
			campaign_solo = FLAG(0),
			campaign_coop = FLAG(1),
			freeforall = FLAG(2),
			team_game = FLAG(3),
			user_leading = FLAG(4),
			user_not_leading = FLAG(5),
			timed_game = FLAG(6),
			untimed_game = FLAG(7),
			other_score_valid = FLAG(8),
			other_score_invalid = FLAG(9),
			player_is_arming_bomb = FLAG(10),
			player_talking = FLAG(11),
		};
		enable_game_engine_state_flags enable_game_engine_state_flags;//0xE
		enum class disable_unit_flags : __int16
		{
			default = FLAG(0),
			grenade_type_is_none = FLAG(1),
			grenade_type_is_frag = FLAG(2),
			grenade_type_is_plasma = FLAG(3),
			unit_is_single_wielding = FLAG(4),
			unit_is_dual_wielding = FLAG(5),
			unit_is_unzoomed = FLAG(6),
			unit_is_zoomed_level_1 = FLAG(7),
			unit_is_zoomed_level_2 = FLAG(8),
			grenades_disabled = FLAG(9),
			binoculars_enabled = FLAG(10),
			motion_sensor_enabled = FLAG(11),
			shield_enabled = FLAG(12),
			dervish = FLAG(13),
		};
		disable_unit_flags disable_unit_flags;//0x10
		enum class disable_extra_flags : __int16
		{
			autoaimfriendly = FLAG(0),
			autoaimplasma = FLAG(1),
			autoaimheadshot = FLAG(2),
			autoaimvulnerable = FLAG(3),
			autoaiminvincible = FLAG(4),
		};
		disable_extra_flags disable_extra_flags;//0x12
		enum class disable_weapon_flags : __int16
		{
			primary_weapon = FLAG(0),
			secondary_weapon = FLAG(1),
			backpack_weapon = FLAG(2),
			age_below_cutoff = FLAG(3),
			clip_below_cutoff = FLAG(4),
			total_below_cutoff = FLAG(5),
			overheated = FLAG(6),
			out_of_ammo = FLAG(7),
			lock_target_available = FLAG(8),
			locking = FLAG(9),
			locked = FLAG(10),
		};
		disable_weapon_flags disable_weapon_flags;//0x14
		enum class disable_game_engine_state_flags : __int16
		{
			campaign_solo = FLAG(0),
			campaign_coop = FLAG(1),
			freeforall = FLAG(2),
			team_game = FLAG(3),
			user_leading = FLAG(4),
			user_not_leading = FLAG(5),
			timed_game = FLAG(6),
			untimed_game = FLAG(7),
			other_score_valid = FLAG(8),
			other_score_invalid = FLAG(9),
			player_is_arming_bomb = FLAG(10),
			player_talking = FLAG(11),
		};
		disable_game_engine_state_flags disable_game_engine_state_flags;//0x16
		__int8 age_cutoff;//0x18
		__int8 clip_cutoff;//0x19
		__int8 total_cutoff;//0x1A
		PAD(0x1);//0x1B
		enum class e_anchor : __int16
		{
			health_and_shield = 0,
			weapon_hud = 1,
			motion_sensor = 2,
			scoreboard = 3,
			crosshair = 4,
			lockon_target = 5,
		};
		e_anchor anchor;//0x1C
		enum class e_flags : __int16
		{
			flip_horizontally = FLAG(0),
			flip_vertically = FLAG(1),
			scope_mirror_horizontally = FLAG(2),
			scope_mirror_vertically = FLAG(3),
			scope_stretch = FLAG(4),
		};
		e_flags flags;//0x1E
		tag_reference bitmap;//0x20
		tag_reference shader;//0x28
		__int8 fullscreen_sequence_index;//0x30
		__int8 halfscreen_sequence_index;//0x31
		__int8 quarterscreen_sequence_index;//0x32
		PAD(0x1);//0x33
		__int16 fullscreen_offset_x;//0x34
		__int16 fullscreen_offset_y;//0x36
		__int16 halfscreen_offset_x;//0x38
		__int16 halfscreen_offset_y;//0x3A
		__int16 quarterscreen_offset_x;//0x3C
		__int16 quarterscreen_offset_y;//0x3E
		float fullscreen_registration_point_x;//0x40
		float fullscreen_registration_point_y;//0x44
		float halfscreen_registration_point_x;//0x48
		float halfscreen_registration_point_y;//0x4C
		float quarterscreen_registration_point_x;//0x50
		float quarterscreen_registration_point_y;//0x54
		struct s_effect_block
		{
			enum class e_flags : __int16
			{
				apply_scale = FLAG(0),
				apply_theta = FLAG(1),
				apply_offset = FLAG(2),
			};
			e_flags flags;//0x0
			PAD(0x2);//0x2
			string_id input_name_1;//0x4
			string_id range_name_1;//0x8
			float time_period_in_seconds_1;//0xC
			struct s_data_block_1
			{
				__int8 value;//0x0
			};
			TAG_BLOCK_SIZE_ASSERT(s_data_block_1, 0x1);
			tag_block<s_data_block_1> data_1;//0x10
			string_id input_name_2;//0x18
			string_id range_name_2;//0x1C
			float time_period_in_seconds_2;//0x20
			struct s_data_block_2
			{
				__int8 value;//0x0
			};
			TAG_BLOCK_SIZE_ASSERT(s_data_block_2, 0x1);
			tag_block<s_data_block_2> data_2;//0x24
			string_id input_name_3;//0x2C
			string_id range_name_3;//0x30
			float time_period_in_seconds_3;//0x34
			struct s_data_block_3
			{
				__int8 value;//0x0
			};
			TAG_BLOCK_SIZE_ASSERT(s_data_block_3, 0x1);
			tag_block<s_data_block_3> data_3;//0x38
			string_id input_name_4;//0x40
			string_id range_name_4;//0x44
			float time_period_in_seconds_4;//0x48
			struct s_data_block_4
			{
				__int8 value;//0x0
			};
			TAG_BLOCK_SIZE_ASSERT(s_data_block_4, 0x1);
			tag_block<s_data_block_4> data_4;//0x4C
			string_id input_name_5;//0x54
			string_id range_name_5;//0x58
			float time_period_in_seconds_5;//0x5C
			struct s_data_block_5
			{
				__int8 value;//0x0
			};
			TAG_BLOCK_SIZE_ASSERT(s_data_block_5, 0x1);
			tag_block<s_data_block_5> data;//0x60
		};
		TAG_BLOCK_SIZE_ASSERT(s_effect_block, 0x68);
		tag_block<s_effect_block> effect;//0x58
		enum class e_special_hud_type : __int16
		{
			unspecial = 0,
			sb_player_emblem = 1,
			sb_other_player_emblem = 2,
			sb_player_score_meter = 3,
			sb_other_player_score_meter = 4,
			unit_shield_meter = 5,
			motion_sensor = 6,
			territory_meter = 7,
		};
		e_special_hud_type special_hud_type;//0x60
		PAD(0x2);//0x62
	};
	TAG_BLOCK_SIZE_ASSERT(s_bitmap_widgets_block, 0x64);
	tag_block<s_bitmap_widgets_block> bitmap_widgets;//0x8
	struct s_text_widgets_block
	{
		string_id name;//0x0
		e_input input_1;//0x4
		e_input input_2;//0x5
		e_input input_3;//0x6
		e_input input_4;//0x7
		enum class enable_unit_flags : __int16
		{
			default = FLAG(0),
			grenade_type_is_none = FLAG(1),
			grenade_type_is_frag = FLAG(2),
			grenade_type_is_plasma = FLAG(3),
			unit_is_single_wielding = FLAG(4),
			unit_is_dual_wielding = FLAG(5),
			unit_is_unzoomed = FLAG(6),
			unit_is_zoomed_level_1 = FLAG(7),
			unit_is_zoomed_level_2 = FLAG(8),
			grenades_disabled = FLAG(9),
			binoculars_enabled = FLAG(10),
			motion_sensor_enabled = FLAG(11),
			shield_enabled = FLAG(12),
			dervish = FLAG(13),
		};
		enable_unit_flags enable_unit_flags;//0x8
		enum class enable_extra_flags : __int16
		{
			autoaimfriendly = FLAG(0),
			autoaimplasma = FLAG(1),
			autoaimheadshot = FLAG(2),
			autoaimvulnerable = FLAG(3),
			autoaiminvincible = FLAG(4),
		};
		enable_extra_flags enable_extra_flags;//0xA
		enum class enable_weapon_flags : __int16
		{
			primary_weapon = FLAG(0),
			secondary_weapon = FLAG(1),
			backpack_weapon = FLAG(2),
			age_below_cutoff = FLAG(3),
			clip_below_cutoff = FLAG(4),
			total_below_cutoff = FLAG(5),
			overheated = FLAG(6),
			out_of_ammo = FLAG(7),
			lock_target_available = FLAG(8),
			locking = FLAG(9),
			locked = FLAG(10),
		};
		enable_weapon_flags enable_weapon_flags;//0xC
		enum class enable_game_engine_state_flags : __int16
		{
			campaign_solo = FLAG(0),
			campaign_coop = FLAG(1),
			freeforall = FLAG(2),
			team_game = FLAG(3),
			user_leading = FLAG(4),
			user_not_leading = FLAG(5),
			timed_game = FLAG(6),
			untimed_game = FLAG(7),
			other_score_valid = FLAG(8),
			other_score_invalid = FLAG(9),
			player_is_arming_bomb = FLAG(10),
			player_talking = FLAG(11),
		};
		enable_game_engine_state_flags enable_game_engine_state_flags;//0xE
		enum class disable_unit_flags : __int16
		{
			default = FLAG(0),
			grenade_type_is_none = FLAG(1),
			grenade_type_is_frag = FLAG(2),
			grenade_type_is_plasma = FLAG(3),
			unit_is_single_wielding = FLAG(4),
			unit_is_dual_wielding = FLAG(5),
			unit_is_unzoomed = FLAG(6),
			unit_is_zoomed_level_1 = FLAG(7),
			unit_is_zoomed_level_2 = FLAG(8),
			grenades_disabled = FLAG(9),
			binoculars_enabled = FLAG(10),
			motion_sensor_enabled = FLAG(11),
			shield_enabled = FLAG(12),
			dervish = FLAG(13),
		};
		disable_unit_flags disable_unit_flags;//0x10
		enum class disable_extra_flags : __int16
		{
			autoaimfriendly = FLAG(0),
			autoaimplasma = FLAG(1),
			autoaimheadshot = FLAG(2),
			autoaimvulnerable = FLAG(3),
			autoaiminvincible = FLAG(4),
		};
		disable_extra_flags disable_extra_flags;//0x12
		enum class disable_weapon_flags : __int16
		{
			primary_weapon = FLAG(0),
			secondary_weapon = FLAG(1),
			backpack_weapon = FLAG(2),
			age_below_cutoff = FLAG(3),
			clip_below_cutoff = FLAG(4),
			total_below_cutoff = FLAG(5),
			overheated = FLAG(6),
			out_of_ammo = FLAG(7),
			lock_target_available = FLAG(8),
			locking = FLAG(9),
			locked = FLAG(10),
		};
		disable_weapon_flags disable_weapon_flags;//0x14
		enum class disable_game_engine_state_flags : __int16
		{
			campaign_solo = FLAG(0),
			campaign_coop = FLAG(1),
			freeforall = FLAG(2),
			team_game = FLAG(3),
			user_leading = FLAG(4),
			user_not_leading = FLAG(5),
			timed_game = FLAG(6),
			untimed_game = FLAG(7),
			other_score_valid = FLAG(8),
			other_score_invalid = FLAG(9),
			player_is_arming_bomb = FLAG(10),
			player_talking = FLAG(11),
		};
		disable_game_engine_state_flags disable_game_engine_state_flags;//0x16
		__int8 age_cutoff;//0x18
		__int8 clip_cutoff;//0x19
		__int8 total_cutoff;//0x1A
		PAD(0x1);//0x1B
		enum class e_anchor : __int16
		{
			health_and_shield = 0,
			weapon_hud = 1,
			motion_sensor = 2,
			scoreboard = 3,
			crosshair = 4,
			lockon_target = 5,
		};
		e_anchor anchor;//0x1C
		enum class e_flags : __int16
		{
			string_is_a_number = FLAG(0),
			force_2digit_number = FLAG(1),
			force_3digit_number = FLAG(2),
			talking_player_hack = FLAG(3),
		};
		e_flags flags;//0x1E
		tag_reference shader;//0x20
		string_id string;//0x28
		enum class e_justification : __int16
		{
			left = 0,
			center = 1,
			right = 2,
		};
		e_justification justification;//0x2C
		PAD(0x2);//0x2E
		enum class e_fullscreen_font_index : __int8
		{
			defualt = 0,
			number_font = 1,
		};
		e_fullscreen_font_index fullscreen_font_index;//0x30
		enum class e_halfscreen_font_index : __int8
		{
			defualt = 0,
			number_font = 1,
		};
		e_halfscreen_font_index halfscreen_font_index;//0x31
		enum class e_quarterscreen_font_index : __int8
		{
			defualt = 0,
			number_font = 1,
		};
		e_quarterscreen_font_index quarterscreen_font_index;//0x32
		PAD(0x1);//0x33
		float fullscreen_scale;//0x34
		float halfscreen_scale;//0x38
		float quarterscreen_scale;//0x3C
		__int16 fullscreen_offset_x;//0x40
		__int16 fullscreen_offset_y;//0x42
		__int16 halfscreen_offset_x;//0x44
		__int16 halfscreen_offset_y;//0x46
		__int16 quarterscreen_offset_x;//0x48
		__int16 quarterscreen_offset_y;//0x4A
		struct s_effect_block
		{
			enum class e_flags : __int16
			{
				apply_scale = FLAG(0),
				apply_theta = FLAG(1),
				apply_offset = FLAG(2),
			};
			e_flags flags;//0x0
			PAD(0x2);//0x2
			string_id input_name_1;//0x4
			string_id range_name_1;//0x8
			float time_period_in_seconds_1;//0xC
			struct s_data_block_1
			{
				__int8 value;//0x0
			};
			TAG_BLOCK_SIZE_ASSERT(s_data_block_1, 0x1);
			tag_block<s_data_block_1> data_1;//0x10
			string_id input_name_2;//0x18
			string_id range_name_2;//0x1C
			float time_period_in_seconds_2;//0x20
			struct s_data_block_2
			{
				__int8 value;//0x0
			};
			TAG_BLOCK_SIZE_ASSERT(s_data_block_2, 0x1);
			tag_block<s_data_block_2> data_2;//0x24
			string_id input_name_3;//0x2C
			string_id range_name_3;//0x30
			float time_period_in_seconds_3;//0x34
			struct s_data_block_3
			{
				__int8 value;//0x0
			};
			TAG_BLOCK_SIZE_ASSERT(s_data_block_3, 0x1);
			tag_block<s_data_block_3> data_3;//0x38
			string_id input_name_4;//0x40
			string_id range_name_4;//0x44
			float time_period_in_seconds_4;//0x48
			struct s_data_block_4
			{
				__int8 value;//0x0
			};
			TAG_BLOCK_SIZE_ASSERT(s_data_block_4, 0x1);
			tag_block<s_data_block_4> data_4;//0x4C
			string_id input_name_5;//0x54
			string_id range_name_5;//0x58
			float time_period_in_seconds_5;//0x5C
			struct s_data_block_5
			{
				__int8 value;//0x0
			};
			TAG_BLOCK_SIZE_ASSERT(s_data_block_5, 0x1);
			tag_block<s_data_block_5> data;//0x60
		};
		TAG_BLOCK_SIZE_ASSERT(s_effect_block, 0x68);
		tag_block<s_effect_block> effect;//0x4C
	};
	TAG_BLOCK_SIZE_ASSERT(s_text_widgets_block, 0x54);
	tag_block<s_text_widgets_block> text_widgets;//0x10
	__int16 low_clip_cutoff;//0x18
	__int16 low_ammo_cutoff;//0x1A
	float age_cutoff;//0x1C
	struct s_screen_effect_widgets_block
	{
		string_id name;//0x0
		e_input input_1;//0x4
		e_input input_2;//0x5
		e_input input_3;//0x6
		e_input input_4;//0x7
		enum class enable_unit_flags : __int16
		{
			default = FLAG(0),
			grenade_type_is_none = FLAG(1),
			grenade_type_is_frag = FLAG(2),
			grenade_type_is_plasma = FLAG(3),
			unit_is_single_wielding = FLAG(4),
			unit_is_dual_wielding = FLAG(5),
			unit_is_unzoomed = FLAG(6),
			unit_is_zoomed_level_1 = FLAG(7),
			unit_is_zoomed_level_2 = FLAG(8),
			grenades_disabled = FLAG(9),
			binoculars_enabled = FLAG(10),
			motion_sensor_enabled = FLAG(11),
			shield_enabled = FLAG(12),
			dervish = FLAG(13),
		};
		enable_unit_flags enable_unit_flags;//0x8
		enum class enable_extra_flags : __int16
		{
			autoaimfriendly = FLAG(0),
			autoaimplasma = FLAG(1),
			autoaimheadshot = FLAG(2),
			autoaimvulnerable = FLAG(3),
			autoaiminvincible = FLAG(4),
		};
		enable_extra_flags enable_extra_flags;//0xA
		enum class enable_weapon_flags : __int16
		{
			primary_weapon = FLAG(0),
			secondary_weapon = FLAG(1),
			backpack_weapon = FLAG(2),
			age_below_cutoff = FLAG(3),
			clip_below_cutoff = FLAG(4),
			total_below_cutoff = FLAG(5),
			overheated = FLAG(6),
			out_of_ammo = FLAG(7),
			lock_target_available = FLAG(8),
			locking = FLAG(9),
			locked = FLAG(10),
		};
		enable_weapon_flags enable_weapon_flags;//0xC
		enum class enable_game_engine_state_flags : __int16
		{
			campaign_solo = FLAG(0),
			campaign_coop = FLAG(1),
			freeforall = FLAG(2),
			team_game = FLAG(3),
			user_leading = FLAG(4),
			user_not_leading = FLAG(5),
			timed_game = FLAG(6),
			untimed_game = FLAG(7),
			other_score_valid = FLAG(8),
			other_score_invalid = FLAG(9),
			player_is_arming_bomb = FLAG(10),
			player_talking = FLAG(11),
		};
		enable_game_engine_state_flags enable_game_engine_state_flags;//0xE
		enum class disable_unit_flags : __int16
		{
			default = FLAG(0),
			grenade_type_is_none = FLAG(1),
			grenade_type_is_frag = FLAG(2),
			grenade_type_is_plasma = FLAG(3),
			unit_is_single_wielding = FLAG(4),
			unit_is_dual_wielding = FLAG(5),
			unit_is_unzoomed = FLAG(6),
			unit_is_zoomed_level_1 = FLAG(7),
			unit_is_zoomed_level_2 = FLAG(8),
			grenades_disabled = FLAG(9),
			binoculars_enabled = FLAG(10),
			motion_sensor_enabled = FLAG(11),
			shield_enabled = FLAG(12),
			dervish = FLAG(13),
		};
		disable_unit_flags disable_unit_flags;//0x10
		enum class disable_extra_flags : __int16
		{
			autoaimfriendly = FLAG(0),
			autoaimplasma = FLAG(1),
			autoaimheadshot = FLAG(2),
			autoaimvulnerable = FLAG(3),
			autoaiminvincible = FLAG(4),
		};
		disable_extra_flags disable_extra_flags;//0x12
		enum class disable_weapon_flags : __int16
		{
			primary_weapon = FLAG(0),
			secondary_weapon = FLAG(1),
			backpack_weapon = FLAG(2),
			age_below_cutoff = FLAG(3),
			clip_below_cutoff = FLAG(4),
			total_below_cutoff = FLAG(5),
			overheated = FLAG(6),
			out_of_ammo = FLAG(7),
			lock_target_available = FLAG(8),
			locking = FLAG(9),
			locked = FLAG(10),
		};
		disable_weapon_flags disable_weapon_flags;//0x14
		enum class disable_game_engine_state_flags : __int16
		{
			campaign_solo = FLAG(0),
			campaign_coop = FLAG(1),
			freeforall = FLAG(2),
			team_game = FLAG(3),
			user_leading = FLAG(4),
			user_not_leading = FLAG(5),
			timed_game = FLAG(6),
			untimed_game = FLAG(7),
			other_score_valid = FLAG(8),
			other_score_invalid = FLAG(9),
			player_is_arming_bomb = FLAG(10),
			player_talking = FLAG(11),
		};
		disable_game_engine_state_flags disable_game_engine_state_flags;//0x16
		__int8 age_cutoff;//0x18
		__int8 clip_cutoff;//0x19
		__int8 total_cutoff;//0x1A
		PAD(0x1);//0x1B
		enum class e_anchor : __int16
		{
			health_and_shield = 0,
			weapon_hud = 1,
			motion_sensor = 2,
			scoreboard = 3,
			crosshair = 4,
			lockon_target = 5,
		};
		e_anchor anchor;//0x1C
		enum class e_flags : __int16
		{
			unused = FLAG(0),
		};
		e_flags flags;//0x1E
		tag_reference bitmap;//0x20
		tag_reference fullscreen_screen_effect;//0x28
		tag_reference halfscreen_screen_effect;//0x30
		tag_reference quarterscreen_screen_effect;//0x38
		__int8 fullscreen_sequence_index;//0x40
		__int8 halfscreen_sequence_index;//0x41
		__int8 quarterscreen_sequence_index;//0x42
		PAD(0x1);//0x43
		__int16 fullscreen_offset_x;//0x44
		__int16 fullscreen_offset_y;//0x46
		__int16 halfscreen_offset_x;//0x48
		__int16 halfscreen_offset_y;//0x4A
		__int16 quarterscreen_offset_x;//0x4C
		__int16 quarterscreen_offset_y;//0x4E
	};
	TAG_BLOCK_SIZE_ASSERT(s_screen_effect_widgets_block, 0x50);
	tag_block<s_screen_effect_widgets_block> screen_effect_widgets;//0x20
};
TAG_GROUP_SIZE_ASSERT(s_new_hud_definition_group_definition, 0x28);
