#pragma once

struct s_user_interface_guide_state_manager_string
{
	int field_0;
	wchar_t string[24];
	int field_34;
	int field_38;
};
CHECK_STRUCT_SIZE(s_user_interface_guide_state_manager_string, 60);

// Not 100% on the size yet
#pragma pack(push, 1)
class c_user_interface_guide_state_manager
{
public:
	HANDLE m_xnotify_listener;
	bool m_unk_bool_4;
	char m_pad_5[3];
	XUSER_SIGNIN_STATE m_sign_in_state;
	char m_field_C;
	bool m_unk_bool_D;
	char m_pad_E;
	bool m_unk_bool_F;
	bool m_strings_initialized;
	XSESSION_INFO m_xsession_info;
	bool m_from_game_invite;
	char m_username[16];
	char m_gap_10[1166];
	s_user_interface_guide_state_manager_string m_strings[4];
	char m_gap_5DC[12];
	char m_field_5E8[28];
	int m_field_604;
	char m_field_608;
	bool m_unk_bool_609;
	char m_pad_609[6];
};
#pragma pack(pop)
CHECK_STRUCT_SIZE(c_user_interface_guide_state_manager, 1552);

c_user_interface_guide_state_manager* user_interface_guide_state_manager_get(void);
