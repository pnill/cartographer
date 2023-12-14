#pragma once
#include "item_definitions.h"

#include "Blam/Engine/cache/predicted_resources.h"
#include "Blam/Engine/game/aim_assist.h"
#include "Blam/Engine/game/players.h"
#include "Blam/Engine/math/integer_math.h"
#include "Blam/Engine/math/periodic_functions.h"
#include "Blam/Engine/objects/damage_reporting.h"

enum e_weapon_definition_flags : int32
{
    _weapon_definition_vertical_heat_display = FLAG(0),
    _weapon_definition_mutually_exclusive_triggers = FLAG(1),
    _weapon_definition_attacks_automatically_on_bump = FLAG(2),
    _weapon_definition_must_be_readied = FLAG(3),
    _weapon_definition_doesnt_count_toward_maximum = FLAG(4),
    _weapon_definition_aim_assists_only_when_zoomed = FLAG(5),
    _weapon_definition_prevents_grenade_throwing = FLAG(6),
    _weapon_definition_must_be_picked_up = FLAG(7),
    _weapon_definition_holds_triggers_when_dropped = FLAG(8),
    _weapon_definition_prevents_melee_attack = FLAG(9),
    _weapon_definition_detonates_when_dropped = FLAG(10),
    _weapon_definition_cannot_fire_at_maximum_age = FLAG(11),
    _weapon_definition_secondary_trigger_overrides_grenades = FLAG(12),
    _weapon_definition_obsolete_does_not_depower_active_camo_in_multilplayer = FLAG(13),
    _weapon_definition_enables_integrated_night_vision = FLAG(14),
    _weapon_definition_ais_use_weapon_melee_damage = FLAG(15),
    _weapon_definition_forces_no_binoculars = FLAG(16),
    _weapon_definition_loop_fp_firing_animation = FLAG(17),
    _weapon_definition_prevents_sprinting = FLAG(18),
    _weapon_definition_cannot_fire_while_boosting = FLAG(19),
    _weapon_definition_prevents_driving = FLAG(20),
    _weapon_definition_prevents_gunning = FLAG(21),
    _weapon_definition_can_be_dual_wielded = FLAG(22),
    _weapon_definition_can_only_be_dual_wielded = FLAG(23),
    _weapon_definition_melee_only = FLAG(24),
    _weapon_definition_cant_fire_if_parent_dead = FLAG(25),
    _weapon_definition_weapon_ages_with_each_kill = FLAG(26),
    _weapon_definition_weapon_uses_old_dual_fire_error_code = FLAG(27),
    _weapon_definition_primary_trigger_melee_attacks = FLAG(28),
    _weapon_definition_cannot_be_used_by_player = FLAG(29)
};

enum e_secondary_trigger_mode : int16
{
    _secondary_trigger_mode_normal = 0,
    _secondary_trigger_mode_slaved_to_primary = 1,
    _secondary_trigger_mode_inhibits_primary = 2,
    _secondary_trigger_mode_loads_alterate_ammunition = 3,
    _secondary_trigger_mode_loads_multiple_primary_ammunition = 4
};

enum e_movement_penalized : int16
{
    _movement_penalized_always = 0,
    _movement_penalized_when_zoomed = 1,
    _movement_penalized_when_zoomed_or_reloading = 2
};

enum e_multiplayer_weapon_type : int16
{
    _multiplayer_weapon_none = 0,
    _multiplayer_weapon_ctf_flag = 1,
    _multiplayer_weapon_oddball_ball = 2,
    _multiplayer_weapon_headhunter_head = 3,
    _multiplayer_weapon_juggernaut_powerup = 4
};

enum e_weapon_type : int16
{
    _weapon_type_undefined = 0,
    _weapon_type_shotgun = 1,
    _weapon_type_needler = 2,
    _weapon_type_plasma_pistol = 3,
    _weapon_type_plasma_rifle = 4,
    _weapon_type_rocket_launcher = 5,
};

enum e_tracking_type : int16
{
    _tracking_type_no_tracking = 0,
    _tracking_human_tracking = 1,
    _tracking_plasma_tracking = 2
};

enum e_magazine_definition_flags : int32
{
    _magazine_wastes_rounds_when_reloaded = FLAG(0),
    _magazine_every_round_must_be_chambered = FLAG(1)
};

enum e_weapon_trigger_definition_flags : int32
{
    _weapon_trigger_definition_autofire_single_action_only = FLAG(0)
};

enum e_weapon_trigger_definition_input : int16
{
    _weapon_trigger_input_right_trigger = 0,
    _weapon_trigger_input_left_trigger = 1,
    _weapon_trigger_input_melee_attack = 2
};

enum e_weapon_trigger_definition_behavior : int16
{
    _weapon_trigger_behavior_spew = 0,
    _weapon_trigger_behavior_latch = 1,
    _weapon_trigger_behavior_latchautofire = 2,
    _weapon_trigger_behavior_charge = 3,
    _weapon_trigger_behavior_latchzoom = 4,
    _weapon_trigger_behavior_latchrocketlauncher = 5
};

enum e_weapon_trigger_definition_prediction : int16
{
    _weapon_trigger_prediction_none = 0,
    _weapon_trigger_prediction_spew = 1,
    _weapon_trigger_prediction_charge = 2
};

enum e_weapon_barrel_definition_flags : int32
{
    _weapon_barrel_definition_tracks_fired_projectile = FLAG(0),        // Poo Poo Ca Ca Pee Pee 
    _weapon_barrel_definition_random_firing_effects = FLAG(1),          // Rather Than Being Chosen Sequentially Firing Effects Are Picked Randomly
    _weapon_barrel_definition_can_fire_with_partial_ammo = FLAG(2),     // Allows A Weapon To Be Fired As Long As There Is A Nonzero Amount Of Ammunition Loaded
    _weapon_barrel_definition_projectiles_use_weapon_origin = FLAG(3),  // Instead Of Coming Out Of The Magic First Person Camera Origin The Projectiles For This Weapon Actually Come Out Of The Gun          
    _weapon_barrel_definition_ejects_during_chamber = FLAG(4),          // This Triggers Ejection Port Is Started During The Key Frame Of Its Chamber Animation
    _weapon_barrel_definition_use_error_when_unzoomed = FLAG(5),
    _weapon_barrel_definition_projectile_vector_cannot_be_adjusted = FLAG(6),   // Projectiles Fired By This Weapon Cannot Have Their Direction Adjusted By The AI To Hit The Target
    _weapon_barrel_definition_projectiles_have_identical_error = FLAG(7),   
    _weapon_barrel_definition_projectiles_fire_parallel = FLAG(8),              // If There Are Multiple Guns For This Trigger The Projectiles Emerge In Parallel Beams rather Than Independant Aiming
    _weapon_barrel_definition_cant_fire_when_others_firing = FLAG(9),
    _weapon_barrel_definition_cant_fire_when_others_recovering = FLAG(10),
    _weapon_barrel_definition_dont_clear_fire_bit_after_recovering = FLAG(11),
    _weapon_barrel_definition_stagger_fire_across_multiple_markers = FLAG(12),
    _weapon_barrel_definition_fires_locked_projectiles = FLAG(13)
};

enum e_barrel_prediction_type : int16
{
    _barrel_prediction_type_none = 0,
    _barrel_prediction_type_continuous = 1,
    _barrel_prediction_type_instant = 2
};

enum e_barrel_firing_noise : int16
{
    _barrel_firing_noise_silent = 0,
    _barrel_firing_noise_medium = 1,
    _barrel_firing_noise_loud = 2,
    _barrel_firing_noise_shout = 3,
    _barrel_firing_noise_quiet = 4
};

enum e_barrel_distribution_function : int16
{
    _barrel_distribution_function_point = 0,
    _barrel_distribution_function_horizontal_fan = 1
};

struct s_melee_damage_parameters
{
    real_euler_angles2d damage_pyramid_angles;
    real32 damage_pyramid_depth;
    // Explaination("melee combo damage", "")

    tag_reference first_hit_melee_damage;   // jpt!
    tag_reference first_hit_melee_response; // jpt!
    tag_reference second_hit_melee_damage;  // jpt!
    tag_reference second_hit_melee_response;// jpt!
    tag_reference third_hit_melee_damage;   // jpt!
    tag_reference third_hit_melee_response; // jpt!

    // this is only important for the energy sword
    tag_reference lunge_melee_damage;   // jpt!
    // this is only important for the energy sword
    tag_reference lunge_melee_response;  // jpt!
};
CHECK_STRUCT_SIZE(s_melee_damage_parameters, 76);

struct weapon_tracking_struct_block
{
    e_tracking_type tracking_type;
    int16 pad;
};
CHECK_STRUCT_SIZE(weapon_tracking_struct_block, 4);

// max count: k_player_character_type_count
struct weapon_first_person_interface_definition
{
    tag_reference model;        // mode
    tag_reference animations;   // jmad
};
TAG_BLOCK_SIZE_ASSERT(weapon_first_person_interface_definition, 16);

struct weapon_interface_definition_new
{
    // Explaination("interface", "")

    weapon_first_person_interface_definition shared_interface; // default interface? (unsure as this is labeled as padding)
    tag_block<weapon_first_person_interface_definition> first_person;
    tag_reference new_hud_interface;  // nhdt
};
CHECK_STRUCT_SIZE(weapon_interface_definition_new, 32);

// max count: MAXIMUM_NUMBER_OF_MAGAZINE_OBJECTS_PER_MAGAZINE
struct weapon_ammunition_object
{
    int16 rounds;
    int16 pad;
    tag_reference equipment;    // eqip
};
TAG_BLOCK_SIZE_ASSERT(weapon_ammunition_object, 12);

// max count: MAXIMUM_NUMBER_OF_MAGAZINES_PER_WEAPON
struct weapon_magazine_definition
{
    e_magazine_definition_flags flags;
    int16 rounds_recharged_per_second;
    int16 rounds_total_initial;
    int16 rounds_total_maximum;
    int16 rounds_loaded_maximum;
    int16 rounds_inventory_maximum;
    int16 pad;
    real32 reload_time_seconds;     // the length of time it takes to load a single magazine into the weapon
    int16 rounds_reloaded;
    int16 pad1;
    real32 chamber_time_seconds;    // the length of time it takes to chamber the next round
    int8 pad2[8];
    int8 pad3[16];

    tag_reference reloading_effect;
    tag_reference reloading_damage_effect;    // jpt!
    tag_reference chambering_effect;
    tag_reference chambering_damage_effect;   // jpt!
    tag_block<weapon_ammunition_object> magazines;
};
TAG_BLOCK_SIZE_ASSERT(weapon_magazine_definition, 92);

// max count: k_weapon_trigger_count
struct weapon_trigger_definition
{
    // the one place bungie decides to use member datatypes...
    enum e_autofire_action : int16
    {
        _autofire_action_fire = 0,
        _autofire_action_charge = 1,
        _autofire_action_track = 2,
        _autofire_action_fire_other = 3
    };
    struct s_autofire_fields
    {
        // Explaination("AUTOFIRE", "")
        real32 autofire_time;
        real32 autofire_throw;
        e_autofire_action secondary_action;
        e_autofire_action primary_action;
    };
    CHECK_STRUCT_SIZE(s_autofire_fields, 12);

    enum e_charging_overcharged_action : int16
    {
        _charging_overcharged_action_none = 0,
        _charging_overcharged_action_explode = 1,
        _charging_overcharged_action_discharge = 2
    };
    struct s_charging_fields
    {
        // Explaination("CHARGING", "")
        
        real32 charging_time_seconds;   // the amount of time it takes for this trigger to become fully charged
        real32 charged_time_seconds;    // the amount of time this trigger can be charged before becoming overcharged
        e_charging_overcharged_action overcharged_action;
        int16 pad;
        real32 charged_illumination;    // the amount of illumination given off when the weapon is fully charged
        real32 spew_time_seconds;       // length of time the weapon will spew (fire continuously) while discharging
        tag_reference charging_effect;  // the charging effect is created once when the trigger begins to charge
        // the charging effect is created once when the trigger begins to charge
        tag_reference charging_damage_effect; // jpt!
    };
    CHECK_STRUCT_SIZE(s_charging_fields, 36);

    e_weapon_trigger_definition_flags flags;
    e_weapon_trigger_definition_input input;
    e_weapon_trigger_definition_behavior behavior;
    
    int16 primary_barrel;   // index: weapon_barrels
    int16 secondary_barrel; // index: weapon_barrels

    e_weapon_trigger_definition_prediction prediction;
    int16 pad;
    s_autofire_fields autofire;
    s_charging_fields charging;
};
TAG_BLOCK_SIZE_ASSERT(weapon_trigger_definition, 64);

// max count: k_weapon_barrel_effect_count
struct barrel_firing_effect
{
    int16 shot_count_lower_bound;   // the minimum number of times this firing effect will be used, once it has been chosen
    int16 shot_count_upper_bound;   // the maximum number of times this firing effect will be used, once it has been chosen
    tag_reference firing_effect;    // this effect is used when the weapon is loaded and fired normally
    tag_reference misfire_effect;   // this effect is used when the weapon is loaded but fired while overheated
    tag_reference empty_effect;     // this effect is used when the weapon is not loaded
    // jpt!
    tag_reference firing_damage;    // this effect is used when the weapon is loaded and fired normally
    // jpt!
    tag_reference misfire_damage;   // this effect is used when the weapon is loaded but fired while overheated
    // jpt!
    tag_reference empty_damage;     // this effect is used when the weapon is not loaded
};
TAG_BLOCK_SIZE_ASSERT(barrel_firing_effect, 52);

// max count: k_weapon_barrel_count
struct weapon_barrel_definition
{
    e_weapon_barrel_definition_flags flags;
    // Explaination("firing", "")
    
    real_bounds rounds_per_second;      // the number of firing effects created per second
    real32 acceleration_time_seconds;   // the continuous firing time it takes for the weapon to achieve its final rounds per second
    real32 deceleration_time_seconds;   // the continuous idle time it takes for the weapon to return from its final rounds per second to its initial
    real32 barrel_spin_scale;           // scale the barrel spin speed by this amount
    real32 blurred_rate_of_fire;        // a percentage between 0 and 1 which controls how soon in its firing animation the weapon blurs
    short_bounds shots_per_fire;        // allows designer caps to the shots you can fire from one firing action
    real32 fire_recovery_time_seconds;  // how long after a set of shots it takes before the barrel can fire again
    real32 soft_recovery_fraction;      // how much of the recovery allows shots to be queued
    int16 magazine_index;               // the magazine from which this trigger draws its ammunition
    int16 rounds_per_shot;              // the number of rounds expended to create a single firing effect
    int16 minimum_rounds_loaded;        // the minimum number of rounds necessary to fire the weapon
    int16 rounds_between_tracers;       // the number of non-tracer rounds fired between tracers
    string_id optional_barrel_marker_name;

    // Explaination("prediction properties", "what the behavior of this barrel is in a predicted network game")
    
    e_barrel_prediction_type prediction_type;
    e_barrel_firing_noise firing_noise;         // how loud this weapon appears to the AI

    // Explaination("error", "")
    
    real32 error_acceleration_time_seconds;   // the continuous firing time it takes for the weapon to achieve its final error
    real32 error_deceleration_time_seconds;   // the continuous idle time it takes for the weapon to return to its initial error
    real_bounds damage_error;           // the range of angles (in degrees) that a damaged weapon will skew fire

    // Explaination("dual weapon error", "")
    
    real32 dual_weapon_error_acceleration_time_seconds;   // the continuous firing time it takes for the weapon to achieve its final error
    real32 dual_weapon_error_deceleration_time_seconds;   // the continuous idle time it takes for the weapon to return to its initial error
    real32 runtime_dual_weapon_error_acceleration_rate;
    real32 runtime_dual_weapon_error_deceleration_rate;
    real32 dual_weapon_error_minimum_error_degrees;
    real_bounds dual_weapon_error_angle_degrees;
    real32 dual_wield_damage_scale;

    // Explaination("projectile", "")

    e_barrel_distribution_function distribution_function;
    int16 projectiles_per_shot;
    real_angle distribution_angle_degrees;
    real_angle projectile_minimum_error_degrees;
    real_bounds projectile_error_angle_degrees;
    real_point3d first_person_offset_world_units;   // +x is forward, +z is up, +y is left
    e_damage_reporting_type damage_effect_reporting_type;
    int8 pad1[3];
    tag_reference projectile;       // proj
    tag_reference damage_effect;    // jpt!

    // Explaination("misc", "")

    real32 ejection_port_recovery_time; // the amount of time (in seconds) it takes for the ejection port to transition from 1.0 (open) to 0.0 (closed) after a shot has been fired
    real32 illumination_recovery_time;  // the amount of time (in seconds) it takes the illumination function to transition from 1.0 (bright) to 0.0 (dark) after a shot has been fired
    real32 heat_generated_per_round;    // the amount of heat generated each time the trigger is fired
    real32 age_generated_per_round;     // the amount the weapon ages each time the trigger is fired
    real32 overload_time_seconds;       // the next trigger fires this often while holding down this trigger
    
    // Explaination("angle change (recoil)", "")

    real_bounds angle_change_per_shot;  // angle change per shot of the weapon during firing
    real32 angle_change_acceleration_time_seconds;   // the continuous firing time it takes for the weapon to achieve its final angle change per shot
    real32 angle_change_deceleration_time_seconds;   // the continuous idle time it takes for the weapon to return to its initial angle change per shot

    /// function used to scale between initial and final angle change per shot
    e_transition_function_type angle_change_function;
    int16 pad;
    real32 runtime_angle_change_acceleration_rate;
    real32 runtime_angle_change_deceleration_rate;
    real32 runtime_illumination_recovery_rate;
    real32 runtime_ejection_port_recovery_rate;
    real32 runtime_rate_of_fire_acceleration_rate;
    real32 runtime_rate_of_fire_deceleration_rate;
    real32 runtime_error_acceleration_rate;
    real32 runtime_error_deceleration_rate;
    tag_block<barrel_firing_effect> firing_effects; // firing effects determine what happens when this trigger is fired
};
TAG_BLOCK_SIZE_ASSERT(weapon_barrel_definition, 236);

struct _weapon_definition
{
    _item_definition item;

    /* Explaination("$$$ WEAPON $$$", "All weapons should have 'primary trigger' and 'secondary trigger' markers as appropriate.
    Blurred permutations are called '$primary-blur' and '$secondary-blur'.")*/

    e_weapon_definition_flags flags;
    string_id unk_string_id;
    e_secondary_trigger_mode secondary_trigger_mode;
    int16 maximum_alternate_shots_loaded;   // if the second trigger loads alternate ammunition, this is the maximum number of shots that can be loaded at a time
    real32 turn_on_time;                    // how long after being readied it takes this weapon to switch its 'turned_on' attachment to 1.0


    // Explaination("old obsolete export to functions", "")
    real32 readyTimeSeconds;
    tag_reference ready_effect;         //
    tag_reference ready_damage_effect;  // jpt!

    // Explaination("heat", "")

    real32 heat_recovery_threshold;     // the heat value a weapon must return to before leaving the overheated state, once it has become overheated in the first place
    real32 overheated_threshold;        // the heat value over which a weapon first becomes overheated (should be greater than the heat recovery threshold)
    real32 heat_detonation_threshold;   // the heat value above which the weapon has a chance of exploding each time it is fired
    real32 heat_detonation_fraction;    // the percent chance (between 0.0 and 1.0) the weapon will explode when fired over the heat detonation threshold
    real32 heat_loss_per_second;        // the amount of heat lost each second when the weapon is not being fired
    real32 heat_illumination;           // the amount of illumination given off when the weapon is overheated
    real32 overheated_heat_loss_per_second; // the amount of heat lost each second when the weapon is not being fired
    tag_reference overheated;
    tag_reference overheated_damage_effect; // jpt!
    tag_reference detonation;
    tag_reference detonation_damage_effect; // jpt!
    tag_reference player_melee_damage;      // jpt!
    tag_reference player_melee_response;    // jpt!
    /* Explaination("melee aim assist", "magnetism angle: the maximum angle that magnetism works at full strength
        magnetism range : the maximum distance that magnetism works at full strength
        throttle magnitude : additional throttle to apply towards target when melee - ing w / magnetism
        throttle minimum distance : minimum distance to target that throttle magnetism kicks in
        throttle maximum adjustment angle : maximum angle throttle magnetism will have an effect, relative to the players movement throttle
        ")*/
    melee_aim_assist_parameters melee_aim_assist;

    /* Explaination("melee damage parameters", 
    "damage pyramid angles: defines the frustum from the camera that the melee-attack uses to find targets
    damage pyramid depth : how far the melee attack searches for a target")*/
    s_melee_damage_parameters melee_damage_parameters;

    e_damage_reporting_type melee_damage_reporting_type;
    int8 pad;

    // Explaination("zoom", "")

    int16 magnification_levels;    // the number of magnification levels this weapon allows

    real_bounds magnification_range;
    // Explaination("weapon aim assist", "")
    aim_assist_parameters weapon_aim_assist;

    // Explaination("movement", "")

    e_movement_penalized movement_penalized;
    int16 pad1;
    real32 forward_movement_penalty;    // percent slowdown to forward movement for units carrying this weapon
    real32 sideways_movement_penalty;   // percent slowdown to sideways and backward movement for units carrying this weapon
    
    // Explaination("AI targeting parameters", "")
    real32 ai_scariness;
    
    // Explaination("miscellaneous", "")

    real32 weapon_poweron_time_seconds;
    real32 weapon_poweroff_time_seconds;
    tag_reference weapon_poweron_effect;
    tag_reference weapon_poweroff_effect;
    real32 age_heat_recovery_penalty;   // how much the weapon's heat recovery is penalized as it ages

    
    real32 age_rate_of_fire_penalty;    // how much the weapon's rate of fire is penalized as it ages
    real32 age_misfire_start;           // the age threshold when the weapon begins to misfire
    real32 age_misfire_chance;          // at age 1.0, the misfire chance per shot
    tag_reference pickup_sound;         // snd!
    tag_reference zoomin_sound;         // snd!
    tag_reference zoomout_sound;        // snd!
    real32 active_camo_ding;            // how much to decrease active camo when a round is fired
    real32 active_camo_regrowth_rate;   // how fast to increase active camo (per tick) when a round is fired
    string_id handle_node;              // the node that get's attached to the unit's hand
    
    // Explaination("weapon labels", "")

    string_id weapon_class;
    string_id weapon_name;

    e_multiplayer_weapon_type multiplayer_weapon_type;

    // Explaination("more miscellaneous", "")

    e_weapon_type weapon_type;
    weapon_tracking_struct_block tracking;
    weapon_interface_definition_new player_interface;

    tag_block<predicted_resource> predicted_resources;
    tag_block<weapon_magazine_definition> magazines;
    tag_block<weapon_trigger_definition> new_triggers;
    tag_block<weapon_barrel_definition> barrels;

    real32 weapon_power_on_velocity;
    real32 weapon_power_off_velocity;

    // Explaination("first-person movement control", "")
    real32 max_movement_acceleration;
    real32 max_movement_velocity;
    real32 max_turning_acceleration;
    real32 max_turning_velocity;
    tag_reference deployed_vehicle; // vehi
    tag_reference age_effect;       // effe
    tag_reference aged_weapon;      // weap
    real_vector3d first_person_weapon_offset;
    real_vector2d first_person_scope_size;
};
TAG_GROUP_SIZE_ASSERT(_weapon_definition, 796);

weapon_first_person_interface_definition* first_person_interface_definition_get(const _weapon_definition* definition, e_character_type character_type);

void weapon_definitions_apply_patches(void);
