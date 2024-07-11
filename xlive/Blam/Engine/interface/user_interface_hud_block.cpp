#include "stdafx.h"

#include "user_interface_hud_block.h"



// c_hud_widget virtual functions

c_hud_widget::~c_hud_widget()
{
	//return INVOKE_TYPE(0x240FF8, 0x0, c_user_interface_widget*(__thiscall*)(c_model_widget*, char), lpMem, a2);
}

void c_hud_widget::update()
{
	INVOKE_TYPE(0x240E26, 0x0, void(__thiscall*)(c_hud_widget*), this);
}

void c_hud_widget::render_widget(rectangle2d* viewport_bounds)
{
	INVOKE_TYPE(0x240E84, 0x0, void(__thiscall*)(c_hud_widget*, rectangle2d*), this, viewport_bounds);
}

int c_hud_widget::get_intro_delay()
{
	return INVOKE_TYPE(0x240E15, 0x0, int(__thiscall*)(c_hud_widget*), this);
}

c_user_interface_text* c_hud_widget::get_interface()
{
	//return INVOKE_TYPE(0x240E12, 0x0, c_user_interface_text*(__thiscall*)(c_hud_widget*), this);
	return nullptr;
}
