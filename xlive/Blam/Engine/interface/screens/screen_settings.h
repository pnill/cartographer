#pragma once

#include "interface/user_interface_widget_list.h"
#include "interface/user_interface_widget_list_item.h"
#include "interface/user_interface_widget_window.h"

/* constants  */

enum
{

	// note :
	// -h2x tag visible items : 2 , screen class items : 3
	// -h2v tag visible items : 6 , screen class items : 7
	// -both have an extra "unused" item which might be the incomplete ui\screens\game_shell\settings_screen\extras\extras_enabled_dialog.wgit
	// -it has the texts done and looks semi-okayish , should be worth restoring
	k_no_of_visible_items_for_settings = 7,
};

/* classes */

class c_settings_list : public c_list_widget
{
protected:
	c_list_item_widget m_list_items[k_no_of_visible_items_for_settings];
	c_slot2<c_settings_list, struct s_event_record*, datum> m_slot;
	bool field_464; // probably for unlocking "unused" item

	void handle_item_pressed_event(struct s_event_record* const& event, datum* pitem_index);
	void handle_item_player_profile(struct s_event_record* const& event);
	void handle_item_variant(struct s_event_record* const& event);
	void handle_item_video(struct s_event_record* const& event);
	void handle_item_audio(struct s_event_record* const& event);
	void handle_item_network(struct s_event_record* const& event);
	void handle_item_about(struct s_event_record* const& event);
	//custom addition
	void handle_item_guide(struct s_event_record* const& event);


public:
	c_settings_list(int16 user_flags);

	// c_settings_list virtual functions

	virtual ~c_settings_list(void) = default;
	virtual c_list_item_widget* get_list_items(void) override;
	virtual int32 get_list_items_count(void) override;
	virtual void update_list_items(c_list_item_widget* item, int32 skin_index) override;

};
ASSERT_STRUCT_SIZE(c_settings_list, 0x468);


class c_screen_settings : protected c_screen_with_menu
{
protected:
	c_settings_list m_settings_list;
public:
	static void* load(class c_screen_parameters* parameters);
	static void apply_patches_on_map_load();
	c_screen_settings(e_user_interface_channel_type channel_type, e_user_interface_render_window window_index, int16 user_flags);

	// c_screen_settings virtual functions

	virtual ~c_screen_settings() = default;
	virtual void post_initialize_button_keys() override;
	virtual const void* load_proc(void) const override;
};
ASSERT_STRUCT_SIZE(c_screen_settings, 0xEC8);
