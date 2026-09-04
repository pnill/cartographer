#pragma once

/* constants */

enum
{
	HUD_MESSAGE_TEXT_TAG = 'hmt ',
};

/* macros */

#define hud_state_messages_get(index)	((struct hud_state_messages*)tag_get(HUD_MESSAGE_TEXT_TAG, (index)))

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
	tag_data text_data;
	s_tag_block message_elements;	// hud_state_message_element
	s_tag_block messages;			// hud_state_message_definition
	int8 pad[84];
};
ASSERT_STRUCT_SIZE(hud_state_messages, 108);

/* public code */

void hud_messaging_apply_hooks(void);

void __cdecl hud_messaging_update(int32 user_index);

void __cdecl hud_messaging_clear(void);

void __cdecl hud_messaging_post(int32 user_index, string_id string_id);
