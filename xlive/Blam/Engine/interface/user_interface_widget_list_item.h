#pragma once
#include "user_interface_group_widget.h"
#include "signal_slot.h"

/* classes */

class c_list_item_widget : public c_group_widget
{
public:
	c_list_item_widget(void);
	
	void set_last_data_index(int32 index);
	
	datum get_last_data_index(void) const;

	void set_item_transitioning(bool state);


	// c_list_item_widget virtual functions

	virtual ~c_list_item_widget(void) = default;
	virtual void update(void) override;
	virtual int32 get_intro_delay(void) override;
	virtual bool handle_event(s_event_record* event) override;
	virtual void construct_animation_on_region_enter(int32 a1) override;
	virtual void construct_animation_on_region_leave(int32 a1) override;
	virtual bool can_interact(void) override;
	virtual bool sub_6114B9(void) override;

	// c_list_item_widget additions

	virtual bool last_data_index_valid();

protected:
	int32 m_last_data_index;
	int32 m_intro_delay_milliseconds;
	bool field_78;
	_slot_linker linker_type1;
	_slot_linker linker_type2;
};
ASSERT_STRUCT_SIZE(c_list_item_widget, 0x84);