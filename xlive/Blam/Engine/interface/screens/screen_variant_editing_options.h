#pragma once
#include "interface/user_interface_widget_list.h"
#include "interface/user_interface_widget_list_item.h"
#include "interface/user_interface_widget_window.h"

#define k_variant_editing_options_list_name "variant editing options list"

enum e_variant_editing_options_item : uint8
{
	_variant_editing_options_item_match,
	_variant_editing_options_item_player,
	_variant_editing_options_item_team,
	_variant_editing_options_item_game_type,
	_variant_editing_options_item_vehicle,
	_variant_editing_options_item_equipment,
	_variant_editing_options_item_cartographer,
	k_variant_editing_options_item_count
};

struct c_variant_editing_options_list : public c_list_widget
{
private:
	c_list_item_widget m_items[k_variant_editing_options_item_count];
	c_slot2< c_variant_editing_options_list, s_event_record*, int32> m_slot;

	void handle_item_pressed_event(s_event_record** pevent, datum* pitem_index);


public:
	c_variant_editing_options_list(uint16 user_flags);

	virtual ~c_variant_editing_options_list() = default;

	virtual c_list_item_widget* get_list_items() override;
	virtual int32 get_list_items_count() override;
	virtual void update_list_items(c_list_item_widget* item, int32 skin_index) override;
};
ASSERT_STRUCT_SIZE(c_variant_editing_options_list, 1124);

struct c_screen_variant_editing_options : public c_screen_with_menu
{
private:
	c_variant_editing_options_list m_list;

public:

	c_screen_variant_editing_options(e_user_interface_channel_type ui_channel, e_user_interface_render_window  window_index, uint16 user_flags, e_user_interface_screen_id screen_id);

	virtual ~c_screen_variant_editing_options() = default;

	const void* load_proc() const override;

	static void apply_patches_on_map_load();
	static void apply_patches();
	static void* __cdecl load_editor(s_screen_parameters* parameters);
};
ASSERT_STRUCT_SIZE(c_screen_variant_editing_options, 3780);