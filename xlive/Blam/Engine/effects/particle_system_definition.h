#pragma once

#include "effect_location_definition.h"
#include "particle_property.h"
#include "Blam/Cache/DataTypes/TagBlock.h"
#include "Blam/Cache/DataTypes/TagRef.h"
#include "Blam/Engine/shaders/shader_definitions.h"
#include "Blam/Engine/shaders/shader_postprocess_definitions.h"

#define k_maximum_emitters_per_definition 8
#define k_maximum_particle_systems_per_block 32

enum e_particle_billboard_style : int16
{
    _particle_billboard_screen_facing,
    _particle_billboard_parallel_to_direction,
    _particle_billboard_perpendicular_to_direction,
    _particle_billboard_vertical,
    _particle_billboard_horizontal
};

enum e_particle_definition_flags : int32
{
    _particle_spins = FLAG(0),
    _particle_random_u_mirror = FLAG(1),
    _particle_random_v_mirror = FLAG(2),
    _particle_frame_animation_one_shot = FLAG(3),
    _particle_select_random_sequence = FLAG(4),
    _particle_disable_frame_blending = FLAG(5),
    _particle_can_animate_backwards = FLAG(6),
    _particle_receive_lightmap_lighting = FLAG(7),
    _particle_tint_from_diffuse_texture = FLAG(8),
    _particle_dies_at_rest = FLAG(9),
    _particle_dies_on_structure_collision = FLAG(10),
    _particle_dies_in_media = FLAG(11),
    _particle_dies_in_air = FLAG(12),
    _particle_bitmap_authored_vertically = FLAG(13),
    _particle_has_sweetener = FLAG(14)
};

enum e_particle_coordinate_system : int16
{
    _particle_coordinate_system_world = 0,
    _particle_coordinate_system_local = 1,
    _particle_coordinate_system_parent = 2
};

enum e_particle_system_emitter_emission_shape : int32
{
    _particle_system_emitter_shape_sprayer = 0,
    _particle_system_emitter_shape_disc = 1,
    _particle_system_emitter_shape_globe = 2,
    _particle_system_emitter_shape_implode = 3,
    _particle_system_emitter_shape_tube = 4,
    _particle_system_emitter_shape_halo = 5,
    _particle_system_emitter_shape_impact_contour = 6,
    _particle_system_emitter_shape_impact_area = 7,
    _particle_system_emitter_shape_debris = 8,
    _particle_system_emitter_shape_line = 9
};

// max count: k_maximum_emitters_per_definition 8
class c_particle_emitter_definition
{
    tag_reference particle_physics; // pmov

    // Explaination("particle emission rate (particles/tick)", "")
    c_particle_property particle_emmision_rate;
    // Explaination("particle lifespan(seconds)", "")
    c_particle_property particle_life_span;
    // Explaination("particle velocity(world units/second)", "")
    c_particle_property particle_velocity;
    // Explaination("particle angular velocity(degrees/second)", "")
    c_particle_property particle_angular_velocity;
    // Explaination("particle size(world units)", "")
    c_particle_property particle_size;
    // Explaination("particle tint", "")
    c_particle_property particle_tint;
    // Explaination("particle alpha", "")
    c_particle_property particle_alpha;

    // Explaination("EMISSION SETTINGS", "")

    e_particle_system_emitter_emission_shape emission_shape;
    // Explaination("emission radius(world units)", "")
    c_particle_property emission_radius;
    // Explaination("emission angle(degrees)", "")
    c_particle_property emission_angle;

    real_point3d translational_offset;
    // particle initial velocity direction relative to the location's forward
    real_euler_angles2d relative_direction;

    byte pad[8];
};
CHECK_STRUCT_SIZE(c_particle_emitter_definition, 0xB8);

// max count: k_maximum_particle_systems_per_block 32
class c_particle_system_definition
{
    tag_reference particle;
    //Block reference effect_locations_block
    int32 location;
    e_particle_coordinate_system coordinate_system;
    e_effect_environment_create_in create_in_environment;
    e_effect_violence_create_in create_in_violence;
    e_particle_system_camera_mode camera_mode;
    // use values between -10 and 10 to move closer and farther from camera (positive is closer)
    int16 sort_bias;
    e_particle_system_flags flags;
    //defaults to 0.0
    real32 lod_in_distance;
    //defaults to 0.0
    real32 lod_feather_in_delta;
    int32 pad;
    //defaults to 30.0
    real32 lod_out_distance;
    //defaults to 10.0
    real32 lod_feather_out_delta;
    int32 pad_2;
    tag_block<c_particle_emitter_definition> emitters;
};
CHECK_STRUCT_SIZE(c_particle_system_definition, 56);

class c_particle_definition
{
    e_particle_definition_flags flags;
    e_particle_billboard_style billboard_style;
    int8 pad[2];
    int16 first_sequence_index;
    int16 sequence_count;

    tag_reference shader_template; // stem
    tag_block<s_shader_parameter> shader_parameters;
    // Explaination("Color", "controls how the color of the particle changes as a function of its input")
    c_particle_property color;
    // Explaination("Alpha", "seperate from color, controls how the particle fades	as a function of its input")
    c_particle_property alpha;
    // Explaination("Scale", "controls how the size of a particle changes as a function of its input")
    c_particle_property scale;
    // Explaination("Rotation", "controls how the particle rotates. 0= 0 degrees, .5= 180 degrees, 1.0= 360 degrees")
    c_particle_property rotation;
    // Explaination("Frame index", "provides finer grain control over how the animation	happens.a output of 0.25 means that when that
    // input is fed in, the particle will be 25 % of the way through its animation.")
    c_particle_property frame_index;

    // Explaination("Spawned Effects", "collision occurs when particle physics has collision, death spawned when particle dies")
    tag_reference collision_effect; // effe, !snd, foot

    tag_reference death_effect; // effe, !snd, foot

    tag_block<effect_location_definition> locations;
    tag_block<c_particle_system_definition> attached_particle_systems;
    tag_block<s_shader_postprocess_definition_new> shader_post_processing;

    int8 pad_1[40];
};
TAG_GROUP_SIZE_ASSERT(c_particle_definition, 188);
