#pragma once
#include "instanced_geometry_definitions.h"
#include "leaf_map.h"
#include "structure_audibility.h"
#include "structure_lightmap_definitions.h"
#include "structure_runtime_decals.h"
#include "ai/path.h"
#include "cache/predicted_resources.h"
#include "decorators/decorator_definitions.h"
#include "game/game_portals.h"
#include "memory/static_arrays.h"
#include "physics/collision_bsp_definition.h"
#include "physics/structure_physics.h"
#include "render/render_debug_structure.h"
#include "render/render_water.h"
#include "sound/sound_clusters.h"
#include "tag_files/tag_import_definitions.h"

#define MAXIMUM_ENVIRONMENT_OBJECT_PALETTE_ENTRIES_PER_STRUCTURE 100
#define MAXIMUM_ENVIRONMENT_OBJECTS_PER_STRUCTURE 16384
#define MAXIMUM_MARKERS_PER_STRUCTURE 1024
#define MAXIMUM_BREAKABLE_SURFACES_PER_STRUCTURE 8448
#define MAXIMUM_SKIES_PER_SCENARIO 32
#define MAXIMUM_STRIP_INDICES_PER_GEOMETRY_SECTION 65535
#define k_maximum_instance_geometry_instances_per_cluster 1024
#define MAXIMUM_CLUSTER_PORTALS_PER_CLUSTER 512
#define MAXIMUM_CLUSTERS_PER_STRUCTURE 512
#define MAXIMUM_DETAIL_OBJECTS_PER_STRUCTURE 2097152
#define MAXIMUM_DETAIL_OBJECT_CELLS_PER_STRUCTURE 262144
#define MAXIMUM_DETAIL_OBJECT_LAYERS_PER_STRUCTURE 32
#define MAXIMUM_DETAIL_OBJECT_COUNT (MAXIMUM_DETAIL_OBJECT_LAYERS_PER_STRUCTURE * MAXIMUM_DETAIL_OBJECT_CELLS_PER_STRUCTURE)
#define MAXIMUM_PLANES_PER_WEATHER_POLYHEDRON 16
#define MAXIMUM_WEATHER_POLYHEDRA_PER_STRUCTURE 32
#define MAXIMUM_FOG_PLANES_PER_STRUCTURE 127
#define MAXIMUM_VERTICES_PER_CLUSTER_PORTAL 128
#define MAXIMUM_CLUSTER_PORTALS_PER_STRUCTURE 512
#define MAXIMUM_NODES_PER_BSP3D 131072
#define MAXIMUM_COLLISION_MATERIALS_PER_STRUCTURE 512

enum e_cluster_portal_flags : uint32
{
    _cluster_portal_ai_cannot_hear_through_this = FLAG(0),
    _cluster_portal_one_way = FLAG(1),
    _cluster_portal_door = FLAG(2),
    _cluster_portal_no_way = FLAG(3),
    _cluster_portal_one_way_reversed = FLAG(4),
    _cluster_portal_no_one_can_hear_through_this = FLAG(5)
};

enum e_structure_fog_plane_flags : uint16
{
    _structure_fog_plane_extend_infinitely_while_visible = FLAG(0),
    _structure_fog_plane_do_not_flood_fill = FLAG(1),
    _structure_fog_plane_aggressive_flood_fill = FLAG(2)
};

enum e_structure_cluster_flags : uint16
{
    _structure_cluster_one_way_portal = FLAG(0),
    _structure_cluster_door_portal = FLAG(1),
    _structure_cluster_post_processed_geometry = FLAG(2),
    _structure_cluster_is_the_sky = FLAG(3)
};

// max count: MAXIMUM_BSPS_PER_COLLISION_REGION
struct node_render_leaves
{
    tag_block<structure_leaf> collision_leaves;
    tag_block<structure_surface_reference> surface_references;
};
TAG_BLOCK_SIZE_ASSERT(node_render_leaves, 16);

// max: MAXIMUM_NODES_PER_BSP3D
struct unused_structure_node
{
    int8 pad_1[6];
};
TAG_BLOCK_SIZE_ASSERT(unused_structure_node, 6);

// max: MAXIMUM_COLLISION_MATERIALS_PER_STRUCTURE
struct structure_collision_material
{
    tag_reference old_shader; // shad
    int16 pad_1;
    // BlockIndex1("structure_bsp_conveyor_surface_block")
    uint16 conveyor_surface_index;
    tag_reference new_shader; // shad
};
TAG_BLOCK_SIZE_ASSERT(structure_collision_material, 20);

// max: MAXIMUM_CLUSTER_PORTALS_PER_STRUCTURE
struct cluster_portal
{
    uint16 back_cluster;
    uint16 front_cluster;
    uint32 plane_index;
    real_point3d centroid;
    real32 bounding_radius;
    e_cluster_portal_flags flags;
    // max: MAXIMUM_VERTICES_PER_CLUSTER_PORTAL
    tag_block<real_point3d> vertices;
};
TAG_BLOCK_SIZE_ASSERT(cluster_portal, 36);

// max: MAXIMUM_FOG_PLANES_PER_STRUCTURE
struct s_structure_fog_plane
{
    uint16 scenario_planar_for_index;
    int16 pad_1;
    real_plane3d plane;
    e_structure_fog_plane_flags flags;
    int16 priority;
};
TAG_BLOCK_SIZE_ASSERT(s_structure_fog_plane, 24);

// max: MAXIMUM_WEATHER_POLYHEDRA_PER_STRUCTURE
struct structure_weather_polyhedron
{
    real_point3d bounding_sphere_center;
    real32 bounding_sphere_radius;
    // max: MAXIMUM_PLANES_PER_WEATHER_POLYHEDRON
    tag_block<real_plane3d> planes;
};
TAG_BLOCK_SIZE_ASSERT(structure_weather_polyhedron, 24);

// max count: MAXIMUM_WEATHER_PALETTE_ENTRIES_PER_STRUCTURE 32
struct structure_weather_palette_entry
{
    static_string32 name;
    tag_reference weather_system;   // weat
    short pad[2];
    int pad1[8];
    tag_reference wind;             // wind
    real_vector3d wind_direction;
    float wind_magnitude;
    int pad2;
    static_string32 wind_scale_function;
};
TAG_BLOCK_SIZE_ASSERT(structure_weather_palette_entry, 136);

// max: MAXIMUM_DETAIL_OBJECT_COUNT
struct detail_object_count
{
    int16 unknown_1;
};
TAG_BLOCK_SIZE_ASSERT(detail_object_count, 2);

// max: MAXIMUM_DETAIL_OBJECTS_PER_STRUCTURE
struct detail_object
{
    uint32 unknown_1;
    uint16 unknown_2;
};
TAG_BLOCK_SIZE_ASSERT(detail_object, 8);

// max: MAXIMUM_DETAIL_OBJECT_CELLS_PER_STRUCTURE
struct detail_object_cell_definition
{
    uint16 unknown_1;
    uint16 unknown_2;
    uint16 unknown_3;
    uint16 unknown_4;
    uint32 unknown_5;
    uint32 unknown_6;
    uint32 unknown_7;
    int8 pad_1[12];
};
TAG_BLOCK_SIZE_ASSERT(detail_object_cell_definition, 32);

// max: 1
// names can be inferred from MCC 0x0103BBF at a future time for all child objects
// used to access dobc tag files
struct structure_detail_object_data
{
    tag_block<detail_object_cell_definition> cells;
    tag_block<detail_object> instances;
    tag_block<detail_object_count> counts;
    tag_block<real_vector4d> reference_z_vectors;
    int32 pad_1;
};
TAG_BLOCK_SIZE_ASSERT(structure_detail_object_data, 36);

// max: MAXIMUM_CLUSTERS_PER_STRUCTURE
struct structure_cluster
{
    geometry_section_info geometry_section_info;
    geometry_block_info section_block_info;
    // max: 1
    tag_block<geometry_section> cluster_data;

    // Explaination("CLUSTER INFO", "EMPTY STRING")
    real_rectangle3d bounds;

    uint8 scenario_sky_index;
    uint8 media_index;
    uint8 scenario_visible_sky_index;
    uint8 scenario_atmospheric_fog_index;
    uint8 planar_fog_designator;
    uint8 visible_fog_plane_index;
    // BlockIndex1("structure_bsp_background_sound_palette_block")
    uint16 background_sound_index;
    // BlockIndex1("structure_bsp_sound_environment_palette_block")
    uint16 sound_environment_index;
    // BlockIndex1("structure_bsp_weather_palette_block")
    uint16 weather_index;
    uint16 transition_structure_bsp;

    int8 pad_1[3];
    e_structure_cluster_flags flags;
    int16 pad_2;

    tag_block<predicted_resource> predicted_resources;
    // max: MAXIMUM_CLUSTER_PORTALS_PER_CLUSTER
    tag_block<uint16> portal_indices;

    int32 checksum_from_structure;
    // max: k_maximum_instance_geometry_instances_per_cluster
    tag_block<uint16> instanced_geometry_indices;
    // max: MAXIMUM_STRIP_INDICES_PER_GEOMETRY_SECTION
    tag_block<uint16> strip_indices;
    /****************************************
	* definition_name: cluster_mopp_code_data
	* flags: 0
	* alignment_bit: 16
	* **************************************/
    // DataSize(1048576)
    data_reference collision_mopp_code;
};
TAG_BLOCK_SIZE_ASSERT(structure_cluster, 176);

// max: MAXIMUM_COLLISION_MATERIALS_PER_STRUCTURE
struct structure_conveyor_surface
{
    real_vector3d u;
    real_vector3d v;
};
TAG_BLOCK_SIZE_ASSERT(structure_conveyor_surface, 24);

// max: MAXIMUM_BREAKABLE_SURFACES_PER_STRUCTURE
struct structure_breakable_surface
{
    // BlockIndex1("structure_bsp_instanced_geometry_instances_block")
    uint16 instanced_geometry_instance;
    uint16 breakable_surface_index;
    real_point3d centroid;
    real32 radius;
    uint32 collision_surface_index;
};

// max: MAXIMUM_MARKERS_PER_STRUCTURE
struct structure_marker
{
    static_string32 name;
    real_quaternion rotation;
    real_point3d position;
};
TAG_BLOCK_SIZE_ASSERT(structure_marker, 60);


// max: MAXIMUM_ENVIRONMENT_OBJECT_PALETTE_ENTRIES_PER_STRUCTURE
struct structure_environment_object_palette_entry
{
    // TagReference("scen")
    tag_reference definition;
    // TagReference("mode")
    tag_reference model;
    c_flags<e_object_type, uint32, k_object_types_count> object_flags;
};
TAG_BLOCK_SIZE_ASSERT(structure_environment_object_palette_entry, 20);

// max: MAXIMUM_ENVIRONMENT_OBJECTS_PER_STRUCTURE
struct structure_environment_object
{
    static_string32 name;
    real_quaternion rotation;
    real_point3d translation;
    // BlockIndex1("structure_bsp_environment_object_palette_block")
    uint16 palette_index;
    datum unique_id;
    c_static_string<4> exported_object_type;
	static_string32 scenario_object_name;
};
TAG_BLOCK_SIZE_ASSERT(structure_environment_object, 104);

// max: 32*k_kilo
struct s_transparent_plane
{
    uint16 section_index;
    uint16 part_index;
    real_plane3d plane;
};
TAG_BLOCK_SIZE_ASSERT(s_transparent_plane, 20);

struct structure_bsp
{
    tag_block<tag_import_info> import_info;
    int32 pad_1;
    tag_block<structure_collision_material> collision_materials;
    tag_block<collision_bsp> collision;

    // bounds of the valid height in Z where vehicles can operate or get pushed by an unstoppable force.
    real_bounds vehicle_z_limits;

    tag_block<unused_structure_node> unused_nodes;
    tag_block<structure_leaf> leaves;

    real_rectangle3d world_bounds;

    tag_block<structure_surface_reference> surface_references;

    data_reference cluster_data;
    tag_block<cluster_portal> cluster_portals;

    tag_block<s_structure_fog_plane> fog_planes;

    int8 pad_2[24];

    tag_block<structure_weather_palette_entry> weather_palette;
    tag_block<structure_weather_polyhedron> weather_polyhedra;
    tag_block<structure_detail_object_data> detail_objects;
    tag_block<structure_cluster> clusters;
    tag_block<geometry_material> materials;

	// Max: MAXIMUM_SKIES_PER_SCENARIO
    tag_block<uint16> sky_owner_cluster;

	tag_block<structure_conveyor_surface> conveyor_surfaces;
    tag_block<structure_breakable_surface> breakable_surfaces;
    tag_block<pathfinding_data> pathfinding_data;

    // max: MAXIMUM_EDGES_PER_COLLISION_BSP
    tag_block<uint8> pathfinding_edges;

    tag_block<structure_background_sound_palette_entry> background_sound_palette;
    tag_block<structure_sound_environment_palette_entry> sound_environment_palette;

    /****************************************
	* definition_name: structure_bsp_cluster_encoded_sound_data
	* flags: 0
	* alignment_bit: 0
	****************************************/
	// DataSize(131072)
    data_reference sound_pas_data;

    tag_block<structure_marker> markers;
    tag_block<structure_runtime_decal> runtime_decals;
    tag_block<structure_environment_object_palette_entry> environment_object_palette;
    tag_block<structure_environment_object> environment_objects;
    tag_block<structure_lightmap_data> lightmap_data;

    uint32 pad_3;

    tag_block<map_leaf> map_leaves;
    tag_block<leaf_connection> leaf_connections;
    tag_block<error_report_category> errors;
    tag_block<structure_precomputed_lighting> precomputed_lighting;
    tag_block<structure_instanced_geometry_definition> instanced_geometry_definitions;
    tag_block< structure_instanced_geometry_instance> instanced_geometry_instances;

    tag_block<s_structure_sound_cluster> ambience_sound_clusters;
    tag_block<s_structure_sound_cluster> reverb_sound_clusters;
    tag_block<s_transparent_plane> transparent_planes;

    int8 pad_4[96];
    /// Distances this far and longer from limit origin will pull you back in.
    real32 vehicle_spherical_limit_radius;
    /// Center of space in which vehicle can move.
    real_point3d vehicle_spherical_limit_center;

    tag_block<s_structure_debug_info> debug_info;

	// TagReference("DECP")
    tag_reference decorators;

    s_structure_physics structure_physics;

    tag_block<s_water_definition> water_definitions;
    tag_block<s_structure_portal_device_map> portal_device_map;
    tag_block<s_structure_audibility> audibility;
    tag_block<s_object_fake_lightprobe> fake_lightprobes;
    tag_block<c_decorator_placement_definition> decorator_placement;
};
TAG_GROUP_SIZE_ASSERT(structure_bsp, 572);

int16 get_global_structure_bsp_index(void);
structure_bsp* get_global_structure_bsp(void);