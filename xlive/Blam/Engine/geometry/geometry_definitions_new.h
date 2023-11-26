#pragma once
#include "Blam/Engine/math/real_math.h"
#include "Blam/Engine/rasterizer/rasterizer_vertex_buffers.h"

#define MAXIMUM_PARTS_PER_GEOMETRY_SECTION 255
#define MAXIMUM_SUBPARTS_PER_SECTION 32768
#define MAXIMUM_VERTICES_PER_GEOMETRY_SECTION 32767
#define MAXIMUM_STRIP_INDICES_PER_GEOMETRY_SECTION 65535
#define MAXIMUM_RIGID_POINT_GROUPS_PER_GEOMETRY_SECTION 32767
#define MAXIMUM_MATERIALS_PER_GEOMETRY 1024
#define MAXIMUM_PROPERTIES_PER_MATERIAL 16

enum e_geometry_classification : int16
{
    _geometry_classification_worldspace = 0,
    _geometry_classification_rigid = 1,
    _geometry_classification_rigid_boned = 2,
    _geometry_classification_skinned = 3,
    _geometry_classification_unsupported_reimport = 4
};

enum e_geometry_compression_flags : int16
{
    _geometry_compression_flag_compressed_position = FLAG(0),
    _geometry_compression_flag_compressed_texcoord = FLAG(1),
    _geometry_compression_flag_compressed_secondary_texcoord = FLAG(2)
};

enum e_section_lighting_flags : int16
{
    _section_lighting_has_lm_texcoords = FLAG(0),
    _section_lighting_has_lm_inc_rad = FLAG(1),
    _section_lighting_has_lm_colors = FLAG(2),
    _section_lighting_has_lm_prt = FLAG(3)
};

enum e_geometry_part_type : int16
{
    _geometry_part_type_not_drawn = 0,
    _geometry_part_type_opaque_shadow_only = 1,
    _geometry_part_type_opaque_shadow_casting = 2,
    _geometry_part_type_opaque_nonshadowing = 3,
    _geometry_part_type_transparent = 4,
    _geometry_part_type_lightmap_only = 5
};

enum e_geometry_part_flags : int16
{
    _geometry_part_flag_decalable = FLAG(0),
    _geometry_part_flag_new_part_types = FLAG(1),
    _geometry_part_flag_dislikes_photons = FLAG(2),
    _geometry_part_flag_override_triangle_list = FLAG(3),
    _geometry_part_flag_ignored_by_lightmapper = FLAG(4)
};

enum e_geometry_material_property_type : int16
{
    _geometry_material_property_type_lightmap_resolution = 0,
    _geometry_material_property_type_lightmap_power = 1,
    _geometry_material_property_type_lightmap_half_life = 2,
    _geometry_material_property_type_lightmap_diffuse_scale = 3
};

// max count: 1
struct geometry_compression_info
{
    real_rectangle3d position_bounds;
    real_rectangle2d texcoord_bounds;
    real_rectangle2d secondary_texcoord_bounds;
};
CHECK_STRUCT_SIZE(geometry_compression_info, 56);

struct geometry_section_info
{
    // Explaination("SECTION INFO", "EMPTY STRING")
    
    uint16 total_vertex_count;
    uint16 total_triangle_count;
    uint16 total_part_count;
    uint16 shadow_casting_triangle_count;
    uint16 shadow_casting_part_count;
    uint16 opaque_point_count;
    uint16 opaque_vertex_count;
    uint16 opaque_part_count;
    uint8 opaque_max_nodes_vertex;
    uint8 transparent_max_nodes_vertex;
    uint16 shadow_casting_rigid_triangle_count;

    e_geometry_classification geometry_classification;
    e_geometry_compression_flags geometry_compression_flags;
    tag_block<geometry_compression_info> compression_info;

    uint8 hardware_node_count;
    uint8 node_map_size;
    uint16 software_plane_count;
    uint16 total_subpart_count;

    e_section_lighting_flags section_lighting_flags;
};
CHECK_STRUCT_SIZE(geometry_section_info, 40);

// max count: MAXIMUM_PARTS_PER_GEOMETRY_SECTION
struct geometry_part
{
    e_geometry_part_type type;
    e_geometry_part_flags flags;

    uint16 material; // Block index: global_geometry_material_block
    uint16 strip_start_index;
    uint16 strip_length;
    uint16 first_subpart_index;
    uint16 subpart_count;
    uint8 max_nodes_vertex;
    uint8 contributing_compound_node_count;

    // Explaination("CENTROID", "EMPTY STRING")
    real_point3d position;

    int8 node_indices[4];
    real32 node_weights[3];
    real32 lod_mipmap_magic_number;
    int8 pad[24];
};
TAG_BLOCK_SIZE_ASSERT(geometry_part, 72);

// max count: MAXIMUM_SUBPARTS_PER_SECTION
struct geometry_subpart
{
    uint16 indices_start_index;
    uint16 indices_length;
    uint16 visibility_bounds_index;
    uint16 part_index;
};
TAG_BLOCK_SIZE_ASSERT(geometry_subpart, 8);

// max count: MAXIMUM_SUBPARTS_PER_SECTION
struct geometry_visibility
{
    real_point3d position;
    real32 radius;
    int8 node;
    int8 pad[3];
};
TAG_BLOCK_SIZE_ASSERT(geometry_visibility, 20);

// max count: MAXIMUM_VERTICES_PER_GEOMETRY_SECTION
struct geometry_point
{
    real_point3d position;
    int32 node_indices_old[4];
    real32 node_weights[4];
    int32 node_index_new[4];
    int32 use_new_node_indices;
    int32 adjusted_compound_node_index;
};
CHECK_STRUCT_SIZE(geometry_point, 68);

// max count: MAXIMUM_VERTICES_PER_GEOMETRY_SECTION
struct geometry_vertex
{
    geometry_point point;
    real_point2d texcoord;
    real_vector3d normal;
    real_vector3d binormal;
    real_vector3d tangent;
    real_vector3d anisotropic_binormal;
    real_point2d secondary_texcoord;
    real_rgb_color primary_lightmap_color;
    real_point2d primary_lightmap_texcoord;
    real_vector3d primary_lightmap_incident_direction;
    real_vector3d unk_0;
    real_point2d unk_1;
    real_point3d unk_2;
};
TAG_BLOCK_SIZE_ASSERT(geometry_vertex, 196);

struct geometry_section
{
    tag_block<geometry_part> parts;
    tag_block<geometry_subpart> subparts;
    tag_block<geometry_visibility> visibility_bounds;
    tag_block<geometry_vertex> raw_vertices;
    tag_block<uint16> strip_indices;    // max count: MAXIMUM_STRIP_INDICES_PER_GEOMETRY_SECTION

    data_block visibility_mopp_code;  // Data Size: 393216
    tag_block<uint16> mopp_reorder_table;
    tag_block<rasterizer_vertex_buffer> vertex_buffers;
    int32 pad;
};
CHECK_STRUCT_SIZE(geometry_section, 68);

// max count: MAXIMUM_RIGID_POINT_GROUPS_PER_GEOMETRY_SECTION
struct geometry_rigid_point_group
{
    uint8 rigid_node_index;
    uint8 nodes_point;
    uint16 point_count;
};
TAG_BLOCK_SIZE_ASSERT(geometry_rigid_point_group, 4);

struct geometry_point_data
{
    tag_block<geometry_point> raw_points;
    data_block runtime_point_data;    // Data size: 1048544
    tag_block<geometry_rigid_point_group> rigidPointGroups;
    tag_block<uint16> vertex_point_indices;
};
CHECK_STRUCT_SIZE(geometry_point_data, 32);

// max count: MAXIMUM_PROPERTIES_PER_MATERIAL
struct geometry_material_property
{
    e_geometry_material_property_type type;
    int16 int_value;
    real32 real_value;
};
TAG_BLOCK_SIZE_ASSERT(geometry_material_property, 8);

// max count: MAXIMUM_MATERIALS_PER_GEOMETRY
struct geometry_material
{
    tag_reference old_shader;   // shad
    tag_reference shader;       // shad
    tag_block<geometry_material_property> properties;

    int32 pad;
    int8 breakable_surface_index;
    int8 pad1[3];
};
TAG_BLOCK_SIZE_ASSERT(geometry_material, 32);
