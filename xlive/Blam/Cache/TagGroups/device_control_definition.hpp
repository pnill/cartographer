#pragma once
#pragma pack(push,1)
#include "device_definition.hpp"
/*********************************************************************
* name: device_control
* group_tag : ctrl
* header size : 320
* *********************************************************************/
struct s_device_control_group_definition :TagGroup<'ctrl'>
{
	s_device_group_definition deviceTag;
	enum class e_type : __int16
	{
		toggle_switch = 0,
		on_button = 1,
		off_button = 2,
		call_button = 3,
	};
	e_type type;//0x11C
	enum class e_triggers_when : __int16
	{
		touched_by_player = 0,
		destroyed = 1,
	};
	e_triggers_when triggers_when;//0x11E
	float call_value;//0x120
	string_id action_string;//0x124
	tag_reference on;//0x128
	tag_reference off;//0x130
	tag_reference deny;//0x138
};
TAG_GROUP_SIZE_ASSERT(s_device_control_group_definition, 0x140);

#pragma pack(pop)


