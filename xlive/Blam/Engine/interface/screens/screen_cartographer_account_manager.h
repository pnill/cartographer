#pragma once
#include "interface/user_interface_widget_list.h"
#include "interface/user_interface_widget_list_item.h"
#include "interface/user_interface_widget_window.h"

/* constants */

enum
{
	k_no_of_visible_items_for_cartographer_account_manager_list = 4,
	k_cartographer_account_email_and_password_max_length = 128,
};

/* enums */

enum e_cartographer_account_manager_screen_type : int8
{
	_cartographer_account_manager_screen_type_list,
	_cartographer_account_manager_screen_type_list_remove_account,
	_cartographer_account_manager_screen_type_create_account,
	_cartographer_account_manager_screen_type_add_account,
	
	k_cartographer_account_manager_screen_type_count,

	_cartographer_account_manager_screen_type_none = NONE,
};

/* classes */

class c_cartographer_account_manager_edit_list : public c_list_widget
{
private:
	// 4 items displayed at once in the list
	c_list_item_widget m_list_item_widgets[k_no_of_visible_items_for_cartographer_account_manager_list];
	c_slot2<c_cartographer_account_manager_edit_list, s_event_record*, int32> m_slot_2;

	e_cartographer_account_manager_screen_type m_cartographer_screen_type;

	union
	{
		struct
		{
			bool account_delete_mode;
		} m_account_list;

		struct
		{
			wchar_t user_name[XUSER_NAME_SIZE];
			wchar_t email[k_cartographer_account_email_and_password_max_length];
			wchar_t password[k_cartographer_account_email_and_password_max_length];
			wchar_t password_confirmation[k_cartographer_account_email_and_password_max_length];
		} m_account_create;

		struct
		{
			bool remember_me;
			wchar_t email_or_username[k_cartographer_account_email_and_password_max_length];
			wchar_t password[k_cartographer_account_email_and_password_max_length];
		} m_account_add;
	};
	// button handler callback
	void handle_item_pressed_event(s_event_record* const& event, int32* pitem_index);

	void handle_item_pressed_event_for_add_account(s_event_record* const& event, int32* pitem_index);
	void handle_item_pressed_event_for_listed_accounts(s_event_record* const& event, int32* pitem_index);
	void handle_item_pressed_event_for_create_account(s_event_record* const& event, int32* pitem_index);


public:
	int32 m_default_focused_item;

	c_cartographer_account_manager_edit_list(uint16 _flags, int32 _button_count, int32 _default_selected_button, e_cartographer_account_manager_screen_type _screen_type);

	bool account_removal_mode(void) const
	{
		return m_cartographer_screen_type == _cartographer_account_manager_screen_type_list_remove_account;
	}

	// c_cartographer_account_manager_edit_list virtual functions

	virtual ~c_cartographer_account_manager_edit_list(void) = default;
	virtual void pre_destroy(void) override;
	virtual c_list_item_widget* get_list_items(void) override;
	virtual int32 get_list_items_count(void) override;
	virtual void update_list_items(c_list_item_widget* item, int32 skin_index) override;

};
//static_assert(offsetof(c_cartographer_account_manager_edit_list, m_slot_2) == 708);
//static_assert(offsetof(c_cartographer_account_manager_edit_list, m_list_data) == 112);
//static_assert(offsetof(c_cartographer_account_manager_edit_list, gap_70[56]) == 172);

class c_cartographer_account_manager_menu : protected c_screen_with_menu
{
private:
	c_cartographer_account_manager_edit_list m_account_edit_list;
	e_cartographer_account_manager_screen_type m_cartographer_screen_type;


public:
	static bool g_accounting_go_back_to_list;
	static int32 g_accounting_active_handle_count;

	c_cartographer_account_manager_menu(e_user_interface_channel_type _ui_channel, e_user_interface_render_window _window_index, uint16 _flags, e_cartographer_account_manager_screen_type _screen_type, int32 _button_count, int32 _selected_button);
	
	static bool is_accounting_active_handle(void);
	static void update_accounting_active_handle(bool active);
	static void set_menu_open_context(e_cartographer_account_manager_screen_type screen_type);

	static void* __cdecl load(c_screen_parameters* a1);
	static void* __cdecl load_default_context(c_screen_parameters* a1);
	static c_cartographer_account_manager_menu* load_for_account_create_context(void);
	static c_cartographer_account_manager_menu* load_for_account_add_context(void);
	static c_cartographer_account_manager_menu* load_for_account_list_context(void);
	static c_cartographer_account_manager_menu* load_for_account_remove_from_list_context(void);

	// c_cartographer_account_manager_menu virtual functions

	virtual ~c_cartographer_account_manager_menu(void) = default;
	virtual void pre_destroy(void) override;
	virtual void initialize(c_screen_parameters const* parameters) override;
	virtual void post_initialize(void) override;
	virtual const void* load_proc(void) const override;
};
// ASSERT_STRUCT_SIZE(c_cartographer_account_manager_menu, 3396);

/* prototypes */

void cartographer_account_manager_open_list(void);
