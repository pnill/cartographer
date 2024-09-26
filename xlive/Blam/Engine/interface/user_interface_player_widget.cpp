#include "stdafx.h"

#include "user_interface_player_widget.h"

// c_player_widget virtual functions


int32 c_player_widget::setup_children()
{
	//return INVOKE_TYPE(0x220441, 0x0, int32(__thiscall*)(c_player_widget*), this);
	int result = c_user_interface_widget::setup_children();
	this->m_visible = false;
	return result;
}


c_player_widget_representation::c_player_widget_representation()
{
	m_flags = 0;
}

void c_player_widget_representation::set_player_name_from_configuration(s_player_properties* configuration)
{
	INVOKE_TYPE(0x2205BD, 0x0, void(__thiscall*)(c_player_widget_representation*, s_player_properties*), this, configuration);
}

void c_player_widget_representation::set_player_rank(int32 rank)
{
	INVOKE_TYPE(0x220A12, 0x0, void(__thiscall*)(c_player_widget_representation*, int32), this, rank);
}

void c_player_widget_representation::set_player_team(e_game_team team)
{
	INVOKE_TYPE(0x2205F8, 0x0, void(__thiscall*)(c_player_widget_representation*, e_game_team), this, team);
}

void c_player_widget_representation::set_player_team_name(string_id team_name)
{
	INVOKE_TYPE(0x2205EB, 0x0, void(__thiscall*)(c_player_widget_representation*, string_id), this, team_name);
}

void c_player_widget_representation::set_player_is_observer(bool observer)
{
	INVOKE_TYPE(0x220607, 0x0, void(__thiscall*)(c_player_widget_representation*, bool), this, observer);
}

void c_player_widget_representation::set_fake_player(bool fake)
{
	INVOKE_TYPE(0x220617, 0x0, void(__thiscall*)(c_player_widget_representation*, bool), this, fake);
}

void c_player_widget_representation::set_user_role(int32 role)
{
	INVOKE_TYPE(0x220624, 0x0, void(__thiscall*)(c_player_widget_representation*, int32), this, role);
}

void c_player_widget_representation::set_appearance(s_player_profile_traits* appearance)
{
	INVOKE_TYPE(0x2205CA, 0x0, void(__thiscall*)(c_player_widget_representation*, s_player_profile_traits*), this, appearance);
}
