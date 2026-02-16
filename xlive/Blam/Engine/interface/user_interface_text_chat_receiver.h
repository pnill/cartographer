#pragma once
#include "user_interface_text_chat.h"

/* classes */

#pragma pack(push,1)
class c_user_interface_text_chat_receiver
{
	int32 m_line_no;
	int32 field_4;
	int32 field_8;
	int32 m_message_time;
	wchar_t* m_message_line[30];
	int16 field_88;
	int8 gap_8A[7538];
	string_id m_vote_error_message;
	void* raw_string_ptr;
	bool m_valid;

public:
	string_id get_vote_error_message();
	void post_incoming_messages();
	void post_vote_error(const wchar_t* Source);
	void add_chat_message(s_text_chat_message* message);
};
#pragma pack(pop)
ASSERT_STRUCT_SIZE(c_user_interface_text_chat_receiver, 0x1E05);