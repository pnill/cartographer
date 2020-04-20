#pragma once
#ifndef ITEM_H
#define ITEM_H
#pragma pack(push,1)
#include "object_definition.hpp"
/*********************************************************************
* name: item
* group_tag : item
* header size : 300
* *********************************************************************/
struct s_item_group_definition :TagGroup<'item'>
{
	s_object_group_definition objectTag;
	enum class e_flags : __int32
	{
		always_maintains_z_up = FLAG(0),
		destroyed_by_explosions = FLAG(1),
		unaffected_by_gravity = FLAG(2),
	};
	e_flags flags;//0xBC
	__int16 old_message_index;//0xC0
	__int16 sort_order;//0xC2
	float multiplayer_onground_scale;//0xC4
	float campaign_onground_scale;//0xC8
	string_id pickup_message;//0xCC
	string_id swap_message;//0xD0
	string_id pickup_or_dual_msg;//0xD4
	string_id swap_or_dual_msg;//0xD8
	string_id dualonly_msg;//0xDC
	string_id picked_up_msg;//0xE0
	string_id singluar_quantity_msg;//0xE4
	string_id plural_quantity_msg;//0xE8
	string_id switchto_msg;//0xEC
	string_id switchto_from_ai_msg;//0xF0
	tag_reference unused;//0xF4
	tag_reference collision_sound;//0xFC
	struct s_predicted_bitmaps_block
	{
		tag_reference bitmap;//0x0
	};
	TAG_BLOCK_SIZE_ASSERT(s_predicted_bitmaps_block, 0x8);
	tag_block<s_predicted_bitmaps_block> predicted_bitmaps;//0x104
	tag_reference detonation_damage_effect;//0x10C
	real_bounds detonation_delay;//0x114
	
	tag_reference detonating_effect;//0x11C
	tag_reference detonation_effect;//0x124
};
TAG_GROUP_SIZE_ASSERT(s_item_group_definition, 0x12C);

#pragma pack(pop)
#endif

