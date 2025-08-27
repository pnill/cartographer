#pragma once
#include "interface/multiplayer_variant_settings_interface_definition.h"
#include "interface/user_interface_widget.h"
#include "interface/user_interface_widget_list.h"
#include "interface/user_interface_widget_list_item.h"
#include "interface/user_interface_widget_window.h"

#define k_variant_options_list_name "variant category options list"
#define k_variant_options_list_empty_name "EMPTY variant category options list"
#define k_variant_options_list_item_count 12

struct s_variant_options_list_item
{
	int32 unk;
	s_text_value_pair_definition* sily_definition;
	e_variant_setting_parameter_type parameter_type;
};
ASSERT_STRUCT_SIZE(s_variant_options_list_item, 12);

static e_variant_setting_category_type g_previous_variant_setting_category;

class c_variant_options_list : public c_list_widget
{
public:
	c_list_item_widget m_items[k_variant_options_list_item_count];
	e_variant_setting_category_type m_variant_setting_category_type;
	s_variant_setting_edit_reference* m_variant_setting_reference;
	c_slot2<c_variant_options_list, s_event_record*, datum> m_slot;
	bool m_is_quick_options;

	void handle_item_pressed_event(s_event_record** event, datum* pitem_index);

public:
	c_variant_options_list(uint16 user_flags);

	virtual ~c_variant_options_list() = default;
	virtual int32 link_item_widgets() override;
	virtual c_list_item_widget* get_list_items() override;
	virtual int32 get_list_items_count() override;
	virtual void update_list_items(c_list_item_widget* item, int32 skin_index) override;

	void update_list_items_cartographer(c_list_item_widget* item, int32 skin_index) const;
	void update_list_items_custom(c_list_item_widget* item, int32 skin_index) const;
	void update_list_items_default(c_list_item_widget* item, int32 skin_index) const;

	void link_item_widgets_cartographer();
	void link_item_widgets_headhunter();
	void link_item_widgets_default();

	void set_quick_options(bool state);
	void set_variant_setting_category_type(e_variant_setting_category_type category);
};
ASSERT_STRUCT_SIZE(c_variant_options_list, 1796);

class c_screen_variant_options : public c_screen_with_menu
{
private:
	c_variant_options_list m_list;

	static void* load_editor(s_screen_parameters* parameters);
	static void* load_quick_options(s_screen_parameters* parameters);

public:
	static void new_instance(e_variant_setting_category_type category, e_user_interface_channel_type channel, e_user_interface_render_window window, int16 user_flags);
	c_screen_variant_options(e_user_interface_channel_type ui_channel, e_user_interface_render_window  window_index, uint16 user_flags, e_user_interface_screen_id screen_id);

	virtual ~c_screen_variant_options() = default;
	virtual void update() override;
	virtual const void* load_proc() const override;

	void set_variant_setting_category_type(e_variant_setting_category_type category);
};
ASSERT_STRUCT_SIZE(c_screen_variant_options, 4452);