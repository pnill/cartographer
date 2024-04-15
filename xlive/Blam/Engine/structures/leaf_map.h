#pragma once
#include "tag_files/tag_block.h"

#define MAXIMUM_VERTICES_PER_LEAF_CONNECTION 64
#define MAXIMUM_CONNECTIONS_PER_LEAF_MAP 524288
#define MAXIMUM_CONNECTIONS_PER_MAP_LEAF 512
#define MAXIMUM_FACES_PER_MAP_LEAF 512
#define MAXIMUM_LEAVES_PER_BSP3D 65536

// max: MAXIMUM_CONNECTIONS_PER_LEAF_MAP
struct leaf_connection
{
	uint32 plane_index;
	uint32 back_leaf_index;
	uint32 front_leaf_index;

	// max: MAXIMUM_VERTICES_PER_LEAF_CONNECTION
	tag_block<real_point3d> vertices;

	real32 area;
};
TAG_BLOCK_SIZE_ASSERT(leaf_connection, 24);

// max: MAXIMUM_FACES_PER_MAP_LEAF
struct map_leaf_face
{
	uint32 node_index;

	// max: MAXIMUM_VERTICES_PER_LEAF_CONNECTION
	tag_block<real_point3d> vertices;
};
TAG_BLOCK_SIZE_ASSERT(map_leaf_face, 12);

// max: MAXIMUM_LEAVES_PER_BSP3D
struct map_leaf
{
	tag_block<map_leaf_face> faces;

	// max: MAXIMUM_CONNECTIONS_PER_MAP_LEAF
	tag_block<uint32> connection_indices;
};
TAG_BLOCK_SIZE_ASSERT(map_leaf, 16);

// max count: MAXIMUM_LEAVES_PER_BSP3D
struct structure_leaf
{
	uint16 cluster;
	uint16 surface_reference_count;
	uint32 first_surface_reference_index;
};
TAG_BLOCK_SIZE_ASSERT(structure_leaf, 8);