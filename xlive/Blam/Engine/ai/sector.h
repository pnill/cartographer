#pragma once
#include "Blam/Common/Common.h"

enum e_pathfinding_sector_flags : short
{
    pathfinding_sector_flag_sector_walkable = FLAG(0),
    pathfinding_sector_flag_sector_breakable = FLAG(1),
    pathfinding_sector_flag_sector_mobile = FLAG(2),
    pathfinding_sector_flag_sector_bsp_source = FLAG(3),
    pathfinding_sector_flag_floor = FLAG(4),
    pathfinding_sector_flag_ceiling = FLAG(5),
    pathfinding_sector_flag_wall_north = FLAG(6),
    pathfinding_sector_flag_wall_south = FLAG(7),
    pathfinding_sector_flag_wall_east = FLAG(8),
    pathfinding_sector_flag_wall_west = FLAG(9),
    pathfinding_sector_flag_crouchable = FLAG(10),
    pathfinding_sector_flag_aligned = FLAG(11),
    pathfinding_sector_flag_sector_step = FLAG(12),
    pathfinding_sector_flag_sector_interior = FLAG(13)
};

// max count: UNSIGNED_SHORT_MAX-1
struct sector
{
    e_pathfinding_sector_flags pathfinding_sector_flags;
    short hint_index;
    int first_link;     // Do not Set Manually
};
TAG_BLOCK_SIZE_ASSERT(sector, 8);

enum e_sector_link_flags : short
{
    sector_link_flag_sector_link_from_collision_edge = FLAG(0),
    sector_link_flag_sector_intersection_link = FLAG(1),
    sector_link_flag_sector_link_bsp2d_creation_error = FLAG(2),
    sector_link_flag_sector_link_topology_error = FLAG(3),
    sector_link_flag_sector_link_chain_error = FLAG(4),
    sector_link_flag_sector_link_both_sectors_walkable = FLAG(5),
    sector_link_flag_sector_link_magic_hanging_link = FLAG(6),
    sector_link_flag_sector_link_threshold = FLAG(7),
    sector_link_flag_sector_link_crouchable = FLAG(8),
    sector_link_flag_sector_link_wall_base = FLAG(9),
    sector_link_flag_sector_link_ledge = FLAG(10),
    sector_link_flag_sector_link_leanable = FLAG(11),
    sector_link_flag_sector_link_start_corner = FLAG(12),
    sector_link_flag_sector_link_end_corner = FLAG(13)
};

// max count: MAXIMUM_EDGES_PER_COLLISION_BSP 262144 
struct sector_link_block
{
    short vertex1;
    short vertex2;
    e_sector_link_flags link_flags;
    short hint_index;
    short forward_link;
    short reverse_link;
    short left_sector;
    short right_sector;
};
TAG_BLOCK_SIZE_ASSERT(sector_link_block, 16);
