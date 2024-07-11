#pragma once
#include "user_interface_widget.h"
#include "user_interface_text_block.h"
#include "signal_slot.h"

/* classes */

class c_button_widget : protected c_user_interface_widget
{
protected:
	c_small_user_interface_text text_interface;
	int32 m_flags;
	int16 m_button_index;
	uint8 gap_FA[2];
	_slot2<> *signal;

public:
	c_button_widget(int16 button_index, int16 user_flags);

	// c_button_widget virtual functions

	virtual ~c_button_widget();
	virtual void update() override;
	virtual void render_widget(rectangle2d* viewport_bounds) override;
	virtual void* get_mouse_region(rectangle2d* mouse_region_out) override;
	virtual int get_intro_delay() override;
	virtual char handle_event(s_event_record* event) override;
	virtual bool can_interact() override;
	virtual c_user_interface_text* get_interface() override;

	// c_button_widget additions

	virtual void setup_texts();
	virtual int32 get_bitmap_active_index();
};
ASSERT_STRUCT_SIZE(c_button_widget, 0x100);