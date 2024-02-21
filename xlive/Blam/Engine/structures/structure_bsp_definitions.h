#pragma once
#include "tag_files/tag_block.h"

#define MAXIMUM_LEAVES_PER_BSP3D 65536
#define MAXIMUM_SURFACE_REFERENCES_PER_STRUCTURE 262144

// max count: MAXIMUM_LEAVES_PER_BSP3D
struct structure_leaf
{
    uint16 cluster;
    uint16 surface_reference_count;
    uint32 first_surface_reference_index;
};
TAG_BLOCK_SIZE_ASSERT(structure_leaf, 8);

// max count: MAXIMUM_SURFACE_REFERENCES_PER_STRUCTURE
struct structure_surface_reference
{
    uint16 strip_index;
    uint16 lightmap_triangle_index;
    uint32 bsp_node_index;
};
TAG_BLOCK_SIZE_ASSERT(structure_surface_reference, 8);

// max count: MAXIMUM_BSPS_PER_COLLISION_REGION
struct node_render_leaves
{
    tag_block<structure_leaf> collision_leaves;
    tag_block<structure_surface_reference> surface_references;
};
TAG_BLOCK_SIZE_ASSERT(node_render_leaves, 16);
