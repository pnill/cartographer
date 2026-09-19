#pragma once
#include "cseries/language.h"
#include "interface/user_interface_widget_list.h"
#include "interface/user_interface_widget_list_item.h"
#include "interface/user_interface_widget_window.h"

/* constants */
enum
{
	k_no_of_visible_items_for_splitscren = 3
};

/* classes */

class c_splitscreen_edit_list : public c_list_widget
{
protected:
	c_list_item_widget m_list_items[k_no_of_visible_items_for_splitscren];
	c_slot2<c_splitscreen_edit_list, s_event_record*, datum> m_slot;

	void handle_item_pressed_event(s_event_record* const& event, datum* pitem_index);


public:
	c_splitscreen_edit_list(uint16 user_flags);

	// c_splitscreen_edit_list virtual functions

	virtual ~c_splitscreen_edit_list() = default;
	virtual c_list_item_widget* get_list_items() override;
	virtual int32 get_list_items_count() override;
	virtual void update_list_items(c_list_item_widget* item, int32 skin_index) override;

};


class c_screen_splitscreen_menu : public c_screen_with_menu
{
protected:
	c_splitscreen_edit_list m_splitscreen_edit_list;

public:
	static void* load(c_screen_parameters* parameters);
	static const wchar_t* const get_header_string();
	static const wchar_t* const get_option_string();
	c_screen_splitscreen_menu(e_user_interface_channel_type channel_type, e_user_interface_render_window window_index, uint16 user_flags);

	// c_screen_splitscreen_menu virtual functions

	virtual ~c_screen_splitscreen_menu() = default;
	virtual void initialize(c_screen_parameters const* parameters) override;
	virtual void post_initialize() override;
	virtual const void* load_proc() const override;

};

/* constants */

extern const wchar_t* const k_splitscreen_header_string[k_language_count];
