#include "stdafx.h"
#include "screen_cartographer_account_manager.h"

#include "memory/data.h"
#include "interface/screens/screen_xbox_live_task_progress_dialog.h"

#include "screen_cartographer_errors.h"
#include "screen_virtual_keyboard.h"

#include "H2MOD/GUI/ImGui_Integration/ImGui_Handler.h"

#include "H2MOD/Modules/Accounts/Accounts.h"
#include "H2MOD/Modules/Accounts/AccountLogin.h"
#include "H2MOD/Modules/Accounts/AccountCreate.h"
#include "H2MOD/Modules/CustomMenu/CustomMenuGlobals.h"
#include "H2MOD/Modules/OnScreenDebug/OnscreenDebug.h"
#include "H2MOD/Modules/Shell/Config.h"
#include "H2MOD/Utils/Utils.h"

static bool g_account_manager_remove_mode;
bool c_cartographer_account_manager_menu::accountingGoBackToList = false;
int c_cartographer_account_manager_menu::accountingActiveHandleCount = 0;

struct {
	char email_or_username[128];
	char password[128];
} g_account_add_login_data;

struct {
	char user_name[XUSER_NAME_SIZE];
	char email[128];
	char password[128];
} g_account_create_data;

e_cartographer_account_manager_screen_type g_open_cartographer_account_manager_context = _cartographer_account_manager_screen_type_none;

void c_cartographer_account_manager_menu::set_menu_open_context(e_cartographer_account_manager_screen_type screen_type)
{
	g_open_cartographer_account_manager_context = screen_type;
}

c_cartographer_account_manager_menu* c_cartographer_account_manager_menu::open_account_list_context()
{
	set_menu_open_context(_cartographer_account_manager_screen_type_list);
	return (c_cartographer_account_manager_menu*)ui_custom_cartographer_load_menu(c_cartographer_account_manager_menu::open, 3);
}

c_cartographer_account_manager_menu* c_cartographer_account_manager_menu::open_account_remove_from_list_context()
{
	set_menu_open_context(_cartographer_account_manager_screen_type_list_remove_account);
	return (c_cartographer_account_manager_menu*)ui_custom_cartographer_load_menu(c_cartographer_account_manager_menu::open, 3);
}

c_cartographer_account_manager_menu* c_cartographer_account_manager_menu::open_account_create_context() 
{
	set_menu_open_context(_cartographer_account_manager_screen_type_create_account);
	return (c_cartographer_account_manager_menu*)ui_custom_cartographer_load_menu(c_cartographer_account_manager_menu::open);
}

c_cartographer_account_manager_menu* c_cartographer_account_manager_menu::open_account_add_context() 
{
	set_menu_open_context(_cartographer_account_manager_screen_type_add_account);
	return (c_cartographer_account_manager_menu*)ui_custom_cartographer_load_menu(c_cartographer_account_manager_menu::open);
}

bool c_cartographer_account_manager_menu::IsAccountingActiveHandle() {
	return accountingActiveHandleCount > 0;
}

void c_cartographer_account_manager_menu::UpdateAccountingActiveHandle(bool active) {
	accountingActiveHandleCount += active ? 1 : -1;
	addDebugText("Accounting active: %d", accountingActiveHandleCount);
	if (accountingActiveHandleCount <= 0) {
		SaveH2Accounts();
	}
}

void* c_cartographer_account_manager_menu::open_default_context(s_screen_parameters* parameters)
{
	set_menu_open_context(_cartographer_account_manager_screen_type_list);
	return open(parameters);
}

void* __cdecl c_cartographer_account_manager_menu::open(s_screen_parameters* parameters)
{
	ASSERT(g_open_cartographer_account_manager_context != _cartographer_account_manager_screen_type_none);

	c_cartographer_account_manager_menu* account_list_menu = nullptr;
	uint8* ui_buffer = (uint8*)ui_pool_allocate_space(sizeof(c_cartographer_account_manager_menu), 0);

	int32 button_count = 0;
	int32 selected_button_index = 0;

	c_cartographer_account_manager_menu::UpdateAccountingActiveHandle(true);
	switch (g_open_cartographer_account_manager_context)
	{
	case _cartographer_account_manager_screen_type_list:
		button_count = H2AccountCount + (H2AccountCount > 0 ? 4 : 3);
		selected_button_index = H2AccountLastUsed;
		break;
	case _cartographer_account_manager_screen_type_list_remove_account:
		button_count = H2AccountCount + (H2AccountCount > 0 ? 4 : 3);
		selected_button_index = 0;
		break;
	case _cartographer_account_manager_screen_type_create_account:
		button_count = 4;
		selected_button_index = 0;
		c_cartographer_account_manager_menu::accountingGoBackToList = true;
		break;
	case _cartographer_account_manager_screen_type_add_account:
		button_count = 4;
		selected_button_index = 0;
		c_cartographer_account_manager_menu::accountingGoBackToList = true;
		break;
	case _carrographer_account_manager_screen_type_end:
	case _cartographer_account_manager_screen_type_none:
	default:
		break;
	}

	if (ui_buffer) {
		account_list_menu = new (ui_buffer) c_cartographer_account_manager_menu(
			parameters->m_channel_type, parameters->m_window_index, parameters->user_flags, g_open_cartographer_account_manager_context, button_count, selected_button_index);
		account_list_menu->m_allocated = true;
	}

	g_open_cartographer_account_manager_context = _cartographer_account_manager_screen_type_none;

	user_interface_register_screen_to_channel(account_list_menu, parameters);
	return account_list_menu;
}

c_cartographer_account_manager_menu::c_cartographer_account_manager_menu(e_user_interface_channel_type _ui_channel, e_user_interface_render_window _window_index, uint16 _flags, e_cartographer_account_manager_screen_type _screen_type, int32 _button_count, int32 _selected_button) :
	c_screen_with_menu(_screen_brightness_level, _ui_channel, _window_index, _flags, &this->m_account_edit_list),
	m_account_edit_list(_flags, _button_count, _selected_button, _screen_type)
{
	m_cartographer_screen_type = _screen_type;
}

c_cartographer_account_manager_edit_list::c_cartographer_account_manager_edit_list(uint16 _flags, int32 _button_count, int32 _default_selected_button, e_cartographer_account_manager_screen_type _screen_type) :
	c_list_widget(_flags),
	field_2C0(0),
	m_slot_2(this, &c_cartographer_account_manager_edit_list::button_handler)
{
	this->m_cartographer_screen_type = _screen_type;
	this->m_default_focused_button = _default_selected_button;

	s_data_array* account_list_data = ui_list_data_new("cartographer account manager list", _button_count, 4);
	this->m_list_data = account_list_data;

	data_make_valid(account_list_data);

	for (int32 i = 0; i < this->m_list_data->datum_max_elements; i++) {
		datum_new(this->m_list_data);
	}

	// no fucking clue what's this, maybe related to this->m_slot_2.c_slot_vtbl data offset
	// because this->m_slot_2.c_slot_vtbl is at offset 708
	if ((void*)this == (void*)-(int32)offsetof(c_cartographer_account_manager_edit_list, m_slot_2)) {
		linker_type2.link(nullptr);
	}
	else {
		linker_type2.link(&this->m_slot_2);
	}

	switch (m_cartographer_screen_type)
	{
	case _cartographer_account_manager_screen_type_none:
		break;
	case _cartographer_account_manager_screen_type_list:
	case _cartographer_account_manager_screen_type_list_remove_account:
		break;
	case _cartographer_account_manager_screen_type_create_account:
		csmemset(&m_account_create, 0, sizeof(m_account_create));

		wchar_t* placeholder_username, *placeholder_email, *placeholder_password;
		get_cartographer_account_manager_label(m_cartographer_screen_type, _screen_type_account_create_button_placeholder_username_text, &placeholder_username);
		get_cartographer_account_manager_label(m_cartographer_screen_type, _screen_type_account_create_button_placeholder_email_text, &placeholder_email);
		get_cartographer_account_manager_label(m_cartographer_screen_type, _screen_type_account_create_button_placeholder_password_text, &placeholder_password);
		wcsncpy(m_account_create.user_name, placeholder_username, XUSER_NAME_SIZE);
		wcsncpy(m_account_create.email, placeholder_email, ARRAYSIZE(m_account_create.email));
		wcsncpy(m_account_create.password, placeholder_password, ARRAYSIZE(m_account_create.password));
		break;
	case _cartographer_account_manager_screen_type_add_account:
		csmemset(&m_account_add, 0, sizeof(m_account_add));

		get_cartographer_account_manager_label(m_cartographer_screen_type, _screen_type_account_add_button_placeholder_username_text, &placeholder_username);
		get_cartographer_account_manager_label(m_cartographer_screen_type, _screen_type_account_add_button_placeholder_password_text, &placeholder_password);
		wcsncpy(m_account_add.email_or_username, placeholder_username, ARRAYSIZE(m_account_add.email_or_username));
		wcsncpy(m_account_add.password, placeholder_password, ARRAYSIZE(m_account_add.password));
		break;
	case _carrographer_account_manager_screen_type_end:
	default:
		break;
	}
}

int g_account_manager_master_login_code;
HANDLE g_account_manager_login_thread_handle = INVALID_HANDLE_VALUE;

extern void* ui_load_cartographer_update_notice_menu();

void* ui_load_cartographer_invalid_login_token()
{
	c_cartographer_account_manager_menu::accountingGoBackToList = true;
	c_cartographer_account_manager_menu::UpdateAccountingActiveHandle(true);
	return c_cartographer_error_menu::open_by_error_id(_cartographer_error_id_invalid_login_token);
}

extern bool g_force_cartographer_update;

void xbox_live_task_progress_callback(DWORD a1)
{
	// if the g_account_manager_login_thread_handle handle is INVALID_HANDLE_VALUE, it means that the login thread has ended
	if (g_account_manager_login_thread_handle == INVALID_HANDLE_VALUE)
	{
		// this is the ptr of the callback, if it gets set to null, it will close the menu
		*(DWORD*)(a1 + 2652) = NULL;

		if (g_account_manager_master_login_code < 0)
		{
			switch (g_account_manager_master_login_code)
			{
			case ERROR_CODE_CURL_HANDLE:
			case ERROR_CODE_CURL_SOCKET_FAILED:
			case ERROR_CODE_ACCOUNT_DATA:
			case ERROR_CODE_INVALID_PARAM:
				c_cartographer_error_menu::open_by_error_id(_cartographer_error_id_internal_error);
				return;

			case ERROR_CODE_CURL_EASY_PERF:
				c_cartographer_error_menu::open_by_error_id(_cartographer_error_id_account_login_connection_failed);
				return;

			case ERROR_CODE_INVALID_VERSION:
				ui_load_cartographer_update_notice_menu();
				return;

			case ERROR_CODE_INVALID_LOGIN_TOKEN:
				ui_load_cartographer_invalid_login_token();
				return;

			case ERROR_CODE_INVALID_LOGIN_ID:
				c_cartographer_error_menu::open_by_error_id(_cartographer_error_id_login_invalid_account_id);
				return;

			case ERROR_CODE_INVALID_PASSWORD:
				c_cartographer_error_menu::open_by_error_id(_cartographer_error_id_login_incorrect_password);
				return;

			case ERROR_CODE_MACHINE_BANNED:
				c_cartographer_error_menu::open_by_error_id(_cartographer_error_id_login_machine_banned);
				return;

			case ERROR_CODE_ACCOUNT_BANNED:
				c_cartographer_error_menu::open_by_error_id(_cartographer_error_id_login_account_banned);
				return;

			case ERROR_CODE_ACCOUNT_DISABLED:
				c_cartographer_error_menu::open_by_error_id(_cartographer_error_id_login_account_disabled);
				return;

			case ERROR_CODE_MACHINE_SERIAL_INSUFFICIENT:
				c_cartographer_error_menu::open_by_error_id(_cartographer_error_id_login_insufficient_machine_identifiers);
				return;

			default:
				//unknown error!
				c_cartographer_error_menu::open_by_error_id(_cartographer_error_id_unknown_unhandled_error);
				return;
			}
		}
		else {
			if (g_account_manager_master_login_code == SUCCESS_CODE_MACHINE_SERIAL_INSUFFICIENT) 
			{
				c_cartographer_error_menu::open_by_error_id(_cartographer_error_id_login_insufficient_machine_identifiers);
				return;
			}
			else 
			{
				if (g_force_cartographer_update)
				{
					//Just in case save the config to prevent a high iq individual from getting themsevles in an update loop.
					SaveH2Config();
					g_force_cartographer_update = false;
					ui_load_cartographer_update_notice_menu();
					return;
				}
				ImGuiHandler::ToggleWindow(ImGuiHandler::ImMOTD::windowName);
				// extern int notify_xlive_ui;
				// notify_xlive_ui = 0;
				// sub_248beb_nak_deconstructor_Login_Warn();
				// GSCustomMenuCall_Login_Warn();
				return;
			}
		}
	}
}

DWORD WINAPI thread_account_login_proc_cb(LPVOID lParam)
{
	int button_id = (int)lParam;

	//gotta delay it a little to make sure the menu's decide to render correctly.
	Sleep(200L);

	if (button_id == -1) 
	{
		if (HandleGuiLogin(NULL, g_account_add_login_data.email_or_username, g_account_add_login_data.password, &g_account_manager_master_login_code)) 
		{
			H2AccountLastUsed = 0;
		}
		SecureZeroMemory(g_account_add_login_data.email_or_username, sizeof(g_account_add_login_data.email_or_username));
		SecureZeroMemory(g_account_add_login_data.password, sizeof(g_account_add_login_data.password));
	}
	else 
	{
		//login to account
		if (HandleGuiLogin(H2AccountArrayLoginToken[button_id], NULL, NULL, &g_account_manager_master_login_code))
		{
			H2AccountLastUsed = button_id;
		}
	}

	c_cartographer_account_manager_menu::UpdateAccountingActiveHandle(false);

	g_account_manager_login_thread_handle = INVALID_HANDLE_VALUE;
	return 0;
}

void c_cartographer_account_manager_edit_list::cartographer_type_add_button_handler(s_event_record* event_record, int32* a3)
{
	int button_id = DATUM_INDEX_TO_ABSOLUTE_INDEX(*a3);

	e_user_interface_render_window	parent_render_window = this->get_parent_render_window();
	e_user_interface_channel_type	parent_screen_ui_channel = this->get_parent_channel();

	if (button_id == 0) 
	{
		ui_load_virtual_keyboard(m_account_add.email_or_username, ARRAYSIZE(m_account_add.email_or_username), 16);
	}
	else if (button_id == 1) 
	{
		ui_load_virtual_keyboard(m_account_add.password, ARRAYSIZE(m_account_add.password), 16);
	}
	else if (button_id == 2) 
	{
		m_account_add.remember_me = !m_account_add.remember_me;
		AccountEdit_remember = m_account_add.remember_me;
	}
	else if (button_id == 3) // login button id
	{
		if (g_account_manager_login_thread_handle == INVALID_HANDLE_VALUE) {

			c_cartographer_account_manager_menu::accountingGoBackToList = false;
			c_cartographer_account_manager_menu::UpdateAccountingActiveHandle(true);
			snprintf(g_account_add_login_data.email_or_username, ARRAYSIZE(g_account_add_login_data.email_or_username), "%S", m_account_add.email_or_username);
			snprintf(g_account_add_login_data.password, ARRAYSIZE(g_account_add_login_data.password), "%S", m_account_add.password);
			g_account_manager_login_thread_handle = CreateThread(NULL, 0, thread_account_login_proc_cb, (LPVOID)NONE, 0, NULL);

			c_xbox_live_task_progress_menu::open(xbox_live_task_progress_callback);
			user_interface_back_out_from_channel(parent_screen_ui_channel, parent_render_window);
		}
	}
}

void c_cartographer_account_manager_edit_list::cartographer_type_list_button_handler(s_event_record* event_record, int32* a3)
{
	int button_id = DATUM_INDEX_TO_ABSOLUTE_INDEX(*a3);

	e_user_interface_render_window	parent_render_window = this->get_parent_render_window();
	e_user_interface_channel_type	parent_screen_ui_channel = this->get_parent_channel();

	if (button_id == H2AccountCount + 1) // create account
	{
		if (!this->account_removal_mode()) 
		{
			c_cartographer_account_manager_menu::open_account_create_context();
		}
	}
	else if (button_id == H2AccountCount + 2) // add account
	{
		if (!this->account_removal_mode()) 
		{
			c_cartographer_account_manager_menu::open_account_add_context();
		}
	}
	else if (button_id == H2AccountCount + 3) // account removal mode switch
	{ 
		//remove next selected account.
		g_account_manager_remove_mode = !this->account_removal_mode();
		user_interface_back_out_from_channel(parent_screen_ui_channel, parent_render_window);
		g_account_manager_remove_mode ? c_cartographer_account_manager_menu::open_account_remove_from_list_context() : c_cartographer_account_manager_menu::open_account_list_context();
	}
	/*else if (button_id == H2AccountCount + 4)
	{
		extern void* ui_load_cartographer_guide_menu();
		ui_load_cartographer_guide_menu();
	}*/
	else if (H2AccountCount > 0 && button_id >= 0 && button_id < H2AccountCount) // account selection (either for deletion or login)
	{ 
		if (this->account_removal_mode()) 
		{
			H2AccountAccountRemove(button_id);
			// re-open the menu
			g_account_manager_remove_mode = false; // reset global accountRemove state
			H2AccountLastUsed = 0;
			c_cartographer_account_manager_menu::open_account_list_context();
			user_interface_back_out_from_channel(parent_screen_ui_channel, parent_render_window);
		}
		else if (g_account_manager_login_thread_handle == INVALID_HANDLE_VALUE) 
		{
			c_cartographer_account_manager_menu::accountingGoBackToList = false;
			c_cartographer_account_manager_menu::UpdateAccountingActiveHandle(true);
			g_account_manager_login_thread_handle = CreateThread(NULL, 0, thread_account_login_proc_cb, (LPVOID)button_id, 0, NULL);
			c_xbox_live_task_progress_menu::open(xbox_live_task_progress_callback);
			user_interface_back_out_from_channel(parent_screen_ui_channel, parent_render_window);
		}
	}
	else if (button_id == H2AccountCount) 
	{ // play offline button
		BYTE abEnet[6];
		BYTE abOnline[20];
		XNetRandom(abEnet, sizeof(abEnet));
		XNetRandom(abOnline, sizeof(abOnline));
		g_account_manager_remove_mode = false;
		if (ConfigureUserDetails("[Username]", "12345678901234567890123456789012", rand(), 0, H2Config_ip_lan, ByteToHexStr(abEnet, 6).c_str(), ByteToHexStr(abOnline, 20).c_str(), false))
		{
			//show select profile gui
			extern int notify_xlive_ui;
			notify_xlive_ui = 0;
			H2Config_master_ip = inet_addr("127.0.0.1");
			H2Config_master_port_relay = 2001;
			XUserSignInSetStatusChanged(0);
			user_interface_back_out_from_channel(parent_screen_ui_channel, parent_render_window);
		}
	}
}

HANDLE g_account_manager_thread_handle = 0;

static DWORD WINAPI thread_account_create_proc_cb(LPVOID lParam)
{
	//gotta delay it a little to make sure the menu's decide to render correctly.
	Sleep(200L);

	//submit account creation.
	if (HandleGuiAccountCreate(g_account_create_data.user_name, g_account_create_data.email, g_account_create_data.password))
	{
		c_cartographer_error_menu::open_by_error_id(_cartographer_error_id_account_create_verification_email_sent);

		SecureZeroMemory(g_account_create_data.user_name, sizeof(g_account_create_data.user_name));
		SecureZeroMemory(g_account_create_data.email, sizeof(g_account_create_data.email));
		SecureZeroMemory(g_account_create_data.password, sizeof(g_account_create_data.password));
	}

	c_cartographer_account_manager_menu::UpdateAccountingActiveHandle(false);

	g_account_manager_thread_handle = 0;
	return 0;
}

void c_cartographer_account_manager_edit_list::cartographer_type_create_account(s_event_record* event_record, int32* a3)
{
	int button_id = DATUM_INDEX_TO_ABSOLUTE_INDEX(*a3);

	e_user_interface_render_window	parent_render_window = this->get_parent_render_window();
	e_user_interface_channel_type	parent_screen_ui_channel = this->get_parent_channel();

	if (button_id == 0) 
	{
		ui_load_virtual_keyboard(m_account_create.user_name, ARRAYSIZE(m_account_create.user_name) /* Wide string buffer size */, 0);
	}
	else if (button_id == 1) 
	{
		ui_load_virtual_keyboard(m_account_create.email, ARRAYSIZE(m_account_create.email), 16);
	}
	else if (button_id == 2) 
	{
		ui_load_virtual_keyboard(m_account_create.password, ARRAYSIZE(m_account_create.password), 16);
	}
	else if (button_id == 3) 
	{
		if (!g_account_manager_thread_handle) 
		{
			c_cartographer_account_manager_menu::accountingGoBackToList = false;
			c_cartographer_account_manager_menu::UpdateAccountingActiveHandle(true);

			snprintf(g_account_create_data.user_name, ARRAYSIZE(g_account_create_data.user_name), "%S", m_account_create.user_name);
			snprintf(g_account_create_data.email, ARRAYSIZE(g_account_create_data.email), "%S", m_account_create.email);
			snprintf(g_account_create_data.password, ARRAYSIZE(g_account_create_data.password), "%S", m_account_create.password);

			g_account_manager_login_thread_handle = CreateThread(NULL, 0, thread_account_login_proc_cb, (LPVOID)NONE, 0, NULL);

			user_interface_back_out_from_channel(parent_screen_ui_channel, parent_render_window);
			c_cartographer_error_menu::open_by_error_id(_cartographer_error_id_account_create_processing_account_notice);
			g_account_manager_thread_handle = CreateThread(NULL, 0, thread_account_create_proc_cb, (LPVOID)0, 0, NULL);
		}
	}

	return;
}

void c_cartographer_account_manager_edit_list::button_handler(s_event_record* event_record, int32* a3)
{
	switch (m_cartographer_screen_type)
	{
	case _cartographer_account_manager_screen_type_list:
	case _cartographer_account_manager_screen_type_list_remove_account:
		cartographer_type_list_button_handler(event_record, a3);
		break;
	case _cartographer_account_manager_screen_type_create_account:
		cartographer_type_create_account(event_record, a3);
		break;
	case _cartographer_account_manager_screen_type_add_account:
		cartographer_type_add_button_handler(event_record, a3);
		break;

	case _carrographer_account_manager_screen_type_end:
	default:
		break;
	}

	return;
}

void cartographer_account_manager_open_list() {
	if (!c_cartographer_account_manager_menu::IsAccountingActiveHandle() && ReadH2Accounts()) {
		c_cartographer_account_manager_menu::open_account_list_context();
	}
	else {
		if (!c_cartographer_account_manager_menu::IsAccountingActiveHandle())
			c_cartographer_error_menu::open_by_error_id(_cartographer_error_id_login_account_already_in_use);
	}
}


