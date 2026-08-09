#pragma once
#include "user_interface.h"

/* enums */

enum e_focussed_widget_type
{
	_focussed_widget_type_selected = 0,
	_focussed_widget_type_hovered, //doesnt exist in h2x
	k_focussed_widget_types
};

/* classes */

class c_user_interface_channel
{
public:
	bool active_or_incoming_screen_exists(void);
	void transition_out(void);

	// c_user_interface_channel virtual functions

	virtual ~c_user_interface_channel(void);
	virtual void dispose_screens_wrapper(void);
	virtual void sub_637627(void);
	virtual void dispose_screens(void);
	virtual void update_channel(void);
	virtual class c_screen_widget* sub_638025(rectangle2d* bounds);
	virtual void register_incoming_screen(class c_screen_widget* new_screen, class c_screen_parameters* parameters);
	virtual void  retreat_one_step(void);
	virtual int32  destroy_target_screen(class c_screen_widget* a2);
	virtual void construct_parameters_from_active_screen(void);
	virtual void  load_screen_from_incoming_parameters(void);


	class c_screen_widget* incoming_screen_get(void)
	{
		return m_incoming_screen;
	}

private:
	int32 m_window_index;
	class c_screen_widget* m_active_screen;
	class c_screen_widget* m_incoming_screen;
	c_screen_parameters m_incoming_screen_parameters;
	class c_screen_widget* m_outgoing_screen;
	class c_user_interface_widget* m_focussed_widgets[k_focussed_widget_types];
};
ASSERT_STRUCT_SIZE(c_user_interface_channel, 0x3C);
