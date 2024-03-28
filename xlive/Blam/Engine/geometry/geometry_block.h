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
    int16 primary_locator;
    int16 secondary_locator;
    int32 resource_data_size;
    int32 resource_data_offset;
};
TAG_BLOCK_SIZE_ASSERT(geometry_block_resource, 16);

// max count: 1
struct geometry_block_info
{
    // Explaination("BLOCK INFO", "EMPTY STRING")
    int32 block_offset;
    int32 block_size;
    int32 section_data_size;
    int32 resource_data_size;
    tag_block<geometry_block_resource> resources;

    datum geometry_tag_index;
    int16 owner_tag_section_offset;
    bool from_geometry_cache;
    int8 pad;

    datum geometry_cache_index;
};
CHECK_STRUCT_SIZE(geometry_block_info, 36);

// max count: 64*k_kilo 65536
struct indices_block
{
    short index;
};
TAG_BLOCK_SIZE_ASSERT(indices_block, 2);
