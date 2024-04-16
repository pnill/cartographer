#pragma once
#include "errors/error_reports.h"
#include "geometry/geometry_block.h"
#include "geometry/geometry_definitions_new.h"
#include "math/matrix_math.h"
#include "render/render_prt.h"
#include "structures/structure_bsp_definitions.h"
#include "tag_files/string_id.h"
#include "tag_files/tag_import_definitions.h"

enum e_render_model_definition_flags : int16
{
	_render_model_definition_force_third_person_bit = FLAG(0),
	_render_model_definition_force_carmackreverse = FLAG(1),
	_render_model_definition_force_node_maps = FLAG(2),
	_render_model_definition_geometry_postprocessed = FLAG(3)
};

enum e_render_model_section_flags : int16
{
    _render_model_section_geometry_postprocessed = FLAG(0)
};

enum e_detail_levels : int16
{
    _detail_level_l1_super_low = FLAG(0),
    _detail_level_l2_low = FLAG(1),
    _detail_level_l3_medium = FLAG(2),
    _detail_level_l4_high = FLAG(3),
    _detail_level_l5_super_high = FLAG(4),
    _detail_level_l6_hollywood = FLAG(5)
};

// max count: MAXIMUM_PERMUTATIONS_PER_MODEL_REGION
struct render_model_permutation
{
    string_id name;
    int16 l1_section_index; // Super low
    int16 l2_section_index; // Low
    int16 l3_section_index; // Medium
    int16 l4_section_index; // High
    int16 l5_section_index; // Super High
    int16 l6_section_index; // Hollywood
};
ASSERT_STRUCT_SIZE(render_model_permutation, 16);

// max count: MAXIMUM_REGIONS_PER_MODEL
struct render_model_region
{
    string_id name;
    int16 node_map_offset;
    int16 node_map_size;
    tag_block<render_model_permutation> permutations;
};
ASSERT_STRUCT_SIZE(render_model_region, 16);

struct render_model_section_data
{
    geometry_section section;
    geometry_point_data point_data;
    tag_block<uint8> node_map;
    int32 pad;
};
ASSERT_STRUCT_SIZE(render_model_section_data, 112);

// max count: MAXIMUM_SECTIONS_PER_RENDER_MODEL
struct render_model_section
{
    e_geometry_classification global_geometry_classification;
    int16 pad;
    geometry_section_info section_info;

    int16 rigid_node;   // Block index: render_model_node_block
    e_render_model_section_flags flags;
    tag_block<render_model_section_data> section_data;
    geometry_block_info geometry_block_info;
};
ASSERT_STRUCT_SIZE(render_model_section, 92);

// max count: MAXIMUM_NODES_PER_MODEL
struct render_model_compound_node
{
    int8 node_indices[4];
    real32 node_weights[3];
};
ASSERT_STRUCT_SIZE(render_model_compound_node, 16);

// max count: NUMBER_OF_GEOMETRY_DETAIL_LEVELS
struct render_model_section_group
{
    e_detail_levels detail_levels;
    int16 pad;
    tag_block<render_model_compound_node> compound_nodes;
};
ASSERT_STRUCT_SIZE(render_model_section_group, 12);

// max count: MAXIMUM_NODES_PER_MODEL
struct render_model_node
{
    string_id name;
    int16 parent_node;          // Block index: render_model_node
    int16 first_child_node;     // Block index: render_model_node
    int16 next_sibling_node;    // Block index: render_model_node
    int16 import_node_index;

    real_point3d default_translation;
    real_quaternion default_rotation;
    real_matrix4x3 default_inverse_matrix;
    real32 distance_from_parent;
};
ASSERT_STRUCT_SIZE(render_model_node, 96);

// max count: MAXIMUM_MARKERS_PER_RENDER_MODEL_MARKER_GROUP
struct render_model_marker
{
    uint8 region_index;
    uint8 permutation_index;
    uint8 node_index;
    uint8 pad;
    real_point3d position;
    real_quaternion rotation;
    real32 scale;
};
ASSERT_STRUCT_SIZE(render_model_marker, 36);

// max count: MAXIMUM_MARKER_GROUPS_PER_RENDER_MODEL
struct render_model_marker_group
{
    string_id name;
    tag_block<render_model_marker> markers;
};
ASSERT_STRUCT_SIZE(render_model_marker_group, 12);

// max count: MAXIMUM_SECTIONS_PER_RENDER_MODEL
struct section_render_leaves
{
    tag_block<node_render_leaves> node_render_leaves;
};
ASSERT_STRUCT_SIZE(section_render_leaves, 8);

struct render_model_definition
{
	string_id name;
	e_render_model_definition_flags flags;
	int16 pad;
	int32 model_checksum;
    tag_block<tag_import_info> import_info;
    tag_block<geometry_compression_info> compression_info;
    tag_block<render_model_region> regions;
    tag_block<render_model_section> sections;
    tag_block<uint32> invalid_section_pair_bits;    // max count: ((((MAXIMUM_SECTIONS_PER_RENDER_MODEL*(MAXIMUM_SECTIONS_PER_RENDER_MODEL - 1)/2)+(LONG_BITS-1))>>LONG_BITS_BITS))
    tag_block<render_model_section_group> section_groups;

    int8 l1_section_group_index; // Super low
    int8 l2_section_group_index; // Low
    int8 l3_section_group_index; // Medium
    int8 l4_section_group_index; // High
    int8 l5_section_group_index; // Super High
    int8 l6_section_group_index; // Hollywood

    int16 pad1;
    int32 node_list_checksum;
    tag_block<render_model_node> nodes;
    tag_block<uint8> node_map_old;  // max count: MAXIMUM_NODES_PER_RENDER_MODEL_SECTION*MAXIMUM_REGIONS_PER_MODEL
    tag_block<render_model_marker_group> marker_groups;
    tag_block<geometry_material> materials;
    tag_block<error_report_category> errors;

    real_angle dont_draw_over_camera_cosine_angle;  // dont draw fp model when camera > this angle cosine (-1,1) Sugg. -0.2. 0 disables.
    tag_block<prt_info> prt_info;
    tag_block<section_render_leaves> section_render_leaves;
};
ASSERT_STRUCT_SIZE(render_model_definition, 132);
