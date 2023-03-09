#include "stdafx.h"
#include "cluster_partitions.h"

#include "Blam/Engine/objects/reference_lists.h"

#define MAXIMUM_CLUSTERS_PER_DATUM 256

void cluster_partition_reconnect(cluster_partition* partition,
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
	if (clusters_in_sphere > 0)
	{
		for (DWORD i = 0; i < clusters_in_sphere; i++)
		{
			reference_list_add(partition->array1, first_cluster_reference, cluster_reference[i], 0, 0);
			reference_list_add(partition->array0, &partition->cluster_references[cluster_reference[i]], object_datum, payload_size, payload);
		}
	}
}