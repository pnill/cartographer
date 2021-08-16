#pragma once
#include "Blam\Cache\DataTypes\BlamDataTypes.h"
#include "Blam\Cache\TagGroups.hpp"
#include "Blam\Math\BlamMath.h"

/*********************************************************************
* name: globals
* group_tag : matg
* header size : 644
* *********************************************************************/

#pragma pack(push,1)
struct s_globals_group_definition :TagGroup<'matg'>
{
	PAD(0xAC);//0x0
	enum class e_language : __int32
	{
		english = 0,
		japanese = 1,
		german = 2,
		french = 3,
		spanish = 4,
		italian = 5,
		korean = 6,
		chinese = 7,
		portuguese = 8,
	};
	e_language language;//0xAC
	struct s_havok_cleanup_resources_block
	{
		tag_reference object_cleanup_effect;//0x0
	};
	TAG_BLOCK_SIZE_ASSERT(s_havok_cleanup_resources_block, 0x8);
	tag_block<s_havok_cleanup_resources_block> havok_cleanup_resources;//0xB0
	struct s_collision_damage_block
	{
		tag_reference collision_damage;//0x0
		float min_game_acc_default;//0x8
		float max_game_acc_default;//0xC
		float min_game_scale_default;//0x10
		float max_game_scale_default;//0x14
		float min_abs_acc_default;//0x18
		float max_abs_acc_default;//0x1C
		float min_abs_scale_default;//0x20
		float max_abs_scale_default;//0x24
		PAD(0x20);//0x28
	};
	TAG_BLOCK_SIZE_ASSERT(s_collision_damage_block, 0x48);
	tag_block<s_collision_damage_block> collision_damage;//0xB8
	struct s_sound_globals_block
	{
		tag_reference sound_classes;//0x0
		tag_reference sound_effects;//0x8
		tag_reference sound_mix;//0x10
		tag_reference sound_combat_dialogue_constants;//0x18
		datum sound_gestalt_reference;//0x20
	};
	TAG_BLOCK_SIZE_ASSERT(s_sound_globals_block, 0x24);
	tag_block<s_sound_globals_block> sound_globals;//0xC0
	struct s_ai_globals_block
	{
		float danger_broadly_facing;//0x0
		PAD(0x4);//0x4
		float danger_shooting_near;//0x8
		PAD(0x4);//0xC
		float danger_shooting_at;//0x10
		PAD(0x4);//0x14
		float danger_extremely_close;//0x18
		PAD(0x4);//0x1C
		float danger_shield_damage;//0x20
		float danger_exetended_shield_damage;//0x24
		float danger_body_damage;//0x28
		float danger_extended_body_damage;//0x2C
		PAD(0x30);//0x30
		tag_reference global_dialogue_tag;//0x60
		string_id default_mission_dialogue_sound_effect;//0x68
		PAD(0x14);//0x6C
		float jump_down;//0x80
		float jump_step;//0x84
		float jump_crouch;//0x88
		float jump_stand;//0x8C
		float jump_storey;//0x90
		float jump_tower;//0x94
		float max_jump_down_height_down;//0x98
		float max_jump_down_height_step;//0x9C
		float max_jump_down_height_crouch;//0xA0
		float max_jump_down_height_stand;//0xA4
		float max_jump_down_height_storey;//0xA8
		float max_jump_down_height_tower;//0xAC
		real_bounds hoist_step;//0xB0		
		real_bounds hoist_crouch;//0xB8		
		real_bounds hoist_stand;//0xC0		
		PAD(0x18);//0xC8
		real_bounds vault_step;//0xE0		
		real_bounds vault_crouch;//0xE8		
		PAD(0x30);//0xF0
		struct s_gravemind_properties_block
		{
			float min_retreat_time;//0x0
			float ideal_retreat_time;//0x4
			float max_retreat_time;//0x8
		};
		TAG_BLOCK_SIZE_ASSERT(s_gravemind_properties_block, 0xC);
		tag_block<s_gravemind_properties_block> gravemind_properties;//0x120
		PAD(0x30);//0x128
		float scary_target_threhold;//0x158
		float scary_weapon_threhold;//0x15C
		float player_scariness;//0x160
		float berserking_actor_scariness;//0x164
	};
	TAG_BLOCK_SIZE_ASSERT(s_ai_globals_block, 0x168);
	tag_block<s_ai_globals_block> ai_globals;//0xC8
	struct s_damage_table_block
	{
		struct s_damage_groups_block
		{
			string_id name;//0x0
			struct s_armor_modifiers_block
			{
				string_id name;//0x0
				float damage_multiplier;//0x4
			};
			TAG_BLOCK_SIZE_ASSERT(s_armor_modifiers_block, 0x8);
			tag_block<s_armor_modifiers_block> armor_modifiers;//0x4
		};
		TAG_BLOCK_SIZE_ASSERT(s_damage_groups_block, 0xC);
		tag_block<s_damage_groups_block> damage_groups;//0x0
	};
	TAG_BLOCK_SIZE_ASSERT(s_damage_table_block, 0x8);
	tag_block<s_damage_table_block> damage_table;//0xD0	
	PAD(0x8);//0xD8
	struct s_sounds_block
	{
		tag_reference sound_obsolete;//0x0
	};
	TAG_BLOCK_SIZE_ASSERT(s_sounds_block, 0x8);
	tag_block<s_sounds_block> sounds;//0xE0
	struct s_camera_block
	{
		tag_reference default_unit_camera_track;//0x0
		float default_change_pause;//0x8
		float first_person_change_pause;//0xC
		float following_camera_change_pause;//0x10
	};
	TAG_BLOCK_SIZE_ASSERT(s_camera_block, 0x14);
	tag_block<s_camera_block> camera;//0xE8
	struct s_player_control_block
	{
		float magnetism_friction;//0x0
		float magnetism_adhesion;//0x4
		float inconsequential_target_scale;//0x8
		PAD(0xC);//0xC
		real_point2d crosshair_location;//0x18		
		float seconds_to_start;//0x20
		float seconds_to_full_speed;//0x24
		float decay_rate;//0x28
		float full_speed_multiplier;//0x2C
		float pegged_magnitude;//0x30
		float pegged_angular_threshold;//0x34
		PAD(0x8);//0x38
		float look_default_pitch_rate;//0x40
		float look_default_yaw_rate;//0x44
		float look_peg_threshold;//0x48
		float look_yaw_acceleration_time;//0x4C
		float look_yaw_acceleration_scale;//0x50
		float look_pitch_acceleration_time;//0x54
		float look_pitch_acceleration_scale;//0x58
		float look_autolevelling_scale;//0x5C
		PAD(0x8);//0x60
		float gravity_scale;//0x68
		PAD(0x2);//0x6C
		__int16 minimum_autolevelling_ticks;//0x6E
		angle minimum_angle_for_vehicle_flipping;//0x70
		struct s_look_function_block
		{
			float scale;//0x0
		};
		TAG_BLOCK_SIZE_ASSERT(s_look_function_block, 0x4);
		tag_block<s_look_function_block> look_function;//0x74
		float minimum_action_hold_time;//0x7C
	};
	TAG_BLOCK_SIZE_ASSERT(s_player_control_block, 0x80);
	tag_block<s_player_control_block> player_control;//0xF0
	struct s_difficulty_block
	{
		float easy_enemy_damage;//0x0
		float normal_enemy_damage;//0x4
		float hard_enemy_damage;//0x8
		float imposs_enemy_damage;//0xC
		float easy_enemy_vitality;//0x10
		float normal_enemy_vitality;//0x14
		float hard_enemy_vitality;//0x18
		float imposs_enemy_vitality;//0x1C
		float easy_enemy_shield;//0x20
		float normal_enemy_shield;//0x24
		float hard_enemy_shield;//0x28
		float imposs_enemy_shield;//0x2C
		float easy_enemy_recharge;//0x30
		float normal_enemy_recharge;//0x34
		float hard_enemy_recharge;//0x38
		float imposs_enemy_recharge;//0x3C
		float easy_friend_damage;//0x40
		float normal_friend_damage;//0x44
		float hard_friend_damage;//0x48
		float imposs_friend_damage;//0x4C
		float easy_friend_vitality;//0x50
		float normal_friend_vitality;//0x54
		float hard_friend_vitality;//0x58
		float imposs_friend_vitality;//0x5C
		float easy_friend_shield;//0x60
		float normal_friend_shield;//0x64
		float hard_friend_shield;//0x68
		float imposs_friend_shield;//0x6C
		float easy_friend_recharge;//0x70
		float normal_friend_recharge;//0x74
		float hard_friend_recharge;//0x78
		float imposs_friend_recharge;//0x7C
		float easy_infection_forms;//0x80
		float normal_infection_forms;//0x84
		float hard_infection_forms;//0x88
		float imposs_infection_forms;//0x8C
		PAD(0x10);//0x90
		float easy_rate_of_fire;//0xA0
		float normal_rate_of_fire;//0xA4
		float hard_rate_of_fire;//0xA8
		float imposs_rate_of_fire;//0xAC
		float easy_projectile_error;//0xB0
		float normal_projectile_error;//0xB4
		float hard_projectile_error;//0xB8
		float imposs_projectile_error;//0xBC
		float easy_burst_error;//0xC0
		float normal_burst_error;//0xC4
		float hard_burst_error;//0xC8
		float imposs_burst_error;//0xCC
		float easy_new_target_delay;//0xD0
		float normal_new_target_delay;//0xD4
		float hard_new_target_delay;//0xD8
		float imposs_new_target_delay;//0xDC
		float easy_burst_separation;//0xE0
		float normal_burst_separation;//0xE4
		float hard_burst_separation;//0xE8
		float imposs_burst_separation;//0xEC
		float easy_target_tracking;//0xF0
		float normal_target_tracking;//0xF4
		float hard_target_tracking;//0xF8
		float imposs_target_tracking;//0xFC
		float easy_target_leading;//0x100
		float normal_target_leading;//0x104
		float hard_target_leading;//0x108
		float imposs_target_leading;//0x10C
		float easy_overcharge_chance;//0x110
		float normal_overcharge_chance;//0x114
		float hard_overcharge_chance;//0x118
		float imposs_overcharge_chance;//0x11C
		float easy_special_fire_delay;//0x120
		float normal_special_fire_delay;//0x124
		float hard_special_fire_delay;//0x128
		float imposs_special_fire_delay;//0x12C
		float easy_guidance_vs_player;//0x130
		float normal_guidance_vs_player;//0x134
		float hard_guidance_vs_player;//0x138
		float imposs_guidance_vs_player;//0x13C
		float easy_melee_delay_base;//0x140
		float normal_melee_delay_base;//0x144
		float hard_melee_delay_base;//0x148
		float imposs_melee_delay_base;//0x14C
		float easy_melee_delay_scale;//0x150
		float normal_melee_delay_scale;//0x154
		float hard_melee_delay_scale;//0x158
		float imposs_melee_delay_scale;//0x15C
		PAD(0x10);//0x160
		float easy_grenade_chance_scale;//0x170
		float normal_grenade_chance_scale;//0x174
		float hard_grenade_chance_scale;//0x178
		float imposs_grenade_chance_scaley;//0x17C
		float easy_grenade_timer_scale;//0x180
		float normal_grenade_timer_scale;//0x184
		float hard_grenade_timer_scale;//0x188
		float imposs_grenade_timer_scale;//0x18C
		PAD(0x30);//0x190
		float easy_major_upgrade_normal;//0x1C0
		float normal_major_upgrade_normal;//0x1C4
		float hard_major_upgrade_normaly;//0x1C8
		float imposs_major_upgrade_normal;//0x1CC
		float easy_major_upgrade_few;//0x1D0
		float normal_major_upgrade_few;//0x1D4
		float hard_major_upgrade_few;//0x1D8
		float imposs_major_upgrade_few;//0x1DC
		float easy_major_upgrade_many;//0x1E0
		float normal_major_upgrade_many;//0x1E4
		float hard_major_upgrade_many;//0x1E8
		float imposs_major_upgrade_many;//0x1EC
		float easy_player_vehicle_ram_chance;//0x1F0
		float normal_player_vehicle_ram_chance;//0x1F4
		float hard_player_vehicle_ram_chance;//0x1F8
		float imposs_player_vehicle_ram_chance;//0x1FC
		PAD(0x84);//0x200
	};
	TAG_BLOCK_SIZE_ASSERT(s_difficulty_block, 0x284);
	tag_block<s_difficulty_block> difficulty;//0xF8
	struct s_grenades_block
	{
		__int16 maximum_count;//0x0
		PAD(0x2);//0x2
		tag_reference throwing_effect;//0x4
		PAD(0x10);//0xC
		tag_reference equipment;//0x1C
		tag_reference projectile;//0x24
	};
	TAG_BLOCK_SIZE_ASSERT(s_grenades_block, 0x2C);
	tag_block<s_grenades_block> grenades;//0x100
	struct s_rasterizer_data_block
	{
		tag_reference distance_attenuation;//0x0
		tag_reference vector_normalization;//0x8
		tag_reference gradients;//0x10
		tag_reference unused0;//0x18
		tag_reference unused1;//0x20
		tag_reference unused2;//0x28
		tag_reference glow;//0x30
		tag_reference unused3;//0x38
		tag_reference unused4;//0x40
		PAD(0x10);//0x48
		struct s_global_vertex_shaders_block
		{
			tag_reference vertex_shader;//0x0
		};
		TAG_BLOCK_SIZE_ASSERT(s_global_vertex_shaders_block, 0x8);
		tag_block<s_global_vertex_shaders_block> global_vertex_shaders;//0x58
		struct
		{
			tag_reference default_2d;//0x60
			tag_reference default_3d;//0x68
			tag_reference default_cube_map;//0x70

		}default_textures;


		//experimental textures
		tag_reference unused5;//0x78
		tag_reference unused6;//0x80
		tag_reference unused7;//0x88
		tag_reference unused8;//0x90
		tag_reference unused9;//0x98
		tag_reference unused10;//0xA0
		PAD(0x24);//0xA8


		tag_reference global_shader;//0xCC


		struct
		{
			enum class e_flags : __int16
			{
				tint_edge_density = FLAG(0),
			};
			e_flags flags;//0xD4
			PAD(0x2);//0xD6
			float refraction_amount;//0xD8
			float distance_falloff;//0xDC
			real_color_rgb tint_color;//0xE0
			float hyperstealth_refraction;//0xEC
			float hyperstealth_distance_falloff;//0xF0
			real_color_rgb hyperstealth_tint_color;//0xF4
		}active_camouflage;

		//pc_textures
		tag_reference unused11;//0x100
	};
	TAG_BLOCK_SIZE_ASSERT(s_rasterizer_data_block, 0x108);
	tag_block<s_rasterizer_data_block> rasterizer_data;//0x108
	struct s_interface_tags_block
	{
		tag_reference obsolete1;//0x0
		tag_reference obsolete2;//0x8
		tag_reference screen_color_table;//0x10
		tag_reference hud_color_table;//0x18
		tag_reference editor_color_table;//0x20
		tag_reference dialog_color_table;//0x28
		tag_reference hud_globals;//0x30
		tag_reference motion_sensor_sweep_bitmap;//0x38
		tag_reference motion_sensor_sweep_bitmap_mask;//0x40
		tag_reference multiplayer_hud_bitmap;//0x48
		tag_reference pad;//0x50
		tag_reference hud_digits_definition;//0x58
		tag_reference motion_sensor_blip_bitmap;//0x60
		tag_reference interface_goo_map1;//0x68
		tag_reference interface_goo_map2;//0x70
		tag_reference interface_goo_map3;//0x78
		tag_reference mainmenu_ui_globals;//0x80
		tag_reference singleplayer_ui_globals;//0x88
		tag_reference multiplayer_ui_globals;//0x90
	};
	TAG_BLOCK_SIZE_ASSERT(s_interface_tags_block, 0x98);
	tag_block<s_interface_tags_block> interface_tags;//0x110
	struct s_weapon_list_update__weapon_list_enum_in_game_globalsh_block
	{
		tag_reference weapon;//0x0
	};
	TAG_BLOCK_SIZE_ASSERT(s_weapon_list_update__weapon_list_enum_in_game_globalsh_block, 0x8);
	tag_block < s_weapon_list_update__weapon_list_enum_in_game_globalsh_block> weapon_list_update__weapon_list_enum_in_game_globalsh;//0x118
	struct s_cheat_powerups_block
	{
		tag_reference powerup;//0x0
	};
	TAG_BLOCK_SIZE_ASSERT(s_cheat_powerups_block, 0x8);
	tag_block < s_cheat_powerups_block> cheat_powerups;//0x120
	struct s_multiplayer_information_block
	{
		tag_reference flag;//0x0
		tag_reference unit;//0x8
		struct s_vehicles_block
		{
			tag_reference vehicle;//0x0
		};
		TAG_BLOCK_SIZE_ASSERT(s_vehicles_block, 0x8);
		tag_block<s_vehicles_block> vehicles;//0x10
		tag_reference hill_shader;//0x18
		tag_reference flag_shader;//0x20
		tag_reference ball;//0x28
		struct s_sounds_block
		{
			tag_reference sound;//0x0
		};
		TAG_BLOCK_SIZE_ASSERT(s_sounds_block, 0x8);
		tag_block<s_sounds_block> sounds;//0x30
		tag_reference in_game_text;//0x38
		PAD(0x28);//0x40
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
		tag_block<s_general_events_block> general_events;//0x68
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
		tag_block<s_slayer_events_block> slayer_events;//0x70
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
		tag_block<s_ctf_events_block> ctf_events;//0x78
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
		tag_block<s_oddball_events_block> oddball_events;//0x80		
		PAD(0x8);//0x88
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
		tag_block<s_king_events_block> king_events;//0x90
	};
	TAG_BLOCK_SIZE_ASSERT(s_multiplayer_information_block, 0x98);
	tag_block < s_multiplayer_information_block> multiplayer_information;//0x128
	struct s_player_information_block
	{
		tag_reference unused;//0x0
		PAD(0x1C);//0x8
		float walking_speed;//0x24
		PAD(0x4);//0x28
		float run_forward;//0x2C
		float run_backward;//0x30
		float run_sideways;//0x34
		float run_acceleration;//0x38
		float sneak_forward;//0x3C
		float sneak_backward;//0x40
		float sneak_sideways;//0x44
		float sneak_acceleration;//0x48
		float airborne_acceleration;//0x4C
		PAD(0x10);//0x50
		float grenade_origin_x;//0x60
		float grenade_origin_y;//0x64
		float grenade_origin_z;//0x68
		PAD(0xC);//0x6C
		float stun_movement_penalty;//0x78
		float stun_turning_penalty;//0x7C
		float stun_jumping_penalty;//0x80
		float minimum_stun_time;//0x84
		float maximum_stun_time;//0x88
		PAD(0x8);//0x8C
		real_bounds first_person_idle_time;//0x94		
		float first_person_skip_fraction;//0x9C
		PAD(0x10);//0xA0
		tag_reference coop_respawn_effect;//0xB0
		__int32 binoculars_zoom_count;//0xB8
		float binoculars_zoom_range_lower;//0xBC
		float binoculars_zoom_range_upper;//0xC0
		tag_reference binoculars_zoom_in_sound;//0xC4
		tag_reference binoculars_zoom_out_sound;//0xCC
		PAD(0x10);//0xD4
		tag_reference active_camouflage_on;//0xE4
		tag_reference active_camouflage_off;//0xEC
		tag_reference active_camouflage_error;//0xF4
		tag_reference active_camouflage_ready;//0xFC
		tag_reference flashlight_on;//0x104
		tag_reference flashlight_off;//0x10C
		tag_reference ice_cream;//0x114
	};
	TAG_BLOCK_SIZE_ASSERT(s_player_information_block, 0x11C);
	tag_block < s_player_information_block> player_information;//0x130
	struct s_player_representation_block
	{
		tag_reference first_person_hands;//0x0
		tag_reference first_person_body;//0x8
		PAD(0xA0);//0x10
		tag_reference third_person_unit;//0xB0
		string_id third_person_variant;//0xB8
	};
	TAG_BLOCK_SIZE_ASSERT(s_player_representation_block, 0xBC);
	tag_block < s_player_representation_block> player_representation;//0x138
	struct s_falling_damage_block
	{
		PAD(0x8);//0x0
		real_bounds harmful_falling_distance;//0x8

		tag_reference falling_damage;//0x10
		PAD(0x8);//0x18
		float maximum_falling_distance;//0x20
		tag_reference distance_damage;//0x24
		tag_reference vehicle_environemtn_collision_damage_effect;//0x2C
		tag_reference vehicle_killed_unit_damage_effect;//0x34
		tag_reference vehicle_collision_damage;//0x3C
		tag_reference flaming_death_damage;//0x44
		PAD(0x1C);//0x4C
	};
	TAG_BLOCK_SIZE_ASSERT(s_falling_damage_block, 0x68);
	tag_block<s_falling_damage_block> falling_damage;//0x140
	struct s_old_materials_block
	{
		string_id new_material_name;//0x0
		string_id new_general_material_name;//0x4
		float ground_friction_scale;//0x8
		float ground_friction_normal_k1_scale;//0xC
		float ground_friction_normal_k0_scale;//0x10
		float ground_depth_scale;//0x14
		float ground_damp_fraction_scale;//0x18
		tag_reference melee_hit_sound;//0x1C
	};
	TAG_BLOCK_SIZE_ASSERT(s_old_materials_block, 0x24);
	tag_block<s_old_materials_block> old_materials;//0x148
	struct s_materials_block
	{
		string_id name;//0x0
		string_id parent_name;//0x4
		PAD(0x2);//0x8
		enum class e_flags : __int16
		{
			flammable = FLAG(0),
			biomass = FLAG(1),
		};
		e_flags flags;//0xA
		enum class e_old_material_type : __int16
		{
			dirt = 0,
			sand = 1,
			stone = 2,
			snow = 3,
			wood = 4,
			metal_hollow = 5,
			metal_thin = 6,
			metal_thick = 7,
			rubber = 8,
			glass = 9,
			force_field = 10,
			grunt = 11,
			hunter_armor = 12,
			hunter_skin = 13,
			elite = 14,
			jackal = 15,
			jackal_energy_shield = 16,
			engineer_skin = 17,
			engineer_force_field = 18,
			flood_combat_form = 19,
			flood_carrier_form = 20,
			cyborg_armor = 21,
			cyborg_energy_shield = 22,
			human_armor = 23,
			human_skin = 24,
			sentinel = 25,
			monitor = 26,
			plastic = 27,
			water = 28,
			leaves = 29,
			elite_energy_shield = 30,
			ice = 31,
			hunter_shield = 32,
		};
		e_old_material_type old_material_type;//0xC
		PAD(0x2);//0xE
		string_id general_armor;//0x10
		string_id specific_armor;//0x14
		PAD(0x4);//0x18
		float friction;//0x1C
		float restitution;//0x20
		float density;//0x24
		tag_reference old_material_physics;//0x28
		tag_reference breakable_surface;//0x30
		tag_reference sound_sweetener_small;//0x38
		tag_reference sound_sweetener_medium;//0x40
		tag_reference sound_sweetener_large;//0x48
		tag_reference sound_sweetener_rolling;//0x50
		tag_reference sound_sweetener_grinding;//0x58
		tag_reference sound_sweetener_melee;//0x60
		PAD(8);//0x68
		tag_reference effect_sweetener_small;//0x70
		tag_reference effect_sweetener_medium;//0x78
		tag_reference effect_sweetener_large;//0x80
		tag_reference effect_sweetener_rolling;//0x88
		tag_reference effect_sweetener_grinding;//0x90
		tag_reference effect_sweetener_melee;//0x98
		PAD(8);//0xA0
		enum class e_sweetener_inheritance_flags : __int32
		{
			sound_small = FLAG(0),
			sound_medium = FLAG(1),
			sound_large = FLAG(2),
			sound_rolling = FLAG(3),
			sound_grinding = FLAG(4),
			sound_melee = FLAG(5),
			NUM_unk = FLAG(6),
			effect_small = FLAG(7),
			effect_medium = FLAG(8),
			effect_large = FLAG(9),
			effect_rolling = FLAG(10),
			effect_grinding = FLAG(11),
			effect_melee = FLAG(12),
			NUM_unk2 = FLAG(13),
		};
		e_sweetener_inheritance_flags sweetener_inheritance_flags;//0xA8
		tag_reference material_effects;//0xAC
	};
	TAG_BLOCK_SIZE_ASSERT(s_materials_block, 0xB4);
	tag_block<s_materials_block> materials;//0x150
	struct s_multiplayer_ui_block
	{
		tag_reference random_player_names;//0x0
		struct s_obsolete_profile_colors_block
		{
			real_color_rgb color;//0x0
		};
		TAG_BLOCK_SIZE_ASSERT(s_obsolete_profile_colors_block, 0xC);
		tag_block<s_obsolete_profile_colors_block> obsolete_profile_colors;//0x8
		struct s_team_colors_block
		{
			real_color_rgb color;//0x0
		};
		TAG_BLOCK_SIZE_ASSERT(s_team_colors_block, 0xC);
		tag_block<s_team_colors_block> team_colors;//0x10
		tag_reference team_names;//0x18
	};
	TAG_BLOCK_SIZE_ASSERT(s_multiplayer_ui_block, 0x20);
	tag_block<s_multiplayer_ui_block> multiplayer_ui;//0x158
	struct s_profile_colors_block
	{
		real_color_rgb color;//0x0
	};
	TAG_BLOCK_SIZE_ASSERT(s_profile_colors_block, 0xC);
	tag_block<s_profile_colors_block> profile_colors;//0x160
	tag_reference multiplayer_globals;//0x168
	struct s_runtime_level_data_block
	{
		struct s_campaign_levels_block
		{
			__int32 campaign_id;//0x0
			__int32 map_id;//0x4
			tag_string256 path;//0x8
		};
		TAG_BLOCK_SIZE_ASSERT(s_campaign_levels_block, 0x108);
		tag_block<s_campaign_levels_block> campaign_levels;//0x0
	};
	TAG_BLOCK_SIZE_ASSERT(s_runtime_level_data_block, 0x8);
	tag_block<s_runtime_level_data_block> runtime_level_data;//0x170
	struct s_ui_level_data_block
	{
		struct s_campaigns_block
		{
			__int32 campaign_id;//0x0
			tag_unicode32 english_name;//0x4
			tag_unicode32 japanese_name;//0x44
			tag_unicode32 german_name;//0x84
			tag_unicode32 french_name;//0xC4
			tag_unicode32 spanish_name;//0x104
			tag_unicode32 italian_name;//0x144
			tag_unicode32 korean_name;//0x184
			tag_unicode32 chinese_name;//0x1C4
			tag_unicode32 portuguese_name;//0x204
			tag_unicode128 english_description;//0x244
			tag_unicode128 japanese_description;//0x344
			tag_unicode128 german_description;//0x444
			tag_unicode128 french_description;//0x544
			tag_unicode128 spanish_description;//0x644
			tag_unicode128 italian_description;//0x744
			tag_unicode128 korean_description;//0x844
			tag_unicode128 chinese_description;//0x944
			tag_unicode128 portuguese_description;//0xA44
		};
		TAG_BLOCK_SIZE_ASSERT(s_campaigns_block, 0xB44);
		tag_block<s_campaigns_block> campaigns;//0x0
		struct s_campaign_levels_block
		{
			__int32 campaign_id;//0x0
			__int32 map_id;//0x4
			tag_reference bitmap;//0x8
			tag_unicode32 english_name;//0x10
			tag_unicode32 japanese_name;//0x50
			tag_unicode32 german_name;//0x90
			tag_unicode32 french_name;//0xD0
			tag_unicode32 spanish_name;//0x110
			tag_unicode32 italian_name;//0x150
			tag_unicode32 korean_name;//0x190
			tag_unicode32 chinese_name;//0x1D0
			tag_unicode32 portuguese_name;//0x210
			tag_unicode128 english_description;//0x250
			tag_unicode128 japanese_description;//0x350
			tag_unicode128 german_description;//0x450
			tag_unicode128 french_description;//0x550
			tag_unicode128 spanish_description;//0x650
			tag_unicode128 italian_description;//0x750
			tag_unicode128 korean_description;//0x850
			tag_unicode128 chinese_description;//0x950
			tag_unicode128 portuguese_description;//0xA50
		};
		TAG_BLOCK_SIZE_ASSERT(s_campaign_levels_block, 0xB50);
		tag_block<s_campaign_levels_block> campaign_levels;//0x8
		struct s_multiplayer_levels_block
		{
			__int32 map_id;//0x0
			tag_reference bitmap;//0x4
			tag_unicode32 english_name;//0xC
			tag_unicode32 japanese_name;//0x4C
			tag_unicode32 german_name;//0x8C
			tag_unicode32 french_name;//0xCC
			tag_unicode32 spanish_name;//0x10C
			tag_unicode32 italian_name;//0x14C
			tag_unicode32 korean_name;//0x18C
			tag_unicode32 chinese_name;//0x1CC
			tag_unicode32 portuguese_name;//0x20C
			tag_unicode128 english_description;//0x24C
			tag_unicode128 japanese_description;//0x34C
			tag_unicode128 german_description;//0x44C
			tag_unicode128 french_description;//0x54C
			tag_unicode128 spanish_description;//0x64C
			tag_unicode128 italian_description;//0x74C
			tag_unicode128 korean_description;//0x84C
			tag_unicode128 chinese_description;//0x94C
			tag_unicode128 portuguese_description;//0xA4C
			tag_string256 path;//0xB4C
			__int32 sort_order;//0xC4C
			enum class e_flags : __int8
			{
				unlockable = FLAG(0),
			};
			e_flags flags;//0xC50
			PAD(0x3);//0xC51
			__int8 max_teams_none;//0xC54
			__int8 max_teams_ctf;//0xC55
			__int8 max_teams_slayer;//0xC56
			__int8 max_teams_oddball;//0xC57
			__int8 max_teams_koth;//0xC58
			__int8 max_teams_race;//0xC59
			__int8 max_teams_headhunter;//0xC5A
			__int8 max_teams_juggernaut;//0xC5B
			__int8 max_teams_territories;//0xC5C
			__int8 max_teams_assault;//0xC5D
			__int8 max_teams_stub_10;//0xC5E
			__int8 max_teams_stub_11;//0xC5F
			__int8 max_teams_stub_12;//0xC60
			__int8 max_teams_stub_13;//0xC61
			__int8 max_teams_stub_14;//0xC62
			__int8 max_teams_stub_15;//0xC63
		};
		TAG_BLOCK_SIZE_ASSERT(s_multiplayer_levels_block, 0xC64);
		tag_block<s_multiplayer_levels_block> multiplayer_levels;//0x10
	};
	TAG_BLOCK_SIZE_ASSERT(s_ui_level_data_block, 0x18);
	tag_block<s_ui_level_data_block> ui_level_data;//0x178
	tag_reference default_global_lighting;//0x180
	PAD(0x284 - 0x188);//0x188
};
TAG_GROUP_SIZE_ASSERT(s_globals_group_definition, 0x284);

#pragma pack(pop)

//Redundant defined in tags namespace
//s_globals_group_definition *get_matg_globals_ptr()
//{
//	return *Memory::GetAddress<s_globals_group_definition**>(0x479E70, 0x4A642C);
//}
