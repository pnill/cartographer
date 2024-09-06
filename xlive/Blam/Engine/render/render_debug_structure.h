#pragma once
#include "tag_files/tag_block.h"

/* constants */

#define MAXIMUM_CLUSTERS_PER_STRUCTURE 512
#define MAXIMUM_FOG_PLANES_PER_STRUCTURE 127
#define MAXIMUM_FOG_ZONES_PER_STRUCTURE 127

/* enums */

enum e_structure_debug_info_render_line_type : uint16
{
	_structure_debug_info_render_line_fog_plane_boundary_edge = 0,
	_structure_debug_info_render_line_fog_plane_internal_edge = 1,
	_structure_debug_info_render_line_fog_zone_flood_fill = 2,
	_structure_debug_info_render_line_fog_zone_cluster_centroid = 3,
	_structure_debug_info_render_line_fog_zone_cluster_geometry = 4,
	_structure_debug_info_render_line_fog_zone_portal_centroid = 5,
	_structure_debug_info_render_line_fog_zone_portal_geometry = 6
};

enum e_structure_cluster_debug_warnings : uint16
{
	_structure_cluster_debug_warning_multiple_visible_fog_planes = FLAG(0),
	_structure_cluster_debug_warning_visible_fog_cluster_omission = FLAG(1),
	_structure_cluster_debug_warning_fog_plane_missed_render_bsp = FLAG(2)
};
enum e_structure_cluster_debug_errors : uint16
{
	_structure_cluster_debug_error_multiple_fog_planes = FLAG(0),
	_structure_cluster_debug_error_fog_zone_collision = FLAG(1),
	_structure_cluster_debug_error_fog_zone_immersion = FLAG(2)
};

/* structures */

// max: SHORT_MAX
struct s_structure_debug_info_render_line
{
	e_structure_debug_info_render_line_type type;
	int16 code;
	int16 pad_thai;
	int16 pad;
	real_point3d point_0;
	real_point3d point_1;
};
ASSERT_STRUCT_SIZE(s_structure_debug_info_render_line, 32);

// max: MAXIMUM_CLUSTERS_PER_STRUCTURE
struct s_structure_cluster_debug_info
{
	e_structure_cluster_debug_errors errors;
	e_structure_cluster_debug_warnings warnings;
	int8 pad[28];
	tag_block<s_structure_debug_info_render_line> lines;

	// max: SHORT_MAX
	tag_block<int32> fog_plane_indices;
	// max: SHORT_MAX
	tag_block<int32> visible_fog_plane_indices;
	// max: SHORT_MAX
	tag_block<int32> visible_fog_omission_cluster_indices;
	// max: SHORT_MAX
	tag_block<int32> containing_fog_zone_indices;
};
ASSERT_STRUCT_SIZE(s_structure_cluster_debug_info, 72);

// max: MAXIMUM_FOG_PLANES_PER_STRUCTURE
struct s_structure_fog_plane_debug_info
{
	uint32 fog_zone_index;
	int8 pad[24];
	uint32 connected_plane_designator;
	tag_block<s_structure_debug_info_render_line> lines;

	// max: SHORT_MAX
	tag_block<int32> intersected_cluster_indices;
	// max: SHORT_MAX
	tag_block<int32> inf_extent_cluster_indices;
};
ASSERT_STRUCT_SIZE(s_structure_fog_plane_debug_info, 56);

// max: MAXIMUM_FOG_ZONES_PER_STRUCTURE
struct s_structure_fog_zone_debug_info
{
	uint32 media_index_scenario_fog_pane;
	uint32 base_fog_plane_index;
	int8 pad[24];
	tag_block<s_structure_debug_info_render_line> lines;

	// max: SHORT_MAX
	tag_block<int32> immersed_cluster_indices;
	// max: SHORT_MAX
	tag_block<int32> bounding_fog_plane_indices;
	// max: SHORT_MAX
	tag_block<int32> collision_fog_plance_indices;
};
ASSERT_STRUCT_SIZE(s_structure_fog_zone_debug_info, 64);

// max: 1
struct s_structure_debug_info
{
	int8 pad[64];
	tag_block<s_structure_cluster_debug_info> clusters;
	tag_block<s_structure_fog_plane_debug_info> fog_planes;
	tag_block<s_structure_fog_zone_debug_info> fog_zones;
};
ASSERT_STRUCT_SIZE(s_structure_debug_info, 88);
