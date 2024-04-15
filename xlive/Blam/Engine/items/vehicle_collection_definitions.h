#pragma once
#include "tag_files/string_id.h"
#include "tag_files/tag_block.h"
#include "tag_files/tag_reference.h"

// max: 32
struct vehicle_permutation_definition
{
	/// relatively how likely this vehicle will be chosen
	real32 weight;
	tag_reference vehicle; // effe
	string_id variant_name;
};
TAG_BLOCK_SIZE_ASSERT(vehicle_permutation_definition, 16);

struct vehicle_collection_definition
{
	tag_block<vehicle_permutation_definition> vehicles;
	/// in seconds. 0 sets to default
	uint16 spawn_time;
	uint16 pad_1;
};
TAG_GROUP_SIZE_ASSERT(vehicle_collection_definition, 12);