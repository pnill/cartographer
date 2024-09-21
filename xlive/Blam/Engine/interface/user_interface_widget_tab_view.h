#pragma once
#include "user_interface_widget.h"

/* classes */

class c_tab_view_widget : protected c_user_interface_widget
{
protected:
	c_user_interface_widget* field_70;

public:
	c_tab_view_widget(int16 user_flags);

	// c_tab_view_widget virtual functions

	virtual ~c_tab_view_widget()
	{
	}
	virtual int32 setup_children() override;
	virtual bool handle_event(s_event_record* event) override;
	virtual c_user_interface_text* get_interface() override;

};
ASSERT_STRUCT_SIZE(c_tab_view_widget, 0x74);