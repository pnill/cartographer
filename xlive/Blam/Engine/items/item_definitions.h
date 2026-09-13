#pragma once
#include "objects/object_definition.h"

/* constants */

enum
{
	ITEM_DEFINITION_TAG = 'item',
	ITEM_DEFINITION_VERSION = 2,				// TODO: verify
	ITEM_COLLECTION_DEFINITION_TAG = 'itmc',
	ITEM_COLLECTION_DEFINITION_VERSION = 0,		// TODO: verify
};

/* macros */

#define item_definition_get(index)				((struct item_definition*)tag_get(ITEM_DEFINITION_TAG, (index)))
#define item_collection_definition_get(index)	((struct item_collection_definition*)tag_get(ITEM_COLLECTION_DEFINITION_TAG, (index)))

/* enums */

enum e_item_definition_flags : uint32
{
	_item_definition_always_maintains_z_up = FLAG(0),
	_item_definition_destroyed_by_explosions = FLAG(1),
	_item_definition_unaffected_by_gravity = FLAG(2)
};

/* structures */

struct _item_definition
{
	// Explaination("$$$ ITEM $$$", "")

	e_item_definition_flags flags;
	int16 old_message_index;
	int16 sort_order;
	real32 multiplayer_onground_scale;
	real32 campaign_onground_scale;

	// Explaination("NEW hud messages", "everything you need to display stuff")

	string_id pickup_message;
	string_id swap_message;
	string_id pickup_or_dual_msg;
	string_id swap_or_dual_msg;
	string_id dualonly_msg;
	string_id picked_up_msg;
	string_id singluar_quantity_msg;
	string_id plural_quantity_msg;
	string_id switchto_msg;
	string_id switchto_from_ai_msg;
	tag_reference unused;           // foot
	tag_reference collision_sound;  // snd!

	// max count: 8
	s_tag_block predicted_bitmaps;				// tag_reference (bitm)

	tag_reference detonation_damage_effect;     // jpt!
	real_vector2d detonation_delay_seconds;
	tag_reference detonating_effect;            // effe
	tag_reference detonation_effect;            // effe
};

struct item_definition
{
	_object_definition object;
	_item_definition item;
};
ASSERT_STRUCT_SIZE(item_definition, 300);

// max count: 32
struct item_permutation_definition
{
	real32 weight;	// relatively how likely this item will be chosen

	// which item to 
	tag_reference item;	// item
	string_id variant_name;
};
ASSERT_STRUCT_SIZE(item_permutation_definition, 16);

struct item_collection_definition
{
	s_tag_block item_permutations;	// item_permutation_definition
	int16 spawn_time;
	int16 pad;
};
ASSERT_STRUCT_SIZE(item_collection_definition, 12);
