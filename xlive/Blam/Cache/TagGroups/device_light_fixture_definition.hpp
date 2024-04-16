#pragma once
#include "device_definition.hpp"

/*********************************************************************
* name: device_light_fixture
* group_tag : lifi
* header size : 284
* *********************************************************************/

#pragma pack(push,1)
struct s_device_light_fixture_group_definition
{
	s_device_group_definition deviceTag;
};
ASSERT_STRUCT_SIZE(s_device_light_fixture_group_definition, 0x11C);
#pragma pack(pop)

