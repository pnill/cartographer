#include "stdafx.h"
#include "screen_cartographer_errors.h"

#include "screen_cartographer_account_manager.h"

#include "input/input_windows.h"
#include "interface/user_interface.h"
#include "interface/user_interface_controller.h"
#include "interface/user_interface_memory.h"
#include "interface/user_interface_utilities.h"
#include "main/game_preferences.h"
#include "H2MOD/Modules/Accounts/Accounts.h"

/* structures */

struct s_cartographer_error_globals
{
	const wchar_t* header_text;
	const wchar_t* subheader_text;
};

/* constants */

static const s_cartographer_error_globals k_cartographer_error_globals[k_language_count][k_cartographer_error_id_count] =
{
	{
		{
			L"Error!",
			L"Generic Error."
		},
		{
			L"Outdated Version!",
			L"You are using an outdated version of Project Cartographer! Please install the latest version.",
		},
		{
			L"Invalid Login Token!",
			L"Login Again."
		},
		{
			L"Invalid Account ID!",
			L"The Username or Email Address you entered is not one of an existing account. Please check your spelling or create a new account if you don't have one already."
		},
		{
			L"Incorrect Password!",
			L"The password you entered is incorrect!"
		},
		{
			L"BANNED!",
			L"The computer you are currently using is banned! If you think this is a mistake, post on the online forum pleading your innocence; if you have any you cheater!"
		},
		{
			L"BANNED!",
			L"Your account is banned! If you think this is a mistake, post on the online forum pleading your innocence; if you have any you cheater!",
		},
		{
			L"Account Disabled!",
			L"Your account is currently disabled, closed or deleted! Please contact an administrator if this was a mistake."
		},
		{
			L"Unknown Error!?",
			L"The error that has occured has not been handled. Please contact an admin including any log files you have and what you did."
		},
		{
			L"Something's Broken!",
			L"An internal error has occured. We've probably noticed this already but contact an admin if this persists."
		},
		{
			L"Accounts in Use",
			L"Another instance of Halo 2 / H2Server is currently signing in, please try again after it finishes."
		},
		{
			L"Insufficient PC Identification!",
			L"Your PC does not have sufficient unique identifiers available. Please contact an admin about linking this PC with another that you own that does have sufficient info. This is to ensure fair online play."
		},
		{
			L"Invalid Email!",
			L"The Email address you have entered is invalid! Please double check your spelling."
		},
		{
			L"Invalid Username!",
			L"The Username you have entered is invalid! Please ensure you have formed it correctly using only allowed symbols!"
		},
		{
			L"Invalid Password!",
			L"The Password you have entered is invalid! Please ensure you have formed it correctly using only allowed symbols!"
		},
		{
			L"Email Already in Use!",
			L"The Email Address you have entered is already in use! You cannot use an email for multiple accounts. Please use a different email, sign in to that account or reset its password if you have forgotton it."
		},
		{
			L"Username Taken!",
			L"The Username you have entered is already in use!"
		},
		{
			L"Password mismatch!",
			L"Password confirmation does not match the actual password!"
		},
		{
			L"BANNED Email Provider!",
			L"The Email Address you have entered is using a domain name that has been banned! We do not allow disposable email addresses! If this is a mistake please contact an admin."
		},
		{
			L"Verification Email Sent!",
			L"An email has been sent to the email address submitted. Please follow the instuctions in the email to activate your account."
		},
		{
			L"Creating Account...",
			L"Processing your new account...\r\nPlease wait."
		},
		{
			L"Logging in...",
			L"Please wait while you are being logged in."
		},
		{
			L"Connection Failed!",
			L"Please visit:\r\nhttps://halo2pc.com/connection\r\nfor help with troubleshooting potential issues."
		},
		{
			L"Restart Required",
			L"The setting you have just changed requires that you restart your game for it to take effect."
		},
	},
};

/* public code */

void c_cartographer_error_menu::get_error_label(e_cartographer_error_id error_id, const wchar_t** out_header_text, const wchar_t** out_subheader_text)
{
	*out_header_text = k_cartographer_error_globals[_language_english][error_id].header_text;
	*out_subheader_text = k_cartographer_error_globals[_language_english][error_id].subheader_text;
	return;
}

void* c_cartographer_error_menu::load_by_error_id(e_cartographer_error_id error_id) {

	c_screen_parameters params;
	c_cartographer_error_menu* error_menu = NULL;

	params.initialize_default_user(
		user_interface_controller_get_signed_in_controllers_mask() | FLAG(k_windows_device_controller_index),
		_user_interface_channel_type_game_error,
		_window_4,
		c_cartographer_error_menu::load
	);

	error_menu = (c_cartographer_error_menu*)params.execute_load_function();

	error_menu->m_error_id = error_id;
	return error_menu;
}

void* c_cartographer_error_menu::load(c_screen_parameters* parameters)
{
	c_cartographer_error_menu* error_menu = nullptr;
	BYTE* ui_buffer = ui_pool_allocate_space(sizeof(c_cartographer_error_menu), 0);
	if (ui_buffer) {
		error_menu = new (ui_buffer) c_cartographer_error_menu(
			parameters->get_channel_type(),
			parameters->get_window_index(),
			parameters->get_user_flags()
		);
		
		error_menu->m_allocated	= true;
	}
	user_interface_register_screen_to_channel(error_menu, parameters);
	return error_menu;
}


c_cartographer_error_menu::c_cartographer_error_menu(e_user_interface_channel_type _ui_channel, e_user_interface_render_window _window_index, uint16 _flags) :
	c_screen_widget(_screen_error_dialog_ok, _ui_channel, _window_index, _flags)
{
	m_error_id = _cartpgrapher_error_id_none;
}

void c_cartographer_error_menu::pre_destroy()
{
	switch (m_error_id)
	{
	case _cartographer_error_id_invalid_login_token:
		if (c_cartographer_account_manager_menu::g_accounting_go_back_to_list && c_cartographer_account_manager_menu::is_accounting_active_handle()) 
		{
			c_cartographer_account_manager_menu::load_for_account_add_context();
			c_cartographer_account_manager_menu::g_accounting_go_back_to_list = true;
		}

		// Make sure we read the accounts file again so we can modify it later on
		ReadH2Accounts();
		c_cartographer_account_manager_menu::update_accounting_active_handle(false);
		break;
	case _cartpgrapher_error_id_none:
	default:
		break;
	}
}

bool c_cartographer_error_menu::handle_event(s_event_record* event)
{
	bool result = false;
	if (event->type == _user_interface_event_type_keyboard_button_pressed
		|| event->type == _user_interface_event_type_gamepad_button_pressed
		|| event->type == _user_interface_event_type_mouse_button_left_click)
	{
		if (event->component == _user_interface_keyboard_component_button_letter_a
			|| event->component == _controller_component_button_a)
		{
			start_widget_animation(3);
			result = true;
		}
	}

	//return c_screen_widget::handle_event(event);
	return result;
}

void c_cartographer_error_menu::initialize(c_screen_parameters const* parameters)
{
	s_interface_expected_screen_layout layout;
	csmemset(&layout, 0, sizeof(layout));
	layout.panes_count = 1;

	datum widget_tag_datum = user_interface_get_screen_tag_index_by_id(this->m_screen_id);
	if (widget_tag_datum != NONE)
	{
		this->verify_and_load_from_layout(widget_tag_datum, &layout);
	}
	this->setup_children();

	//update header and subheader labels

	const wchar_t* header_text = L"<unknown-error>";
	const wchar_t* subheader_text = L"<unknown-error-subheader>";

	if (m_error_id != _cartpgrapher_error_id_none)
	{
		get_error_label(m_error_id, &header_text, &subheader_text);
	}

	m_header_text.set_text(header_text);
	c_text_widget* subheader_text_widget = try_find_text_widget(k_sub_header_text_block_index);
	if (subheader_text_widget)
	{
		subheader_text_widget->set_text(subheader_text);
	}
}

const void* c_cartographer_error_menu::load_proc(void) const
{
	return c_cartographer_error_menu::load;
}
