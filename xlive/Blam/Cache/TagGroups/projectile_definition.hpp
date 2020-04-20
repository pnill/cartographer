#pragma once
#ifndef PROJ_H
#define PROJ_H
#pragma pack(push,1)
#include "object_definition.hpp"
/*********************************************************************
* name: projectile
* group_tag : proj
* header size : 420
* *********************************************************************/
struct s_projectile_group_definition :TagGroup<'proj'>
{
	s_object_group_definition objectTag;
	enum class e_flags : __int32
	{
		oriented_along_velocity = FLAG(0),
		ai_must_use_ballistic_aiming = FLAG(1),
		detonation_max_time_if_attached = FLAG(2),
		has_super_combining_explosion = FLAG(3),
		damage_scales_based_on_distance = FLAG(4),
		travels_instantaneously = FLAG(5),
		steering_adjusts_orientation = FLAG(6),
		dont_noise_up_steering = FLAG(7),
		can_track_behind_itself = FLAG(8),
		robotron_steering = FLAG(9),
		faster_when_owned_by_player = FLAG(10),
	};
	e_flags flags;//0xBC
	enum class e_detonation_timer_starts : __int16
	{
		immediately = 0,
		after_first_bounce = 1,
		when_at_rest = 2,
		after_first_bounce_off_any_surface = 3,
	};
	e_detonation_timer_starts detonation_timer_starts;//0xC0
	enum class e_impact_noise : __int16
	{
		silent = 0,
		medium = 1,
		loud = 2,
		shout = 3,
		quiet = 4,
	};
	e_impact_noise impact_noise;//0xC2
	float ai_perception_radius;//0xC4
	float collision_radius;//0xC8
	float arming_time;//0xCC
	float danger_radius;//0xD0
	real_bounds timer;//0xD4

	float minimum_velocity;//0xDC
	float maximum_range;//0xE0
	enum class e_detonation_noise : __int16
	{
		silent = 0,
		medium = 1,
		loud = 2,
		shout = 3,
		quiet = 4,
	};
	e_detonation_noise detonation_noise;//0xE4
	__int16 super_det_projectile_count;//0xE6
	tag_reference detonation_started;//0xE8
	tag_reference detonation_effect_airborne;//0xF0
	tag_reference detonation_effect_ground;//0xF8
	tag_reference detonation_damage;//0x100
	tag_reference attached_detonation_damage;//0x108
	tag_reference super_detonation;//0x110
	tag_reference super_detonation_damage;//0x118
	tag_reference detonation_sound;//0x120
	enum class e_damage_reporting_type : __int8
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
	e_damage_reporting_type damage_reporting_type;//0x128
	PAD(0x3);//0x129
	tag_reference super_attached_detonation_damage;//0x12C
	float material_effect_radius;//0x134
	tag_reference flyby_sound;//0x138
	tag_reference impact_effect;//0x140
	tag_reference impact_damage;//0x148
	float boarding_detonation_time;//0x150
	tag_reference boarding_detonation_damage;//0x154
	tag_reference boarding_attached_detonation_damage;//0x15C
	float air_gravity_scale;//0x164
	real_bounds air_damage_range;//0x168

	float water_gravity_scale;//0x170
	real_bounds water_damage_range;//0x174	
	float initial_velocity;//0x17C
	float final_velocity;//0x180
	angle_bounds guided_angular_velocity_lower;//0x184
	real_bounds acceleration_range;//0x18C

	PAD(0x4);//0x194
	float targeted_leading_fraction;//0x198
	struct s_material_responses_block
	{
		enum class e_flags : __int16
		{
			cannot_be_overpenetrated = FLAG(0),
		};
		e_flags flags;//0x0
		enum class e_response : __int16
		{
			impact_detonate = 0,
			fizzle = 1,
			overpenetrate = 2,
			attach = 3,
			bounce = 4,
			bounce_dud = 5,
			fizzle_ricochet = 6,
		};
		e_response response;//0x2
		tag_reference do_not_use_old_effect;//0x4
		string_id material_name;//0xC
		PAD(0x4);//0x10
	
		e_response potential_result_response;//0x14
		enum class e_potential_result_flags : __int16
		{
			only_against_units = FLAG(0),
			never_against_units = FLAG(1),
		};
		e_potential_result_flags potential_result_flags;//0x16
		float chance_fraction;//0x18
		angle_bounds between;//0x1C		
		real_bounds and;//0x24

		tag_reference do_not_use_old_effect2;//0x2C
		enum class e_scale_effects_by : __int16
		{
			damage = 0,
			angle = 1,
		};
		e_scale_effects_by scale_effects_by;//0x34
		PAD(0x2);//0x36
		angle angular_noise;//0x38
		float velocity_noise;//0x3C
		tag_reference do_not_use_old_detonation_effect;//0x40
		float initial_friction;//0x48
		float maximum_distance;//0x4C
		float parallel_friction;//0x50
		float perpendicular_friction;//0x54
	};
	TAG_BLOCK_SIZE_ASSERT(s_material_responses_block, 0x58);
	tag_block<s_material_responses_block> material_responses;//0x19C
};
TAG_GROUP_SIZE_ASSERT(s_projectile_group_definition, 0x1A4);

#pragma pack(pop)
#endif
