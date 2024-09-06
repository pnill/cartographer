#pragma once
#include "objects/object_identifier.h"
#include "render/render_lights.h"
#include "tag_files/tag_reference.h"

#define MAXIMUM_LIGHTS_PER_MAP 350
#define MAXIMUM_LIGHTMAP_GROUPS_PER_STRUCTURE 128
#define k_maximum_fake_lightprobes_per_structure_bsp 128

enum e_structure_precomputed_lighting_type : uint16
{
	_structure_precomputed_lighting_free_standing = 0,
	_structure_precomputed_lighting_attached_to_editor_object = 1,
	_structure_precomputed_lighting_attached_to_structure_object = 2
};

struct structure_precomputed_lighting_visiblity
{
	uint16 projection_count;
	uint16 cluster_count;
	uint16 volumes_count;
	int16 pad;

	/****************************************
	* definition_name: projection_tag_data_definition
	* flags: 0
	* alignment_bit: 0
	****************************************/
	// DataSize(2664)
	data_reference projections;
	/****************************************
	* definition_name: visibility_clusters_tag_data_definition
	* flags: 0
	* alignment_bit: 0
	****************************************/
	// DataSize(3328)
	data_reference visibility_clusters;
	/****************************************
	* definition_name: cluster_remap_table_tag_data_definition
	* flags: 0
	* alignment_bit: 0
	****************************************/
	// DataSize(512)
	data_reference cluster_remap_table;
	/****************************************
	* definition_name: visibility_volumes_table_tag_data_definition
	* flags: 0
	* alignment_bit: 0
	****************************************/
	// DataSize(135168)
	data_reference visibility_volumes;
};
ASSERT_STRUCT_SIZE(structure_precomputed_lighting_visiblity, 40);

// max: MAXIMUM_LIGHTS_PER_MAP
struct structure_precomputed_lighting
{
	uint32 index;
	e_structure_precomputed_lighting_type type;
	uint8 attachment_index;
	uint8 object_type;
	structure_precomputed_lighting_visiblity visiblity;
};
ASSERT_STRUCT_SIZE(structure_precomputed_lighting, 48);

// max: MAXIMUM_LIGHTMAP_GROUPS_PER_STRUCTURE
struct structure_lightmap_data
{
	tag_reference bitmap_group; // bitm
};
ASSERT_STRUCT_SIZE(structure_lightmap_data, 8);

// max: k_maximum_fake_lightprobes_per_structure_bsp
struct s_object_fake_lightprobe
{
	c_object_identifier object_identifier;
	render_lighting render_lighting;
};
ASSERT_STRUCT_SIZE(s_object_fake_lightprobe, 92);