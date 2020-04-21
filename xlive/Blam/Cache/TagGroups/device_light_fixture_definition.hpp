#pragma once
#pragma pack(push,1)
#include "device_definition.hpp"
/*********************************************************************
* name: device_light_fixture
* group_tag : lifi
* header size : 284
* *********************************************************************/
struct s_device_light_fixture_group_definition :TagGroup<'lifi'>
{
	s_device_group_definition deviceTag;
};
TAG_GROUP_SIZE_ASSERT(s_device_light_fixture_group_definition, 0x11C);
#pragma pack(pop)

