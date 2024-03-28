#pragma once
#include "geometry/geometry_block.h"

#define k_max_cluster_data_size 34560
#define k_max_pca_per_vertex_data_size 150405120

// max count: MAXIMUM_SECTIONS_PER_RENDER_MODEL
struct prt_section_info
{
    uint32 section_index;
    uint32 pca_data_offset;
};
TAG_BLOCK_SIZE_ASSERT(prt_section_info, 8);

// max count: NUMBER_OF_GEOMETRY_DETAIL_LEVELS
struct prt_lod_info
{
    int32 cluster_offset;
    tag_block<prt_section_info> sectionInfo;
};
TAG_BLOCK_SIZE_ASSERT(prt_lod_info, 12);

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
    tag_block<prt_lod_info> lod_info;

    tag_block<real32> cluster_basis;    // max count: k_max_cluster_data_size
    tag_block<real32> rawpcadata;       // max count: k_max_pca_per_vertex_data_size
    tag_block<rasterizer_vertex_buffer> vertex_buffers; // max count: MAXIMUM_SECTIONS_PER_RENDER_MODEL
    geometry_block_info geometry_block_info;
};
CHECK_STRUCT_SIZE(prt_info, 88);
