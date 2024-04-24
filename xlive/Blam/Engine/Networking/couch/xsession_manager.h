#pragma once

class c_xsession_manager
{
public:
	XSESSION_INFO* get_session(void);
	bool failed(void);
	bool session_valid(void);

	bool m_unk_bool_0;
	int8 m_pad[3];
	HANDLE m_session_handle;
	XSESSION_INFO m_session;
	int8 m_gap_44[12];
	uint32 m_network_time;
	bool m_session_valid;
	bool m_failed;
	int8 m_gap_55[2];
	int32 m_size;
	int32 m_xuid_count_5C;
	XUID m_xuids_60[16];
	uint32 m_updating_player_count;
	XUID m_updating_xuids[16];
	int32 m_xuid_update_state;
	int8 m_gap_16C[4];
	int32 m_xuid_count_170;
	BOOL m_field_174;
	XUID m_xuids_178[16];
	BOOL m_private_slots[16];
	XOVERLAPPED m_overlap;
	uint32 m_state;
};
ASSERT_STRUCT_SIZE(c_xsession_manager, 600);

c_xsession_manager* global_xsession_manager_get(void);
