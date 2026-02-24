#pragma once
#include "geometry/geometry_block.h"
#include "geometry/geometry_definitions_new.h"
#include "objects/object_types.h"
#include "physics/structure_physics.h"

/* constants */

enum
{
	MAXIMUM_ENVIRONMENT_OBJECT_PALETTE_ENTRIES_PER_STRUCTURE = 100,
	MAXIMUM_ENVIRONMENT_OBJECTS_PER_STRUCTURE = 16384,
	MAXIMUM_MARKERS_PER_STRUCTURE = 1024,
	MAXIMUM_BREAKABLE_SURFACES_PER_STRUCTURE = 8448,
	MAXIMUM_SKIES_PER_SCENARIO = 32,
	k_maximum_instance_geometry_instances_per_cluster = 1024,
	MAXIMUM_CLUSTER_PORTALS_PER_CLUSTER = 512,
	MAXIMUM_DETAIL_OBJECTS_PER_STRUCTURE = 2097152,
	MAXIMUM_DETAIL_OBJECT_CELLS_PER_STRUCTURE = 262144,
	MAXIMUM_DETAIL_OBJECT_LAYERS_PER_STRUCTURE = 32,
	MAXIMUM_DETAIL_OBJECT_COUNT = (MAXIMUM_DETAIL_OBJECT_LAYERS_PER_STRUCTURE * MAXIMUM_DETAIL_OBJECT_CELLS_PER_STRUCTURE),
	MAXIMUM_PLANES_PER_WEATHER_POLYHEDRON = 16,
	MAXIMUM_WEATHER_POLYHEDRA_PER_STRUCTURE = 32,
	MAXIMUM_FOG_PLANES_PER_STRUCTURE = 127,
	MAXIMUM_VERTICES_PER_CLUSTER_PORTAL = 128,
	MAXIMUM_CLUSTER_PORTALS_PER_STRUCTURE = 512,
	MAXIMUM_NODES_PER_BSP3D = 131072,
	MAXIMUM_COLLISION_MATERIALS_PER_STRUCTURE = 512,
	MAXIMUM_FOG_ZONES_PER_STRUCTURE = 127,
};

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

// max: MAXIMUM_NODES_PER_BSP3D
struct unused_structure_node
{
	int8 pad_1[6];
};
ASSERT_STRUCT_SIZE(unused_structure_node, 6);

// max: MAXIMUM_COLLISION_MATERIALS_PER_STRUCTURE
struct structure_collision_material
{
	tag_reference old_shader; // shad
	int16 global_material_index;
	// BlockIndex1("structure_bsp_conveyor_surface_block")
	uint16 conveyor_surface_index;
	tag_reference new_shader; // shad
};
ASSERT_STRUCT_SIZE(structure_collision_material, 20);

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
	s_tag_block vertices;	// struct: real_point3d
};
ASSERT_STRUCT_SIZE(cluster_portal, 36);

// max: MAXIMUM_FOG_PLANES_PER_STRUCTURE
struct s_structure_fog_plane
{
	uint16 scenario_planar_for_index;
	int16 pad_1;
	real_plane3d plane;
	e_structure_fog_plane_flags flags;
	int16 priority;
};
ASSERT_STRUCT_SIZE(s_structure_fog_plane, 24);

// max: MAXIMUM_WEATHER_POLYHEDRA_PER_STRUCTURE
struct structure_weather_polyhedron
{
	real_point3d bounding_sphere_center;
	real32 bounding_sphere_radius;
	// max: MAXIMUM_PLANES_PER_WEATHER_POLYHEDRON
	s_tag_block planes;	// struct: real_plane3d
};
ASSERT_STRUCT_SIZE(structure_weather_polyhedron, 24);

// max count: MAXIMUM_WEATHER_PALETTE_ENTRIES_PER_STRUCTURE 32
struct structure_weather_palette_entry
{
	char name[k_tag_string_length];
	tag_reference weather_system;   // weat
	int16 pad[2];
	int32 pad1[8];
	tag_reference wind;             // wind
	real_vector3d wind_direction;
	real32 wind_magnitude;
	int32 pad2;
	char wind_scale_function[k_tag_string_length];
};
ASSERT_STRUCT_SIZE(structure_weather_palette_entry, 136);

// max: MAXIMUM_DETAIL_OBJECT_COUNT
struct detail_object_count
{
	int16 unknown_1;
};
ASSERT_STRUCT_SIZE(detail_object_count, 2);

// max: MAXIMUM_DETAIL_OBJECTS_PER_STRUCTURE
struct detail_object
{
	uint32 unknown_1;
	uint16 unknown_2;
};
ASSERT_STRUCT_SIZE(detail_object, 8);

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
ASSERT_STRUCT_SIZE(detail_object_cell_definition, 32);

// max: 1
// names can be inferred from MCC 0x0103BBF at a future time for all child objects
// used to access dobc tag files
struct structure_detail_object_data
{
	s_tag_block cells;					// struct: detail_object_cell_definition
	s_tag_block instances;				// struct: detail_object
	s_tag_block counts;					// struct: detail_object_count
	s_tag_block reference_z_vectors;	// struct: real_vector4d
	int32 pad_1;
};
ASSERT_STRUCT_SIZE(structure_detail_object_data, 36);

// max: MAXIMUM_CLUSTERS_PER_STRUCTURE
struct structure_cluster
{
	geometry_section_info geometry_section_info;
	geometry_block_info section_block_info;
	// max: 1
	s_tag_block cluster_data;	// struct: geometry_section

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

	s_tag_block predicted_resources;	//struct: predicted_resource
	// max: MAXIMUM_CLUSTER_PORTALS_PER_CLUSTER
	s_tag_block portal_indices;			// struct: uint16

	int32 checksum_from_structure;
	// max: k_maximum_instance_geometry_instances_per_cluster
	s_tag_block instanced_geometry_indices;	// struct: uint16
	// max: MAXIMUM_STRIP_INDICES_PER_GEOMETRY_SECTION
	s_tag_block strip_indices;				// struct: uint16
	/****************************************
	* definition_name: cluster_mopp_code_data
	* flags: 0
	* alignment_bit: 16
	* **************************************/
	// DataSize(1048576)
	data_reference collision_mopp_code;
};
ASSERT_STRUCT_SIZE(structure_cluster, 176);

// max: MAXIMUM_COLLISION_MATERIALS_PER_STRUCTURE
struct structure_conveyor_surface
{
	real_vector3d u;
	real_vector3d v;
};
ASSERT_STRUCT_SIZE(structure_conveyor_surface, 24);

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
	char name[k_tag_string_length];
	real_quaternion rotation;
	real_point3d position;
};
ASSERT_STRUCT_SIZE(structure_marker, 60);


// max: MAXIMUM_ENVIRONMENT_OBJECT_PALETTE_ENTRIES_PER_STRUCTURE
struct structure_environment_object_palette_entry
{
	tag_reference definition;   // scen
	tag_reference model;        // mode
	c_flags_no_init<e_object_type, uint32, k_object_types_count> object_flags;
};
ASSERT_STRUCT_SIZE(structure_environment_object_palette_entry, 20);

// max: MAXIMUM_ENVIRONMENT_OBJECTS_PER_STRUCTURE
struct structure_environment_object
{
	char name[k_tag_string_length];
	real_quaternion rotation;
	real_point3d translation;
	// BlockIndex1("structure_bsp_environment_object_palette_block")
	uint16 palette_index;
	datum unique_id;
	char exported_object_type[4];
	char scenario_object_name[k_tag_string_length];
};
ASSERT_STRUCT_SIZE(structure_environment_object, 104);

// max: 32*k_kilo
struct s_transparent_plane
{
	uint16 section_index;
	uint16 part_index;
	real_plane3d plane;
};
ASSERT_STRUCT_SIZE(s_transparent_plane, 20);

struct structure_bsp
{
	s_tag_block import_info;			// struct: tag_import_info
	int32 pad_1;
	s_tag_block collision_materials;	// struct: structure_collision_material
	s_tag_block collision;				// struct: collision_bsp

	// bounds of the valid height in Z where vehicles can operate or get pushed by an unstoppable force.
	real_bounds vehicle_z_limits;

	s_tag_block unused_nodes;			// struct: unused_structure_node
	s_tag_block leaves;	// struct: structure_leaf

	real_rectangle3d world_bounds;

	s_tag_block surface_references;	// struct: structure_surface_reference

	data_reference cluster_data;
	s_tag_block cluster_portals;		// struct: cluster_portal

	s_tag_block fog_planes;			// struct: s_structure_fog_plane

	int8 pad_2[24];

	s_tag_block weather_palette;	// struct: structure_weather_palette_entry
	s_tag_block weather_polyhedra;	// struct: structure_weather_polyhedron
	s_tag_block detail_objects;		// struct: structure_detail_object_data
	s_tag_block clusters;			// struct: structure_cluster
	s_tag_block materials;			// struct: geometry_material

	// Max: MAXIMUM_SKIES_PER_SCENARIO
	s_tag_block sky_owner_cluster;	// struct:uint16

	s_tag_block conveyor_surfaces;	// struct: structure_conveyor_surface
	s_tag_block breakable_surfaces;	// struct: structure_breakable_surface
	s_tag_block pathfinding_data;	// struct: pathfinding_data

	// max: MAXIMUM_EDGES_PER_COLLISION_BSP
	s_tag_block pathfinding_edges;	// struct: uint8

	s_tag_block background_sound_palette;	// struct: structure_background_sound_palette_entry
	s_tag_block sound_environment_palette;	// struct: structure_sound_environment_palette_entry

	/****************************************
	* definition_name: structure_bsp_cluster_encoded_sound_data
	* flags: 0
	* alignment_bit: 0
	****************************************/
	// DataSize(131072)
	data_reference sound_pas_data;

	s_tag_block markers;					// struct: structure_marker
	s_tag_block runtime_decals;				// struct: structure_runtime_decal
	s_tag_block environment_object_palette;	// struct: structure_environment_object_palette_entry
	s_tag_block environment_objects;		// struct: structure_environment_object
	s_tag_block lightmap_data;				// struct: structure_lightmap_data

	uint32 pad_3;

	s_tag_block map_leaves;						// struct: map_leaf
	s_tag_block leaf_connections;				// struct: leaf_connection
	s_tag_block errors;							// struct: error_report_category
	s_tag_block precomputed_lighting;			// struct: structure_precomputed_lighting
	s_tag_block instanced_geometry_definitions;	// struct: structure_instanced_geometry_definition
	s_tag_block instanced_geometry_instances;	// struct: structure_instanced_geometry_instance

	s_tag_block ambience_sound_clusters;		// struct: s_structure_sound_cluster
	s_tag_block reverb_sound_clusters;			// struct: s_structure_sound_cluster
	s_tag_block transparent_planes;				// struct: s_transparent_plane

	int8 pad_4[96];
	/// Distances this far and longer from limit origin will pull you back in.
	real32 vehicle_spherical_limit_radius;
	/// Center of space in which vehicle can move.
	real_point3d vehicle_spherical_limit_center;

	s_tag_block debug_info;	// struct: s_structure_debug_info

	tag_reference decorators;	// DECP

	s_structure_physics structure_physics;

	s_tag_block water_definitions;		// struct: s_water_definition
	s_tag_block portal_device_map;		// struct: s_structure_portal_device_map
	s_tag_block audibility;				// struct: s_structure_audibility
	s_tag_block fake_lightprobes;		// struct: s_object_fake_lightprobe
	s_tag_block decorator_placement;	// struct: c_decorator_placement_definition
};
ASSERT_STRUCT_SIZE(structure_bsp, 572);

int16 get_global_structure_bsp_index(void);

structure_bsp* get_global_structure_bsp(void);
