#include "stdafx.h"
#include "user_interface_widget_text_entry.h"

c_text_entry_widget::c_text_entry_widget(int16 button_index, int16 user_flags) :
	c_button_widget(button_index, user_flags)
{
	field_544 = 0;
}

c_text_entry_widget::~c_text_entry_widget()
{
}

void c_text_entry_widget::render_widget(rectangle2d* viewport_bounds)
{
	INVOKE_TYPE(0x230EB7, 0x0, void(__thiscall*)(c_text_entry_widget*, rectangle2d*), this, viewport_bounds);
}

bool c_text_entry_widget::handle_event(s_event_record* event)
{
	return INVOKE_TYPE(0x232202, 0x0, bool(__thiscall*)(c_text_entry_widget*, s_event_record*), this, event);
}

c_user_interface_text* c_text_entry_widget::get_interface()
{
	//return INVOKE_TYPE(0x2237E4, 0x0, c_user_interface_text * (__thiscall*)(c_text_entry_widget*), this);
	return &this->text_interface;
}

void c_text_entry_widget::setup_texts()
{
	INVOKE_TYPE(0x230DFE, 0x0, void(__thiscall*)(c_text_entry_widget*), this);
}
