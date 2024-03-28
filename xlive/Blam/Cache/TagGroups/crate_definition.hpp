#pragma once
#include "objects/object_definition.h"


/*********************************************************************
* name: crate
* group_tag : bloc
* header size : 192
* *********************************************************************/

#pragma pack(push,1)
struct s_crate_group_definition
{
	object_definition objectTag;
	enum class e_flags : __int16
	{
		does_not_block_aoe = FLAG(0),
	};
	e_flags flags;//0xBC
	PAD(0x2);//0xBE
};
TAG_GROUP_SIZE_ASSERT(s_crate_group_definition, 0xC0);
#pragma pack(pop)
