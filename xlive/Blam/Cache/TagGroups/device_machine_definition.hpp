#pragma once
#pragma pack(push,1)
#include "device_definition.hpp"
/*********************************************************************
* name: device_machine
* group_tag : mach
* header size : 308
* *********************************************************************/
struct s_device_machine_group_definition :TagGroup<'mach'>
{
	s_device_group_definition deviceTag;
	enum class e_type : __int16
	{
		door = 0,
		platform = 1,
		gear = 2,
	};
	e_type type;//0x11C
	enum class e_flags : __int16
	{
		pathfinding_obstacle = FLAG(0),
		but_not_when_open = FLAG(1),
		elevator = FLAG(2),
	};
	e_flags flags;//0x11E
	float door_open_time;//0x120
	real_bounds door_occlusion_bound;//0x124

	enum class e_collision_response : __int16
	{
		pause_until_crushed = 0,
		reverse_directions = 1,
	};
	e_collision_response collision_response;//0x12C
	__int16 elevator_node;//0x12E
	enum class e_pathfinding_policy : __int16
	{
		discs = 0,
		sectors = 1,
		cut_out = 2,
		none = 3,
	};
	e_pathfinding_policy pathfinding_policy;//0x130
	PAD(0x2);//0x132
};
TAG_GROUP_SIZE_ASSERT(s_device_machine_group_definition, 0x134);

#pragma pack(pop)

