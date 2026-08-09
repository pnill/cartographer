#pragma once
#include "interface/user_interface.h"
#include "interface/user_interface_widget_list.h"
#include "interface/user_interface_widget_list_item.h"
#include "interface/user_interface_widget_window.h"


/* macro defines */

/* constants */

/* enums */

enum
{
	k_no_of_visible_items_for_video_settings = (8 + 2)
};

/* classes */


class c_video_settings_list : public c_list_widget
{
protected:
	c_list_item_widget m_list_items[k_no_of_visible_items_for_video_settings];
	c_slot2<c_video_settings_list, s_event_record*, datum> m_slot;

	void handle_item_pressed_event(s_event_record* const& event, datum* pitem_index);


public:
	c_video_settings_list(uint16 user_flags);

	// c_video_settings_list virtual functions

	virtual ~c_video_settings_list();
	virtual c_list_item_widget* get_list_items() override;
	virtual int32 get_list_items_count() override;
	virtual void update_list_items(c_list_item_widget* item, int32 skin_index) override;

};
//ASSERT_STRUCT_SIZE(c_video_settings_list, 0x4E8);



class c_screen_video_settings : public c_screen_with_menu
{
protected:
	c_video_settings_list m_video_settings_list;

public:
	static void* load(c_screen_parameters* parameters);
	static void apply_patches_on_map_load();
	c_screen_video_settings(e_user_interface_channel_type channel_type, e_user_interface_render_window window_index, uint16 user_flags);

	// c_screen_video_settings virtual functions

	virtual ~c_screen_video_settings() = default;
	virtual void update() override;
	virtual const void* load_proc() const override;

};
//ASSERT_STRUCT_SIZE(c_screen_video_settings, 0xF48);