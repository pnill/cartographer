#include "stdafx.h"

#include "user_interface_widget_text.h"

c_text_widget::c_text_widget(int16 user_flags) :
	c_user_interface_widget(_widget_type_text, user_flags)
{
	this->intro_animation_delay_ms = 0;
	this->field_74 = 0;
}

c_text_widget::c_text_widget(datum user_index) :
	c_user_interface_widget(_widget_type_text, (user_index == NONE) ? 0 : FLAG(user_index))
{
	this->intro_animation_delay_ms = 0;
	this->field_74 = 0;
}

void c_text_widget::set_text(wchar_t const* text)
{
	this->get_interface()->set_text(text);
}

void c_text_widget::set_text_from_string_id(string_id sid)
{
	INVOKE_TYPE(0x21BF85, 0x0, void(__thiscall*)(c_text_widget*, string_id), this, sid);
}

void c_text_widget::append_text(wchar_t const* text)
{
	this->get_interface()->append_text(text);
}

void c_text_widget::append_text_from_string_id(string_id sid)
{
	INVOKE_TYPE(0x21BFF6, 0x0, void(__thiscall*)(c_text_widget*, string_id), this, sid);
}

void c_text_widget::set_text_properties(int32 flags, int16 animation_index, real_argb_color* text_color, int32 font, rectangle2d* bounds)
{
	INVOKE_TYPE(0x21BC81, 0x0, void(__thiscall*)(c_text_widget*, int32, int16, real_argb_color*, int32, rectangle2d*), this, flags, animation_index, text_color, font, bounds);
}

void c_text_widget::set_field74()
{
	field_74 = true;
}

void c_text_widget::render_widget(rectangle2d* viewport_bounds)
{
	INVOKE_TYPE(0x21BD11, 0x0, void(__thiscall*)(c_text_widget*, rectangle2d*), this, viewport_bounds);
}
int32 c_text_widget::get_intro_delay()
{
	return INVOKE_TYPE(0x21BA94, 0x0, int32(__thiscall*)(c_text_widget*), this);
}



c_small_text_widget::c_small_text_widget(int16 user_flags):
	c_text_widget(user_flags)
{

}

c_small_text_widget::c_small_text_widget(datum user_index):
	c_text_widget(user_index)
{
}

c_small_text_widget::~c_small_text_widget()
{
}

c_user_interface_text* c_small_text_widget::get_interface()
{
	//return INVOKE_TYPE(0x20F5E3, 0x0, c_user_interface_text*(__thiscall*)(c_small_text_widget*), this);
	return &m_interface;
}



c_normal_text_widget::c_normal_text_widget(int16 user_flags):
	c_text_widget(user_flags)
{
}

c_normal_text_widget::c_normal_text_widget(datum user_index):
	c_text_widget(user_index)
{
}

c_normal_text_widget::~c_normal_text_widget()
{
}

c_user_interface_text* c_normal_text_widget::get_interface()
{
	//return INVOKE_TYPE(0x20F6CA, 0x0, c_user_interface_text*(__thiscall*)(c_normal_text_widget*), this);
	return &m_interface;
}


c_long_text_widget::c_long_text_widget(int16 user_flags):
	c_text_widget(user_flags)
{
}

c_long_text_widget::c_long_text_widget(datum user_index):
	c_text_widget(user_index)
{
}

c_long_text_widget::~c_long_text_widget()
{
}

c_user_interface_text* c_long_text_widget::get_interface()
{
	//return INVOKE_TYPE(0x243EF5, 0x0, c_user_interface_text*(__thiscall*)(c_normal_text_widget*), this);
	return &m_interface;
}
