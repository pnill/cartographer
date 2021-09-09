#pragma once
#include "Blam\Cache\DataTypes\BlamDataTypes.h"

/*********************************************************************
* name: item_collection
* group_tag : itmc
* header size : 12
* *********************************************************************/
struct s_item_collection_group_definition
{
	struct s_item_permutations_block
	{
		float weight;//0x0
		tag_reference item;//0x4
		string_id variant_name;//0xC
	};
	TAG_BLOCK_SIZE_ASSERT(s_item_permutations_block, 0x10);
	tag_block<s_item_permutations_block> item_permutations;//0x0
	__int16 spawn_time_in_seconds;//0x8
	PAD(0x2);//0xA
};
TAG_GROUP_SIZE_ASSERT(s_item_collection_group_definition, 0xC);
