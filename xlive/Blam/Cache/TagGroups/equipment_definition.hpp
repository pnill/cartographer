#pragma once
#pragma pack(push,1)
#include "item_definition.hpp"
/*********************************************************************
* name: equipment
* group_tag : eqip
* header size : 316
* *********************************************************************/
struct s_equipment_group_definition :TagGroup<'eqip'>
{
	s_item_group_definition itemTag;
	enum class e_powerup_type : __int16
	{
		none = 0,
		double_speed = 1,
		over_shield = 2,
		active_camouflage = 3,
		fullspectrum_vision = 4,
		health = 5,
		grenade = 6,
	};
	e_powerup_type powerup_type;//0x12C
	enum class e_grenade_type : __int16
	{
		human_fragmentation = 0,
		covenant_plasma = 1,
	};
	e_grenade_type grenade_type;//0x12E
	float powerup_time;//0x130
	tag_reference pickup_sound;//0x134
};
TAG_GROUP_SIZE_ASSERT(s_equipment_group_definition, 0x13C);

#pragma pack(pop)

