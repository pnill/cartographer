#include "stdafx.h"

#include "screen_cartographer_errors.h"
#include "screen_cartographer_account_manager.h"

#include "H2MOD/Modules/CustomMenu/CustomMenuGlobals.h"

void c_cartographer_error_menu::get_error_label(e_cartographer_error_id error_id, wchar_t** out_header_text, wchar_t** out_subheader_text)
{
	wchar_t* error_menu_header_text		[k_language_count][k_cartographer_error_id_end] = {};
	wchar_t* error_menu_subheader_text	[k_language_count][k_cartographer_error_id_end] = {};

	error_menu_header_text		[_language_english][_cartographer_error_id_generic_error] = L"Error!";
	error_menu_subheader_text	[_language_english][_cartographer_error_id_generic_error] =	L"Generic Error.";
	error_menu_header_text		[_language_english][_cartographer_error_id_no_custom_language_categorised_as_other] = L"None";
	error_menu_subheader_text	[_language_english][_cartographer_error_id_no_custom_language_categorised_as_other] = L"There are no custom languages catergorised as Other.";
	error_menu_header_text		[_language_english][_cartographer_error_id_error_reading_custom_language_file] = L"Error";
	error_menu_subheader_text	[_language_english][_cartographer_error_id_error_reading_custom_language_file] = L"An error occured when trying to read the custom language file.\r\nNo Changes have been made.\r\nReview the on screen debug log for more details.";
	error_menu_header_text		[_language_english][_cartographer_error_id_error_outdated_version] = L"Outdated Version!";
	error_menu_subheader_text	[_language_english][_cartographer_error_id_error_outdated_version] = L"You are using an outdated version of Project Cartographer! Please install the latest version.";
	error_menu_header_text		[_language_english][_cartographer_error_id_invalid_login_token] = L"Invalid Login Token!";
	error_menu_subheader_text	[_language_english][_cartographer_error_id_invalid_login_token] = L"Login Again.";
	error_menu_header_text		[_language_english][_cartographer_error_id_login_invalid_account_id] = L"Invalid Account ID!";
	error_menu_subheader_text	[_language_english][_cartographer_error_id_login_invalid_account_id] = L"The Username or Email Address you entered is not one of an existing account. Please check your spelling or create a new account if you don't have one already.";
	error_menu_header_text		[_language_english][_cartographer_error_id_login_incorrect_password] = L"Incorrect Password!";
	error_menu_subheader_text	[_language_english][_cartographer_error_id_login_incorrect_password] = L"The password you entered is incorrect!";
	error_menu_header_text		[_language_english][_cartographer_error_id_login_machine_banned] = L"BANNED!";
	error_menu_subheader_text	[_language_english][_cartographer_error_id_login_machine_banned] = L"The computer you are currently using is banned! If you think this is a mistake, post on the online forum pleading your innocence; if you have any you cheater!";
	error_menu_header_text		[_language_english][_cartographer_error_id_login_account_banned] = L"BANNED!";
	error_menu_subheader_text	[_language_english][_cartographer_error_id_login_account_banned] = L"Your account is banned! If you think this is a mistake, post on the online forum pleading your innocence; if you have any you cheater!";
	error_menu_header_text		[_language_english][_cartographer_error_id_login_account_disabled] = L"Account Disabled!";
	error_menu_subheader_text	[_language_english][_cartographer_error_id_login_account_disabled] = L"Your account is currently disabled, closed or deleted! Please contact an administrator if this was a mistake.";
	error_menu_header_text		[_language_english][_cartographer_error_id_unknown_unhandled_error] = L"Unknown Error!?";
	error_menu_subheader_text	[_language_english][_cartographer_error_id_unknown_unhandled_error] = L"The error that has occured has not been handled. Please contact an admin including any log files you have and what you did.";
	error_menu_header_text		[_language_english][_cartographer_error_id_internal_error] = L"Something's Broken!";
	error_menu_subheader_text	[_language_english][_cartographer_error_id_internal_error] = L"An internal error has occured. We've probably noticed this already but contact an admin if this persists.";
	error_menu_header_text		[_language_english][_cartographer_error_id_login_account_already_in_use] = L"Accounts in Use";
	error_menu_subheader_text	[_language_english][_cartographer_error_id_login_account_already_in_use] = L"Another instance of Halo 2 / H2Server is currently signing in, please try again after it finishes.";
	error_menu_header_text		[_language_english][_cartographer_error_id_login_insufficient_machine_identifiers] = L"Insufficient PC Identification!";
	error_menu_subheader_text	[_language_english][_cartographer_error_id_login_insufficient_machine_identifiers] = L"Your PC does not have sufficient unique identifiers available. Please contact an admin about linking this PC with another that you own that does have sufficient info. This is to ensure fair online play.";
	error_menu_header_text		[_language_english][_cartographer_error_id_account_create_invalid_email] = L"Invalid Email!";
	error_menu_subheader_text	[_language_english][_cartographer_error_id_account_create_invalid_email] = L"The Email address you have entered is invalid! Please double check your spelling.";
	error_menu_header_text		[_language_english][_cartographer_error_id_account_create_invalid_username] = L"Invalid Username!";
	error_menu_subheader_text	[_language_english][_cartographer_error_id_account_create_invalid_username] = L"The Username you have entered is invalid! Please ensure you have formed it correctly using only allowed symbols!";
	error_menu_header_text		[_language_english][_cartographer_error_id_account_create_invalid_password] = L"Invalid Password!";
	error_menu_subheader_text	[_language_english][_cartographer_error_id_account_create_invalid_password] = L"The Password you have entered is invalid! Please ensure you have formed it correctly using only allowed symbols!";
	error_menu_header_text		[_language_english][_cartographer_error_id_account_create_email_already_used] = L"Email Already in Use!";
	error_menu_subheader_text	[_language_english][_cartographer_error_id_account_create_email_already_used] = L"The Email Address you have entered is already in use! You cannot use an email for multiple accounts. Please use a different email, sign in to that account or reset its password if you have forgotton it.";
	error_menu_header_text		[_language_english][_cartographer_error_id_account_create_username_taken] = L"Username Taken!";
	error_menu_subheader_text	[_language_english][_cartographer_error_id_account_create_username_taken] = L"The Username you have entered is already in use!";
	error_menu_header_text		[_language_english][_cartographer_error_id_account_create_blacklisted_email_provider] = L"BANNED Email Provider!";
	error_menu_subheader_text	[_language_english][_cartographer_error_id_account_create_blacklisted_email_provider] = L"The Email Address you have entered is using a domain name that has been banned! We do not allow disposable email addresses! If this is a mistake please contact an admin.";
	error_menu_header_text		[_language_english][_cartographer_error_id_account_create_success] = L"Account Successfully Created!";
	error_menu_subheader_text	[_language_english][_cartographer_error_id_account_create_success] = L"The account you just entered has been successfully created! You may now use those details to login.";
	error_menu_header_text		[_language_english][_cartographer_error_id_account_create_verification_email_sent] = L"Verification Email Sent!";
	error_menu_subheader_text	[_language_english][_cartographer_error_id_account_create_verification_email_sent] = L"An email has been sent to the email address submitted. Please follow the instuctions in the email to activate your account.";
	error_menu_header_text		[_language_english][_cartographer_error_id_account_create_processing_account_notice] = L"Creating Account...";
	error_menu_subheader_text	[_language_english][_cartographer_error_id_account_create_processing_account_notice] = L"Processing your new account...\r\nPlease wait.";
	error_menu_header_text		[_language_english][_cartographer_error_id_account_login_please_wait_notice] = L"Logging in...";
	error_menu_subheader_text	[_language_english][_cartographer_error_id_account_login_please_wait_notice] = L"Please wait while you are being logged in.";
	error_menu_header_text		[_language_english][_cartographer_error_id_account_login_connection_failed] = L"Connection Failed!";
	error_menu_subheader_text	[_language_english][_cartographer_error_id_account_login_connection_failed] = L"Please visit:\r\nhttps://halo2pc.com/connection\r\nfor help with troubleshooting potential issues.";
	error_menu_header_text		[_language_english][_cartographer_error_id_setting_requiring_game_restart] = L"Restart Required";
	error_menu_subheader_text	[_language_english][_cartographer_error_id_setting_requiring_game_restart] = L"The setting you have just changed requires that you restart your game for it to take effect.";

	*out_header_text = error_menu_header_text[_language_english][error_id];
	*out_subheader_text = error_menu_subheader_text[_language_english][error_id];
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

	// no fucking clue what's this, maybe related to this->m_slot_2.c_slot_vtbl data offset
	// because this->m_slot_2.c_slot_vtbl is at offset 708
	if ((void*)this == (void*)-offsetof(c_cartographer_error_edit_list, m_slot_2)) {
		linker_type2.link(nullptr);
	}
	else {
		linker_type2.link(&this->m_slot_2);
	}
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
