#include "stdafx.h"
#include "cluster_partitions.h"

#include "Blam/Engine/objects/reference_lists.h"

#define MAXIMUM_CLUSTERS_PER_DATUM 256

void __cdecl cluster_partition_reconnect(
	cluster_partition* partition,
	const datum object_datum,
	int* first_cluster_reference,
	const real_point3d* position,
	const float radius,
	const s_location* location,
	const s_game_cluster_bit_vectors* cluster_bitvector,
	const int payload_size,
	const void* payload,
	bool* cluster_overflow)
{
	short clusters_in_sphere;
	short intersected_references;
	short cluster_reference[MAXIMUM_CLUSTERS_PER_DATUM];

	if (cluster_bitvector)
	{
		clusters_in_sphere = structure_clusters_in_bitvector(cluster_bitvector, MAXIMUM_CLUSTERS_PER_DATUM, cluster_reference, &intersected_references);
	}
	else
	{
		clusters_in_sphere = structure_clusters_in_sphere(location->cluster, position, radius, MAXIMUM_CLUSTERS_PER_DATUM, cluster_reference, &intersected_references);
	}

	*cluster_overflow = intersected_references > clusters_in_sphere;
	
	
	for (DWORD i = 0; i < clusters_in_sphere; i++)
	{
		short current_cluster_reference = cluster_reference[i];
		reference_list_add(partition->array1, first_cluster_reference, current_cluster_reference, 0, nullptr);
		reference_list_add(partition->array0, &partition->cluster_references[current_cluster_reference], object_datum, payload_size, payload);
	}
}
