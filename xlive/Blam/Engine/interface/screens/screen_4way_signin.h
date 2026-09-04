#pragma once

#include "interface/user_interface_widget_window.h"

/* enums */

enum e_4_way_signin_type
{
	_4_way_signin_type_campaign = 0,
	_4_way_signin_type_splitscreen,
	_4_way_signin_type_system_link,
	_4_way_signin_type_xbox_live,
	_4_way_signin_type_crossgame_invite,
};

/* classes */

class c_screen_4way_signin : protected c_screen_widget
{
public:

	static void* load(class c_screen_parameters* parameters);
	static void* load_for_crossgame_invite(class c_screen_parameters* parameters);
	static void* load_for_xbox_live(class c_screen_parameters* parameters);
	static void* load_for_system_link(class c_screen_parameters* parameters);
	static void* load_for_splitscreen(class c_screen_parameters* parameters);
	static void* load_for_campaign(class c_screen_parameters* parameters);
	
	static void apply_patches_on_map_load();
	static void apply_instance_patches();

	c_screen_4way_signin(e_user_interface_channel_type channel_type, e_user_interface_render_window window_index, int16 user_flags);

	// c_screen_4way_signin virtual functions
	virtual ~c_screen_4way_signin() = default;
	virtual void update() override;
	virtual bool handle_event(struct s_event_record* event) override;
	virtual void initialize(class c_screen_parameters const* parameters) override;
	virtual const void* load_proc(void) const override;

protected:
	e_4_way_signin_type m_call_context;

	bool handle_controller_button_pressed_event(struct s_event_record* event);
	bool handle_invalid_controller_event(struct s_event_record* event) const;
	bool handle_automation_event(struct s_event_record* event);
	bool handle_split_input_event(struct s_event_record* event);
	void update_button_key_texts(void);
};
ASSERT_STRUCT_SIZE(c_screen_4way_signin, 0xA60);