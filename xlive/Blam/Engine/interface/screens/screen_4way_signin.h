#pragma once

#include "interface/user_interface_widget_window.h"

enum e_4_way_signin_type
{
	_4_way_signin_type_campaign = 0x0,
	_4_way_signin_type_splitscreen = 0x1,
	_4_way_signin_type_system_link = 0x2,
	_4_way_signin_type_xbox_live = 0x3,
	_4_way_signin_type_crossgame_invite = 0x4,
};


class c_screen_4way_signin : protected c_screen_widget
{
protected:
	e_4_way_signin_type m_call_context;

	bool handle_main_events(s_event_record* event);
	bool handle_default_events(s_event_record* event);
public:

	static void* load(s_screen_parameters* parameters);
	static void* load_for_crossgame_invite(s_screen_parameters* parameters);
	static void* load_for_xbox_live(s_screen_parameters* parameters);
	static void* load_for_system_link(s_screen_parameters* parameters);
	static void* load_for_splitscreen(s_screen_parameters* parameters);
	static void* load_for_campaign(s_screen_parameters* parameters);
	
	static void apply_patches_on_map_load();
	static void apply_instance_patches();

	c_screen_4way_signin(e_user_interface_channel_type channel_type, e_user_interface_render_window window_index, int16 user_flags);

	// c_screen_4way_signin virtual functions

	virtual ~c_screen_4way_signin();
	virtual void update() override;
	virtual bool handle_event(s_event_record* event) override;
	virtual void initialize(s_screen_parameters* parameters) override;
	virtual void* load_proc() override;
};
ASSERT_STRUCT_SIZE(c_screen_4way_signin, 0xA60);