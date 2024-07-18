#pragma once

#include "interface/user_interface_widget_list.h"
#include "interface/user_interface_widget_list_item.h"
#include "interface/user_interface_widget_window.h"

/* macro defines */

#define k_no_of_visible_items_for_squad_settings 7

/* classes */

class c_squad_settings_list : public c_list_widget
{
protected:
	c_list_item_widget m_list_items[k_no_of_visible_items_for_squad_settings];
	c_slot2<c_squad_settings_list,s_event_record*,datum> m_slot;
	bool m_party_mgmt_item_deleted;
	uint8 gap_465[3];

	bool handle_item_pressed_event(s_event_record** pevent, datum* pitem_index);
	bool handle_item_change_map(s_event_record** pevent);
	bool handle_item_change_variant(s_event_record** pevent);
	bool handle_item_change_level(s_event_record** pevent);
	bool handle_item_change_difficulty(s_event_record** pevent);
	bool handle_item_quick_options(s_event_record** pevent);
	bool handle_item_switch_to_coop(s_event_record** pevent);
	bool handle_item_switch_to_arranged(s_event_record** pevent);
	bool handle_item_switch_to_optimatch(s_event_record** pevent);
	bool handle_item_change_hopper(s_event_record** pevent);
	bool handle_item_party_management(s_event_record** pevent);


public:
	c_squad_settings_list(int16 user_flags);
	uint16 get_last_item_type();
	bool party_management_exists();
	void party_management_delete_item();
	
	// c_squad_settings_list virtual functions
	
	virtual ~c_squad_settings_list();
	bool handle_event(s_event_record* event) override;
	virtual c_list_item_widget* get_list_items() override;
	virtual int32 get_list_items_count() override;
	virtual void update_list_items(c_list_item_widget* item, int32 skin_index) override;

};
ASSERT_STRUCT_SIZE(c_squad_settings_list, 0x468);


class c_screen_squad_settings : protected c_screen_widget
{
protected:
	c_squad_settings_list m_squad_settings_list;
public:
	static void* load(s_screen_parameters* parameters);	
	static void apply_instance_patches();	
	static void apply_patches_on_map_load();
	c_screen_squad_settings(e_user_interface_channel_type channel_type, e_user_interface_render_window window_index, int16 user_flags);

	// c_screen_squad_settings virtual functions

	virtual ~c_screen_squad_settings();
	virtual void update() override;
	virtual bool handle_event(s_event_record* event) override;
	virtual void initialize(s_screen_parameters* parameters) override;
	virtual void* load_proc() override;
};
ASSERT_STRUCT_SIZE(c_screen_squad_settings, 0xEC4);
