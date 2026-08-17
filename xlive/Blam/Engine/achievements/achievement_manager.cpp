#include "stdafx.h"
#include "achievement_manager.h"

#include "achievement_live_interface.h"

#include "networking/online/online_account_xbox.h"
#include "networking/panorama/panorama_user_history.h"

/* public code */

void c_achievement_manager::start_upload(void)
{
	INVOKE_TYPE(0x4A808, 0x0, void(__thiscall*)(c_achievement_manager*), this);
	
	return;
}

void c_achievement_manager::submit_event(
	int32* event)
{
	INVOKE_TYPE(0x4B553, 0x0, void(__thiscall*)(c_achievement_manager*, int32*), this, event);
	return;
}

void c_achievement_manager::handle_live_signin_notification(
	bool signed_in)
{
	error(_error_delayed, "achievements: handle_live_signin_notification: %d", signed_in);

	m_signed_in = signed_in;
	m_field_0 = false;
	m_field_1 = false;
	m_live_interface->process_live_signin_state(signed_in);

	return;
}

void c_achievement_manager::start_level_chosen(
	int8 level)
{
	m_signed_in = online_connected_to_xbox_live();
	
	error(_error_delayed, "achievements: start_level_chosen: live: %d", m_signed_in);
	
	if (m_signed_in)
	{
		m_level = level;
		m_achievement = 1;
	}
	else
	{
		m_level = 0;
		m_achievement = 0;
	}
	
	m_field_0 = m_level;
	m_field_1 = m_achievement;
	
	error(_error_delayed, "achievements: set_state: saving checkpoints: %d, earning achievements: %d", m_level, m_achievement);
	
	disable_all_achievements();
	field_AF = true;
	
	return;
}

void c_achievement_manager::disable_all_achievements(void)
{
	INVOKE_TYPE(0x4A6C6, 0x0, void(__thiscall*)(c_achievement_manager*), this);

	return;
}

c_achievement_manager* achievement_manager_get(void)
{
	return *Memory::GetAddress<c_achievement_manager**>(0x482D48);
}

c_panorama_user_history* achievement_history_get(void)
{
	return panorama_user_history_get();
}
