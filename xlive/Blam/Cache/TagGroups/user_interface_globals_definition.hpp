#pragma once
#include "Blam/Cache/TagGroups.hpp"
#include "Blam/Cache/DataTypes/BlamDataTypes.h"
#include "Util/Memory.h"

#pragma pack(push,1)
struct user_interface_globals_definition :TagGroup<'wgtz'>
{
	tag_reference shared_globals;
	struct s_screen_widgets_block
	{
		tag_reference widget_tag;
	};
	TAG_BLOCK_SIZE_ASSERT(s_screen_widgets_block, 0x8);
	tag_block<s_screen_widgets_block> screen_widgets;
	tag_reference mp_variant_settings_ui;
	tag_reference game_hopper_descriptions;
};
TAG_GROUP_SIZE_ASSERT(user_interface_globals_definition, 0x20);
#pragma pack(pop)

/* gets the current loaded user_interface_globals tag aka wgtz depending on the scenario type */
user_interface_globals_definition *get_user_interface_globals_ptr()
{
	//dives into globals/globals tag and gets the block "Interface Tags"
	//from there checks current scnr type and itself returns a pointer for the correct one
	typedef user_interface_globals_definition*(_cdecl* get_user_interface_globals_t)();
	get_user_interface_globals_t get_user_interface_globals = Memory::GetAddress<get_user_interface_globals_t*>(0x20BB1D, 0x1F2C5A);
	return get_user_interface_globals();
}
