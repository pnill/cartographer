#pragma once

#include "Blam/Cache/DataTypes/TagBlock.h"
#include "Blam/Engine/ai/path_flying.h"
#include "Blam/Engine/cseries/cseries_strings.h"


enum e_zone_definition_flags : int
{
    zone_definition_flags_manual_bsp_index = FLAG(0)
};

enum e_firing_position_flags : short
{
    firing_position_flag_open = FLAG(0),
    firing_position_flag_partial = FLAG(1),
    firing_position_flag_closed = FLAG(2),
    firing_position_flag_mobile = FLAG(3),
    firing_position_flag_wall_lean = FLAG(4),
    firing_position_flag_perch = FLAG(5),
    firing_position_flag_ground_point = FLAG(6),
    firing_position_flag_dynamic_cover_point = FLAG(7)
};

// max count: 512
struct firing_position_definition
{
    // Explaination("CONTROLS~", "Ctrl-N: Creates a new area and assigns it to the current selection of firing points.")
    real_point3d position_local;
    short reference_frame;

    e_firing_position_flags flags;
    short area;                     // Block index: areas
    short cluster_index;            // References the cluster index of the current scenario structure bsp
    int pad;
    real_euler_angles2d normal;
};
TAG_BLOCK_SIZE_ASSERT(firing_position_definition, 32);

enum e_area_flags : int
{
    area_flag_vehicle_area = FLAG(0),
    area_flag_perch = FLAG(1),
    area_flag_manual_reference_frame = FLAG(2)
};

// max count: 64
struct area_definition
{
    static_string32 name;
    e_area_flags area_flags;

    // The below fields are populated on cache compile
    real_point3d position;
    short field_30;
    short field_32;
    float field_34;
    short firing_position_start_index;
    short firing_position_count;
    short field_3C;
    short field_3E;
    int field_40;
    char field_44[4];
    short field_48;
    short field_4A;
    float field_4C[12];

    short manual_reference_frame;
    short pad;
    tag_block<flight_hint_reference> flight_hints;
};
TAG_BLOCK_SIZE_ASSERT(area_definition, 136);

// max count: 128
struct zone_definition
{
    static_string32 name;
    e_zone_definition_flags flags;
    short manual_bsp;       // Block index: scenario_structure_bsp_reference
    short pad;
    tag_block<firing_position_definition> firing_positions;
    tag_block<area_definition> areas;
};
TAG_BLOCK_SIZE_ASSERT(zone_definition, 56);

enum e_order_area_type : short
{
    order_area_type_deault = 0,
    order_area_type_search = 1,
    order_area_type_goal = 2
};

// max count: 32
struct orders_area_reference_definition
{
    e_order_area_type area_type;
    short pad;
    short zone;             // Block index: zone_definition
    short area;             // Block index: the area tagblock index from the zone_definition block above
};
TAG_BLOCK_SIZE_ASSERT(orders_area_reference_definition, 8);
