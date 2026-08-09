#pragma once
#include "user_interface_channel.h"

/* structures */

struct s_user_interface_widget_stack
{
	s_user_interface_widget_stack* old_stack_item;
	c_screen_parameters parameters;
	int32 screen_menu_id;
};
ASSERT_STRUCT_SIZE(s_user_interface_widget_stack, 0x28);


/* classes */

class c_channel_with_history : public c_user_interface_channel
{
private:
	s_user_interface_widget_stack* m_widget_stack;
	int8 field_40;
	int8 field_41;
	int16 field_42;

public:

	// c_channel_with_history virtual functions

	virtual ~c_channel_with_history();
	virtual void dispose_screens() override;
	virtual void register_incoming_screen(c_screen_widget* new_screen, c_screen_parameters* parameters) override;
	virtual void  retreat_one_step() override;
	virtual void construct_parameters_from_active_screen() override;
	virtual void  load_screen_from_incoming_parameters() override;

	// c_channel_with_history addons

	virtual void retreat_steps(int16 number_of_steps);
	virtual void try_retreat_to_menu(e_user_interface_screen_id menu_id);


};
ASSERT_STRUCT_SIZE(c_channel_with_history, 0x44);
