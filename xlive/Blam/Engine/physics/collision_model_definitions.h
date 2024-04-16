#pragma once
#include "collision_bsp_definition.h"
#include "errors/error_reports.h"
#include "havok/physics/collide/shape/convex/triangle/hkTriangle.h"
#include "havok/physics/collide/shape/mesh/hkMeshShape.h"
#include "tag_files/string_id.h"
#include "tag_files/tag_block.h"
#include "tag_files/tag_import_definitions.h"

#define MAXIMUM_MATERIALS_PER_MODEL 32
#define MAXIMUM_REGIONS_PER_MODEL 16
#define MAXIMUM_PERMUTATIONS_PER_MODEL_REGION 32
#define MAXIMUM_BSPS_PER_COLLISION_REGION  64
#define MAXIMUM_BSP_PHYSICS_PER_MODEL 1024
#define MAXIMUM_PATHFINDING_SPHERES_PER_COLLISION_MODEL 32
#define MAXIMUM_NODES_PER_COLLISION_MODEL 255

enum e_collision_model_pathfinding_sphere_flags : uint16
{
	_collision_model_pathfinding_sphere_remains_when_open = FLAG(0),
	_collision_model_pathfinding_sphere_vehicle_only = FLAG(1),
	_collision_model_pathfinding_sphere_with_sectors = FLAG(2)
};

enum e_collision_model_definition_flags : uint32
{
	_collision_model_definition_contains_open_edges = FLAG(0)
};

// max: MAXIMUM_NODES_PER_COLLISION_MODEL
struct collision_model_node
{
	string_id name;
	int16 pad;
	// BlockIndex1("collision_model_node_block")
	uint16 parent_node;
	// BlockIndex1("collision_model_node_block")
	uint16 next_sibling_node;
	// BlockIndex1("collision_model_node_block")
	uint16 first_child_node;
};
ASSERT_STRUCT_SIZE(collision_model_node, 12);

// max: MAXIMUM_PATHFINDING_SPHERES_PER_COLLISION_MODEL
struct collision_model_pathfinding_sphere
{
	// BlockIndex1("collision_model_node_block")
	uint16 node;
	e_collision_model_pathfinding_sphere_flags flags;
	real_point3d center;
	real32 radius;
};
ASSERT_STRUCT_SIZE(collision_model_pathfinding_sphere, 20);

// max: MAXIMUM_BSP_PHYSICS_PER_MODEL
// This structure either stores or has something to do with generating the mopp code for havok physics
struct s_collision_bsp_physics_definition
{
	// This may be the wrong havok object, but seems to line up and fit
	// before the next instance of hkBaseObject
	hkTriangleShape triangle;
	// todo: figure out which havok objects these are
	int8 pad[52];
};
ASSERT_STRUCT_SIZE(s_collision_bsp_physics_definition, 116);

// max: MAXIMUM_BSPS_PER_COLLISION_REGION
struct collision_model_bsp
{
	uint16 node_index;
	int16 pad;
	collision_bsp collision;
};
ASSERT_STRUCT_SIZE(collision_model_bsp, 68);

// max: MAXIMUM_PERMUTATIONS_PER_MODEL_REGION
struct collision_model_permutation
{
	string_id name;
	tag_block<collision_model_bsp> collision_bsp;
	tag_block<s_collision_bsp_physics_definition> physics;
};
ASSERT_STRUCT_SIZE(collision_model_permutation, 20);

// max: MAXIMUM_REGIONS_PER_MODEL
struct collision_model_region
{
	string_id name;
	tag_block<collision_model_permutation> permutations;
};
ASSERT_STRUCT_SIZE(collision_model_region, 12);

// max: MAXIMUM_MATERIALS_PER_MODEL
struct collision_model_material
{
	string_id name;
};
ASSERT_STRUCT_SIZE(collision_model_material, 4);

struct collision_model_definition
{
	tag_block<tag_import_info> import_info;
	tag_block<error_report_category> errors;

	e_collision_model_definition_flags flags;

	tag_block<collision_model_material> materials;
	tag_block<collision_model_region> regions;
	tag_block<collision_model_pathfinding_sphere> pathfinding_spheres;
	tag_block<collision_model_node> nodes;
};
