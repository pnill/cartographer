#pragma once
#include "tag_files/tag_block.h"

/* constants */

#define MAXIMUM_CLUSTERS_PER_STRUCTURE 512
#define MAXIMUM_CLUSTER_PORTALS_PER_STRUCTURE 512

/* structures */

// max: MAXIMUM_CLUSTERS_PER_STRUCTURE
struct s_structure_sound_cluster
{
	int32 pad;
	// max: MAXIMUM_CLUSTER_PORTALS_PER_STRUCTURE
	tag_block<uint16> enclosing_portal_designators;
	// max: MAXIMUM_CLUSTERS_PER_STRUCTURE
	tag_block<uint16> interior_cluster_indices;
};
