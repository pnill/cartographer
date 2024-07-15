#pragma once
#include "memory/static_arrays.h"
#include "tag_files/string_id.h"
#include "tag_files/tag_block.h"
#include "tag_files/tag_reference.h"

#define k_maximum_variants_per_model 64
#define k_style_attitude_count 3

enum e_character_vehicle_flags : uint32
{
	_character_vehicle_flag_passengers_adopt_original_squad = 0,
	k_character_vehicle_flags_count
};

enum e_character_grenade_trajectory_type : uint16
{
	_character_grenade_trajectory_type_toss = 0,
	_character_grenade_trajectory_type_lob = 1,
	_character_grenade_trajectory_type_bounce = 2
};

enum e_character_grenade_type : uint16
{
	_character_grenade_type_human_fragmentation = 0,
	_character_grenade_type_covenant_plasma = 1
};

enum e_character_grenade_flags : uint32
{
	_character_grenade_flag_1 = 0,
	k_character_grenade_flags_count
};

enum e_character_weapon_special_fire_situation : uint16
{
	_character_weapon_special_fire_situation_never = 0,
	_character_weapon_special_fire_situation_enemy_visible = 1,
	_character_weapon_special_fire_situation_enemy_out_of_sight = 2,
	_character_weapon_special_fire_situation_strafing = 3
};

enum e_character_weapon_special_fire_mode : uint16
{
	_character_weapon_special_fire_mode_none = 0,
	_character_weapon_special_fire_mode_over_charge = 1,
	_character_weapon_special_fire_mode_secondary_trigger = 2
};

enum e_character_weapon_flags : uint32
{
	_character_weapon_flag_bursting_inhibits_movement = 0,
	_character_weapon_flag_must_crouch_to_shoot = 1,
	_character_weapon_flag_use_extended_safe_to_save_range = 2,
	k_character_weapon_flag_count
};

enum e_character_boarding_flags : uint32
{
	_character_boarding_flag_airborne_boarding = 0,
	k_character_boarding_flag_count
};

enum e_character_presearch_flags : uint32
{
	_character_presearch_flag_1 = 0,
	k_character_presearch_flag_count
};

enum e_character_search_flags : uint32
{
	_character_search_flag_crouch_on_investigate = 0,
	_character_search_flag_walk_on_pursuit = 1,
	k_character_search_flag_count
};

enum e_character_retreat_flags : uint32
{
	_character_retreat_flag_zig_zag_when_fleeing = 0,
	_character_retreat_flag_unused = 1,
	k_character_retreat_flag_count
};

enum e_character_cover_flags : uint32
{
	_character_cover_flags_1 = 0,
	k_character_cover_flag_count
};

enum e_character_charge_flags : uint32
{
	_character_charge_flag_offhand_melee_allowed = 0,
	_character_charge_flag_berserk_whenever_charge = 1,
	k_character_char_flag_count
};

enum e_character_engage_flags : uint32
{
	_character_engage_flag_engage_perch = 0,
	_character_engage_flag_flight_constant_move = 1,
	_character_engage_flag_flight_fight_constant_movement = 2,
	k_character_engage_flag_count
};

enum e_character_movement_hint_flags : uint32
{
	_character_movement_hint_flag_vault_step = 0,
	_character_movement_hint_flag_vault_crouch = 1,
	_character_movement_hint_flag_mount_step = 5,
	_character_movement_hint_flag_mount_crouch = 6,
	_character_movement_hint_flag_mount_stand = 7,
	_character_movement_hint_flag_hoist_crouch = 11,
	_character_movement_hint_flag_hoist_stand = 12,
	k_character_movement_hint_flag_count = 8
};

enum e_character_movement_jump_height : uint16
{
	_character_movement_jump_height_none = 0,
	_character_movement_jump_height_down = 1,
	_character_movement_jump_height_step = 2,
	_character_movement_jump_height_crouch = 3,
	_character_movement_jump_height_stand = 4,
	_character_movement_jump_height_storey = 5,
	_character_movement_jump_height_tower = 6,
	_character_movement_jump_height_infinite = 7
};

enum e_character_obstacle_size : uint16
{
	_character_movement_obstacle_none = 0,
	_character_movement_obstacle_tiny = 1,
	_character_movement_obstacle_small = 2,
	_character_movement_obstacle_medium = 3,
	_character_movement_obstacle_large = 4,
	_character_movement_obstacle_huge = 5,
	_character_movement_obstacle_immobile = 6
};

enum e_character_movement_flags : uint32
{
	_character_movement_flag_danger_crouch_allow_movement = 0,
	_character_movement_flag_no_side_step = 1,
	_character_movement_flag_prefer_to_combat_near_friends = 2,
	_character_movement_flag_hop_to_cover_path_segments = 3,
	_character_movement_flags_perch = 4,
	_character_movement_flags_has_flying_move = 5,
	_character_movement_flags_disallow_crouch = 6,
	k_character_movement_flags_count
};

enum e_character_perception_flags : uint32
{
	_character_perception_flag_1 = 0,
	k_character_perception_flag_count
};

enum e_character_vitality_flags : uint32
{
	_character_vitality_flag_unused = 0,
	k_character_vitality_flag_count
};

enum e_character_general_type : uint16
{
	_character_general_type_elite = 0,
	_character_general_type_jackal = 1,
	_character_general_type_grunt = 2,
	_character_general_type_hunter = 3,
	_character_general_type_engineer = 4,
	_character_general_type_assassin = 5,
	_character_general_type_player = 6,
	_character_general_type_marine = 7,
	_character_general_type_crew = 8,
	_character_general_type_combat_form = 9,
	_character_general_type_infection_form = 10,
	_character_general_type_carrier_form = 11,
	_character_general_type_monitor = 12,
	_character_general_type_sentinel = 13,
	_character_general_type_none = 14,
	_character_general_type_mounted_weapon = 15,
	_character_general_type_brute = 16,
	_character_general_type_prophet = 17,
	_character_general_type_bugger = 18,
	_character_general_type_juggernaut = 19
};

enum e_character_general_flags : uint32
{
	_character_general_flag_swarm = 0,
	_character_general_flag_flying = 1,
	_character_general_flag_dual_wields = 2,
	_character_general_flag_uses_gravemind = 3,
	k_character_general_flag_count
};

enum e_character_flags : uint32
{
	_character_flag_1 = 0,
	k_character_flag_count
};

// max: 100
struct character_vehicle_properties
{
	tag_reference unit; // unit
	tag_reference style; // style;

	c_flags_no_init<e_character_vehicle_flags, uint32, k_character_vehicle_flags_count> flags;

	// Explaination("Pathfinding", "")

	/// (Ground vehicles)
	real32 ai_pathfinding_radius_world_units;

	/// (All vehicles) Distance within which goal is considered reached
	real32 ai_destination_radius_world_units;

	/// (All vehicles)Distance from goal at which AI starts to decelerate
	real32 ai_deceleration_distance_world_units;

	// Explaination("Turning", "")

	/// (Warthog, Pelican, Ghost) Idealized average turning radius (should reflect actual vehicle physics)
	real32 ai_turning_radius;

	/// (Warthog-type) Idealized minimum turning radius (should reflect actual vehicle physics)
	real32 ai_inner_turning_radius;

	/// (Warthogs, ghosts) Ideal turning radius for rounding turns (barring obstacles, etc.)
	real32 ai_ideal_turning_radius;

	// Explaination("Steering", "")

	/// (Banshee)
	real32 ai_banshee_steering_maximum;

	/// (Warthog, ghosts, wraiths)Maximum steering angle from forward (ultimately controls turning speed)
	real32 ai_max_steering_angle_degrees;

	/// (pelicans, dropships, ghosts, wraiths)Maximum delta in steering angle from one tick to the next (ultimately controls turn acceleration)
	real32 ai_max_steering_delta_degrees;

	/// (Warthog, ghosts, wraiths)
	real32 ai_oversteering_scale;

	/// (Banshee) Angle to goal at which AI will oversteer
	real_bounds ai_oversteering_bounds;

	/// (Ghosts, Dropships) Distance within which Ai will strafe to target (as opposed to turning)
	real32 ai_sideslip_distance;

	/// (Banshee-style) Look-ahead distance for obstacle avoidance
	real32 ai_avoidance_distance_world_units;

	/// (Banshees)The minimum urgency with which a turn can be made (urgency = percent of maximum steering delta)
	real32 ai_min_urgency;

	// Explaination("Throttle", "")

	/// (All vehicles)
	real32 ai_throttle_maximum;

	/// (Warthogs, Dropships, ghosts)scale on throttle when within 'ai deceleration distance' of goal (0...1)
	real32 ai_goal_min_throttle_scale;

	/// (Warthogs, ghosts) Scale on throttle due to nearness to a turn (0...1)
	real32 ai_turn_min_throttle_scale;

	/// (Warthogs, ghosts) Scale on throttle due to facing away from intended direction (0...1)
	real32 ai_direction_min_throttle_scale;

	/// (warthogs, ghosts) The maximum allowable change in throttle between ticks
	real32 ai_acceleration_scale;

	/// (dropships, sentinels) The degree of throttle blending between one tick and the next (0 = no blending)
	real32 ai_throttle_blend;

	/// (dropships, warthogs, ghosts) About how fast I can go.
	real32 theoretical_max_speed_world_units;

	/// (dropships, warthogs) scale on the difference between desired and actual speed, applied to throttle
	real32 error_scale;

	// Explaination("Combat", "")
	real32 ai_allowable_aim_deviation_angle;

	// Explaination("Behavior", "")

	/// (All vehicles) The distance at which the tight angle criterion is used for deciding to vehicle charge
	real32 ai_charge_tight_angle_distance;

	/// (All vehicles) Angle cosine within which the target must be when target is closer than tight angle distance in order to charge
	real32 ai_charge_tight_angle;

	/// (All vehicles) Time delay between vehicle charges
	real32 ai_charge_repeat_timeout;

	/// (All vehicles) In deciding when to abort vehicle charge, look ahead these many seconds to predict time of contact
	real32 ai_charge_lookahead_time;

	/// Consider charging the target when it is within this range (0 = infinite distance)
	real32 ai_charge_consider_distance;

	/// Abort the charge when the target get more than this far away (0 = never abort)
	real32 ai_charge_abort_distance;

	/// The ram behavior stops after a maximum of the given number of seconds
	real32 vehicle_ram_timeout;

	/// The ram behavior freezes the vehicle for a given number of seconds after performing the ram
	real32 ram_paralysis_time;

	/// (All vehicles) Trigger a cover when recent damage is above given threshold (damage_vehicle_cover impulse)
	real32 ai_cover_damage_threshold;

	/// (All vehicles) When executing vehicle-cover, minimum distance from the target to flee to
	real32 ai_cover_min_distance;

	/// (All vehicles) How long to stay away from the target
	real32 ai_cover_time;

	/// (All vehicles) Boosting allowed when distance to cover destination is greater then this.
	real32 ai_cover_min_boost_distance;

	/// If vehicle turtling behavior is enabled, turtling is initiated if 'recent damage' surpasses the given threshold
	real32 turtling_recent_damage_threshold;

	/// If the vehicle turtling behavior is enabled, turtling occurs for at least the given time
	real32 turtling_min_time_seconds;

	/// The turtled state times out after the given number of seconds
	real32 turtling_timeout_seconds;

	e_character_obstacle_size obstacle_ignore_size;

	int16 pad;
};
ASSERT_STRUCT_SIZE(character_vehicle_properties, 180);

// max: 10
struct character_grenade_properties
{
	c_flags_no_init<e_character_grenade_flags, uint32, k_character_grenade_flags_count> flags;
	e_character_grenade_type type;
	e_character_grenade_trajectory_type trajectory;

	int16 pad;

	/// how many enemies must be within the radius of the grenade before we will consider throwing there
	int16 minimum_enemy_count;

	/// we consider enemies within this radius when determining where to throw
	real32 enemy_radius_world_units;

	/// how fast we LIKE to throw our grenades
	real32 grenade_ideal_velocity_world_units_per_second;

	/// the fastest we can possibly throw our grenades
	real32 grenade_velocity_world_units_per_second;

	/// ranges within which we will consider throwing a grenade
	real_bounds grenade_range_world_units;

	/// we won't throw if there are friendlies around our target within this range
	real32 collateral_damage_radius_world_units;

	/// how likely we are to throw a grenade in one second
	real32 grenade_chance;

	/// How long we have to wait after throwing a grenade before we can throw another one
	real32 grenade_throw_delay_seconds;

	/// how likely we are to throw a grenade to flush out a target in one second
	real32 grenade_uncover_chance;

	/// how likely we are to throw a grenade against a vehicle
	real32 anti_vehicle_grenade_chance;

	// Explaination("Grenade drop when killed", "")

	/// number of grenades that we start with
	short_bounds grenade_count;

	/// how likely we are not to drop any grenades when we die, even if we still have some
	real32 dont_drop_grenades_chance;
};
ASSERT_STRUCT_SIZE(character_grenade_properties, 60);

// max: 2
struct character_firing_pattern
{
	/// how many times per second we pull the trigger (zero = continuously held down)
	real32 rate_of_fire;

	/// how well our bursts track moving targets. 0.0= fire at the position they were standing when we started the burst. 1.0= fire at current position
	real32 target_tracking;

	/// how much we lead moving targets. 0.0= no prediction. 1.0= predict completely.
	real32 target_leading;

	// Explaination("burst geometry", "at the start of every burst we pick a random point near the target to fire at, on either the left or the right side.
	// the burst origin angle controls whether this error is exactly horizontal or might have some vertical component.
	//	over the course of the burst we move our projectiles back in the opposite direction towards the target. this return motion is also controlled by an angle that specifies how close to the horizontal it is.
	//	for example if the burst origin angle and the burst return angle were both zero, and the return length was the same as the burst length, every burst would start the same amount away from the target(on either the left or right) and move back to exactly over the target at the end of the burst.")

	/// how far away from the target the starting point is
	real32 burst_origin_radius_world_units;

	/// the range from the horizontal that our starting error can be
	real32 burst_origin_angle_degrees;

	/// how far the burst point moves back towards the target (could be negative)
	real_bounds burst_return_length_world_units;

	/// the range from the horizontal that the return direction can be
	real32 burst_return_angle_degrees;

	/// how long each burst we fire is
	real_bounds burst_duration_seconds;

	/// how long we wait between bursts
	real_bounds burst_separation_seconds;

	/// what fraction of its normal damage our weapon inflicts (zero = no modifier)
	real32 weapon_damage_modifier;

	/// error added to every projectile we fire
	real32 projectile_error_degrees;

	/// the maximum rate at which we can sweep our fire (zero = unlimited)
	real32 burst_angular_velocity_degrees_per_seconds;

	/// cap on the maximum angle by which we will miss target (restriction on burst origin radius)
	real32 maximum_error_angle_degrees;
};
ASSERT_STRUCT_SIZE(character_firing_pattern, 64);

// max: 100
struct character_firing_pattern_properties
{
	tag_reference weapon; // weap
	tag_block<character_firing_pattern> firing_patterns;
};
ASSERT_STRUCT_SIZE(character_firing_pattern_properties, 16);

// max: 100
struct character_weapon_properties
{
	c_flags_no_init<e_character_weapon_flags, uint32, k_character_weapon_flag_count> flags;

	tag_reference weapon; // weap

	// Explaination("Combat ranges", "")

	/// we can only fire our weapon at targets within this distance
	real32 maximum_firing_range_world_units;

	/// weapon will not be fired at target closer than given distance
	real32 minimum_firing_range_world_units;

	real_bounds normal_combat_range_world_units;

	/// we offset our burst targets randomly by this range when firing at non-visible enemies (zero = never)
	real32 bombardment_range;

	/// Specific target regions on a vehicle or unit will be fired upon only under the given distance
	real32 max_special_target_distance_world_units;

	real_bounds timid_combat_range_world_units;
	real_bounds aggressive_combat_range_world_units;

	// Explaination("Ballistic Firing", "")

	/// we try to aim our shots super-ballistically if target is outside this range (zero = never)
	real32 super_ballistic_range;

	/// At the min range, the min ballistic fraction is used, at the max, the max ballistic fraction is used
	real_bounds ballistic_firing_bounds_world_units;

	/// Controls speed and degree of arc. 0 = high, slow, 1 = low, fast
	real_bounds ballistic_fraction_bounds;

	// Explaination("Behavior", "")

	real_bounds first_burst_delay_time_seconds;

	real32 surprise_delay_time_seconds;

	real32 surprise_fire_wildly_time_seconds;

	real32 death_fire_wildly_chance;

	real32 death_fire_wildly_time_seconds;

	/// custom standing gun offset for overriding the default in the base actor
	real_vector3d custom_stand_gun_offset;

	/// custom crouching gun offset for overriding the default in the base actor
	real_vector3d custom_crouch_gun_offset;

	// Explaination("special-case firing properties", "")

	/// the type of special weapon fire that we can use
	e_character_weapon_special_fire_mode special_fire_mode;

	/// when we will decide to use our special weapon fire mode
	e_character_weapon_special_fire_situation special_fire_situation;

	/// how likely we are to use our special weapon fire mode
	real32 special_fire_chance;

	/// how long we must wait between uses of our special weapon fire mode
	real32 special_fire_delay_seconds;

	/// damage modifier for special weapon fire (applied in addition to the normal damage modifier. zero = no change)
	real32 special_damage_modifier;

	/// projectile error angle for special weapon fire (applied in addition to the normal error)
	real32 special_projectile_error_degrees;

	// Explaination("Weapon drop when killed", "")

	/// amount of ammo loaded into the weapon that we drop (in fractions of a clip, e.g. 0.3 to 0.5)
	real_bounds drop_weapon_loaded;

	/// total number of rounds in the weapon that we drop (ignored for energy weapons)
	short_bounds drop_weapon_ammo;

	// Explaination("Accuracy", "Parameters control how accuracy changes over the duration of a series of bursts
	// Accuracy is an analog value between 0 and 1. At zero, the parameters of the first
	//	firing - pattern block is used.At 1, the parameters in the second block is used.In
	//	between, all the values are linearly interpolated")

	/// Indicates starting and ending accuracies at normal difficulty
	real_bounds normal_accuracy_bounds;

	/// The amount of time it takes the accuracy to go from starting to ending
	real32 normal_accuracy_time;

	/// Indicates starting and ending accuracies at heroic difficulty
	real_bounds heroic_accuracy_bounds;

	/// The amount of time it takes the accuracy to go from starting to ending
	real32 heroic_accuracy_time;

	/// Indicates starting and ending accuracies at legendary difficulty
	real_bounds legendary_accuracy_bounds;

	/// The amount of time it takes the accuracy to go from starting to ending
	real32 legendary_accuracy_time;

	tag_block<character_firing_pattern> firing_patterns;

	tag_reference weapon_melee_damage; // jpt!
};
ASSERT_STRUCT_SIZE(character_weapon_properties, 204);

// max: 1
struct character_boss_properties
{
	int32 pad;

	/// when more than x damage is caused a juggernaut flurry is triggered
	real32 flurry_damage_threshold;

	/// flurry lasts this long
	real32 flurry_time_seconds;
};
ASSERT_STRUCT_SIZE(character_boss_properties, 12);

// max: 1
struct character_boarding_properties
{
	c_flags_no_init<e_character_boarding_flags, uint32, k_character_boarding_flag_count> flags;

	/// maximum distance from entry point that we will consider boarding
	real32 maximum_distance_world_units;

	/// give up trying to get in boarding seat if entry point is further away than this
	real32 abort_distance_world_units;

	/// maximum speed at which we will consider boarding
	real32 maximum_speed_world_units;
};
ASSERT_STRUCT_SIZE(character_boarding_properties, 16);

// max: 1
struct character_vocalization_properties
{
	/// How long does the player look at an AI before the AI responds?
	real32 look_comment_time_seconds;

	/// How long does the player look at the AI before he responds with his 'long look' comment?
	real32 look_long_comment_time_seconds;
};
ASSERT_STRUCT_SIZE(character_vocalization_properties, 8);

// max: k_style_attitude_count
struct character_idle_properties
{
	int32 pad;

	/// time range for delays between idle poses
	real_bounds idle_pose_delay_time_bounds;
};
ASSERT_STRUCT_SIZE(character_idle_properties, 12);

// max: k_style_attitude_count
struct character_presearch_properties
{
	c_flags_no_init<e_character_presearch_flags, uint32, k_character_presearch_flag_count> flags;

	/// If the min presearch time expires and the target is (actually) outside the min-certainty radius, presearch turns off
	real_bounds minimum_presearch_time_bounds;

	/// Presearch turns off after the given time
	real_bounds maximum_presearch_time_bounds;

	real32 minimum_certainty_radius;
	real32 unused;

	/// if the min suppressing time expires and the target is outside the min-certainty radius, suppressing fire turns off
	real_bounds supressing_time_bounds;
};
ASSERT_STRUCT_SIZE(character_presearch_properties, 36);

// max: k_style_attitude_count
struct character_search_properties
{
	c_flags_no_init<e_character_search_flags, uint32, k_character_search_flag_count> flags;

	real_bounds search_time;

	// Explaination("Uncover", "")

	/// Distance of uncover point from target. Hard lower limit, soft upper limit.
	real_bounds uncover_distance_bounds;
};
ASSERT_STRUCT_SIZE(character_search_properties, 20);

// max: k_style_attitude_count
struct character_retreat_properties
{
	c_flags_no_init<e_character_retreat_flags, uint32, k_character_retreat_flag_count> flags;

	/// When shield vitality drops below given amount, retreat is triggered by low_shield_retreat_impulse
	real32 shield_threshold;

	/// When confronting an enemy of over the given scariness, retreat is triggered by scary_target_retreat_impulse
	real32 scary_target_threshold;

	/// When perceived danger rises above the given threshold, retreat is triggered by danger_retreat_impulse
	real32 danger_threshold;

	/// When enemy closer than given threshold, retreat is triggered by proximity_retreat_impulse
	real32 proximity_threshold;

	/// actor cowers for at least the given amount of time
	real_bounds force_cower_time_bounds;

	/// actor times out of cower after the given amount of time
	real_bounds cower_time_out_bounds;

	/// If target reaches is within the given proximity, an ambush is triggered by the proximity ambush impulse
	real32 proximity_ambush_threshold;

	/// If target is less than threshold (0-1) aware of me, an ambush is triggered by the vulnerable enemy ambush impulse
	real32 awareness_ambush_threshold;

	/// If leader-dead-retreat-impulse is active, gives the chance that we will flee when our leader dies within 4 world units of us
	real32 leader_dead_retreat_chance;

	/// If peer-dead-retreat-impulse is active, gives the chance that we will flee when one of our peers (friend of the same race) dies within 4 world units of us
	real32 peer_dead_retreat_chance;

	/// If peer-dead-retreat-impulse is active, gives the chance that we will flee when a second peer (friend of the same race) dies within 4 world units of us
	real32 second_peer_dead_retreat_chance;

	/// The angle from the intended destination direction that a zig-zag will cause
	real32 zig_zag_angle_degrees;

	/// How long it takes to zig left and then zag right.
	real32 zig_zag_period_seconds;

	/// The likelihood of throwing down a grenade to cover our retreat
	real32 retreat_grenade_chance;

	/// If I want to flee and I don't have flee animations with my current weapon, throw it away and try a ...
	tag_reference backup_weapon; // weap
};
ASSERT_STRUCT_SIZE(character_retreat_properties, 76);

// max: k_style_attitude_count
struct character_cover_properties
{
	c_flags_no_init<e_character_cover_flags, uint32, k_character_cover_flag_count> flags;

	/// how long we stay behind cover after seeking cover
	real_bounds hide_behind_cover_time_seconds;

	/// When vitality drops below this level, possibly trigger a cover
	real32 cover_vitality_threshold;

	/// trigger cover when shield drops below this fraction (low shield cover impulse must be enabled)
	real32 cover_shield_fraction;

	/// amount of time I will wait before trying again after covering
	real32 cover_check_delay;

	/// Emerge from cover when shield fraction reaches threshold
	real32 emerge_from_cover_when_shield_fraction_reaches_threshold;

	/// Danger must be this high to cover. At a danger level of 'danger threshold', the chance of seeking cover is the cover chance lower bound (below)
	real32 cover_danger_threshold;

	/// At or above danger level of upper threshold, the chance of seeking cover is the cover chance upper bound (below)
	real32 danger_upper_threshold;

	// Explaination("Cover chance", "The Bounds on the chance of seeking cover.
	// The lower bound is valid when the danger is at 'danger threshold'
	//	The upper bound is valid when the danger is at or above 'danger upper threshold'.
	//	It is interpolated linearly everywhere in between.
	//	All chances are expressed as 'chance of triggering cover in a 1 second period'.")

	/// Bounds on the chances of seeking cover.
	real_bounds cover_chance;

	/// When the proximity_self_preservation impulse is enabled, triggers self-preservation when target within this distance
	real32 proximity_self_preservation;

	/// Disallow covering from visible target under the given distance away
	real32 disallow_cover_distance_world_units;

	/// When self preserving from a target less than given distance, causes melee attack (assuming proximity_melee_impulse is enabled)
	real32 proximity_melee_distance;

	/// When danger from an unreachable enemy surpasses threshold, actor cover (assuming unreachable_enemy_cover impulse is enabled)
	real32 unreachable_enemy_danger_threshold;

	/// When target is aware of me and surpasses the given scariness, self-preserve (scary_target_cover_impulse)
	real32 scary_target_threshold;
};
ASSERT_STRUCT_SIZE(character_cover_properties, 64);

// max: k_style_attitude_count
struct character_evasion_properties
{
	/// Consider evading when immediate danger surpasses threshold
	real32 evasion_danger_threshold;

	/// Wait at least this delay between evasions
	real32 evasion_delay_timer;

	/// If danger is above threshold, the chance that we will evade. Expressed as chance of evading within a 1 second time period
	real32 evasion_chance;

	/// If target is within given proximity, possibly evade
	real32 evasion_proximity_threshold;

	/// Chance of retreating (fleeing) after danger avoidance dive
	real32 dive_retreat_chance;
};
ASSERT_STRUCT_SIZE(character_evasion_properties, 20)

// max: k_style_attitude_count
struct character_charge_properties
{
	c_flags_no_init<e_character_charge_flags, uint32, k_character_char_flag_count> flags;

	real32 melee_consider_range;

	/// chance of initiating a melee within a 1 second period
	real32 melee_chance;
	real32 melee_attack_range;
	real32 melee_abort_range;

	/// Give up after given amount of time spent charging
	real32 melee_attack_timeout_seconds;

	/// don't attempt again before given time since last melee
	real32 melee_attack_delay_timer_seconds;

	real_bounds melee_leap_range;
	real32 melee_leap_chance;
	real32 ideal_leap_velocity;
	real32 max_leap_velocity;
	real32 melee_leap_ballistic;

	/// time between melee leaps
	real32 melee_delay_timer_seconds;

	/// when I berserk, I pull out a ...
	tag_reference berserk_weapon; // weap
};
ASSERT_STRUCT_SIZE(character_charge_properties, 64);

// max: k_style_attitude_count
struct character_engage_properties
{
	c_flags_no_init<e_character_engage_flags, uint32, k_character_engage_flag_count> flags;

	/// When danger rises above the threshold, the actor crouches
	real32 crouch_danger_threshold;

	/// When danger drops below this threshold, the actor can stand again.
	real32 stand_danger_threshold;

	/// When danger goes above given level, this actor switches firing positions
	real32 fight_danger_move_threshold;
};
ASSERT_STRUCT_SIZE(character_engage_properties, 16)

// max: k_style_attitude_count
struct character_ready_properties
{
	/// Character will pause for given time before engaging threat
	real_bounds ready_time_bounds;
};
ASSERT_STRUCT_SIZE(character_ready_properties, 8);

// max: k_style_attitude_count
struct character_swarm_properties
{
	/// After the given number of deaths, the swarm scatters
	uint16 scatter_killed_count;
	uint16 pad;

	/// the distance from the target that the swarm scatters
	real32 scatter_radius;

	/// amount of time to remain scattered
	real32 scatter_time;

	real_bounds hound_distance;

	/// amount of randomness added to creature's throttle
	real32 perlin_offset_scale;

	/// how fast the creature changes random offset to throttle
	real_bounds offset_period;

	/// a random offset lower then given threshold is made 0. (threshold of 1 = no movement)
	real32 perlin_idle_movement_threshold;

	/// a random offset lower then given threshold is made 0. (threshold of 1 = no movement)
	real32 perlin_combat_movement_threshold;
};
ASSERT_STRUCT_SIZE(character_swarm_properties, 40);

// max: 1
struct character_movement_properties
{
	c_flags_no_init<e_character_movement_flags, uint32, k_character_movement_flags_count> flags;

	real32 pathfinding_radius;
	real32 destination_radius;
	real32 dive_grenade_chance;

	e_character_obstacle_size obstacle_leap_min_size;
	e_character_obstacle_size obstacle_leap_max_size;
	e_character_obstacle_size obstacle_ignore_size;
	e_character_obstacle_size obstacle_smashable_size;

	int16 pad;

	e_character_movement_jump_height jump_height;
	c_flags_no_init<e_character_movement_hint_flags, uint32, k_character_movement_hint_flag_count> movement_hints;

	real32 throttle_scale;
};
ASSERT_STRUCT_SIZE(character_movement_properties, 36);

// max: 1
struct character_look_properties
{
	/// how far we can turn our weapon
	real_euler_angles2d maximum_aiming_deviation_degrees;

	/// how far we can turn our head
	real_euler_angles2d maximum_looking_deviation_degrees;

	int8 padding[16];

	/// how far we can turn our head left/right away from our aiming vector when not in combat
	real_euler_angles2d non_combat_look_delta_degree;

	/// how far we can turn our head left/right away from our aiming vector when in combat
	real_euler_angles2d combat_look_delta_degree;

	/// rate at which we change look around randomly when not in combat
	real_bounds non_combat_idle_looking_seconds;

	/// rate at which we change aiming directions when looking around randomly when not in combat
	real_bounds non_combat_idle_aiming_seconds;

	/// rate at which we change look around randomly when searching or in combat
	real_bounds combat_idle_looking_seconds;

	/// rate at which we change aiming directions when looking around randomly when searching or in combat
	real_bounds comba_idle_aiming_seconds;
};
ASSERT_STRUCT_SIZE(character_look_properties, 80);

// max: 4
struct character_perception_properties
{
	c_flags_no_init<e_character_perception_flags, uint32, k_character_perception_flag_count> flags;

	/// maximum range of sight
	real32 max_vision_dstance_world_units;

	/// horizontal angle within which we see targets out to our maximum range
	real32 central_vision_angle_degrees;

	/// maximum horizontal angle within which we see targets at range
	real32 max_vision_angle_degrees;

	/// maximum horizontal angle within which we can see targets out of the corner of our eye
	real32 peripheral_vision_angle_degrees;

	/// maximum range at which we can see targets our of the corner of our eye
	real32 peripheral_distance_world_units;

	/// maximum range at which sounds can be heard
	real32 hearing_distance_world_units;

	/// random chance of noticing a dangerous enemy projectile (e.g. grenade)
	real32 notice_projectile_chance;

	/// random chance of noticing a dangerous vehicle
	real32 notice_vehicle_chance;

	/// time required to acknowledge a visible enemy when we are already in combat or searching for them
	real32 combat_perception_time_seconds;

	/// time required to acknowledge a visible enemy when we have been alerted
	real32 guard_perception_time_seconds;

	/// time required to acknowledge a visible enemy when we are not alerted
	real32 non_combat_perception_time_seconds;

	/// If a new prop is acknowledged within the given distance, surprise is registerd
	real32 first_ack_surprise_distance_world_units;
};
ASSERT_STRUCT_SIZE(character_perception_properties, 52);

// max: 1
struct character_placement_properties
{
	int32 pad;

	real32 few_upgrade_chance_easy;
	real32 few_upgrade_chance_normal;
	real32 few_upgrade_chance_heroic;
	real32 few_upgrade_chance_legendary;

	real32 normal_upgrade_chance_easy;
	real32 normal_upgrade_chance_normal;
	real32 normal_upgrade_chance_heroic;
	real32 normal_upgrade_chance_legendary;

	real32 many_upgrade_chance_easy;
	real32 many_upgrade_chance_normal;
	real32 many_upgrade_chance_heroic;
	real32 many_upgrade_chance_legendary;
};

// max: 1
struct character_vitality_properties
{
	c_flags_no_init<e_character_vitality_flags, uint32, k_character_vitality_flag_count> flags;

	/// maximum body vitality of our unit
	real32 normal_body_vitality;

	/// maximum shield vitality of our unit
	real32 normal_sheild_vitality;

	/// maximum body vitality of our unit (on legendary)
	real32 legendary_body_vitality;

	/// maximum shield vitality of our unit (on legendary)
	real32 legendary_shield_vitality;

	/// fraction of body health that can be regained after damage
	real32 body_recharge_fraction;

	/// damage necessary to trigger a soft ping when shields are up
	real32 soft_ping_threshold_with_shields;

	/// damage necessary to trigger a soft ping when shields are down
	real32 soft_ping_threshold_no_shields;

	/// minimum time before a soft ping can be interrupted
	real32 soft_ping_min_interrupt_time;

	/// damage necessary to trigger a hard ping when shields are up
	real32 hard_ping_threshold_with_shields;

	/// damage necessary to trigger a hard ping when shields are down
	real32 hard_ping_threshold_no_shields;

	/// minimum time before a hard ping can be interrupted
	real32 hard_ping_min_interrupt_time;

	/// current damage begins to fall after a time delay has passed since last the damage
	real32 current_damage_decay_delay;

	/// amount of time it would take for 100% current damage to decay to 0
	real32 current_damage_decay_time;

	/// recent damage begins to fall after a time delay has passed since last the damage
	real32 recent_damage_decay_delay;

	/// amount of time it would take for 100% recent damage to decay to 0
	real32 recent_damage_decay_time;

	/// amount of time delay before a shield begins to recharge
	real32 body_recharge_delay_time;

	/// amount of time for shields to recharge completely
	real32 body_recharge_time;

	/// amount of time delay before a shield begins to recharge
	real32 shield_recharge_delay_time;

	/// amount of time for shields to recharge completely
	real32 shield_recharge_time;

	/// stun level that triggers the stunned state (currently, the 'stunned' behavior)
	real32 stun_threshold;

	real_bounds stun_time_bounds_seconds;

	/// Amount of shield damage sustained before it is considered 'extended'
	real32 extended_shield_damage_threshold;

	/// Amount of body damage sustained before it is considered 'extended'
	real32 extended_body_damage_threshold;

	/// when I die and explode, I damage stuff within this distance of me.
	real32 suicide_radius;
};
ASSERT_STRUCT_SIZE(character_vitality_properties, 104)

// max: 1
struct character_general_properties
{
	c_flags_no_init<e_character_general_flags, uint32, k_character_general_flag_count> flags;
	e_character_general_type type;
	int16 pad;

	// the inherent scariness of the character
	real32 scariness;
};
ASSERT_STRUCT_SIZE(character_general_properties, 12);

// max: k_maximum_variants_per_model
struct character_variant
{
	string_id variant_name;
	int16 variant_index;
	int16 pad;
	string_id variant_designator;
};
ASSERT_STRUCT_SIZE(character_variant, 12);

struct character_definition
{
	c_flags_no_init<e_character_flags, uint32, k_character_flag_count> flags;

	tag_reference parent_character; // char
	tag_reference unit; // unit

	// Creature reference for swarm characters ONLY
	tag_reference creature; // crea

	tag_reference style; // styl
	tag_reference major_character; // char

	tag_block<character_variant> variants;
	tag_block<character_general_properties> general;
	tag_block<character_vitality_properties> vitality;
	tag_block<character_placement_properties> placement;
	tag_block<character_perception_properties> perception;
	tag_block<character_look_properties> look;
	tag_block<character_movement_properties> movement;
	tag_block<character_swarm_properties> swarm;
	tag_block<character_ready_properties> ready;
	tag_block<character_engage_properties> engage;
	tag_block<character_charge_properties> charge;

	// Explaination("Danger Values", "Danger values can be found in the ai-globals section of the globals tag.")

	tag_block<character_evasion_properties> evasion;
	tag_block<character_cover_properties> cover;
	tag_block<character_retreat_properties> retreat;
	tag_block<character_search_properties> search;
	tag_block<character_presearch_properties> presearch;
	tag_block<character_idle_properties> idle;
	tag_block<character_vocalization_properties> vocalization;
	tag_block<character_boarding_properties> boarding;
	tag_block<character_boss_properties> boss;
	tag_block<character_weapon_properties> weapon;
	tag_block<character_firing_pattern_properties> firing_pattern;
	tag_block<character_grenade_properties> grenade;
	tag_block<character_vehicle_properties> vehicle;
};
ASSERT_STRUCT_SIZE(character_definition, 236);