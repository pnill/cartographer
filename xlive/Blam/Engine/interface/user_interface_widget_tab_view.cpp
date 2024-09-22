#include "stdafx.h"
#include "user_interface_widget_tab_view.h"

c_tab_view_widget::c_tab_view_widget(int16 user_flags):
	c_user_interface_widget(_widget_type_table_view, user_flags)
{
	//return INVOKE_TYPE(0x220A3A, 0x0, c_tab_view_widget(*__thiscall*)(c_tab_view_widget*, __int16), this,a2);

	this->field_70 = nullptr;
}

c_tab_view_widget::~c_tab_view_widget()
{
}

int32 c_tab_view_widget::setup_children()
{
	//return INVOKE_TYPE(0x220A5F, 0x0, int32(__thiscall*)(c_tab_view_widget*), this);

	if (get_children())
	{
		field_70 = get_children();
	}

	return this->setup_children();
}

bool c_tab_view_widget::handle_event(s_event_record* event)
{
	return INVOKE_TYPE(0x220A6E, 0x0, bool(__thiscall*)(c_tab_view_widget*, s_event_record*), this, event);
}

c_user_interface_text* c_tab_view_widget::get_interface()
{
	//return INVOKE_TYPE(0x220A5C, 0x0, c_user_interface_text*(__thiscall*)(c_tab_view_widget*),this);
	return nullptr;
}
