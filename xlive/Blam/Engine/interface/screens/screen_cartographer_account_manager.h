#pragma once
#include "interface/user_interface_headers.h"
#include "main/game_preferences.h"

#include "H2MOD/Modules/Accounts/Accounts.h"

enum e_cartographer_account_manager_screen_type
{
	_cartographer_account_manager_screen_type_none = NONE,
	_cartographer_account_manager_screen_type_list,
	_cartographer_account_manager_screen_type_list_remove_account,
	_cartographer_account_manager_screen_type_create_account,
	_cartographer_account_manager_screen_type_add_account, 
	 
	_carrographer_account_manager_screen_type_end
};

enum e_cartographer_screen_type_account_list_string_table
{
	_screen_type_account_list_header_text = 0,
	_screen_type_account_list_subheader_text,

	// last indexes are reserved for buttons
	k_screen_type_account_list_button_text_index,
	_screen_type_account_list_button_play_offline_text = k_screen_type_account_list_button_text_index,
	_screen_type_account_list_button_create_account_text,
	_screen_type_account_list_button_add_account_text,
	_screen_type_account_list_button_remove_account_text,
	_screen_type_account_list_button_cancel_remove_account_text,
	_screen_type_account_list_button_unnamed_account,

	k_screen_type_account_list_unknown_text
};

enum e_cartographer_screen_type_account_create_string_table
{
	_screen_type_account_create_header_text = 0,
	_screen_type_account_create_subheader_text,

	k_screen_type_account_create_button_text_index,
	_screen_type_account_create_button_placeholder_username_text = k_screen_type_account_list_button_text_index,
	_screen_type_account_create_button_placeholder_email_text,
	_screen_type_account_create_button_placeholder_password_text,
	_screen_type_account_create_button_create_account,

	k_screen_type_account_create_unknown_text
};

enum e_cartographer_screen_type_account_add_string_table
{
	_screen_type_account_add_header_text = 0,
	_screen_type_account_add_subheader_text,

	k_screen_type_account_add_button_text_index,
	_screen_type_account_add_button_placeholder_username_text = k_screen_type_account_add_button_text_index,
	_screen_type_account_add_button_placeholder_password_text,
	_screen_type_account_add_button_remember_me_text,
	_screen_type_account_add_button_dont_remember_me_text,
	// same index, + 1 when parsed
	// ### TODO FIXME improve the system
	_screen_type_account_add_button_login_text = _screen_type_account_add_button_dont_remember_me_text,

	k_screen_type_account_add_unknown_label
};

static void get_cartographer_account_manager_label(e_cartographer_account_manager_screen_type screen_type, int32 label_id, wchar_t** out_label)
{
	wchar_t** label_table[k_language_count][_carrographer_account_manager_screen_type_end] = {};

	static wchar_t* screen_type_list_add_account_eng[] =
	{
		L"Add account",
		L"Enter your account's Username\r\n[or Email] and Password to Login.",
		L"[Username]",
		L"[Password]",
		L"-Remember me",
		L"-Don't remember me",
		L"Login",
		L"<unnamed>"
		L"<unknown label>",
	};

	static wchar_t* screen_type_list_table_eng[] =
	{
		L"Online Accounts",
		L"Select an Account to Sign in to or use options to create/add/remove them.",
		L">Play Offline",
		L">Create Account",
		L">Add Account",
		L"-Remove Account",
		L"-Cancel Remove",
		L"<unknown label>"
	};

	static wchar_t* screen_type_list_create_account_eng[] =
	{
		L"Create Account",
		L"Enter a username, email and password for your new account.",
		L"[Username]",
		L"[Email]",
		L"[Password]",
		L"Create Account",
		L"<unknown label>"
	};
	
	label_table[_language_english][_cartographer_account_manager_screen_type_list] = screen_type_list_table_eng;
	label_table[_language_english][_cartographer_account_manager_screen_type_create_account] = screen_type_list_create_account_eng;
	label_table[_language_english][_cartographer_account_manager_screen_type_add_account] = screen_type_list_add_account_eng;

	// ### TODO FIXME localization
	*out_label = label_table[_language_english][screen_type][label_id];
}

class c_cartographer_account_manager_edit_list : public c_list_widget
{
private:
	// 4 items displayed at once in the list
	c_list_item_widget m_list_item_widgets[4];
	int field_2C0;
	c_slot2<c_cartographer_account_manager_edit_list, s_event_record*, int32> m_slot_2;

public:
	int32 m_default_focused_button;
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
			wchar_t email[128];
			wchar_t password[128];
		} m_account_create;

		struct
		{
			bool remember_me;
			wchar_t email_or_username[128];
			wchar_t password[128];
		} m_account_add;
	};
	// button handler callback

	c_cartographer_account_manager_edit_list(uint16 _flags, int32 _button_count, int32 _default_selected_button, e_cartographer_account_manager_screen_type _screen_type);
	
	virtual ~c_cartographer_account_manager_edit_list()
	{
		switch (m_cartographer_screen_type)
		{
		case _cartographer_account_manager_screen_type_list:
		case _cartographer_account_manager_screen_type_list_remove_account:
			break;
		case _cartographer_account_manager_screen_type_create_account:
			csmemset(&m_account_create, 0, sizeof(m_account_create));
			break;
		case _cartographer_account_manager_screen_type_add_account:
			csmemset(&m_account_add, 0, sizeof(m_account_add));
			break;
		case _cartographer_account_manager_screen_type_none:
		case _carrographer_account_manager_screen_type_end:
		default:
			break;
		}
	}

	virtual c_list_item_widget* get_list_items() override
	{
		return m_list_item_widgets;
	}

	virtual int32 get_list_items_count() override
	{
		return NUMBEROF(m_list_item_widgets);
	}

	void update_list_items(c_list_item_widget* item, int32 skin_index) override
	{
		int32 list_item_index = DATUM_INDEX_TO_ABSOLUTE_INDEX(item->get_last_data_index());

		if (item->get_last_data_index() == NONE)
		{
			return;
		}

		c_text_widget* text_widget = item->try_find_text_widget(0);
		if (text_widget)
		{
			int32 button_count_allocated = m_list_data->datum_max_elements;
			wchar_t* button_label = nullptr;
			wchar_t username_wide[XUSER_NAME_SIZE];

			switch (m_cartographer_screen_type)
			{
			case _cartographer_account_manager_screen_type_list:
			case _cartographer_account_manager_screen_type_list_remove_account:
			{
				// play offline, add account, create account
				int32 default_button_count = 3;
				if (button_count_allocated > default_button_count)
				{
					// we have accounts in the list
					if (list_item_index > button_count_allocated - 1 - 4)
					{
						// button text
						int32 label_index = 4 - (button_count_allocated - list_item_index);
						if (account_removal_mode()
							&& label_index == 3)
						{
							// go to next, which is cancel remove
							label_index += 1;
						}

						get_cartographer_account_manager_label(_cartographer_account_manager_screen_type_list, k_screen_type_account_list_button_text_index + label_index, &button_label);
					}
					else
					{
						if (H2AccountArrayUsername[list_item_index] != NULL)
						{
							swprintf(username_wide, XUSER_NAME_SIZE, L"%S", H2AccountArrayUsername[list_item_index]);
							button_label = username_wide;
						}
						else
						{
							get_cartographer_account_manager_label(_cartographer_account_manager_screen_type_list, _screen_type_account_list_button_unnamed_account, &button_label);
						}
					}
				}
				else
				{
					// just 3 buttons, play offline, create account and add
					int32 label_index = 3 - (button_count_allocated - list_item_index);
					get_cartographer_account_manager_label(_cartographer_account_manager_screen_type_list, k_screen_type_account_list_button_text_index + label_index, &button_label);
				}
			}
			break;
			case _cartographer_account_manager_screen_type_create_account:
				switch (k_screen_type_account_create_button_text_index + list_item_index)
				{
				case _screen_type_account_create_button_placeholder_username_text:
					button_label = m_account_create.user_name;
					break;
				case _screen_type_account_create_button_placeholder_email_text:
					button_label = m_account_create.email;
					break;
				case _screen_type_account_create_button_placeholder_password_text:
					button_label = m_account_create.password;
					break;
				default:
					get_cartographer_account_manager_label(m_cartographer_screen_type, k_screen_type_account_create_button_text_index + list_item_index, &button_label);
					break;
				}
				break;
			case _cartographer_account_manager_screen_type_add_account:
				switch (k_screen_type_account_add_button_text_index + list_item_index)
				{
				case _screen_type_account_add_button_placeholder_username_text:
					button_label = m_account_add.email_or_username;
					break;
				case _screen_type_account_add_button_placeholder_password_text:
					button_label = m_account_add.password;
					break;
				case _screen_type_account_add_button_remember_me_text:
					get_cartographer_account_manager_label(m_cartographer_screen_type, k_screen_type_account_add_button_text_index + list_item_index + (m_account_add.remember_me ? 0 : 1), &button_label);
					break;
				case _screen_type_account_add_button_login_text:
					get_cartographer_account_manager_label(m_cartographer_screen_type, k_screen_type_account_add_button_text_index + list_item_index + 1, &button_label);
					break;
				}
				break;
			case _cartographer_account_manager_screen_type_none:
			case _carrographer_account_manager_screen_type_end:
			default:
				break;
			}

			ASSERT(button_label != NULL);
			text_widget->set_text(button_label);
		}
	}

	bool account_removal_mode() const
	{
		return m_cartographer_screen_type == _cartographer_account_manager_screen_type_list_remove_account;
	}

	// button handlers
	void button_handler(s_event_record* event_record, int32* a3);

	void cartographer_type_list_button_handler(s_event_record* event_record, int32* a3);
	void cartographer_type_add_button_handler(s_event_record* event_record, int32* a3);
	void cartographer_type_create_account(s_event_record* event_record, int32* a3);
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
	static bool accountingGoBackToList;
	static int accountingActiveHandleCount;
	static bool IsAccountingActiveHandle();
	static void UpdateAccountingActiveHandle(bool active);

	static void set_menu_open_context(e_cartographer_account_manager_screen_type screen_type);

	static c_cartographer_account_manager_menu* __cdecl open_account_create_context();
	static c_cartographer_account_manager_menu* __cdecl open_account_add_context();
	static c_cartographer_account_manager_menu* __cdecl open_account_list_context();
	static c_cartographer_account_manager_menu* __cdecl open_account_remove_from_list_context();

	static void* __cdecl open(s_screen_parameters* a1);
	static void* __cdecl open_default_context(s_screen_parameters* a1);

	c_cartographer_account_manager_menu(e_user_interface_channel_type _ui_channel, e_user_interface_render_window _window_index, uint16 _flags, e_cartographer_account_manager_screen_type _screen_type, int32 _button_count, int32 _selected_button);

	virtual ~c_cartographer_account_manager_menu()
	{
		switch (m_cartographer_screen_type)
		{
		case _cartographer_account_manager_screen_type_list:
		case _cartographer_account_manager_screen_type_list_remove_account:
			break;
		case _cartographer_account_manager_screen_type_create_account:
			if (c_cartographer_account_manager_menu::accountingGoBackToList
				&& c_cartographer_account_manager_menu::IsAccountingActiveHandle())
			{
				c_cartographer_account_manager_menu::accountingGoBackToList = false;
			}
			break;

		case _cartographer_account_manager_screen_type_add_account:
			if (c_cartographer_account_manager_menu::accountingGoBackToList
				&& c_cartographer_account_manager_menu::IsAccountingActiveHandle())
			{
				c_cartographer_account_manager_menu::accountingGoBackToList = false;
			}
			break;

		case _carrographer_account_manager_screen_type_end:
		default:
			break;
		}
		UpdateAccountingActiveHandle(false);
	}

	// c_screen_with_menu specific interface
	virtual void initialize(s_screen_parameters* screen_parameters) override
	{
		c_screen_with_menu::initialize(screen_parameters);

		wchar_t* header_text = nullptr;
		wchar_t* subheader_text = nullptr;

		switch (m_cartographer_screen_type)
		{
		case _cartographer_account_manager_screen_type_list:
		case _cartographer_account_manager_screen_type_list_remove_account:
			get_cartographer_account_manager_label(_cartographer_account_manager_screen_type_list, _screen_type_account_list_header_text, &header_text);
			get_cartographer_account_manager_label(_cartographer_account_manager_screen_type_list, _screen_type_account_list_subheader_text, &subheader_text);
			break;
		case _cartographer_account_manager_screen_type_create_account:
			get_cartographer_account_manager_label(m_cartographer_screen_type, _screen_type_account_create_header_text, &header_text);
			get_cartographer_account_manager_label(m_cartographer_screen_type, _screen_type_account_create_subheader_text, &subheader_text);
			break;
		case _cartographer_account_manager_screen_type_add_account:
			get_cartographer_account_manager_label(m_cartographer_screen_type, _screen_type_account_add_header_text, &header_text);
			get_cartographer_account_manager_label(m_cartographer_screen_type, _screen_type_account_add_subheader_text, &subheader_text);
			break;
		case _cartographer_account_manager_screen_type_none:
		case _carrographer_account_manager_screen_type_end:
		default:
			break;
		}

		ASSERT(header_text != NULL && subheader_text != NULL);

		m_header_text.set_text(header_text);
		c_text_widget* subheader_text_widget = try_find_text_widget(2);
		if (subheader_text_widget)
		{
			subheader_text_widget->set_text(subheader_text);
		}
	}

	virtual void post_initialize() override
	{
		m_account_edit_list.set_focused_item_index(m_account_edit_list.m_default_focused_button != NONE ? m_account_edit_list.m_default_focused_button : 0);
		return c_screen_with_menu::post_initialize();
	}

	virtual void* load_proc() override 
	{
		return c_cartographer_account_manager_menu::open_default_context;
	}
};
// ASSERT_STRUCT_SIZE(c_cartographer_account_manager_menu, 3396);

void cartographer_account_manager_open_list();