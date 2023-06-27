#pragma once
#include "Blam/Cache/DataTypes/TagRef.h"
#include "Blam/Common/Common.h"
#include "Blam/Engine/cseries/cseries_strings.h"
#include "Blam/Math/real_math.h"

#define MAXIMUM_NODES_PER_BSP2D 131072
#define MAXIMUM_BSPS_PER_COLLISION_REGION 64
#define k_maximum_instance_geometry_instances_per_structure_bsp 1024
#define MAXIMUM_WEATHER_PALETTE_ENTRIES_PER_STRUCTURE 32

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

// max count: k_maximum_instance_geometry_instances_per_structure_bsp 1024
struct instanced_geometry_reference
{
	short pathfinding_object_index;
	short pad;
};
TAG_BLOCK_SIZE_ASSERT(instanced_geometry_reference, 4);

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
