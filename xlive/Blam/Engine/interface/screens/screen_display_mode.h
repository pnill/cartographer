#pragma once
#include "cseries/language.h"
#include "interface/user_interface.h"
#include "interface/user_interface_widget_list.h"
#include "interface/user_interface_widget_list_item.h"
#include "interface/user_interface_widget_window.h"

/* constants */

enum
{
	k_no_of_visible_items_for_display_mode = (2+1)
};

extern const wchar_t* const k_borderless_string[k_language_count];

/* classes */


class c_display_mode_edit_list : public c_list_widget
{
protected:
	c_list_item_widget m_list_items[k_no_of_visible_items_for_display_mode];
	bool m_multiplayer_menu;
	c_slot2<c_display_mode_edit_list, s_event_record*, datum> m_slot;

	void handle_item_pressed_event(s_event_record* const& event, datum* pitem_index);


public:
	c_display_mode_edit_list(uint16 user_flags);
	void set_using_mp_screen(bool param);

	// c_display_mode_edit_list virtual functions

	virtual ~c_display_mode_edit_list() = default;
	virtual void setup_children() override;
	virtual c_list_item_widget* get_list_items() override;
	virtual int32 get_list_items_count() override;
	virtual void update_list_items(c_list_item_widget* item, int32 skin_index) override;

};
//ASSERT_STRUCT_SIZE(c_display_mode_edit_list, 0x1D4);


class c_screen_display_mode_menu : public c_screen_with_menu
{

protected:
	c_display_mode_edit_list m_display_mode_edit_list;

public:
	static void* __cdecl load(c_screen_parameters* parameters);
	static void* __cdecl load_mp(c_screen_parameters* parameters);

	c_screen_display_mode_menu(e_user_interface_channel_type channel_type, e_user_interface_render_window window_index, uint16 user_flags);

	// c_screen_display_mode_menu virtual functions

	virtual ~c_screen_display_mode_menu() = default;
	virtual const void* load_proc() const override;
};
//ASSERT_STRUCT_SIZE(c_screen_display_mode_menu, 0xC34);