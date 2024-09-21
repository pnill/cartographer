#include "stdafx.h"
#include "screen_cartographer_errors.h"

#include "screen_cartographer_account_manager.h"

#include "H2MOD/Modules/CustomMenu/CustomMenuGlobals.h"


/* constants */

const s_cartographer_error_globals k_cartographer_error_globals[k_language_count][k_cartographer_error_id_count] = 
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
			L"BANNED Email Provider!",
			L"The Email Address you have entered is using a domain name that has been banned! We do not allow disposable email addresses! If this is a mistake please contact an admin."
		},
		{
			L"Account Successfully Created!",
			L"The account you just entered has been successfully created! You may now use those details to login."
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

void c_cartographer_error_menu::get_error_label(e_cartographer_error_id error_id, wchar_t** out_header_text, wchar_t** out_subheader_text)
{
	*out_header_text = k_cartographer_error_globals[_language_english][error_id].header_text;
	*out_subheader_text = k_cartographer_error_globals[_language_english][error_id].subheader_text;
	return;
}

void* c_cartographer_error_menu::open_by_error_id(e_cartographer_error_id error_id) {

	c_cartographer_error_menu* error_menu = (c_cartographer_error_menu*)ui_custom_cartographer_load_menu(c_cartographer_error_menu::open, 1);
	error_menu->m_error_id = error_id;

	return error_menu;
}

void* __cdecl c_cartographer_error_menu::open(s_screen_parameters* parameters)
{
	c_cartographer_error_menu* error_menu = nullptr;
	BYTE* ui_buffer = ui_pool_allocate_space(sizeof(c_cartographer_error_menu), 0);
	if (ui_buffer) {
		error_menu = new (ui_buffer) c_cartographer_error_menu(parameters->m_channel_type, parameters->m_window_index, parameters->user_flags);
		error_menu->m_allocated	= true;
	}
	user_interface_register_screen_to_channel(error_menu, parameters);
	return error_menu;
}

void c_cartographer_error_edit_list::button_handler(s_event_record* a2, int32* a3)
{
	int button_id = DATUM_INDEX_TO_ABSOLUTE_INDEX(*a3);

	e_user_interface_render_window	parent_render_window = this->get_parent_render_window();
	e_user_interface_channel_type	parent_screen_ui_channel = this->get_parent_channel();

	user_interface_back_out_from_channel(parent_screen_ui_channel, parent_render_window);
	return;
}

c_cartographer_error_edit_list::c_cartographer_error_edit_list(uint32 _flags) :
	c_list_widget(_flags),
	m_field_2C0(0),
	m_slot_2(this, &c_cartographer_error_edit_list::button_handler)
{
	this->m_list_data = nullptr;
	linker_type2.link(&this->m_slot_2);
}

c_cartographer_error_menu::c_cartographer_error_menu(e_user_interface_channel_type _ui_channel, e_user_interface_render_window _window_index, uint16 _flags) :
	c_screen_with_menu(_screen_brightness_level, _ui_channel, _window_index, _flags, &m_error_edit_list),
	m_error_edit_list(_flags)
{
	m_error_id = _cartpgrapher_error_id_none;
}

c_cartographer_error_menu::~c_cartographer_error_menu()
{
	switch (m_error_id)
	{
	case _cartographer_error_id_invalid_login_token:
		if (c_cartographer_account_manager_menu::accountingGoBackToList && c_cartographer_account_manager_menu::IsAccountingActiveHandle()) {
			c_cartographer_account_manager_menu::open_account_add_context();
			c_cartographer_account_manager_menu::accountingGoBackToList = true;
		}
		c_cartographer_account_manager_menu::UpdateAccountingActiveHandle(false);
		break;
	case _cartpgrapher_error_id_none:
	default:
		break;
	}
}
