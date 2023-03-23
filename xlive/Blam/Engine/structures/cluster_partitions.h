#pragma once
#include "Blam/Engine/DataArray/DataArray.h"
#include "Blam/Engine/math/real_math.h"
#include "Blam/Engine/scenario/scenario.h"

struct s_game_cluster_bit_vectors
{
	int cluster_bitvector;
};

struct cluster_partition
{
	int* cluster_references;
	s_data_array* array0;
	s_data_array* array1;
};

void cluster_partition_reconnect(cluster_partition* partition,
	const datum object_datum,
	int* first_cluster_reference,
	const real_point3d* position,
	const float radius,
	const s_location* location,
	const s_game_cluster_bit_vectors* cluster_bitvector,
	const int payload_size,
	const void* payload,
	bool* cluster_overflow);
