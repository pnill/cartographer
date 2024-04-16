#pragma once

#include "leaf_map.h"
#include "structures.h"
#include "geometry/geometry_block.h"
#include "geometry/geometry_definitions_new.h"
#include "physics/collision_model_definitions.h"
#include "math/matrix_math.h"

#define k_maximum_instance_geometry_instances_per_structure_bsp 1024
#define k_maximum_instance_geometry_definitions_per_structure_bsp 512

enum e_structure_instanced_geometry_instance_lightmap_policy : uint16
{
	_structure_instanced_geometry_instance_lightmap_per_pixel = 0,
	_structure_instanced_geometry_instance_lightmap_per_vertex = 1
};

enum e_structure_instanced_geometry_instance_pathfinding_policy : uint16
{
	_structure_instanced_geometry_instance_pathfinding_cut_out = 0,
	_structure_instanced_geometry_instance_pathfinding_static = 1,
	_structure_instanced_geometry_instance_pathfinding_none = 2
};

enum e_structure_instanced_geometry_instance_flags : uint16
{
	_structure_instanced_geometry_instance_not_in_light_probes = FLAG(0)
};

// max: k_maximum_instance_geometry_instances_per_structure_bsp
struct structure_instanced_geometry_instance
{
	real32 scale;
	real_matrix3x3 transform;
	real_point3d position;
	// BlockIndex1("structure_bsp_instanced_geometry_definition_block")
	uint16 instance_definition;
	e_structure_instanced_geometry_instance_flags flags;

	int8 pad[20];

	int32 checksum;
	string_id name;
	e_structure_instanced_geometry_instance_pathfinding_policy pathfinding_policy;
	e_structure_instanced_geometry_instance_lightmap_policy lightmap_policy;
};
ASSERT_STRUCT_SIZE(structure_instanced_geometry_instance, 88);

// max: 1
struct structure_instanced_geometry_render_info
{
	geometry_section_info section;
	geometry_block_info block;

	// max: 1
	tag_block<geometry_section_info> cluster_data;

	// max: MAXIMUM_STRIP_INDICES_PER_GEOMETRY_SECTION
	tag_block<uint16> index_reorder_table;
};
ASSERT_STRUCT_SIZE(structure_instanced_geometry_render_info, 92);

// max: k_maximum_instance_geometry_definitions_per_structure_bsp
struct structure_instanced_geometry_definition
{
	structure_instanced_geometry_render_info render_info;
	int32 checksum;
	real_point3d bounding_sphere_center;
	real32 bounding_sphere_radius;
	collision_bsp collision_info;
	tag_block<s_collision_bsp_physics_definition> bsp_physics;
	tag_block<structure_leaf> render_leaves;
	tag_block<structure_surface_reference> surface_references;
};
ASSERT_STRUCT_SIZE(structure_instanced_geometry_definition, 200);

// max count: k_maximum_instance_geometry_instances_per_structure_bsp
struct instanced_geometry_reference
{
	short pathfinding_object_index;
	short pad;
};
ASSERT_STRUCT_SIZE(instanced_geometry_reference, 4);