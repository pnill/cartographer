#pragma once

/* structures */

// todo : verify this struct
struct s_panaroma_friends_block
{
	uint32 buffer_size;
	uint8* buffer;
	uint32 result;
};
ASSERT_STRUCT_SIZE(s_panaroma_friends_block, 12);


/* classes */
class c_networking_panaroma_friends
{
protected:
	void* m_current_task;
	PXOVERLAPPED pOverlapped;
	uint32 field_8;
	uint32 field_C;
	uint32 field_10;
	uint32 field_14;
	uint32 field_18;
	uint32 field_1C;
	HANDLE m_enumerator;
	uint32 field_24;
	uint8 gap_28[24];
	s_panaroma_friends_block* m_pending_friends_block;
	s_panaroma_friends_block* field_44;
	bool field_48;
	bool m_op_pending;
	uint8 gap_4A[6];

public:
	bool has_active_task();
	void initialize_startup();
};
ASSERT_STRUCT_SIZE(c_networking_panaroma_friends, 0x50);

c_networking_panaroma_friends* get_networking_panaroma_friends(void);