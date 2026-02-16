#pragma once
#include "tag_files/tag_block.h"
#include "tag_files/tag_reference.h"

// max count: 32
struct item_permutation_definition
{
	real32 weight;	// relatively how likely this item will be chosen
	
	// which item to 
	tag_reference item;	// item
	string_id variant_name;
};
ASSERT_STRUCT_SIZE(item_permutation_definition, 16);

struct item_collection_definition
{
	tag_block<item_permutation_definition> item_permutations;
	int16 spawn_time;
	int16 pad;
};
ASSERT_STRUCT_SIZE(item_collection_definition, 12);
