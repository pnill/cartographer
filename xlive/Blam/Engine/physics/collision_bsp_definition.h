#pragma once
#include "tag_files/tag_block.h"

#define MAXIMUM_NODES_PER_BSP3D 131072
#define MAXIMUM_PLANES_PER_BSP3D 65536
#define MAXIMUM_LEAVES_PER_BSP3D 65536
#define MAXIMUM_BSP2D_REFERENCES_PER_COLLISION_BSP 131072
#define MAXIMUM_NODES_PER_BSP2D 131072
#define MAXIMUM_SURFACES_PER_COLLISION_BSP 131072
#define MAXIMUM_EDGES_PER_COLLISION_BSP 262144
#define MAXIMUM_VERTICES_PER_COLLISION_BSP 131072

enum e_collision_surface_flags : uint8
{
	_collision_surface_two_sided = FLAG(0),
	_collision_surface_invisible = FLAG(1),
	_collision_surface_climbable = FLAG(2),
	_collision_surface_breakable = FLAG(3),
	_collision_surface_invalid = FLAG(4),
	_collision_surface_conveyor = FLAG(5)
};

enum e_collision_leaf_flags : uint8
{
	_collision_leaf_contains_double_sided_surfaces = FLAG(0)
};

// max: MAXIMUM_VERTICES_PER_COLLISION_BSP
struct collision_vertex
{
	real_point3d point;
	uint16 first_edge;
	int16 pad_1;
};
TAG_BLOCK_SIZE_ASSERT(collision_vertex, 16);

// max: MAXIMUM_EDGES_PER_COLLISION_BSP
struct collision_edge
{
	uint16 start_vertex;
	uint16 end_vertex;
	uint16 forward_edge;
	uint16 reverse_edge;
	uint16 left_surface;
	uint16 right_surface;
};
TAG_BLOCK_SIZE_ASSERT(collision_edge, 12);

// max: MAXIMUM_SURFACES_PER_COLLISION_BSP
struct collision_surface
{
	uint16 plane;
	uint16 first_edge;
	e_collision_surface_flags flags;
	int8 breakable_surface;
	uint16 material;
};
TAG_BLOCK_SIZE_ASSERT(collision_surface, 8);

// max: MAXIMUM_NODES_PER_BSP2D
struct bsp2d_node
{
	real_plane2d plane;
	uint16 left_child;
	uint16 right_child;
};
TAG_BLOCK_SIZE_ASSERT(bsp2d_node, 16);

// max: MAXIMUM_BSP2D_REFERENCES_PER_COLLISION_BSP
struct bsp2d_reference
{
	uint16 plane;
	uint16 bsp_2d_node;
};
TAG_BLOCK_SIZE_ASSERT(bsp2d_reference, 4);

// max: MAXIMUM_LEAVES_PER_BSP3D
struct collision_leaf
{
	e_collision_leaf_flags flags;
	int8 bsp_2d_reference_count;
	uint16 first_bsp_2d_reference;
};
TAG_BLOCK_SIZE_ASSERT(collision_leaf, 4);

// max: MAXIMUM_NODES_PER_BSP3D
struct bsp3d_node
{
	int8 data[8];
};
TAG_BLOCK_SIZE_ASSERT(bsp3d_node, 8);

struct collision_bsp
{
	tag_block<bsp3d_node> bsp_3d_nodes;
	// max: MAXIMUM_PLANES_PER_BSP3D
	tag_block<real_plane3d> planes;
	tag_block<collision_leaf> leaves;
	tag_block<bsp2d_reference> bsp_2d_references;
	tag_block<bsp2d_node> bsp_2d_nodes;
	tag_block<collision_surface> surfaces;
	tag_block<collision_edge> edges;
	tag_block<collision_vertex> vertices;
};
TAG_GROUP_SIZE_ASSERT(collision_bsp, 64);