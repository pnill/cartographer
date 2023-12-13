#pragma once
#include "effect_location_definition.h"
#include "particle_property.h"
#include "particle_system_definition.h"
#include "Blam/Cache/DataTypes/TagRef.h"
#include "Blam/Engine/geometry/geometry_block.h"
#include "Blam/Engine/math/real_math.h"
#include "Blam/Engine/rasterizer/rasterizer_vertex_buffers.h"



enum e_particle_model_definition_flags : int32
{
    _particle_model_spins = FLAG(0),
    _particle_model_random_u_mirror = FLAG(1),
    _particle_model_random_v_mirror = FLAG(2),
    _particle_model_frame_animation_one_shot = FLAG(3),
    _particle_model_select_random_sequence = FLAG(4),
    _particle_model_disable_frame_blending = FLAG(5),
    _particle_model_can_animate_backwards = FLAG(6),
    _particle_model_receive_lightmap_lighting = FLAG(7),
    _particle_model_tint_from_diffuse_texture = FLAG(8),
    _particle_model_dies_at_rest = FLAG(9),
    _particle_model_dies_on_structure_collision = FLAG(10),
    _particle_model_dies_in_media = FLAG(11),
    _particle_model_dies_in_air = FLAG(12),
    _particle_model_bitmap_authored_vertically = FLAG(13),
    _particle_model_has_sweetener = FLAG(14)
};

enum e_particle_model_orientation : int32
{
    _particle_model_screen_facing,
    _particle_model_parallel_to_direction,
    _particle_model_perpendicular_to_direction,
    _particle_model_vertical,
    _particle_model_horizontal
};

struct s_particle_model_vertex
{
    real_point3d position;
    real_vector3d normal;
    real_vector3d tangent;
    real_vector3d binormal;
    real_point2d texcoord;
};

class c_particle_model
{
public:
    string_id model_name;
    int16 index_start;
    int16 index_count;
};

class c_particle_model_definition
{
public:
    e_particle_model_definition_flags flags;
    e_particle_model_orientation orientation;
    int8 pad[16];
    tag_reference shader; //shad

    // Explaination("SCALE X", "scale of model on x axis")
    c_particle_property scale_x;
    // Explaination("SCALE Y", "scale of model on y axis")
    c_particle_property scale_y;
    // Explaination("SCALE Z", "scale of model on z axis")
    c_particle_property scale_z;
    // Explaination("ROTATION", "rotation where 0=0 degrees, 0.5=180 degrees, 1.0=360 degrees")
    c_particle_property rotation;

    // Explaination("Spawned Effects", "collision occurs when particle physics has collision, death spawned when particle dies")
    tag_reference collision_effect;

    tag_reference death_effect;

    // Explaination("Attached Particle Systems", "Locations:
    // up - particles shoot straight up
    // gravity - particles shoot straight down
    // NONE - opposite of particle direction")
    tag_block<effect_location_definition> locations;

    tag_block<c_particle_system_definition> attached_particle_systems;
    tag_block<c_particle_model> models;
    tag_block<s_particle_model_vertex> raw_vertices;
    tag_block<int16> indices;
    tag_block<rasterizer_vertex_buffer> cached_data;
    geometry_block_info geometry_section_info;
    int8 pad_2[28];
};

class c_particle_model_definition_interface
{
public:
    int32 vtable;
    datum tag_index;
    c_particle_model_definition particle_model_definition;
    c_particle_system_definition* get_attached_particle_system(int32 particle_system_index);
};