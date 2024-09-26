#pragma once

#include "interface/signal_slot.h"
#include "interface/user_interface_widget_list.h"
#include "interface/user_interface_widget_window.h"

/* enums */

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

class c_cartographer_error_menu : protected c_screen_widget
{
protected:
	e_cartographer_error_id m_error_id;
	static void get_error_label(e_cartographer_error_id error_id, wchar_t** out_header_text, wchar_t** out_subheader_text);

public:
	static void* load_by_error_id(e_cartographer_error_id error_id);
	static void* load(s_screen_parameters* a1);

	c_cartographer_error_menu(e_user_interface_channel_type _ui_channel, e_user_interface_render_window _window_index, uint16 _flags);


	// c_cartographer_error_menu virtual functions

	virtual ~c_cartographer_error_menu() = default;
	virtual void pre_destroy() override;
	virtual bool handle_event(s_event_record* event) override;
	virtual void initialize(s_screen_parameters* screen_parameters) override;
	virtual void* load_proc() override;

private:
};
