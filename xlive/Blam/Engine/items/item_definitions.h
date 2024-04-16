#pragma once
#include "objects/object_definition.h"

enum e_item_definition_flags : uint32
{
    _item_definition_always_maintains_z_up = FLAG(0),
    _item_definition_destroyed_by_explosions = FLAG(1),
    _item_definition_unaffected_by_gravity = FLAG(2)
};

struct _item_definition
{
    object_definition object;

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
    tag_block<tag_reference> predicted_bitmaps; // bitm

    tag_reference detonation_damage_effect;     // jpt!
    real_vector2d detonation_delay_seconds;
    tag_reference detonating_effect;            // effe
    tag_reference detonation_effect;            // effe
};
ASSERT_STRUCT_SIZE(_item_definition, 300);
