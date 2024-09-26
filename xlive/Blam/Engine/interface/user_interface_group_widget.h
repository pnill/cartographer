#pragma once
#include "user_interface_widget.h"

/* classes */

class c_group_widget : public c_user_interface_widget
{
public:
	c_group_widget(e_user_interface_widget_type widget_type, int16 user_flags);

	// c_group_widget virtual functions

	virtual ~c_group_widget() = default;
	virtual c_user_interface_text* get_interface() override;

};
ASSERT_STRUCT_SIZE(c_group_widget, 0x70);