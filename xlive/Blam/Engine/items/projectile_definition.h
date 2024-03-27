#pragma once
#include "objects/damage_reporting.h"
#include "objects/object_definition.h"

#define k_maximum_material_responses 200

enum e_projectile_material_response_flags : uint16
{
	_projectile_material_response_flag_cannot_be_overpenetrated = FLAG(0),
};

enum e_projectile_material_response : uint16
{
	_projectile_material_response_impact_detonate = 0,
	_projectile_material_response_fizzle = 1,
	_projectile_material_response_over_penetrate = 2,
	_projectile_material_response_attach = 3,
	_projectile_material_response_bounce = 4,
	_projectile_material_response_bounce_dud = 5,
	_projectile_material_response_fizzle_ricochet = 6
};

enum e_projectile_material_response_unit : uint16
{
	_projectile_material_response_only_against_units = 1,
	_projectile_material_response_never_against_units = 2,
};

enum e_projectile_material_response_scale_effects : uint16
{
	_projectile_material_response_scale_effects_by_damage = 0,
	_projectile_material_response_scale_effects_by_angle = 1,
};

enum e_projectile_definition_flags : uint32
{
	_projectile_flag_oriented_along_velocity = FLAG(0),
	_projectile_ai_must_use_ballistic_aiming = FLAG(1),
	_projectile_detonation_max_time_if_attached = FLAG(2),
	_projectile_has_super_combining_explosion = FLAG(3),
	_projectile_damage_scales_based_on_distance = FLAG(4),
	_projectile_travels_instantaneously = FLAG(5),
	_projectile_steering_adjusts_orientation = FLAG(6),
	_projectile_dont_noise_up_steering = FLAG(7),
	_projectile_can_track_behind_itself = FLAG(8),
	_projectile_ROBOTRON_STEERING = FLAG(9),
	_projectile_faster_when_owned_by_player = FLAG(10)
};

enum e_projectile_detonation_timer_starts : uint16
{
	_projectile_detonation_timer_starts_immediately = 0,
	_projectile_detonation_timer_starts_after_first_bounce = 1,
	_projectile_detonation_timer_starts_when_at_rest = 2,
	_projectile_detonation_timer_starts_after_first_bounce_off_any_surface = 3,
};

enum e_projectile_noise : uint16
{
	_projectile_noise_silent = 0,
	_projectile_noise_medium = 1,
	_projectile_noise_loud = 2,
	_projectile_noise_shout = 3,
	_projectile_noise_quiet = 4
};


struct s_projectile_material_response_definition
{
	e_projectile_material_response_flags flags;
	// Explaination("default result", "(if the potential result, below, fails to happen)")
	e_projectile_material_response response;

	// DO NOT USE
	tag_reference old_effect; // effe

	string_id material_name;

	PAD(4);

	e_projectile_material_response potential_response;
	e_projectile_material_response_unit unit_response;
	real32 chance_fraction;
	real_bounds between;
	real_bounds _and;
	// DO NOT USE
	tag_reference old_effect_2;
	e_projectile_material_response_scale_effects scale_effects_by;
	PAD(2);
	/// the angle of incidence is randomly perturbed by at most this amount to simulate irregularity.
	real_angle angular_noise;
	/// the velocity is randomly perturbed by at most this amount to simulate irregularity.
	real32 velocity_noise;
	// DO NOT USE
	tag_reference old_detenation_effect; // effe

	// Explaination("penetration", "")
	/// the fraction of the projectile's velocity lost on penetration
	real32 initial_fraction;
	/// the maximum distance the projectile can travel through on object of this material
	real32 maximum_distance;

	// Explaination("reflection", "")
	/// the fraction of the projectile's velocity parallel to the surface lost on impact
	real32 parallel_friction;
	/// the fraction of the projectile's velocity perpendicular to the surface lost on impact
	real32 perpendicular_friction;
};
TAG_GROUP_SIZE_ASSERT(s_projectile_material_response_definition, 0x58);

struct _projectile_definition
{
	object_definition object;
	// Explaination("$$$ PROJECTILE $$$", "")

	e_projectile_definition_flags flags;

	e_projectile_detonation_timer_starts detonation_timer_starts;

	e_projectile_noise impact_noise;

	real32 ai_perception_radius_world_units;
	real32 collision_radius_world_units;

	// Explaination("detonation", "")
	// won't detonate before this time elapses
	real32 arming_time_seconds;
	real32 danger_radius_world_units;

	// detonation countdown (zero is untimed)
	real_bounds timer;

	// detonates when slowed below this velocity
	real32 minimum_velocity_world_units_per_second;

	// detonates after traveling this distance
	real32 maximum_range_world_units;

	e_projectile_noise detonation_noise;

	uint16 super_detonation_projectile_count;

	tag_reference detonation_started; // effe

	tag_reference detonation_effect_airborne; // effe

	tag_reference detonation_effect_ground; // effe

	tag_reference detonation_damage; // jpt!

	tag_reference attached_detonation_damage; // jpt!

	tag_reference super_detonation; // effe

	tag_reference super_detonation_damage; // jpt!

	tag_reference detonation_sound; // snd!

	e_damage_reporting_type damage_reporting_type;

	PAD(3);

	tag_reference super_attached_detonation_damage; // jpt!

	// radius within we will generate material effects
	real32 material_effects_radius;

	tag_reference flyby_sound; // snd!

	tag_reference impact_effect; // effe

	tag_reference impact_damage; // jpt!

	// Explaination("boarding fields", "")
	real32 boarding_detonation_time;

	tag_reference boarding_detonation_damage; // jpt!

	tag_reference boarding_attached_detonation_damage; // jpt!

	// Explaination("physics", "")
	// the proportion of normal gravity applied to the projectile when in air.
	real32 air_gravity_scale;
	// the range over which damage is scaled when the projectile is in air.
	real_bounds air_damage_range;
	// the proportion of normal gravity applied to the projectile when in water.
	real32 water_gravity_scale;
	// the range over which damage is scaled when the projectile is in water.
	real_bounds water_damage_range;
	// bullet's velocity when inflicting maximum damage
	real32 initial_velocity;
	// bullet's velocity when inflicting minimum damage
	real32 final_velocity;

	real_bounds guided_angular_velocity;

	real_bounds acceleration_range;

	PAD(4);

	real32 targeted_leading_fraction;

	tag_block< s_projectile_material_response_definition> material_responses;
};
TAG_GROUP_SIZE_ASSERT(_projectile_definition, 0x1A4);