#pragma once

#include "Blam/Cache/DataTypes/TagRef.h"
#include "Blam/Cache/DataTypes/TagBlock.h"
#include "Blam/Math/real_math.h"

#define MAXIMUM_CLUSTERS_PER_STRUCTURE 512

struct s_game_cluster_bit_vectors
{
    int32 cluster_bitvector;
};
CHECK_STRUCT_SIZE(s_game_cluster_bit_vectors, 4);

// max count: MAXIMUM_CLUSTERS_PER_STRUCTURE 512
struct s_scenario_cluster_property
{
    short palette_index;
    short pad;
};
TAG_BLOCK_SIZE_ASSERT(s_scenario_cluster_property, 4);

// max count: MAXIMUM_STRUCTURE_BSPS_PER_SCENARIO 16
struct s_scenario_cluster_data
{
    tag_reference bsp; // sbsp
    tag_block<s_scenario_cluster_property> background_sounds;
    tag_block<s_scenario_cluster_property> sound_environments;
    int bsp_checksum;
    tag_block<real_point3d> cluster_centroids;
    tag_block<s_scenario_cluster_property> weather_properties;
    tag_block<s_scenario_cluster_property> atmospheric_fog_properties;
};
TAG_BLOCK_SIZE_ASSERT(s_scenario_cluster_data, 52);