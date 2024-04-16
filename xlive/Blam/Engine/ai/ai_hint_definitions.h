#pragma once
#include "tag_files/tag_block.h"



#define k_max_flight_hints_per_bsp 50
#define k_max_points_per_flight_hint 10

// max count: 200
struct user_hint_point
{
    real_point3d point;
    short reference_frame;
    short pad;
};
ASSERT_STRUCT_SIZE(user_hint_point, 16);

// max count: 200
struct user_hint_ray_block
{
    user_hint_point point;
    real_vector3d vector;
};
ASSERT_STRUCT_SIZE(user_hint_ray_block, 28);

enum e_user_hint_flags : int
{
    user_hint_flag_bidirectional = FLAG(0),
    user_hint_flag_closed = FLAG(1)
};

// max count: 200
struct user_hint_line_segment
{
    e_user_hint_flags flags;
    user_hint_point point_0;
    user_hint_point point_1;
};
ASSERT_STRUCT_SIZE(user_hint_line_segment, 36);

// max count: 200
struct user_hint_parallelogram_block
{
    e_user_hint_flags flags;
    user_hint_point point_0;
    user_hint_point point_1;
    user_hint_point point_2;
    user_hint_point point_3;
};
ASSERT_STRUCT_SIZE(user_hint_parallelogram_block, 68);

// max count: 200
struct user_hint_polygon
{
    e_user_hint_flags flags;
    tag_block<user_hint_point> points;
};
ASSERT_STRUCT_SIZE(user_hint_polygon, 12);

enum e_force_jump_height : short
{
    force_jump_height_none = 0,
    force_jump_height_down = 1,
    force_jump_height_step = 2,
    force_jump_height_crouch = 3,
    force_jump_height_stand = 4,
    force_jump_height_storey = 5,
    force_jump_height_tower = 6,
    force_jump_height_infinite = 7
};

enum e_control_flags : short
{
    control_flag_magic_lift = FLAG(0)
};

enum e_user_hint_jump_flags : short
{
    user_hint_jump_flag_bidirectional = FLAG(0),
    user_hint_jump_flag_closed = FLAG(1)
};

// max count: 200
struct user_hint_jump
{
    e_user_hint_jump_flags flags;
    short geometry_index;                   // Block index: user_hint_parallelogram
    e_force_jump_height force_jump_height;
    e_control_flags control_flags;
};
ASSERT_STRUCT_SIZE(user_hint_jump, 8);

// max count: 200
struct user_hint_climb
{
    e_user_hint_jump_flags flags;
    short geometry_index;                   // Block index: user_hint_line_segment
};
ASSERT_STRUCT_SIZE(user_hint_climb, 4);

enum e_user_hint_well_flags : int
{
    user_hint_well_flag_bidirectional = FLAG(0)
};

enum e_user_hint_well_point_type : short
{
    user_hint_well_point_type_jump = 0,
    user_hint_well_point_type_climb = 1,
    user_hint_well_point_type_hoist = 2
};

// max count: 200
struct user_hint_well_point
{
    e_user_hint_well_point_type type;
    short pad;
    real_point3d point;
    short reference_frame;
    short pad1;
    int sector_index;
    real_euler_angles2d normal;
}; 
ASSERT_STRUCT_SIZE(user_hint_well_point, 32);

// max count: 200
struct user_hint_well
{
    e_user_hint_well_flags flags;
    tag_block<user_hint_well_point> points;
};
ASSERT_STRUCT_SIZE(user_hint_well, 12);

// max count: k_max_points_per_flight_hint 10
struct user_hint_flight_point
{
    real_point3d point;
};
ASSERT_STRUCT_SIZE(user_hint_flight_point, 12);

// max count: k_max_flight_hints_per_bsp 50
struct user_flight_hint
{
    int pad;
    tag_block<user_hint_flight_point> points;
};
ASSERT_STRUCT_SIZE(user_flight_hint, 12);

// max count: 1
struct user_hint_data
{
    tag_block<user_hint_point> point_geometry;
    tag_block<user_hint_ray_block> ray_geometry;
    tag_block<user_hint_line_segment> line_segment_geometry;
    tag_block<user_hint_parallelogram_block> parallelogram_geometry;
    tag_block<user_hint_polygon> polygon_geometry;
    tag_block<user_hint_jump> jump_hints;
    tag_block<user_hint_climb> climb_hints;
    tag_block<user_hint_well> well_hints;
    tag_block<user_flight_hint> flight_hints;
};
ASSERT_STRUCT_SIZE(user_hint_data, 72);
