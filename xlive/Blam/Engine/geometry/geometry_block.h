#pragma once
#include "tag_files/tag_block.h"

#define MAXIMUM_RESOURCES_PER_GEOMETRY_BLOCK 1024

enum e_geometry_resource_type : char
{
    geometry_resource_type_tag_block = 0,
    geometry_resource_type_tag_data = 1,
    geometry_resource_type_vertex_buffer = 2
};

// max count: MAXIMUM_RESOURCES_PER_GEOMETRY_BLOCK 1024
struct geometry_block_resource
{
    e_geometry_resource_type type;
    char pad[3];
    short primary_locator;
    short secondary_locator;
    int resource_data_size;
    int resource_data_offset;
};
TAG_BLOCK_SIZE_ASSERT(geometry_block_resource, 16);

// max count: 1
struct geometry_block_info
{
    // Explaination("BLOCK INFO", "EMPTY STRING")
    int block_offset;
    int block_size;
    int section_data_size;
    int resource_data_size;
    tag_block<geometry_block_resource> resources;

    int pad;
    short owner_tag_section_offset;
    short pad1;
    int pad2;
};
CHECK_STRUCT_SIZE(geometry_block_info, 36);

// max count: 64*k_kilo 65536
struct indices_block
{
    short index;
};
TAG_BLOCK_SIZE_ASSERT(indices_block, 2);
