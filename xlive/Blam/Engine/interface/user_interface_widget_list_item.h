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
	uint8 gap_79[3];
	_slot1<>* signal1;
	_slot2<>* signal2;

public:
	c_list_item_widget();
	datum get_last_data_index();

	// c_list_item_widget virtual functions

	virtual ~c_list_item_widget();
	virtual void update() override;
	virtual int32 get_intro_delay() override;
	virtual uint8 handle_event(s_event_record* event) override;
	virtual void construct_animation_on_region_enter(int32 a1) override;
	virtual void construct_animation_on_region_leave(int32 a1) override;
	virtual bool can_interact() override;
	virtual bool sub_6114B9() override;

	// c_list_item_widget additions

	virtual bool last_data_index_valid();
};
ASSERT_STRUCT_SIZE(c_list_item_widget, 0x84);