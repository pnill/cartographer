#pragma once
#include "dialogue_definitions.h"
#include "unit_action_system.h"
#include "units.h"

#include "Blam/Engine/ai/ai.h"
#include "Blam/Engine/math/integer_math.h"
#include "Blam/Engine/interface/motion_sensor.h"
#include "Blam/Engine/objects/object_definition.h"
#include "Blam/Engine/objects/objects.h"

enum e_unit_definition_flags : int32
{
    _unit_definition_circular_aiming = FLAG(0),
    _unit_definition_destroyed_after_dying = FLAG(1),
    _unit_definition_halfspeed_interpolation = FLAG(2),
    _unit_definition_fires_from_camera = FLAG(3),
    _unit_definition_entrance_inside_bounding_sphere = FLAG(4),
    _unit_definition_doesnt_show_readied_weapon = FLAG(5),
    _unit_definition_causes_passenger_dialogue = FLAG(6),
    _unit_definition_resists_pings = FLAG(7),
    _unit_definition_melee_attack_is_fatal = FLAG(8),
    _unit_definition_dont_reface_during_pings = FLAG(9),
    _unit_definition_HasNoAiming = FLAG(10),
    _unit_definition_simple_creature = FLAG(11),
    _unit_definition_impact_melee_attaches_to_unit = FLAG(12),
    _unit_definition_impact_melee_dies_on_shields = FLAG(13),
    _unit_definition_cannot_open_doors_automatically = FLAG(14),
    _unit_definition_melee_attackers_cannot_attach = FLAG(15),
    _unit_definition_not_instantly_killed_by_melee = FLAG(16),
    _unit_definition_shield_sapping = FLAG(17),
    _unit_definition_runs_around_flaming = FLAG(18),
    _unit_definition_inconsequential = FLAG(19),
    _unit_definition_special_cinematic_unit = FLAG(20),
    _unit_definition_ignored_by_autoaiming = FLAG(21),
    _unit_definition_shields_fry_infection_forms = FLAG(22),
    _unit_definition_unused = FLAG(23),
    _unit_definition_unused_1 = FLAG(24),
    _unit_definition_acts_as_gunner_for_parent = FLAG(25),
    _unit_definition_controlled_by_parent_gunner = FLAG(26),
    _unit_definition_parents_primary_weapon = FLAG(27),
    _unit_definition_unit_has_boost = FLAG(28)
};

enum e_unit_seat_definition_flags : uint32
{
    _unit_seat_definition_invisible = FLAG(0),
    _unit_seat_definition_locked = FLAG(1),
    _unit_seat_definition_driver = FLAG(2),
    _unit_seat_definition_gunner = FLAG(3),
    _unit_seat_definition_third_person_camera = FLAG(4),
    _unit_seat_definition_allows_weapons = FLAG(5),
    _unit_seat_definition_third_person_on_enter = FLAG(6),
    _unit_seat_definition_first_person_camera_slaved_to_gun = FLAG(7),
    _unit_seat_definition_allow_vehicle_communication_animations = FLAG(8),
    _unit_seat_definition_not_valid_without_driver = FLAG(9),
    _unit_seat_definition_allow_ai_noncombatants = FLAG(10),
    _unit_seat_definition_boarding_seat = FLAG(11),
    _unit_seat_definition_ai_firing_disabled_by_max_acceleration = FLAG(12),
    _unit_seat_definition_boarding_enters_seat = FLAG(13),
    _unit_seat_definition_boarding_need_any_passenger = FLAG(14),
    _unit_seat_definition_controls_open_and_close = FLAG(15),
    _unit_seat_definition_invalid_for_player = FLAG(16),
    _unit_seat_definition_invalid_for_nonplayer = FLAG(17),
    _unit_seat_definition_gunnerplayer_only = FLAG(18),
    _unit_seat_definition_invisible_under_major_damage = FLAG(19)
};

// max count: MAXIMUM_NUMBER_OF_UNIT_CAMERA_TRACKS 2
struct s_unit_camera_track
{
    tag_reference track;
};
TAG_BLOCK_SIZE_ASSERT(s_unit_camera_track, 8);

struct s_unit_camera
{
    string_id camera_marker_name;
    string_id camera_submerged_marker_name;
    real32 pitch_auto_level;
    real_bounds pitch_range;
    tag_block<s_unit_camera_track> camera_tracks;
};
CHECK_STRUCT_SIZE(s_unit_camera, 28);

struct unit_seat_acceleration
{
    real_vector3d acceleration_range;       // World Units Per Second Squared;
    real32 acceleration_action_scale;       // Actions Fail
    real32 acceleration_attach_scale;       // Actions Fail
};
CHECK_STRUCT_SIZE(unit_seat_acceleration, 20);

struct s_unit_additional_node_names
{
    string_id preferredgunnode;    // if found, use this gun marker
};
CHECK_STRUCT_SIZE(s_unit_additional_node_names, 4);

struct unit_boarding_melee_struct_block
{
    tag_reference boarding_melee_damage;    // jpt!
    tag_reference boarding_melee_response;  // jpt!
    tag_reference landing_melee_damage;     // jpt!
    tag_reference flurry_melee_damage;      // jpt!
    tag_reference obstacle_smash_damage;    // jpt!
};
CHECK_STRUCT_SIZE(unit_boarding_melee_struct_block, 40);

// max count: NUMBER_OF_UNIT_HUD_TYPES 2
struct unit_hud_reference
{
    tag_reference new_unit_hud_interface;   // nhdt
};
TAG_BLOCK_SIZE_ASSERT(unit_hud_reference, 8);

// max count: k_powered_seats_count 2
struct powered_seat_definition
{
    real32 driver_powerup_time_seconds;
    real32 driver_powerdown_time_seconds;
};
TAG_BLOCK_SIZE_ASSERT(powered_seat_definition, 8);

// max count: MAXIMUM_INITIAL_WEAPONS_PER_UNIT 4
struct unit_initial_weapon
{
    tag_reference weapon;   // weap
};
TAG_BLOCK_SIZE_ASSERT(unit_initial_weapon, 8);

// max count: MAXIMUM_SEATS_PER_UNIT_DEFINITION 32
struct unit_seat
{
    e_unit_seat_definition_flags flags;
    string_id label;
    string_id marker_name;
    string_id entry_markers_name;
    string_id boarding_grenade_marker;
    string_id boarding_grenade_string;
    string_id boarding_melee_string;

    real32 ping_scale;  // nathan is too lazy to make pings for each seat.
    real32 turnover_time_seconds;   // how much time it takes to evict a rider from a flipped vehicle
    unit_seat_acceleration acceleration;
    real32 ai_scariness;
    int16 boarding_seat;    // block index: unit_seat_block
    real32 listener_interpolation_factor; // how far to interpolate listener position from camera to occupant's head

    /* Explaination("speed dependant turn rates", "when the unit velocity is 0, the yaw/pitch rates are the left values
    at [max speed reference], the yaw/pitch rates are the right values.
    the max speed reference is what the code uses to generate a clamped speed from 0..1
    the exponent controls how midrange speeds are interpreted.") */
    real_vector2d yaw_rate_bounds;      // Degrees Per Second
    real_vector2d pitch_rate_bounds;    // Degrees Per Second;
    real32 min_speed_reference;
    real32 max_speed_reference;
    real32 speed_exponent;

    // Explaination("camera fields", "")
    s_unit_camera unit_camera;

    tag_block<unit_hud_reference> unit_hud_interface;

    string_id enter_seat_string;
    real32 yaw_minimum;
    real32 yaw_maximum;
    tag_reference builtin_gunner;    // char
    
    /* Explaination("entry fields", "note: the entry radius shouldn't exceed 3 world units, 
    as that is as far as the player will search for a vehicle to enter.")*/
        
    real32 entry_radius;                // how close to the entry marker a unit must be
    real32 entry_marker_cone_angle;     // angle from marker forward the unit must be
    real32 entry_marker_facing_angle;   // angle from unit facing the marker must be
    real32 maximum_relative_velocity;
    string_id invisible_seat_region;
    real32 runtime_invisible_seat_region_index;
};
TAG_BLOCK_SIZE_ASSERT(unit_seat, 176);

struct unit_boost_struct_block
{
    real32 boost_peak_power;
    real32 boost_rise_power;
    real32 boost_peak_time;
    real32 boost_fall_power;
    real32 dead_time;
};
CHECK_STRUCT_SIZE(unit_boost_struct_block, 20);

struct s_unit_lipsync_scales
{
    real32 attack_weight;
    real32 decay_weight;
};
CHECK_STRUCT_SIZE(s_unit_lipsync_scales, 8);

struct _unit_definition
{
    object_definition object;
    e_unit_definition_flags flags;
    e_game_team default_team;
    e_ai_sound_volume constant_sound_volume;
    tag_reference integrated_light_toggle;  // effe
    real_angle camera_field_of_view;
    real32 camera_stiffness;
    s_unit_camera unit_camera;
    unit_seat_acceleration acceleration;

    real32 soft_ping_threshold;
    real32 soft_ping_interrupt_time;
    real32 hard_ping_threshold;
    real32 hard_ping_interrupt_time;
    real32 hard_death_threshold;
    real32 feign_death_threshold;
    real32 feign_death_time;

    real32 distance_of_evade_animation; // (World Units) this must be set to tell the AI how far it should expect our evade animation to move us
    real32 distance_of_dive_animation;  // (World Units) this must be set to tell the AI how far it should expect our dive animation to move us

    real32 stunned_movement_threshold;   // if we take this much damage in a short space of time we will play our 'stunned movement' animations
    real32 feign_death_chance;
    real32 feign_repeat_chance;

    // char
    tag_reference spawned_turret_character; // automatically created character when this unit is driven
    short_bounds spawned_actor_count;       // number of actors which we spawn

    real32 spawned_velocity;
    real32 aiming_velocity_maximum;     // Degrees Per Second
    real32 aiming_acceleration_maximum; // Degrees Per Second Squared;
    real32 casual_aiming_modifier;
    real32 looking_velocity_maximum_degrees_per_second;
    real32 looking_acceleration_maximum_degrees_per_second_squared;

    string_id righthandnode;    // where the primary weapon is attached
    string_id lefthandnode;     // where the seconday weapon is attached (for dual-pistol modes)
    s_unit_additional_node_names more_damn_nodes;

    tag_reference melee_damage; // jpt!
    unit_boarding_melee_struct_block your_momma;
    e_motion_sensor_blip_size motion_sensor_blip_size;
    int16 pad;

    tag_block<s_posture_definition> postures;
    tag_block<unit_hud_reference> new_hud_interfaces;
    tag_block<dialogue_variant_definition> dialogue_variants;

    real32 grenade_velocity;    // World Units Per Second
    e_unit_grenade_type grenade_type;
    int16 grenade_count;
    tag_block<powered_seat_definition> powered_seats;
    tag_block<unit_initial_weapon> weapons;
    tag_block<unit_seat> seats;

    // Explaination("Boost", "")
    unit_boost_struct_block boost;

    // Explaination("Lipsync", "")
    s_unit_lipsync_scales lipsync;
};
TAG_GROUP_SIZE_ASSERT(_unit_definition, 492);
