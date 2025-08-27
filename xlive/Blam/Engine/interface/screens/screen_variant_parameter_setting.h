#pragma once
#include "interface/multiplayer_variant_settings_interface_definition.h"
#include "interface/user_interface_widget.h"
#include "interface/user_interface_widget_list.h"
#include "interface/user_interface_widget_list_item.h"
#include "interface/user_interface_widget_window.h"

#define k_variant_parameter_setting_list_name "variant parameter setting list"
#define k_variant_parameter_setting_list_empty_name "EMPTY variant parameter setting list"
#define k_variant_parameter_setting_list_count 6

struct s_variant_parameter_setting_list_item
{
	int32 unk;
	s_text_value_pair_reference_new* text_value_pair_reference;
	int32 value;
};
ASSERT_STRUCT_SIZE(s_variant_parameter_setting_list_item, 12);

static e_variant_setting_parameter_type g_previous_variant_setting_parameter;

class c_variant_parameter_setting_list : public c_list_widget
{
private:
	c_list_item_widget m_items[k_variant_parameter_setting_list_count];
	c_slot2<c_variant_parameter_setting_list, s_event_record*, int32> m_slot;
	e_variant_setting_parameter_type m_variant_setting_parameter_type;
	s_text_value_pair_definition* m_sily_definition;
	int8 m_is_quick_options;

	void handle_item_pressed_event(s_event_record** event, datum* pitem_index);

public:
	c_variant_parameter_setting_list(uint16 user_flags);

	virtual ~c_variant_parameter_setting_list() = default;

	virtual int32 link_item_widgets() override;
	void link_item_widgets_default();
	void link_item_widgets_custom(s_game_variant* variant);
	void link_item_widgets_cartographer();

	virtual c_list_item_widget* get_list_items() override;

	virtual int32 get_list_items_count() override;

	virtual void update_list_items(c_list_item_widget* item, int32 skin_index) override;

	void update_selected_list_item();

	void set_variant_parameter_type(e_variant_setting_parameter_type type);
	e_variant_setting_parameter_type get_variant_parameter_type() const;

	void set_is_quick_options(bool state);
	bool get_is_quick_options() const;
};
ASSERT_STRUCT_SIZE(c_variant_parameter_setting_list, 1004);


class c_screen_variant_parameter_setting : public c_screen_with_menu
{
private:
	c_variant_parameter_setting_list m_list;

public:
	c_screen_variant_parameter_setting(e_user_interface_channel_type ui_channel, e_user_interface_render_window  window_index, uint16 user_flags, e_user_interface_screen_id screen_id);
	static void* __cdecl load_quick_options(s_screen_parameters* parameters);
	static void* __cdecl load_settings(s_screen_parameters* parameters);


	virtual ~c_screen_variant_parameter_setting() = default;

	virtual void post_initialize() override;
	void post_initialize_default();
	void post_initialize_custom(s_game_variant* variant);
	void post_initialize_cartographer();

	const void* load_proc() const override;

	static void new_instance(e_variant_setting_parameter_type parameter_type, e_user_interface_channel_type ui_channel, e_user_interface_render_window ui_window, uint16 user_flags, bool is_quick_options);
};
ASSERT_STRUCT_SIZE(c_screen_variant_parameter_setting, 3660);