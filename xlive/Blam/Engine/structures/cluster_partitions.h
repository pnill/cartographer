#pragma once
#include "tag_files/tag_reference.h"
#include "tag_files/tag_block.h"

#include "memory/data.h"
#include "objects/object_location.h"

/* constants */

enum
{
	MAXIMUM_CLUSTERS_PER_STRUCTURE = 512
};

/* structures */

// max count: MAXIMUM_CLUSTERS_PER_STRUCTURE 512
struct s_scenario_cluster_property
{
	int16 palette_index;
	int16 pad;
};
ASSERT_STRUCT_SIZE(s_scenario_cluster_property, 4);

// max count: MAXIMUM_STRUCTURE_BSPS_PER_SCENARIO 16
struct s_scenario_cluster_data
{
	tag_reference bsp; // sbsp
	tag_block<s_scenario_cluster_property> background_sounds;
	tag_block<s_scenario_cluster_property> sound_environments;
	int32 bsp_checksum;
	tag_block<real_point3d> cluster_centroids;
	tag_block<s_scenario_cluster_property> weather_properties;
	tag_block<s_scenario_cluster_property> atmospheric_fog_properties;
};
ASSERT_STRUCT_SIZE(s_scenario_cluster_data, 52);

struct s_game_cluster_bit_vectors
{
	int32 cluster_bitvector[BIT_VECTOR_SIZE_IN_LONGS(MAXIMUM_CLUSTERS_PER_STRUCTURE)];
};
ASSERT_STRUCT_SIZE(s_game_cluster_bit_vectors, 64);

struct cluster_partition
{
	int32* cluster_references;
	data_array* array0;
	data_array* array1;
};

/* prototypes */

cluster_partition* collideable_object_cluster_partition_get(void);

cluster_partition* noncollideable_object_cluster_partition_get(void);

void __cdecl cluster_partition_make_valid(cluster_partition* partition);

void __cdecl cluster_partition_reconnect(
	cluster_partition* partition,
	datum object_datum,
	int32* first_cluster_reference,
	const real_point3d* position,
	real32 radius,
	const s_location* location,
	const s_game_cluster_bit_vectors* cluster_bitvector,
	int32 payload_size,
	const void* payload,
	bool* cluster_overflow);

