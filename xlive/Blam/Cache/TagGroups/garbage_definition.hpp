#pragma once
#include "item_definition.hpp"

/*********************************************************************
* name: garbage
* group_tag : garb
* header size : 468
* *********************************************************************/

#pragma pack(push,1)
struct s_garbage_group_definition
{
	s_item_group_definition itemTag;
	PAD(0xA8);//0x12C
};
TAG_GROUP_SIZE_ASSERT(s_garbage_group_definition, 0x1D4);
#pragma pack(pop)


