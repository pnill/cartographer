#pragma once
#include "tag_files/string_id.h"

/* structures */

struct s_hud_messaging_user
{
	int8 data[544];
	wchar_t dispay_string_buffer[255];
	int8 data2[50];
	int32 primary_string_creation_time;
	wchar_t primary_string_buffer[63];
	bool primary_string_enabled;
	int8 primary_string_queue_index;
	int8 data3[4];
	int32 primary_string_creation_time_2;
	int32 primary_string_duration_time;
};

struct s_hud_messaging_globals
{
	s_hud_messaging_user local_user[4];
	int8 data[40];
};

/* public code */

void hud_messaging_apply_hooks(void);

void __cdecl hud_messaging_update(int32 user_index);

void __cdecl hud_messaging_add_message_plural(int32 user_index, wchar_t* display_string, wchar_t* plural_string, int32 a4);

void __cdecl hud_messaging_add_message(int32 user_index, wchar_t* display_string);

void __cdecl hud_messaging_display_primary_text(int32 user_index, wchar_t* primary_string, int32 display_ticks);

void __cdecl hud_messaging_clear(void);

void __cdecl hud_messaging_post(int32 user_index, string_id string_id);
