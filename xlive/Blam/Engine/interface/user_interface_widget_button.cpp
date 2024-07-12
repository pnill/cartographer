#include "stdafx.h"

#include "user_interface_widget_button.h"

c_button_widget::c_button_widget(int16 button_index, int16 user_flags):
	c_user_interface_widget(_widget_type_button,user_flags)
{
	m_flags = 0;
	m_button_index = button_index;
	signal = nullptr;
}


// c_button_widget virtual functions

c_button_widget::~c_button_widget()
{

}

void c_button_widget::update()
{
	INVOKE_TYPE(0x2400E9, 0x0, void(__thiscall*)(c_button_widget*), this);
}

void c_button_widget::render_widget(rectangle2d* viewport_bounds)
{
	INVOKE_TYPE(0x23F9B7, 0x0, void(__thiscall*)(c_button_widget*, rectangle2d*), this, viewport_bounds);
}

void* c_button_widget::get_mouse_region(rectangle2d* mouse_region_out)
{
	return INVOKE_TYPE(0x23FFF8, 0x0, void*(__thiscall*)(c_button_widget*, rectangle2d*), this, mouse_region_out);
}

int32 c_button_widget::get_intro_delay()
{
	return INVOKE_TYPE(0x23FCE5, 0x0, int32(__thiscall*)(c_button_widget*), this);
}

bool c_button_widget::handle_event(s_event_record* event)
{
	return INVOKE_TYPE(0x23FCFF, 0x0, bool(__thiscall*)(c_button_widget*, s_event_record*), this, event);
}

bool c_button_widget::can_interact()
{
	//return INVOKE_TYPE(0x23F753, 0x0, bool(__thiscall*)(c_button_widget*),this);
	return true;
}

c_user_interface_text* c_button_widget::get_interface()
{
	//return INVOKE_TYPE(0x23BC5B, 0x0, c_user_interface_text * (__thiscall*)(c_button_widget*), this);
	return &this->text_interface;
}

void c_button_widget::setup_texts()
{
	INVOKE_TYPE(0x23F85B, 0x0, void(__thiscall*)(c_button_widget*), this);
}

int32 c_button_widget::get_bitmap_active_index()
{
	return INVOKE_TYPE(0x23FF21, 0x0, int32(__thiscall*)(c_button_widget*), this);
}
