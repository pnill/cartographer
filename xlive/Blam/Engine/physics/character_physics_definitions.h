#pragma once

#include "physics/physics_models.h"

#define k_character_physics_dead_size 0
#define k_character_physics_sentinel_size 0

enum e_character_physics_definition_flags : uint32
{
    _character_physics_definition_centered_at_origin = FLAG(0),
    _character_physics_definition_shape_spherical = FLAG(1),
    _character_physics_definition_use_player_physics = FLAG(2),
    _character_physics_definition_climb_any_surface = FLAG(3),
    _character_physics_definition_flying = FLAG(4),
    _character_physics_definition_not_physical = FLAG(5),
    _character_physics_definition_dead_character_collision_group = FLAG(6)
};

struct s_character_physics_ground_definition
{
    real_angle maximum_slope_angle;
    real_angle downhill_falloff_angle;
    real_angle downhill_cutoff_angle;
    real_angle uphill_falloff_angle;
    real_angle uphill_cutoff_angle;
    real32 downhill_velocity_scale;
    real32 uphill_velocity_scale;

    // Runtime values
    real32 cosine_maximum_slope_angle;
    real32 negative_sine_downhill_falloff_angle;
    real32 negative_sine_downhill_cutoff_angle;
    real32 sine_uphill_falloff_angle;
    real32 sine_uphill_cutoff_angle;
};
CHECK_STRUCT_SIZE(s_character_physics_ground_definition, 48);

struct s_character_physics_flying_definition
{
    real_angle bank_angle;              // angle at which we bank left/right when sidestepping or turning while moving forwards
    real32 bank_apply_time_seconds;     // time it takes us to apply a bank
    real32 bank_decay_time_seconds;     // time it takes us to recover from a bank
    real32 pitch_ratio;                 // amount that we pitch up/down when moving up or down
    real32 max_velocity;                // (World Units Per Second) max velocity when not crouching

    // max sideways or up/down velocity when not crouching
    real32 max_sidestep_velocity;           // World Units Per Second
    real32 acceleration;                    // World Units Per Second Squared
    real32 deceleration;                    // World Units Per Second Squared
    real32 angular_velocity_maximum;        // (Degrees Per Second) turn rate
    real32 angular_acceleration_maximum;    // (Degrees Per Second Squared) turn acceleration rate
    real32 crouch_velocity_modifier;        // how much slower we fly if crouching (zero = same speed)
};
CHECK_STRUCT_SIZE(s_character_physics_flying_definition, 44);

struct s_character_physics_dead_definition
{
};

struct s_character_physics_sentinel_definition
{
};

struct s_character_physics_definition
{
    e_character_physics_definition_flags flags;
    real32 height_standing;
    real32 height_crouching;
    real32 radius;
    real32 mass;

    string_id living_material_name; // collision material used when character is alive
    string_id dead_material_name;   // collision material used when character is dead
    int16 living_global_material_index;
    int16 dead_global_material_index;

    tag_block<s_physics_model_sphere> dead_sphere_shapes;
    tag_block<s_physics_model_pill> pill_shapes;
    tag_block<s_physics_model_sphere> sphere_shapes;

    // Explaination("ground", "")
    s_character_physics_ground_definition ground_physics;
    
    // Explaination("flying", "")
    s_character_physics_flying_definition flying_physics;
    
    // Explaination("dead", "")
    //s_character_physics_dead_definition dead_physics;
    
    // Explaination("sentinel", "")
    //s_character_physics_sentinel_definition sentinel_physics;
};
CHECK_STRUCT_SIZE(s_character_physics_definition, 148);