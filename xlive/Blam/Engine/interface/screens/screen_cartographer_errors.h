#pragma once

#include "interface/signal_slot.h"
#include "interface/user_interface_widget_list.h"
#include "interface/user_interface_widget_window.h"

enum e_cartographer_error_id
{
	_cartographer_error_id_generic_error,
	_cartographer_error_id_error_outdated_version,
	_cartographer_error_id_invalid_login_token,
	_cartographer_error_id_login_invalid_account_id,
	_cartographer_error_id_login_incorrect_password,
	_cartographer_error_id_login_machine_banned,
	_cartographer_error_id_login_account_banned,
	_cartographer_error_id_login_account_disabled,
	_cartographer_error_id_unknown_unhandled_error,
	_cartographer_error_id_internal_error,
	_cartographer_error_id_login_account_already_in_use,
	_cartographer_error_id_login_insufficient_machine_identifiers,
	_cartographer_error_id_account_create_invalid_email,
	_cartographer_error_id_account_create_invalid_username,
	_cartographer_error_id_account_create_invalid_password,
	_cartographer_error_id_account_create_email_already_used,
	_cartographer_error_id_account_create_username_taken,
	_cartographer_error_id_account_create_blacklisted_email_provider,
	_cartographer_error_id_account_create_success,
	_cartographer_error_id_account_create_verification_email_sent,
	_cartographer_error_id_account_create_processing_account_notice,
	_cartographer_error_id_account_login_please_wait_notice,
	_cartographer_error_id_account_login_connection_failed,
	_cartographer_error_id_setting_requiring_game_restart,

	k_cartographer_error_id_count,
	_cartpgrapher_error_id_none = NONE,
};

/* structures */

struct s_cartographer_error_globals
{
	wchar_t* header_text;
	wchar_t* subheader_text;
};

/* classes */

class c_cartographer_error_edit_list : public c_list_widget
{
public:
	int m_field_2C0;
	c_slot2<c_cartographer_error_edit_list, s_event_record*, int32> m_slot_2;

	c_cartographer_error_edit_list(uint32 _flags);

	virtual c_list_item_widget* get_list_items() override
	{
		return nullptr; // returns pointer to edit list
	}

	virtual int32 get_list_items_count() override
	{
		return 0;
	}

	void update_list_items(c_list_item_widget* item, int32 skin_index) override
	{
	}

	// button handler
	void button_handler(s_event_record* a2, int32* a3);
};

class c_cartographer_error_menu : protected c_screen_with_menu
{
public:
	c_cartographer_error_edit_list m_error_edit_list;
	e_cartographer_error_id m_error_id;

	static void* open_by_error_id(e_cartographer_error_id error_id);
	static void* __cdecl open(s_screen_parameters* a1);

	static void get_error_label(e_cartographer_error_id error_id, wchar_t** out_header_text, wchar_t** out_subheader_text);

	c_cartographer_error_menu(e_user_interface_channel_type _ui_channel, e_user_interface_render_window _window_index, uint16 _flags);

	~c_cartographer_error_menu();

	// interface
	virtual c_user_interface_widget* destructor(uint32 flags) override
	{
		this->~c_cartographer_error_menu();

		return this;
	};

	// c_screen_with_menu specific interface
	virtual void initialize(s_screen_parameters* screen_parameters) override
	{
		c_screen_with_menu::initialize(screen_parameters);

		wchar_t* header_text = L"<unknown-error>";
		wchar_t* subheader_text = L"<unknown-error-subheader>";

		if (m_error_id != _cartpgrapher_error_id_none)
		{
			get_error_label(m_error_id, &header_text, &subheader_text);
		}

		m_header_text.set_text(header_text);
		c_text_widget* subheader_text_widget = try_find_text_widget(2);
		if (subheader_text_widget)
		{
			subheader_text_widget->set_text(subheader_text);
		}
	}

	virtual void* load_proc() override
	{
		return c_cartographer_error_menu::open;
	}
private:
};
