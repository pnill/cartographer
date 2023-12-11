#pragma once
#include "unit_definitions.h"

#include "Blam/Engine/physics/character_physics_definitions.h"

enum e_biped_definition_flags : uint32
{
    _biped_definition_turns_without_animating = FLAG(0),
    _biped_definition_passes_through_other_bipeds = FLAG(1),
    _biped_definition_immune_to_falling_damage = FLAG(2),
    _biped_definition_rotate_while_airborne = FLAG(3),
    _biped_definition_uses_limp_body_physics = FLAG(4),
    _biped_definition_unused = FLAG(5),
    _biped_definition_random_speed_increase = FLAG(6),
    _biped_definition_unused_1 = FLAG(7),
    _biped_definition_spawn_death_children_on_destroy = FLAG(8),
    _biped_definition_stunned_by_emp_damage = FLAG(9),
    _biped_definition_dead_physics_when_stunned = FLAG(10),
    _biped_definition_always_ragdoll_when_dead = FLAG(11)
};

enum e_biped_lock_on_data_flags : uint32
{
    _biped_lock_on_locked_by_human_targeting = FLAG(0),
    _biped_lock_on_locked_by_plasma_targeting = FLAG(1),
    _biped_lock_on_always_locked_by_plasma_targeting = FLAG(2)
};

struct s_biped_lock_on_data
{
    // Explaination("lock-on fields", "")

    e_biped_lock_on_data_flags flags;
    real32 lock_on_distance;
};
CHECK_STRUCT_SIZE(s_biped_lock_on_data, 8);

struct biped_contact_point
{
    string_id marker_name;
};
CHECK_STRUCT_SIZE(biped_contact_point, 4);

struct _biped_definition
{
    _unit_definition unit;

	// Explaination("$$$ BIPED $$$", "")
	
    real_angle moving_turning_speed; // Degrees Per Second
    e_biped_definition_flags flags;
    real32 stationary_turning_threshold;

    // Explaination("jumping and landing", "")

    real32 jump_velocity;   // World Units Per Second

    real32 maximum_soft_landing_time_seconds;   // the longest amount of time the biped can take to recover from a soft landing
    real32 maximum_hard_landing_time_seconds;   // the longest amount of time the biped can take to recover from a hard landing
    real32 minimum_soft_landing_velocity;       // (World Units Per Second) below this velocity the biped does not react when landing
    real32 minimum_hard_landing_velocity;       // (World Units Per Second) below this velocity the biped will not do a soft landing when returning to the ground
    real32 maximum_hard_landing_velocity;       // (World Units Per Second) the velocity corresponding to the maximum landing time
    real32 death_hard_landing_velocity;         // (World Units Per Second) the maximum velocity with which a character can strike the ground and live
    real32 stun_duration;                       // 0 is the default.  Bipeds are stuned when damaged by vehicle collisions, also some are when they take emp damage

    // Explaination("camera, collision, and autoaim", "")
    
    real32 standing_camera_height;              // World Units
    real32 crouching_camera_height;             // World Units
    real32 crouch_transition_time_seconds;
    real_angle camera_interpolation_start;      // looking-downward angle that starts camera interpolation to fp position
    real_angle camera_interpolation_end;        // looking-downward angle at which camera interpolation to fp position is complete
    
    real32 camera_forward_movement_scale;       // amount of fp camera movement forward and back (1.0 is full)
    real32 camera_side_movement_scale;          // amount of fp camera movement side-to-side (1.0 is full)
    real32 camera_vertical_movement_scale;      // amount of fp camera movement vertically (1.0 is full)
    real32 camera_exclusion_distance;           // (World Units) fp camera must always be at least this far out from root node
    real32 autoaim_width;                       // (World Units)

    s_biped_lock_on_data lockon_data;
    int16 physics_control_node_index;
    int16 pad;
    real32 cosine_stationary_turning_threshold;
    real32 crouch_transition_velocity;
    int16 pelvis_node_index;
    int16 head_node_index;

    real32 headshot_acceleration_scale;         // when the biped ragdolls from a head shot it acceleartes based on this value.  0 defaults to the standard acceleration scale
    tag_reference area_damage_effect;           // effe
    s_character_physics_definition physics;

    tag_block<biped_contact_point> contact_points;  // these are the points where the biped touches the ground

    // char
    tag_reference reanimation_character;        // when the flood reanimate this guy, he turns into a ...
    tag_reference death_spawn_character;        // when I die, out of the ashes of my death crawls a ...
    int16 death_spawn_count;
    int16 pad1;
};
TAG_GROUP_SIZE_ASSERT(_biped_definition, 788)