#include "interface/user_interface_widget_list.h"
#include "interface/user_interface_widget_list_item.h"
#include "interface/user_interface_widget_window.h"


/* constants */

enum
{
	k_no_of_visible_items_for_button_settings = 4 + 1
};

/* classes */

class c_button_settings_edit_list : public c_list_widget
{
protected:
	c_list_item_widget m_list_items[k_no_of_visible_items_for_button_settings];
	bool m_qtr_screen;
	c_slot2<c_button_settings_edit_list, s_event_record*, datum> m_slot;

	void handle_item_pressed_event(s_event_record* const& event, datum* pitem_index);

public:
	c_button_settings_edit_list(int16 user_flags);
	void set_using_qtr_screen(bool param);

	// c_button_settings_edit_list virtual functions

	virtual ~c_button_settings_edit_list() = default;
	virtual c_list_item_widget* get_list_items() override;
	virtual int32 get_list_items_count() override;
	virtual void update_list_items(c_list_item_widget* item, int32 skin_index) override;

};
//ASSERT_STRUCT_SIZE(c_button_settings_edit_list, 0x2DC);

class c_screen_button_settings_menu : public c_screen_with_menu
{
protected:
	c_button_settings_edit_list m_button_settings_list;
	int32 field_D3C; 	// non functional unfinished setting
	bool m_using_qtr_arrows;

public:
	static void* load(class c_screen_parameters* parameters);
	static void* load_qtr(class c_screen_parameters* parameters);
	static void apply_patches_on_ui_map_load(void);
	static void apply_patches_on_mp_map_load(void);
	static void apply_instance_patches(void);
	c_screen_button_settings_menu(e_user_interface_channel_type channel_type, e_user_interface_render_window window_index, int16 user_flags, e_user_interface_screen_id screen_id);

	// c_screen_button_settings_menu virtual functions

	virtual ~c_screen_button_settings_menu() = default;
	virtual void update() override;
	virtual bool handle_event(s_event_record* event) override;
	virtual void post_initialize() override;
	virtual void post_initialize_button_keys() override;
	virtual const void* load_proc() const override;

};
//ASSERT_STRUCT_SIZE(c_screen_button_settings_menu, 0xD44);
