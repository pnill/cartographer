#include "stdafx.h"

#include "user_interface_model_block.h"

void c_model_widget::apply_appearance_and_character(s_player_profile_traits* appearance, e_character_type character)
{
	INVOKE_TYPE(0x240CEE, 0x0, void(__thiscall*)(c_model_widget*, s_player_profile_traits*, e_character_type), this, appearance, character);
}


// c_model_widget virtual functions

int32 c_model_widget::setup_children()
{
	return INVOKE_TYPE(0x2407A7, 0x0, int32(__thiscall*)(c_model_widget*), this);
}

void c_model_widget::render_widget(rectangle2d* viewport_bounds)
{
	INVOKE_TYPE(0x2407BD, 0x0, void(__thiscall*)(c_model_widget*, rectangle2d*), this, viewport_bounds);
}

int32 c_model_widget::get_intro_delay()
{
	return INVOKE_TYPE(0x240549, 0x0, int32(__thiscall*)(c_model_widget*), this);
}

c_user_interface_text* c_model_widget::get_interface()
{
	//return INVOKE_TYPE(0x240546, 0x0, c_user_interface_text*(__thiscall*)(c_model_widget*), this);
	return nullptr;
}
