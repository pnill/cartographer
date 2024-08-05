#pragma once

#include "interface/user_interface_widget_list.h"
#include "interface/user_interface_widget_list_item.h"
#include "interface/user_interface_widget_window.h"

/* macro defines */

// note :
// -h2x tag visible items : 2 , screen class items : 3
// -h2v tag visible items : 6 , screen class items : 7
// -both have an extra "unused" item which might be the incomplete ui\screens\game_shell\settings_screen\extras\extras_enabled_dialog.wgit
// -it has the texts done and looks semi-okayish , should be worth restoring
//
#define k_no_of_visible_items_for_settings 7

/* classes */

class c_settings_list : public c_list_widget
{
protected:
	c_list_item_widget m_list_items[k_no_of_visible_items_for_settings];
	c_slot2<c_settings_list, s_event_record**, datum> m_slot;
	bool field_464; // probably for unlocking "unused" item

	void handle_item_pressed_event(s_event_record** pevent, datum* pitem_index);
	void handle_item_player_profile(s_event_record** pevent);
	void handle_item_variant(s_event_record** pevent);
	void handle_item_video(s_event_record** pevent);
	void handle_item_audio(s_event_record** pevent);
	void handle_item_network(s_event_record** pevent);
	void handle_item_about(s_event_record** pevent);
	//custom addition
	void handle_item_guide(s_event_record** pevent);


public:
	c_settings_list(int16 user_flags);

	// c_settings_list virtual functions

	virtual ~c_settings_list();
	virtual c_list_item_widget* get_list_items() override;
	virtual int32 get_list_items_count() override;
	virtual void update_list_items(c_list_item_widget* item, int32 skin_index) override;

};
ASSERT_STRUCT_SIZE(c_settings_list, 0x468);


class c_screen_settings : protected c_screen_with_menu
{
protected:
	c_settings_list m_settings_list;
public:
	static void* load(s_screen_parameters* parameters);
	static void apply_patches_on_map_load();
	c_screen_settings(e_user_interface_channel_type channel_type, e_user_interface_render_window window_index, int16 user_flags);

	// c_screen_settings virtual functions

	virtual c_user_interface_widget* destructor(uint32 flags) override;
	virtual void post_initialize_button_keys() override;
	virtual void* load_proc() override;
};
ASSERT_STRUCT_SIZE(c_screen_settings, 0xEC8);
