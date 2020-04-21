#pragma once
#pragma pack(push,1)
#include "object_definition.hpp"
/*********************************************************************
* name: device
* group_tag : devi
* header size : 284
* *********************************************************************/
struct s_device_group_definition :TagGroup<'devi'>
{
	s_object_group_definition objectTag;
	enum class e_flags : __int32
	{
		position_loops = FLAG(0),
		unused = FLAG(1),
		allow_interpolation = FLAG(2),
	};
	e_flags flags;//0xBC
	float power_transition_time;//0xC0
	float power_acceleration_time;//0xC4
	float position_transition_time;//0xC8
	float position_acceleration_time;//0xCC
	float depowered_position_transition_time;//0xD0
	float depowered_position_acceleration_time;//0xD4
	enum class e_lightmap_flags : __int16
	{
		dont_use_in_lightmap = FLAG(0),
		dont_use_in_lightprobe = FLAG(1),
	};
	e_lightmap_flags lightmap_flags;//0xD8
	PAD(0x2);//0xDA
	tag_reference open_up;//0xDC
	tag_reference close_down;//0xE4
	tag_reference opened;//0xEC
	tag_reference closed;//0xF4
	tag_reference depowered;//0xFC
	tag_reference repowered;//0x104
	float delay_time;//0x10C
	tag_reference delay_effect;//0x110
	float automatic_activation_radius;//0x118
};
TAG_GROUP_SIZE_ASSERT(s_device_group_definition, 0x11C);

#pragma pack(pop)

