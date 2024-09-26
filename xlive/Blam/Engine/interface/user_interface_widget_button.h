#pragma once
#include "user_interface_widget.h"
#include "user_interface_text_block.h"
#include "signal_slot.h"

/* classes */

class c_button_widget : public c_user_interface_widget
{
protected:
	c_small_user_interface_text m_text_interface;
	int32 m_flags;
	int16 m_button_index;
	_slot_linker linker_type1;

public:
	c_button_widget(int16 button_index, uint16 user_flags);

	void set_button_index(int16 button_index);

	// c_button_widget virtual functions

	virtual ~c_button_widget() = default;
	virtual void update() override;
	virtual void render_widget(rectangle2d* viewport_bounds) override;
	virtual void* get_mouse_region(rectangle2d* mouse_region_out) override;
	virtual int32 get_intro_delay() override;
	virtual bool handle_event(s_event_record* event) override;
	virtual bool can_interact() override;
	virtual c_user_interface_text* get_interface() override;

	// c_button_widget additions

	virtual void setup_texts();
	virtual int32 get_bitmap_active_index();
};
ASSERT_STRUCT_SIZE(c_button_widget, 0x100);