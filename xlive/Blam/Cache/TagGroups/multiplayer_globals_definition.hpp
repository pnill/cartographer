#pragma once
#include "Blam\Cache\DataTypes\BlamDataTypes.h"
#include "Blam\Cache\TagGroups.hpp"
#include "Blam\Math\BlamMath.h"

/*********************************************************************
* name: multiplayer_globals
* group_tag : mulg
* header size : 16
* *********************************************************************/

#pragma pack(push,1)
struct s_multiplayer_globals_group_definition :TagGroup<'mulg'>
{
	struct s_universal_block
	{
		tag_reference random_player_names;//0x0
		tag_reference team_names;//0x8
		struct s_team_colors_block
		{
			real_color_rgb color;//0x0
		};
		TAG_BLOCK_SIZE_ASSERT(s_team_colors_block, 0xC);
		tag_block<s_team_colors_block> team_colors;//0x10
		tag_reference multiplayer_text;//0x18
	};
	TAG_BLOCK_SIZE_ASSERT(s_universal_block, 0x20);
	tag_block<s_universal_block> universal;//0x0
	struct s_runtime_block
	{
		tag_reference flag;//0x0
		tag_reference ball;//0x8
		tag_reference unit;//0x10
		tag_reference flag_shader;//0x18
		tag_reference hill_shader;//0x20
		tag_reference head;//0x28
		tag_reference juggernaut_powerup;//0x30
		tag_reference da_bomb;//0x38
		PAD(0x8*5);//0x40		
		struct s_weapons_block
		{
			tag_reference weapon;//0x0
		};
		TAG_BLOCK_SIZE_ASSERT(s_weapons_block, 0x8);
		tag_block<s_weapons_block> weapons;//0x68
		struct s_vehicles_block
		{
			tag_reference vehicle;//0x0
		};
		TAG_BLOCK_SIZE_ASSERT(s_vehicles_block, 0x8);
		tag_block<s_vehicles_block> vehicles;//0x70
		struct s_grenades_block
		{
			tag_reference weapon;//0x0
		};
		TAG_BLOCK_SIZE_ASSERT(s_grenades_block, 0x8);
		tag_block<s_grenades_block> grenades;//0x78
		struct s_powerups_block
		{
			tag_reference weapon;//0x0
		};
		TAG_BLOCK_SIZE_ASSERT(s_powerups_block, 0x8);
		tag_block<s_powerups_block> powerups;//0x80
		tag_reference in_game_text;//0x88
		struct s_sounds_block
		{
			tag_reference sound;//0x0
		};
		TAG_BLOCK_SIZE_ASSERT(s_sounds_block, 0x8);
		tag_block<s_sounds_block> sounds;//0x90
		struct s_general_events_block
		{
			enum class e_flags : __int16
			{
				quantity_message = FLAG(0),
			};
			e_flags flags;//0x0
			PAD(0x2);//0x2
			enum class e_event : __int16
			{
				kill = 0,
				suicide = 1,
				kill_teammate = 2,
				victory = 3,
				team_victory = 4,
				unused1 = 5,
				unused2 = 6,
				NUM_1_min_to_win = 7,
				team_1_min_to_win = 8,
				NUM_30_secs_to_win = 9,
				team_30_secs_to_win = 10,
				player_quit = 11,
				player_joined = 12,
				killed_by_unknown = 13,
				NUM_30_minutes_left = 14,
				NUM_15_minutes_left = 15,
				NUM_5_minutes_left = 16,
				NUM_1_minute_left = 17,
				time_expired = 18,
				game_over = 19,
				respawn_tick = 20,
				last_respawn_tick = 21,
				teleporter_used = 22,
				player_changed_team = 23,
				player_rejoined = 24,
				gained_lead = 25,
				gained_team_lead = 26,
				lost_lead = 27,
				lost_team_lead = 28,
				tied_leader = 29,
				tied_team_leader = 30,
				round_over = 31,
				NUM_30_seconds_left = 32,
				NUM_10_seconds_left = 33,
				kill_falling = 34,
				kill_collision = 35,
				kill_melee = 36,
				sudden_death = 37,
				player_booted_player = 38,
				kill_flag_carrier = 39,
				kill_bomb_carrier = 40,
				kill_sticky_grenade = 41,
				kill_sniper = 42,
				kill_st_melee = 43,
				boarded_vehicle = 44,
				start_team_noti = 45,
				telefrag = 46,
				NUM_10_secs_to_win = 47,
				team_10_secs_to_win = 48,
			};
			e_event event;//0x4
			enum class e_audience : __int16
			{
				cause_player = 0,
				cause_team = 1,
				effect_player = 2,
				effect_team = 3,
				all = 4,
			};
			e_audience audience;//0x6
			PAD(0x4);//0x8
			string_id display_string;//0xC
			enum class e_required_field : __int16
			{
				none = 0,
				cause_player = 1,
				cause_team = 2,
				effect_player = 3,
				effect_team = 4,
			};
			e_required_field required_field;//0x10
			enum class e_excluded_audience : __int16
			{
				none = 0,
				cause_player = 1,
				cause_team = 2,
				effect_player = 3,
				effect_team = 4,
			};
			e_excluded_audience excluded_audience;//0x12
			string_id primary_string;//0x14
			__int32 primary_string_duration;//0x18
			string_id plural_display_string;//0x1C
			PAD(0x1C);//0x20
			float sound_delay_announcer_only;//0x3C
			enum class e_sound_flags : __int16
			{
				announcer_sound = FLAG(0),
			};
			e_sound_flags sound_flags;//0x40
			PAD(0x2);//0x42
			tag_reference sound;//0x44
			tag_reference japanese_sound;//0x4C
			tag_reference german_sound;//0x54
			tag_reference french_sound;//0x5C
			tag_reference spanish_sound;//0x64
			tag_reference italian_sound;//0x6C
			tag_reference korean_sound;//0x74
			tag_reference chinese_sound;//0x7C
			tag_reference portuguese_sound;//0x84
			PAD(0x14);//0x8C
			struct s_sound_permutations_block
			{
				enum class e_sound_flags : __int16
				{
					announcer_sound = FLAG(0),
				};
				e_sound_flags sound_flags;//0x0
				PAD(0x2);//0x2
				tag_reference english_sound;//0x4
				tag_reference japanese_sound;//0xC
				tag_reference german_sound;//0x14
				tag_reference french_sound;//0x1C
				tag_reference spanish_sound;//0x24
				tag_reference italian_sound;//0x2C
				tag_reference korean_sound;//0x34
				tag_reference chinese_sound;//0x3C
				tag_reference portuguese_sound;//0x44
				float probability;//0x4C
			};
			TAG_BLOCK_SIZE_ASSERT(s_sound_permutations_block, 0x50);
			tag_block<s_sound_permutations_block> sound_permutations;//0xA0
		};
		TAG_BLOCK_SIZE_ASSERT(s_general_events_block, 0xA8);
		tag_block<s_general_events_block> general_events;//0x98
		struct s_flavor_events_block
		{
			enum class e_flags : __int16
			{
				quantity_message = FLAG(0),
			};
			e_flags flags;//0x0
			PAD(0x2);//0x2
			enum class e_event : __int16
			{
				double_kill = 0,
				triple_kill = 1,
				killtacular = 2,
				killing_spree = 3,
				running_riot = 4,
				well_placed_kill = 5,
				broke_killing_spree = 6,
				kill_frenzy = 7,
				killtrocity = 8,
				killimajaro = 9,
				NUM_15_in_a_row = 10,
				NUM_20_in_a_row = 11,
				NUM_25_in_a_row = 12,
			};
			e_event event;//0x4
			enum class e_audience : __int16
			{
				cause_player = 0,
				cause_team = 1,
				effect_player = 2,
				effect_team = 3,
				all = 4,
			};
			e_audience audience;//0x6
			PAD(0x4);//0x8
			string_id display_string;//0xC
			enum class e_required_field : __int16
			{
				none = 0,
				cause_player = 1,
				cause_team = 2,
				effect_player = 3,
				effect_team = 4,
			};
			e_required_field required_field;//0x10
			enum class e_excluded_audience : __int16
			{
				none = 0,
				cause_player = 1,
				cause_team = 2,
				effect_player = 3,
				effect_team = 4,
			};
			e_excluded_audience excluded_audience;//0x12
			string_id primary_string;//0x14
			__int32 primary_string_duration;//0x18
			string_id plural_display_string;//0x1C
			PAD(0x1C);//0x20
			float sound_delay_announcer_only;//0x3C
			enum class e_sound_flags : __int16
			{
				announcer_sound = FLAG(0),
			};
			e_sound_flags sound_flags;//0x40
			PAD(0x2);//0x42
			tag_reference sound;//0x44
			tag_reference japanese_sound;//0x4C
			tag_reference german_sound;//0x54
			tag_reference french_sound;//0x5C
			tag_reference spanish_sound;//0x64
			tag_reference italian_sound;//0x6C
			tag_reference korean_sound;//0x74
			tag_reference chinese_sound;//0x7C
			tag_reference portuguese_sound;//0x84
			PAD(0x14);//0x8C
			struct s_sound_permutations_block
			{
				enum class e_sound_flags : __int16
				{
					announcer_sound = FLAG(0),
				};
				e_sound_flags sound_flags;//0x0
				PAD(0x2);//0x2
				tag_reference english_sound;//0x4
				tag_reference japanese_sound;//0xC
				tag_reference german_sound;//0x14
				tag_reference french_sound;//0x1C
				tag_reference spanish_sound;//0x24
				tag_reference italian_sound;//0x2C
				tag_reference korean_sound;//0x34
				tag_reference chinese_sound;//0x3C
				tag_reference portuguese_sound;//0x44
				float probability;//0x4C
			};
			TAG_BLOCK_SIZE_ASSERT(s_sound_permutations_block, 0x50);
			tag_block<s_sound_permutations_block> sound_permutations;//0xA0
		};
		TAG_BLOCK_SIZE_ASSERT(s_flavor_events_block, 0xA8);
		tag_block<s_flavor_events_block> flavor_events;//0xA0
		struct s_slayer_events_block
		{
			enum class e_flags : __int16
			{
				quantity_message = FLAG(0),
			};
			e_flags flags;//0x0
			PAD(0x2);//0x2
			enum class e_event : __int16
			{
				game_start = 0,
				new_target = 1,
			};
			e_event event;//0x4
			enum class e_audience : __int16
			{
				cause_player = 0,
				cause_team = 1,
				effect_player = 2,
				effect_team = 3,
				all = 4,
			};
			e_audience audience;//0x6
			PAD(0x4);//0x8
			string_id display_string;//0xC
			enum class e_required_field : __int16
			{
				none = 0,
				cause_player = 1,
				cause_team = 2,
				effect_player = 3,
				effect_team = 4,
			};
			e_required_field required_field;//0x10
			enum class e_excluded_audience : __int16
			{
				none = 0,
				cause_player = 1,
				cause_team = 2,
				effect_player = 3,
				effect_team = 4,
			};
			e_excluded_audience excluded_audience;//0x12
			string_id primary_string;//0x14
			__int32 primary_string_duration;//0x18
			string_id plural_display_string;//0x1C
			PAD(0x1C);//0x20
			float sound_delay_announcer_only;//0x3C
			enum class e_sound_flags : __int16
			{
				announcer_sound = FLAG(0),
			};
			e_sound_flags sound_flags;//0x40
			PAD(0x2);//0x42
			tag_reference sound;//0x44
			tag_reference japanese_sound;//0x4C
			tag_reference german_sound;//0x54
			tag_reference french_sound;//0x5C
			tag_reference spanish_sound;//0x64
			tag_reference italian_sound;//0x6C
			tag_reference korean_sound;//0x74
			tag_reference chinese_sound;//0x7C
			tag_reference portuguese_sound;//0x84
			PAD(0x14);//0x8C
			struct s_sound_permutations_block
			{
				enum class e_sound_flags : __int16
				{
					announcer_sound = FLAG(0),
				};
				e_sound_flags sound_flags;//0x0
				PAD(0x2);//0x2
				tag_reference english_sound;//0x4
				tag_reference japanese_sound;//0xC
				tag_reference german_sound;//0x14
				tag_reference french_sound;//0x1C
				tag_reference spanish_sound;//0x24
				tag_reference italian_sound;//0x2C
				tag_reference korean_sound;//0x34
				tag_reference chinese_sound;//0x3C
				tag_reference portuguese_sound;//0x44
				float probability;//0x4C
			};
			TAG_BLOCK_SIZE_ASSERT(s_sound_permutations_block, 0x50);
			tag_block<s_sound_permutations_block> sound_permutations;//0xA0
		};
		TAG_BLOCK_SIZE_ASSERT(s_slayer_events_block, 0xA8);
		tag_block<s_slayer_events_block> slayer_events;//0xA8
		struct s_ctf_events_block
		{
			enum class e_flags : __int16
			{
				quantity_message = FLAG(0),
			};
			e_flags flags;//0x0
			PAD(0x2);//0x2
			enum class e_event : __int16
			{
				game_start = 0,
				flag_taken = 1,
				flag_dropped = 2,
				flag_returned_by_player = 3,
				flag_returned_by_timeout = 4,
				flag_captured = 5,
				flag_new_defensive_team = 6,
				flag_return_faliure = 7,
				side_switch_tick = 8,
				side_switch_final_tick = 9,
				side_switch_30_seconds = 10,
				side_switch_10_seconds = 11,
				flag_contested = 12,
				flag_capture_faliure = 13,
			};
			e_event event;//0x4
			enum class e_audience : __int16
			{
				cause_player = 0,
				cause_team = 1,
				effect_player = 2,
				effect_team = 3,
				all = 4,
			};
			e_audience audience;//0x6
			PAD(0x4);//0x8
			string_id display_string;//0xC
			enum class e_required_field : __int16
			{
				none = 0,
				cause_player = 1,
				cause_team = 2,
				effect_player = 3,
				effect_team = 4,
			};
			e_required_field required_field;//0x10
			enum class e_excluded_audience : __int16
			{
				none = 0,
				cause_player = 1,
				cause_team = 2,
				effect_player = 3,
				effect_team = 4,
			};
			e_excluded_audience excluded_audience;//0x12
			string_id primary_string;//0x14
			__int32 primary_string_duration;//0x18
			string_id plural_display_string;//0x1C
			PAD(0x1C);//0x20
			float sound_delay_announcer_only;//0x3C
			enum class e_sound_flags : __int16
			{
				announcer_sound = FLAG(0),
			};
			e_sound_flags sound_flags;//0x40
			PAD(0x2);//0x42
			tag_reference sound;//0x44
			tag_reference japanese_sound;//0x4C
			tag_reference german_sound;//0x54
			tag_reference french_sound;//0x5C
			tag_reference spanish_sound;//0x64
			tag_reference italian_sound;//0x6C
			tag_reference korean_sound;//0x74
			tag_reference chinese_sound;//0x7C
			tag_reference portuguese_sound;//0x84
			PAD(0x14);//0x8C
			struct s_sound_permutations_block
			{
				enum class e_sound_flags : __int16
				{
					announcer_sound = FLAG(0),
				};
				e_sound_flags sound_flags;//0x0
				PAD(0x2);//0x2
				tag_reference english_sound;//0x4
				tag_reference japanese_sound;//0xC
				tag_reference german_sound;//0x14
				tag_reference french_sound;//0x1C
				tag_reference spanish_sound;//0x24
				tag_reference italian_sound;//0x2C
				tag_reference korean_sound;//0x34
				tag_reference chinese_sound;//0x3C
				tag_reference portuguese_sound;//0x44
				float probability;//0x4C
			};
			TAG_BLOCK_SIZE_ASSERT(s_sound_permutations_block, 0x50);
			tag_block<s_sound_permutations_block> sound_permutations;//0xA0
		};
		TAG_BLOCK_SIZE_ASSERT(s_ctf_events_block, 0xA8);
		tag_block<s_ctf_events_block> ctf_events;//0xB0
		struct s_oddball_events_block
		{
			enum class e_flags : __int16
			{
				quantity_message = FLAG(0),
			};
			e_flags flags;//0x0
			PAD(0x2);//0x2
			enum class e_event : __int16
			{
				game_start = 0,
				ball_spawned = 1,
				ball_picked_up = 2,
				ball_dropped = 3,
				ball_reset = 4,
				ball_tick = 5,
			};
			e_event event;//0x4
			enum class e_audience : __int16
			{
				cause_player = 0,
				cause_team = 1,
				effect_player = 2,
				effect_team = 3,
				all = 4,
			};
			e_audience audience;//0x6
			PAD(0x4);//0x8
			string_id display_string;//0xC
			enum class e_required_field : __int16
			{
				none = 0,
				cause_player = 1,
				cause_team = 2,
				effect_player = 3,
				effect_team = 4,
			};
			e_required_field required_field;//0x10
			enum class e_excluded_audience : __int16
			{
				none = 0,
				cause_player = 1,
				cause_team = 2,
				effect_player = 3,
				effect_team = 4,
			};
			e_excluded_audience excluded_audience;//0x12
			string_id primary_string;//0x14
			__int32 primary_string_duration;//0x18
			string_id plural_display_string;//0x1C
			PAD(0x1C);//0x20
			float sound_delay_announcer_only;//0x3C
			enum class e_sound_flags : __int16
			{
				announcer_sound = FLAG(0),
			};
			e_sound_flags sound_flags;//0x40
			PAD(0x2);//0x42
			tag_reference sound;//0x44
			tag_reference japanese_sound;//0x4C
			tag_reference german_sound;//0x54
			tag_reference french_sound;//0x5C
			tag_reference spanish_sound;//0x64
			tag_reference italian_sound;//0x6C
			tag_reference korean_sound;//0x74
			tag_reference chinese_sound;//0x7C
			tag_reference portuguese_sound;//0x84
			PAD(0x14);//0x8C
			struct s_sound_permutations_block
			{
				enum class e_sound_flags : __int16
				{
					announcer_sound = FLAG(0),
				};
				e_sound_flags sound_flags;//0x0
				PAD(0x2);//0x2
				tag_reference english_sound;//0x4
				tag_reference japanese_sound;//0xC
				tag_reference german_sound;//0x14
				tag_reference french_sound;//0x1C
				tag_reference spanish_sound;//0x24
				tag_reference italian_sound;//0x2C
				tag_reference korean_sound;//0x34
				tag_reference chinese_sound;//0x3C
				tag_reference portuguese_sound;//0x44
				float probability;//0x4C
			};
			TAG_BLOCK_SIZE_ASSERT(s_sound_permutations_block, 0x50);
			tag_block<s_sound_permutations_block> sound_permutations;//0xA0
		};
		TAG_BLOCK_SIZE_ASSERT(s_oddball_events_block, 0xA8);
		tag_block<s_oddball_events_block> oddball_events;//0xB8		
		PAD(0x8);//0xC0
		struct s_king_events_block
		{
			enum class e_flags : __int16
			{
				quantity_message = FLAG(0),
			};
			e_flags flags;//0x0
			PAD(0x2);//0x2
			enum class e_event : __int16
			{
				game_start = 0,
				hill_controlled = 1,
				hill_contested = 2,
				hill_tick = 3,
				hill_move = 4,
				hill_controlled_team = 5,
				hill_contested_team = 6,
			};
			e_event event;//0x4
			enum class e_audience : __int16
			{
				cause_player = 0,
				cause_team = 1,
				effect_player = 2,
				effect_team = 3,
				all = 4,
			};
			e_audience audience;//0x6
			PAD(0x4);//0x8
			string_id display_string;//0xC
			enum class e_required_field : __int16
			{
				none = 0,
				cause_player = 1,
				cause_team = 2,
				effect_player = 3,
				effect_team = 4,
			};
			e_required_field required_field;//0x10
			enum class e_excluded_audience : __int16
			{
				none = 0,
				cause_player = 1,
				cause_team = 2,
				effect_player = 3,
				effect_team = 4,
			};
			e_excluded_audience excluded_audience;//0x12
			string_id primary_string;//0x14
			__int32 primary_string_duration;//0x18
			string_id plural_display_string;//0x1C
			PAD(0x1C);//0x20
			float sound_delay_announcer_only;//0x3C
			enum class e_sound_flags : __int16
			{
				announcer_sound = FLAG(0),
			};
			e_sound_flags sound_flags;//0x40
			PAD(0x2);//0x42
			tag_reference sound;//0x44
			tag_reference japanese_sound;//0x4C
			tag_reference german_sound;//0x54
			tag_reference french_sound;//0x5C
			tag_reference spanish_sound;//0x64
			tag_reference italian_sound;//0x6C
			tag_reference korean_sound;//0x74
			tag_reference chinese_sound;//0x7C
			tag_reference portuguese_sound;//0x84
			PAD(0x14);//0x8C
			struct s_sound_permutations_block
			{
				enum class e_sound_flags : __int16
				{
					announcer_sound = FLAG(0),
				};
				e_sound_flags sound_flags;//0x0
				PAD(0x2);//0x2
				tag_reference english_sound;//0x4
				tag_reference japanese_sound;//0xC
				tag_reference german_sound;//0x14
				tag_reference french_sound;//0x1C
				tag_reference spanish_sound;//0x24
				tag_reference italian_sound;//0x2C
				tag_reference korean_sound;//0x34
				tag_reference chinese_sound;//0x3C
				tag_reference portuguese_sound;//0x44
				float probability;//0x4C
			};
			TAG_BLOCK_SIZE_ASSERT(s_sound_permutations_block, 0x50);
			tag_block<s_sound_permutations_block> sound_permutations;//0xA0
		};
		TAG_BLOCK_SIZE_ASSERT(s_king_events_block, 0xA8);
		tag_block<s_king_events_block> king_events;//0xC8		
		PAD(0x8);//0xD0
		struct s_juggernaut_events_block
		{
			enum class e_flags : __int16
			{
				quantity_message = FLAG(0),
			};
			e_flags flags;//0x0
			PAD(0x2);//0x2
			enum class e_event : __int16
			{
				game_start = 0,
				new_juggernaut = 1,
				juggernaut_killed = 2,
			};
			e_event event;//0x4
			enum class e_audience : __int16
			{
				cause_player = 0,
				cause_team = 1,
				effect_player = 2,
				effect_team = 3,
				all = 4,
			};
			e_audience audience;//0x6
			PAD(0x4);//0x8
			string_id display_string;//0xC
			enum class e_required_field : __int16
			{
				none = 0,
				cause_player = 1,
				cause_team = 2,
				effect_player = 3,
				effect_team = 4,
			};
			e_required_field required_field;//0x10
			enum class e_excluded_audience : __int16
			{
				none = 0,
				cause_player = 1,
				cause_team = 2,
				effect_player = 3,
				effect_team = 4,
			};
			e_excluded_audience excluded_audience;//0x12
			string_id primary_string;//0x14
			__int32 primary_string_duration;//0x18
			string_id plural_display_string;//0x1C
			PAD(0x1C);//0x20
			float sound_delay_announcer_only;//0x3C
			enum class e_sound_flags : __int16
			{
				announcer_sound = FLAG(0),
			};
			e_sound_flags sound_flags;//0x40
			PAD(0x2);//0x42
			tag_reference sound;//0x44
			tag_reference japanese_sound;//0x4C
			tag_reference german_sound;//0x54
			tag_reference french_sound;//0x5C
			tag_reference spanish_sound;//0x64
			tag_reference italian_sound;//0x6C
			tag_reference korean_sound;//0x74
			tag_reference chinese_sound;//0x7C
			tag_reference portuguese_sound;//0x84
			PAD(0x14);//0x8C
			struct s_sound_permutations_block
			{
				enum class e_sound_flags : __int16
				{
					announcer_sound = FLAG(0),
				};
				e_sound_flags sound_flags;//0x0
				PAD(0x2);//0x2
				tag_reference english_sound;//0x4
				tag_reference japanese_sound;//0xC
				tag_reference german_sound;//0x14
				tag_reference french_sound;//0x1C
				tag_reference spanish_sound;//0x24
				tag_reference italian_sound;//0x2C
				tag_reference korean_sound;//0x34
				tag_reference chinese_sound;//0x3C
				tag_reference portuguese_sound;//0x44
				float probability;//0x4C
			};
			TAG_BLOCK_SIZE_ASSERT(s_sound_permutations_block, 0x50);
			tag_block<s_sound_permutations_block> sound_permutations;//0xA0
		};
		TAG_BLOCK_SIZE_ASSERT(s_juggernaut_events_block, 0xA8);
		tag_block<s_juggernaut_events_block> juggernaut_events;//0xD8
		struct s_territories_events_block
		{
			enum class e_flags : __int16
			{
				quantity_message = FLAG(0),
			};
			e_flags flags;//0x0
			PAD(0x2);//0x2
			enum class e_event : __int16
			{
				game_start = 0,
				territory_control_gained = 1,
				territory_contest_lost = 2,
				all_territories_cntrld = 3,
				team_territory_ctrl_gained = 4,
				team_territory_ctrl_lost = 5,
				team_all_territories_cntrld = 6,
			};
			e_event event;//0x4
			enum class e_audience : __int16
			{
				cause_player = 0,
				cause_team = 1,
				effect_player = 2,
				effect_team = 3,
				all = 4,
			};
			e_audience audience;//0x6
			PAD(0x4);//0x8
			string_id display_string;//0xC
			enum class e_required_field : __int16
			{
				none = 0,
				cause_player = 1,
				cause_team = 2,
				effect_player = 3,
				effect_team = 4,
			};
			e_required_field required_field;//0x10
			enum class e_excluded_audience : __int16
			{
				none = 0,
				cause_player = 1,
				cause_team = 2,
				effect_player = 3,
				effect_team = 4,
			};
			e_excluded_audience excluded_audience;//0x12
			string_id primary_string;//0x14
			__int32 primary_string_duration;//0x18
			string_id plural_display_string;//0x1C
			PAD(0x1C);//0x20
			float sound_delay_announcer_only;//0x3C
			enum class e_sound_flags : __int16
			{
				announcer_sound = FLAG(0),
			};
			e_sound_flags sound_flags;//0x40
			PAD(0x2);//0x42
			tag_reference sound;//0x44
			tag_reference japanese_sound;//0x4C
			tag_reference german_sound;//0x54
			tag_reference french_sound;//0x5C
			tag_reference spanish_sound;//0x64
			tag_reference italian_sound;//0x6C
			tag_reference korean_sound;//0x74
			tag_reference chinese_sound;//0x7C
			tag_reference portuguese_sound;//0x84
			PAD(0x14);//0x8C
			struct s_sound_permutations_block
			{
				enum class e_sound_flags : __int16
				{
					announcer_sound = FLAG(0),
				};
				e_sound_flags sound_flags;//0x0
				PAD(0x2);//0x2
				tag_reference english_sound;//0x4
				tag_reference japanese_sound;//0xC
				tag_reference german_sound;//0x14
				tag_reference french_sound;//0x1C
				tag_reference spanish_sound;//0x24
				tag_reference italian_sound;//0x2C
				tag_reference korean_sound;//0x34
				tag_reference chinese_sound;//0x3C
				tag_reference portuguese_sound;//0x44
				float probability;//0x4C
			};
			TAG_BLOCK_SIZE_ASSERT(s_sound_permutations_block, 0x50);
			tag_block<s_sound_permutations_block> sound_permutations;//0xA0
		};
		TAG_BLOCK_SIZE_ASSERT(s_territories_events_block, 0xA8);
		tag_block<s_territories_events_block> territories_events;//0xE0
		struct s_invasion_events_block
		{
			enum class e_flags : __int16
			{
				quantity_message = FLAG(0),
			};
			e_flags flags;//0x0
			PAD(0x2);//0x2
			enum class e_event : __int16
			{
				game_start = 0,
				bomb_taken = 1,
				bomb_dropped = 2,
				bomb_returned_by_player = 3,
				bomb_returned_by_timeout = 4,
				bomb_captured = 5,
				bomb_new_defensive_team = 6,
				bomb_return_faliure = 7,
				side_switch_tick = 8,
				side_switch_final_tick = 9,
				side_switch_30_seconds = 10,
				side_switch_10_seconds = 11,
				bomb_returned_by_defusing = 12,
				bomb_placed_on_enemy_post = 13,
				bomb_arming_started = 14,
				bomb_arming_completed = 15,
				bomb_contested = 16,
			};
			e_event event;//0x4
			enum class e_audience : __int16
			{
				cause_player = 0,
				cause_team = 1,
				effect_player = 2,
				effect_team = 3,
				all = 4,
			};
			e_audience audience;//0x6
			PAD(0x4);//0x8
			string_id display_string;//0xC
			enum class e_required_field : __int16
			{
				none = 0,
				cause_player = 1,
				cause_team = 2,
				effect_player = 3,
				effect_team = 4,
			};
			e_required_field required_field;//0x10
			enum class e_excluded_audience : __int16
			{
				none = 0,
				cause_player = 1,
				cause_team = 2,
				effect_player = 3,
				effect_team = 4,
			};
			e_excluded_audience excluded_audience;//0x12
			string_id primary_string;//0x14
			__int32 primary_string_duration;//0x18
			string_id plural_display_string;//0x1C
			PAD(0x1C);//0x20
			float sound_delay_announcer_only;//0x3C
			enum class e_sound_flags : __int16
			{
				announcer_sound = FLAG(0),
			};
			e_sound_flags sound_flags;//0x40
			PAD(0x2);//0x42
			tag_reference sound;//0x44
			tag_reference japanese_sound;//0x4C
			tag_reference german_sound;//0x54
			tag_reference french_sound;//0x5C
			tag_reference spanish_sound;//0x64
			tag_reference italian_sound;//0x6C
			tag_reference korean_sound;//0x74
			tag_reference chinese_sound;//0x7C
			tag_reference portuguese_sound;//0x84
			PAD(0x14);//0x8C
			struct s_sound_permutations_block
			{
				enum class e_sound_flags : __int16
				{
					announcer_sound = FLAG(0),
				};
				e_sound_flags sound_flags;//0x0
				PAD(0x2);//0x2
				tag_reference english_sound;//0x4
				tag_reference japanese_sound;//0xC
				tag_reference german_sound;//0x14
				tag_reference french_sound;//0x1C
				tag_reference spanish_sound;//0x24
				tag_reference italian_sound;//0x2C
				tag_reference korean_sound;//0x34
				tag_reference chinese_sound;//0x3C
				tag_reference portuguese_sound;//0x44
				float probability;//0x4C
			};
			TAG_BLOCK_SIZE_ASSERT(s_sound_permutations_block, 0x50);
			tag_block<s_sound_permutations_block> sound_permutations;//0xA0
		};
		TAG_BLOCK_SIZE_ASSERT(s_invasion_events_block, 0xA8);
		tag_block<s_invasion_events_block> invasion_events;//0xE8
		PAD(0x8);//0xF0
		PAD(0x8);//0xF8
		PAD(0x8);//0x100
		PAD(0x8);//0x108
		tag_reference default_item_collection_1;//0x110
		tag_reference default_item_collection_2;//0x118
		__int32 default_frag_grenade_count;//0x120
		__int32 default_plasma_grenade_count;//0x124
		PAD(0x28);//0x128
		float dynamic_zone_upper_height;//0x150
		float dynamic_zone_lower_height;//0x154
		PAD(0x28);//0x158
		float enemy_inner_radius;//0x180
		float enemy_outer_radius;//0x184
		float enemy_weight;//0x188
		PAD(0x10);//0x18C
		float friend_inner_radius;//0x19C
		float friend_outer_radius;//0x1A0
		float friend_weight;//0x1A4
		PAD(0x10);//0x1A8
		float enemy_vehicle_inner_radius;//0x1B8
		float enemy_vehicle_outer_radius;//0x1BC
		float enemy_vehicle_weight;//0x1C0
		PAD(0x10);//0x1C4
		float friendly_vehicle_inner_radius;//0x1D4
		float friendly_vehicle_outer_radius;//0x1D8
		float friendly_vehicle_weight;//0x1DC
		PAD(0x10);//0x1E0
		float empty_vehicle_inner_radius;//0x1F0
		float empty_vehicle_outer_radius;//0x1F4
		float empty_vehicle_weight;//0x1F8
		PAD(0x10);//0x1FC
		float oddball_inclusion_inner_radius;//0x20C
		float oddball_inclusion_outer_radius;//0x210
		float oddball_inclusion_weight;//0x214
		PAD(0x10);//0x218
		float oddball_exclusion_inner_radius;//0x228
		float oddball_exclusion_outer_radius;//0x22C
		float oddball_exclusion_weight;//0x230
		PAD(0x10);//0x234
		float hill_inclusion_inner_radius;//0x244
		float hill_inclusion_outer_radius;//0x248
		float hill_inclusion_weight;//0x24C
		PAD(0x10);//0x250
		float hill_exclusion_inner_radius;//0x260
		float hill_exclusion_outer_radius;//0x264
		float hill_exclusion_weight;//0x268
		PAD(0x10);//0x26C
		float last_race_flag_inner_radius;//0x27C
		float last_race_flag_outer_radius;//0x280
		float last_race_flag_weight;//0x284
		PAD(0x10);//0x288
		float dead_ally_inner_radius;//0x298
		float dead_ally_outer_radius;//0x29C
		float dead_ally_weight;//0x2A0
		PAD(0x10);//0x2A4
		float controlled_territory_inner_radius;//0x2B4
		float controlled_territory_outer_radius;//0x2B8
		float controlled_territory_weight;//0x2BC
		PAD(0x270);//0x2C0
		struct s_multiplayer_constants_block
		{
			float maximum_random_spawn_bias;//0x0
			float teleporter_recharge_time;//0x4
			float grenade_danger_weight;//0x8
			float grenade_danger_inner_radius;//0xC
			float grenade_danger_outer_radius;//0x10
			float grenade_danger_lead_time;//0x14
			float vehicle_danger_min_speed;//0x18
			float vehicle_danger_weight;//0x1C
			float vehicle_danger_radius;//0x20
			float vehicle_danger_lead_time;//0x24
			float vehicle_nearby_player_dist;//0x28
			PAD(0x94);//0x2C
			tag_reference hill_shader;//0xC0
			PAD(0x10);//0xC8
			float flag_reset_stop_distance;//0xD8
			tag_reference bomb_explode_effect;//0xDC
			tag_reference bomb_explode_dmg_effect;//0xE4
			tag_reference bomb_defuse_effect;//0xEC
			string_id bomb_defusal_string;//0xF4
			string_id blocked_teleporter_string;//0xF8
			PAD(0x64);//0xFC
		};
		TAG_BLOCK_SIZE_ASSERT(s_multiplayer_constants_block, 0x160);
		tag_block<s_multiplayer_constants_block> multiplayer_constants;//0x530
		struct s_state_responses_block
		{
			enum class e_flags : __int16
			{
				unused = FLAG(0),
			};
			e_flags flags;//0x0
			PAD(0x2);//0x2
			enum class e_state : __int16
			{
				waiting_for_space_to_clear = 0,
				observing = 1,
				respawning_soon = 2,
				sitting_out = 3,
				out_of_lives = 4,
				playing_winning = 5,
				playing_tied = 6,
				playing_losing = 7,
				game_over_won = 8,
				game_over_tied = 9,
				game_over_lost = 10,
				you_have_flag = 11,
				enemy_has_flag = 12,
				flag_not_home = 13,
				carrying_oddball = 14,
				you_are_juggy = 15,
				you_control_hill = 16,
				switching_sides_soon = 17,
				player_recently_started = 18,
				you_have_bomb = 19,
				flag_contested = 20,
				bomb_contested = 21,
				limited_lives_left_multiple = 22,
				limited_lives_left_single = 23,
				limited_lives_left_final = 24,
				playing_winning_unlimited = 25,
				playing_tied_unlimited = 26,
				playing_losing_unlimited = 27,
			};
			e_state state;//0x4
			PAD(0x2);//0x6
			string_id ffa_message;//0x8
			string_id team_message;//0xC
			tag_reference NUM_;//0x10
			PAD(0x4);//0x18
		};
		TAG_BLOCK_SIZE_ASSERT(s_state_responses_block, 0x1C);
		tag_block<s_state_responses_block> state_responses;//0x538
		tag_reference scoreboard_hud_definition;//0x540
		tag_reference scoreboard_emblem_shader;//0x548
		tag_reference scoreboard_emblem_bitmap;//0x550
		tag_reference scoreboard_dead_emblem_shader;//0x558
		tag_reference scoreboard_dead_emblem_bitmap;//0x560
	};
	TAG_BLOCK_SIZE_ASSERT(s_runtime_block, 0x568);
	tag_block<s_runtime_block> runtime;//0x8
};
TAG_GROUP_SIZE_ASSERT(s_multiplayer_globals_group_definition, 0x10);
#pragma pack(pop)

