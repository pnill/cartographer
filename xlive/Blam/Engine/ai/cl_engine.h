#pragma once
#include "tag_files/tag_block.h"



enum e_cs_point_set_flags : int
{
    cs_point_set_flag_manual_reference_frame = FLAG(0),
    cs_point_set_flag_turret_deployment = FLAG(1)
};

// max count: 20
struct cs_point
{
    static_string32 name;
    real_point3d position;
    short pad;
    int surface_index;
    real_euler_angles2d facing_direction;
};
ASSERT_STRUCT_SIZE(cs_point, 60);

// max count: 200
struct cs_point_set
{
    static_string32 name;
    tag_block<cs_point> points;
    short bsp_index;            // Block index: scenario_structure_bsp_reference
    short manual_reference_frame;
    e_cs_point_set_flags flags;
};
ASSERT_STRUCT_SIZE(cs_point_set, 48);

// max count: 1
struct cs_script_data
{
	tag_block<cs_point_set> point_sets;
	byte pad[120];
};
ASSERT_STRUCT_SIZE(cs_script_data, 128);
