#pragma once
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
/* gets the current loaded user_interface_globals tag aka wgtz depending on the scenario type */
s_user_interface_globals_group_definition *get_user_interface_globals_ptr()
{
	//dives into globals/globals tag and gets the block "Interface Tags"
	//from there checks current scnr type and itself returns a pointer for the correct one
	typedef s_user_interface_globals_group_definition*(_cdecl matg_interface_tags_get_user_interface_globals)();
	auto pmatg_interface_tags_get_user_interface_globals = h2mod->GetAddress<matg_interface_tags_get_user_interface_globals*>(0x20BB1D, 0x1F2C5A);
	return pmatg_interface_tags_get_user_interface_globals();
}