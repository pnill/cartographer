#pragma once
#include "geometry/geometry_block.h"

/* constants */

enum
{
	k_max_cluster_data_size = 34560,
	k_max_pca_per_vertex_data_size = 150405120
};

/* structures */

// max count: MAXIMUM_SECTIONS_PER_RENDER_MODEL
struct prt_section_info
{
	uint32 section_index;
	uint32 pca_data_offset;
};
ASSERT_STRUCT_SIZE(prt_section_info, 8);

// max count: NUMBER_OF_GEOMETRY_DETAIL_LEVELS
struct prt_lod_info
{
	int32 cluster_offset;
	s_tag_block section_info;	// prt_section_info
};
ASSERT_STRUCT_SIZE(prt_lod_info, 12);

struct prt_info
{
	uint16 sh_order;
	uint16 num_of_clusters;
	uint16 pca_vectors_per_cluster;
	uint16 number_of_rays;
	uint16 number_of_bounces;
	uint16 mat_index_for_sbsfc_scattering;
	real32 length_scale;
	uint16 number_of_lods_in_model;
	int16 pad;
	s_tag_block lod_info;			// prt_lod_info

	// max count: k_max_cluster_data_size
	s_tag_block cluster_basis;		// real32

	// max count: k_max_pca_per_vertex_data_size
	s_tag_block rawpcadata;			// real32

	// max count: MAXIMUM_SECTIONS_PER_RENDER_MODEL
	s_tag_block vertex_buffers;		// rasterizer_vertex_buffer

	geometry_block_info geometry_block_info;
};
ASSERT_STRUCT_SIZE(prt_info, 88);

/* prototypes */

void __cdecl render_prt_begin_frame(void);
