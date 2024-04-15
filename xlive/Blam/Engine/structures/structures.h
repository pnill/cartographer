#pragma once

#include "cseries/cseries_strings.h"
#include "tag_files/tag_reference.h"

#define MAXIMUM_SURFACE_REFERENCES_PER_STRUCTURE 262144
#define MAXIMUM_NODES_PER_BSP2D 131072
#define MAXIMUM_BSPS_PER_COLLISION_REGION 64
#define k_maximum_instance_geometry_instances_per_structure_bsp 1024
#define MAXIMUM_WEATHER_PALETTE_ENTRIES_PER_STRUCTURE 32

// max count: MAXIMUM_SURFACE_REFERENCES_PER_STRUCTURE
struct structure_surface_reference
{
	uint16 strip_index;
	uint16 lightmap_triangle_index;
	uint32 bsp_node_index;
};
TAG_BLOCK_SIZE_ASSERT(structure_surface_reference, 8);

// max count: MAXIMUM_SURFACES_PER_COLLISION_BSP 131072
struct bsp2d_ref
{
	int node_ref_or_sector_ref;
};
TAG_BLOCK_SIZE_ASSERT(bsp2d_ref, 4);

// max count: MAXIMUM_NODES_PER_BSP2D 131072
struct large_bsp2d_node
{
	real_plane2d plane;
	int left_child;
	int right_child;
};
TAG_BLOCK_SIZE_ASSERT(large_bsp2d_node, 20);

#pragma pack(push, 1)
struct s_structure_globals
{
	bool cluster_marker_initialized;
	int8 pad[2];
	int32 cluster_index;
	int32 cluster_instances[512];
	bool instanced_geometry_marker_initialized;
	int8 pad1[3];
	int32 instanced_geometry_index;
	int32 instanced_geometry_instances[1024];
};
#pragma pack(pop)
CHECK_STRUCT_SIZE(s_structure_globals, 6159);

s_structure_globals* structure_globals_get(void);


