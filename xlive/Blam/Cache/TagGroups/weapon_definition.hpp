#pragma once
#ifndef WEAP_H
#define WEAP_H
#pragma pack(push,1)
#include "item_definition.hpp"
/*********************************************************************
* name: weapon
* group_tag : weap
* header size : 796
* *********************************************************************/
struct s_weapon_group_definition :TagGroup<'weap'>
{
	s_item_group_definition itemTag;
	enum class e_flags : __int32
	{
		vertical_heat_display = FLAG(0),
		mutually_exclusive_triggers = FLAG(1),
		attacks_automatically_on_bump = FLAG(2),
		must_be_readied = FLAG(3),
		doesnt_count_toward_maximum = FLAG(4),
		aim_assists_only_when_zoomed = FLAG(5),
		prevents_grenade_throwing = FLAG(6),
		must_be_picked_up = FLAG(7),
		holds_triggers_when_dropped = FLAG(8),
		prevents_melee_attack = FLAG(9),
		detonates_when_dropped = FLAG(10),
		cannot_fire_at_maximum_age = FLAG(11),
		secondary_trigger_overrides_grenades = FLAG(12),
		obsoletedoes_not_depower_active_camo_in_multilplayer = FLAG(13),
		enables_integrated_night_vision = FLAG(14),
		ais_use_weapon_melee_damage = FLAG(15),
		forces_no_binoculars = FLAG(16),
		loop_fp_firing_animation = FLAG(17),
		prevents_sprinting = FLAG(18),
		cannot_fire_while_boosting = FLAG(19),
		prevents_driving = FLAG(20),
		prevents_gunning = FLAG(21),
		can_be_dual_wielded = FLAG(22),
		can_only_be_dual_wielded = FLAG(23),
		melee_only = FLAG(24),
		cant_fire_if_parent_dead = FLAG(25),
		weapon_ages_with_each_kill = FLAG(26),
		weapon_uses_old_dual_fire_error_code = FLAG(27),
		primary_trigger_melee_attacks = FLAG(28),
		cannot_be_used_by_player = FLAG(29),
	};
	e_flags flags;//0x12C
	string_id old_string_id;//0x130
	enum class e_secondary_trigger_mode : __int16
	{
		normal = 0,
		slaved_to_primary = 1,
		inhibits_primary = 2,
		loads_alterate_ammunition = 3,
		loads_multiple_primary_ammunition = 4,
	};
	e_secondary_trigger_mode secondary_trigger_mode;//0x134
	__int16 maximum_alternate_shots_loaded;//0x136
	float turn_on_time;//0x138
	float ready_time;//0x13C
	tag_reference ready_effect;//0x140
	tag_reference ready_damage_effect;//0x148
	float heat_recovery_threshold;//0x150
	float overheated_threshold;//0x154
	float heat_detonation_threshold;//0x158
	float heat_detonation_fraction;//0x15C
	float heat_loss_per_second;//0x160
	float heat_illumination;//0x164
	float overheated_heat_loss_per_second;//0x168
	tag_reference overheated;//0x16C
	tag_reference overheated_damage_effect;//0x174
	tag_reference detonation;//0x17C
	tag_reference detonation_damage_effect;//0x184
	tag_reference player_melee_damage;//0x18C
	tag_reference player_melee_response;//0x194
	angle melee_magnetism_angle;//0x19C  : degrees
	float melee_magnetism_range;//0x1A0
	float throttle_magnitude;//0x1A4
	float throttle_minimum_distance;//0x1A8
	angle throttle_maximum_adjustment_angle;//0x1AC  : degrees
	angle damage_pyramid_angles_yaw;//0x1B0
	angle damage_pyramid_angles_pitch;//0x1B4
	float damage_pyramid_depth;//0x1B8
	tag_reference _1st_hit_melee_damage;//0x1BC
	tag_reference _1st_hit_melee_response;//0x1C4
	tag_reference _2nd_hit_melee_damage;//0x1CC
	tag_reference _2nd_hit_melee_response;//0x1D4
	tag_reference _3rd_hit_melee_damage;//0x1DC
	tag_reference _3rd_hit_melee_response;//0x1E4
	tag_reference lunge_melee_damage;//0x1EC
	tag_reference lunge_melee_response;//0x1F4
	enum class e_melee_damage_reporting_type : __int8
	{
		teh_guardians11 = 0,
		falling_damage = 1,
		generic_collision_damage = 2,
		generic_melee_damage = 3,
		generic_explosion = 4,
		magnum_pistol = 5,
		plasma_pistol = 6,
		needler = 7,
		smg = 8,
		plasma_rifle = 9,
		battle_rifle = 10,
		carbine = 11,
		shotgun = 12,
		sniper_rifle = 13,
		beam_rifle = 14,
		rocket_launcher = 15,
		flak_cannon = 16,
		brute_shot = 17,
		disintegrator = 18,
		brute_plasma_rifle = 19,
		energy_sword = 20,
		frag_grenade = 21,
		plasma_grenade = 22,
		flag_melee_damage = 23,
		bomb_melee_damage = 24,
		bomb_explosion_damage = 25,
		ball_melee_damage = 26,
		human_turret = 27,
		plasma_turret = 28,
		banshee = 29,
		ghost = 30,
		mongoose = 31,
		scorpion = 32,
		spectre_driver = 33,
		spectre_gunner = 34,
		warthog_driver = 35,
		warthog_gunner = 36,
		wraith = 37,
		tank = 38,
		sentinel_beam = 39,
		sentinel_rpg = 40,
		teleporter = 41,
	};
	e_melee_damage_reporting_type melee_damage_reporting_type;//0x1FC
	PAD(0x1);//0x1FD
	__int16 magnification_levels;//0x1FE
	float magnification_range_lower;//0x200
	float magnification_range_upper;//0x204
	angle autoaim_angle;//0x208  : degrees
	float autoaim_range;//0x20C
	angle magnetism_angle;//0x210  : degrees
	float magnetism_range;//0x214
	angle deviation_angle;//0x218  : degrees
	PAD(0x10);//0x21C
	enum class e_movement_penalized : __int16
	{
		always = 0,
		when_zoomed = 1,
		when_zoomed_or_reloading = 2,
	};
	e_movement_penalized movement_penalized;//0x22C
	PAD(0x2);//0x22E
	float forward_movement_penalty;//0x230
	float sideways_movement_penalty;//0x234
	float ai_scariness;//0x238
	float weapon_poweron_time;//0x23C
	float weapon_poweroff_time;//0x240
	tag_reference weapon_poweron_effect;//0x244
	tag_reference weapon_poweroff_effect;//0x24C
	float age_heat_recovery_penalty;//0x254
	float age_rate_of_fire_penalty;//0x258
	float age_misfire_start;//0x25C
	float age_misfire_chance;//0x260
	tag_reference pickup_sound;//0x264
	tag_reference zoomin_sound;//0x26C
	tag_reference zoomout_sound;//0x274
	float active_camo_ding;//0x27C
	float active_camo_regrowth_rate;//0x280
	string_id handle_node;//0x284
	string_id weapon_class;//0x288
	string_id weapon_name;//0x28C
	enum class e_multiplayer_weapon_type : __int16
	{
		none = 0,
		ctf_flag = 1,
		oddball_ball = 2,
		headhunter_head = 3,
		juggernaut_powerup = 4,
	};
	e_multiplayer_weapon_type multiplayer_weapon_type;//0x290
	enum class e_weapon_type : __int16
	{
		undefined = 0,
		shotgun = 1,
		needler = 2,
		plasma_pistol = 3,
		plasma_rifle = 4,
		rocket_launcher = 5,
	};
	e_weapon_type weapon_type;//0x292
	enum class e_tracking_type : __int16
	{
		no_tracking = 0,
		human_tracking = 1,
		plasma_tracking = 2,
	};
	e_tracking_type tracking_type;//0x294
	PAD(0x12);//0x296
	struct s_first_person_block
	{
		tag_reference first_person_model;//0x0
		tag_reference first_person_animations;//0x8
	};
	TAG_BLOCK_SIZE_ASSERT(s_first_person_block, 0x10);
	tag_block<s_first_person_block> first_person;//0x2A8
	tag_reference new_hud_interface;//0x2B0
	struct s_predicted_resources_block
	{
		enum class e_type : __int16
		{
			bitmap = 0,
			sound = 1,
			render_model_geometry = 2,
			cluster_geometry = 3,
			cluster_instanced_geometry = 4,
			lightmap_geometry_object_buckets = 5,
			lightmap_geometry_instance_buckets = 6,
			lightmap_cluster_bitmaps = 7,
			lightmap_instance_bitmaps = 8,
		};
		e_type type;//0x0
		__int16 resource_index;//0x2
		datum tag_index;//0x4
	};
	TAG_BLOCK_SIZE_ASSERT(s_predicted_resources_block, 0x8);
	tag_block<s_predicted_resources_block> predicted_resources;//0x2B8
	struct s_magazines_block
	{
		enum class e_flags : __int32
		{
			wastes_rounds_when_reloaded = FLAG(0),
			every_round_must_be_chambered = FLAG(1),
		};
		e_flags flags;//0x0
		__int16 rounds_recharged;//0x4
		__int16 rounds_total_initial;//0x6
		__int16 rounds_total_maximum;//0x8
		__int16 rounds_loaded_maximum;//0xA
		__int16 maximum_rounds_held;//0xC
		PAD(0x2);//0xE
		float reload_time;//0x10
		__int16 rounds_reloaded;//0x14
		PAD(0x2);//0x16
		float chamber_time;//0x18
		PAD(0x18);//0x1C
		tag_reference reloading_effect;//0x34
		tag_reference reloading_damage_effect;//0x3C
		tag_reference chambering_effect;//0x44
		tag_reference chambering_damage_effect;//0x4C
		struct s_magazines_child_block
		{
			__int16 rounds;//0x0
			PAD(0x2);//0x2
			tag_reference equipment;//0x4
		};
		TAG_BLOCK_SIZE_ASSERT(s_magazines_child_block, 0xC);
		tag_block<s_magazines_child_block> magazines;//0x54
	};
	TAG_BLOCK_SIZE_ASSERT(s_magazines_block, 0x5C);
	tag_block<s_magazines_block> magazines;//0x2C0
	struct s_new_triggers_block
	{
		enum class e_flags : __int32
		{
			autofire_single_action_only = FLAG(0),
		};
		e_flags flags;//0x0
		enum class e_input : __int16
		{
			right_trigger = 0,
			left_trigger = 1,
			melee_attack = 2,
		};
		e_input input;//0x4
		enum class e_behavior : __int16
		{
			spew = 0,
			latch = 1,
			latchautofire = 2,
			charge = 3,
			latchzoom = 4,
			latchrocketlauncher = 5,
		};
		e_behavior behavior;//0x6
		__int16 primary_barrel;//0x8
		__int16 secondary_barrel;//0xA
		enum class e_prediction : __int16
		{
			none = 0,
			spew = 1,
			charge = 2,
		};
		e_prediction prediction;//0xC
		PAD(0x2);//0xE
		float autofire_time;//0x10
		float autofire_throw;//0x14
		enum class e_secondary_action : __int16
		{
			fire = 0,
			charge = 1,
			track = 2,
			fire_other = 3,
		};
		e_secondary_action secondary_action;//0x18
		enum class e_primary_action : __int16
		{
			fire = 0,
			charge = 1,
			track = 2,
			fire_other = 3,
		};
		e_primary_action primary_action;//0x1A
		float charging_time;//0x1C
		float charged_time;//0x20
		enum class e_overcharged_action : __int16
		{
			none = 0,
			explode = 1,
			discharge = 2,
		};
		e_overcharged_action overcharged_action;//0x24
		PAD(0x2);//0x26
		float charged_illumination;//0x28
		float spew_time;//0x2C
		tag_reference charging_effect;//0x30
		tag_reference charging_damage_effect;//0x38
	};
	TAG_BLOCK_SIZE_ASSERT(s_new_triggers_block, 0x40);
	tag_block<s_new_triggers_block> new_triggers;//0x2C8
	struct s_barrels_block
	{
		enum class e_flags : __int32
		{
			tracks_fired_projectile = FLAG(0),
			random_firing_effects = FLAG(1),
			can_fire_with_partial_ammo = FLAG(2),
			projectiles_use_weapon_origin = FLAG(3),
			ejects_during_chamber = FLAG(4),
			use_error_when_unzoomed = FLAG(5),
			projectile_vector_cannot_be_adjusted = FLAG(6),
			projectiles_have_identical_error = FLAG(7),
			projectiles_fire_parallel = FLAG(8),
			cant_fire_when_others_firing = FLAG(9),
			cant_fire_when_others_recovering = FLAG(10),
			dont_clear_fire_bit_after_recovering = FLAG(11),
			stagger_fire_across_multiple_markers = FLAG(12),
			fires_locked_projectiles = FLAG(13),
		};
		e_flags flags;//0x0
		real_bounds rounds_per_second;//0x4		
		float firing_acceleration_time;//0xC
		float firing_deceleration_time;//0x10
		float barrel_spin_scale;//0x14
		float blurred_rate_of_fire;//0x18
		__int16 shots_per_fire_lower;//0x1C
		__int16 shots_per_fire_upper;//0x1E
		float fire_recovery_time;//0x20
		float soft_recovery_fraction;//0x24
		__int16 magazine;//0x28
		__int16 rounds_per_shot;//0x2A
		__int16 minimum_rounds_loaded;//0x2C
		__int16 rounds_between_tracers;//0x2E
		string_id optional_barrel_marker_name;//0x30
		enum class e_prediction_type : __int16
		{
			none = 0,
			continuous = 1,
			instant = 2,
		};
		e_prediction_type prediction_type;//0x34
		enum class e_firing_noise : __int16
		{
			silent = 0,
			medium = 1,
			loud = 2,
			shout = 3,
			quiet = 4,
		};
		e_firing_noise firing_noise;//0x36
		float error_acceleration_time;//0x38
		float error_deceleration_time;//0x3C
		real_bounds damage_error;//0x40

		float dual_weild_error_acceleration_time;//0x48
		float dual_weild_error_deceleration_time;//0x4C
		PAD(0x8);//0x50
		angle minimum_error;//0x58  : degrees
		angle_bounds error_angle;//0x5C  : degrees		
		float dual_wield_damage_scale;//0x64


		enum class e_distribution_function : __int16
		{
			point = 0,
			horizontal_fan = 1,
		};
		e_distribution_function distribution_function;//0x68
		__int16 projectiles_per_shot;//0x6A
		float proj_distribution_angle;//0x6C
		angle proj_minimum_errors;//0x70
		angle_bounds proj_error_angle;//0x74		
		real_point3d first_person_offset;//0x7C

		enum class e_damage_effect_reporting_type : __int8
		{
			teh_guardians11 = 0,
			falling_damage = 1,
			generic_collision_damage = 2,
			generic_melee_damage = 3,
			generic_explosion = 4,
			magnum_pistol = 5,
			plasma_pistol = 6,
			needler = 7,
			smg = 8,
			plasma_rifle = 9,
			battle_rifle = 10,
			carbine = 11,
			shotgun = 12,
			sniper_rifle = 13,
			beam_rifle = 14,
			rocket_launcher = 15,
			flak_cannon = 16,
			brute_shot = 17,
			disintegrator = 18,
			brute_plasma_rifle = 19,
			energy_sword = 20,
			frag_grenade = 21,
			plasma_grenade = 22,
			flag_melee_damage = 23,
			bomb_melee_damage = 24,
			bomb_explosion_damage = 25,
			ball_melee_damage = 26,
			human_turret = 27,
			plasma_turret = 28,
			banshee = 29,
			ghost = 30,
			mongoose = 31,
			scorpion = 32,
			spectre_driver = 33,
			spectre_gunner = 34,
			warthog_driver = 35,
			warthog_gunner = 36,
			wraith = 37,
			tank = 38,
			sentinel_beam = 39,
			sentinel_rpg = 40,
			teleporter = 41,
		};
		e_damage_effect_reporting_type damage_effect_reporting_type;//0x88
		PAD(0x3);//0x89
		tag_reference projectile;//0x8C
		tag_reference damage_effect;//0x94


		float ejection_port_recovery_time;//0x9C
		float illumination_recovery_time;//0xA0
		float heat_generated_per_round;//0xA4
		float age_generated_per_round;//0xA8
		float overload_time;//0xAC


		angle_bounds angle_change_per_shot;//0xB0		
		float recoil_acceleration_time;//0xB8
		float recoil_deceleration_time;//0xBC
		enum class e_angle_change_function : __int16
		{
			linear = 0,
			early = 1,
			very_early = 2,
			late = 3,
			very_late = 4,
			cosine = 5,
			one = 6,
			zero = 7,
		};
		e_angle_change_function angle_change_function;//0xC0
		PAD(0x22);//0xC2
		struct s_firing_effects
		{
			__int16 shot_count_lower_bound;//0x0
			__int16 shot_count_upper_bound;//0x2
			tag_reference firing_effect;//0x4
			tag_reference misfire_effect;//0xC
			tag_reference empty_effect;//0x14
			tag_reference firing_damage;//0x1C
			tag_reference misfire_damage;//0x24
			tag_reference empty_damage;//0x2C
		};
		TAG_BLOCK_SIZE_ASSERT(s_firing_effects, 0x34);
		tag_block<s_firing_effects> firing_effects;//0xE4
	};
	TAG_BLOCK_SIZE_ASSERT(s_barrels_block, 0xEC);
	tag_block<s_barrels_block> barrels;//0x2D0	

	PAD(0x8);//0x2D8

	//first-person movement control

	float max_movement_acceleration;//0x2E0
	float max_movement_velocity;//0x2E4
	float max_turning_acceleration;//0x2E8
	float max_turning_velocity;//0x2EC
	tag_reference deployed_vehicle;//0x2F0
	tag_reference age_effect;//0x2F8
	tag_reference aged_weapon;//0x300
	PAD(0xC);//0x308
	float first_person_scope_size_i;//0x314
	float first_person_scope_size_j;//0x318
};
TAG_GROUP_SIZE_ASSERT(s_weapon_group_definition, 0x31C);

#pragma pack(pop)
#endif
