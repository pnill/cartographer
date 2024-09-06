#pragma once
#include "ai_hint_definitions.h"
#include "sector.h"
#include "sector_geometry.h"
#include "sector_environment_objects.h"

#include "physics/collision_bsp.h"
#include "structures/instanced_geometry_definitions.h"
#include "structures/structures.h"
#include "tag_files/tag_block.h"

enum e_pathfinding_hint_type : short
{
    pathfinding_hint_type_intersection_link = 0,
    pathfinding_hint_type_jump_link = 1,
    pathfinding_hint_type_climb_link = 2,
    pathfinding_hint_type_vault_link = 3,
    pathfinding_hint_type_mount_link = 4,
    pathfinding_hint_type_hoist_link = 5,
    pathfinding_hint_type_wall_jump_link = 6,
    pathfinding_hint_type_breakable_floor = 7
};

// max count: SHORT_MAX 32767
struct pathfinding_hint_data
{
    e_pathfinding_hint_type hint_type;
    short next_hintIndex;
    short hint_data_0;
    short hint_data_1;
    short hint_data_2;
    short hint_data_3;
    short hint_data_4;
    short hint_data_5;
    short hint_data_6;
    short hint_data_7;
}; 
ASSERT_STRUCT_SIZE(pathfinding_hint_data, 20);

// max count: MAXIMUM_STRUCTURE_BSPS_PER_SCENARIO 16
struct pathfinding_data
{
    tag_block<sector> sectors;
    tag_block<sector_link_block> links;
    tag_block<bsp2d_ref> refs;
    tag_block<large_bsp2d_node> bsp2d_nodes;
    tag_block<long_surface_flags> surface_flags;
    tag_block<sector_vertex> vertices;
    tag_block<environment_object_reference> object_refs;
    tag_block<pathfinding_hint_data> pathfinding_hints;
    tag_block<instanced_geometry_reference> instanced_geometry_refs;
    int structure_checksum;
    int pad[8];
    tag_block<user_hint_data> user_placed_hints;
};
ASSERT_STRUCT_SIZE(pathfinding_data, 116);
