#pragma once
#include "Blam\Cache\DataTypes\BlamDataTypes.h"

/*********************************************************************
* name: vehicle_collection
* group_tag : vehc
* header size : 12
* *********************************************************************/
struct s_vehicle_collection_group_definition
{
	struct s_vehicle_permutations_block
	{
		float weight;//0x0
		tag_reference vehicle;//0x4
		string_id variant_name;//0xC
	};
	TAG_BLOCK_SIZE_ASSERT(s_vehicle_permutations_block, 0x10);
	tag_block<s_vehicle_permutations_block> vehicle_permutations;//0x0
	__int16 spawn_time_in_seconds;//0x8
	PAD(0x2);//0xA
};
TAG_GROUP_SIZE_ASSERT(s_vehicle_collection_group_definition, 0xC);
