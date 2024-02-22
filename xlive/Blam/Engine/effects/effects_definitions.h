#pragma once
#include "effects_creation.h"
#include "effect_location_definition.h"
#include "tag_files/tag_block.h"
#include "tag_files/tag_reference.h"
#include "Blam/Engine/math/function_definitions.h"

#include "particle_system_definition.h"

#define k_maximum_effect_parts_per_event_count 32
#define k_maximum_effect_events_count 32

#define k_maximum_effect_accelerations_per_event_count 32

enum e_effect_part_scale_value : int32
{
    _effect_part_scale_velocity = FLAG(0),
    _effect_part_scale_velocity_delta = FLAG(1),
    _effect_part_scale_velocity_cone_angle = FLAG(2),
    _effect_part_scale_angular_velocity = FLAG(3),
    _effect_part_scale_angular_velocity_delta = FLAG(4),
    _effect_part_scale_type_specific_scale = FLAG(5)
};

enum e_effect_part_flags : int16
{
    _effect_event_part_face_down_regardless_of_lcation_decals = FLAG(0),
    _effect_event_part_offset_origin_away_from_geometry_lights = FLAG(1),
    _effect_event_part_never_attached_to_object = FLAG(2),
    _effect_event_part_disabled_for_debugging = FLAG(3),
    _effect_event_part_draw_regardless_of_distance = FLAG(4)
};

enum e_effect_event_flag : int32
{
    _disabled_for_debugging = FLAG(0)
};

enum e_effect_flags : int32
{
    _deleted_when_attachment_deactivates = FLAG(0)
};

// max count: k_maximum_effect_accelerations_per_event_count 32
struct effect_acceleration_definition
{
    e_effect_environment_create_in create_in_environment;
    e_effect_violence_create_in create_in_violence;

    //Block reference effect_locations_block
    int16 location;
    int16 pad;
    real32 acceleration;
    real_angle inner_cone_angle_degrees;
    real_angle outer_cone_angle_degrees;
};
TAG_BLOCK_SIZE_ASSERT(effect_acceleration_definition, 20);

struct c_beam_definition
{
    tag_reference shader; //shad
    //Block reference effect_locations_block
    int16 location;
    int16 pad;
    // Explaination("COLOR", "tint color of the beam")
    c_function_definition color;

    // Explaination("ALPHA", "how much to fade out beam")
    c_function_definition alpha;

    // Explaination("WIDTH", "how wide in world units")
    c_function_definition width;

    // Explaination("LENGTH", "how long in world units")
    c_function_definition length;

    // Explaination("YAW", "rotate the marker")
    c_function_definition yaw;

    // Explaination("PITCH", "rotate the marker")
    c_function_definition pitch;
};
TAG_BLOCK_SIZE_ASSERT(c_beam_definition, 60);

// max count: k_maximum_effect_parts_per_event_count 32
struct effect_part_definition
{
    e_effect_environment_create_in create_in_environment;
    e_effect_violence_create_in create_in_violence;
    // effect.locations index
    int16 location;
    e_effect_part_flags flags;
    tag_group runtime_base_tag_group;
    tag_reference base_type;
    // initial velocity along the location's forward, for decals the distance at which decal is created (defaults to 0.5)
    real_bounds velocity_bounds;
    real_angle velocity_cone_angle;
    real_bounds angular_velocity_bounds;
    real_bounds radius_modifier_bounds;
    e_effect_part_scale_value a_scales_value;
    e_effect_part_scale_value b_scales_value;
};

// max count: k_maximum_effect_events_count 32
struct effect_event_definition
{
    e_effect_event_flag flags;
    real32 skip_fraction;
    real_bounds delay_bounds;
    real_bounds duration_bounds;
    tag_block<effect_part_definition> parts;
    tag_block<c_beam_definition> beams;
    tag_block<effect_acceleration_definition> accelerations;
    tag_block<c_particle_system_definition> particle_systems;
};
TAG_BLOCK_SIZE_ASSERT(effect_event_definition, 0x38);

struct effect_definition
{
    e_effect_flags flags;
    int16 loop_start_event;
    int16 unk_6;
    real32 unk_runtime_float;
    tag_block<effect_location_definition> locations;
    tag_block<effect_event_definition> events;

    // Explaination("Looping Sound", "")
    tag_reference looping_sound; // lsnd
    int16 location;
    int16 pad_1;
    real32 always_play_distance;
    real32 never_play_distance;
};
TAG_GROUP_SIZE_ASSERT(effect_definition, 0x30);
