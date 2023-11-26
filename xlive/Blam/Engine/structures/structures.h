#pragma once
#include "Blam/Cache/TagGroups/scenario_structure_bsp_definition.hpp"

#include "Blam/Engine/cseries/cseries_strings.h"
#include "Blam/Engine/math/real_math.h"

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

int16 get_global_structure_bsp_index(void);

s_scenario_structure_bsp_group_definition* get_global_structure_bsp(void);
