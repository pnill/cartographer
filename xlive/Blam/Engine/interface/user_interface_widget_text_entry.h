#pragma once
#include "user_interface_text_block.h"
#include "user_interface_widget_text.h"
#include "user_interface_widget_button.h"

/* classes */

class c_text_entry_widget : protected c_button_widget
{
protected:
	c_normal_user_interface_text text_interface;
	uint32 field_544;

public:
	c_text_entry_widget(int16 button_index, int16 user_flags);

	// c_text_entry_widget virtual functions

	virtual ~c_text_entry_widget();
	virtual void render_widget(rectangle2d* viewport_bounds) override;
	virtual bool handle_event(s_event_record* event) override;
	virtual c_user_interface_text* get_interface() override;

	// c_button_widget additions

	virtual void setup_texts() override;
};
ASSERT_STRUCT_SIZE(c_text_entry_widget, 0x548);


//Todo : complete these
class c_text_entry_widget_in_game : protected c_text_entry_widget
{
protected:
	uint32 field_548;
	uint32 field_54C;
	uint32 field_550;
	uint32 field_554;
	c_normal_text_widget text_widget;
	uint8 gap_A14[4];
};
ASSERT_STRUCT_SIZE(c_text_entry_widget_in_game, 0xA18);


class c_text_entry_widget_for_pregame_lobby : protected c_text_entry_widget
{

};
ASSERT_STRUCT_SIZE(c_text_entry_widget_for_pregame_lobby, 0x548);