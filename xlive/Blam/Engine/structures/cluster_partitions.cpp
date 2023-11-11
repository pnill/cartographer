#include "stdafx.h"
#include "cluster_partitions.h"

void __cdecl cluster_partition_reconnect(cluster_partition* partition,
	datum object_datum,
	int32* first_cluster_reference,
	const real_point3d* position,
	real32 radius,
	const s_location* location,
	const s_game_cluster_bit_vectors* cluster_bitvector,
	int32 payload_size,
	const void* payload,
	bool* cluster_overflow)
{
	INVOKE(0x37A13E, 0x324EAE, cluster_partition_reconnect, partition, object_datum, first_cluster_reference, position, radius, location, cluster_bitvector, payload_size, payload, cluster_overflow);
	return;
}

cluster_partition* collideable_object_cluster_partition_get(void)
{
	return Memory::GetAddress<cluster_partition*>(0x4E4604, 0x50C8D4);
}

cluster_partition* noncollideable_object_cluster_partition_get(void)
{
	return Memory::GetAddress<cluster_partition*>(0x4E45F8, 0x50C8C8);
}