#include "stdafx.h"

#include "user_interface_group_widget.h"

c_group_widget::c_group_widget(e_user_interface_widget_type widget_type, int16 user_flags):
	c_user_interface_widget(widget_type,user_flags)
{
	//return INVOKE_TYPE(0x242405, 0x0, c_group_widget*(__thiscall*)(c_group_widget*, e_user_interface_widget_type, int16), this, widget_type, user_flags);
}


// c_group_widget virtual functions

c_user_interface_widget* c_group_widget::destructor(uint32 flags)
{
	this->~c_group_widget();
	if (TEST_BIT(flags, 0))
	{

	}

	return this;

	//return INVOKE_TYPE(0x242768, 0x0, c_user_interface_widget*(__thiscall*)(c_user_interface_widget*, char), lpMem,a2);
}
c_user_interface_text* c_group_widget::get_interface()
{
	//return INVOKE_TYPE(0x220050, 0x0, c_user_interface_text*(__thiscall*)(c_small_text_widget*), this);
	return nullptr;
}
