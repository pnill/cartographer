#pragma once
#include "user_interface_group_widget.h"
#include "signal_slot.h"

/* classes */

class c_list_item_widget : public c_group_widget
{
protected:
	datum m_last_data_index;
	int32 m_intro_delay_milliseconds;
	bool field_78;
	_slot_linker linker_type1;
	_slot_linker linker_type2;

public:
	c_list_item_widget();
	datum get_last_data_index();
	void set_item_transitioning();

	

	// c_list_item_widget virtual functions

	virtual ~c_list_item_widget() = default;
	virtual void update() override;
	virtual int32 get_intro_delay() override;
	virtual bool handle_event(s_event_record* event) override;
	virtual void construct_animation_on_region_enter(int32 a1) override;
	virtual void construct_animation_on_region_leave(int32 a1) override;
	virtual bool can_interact() override;
	virtual bool sub_6114B9() override;

	// c_list_item_widget additions

	virtual bool last_data_index_valid();
};
ASSERT_STRUCT_SIZE(c_list_item_widget, 0x84);