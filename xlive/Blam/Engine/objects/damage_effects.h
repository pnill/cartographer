#pragma once

#include "effects/player_effects.h"
#include "game/player_vibration.h"
#include "tag_files/string_id.h"
#include "tag_files/tag_reference.h"
#include "tag_files/tag_block.h"

enum e_damage_effect_flags
{
	_damage_effect_dont_scale_damage_by_distance_bit = 0,
	_damage_effect_area_damage_players_only_bit = 1,		// Area of effect damage only affects players
	k_damage_effect_flag_count
};

enum e_side_effect : int16
{
	_side_effect_none = 0,
	_side_effect_harmless = 1,
	_side_effect_lethal_to_the_unsuspecting = 2,
	_side_effect_emp = 3
};

enum e_damage_category : int16
{
	_damage_category_none = 0,
	_damage_category_falling = 1,
	_damage_category_bullet = 2,
	_damage_category_grenade = 3,
	_damage_category_high_explosive = 4,
	_damage_category_sniper = 5,
	_damage_category_melee = 6,
	_damage_category_flame = 7,
	_damage_category_mounted_weapon = 8,
	_damage_category_vehicle = 9,
	_damage_category_plasma = 10,
	_damage_category_needle = 11,
	_damage_category_shotgun = 12
};

enum e_damage_flags : uint32
{
	_damage_does_not_hurt_owner_bit = 0,
	_damage_can_cause_headshots_bit = 1,
	_damage_pings_resistant_units_bit = 2,
	_damage_does_not_hurt_friends_bit = 3,
	_damage_does_not_ping_units_bit = 4,
	_damage_detonates_explosives_bit = 5,
	_damage_only_hurts_shields_bit = 6,
	_damage_causes_flaming_death_bit = 7,
	_damage_damage_indicators_always_point_down_bit = 8,
	_damage_skips_shields_bit = 9,
	_damage_only_hurts_one_infection_form_bit = 10,
	_damage_obsolete_was_can_cause_multiplayer_headshots_bit = 11,
	_damage_infection_form_pop_bit = 12,
	_damage_ignore_seat_scale_for_dir_dmg_bit = 13,
	_damage_forces_hard_ping_bit = 14,
	_damage_does_not_hurt_players_bit = 15,
	k_damage_flag_count
};

enum e_response_type : int16
{
	_response_type_shielded = 0,
	_response_type_unshielded = 1,
	_response_type_all = 2
};

struct s_damage_effect_sound_effect_definition
{
	string_id effect_name;
	real32 duration;	// Seconds
	c_function_definition effect_scale_function;
};

// max count: 2
struct s_damage_effect_player_response_definition
{
	e_response_type response_type;
	int16 pad;

	/*	Explaination("screen flash", "There are seven screen flash types:
		NONE: DST'= DST
		LIGHTEN : DST'= DST(1 - A) + C
		DARKEN : DST'= DST(1 - A) - C
		MAX : DST'= MAX[DST(1 - C), (C - A)(1-DST)]
		MIN : DST'= MIN[DST(1 - C), (C + A)(1-DST)]
		TINT : DST'= DST(1 - C) + (A*PIN[2C - 1, 0, 1] + A)(1-DST)
		INVERT : DST'= DST(1 - C) + A)

		In the above equations C and A represent the color and alpha of the screen flash,
		DST represents the color in the framebuffer before the screen flash is applied,
		and DST' represents the color after the screen flash is applied.")
	*/

	screen_flash_definition screen_flash;

	// Explaination("vibration", "EMPTY STRING")
	s_vibration_definition vibration;

	// Explaination("sound effect", "EMPTY STRING")
	s_damage_effect_sound_effect_definition sound_effect;
};
ASSERT_STRUCT_SIZE(s_damage_effect_player_response_definition, 76);

struct s_damage_effect_definition
{
	real_vector2d radius;
	real32 cutoff_scale;
	c_flags<e_damage_effect_flags, uint32, k_damage_effect_flag_count> flags;

	// Explaination("damage", "EMPTY STRING")

	e_side_effect side_effect;
	e_damage_category category;
	c_flags<e_damage_flags, uint32, k_damage_flag_count> damage_flags;

	real32 aoe_core_radius;				// (World Units) if this is area of effect damage
	real32 damage_lower_bound;
	real_vector2d damage_upper_bound;
	real_angle dmg_inner_cone;
	real_angle dmg_outer_cone;

	real32 active_camouflage_damage;	// how much more visible this damage makes a player who is active camouflaged
	real32 stun;						// amount of stun added to damaged unit
	real32 maximum_stun;				// damaged unit's stun will never exceed this amount
	real32 stun_time_seconds;			// duration of stun due to this damage

	real32 instantaneous_acceleration;	// 0 - inf
	real32 rider_direct_damage_scale;
	real32 rider_maximum_transfer_damage_scale;
	real32 rider_minimum_transfer_damage_scale;

	string_id general_damage;
	string_id specific_damage;
	real32 ai_stun_radius;				// World Units
	real_vector2d ai_stun_bounds;
	real32 shake_radius;
	real32 emp_radius;
	tag_block<s_damage_effect_player_response_definition> player_responses;

	// Explaination("temporary camera impulse", "EMPTY STRING")
	s_temporary_camera_impulse camera_impulse;

	/*	Explaination("camera shaking", "The wobble function and weight affects how the camera shake oscilates over time.
		If the weight is one, then the wobble function completely scales the translational
		and rotational magnitudes.The less the weight, the less the effect wobble has.
		If the wobble weight is 0 then wobble is completely ignored.")
	*/
	s_player_effect_camera_shaking camera_shaking;

	tag_reference sound;

	/*	Explaination("breaking effect", "Controls particle velocities when a damage effect shatters a materal.

		All particles created within 'forward radius' will be kicked along the
		damage direction with a speed equivalent to 'forward velocity' at the
		epicenter of the damage and 0 at the outer radius.  'Forward exponent'
		is used to modify the velocity scale.A low exponent(like 0.5) means that
		particles between the epicenter and the radius will be kicked out with a speed
		closer to 'forward velocity' than if a higher exponent(like 2.0) was used

		The outward fields work in a similar way, except instead of kicking along the
		damage direction, they get kick away from the damage epicenter.")
	*/

	real32 forward_velocity;	// World Units Per Second
	real32 forward_radius;		// World Units
	real32 forward_exponent;
	real32 outward_velocity;	// World Units Per Second
	real32 outward_radius;		// World Units
	real32 outward_exponent;
};
ASSERT_STRUCT_SIZE(s_damage_effect_definition, 200);
