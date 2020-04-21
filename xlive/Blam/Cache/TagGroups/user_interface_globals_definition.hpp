#pragma once
#ifndef WGTZ_H
#define WGTZ_H
#pragma pack(push,1)
#include "..\Blam\Cache\DataTypes\DataTypes.h"
#include "..\Blam\Cache\TagGroups.hpp"
#include "..\Blam\Cache\DataTypes\TagBlock.h"
/*********************************************************************
* name: user_interface_globals
* group_tag : wgtz
* header size : 32
* *********************************************************************/
struct s_user_interface_globals_group_definition :TagGroup<'wgtz'>
{
	tag_reference shared_globals;//0x0
	struct s_screen_widgets_block
	{
		tag_reference widget_tag;//0x0
	};
	TAG_BLOCK_SIZE_ASSERT(s_screen_widgets_block, 0x8);
	tag_block<s_screen_widgets_block> screen_widgets;//0x8
	tag_reference mp_variant_settings_ui;//0x10
	tag_reference game_hopper_descriptions;//0x18
};
TAG_GROUP_SIZE_ASSERT(s_user_interface_globals_group_definition, 0x20);

#pragma pack(pop)
#endif
