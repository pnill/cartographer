#pragma once

#include "interface/user_interface_widget_list.h"
#include "interface/user_interface_widget_list_item.h"

/* macro defines */

#define k_no_of_visible_items_for_main_menu 6

/* classes */

class c_main_menu_list : protected c_list_widget
{
protected:
	c_list_item_widget m_list_items[k_no_of_visible_items_for_main_menu];
	c_slot2<c_main_menu_list, s_event_record**, datum> m_slot;


	void handle_item_pressed_event(s_event_record** pevent, datum* pitem_index);
	void handle_item_campaign(s_event_record** pevent);
	void handle_item_xbox_live(s_event_record** pevent);
	void handle_item_splitscreen(s_event_record** pevent);
	void handle_item_system_link(s_event_record** pevent);
	void handle_item_settings(s_event_record** pevent);
	//void handle_item_guide(s_event_record** pevent);
	void handle_item_quit(s_event_record** pevent);

public:
	c_main_menu_list(int16 user_flags);
	
	void c_main_menu_list_ctor(int16 user_flags);
	static void apply_instance_patches();

	// c_main_menu_list virtual functions

	virtual c_user_interface_widget* destructor(uint32 flags) override;
	virtual c_list_item_widget* get_list_items() override;
	virtual int32 get_list_items_count() override;
	virtual void update_list_items(c_list_item_widget* item, int32 skin_index) override;

};
ASSERT_STRUCT_SIZE(c_main_menu_list, 0x3E0);


