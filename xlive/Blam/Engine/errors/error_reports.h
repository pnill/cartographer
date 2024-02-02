#pragma once
#include "Blam/Cache/DataTypes/DataRef.h"
#include "Blam/Cache/DataTypes/TagBlock.h"
#include "Blam/Engine/math/color_math.h"


#define MAXIMUM_ERROR_REPORT_CATEGORIES 64
#define MAXIMUM_REPORTS_PER_ERROR_REPORT_CATEGORY 1024
#define MAXIMUM_VERTICES_PER_ERROR_REPORT 1024
#define MAXIMUM_VECTORS_PER_ERROR_REPORT 1024
#define MAXIMUM_LINES_PER_ERROR_REPORT 1024
#define MAXIMUM_TRIANGLES_PER_ERROR_REPORT 1024
#define MAXIMUM_QUADS_PER_ERROR_REPORT 1024
#define MAXIMUM_COMMENTS_PER_ERROR_REPORT 1024

enum e_error_report_type : int16
{
    _error_report_type_silent = 0,
    _error_report_type_comment = 1,
    _error_report_type_warning = 2,
    _error_report_type_error = 3
};

enum e_error_report_flags : int16
{
    _error_report_flag_rendered = FLAG(0),
    _error_report_flag_tangent_space = FLAG(1),
    _error_report_flag_noncritical = FLAG(2),
    _error_report_flag_lightmap_light = FLAG(3),
    _error_report_flag_report_key_is_valid = FLAG(4)
};

struct s_error_report_point
{
    real_point3d position;
    int8 node_indices[4];
    real32 node_weights[4];
};
CHECK_STRUCT_SIZE(s_error_report_point, 32);

// max count: MAXIMUM_VERTICES_PER_ERROR_REPORT
struct error_report_vertex
{
    s_error_report_point point;
    real_argb_color color;
    real32 screen_size;
};
TAG_BLOCK_SIZE_ASSERT(error_report_vertex, 52);

// max count: MAXIMUM_VECTORS_PER_ERROR_REPORT
struct error_report_vector
{
    s_error_report_point point;
    real_argb_color color;
    real_vector3d normal;
    real32 screen_size;
};
TAG_BLOCK_SIZE_ASSERT(error_report_vector, 64);
 
// max count: MAXIMUM_LINES_PER_ERROR_REPORT
struct error_report_line
{
    s_error_report_point points[2];
    real_argb_color color;
};
TAG_BLOCK_SIZE_ASSERT(error_report_line, 80);

// max count: MAXIMUM_TRIANGLES_PER_ERROR_REPORT
struct error_report_triangle
{
    s_error_report_point points[3];
    real_argb_color color;
};
TAG_BLOCK_SIZE_ASSERT(error_report_triangle, 112);

// max count: MAXIMUM_QUADS_PER_ERROR_REPORT
struct error_report_quad
{
    s_error_report_point points[4];
    real_argb_color color;
};
TAG_BLOCK_SIZE_ASSERT(error_report_quad, 144);

// max count: MAXIMUM_COMMENTS_PER_ERROR_REPORT
struct error_report_comment
{
    data_block text;    // Data size: 8192
    s_error_report_point point;
    real_argb_color color;
};
TAG_BLOCK_SIZE_ASSERT(error_report_comment, 56);

// max count: MAXIMUM_REPORTS_PER_ERROR_REPORT_CATEGORY
struct error_reports_block
{
    e_error_report_type type;
    e_error_report_flags flags;

    data_block text;    // Data size: 8192
    char source_filename[32];
    int32 source_line_number;
    tag_block<error_report_vertex> vertices;
    tag_block<error_report_vector> vectors;
    tag_block<error_report_line> lines;
    tag_block<error_report_triangle> triangles;
    tag_block<error_report_quad> quads;
    tag_block<error_report_comment> comments;
    int8 pad[380];
    int32 report_key;
    int32 node_index;
    real_rectangle3d bounds;
    real_argb_color color;
    int8 pad1[84];
};
TAG_BLOCK_SIZE_ASSERT(error_reports_block, 608);

// max count: MAXIMUM_ERROR_REPORT_CATEGORIES
struct error_report_category
{
    char name[256];
    e_error_report_type report_type;
    e_error_report_flags flags;
    int16 pad;
    int16 pad1;
    int8 pad2[404];
    tag_block<error_reports_block> reports;
};
TAG_BLOCK_SIZE_ASSERT(error_report_category, 676);
