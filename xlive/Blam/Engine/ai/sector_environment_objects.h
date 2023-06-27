#pragma once
#include "Blam/Cache/DataTypes/TagBlock.h"
#include "Blam/Common/Common.h"

#define k_maximum_environment_objects_per_scenario 2000		// This is different from the MAXIMUM_ENVIRONMENT_OBJECTS_PER_SCENARIO define

enum e_environment_object_reference_flags : short
{
    environment_object_reference_flag_mobile = FLAG(0)
};

// max count: MAXIMUM_BSPS_PER_COLLISION_REGION*MAXIMUM_REGIONS_PER_MODEL 1024
struct environment_object_bsp_reference
{
    int bsp_reference;
    int first_sector;
    int last_sector;
    short nodeindex;
    short pad;
};
TAG_BLOCK_SIZE_ASSERT(environment_object_bsp_reference, 16);

enum e_projection_sign_flag : byte
{
    projection_sign_flag_projection_sign = FLAG(0)
};

// max count: MAXIMUM_NODES_PER_MODEL 255
struct environment_object_node_reference
{
    short reference_frame_index;
    byte projection_axis;
    e_projection_sign_flag projection_sign;
};
TAG_BLOCK_SIZE_ASSERT(environment_object_node_reference, 4);

// max count: k_maximum_environment_objects_per_scenario 2000
struct environment_object_reference
{
    e_environment_object_reference_flags flags;
    short pad;
    int first_sector;
    int last_sector;

    tag_block<environment_object_bsp_reference> bsps;
    tag_block<environment_object_node_reference> nodes;
};
TAG_BLOCK_SIZE_ASSERT(environment_object_reference, 28);
