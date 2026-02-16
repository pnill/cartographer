#pragma once
#include "tag_files/data_reference.h"
#include "tag_files/tag_block.h"

/* structures */

struct hud_state_message_element
{
	uint8 type;
	uint8 data;
};

struct hud_state_message_definition
{
	char name[k_tag_string_length];
	int16 start_index_into_text_blob;
	int16 start_index_of_message_block;
	int8 panel_count;
	int8 pad0[3];

	int8 pad1[24];
};

struct hud_state_messages
{
	data_reference text_data;
	tag_block<hud_state_message_element> message_elements;
	tag_block<hud_state_message_definition> messages;
	int8 pad[84];
};
ASSERT_STRUCT_SIZE(hud_state_messages, 108);

/* public code */

void hud_messaging_apply_hooks(void);

void __cdecl hud_messaging_update(int32 user_index);

void __cdecl hud_messaging_clear(void);

void __cdecl hud_messaging_post(int32 user_index, string_id string_id);
