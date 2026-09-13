#pragma once
#include "errors/error_reports.h"
#include "geometry/geometry_block.h"
#include "geometry/geometry_definitions_new.h"

/* constants */

enum
{
	k_maximum_lightmap_groups_per_scenario = 256,
	k_maximum_palettes_per_lightmap_group = 128,
	k_maximum_lightmap_geometry_sections_per_lightmap_group = 512,
	k_maximum_lightmap_geometry_render_infos_per_lightmap_group = 1024,
	k_maximum_lighting_environments_per_lightmap_group = 1024,
	k_maximum_vertex_buffer_buckets_per_lightmap_group = 1024,
	k_maximum_vertex_buffer_bucket_references_per_lightmap_group = 2000,
	k_maximum_lightmap_objects_per_lightmap_group = 2000
};

enum
{
	STRUCTURE_LIGHTMAP_TAG = 'ltmp'
};

/* macros */

#define structure_lightmap_definition_get(index) ((struct structure_lightmap *)tag_get(STRUCTURE_LIGHTMAP_TAG, (index)))

/* enums */

enum e_structure_lightmap_type : int16
{
	_structure_lightmap_type_normal = 0
};


enum e_structure_lightmap_flags : int16
{
	_structure_lightmap_bit_0 = 0	// unused
};

enum e_lighting_environment_procedural_overide : int16
{
	_lighting_environment_procedural_overide_no_overide = 0,
	_lighting_environment_procedural_overide_cie_clear_sky = 1,
	_lighting_environment_procedural_overide_cie_partly_cloudy = 2,
	_lighting_environment_procedural_overide_cie_cloudy = 3,
	_lighting_environment_procedural_overide_directional_light = 4,
	_lighting_environment_procedural_overide_cone_light = 5,
	_lighting_environment_procedural_overide_sphere_light = 6,
	_lighting_environment_procedural_overide_hemisphere_light = 7
};

enum e_lighting_environment_flags : int16
{
	_lighting_environment_leave_me_alone_please_bit = 0
};

enum e_lightmap_vertex_buffer_bucket_flags : int16
{
	_lightmap_vertex_buffer_bucket_incident_direction_bit = 0,
	_lightmap_vertex_buffer_bucket_incident_color_bit = 1
};

/* structures */

struct lightmap_geometry_section_cache_data
{
	geometry_section_info geometry;
};
ASSERT_STRUCT_SIZE(lightmap_geometry_section_cache_data, 40);

// max count: k_maximum_lightmap_geometry_sections_per_lightmap_group
struct lightmap_geometry_section
{
	geometry_section_info geometry_info;
	geometry_block_info geometry_block_info;

	tag_block<lightmap_geometry_section_cache_data> cache_data;
};
ASSERT_STRUCT_SIZE(lightmap_geometry_section, 84);

// max count: k_maximum_lightmap_geometry_render_infos_per_lightmap_group
struct lightmap_geometry_render_info
{
	int16 bitmap_index;
	uint8 palette_index;
	uint8 pad;
};
ASSERT_STRUCT_SIZE(lightmap_geometry_render_info, 4);

// max count: k_maximum_lighting_environments_per_lightmap_group
struct structure_lightmap_lighting_environment
{
	real_point3d sample_point;
	real32 red_coefficients[9];
	real32 green_coefficients[9];
	real32 blue_coefficients[9];
	real_vector3d mean_incoming_light_direction;
	real_point3d incoming_light_intensity;
	int32 specular_bitmap_index;
	real_vector3d rotation_axis;
	real32 rotation_speed;
	real_vector3d bump_direction;
	real_rgb_color color_tint;

	e_lighting_environment_procedural_overide procedural_override;
	e_lighting_environment_flags flags;

	real_vector3d procedural_param0;
	real_vector4d procedural_param1;
};
ASSERT_STRUCT_SIZE(structure_lightmap_lighting_environment, 220);

// max count: MAXIMUM_VERTICES_PER_GEOMETRY_SECTION
struct lightmap_bucket_vertex
{
	real_rgb_color primary_lightmap_color;
	real_vector3d primary_lightmap_incident_direction;
};
ASSERT_STRUCT_SIZE(lightmap_bucket_vertex, 24);

struct lightmap_vertex_buffer_bucket_cache_data
{
	tag_block<rasterizer_vertex_buffer> cache_data;
};
ASSERT_STRUCT_SIZE(lightmap_vertex_buffer_bucket_cache_data, 8);

// max count: k_maximum_vertex_buffer_buckets_per_lightmap_group
struct lightmap_vertex_buffer_bucket
{
	e_lightmap_vertex_buffer_bucket_flags flags;
	int16 pad;
	tag_block<lightmap_bucket_vertex> raw_vertices;
	geometry_block_info geometry_block_info;
	tag_block<lightmap_vertex_buffer_bucket_cache_data> cache_data;
};
ASSERT_STRUCT_SIZE(lightmap_vertex_buffer_bucket, 56);

// max count: k_maximum_vertex_buffer_bucket_references_per_lightmap_group
struct lightmap_bucket_reference
{
	uint16 flags;
	int16 bucket_index;
	tag_block<uint16> section_offsets;
};
ASSERT_STRUCT_SIZE(lightmap_vertex_buffer_bucket, 56);

// max count: k_maximum_lightmap_objects_per_lightmap_group
struct lightmap_scenery_object_info
{
	int32 unique_id;
	int16 origin_bsp_index;
	uint8 type;
	uint8 source;
	uint32 render_model_checksum;
};
ASSERT_STRUCT_SIZE(lightmap_scenery_object_info, 12);

// max count: k_maximum_lightmap_groups_per_scenario
struct structure_lightmap_group
{
	e_structure_lightmap_type type;
	e_structure_lightmap_flags flags;
	uint32 structure_checksum;

	tag_block<pixel32> section_palette;
	tag_block<pixel32> writable_palettes;

	tag_reference bitmap_group;	// bitm

	tag_block<lightmap_geometry_section> clusters;
	tag_block<lightmap_geometry_render_info> cluster_render_info;
	tag_block<lightmap_geometry_section> poop_definitions;
	tag_block<structure_lightmap_lighting_environment> lighting_environments;
	tag_block<lightmap_vertex_buffer_bucket> geometry_buckets;
	tag_block<lightmap_geometry_render_info> instance_render_info;
	tag_block<lightmap_bucket_reference> instance_bucket_refs;
	tag_block<lightmap_scenery_object_info> scenery_object_info;
	tag_block<lightmap_bucket_reference> scenery_object_bucket_refs;
};
ASSERT_STRUCT_SIZE(structure_lightmap_group, 104);

struct structure_lightmap
{
	/* Explaination("lightmap options", "The following fields control the behavior of the lightmapper

	RADIANCE ESTIMATE SEARCH DISTANCE UPPER BOUND : the largest distance the code will look for photons.bigger levels need a bigger search radius.Measured, in world units, 0.0 defaults to 1.0

	RADIANCE ESTIMATE SEARCH DISTANCE LOWER BOUND : the smallest distance the code will look for photons.bigger levels need a bigger search radius.Measured, in world units, 0.0 defaults to 1.0

	LUMINELS PER WORLD UNIT : how many lightmap pixels there should be per world unit.  0.0 defaults to 3.0

	OUTPUT WHITE REFERENCE : for experimentation - what wattage the lightmapper considers "white" to be for output.  0.0 defaults to 1.0

	OUTPUT BLACK REFERENCE : for experimentation - what wattage the lightmapper considers "black" to be for output.  0.0 defaults to 0.0

	OUTPUT SCHLICK PARAMETER : controls the way midtones are mapped.  0.0 defaults to 4.5

	DIFFUSE MAP SCALE : controls how diffuse maps are scaled.  0.0 defaults to 1.5

	PRT SUN SCALE : 0.0 defaults to 100.0

	PRT SKY SCALE : 0.0 defaults to 1.0

	PRT INDIRECT SCALE : 0.0 defaults to 1.0

	PRT SCALE : you must set this value.

	PRT SURFACE LIGHT SCALE : 0.0 defaults to 1.0

	PRT SCENARIO LIGHT SCALE : 0.0 defaults to 1.0

	LIGHTPROBE INTERPOLATION OVERIDE(speed) : overide the default sampling behavior")*/

	real_vector2d search_distance;
	real32 luminels_per_world_unit;
	real32 output_white_reference;
	real32 output_black_reference;
	real32 output_schlick_parameter;
	real32 diffuse_map_scale;
	real32 sun_scale;
	real32 sky_scale;
	real32 indirect_scale;
	real32 prt_scale;
	real32 surface_light_scale;
	real32 scenario_light_scale;
	real32 lightprobe_interpolation_overide;

	int8 pad0[72];
	tag_block<structure_lightmap_group> lightmap_groups;

	int8 pad1[12];
	tag_block<error_report_category> errors;

	int8 pad2[104];
};
ASSERT_STRUCT_SIZE(structure_lightmap, 260);
