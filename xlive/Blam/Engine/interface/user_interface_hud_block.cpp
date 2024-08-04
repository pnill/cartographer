#include "stdafx.h"

#include "user_interface_hud_block.h"



// c_hud_widget virtual functions

c_user_interface_widget* c_hud_widget::destructor(uint32 flags)
{
	this->~c_hud_widget();
	if (TEST_BIT(flags, 0))
	{
	}

	return this;
}

void c_hud_widget::update()
{
	INVOKE_TYPE(0x240E26, 0x0, void(__thiscall*)(c_hud_widget*), this);
}

void c_hud_widget::render_widget(rectangle2d* viewport_bounds)
{
	INVOKE_TYPE(0x240E84, 0x0, void(__thiscall*)(c_hud_widget*, rectangle2d*), this, viewport_bounds);
}

int32 c_hud_widget::get_intro_delay()
{
	return INVOKE_TYPE(0x240E15, 0x0, int32(__thiscall*)(c_hud_widget*), this);
}

c_user_interface_text* c_hud_widget::get_interface()
{
	//return INVOKE_TYPE(0x240E12, 0x0, c_user_interface_text*(__thiscall*)(c_hud_widget*), this);
	return nullptr;
}
