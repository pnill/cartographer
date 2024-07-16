#include "stdafx.h"

#include "c_account_create_menu.h"

#include "memory/data.h"
#include "H2MOD/Modules/Accounts/AccountCreate.h"

#include "../c_error_menu.h"
#include "../c_virtual_keyboard_menu.h"
#include "../CustomLanguage.h"

HANDLE hThreadCreate = 0;

static DWORD WINAPI AccountCreateThread(LPVOID lParam)
{
	//gotta delay it a little to make sure the menu's decide to render correctly.
	Sleep(200L);

	char* username = H2CustomLanguageGetLabel(CMLabelMenuId_AccountCreate, 1);
	char* email = H2CustomLanguageGetLabel(CMLabelMenuId_AccountCreate, 2);
	char* pass = H2CustomLanguageGetLabel(CMLabelMenuId_AccountCreate, 3);
	//submit account creation.
	if (HandleGuiAccountCreate(username, email, pass)) {
		//account created.
		//GSCustomMenuCall_Error_Inner(CMLabelMenuId_Error, 0xFFFFF026, 0xFFFFF027);

		//verification email.
		CustomMenuCall_Error_Inner(CMLabelMenuId_Error, 0xFFFFF028, 0xFFFFF029);

		char* username2 = H2CustomLanguageGetLabel(CMLabelMenuId_AccountEdit, 1);
		snprintf(username2, strlen(username) + 1, username);
		char* pass2 = H2CustomLanguageGetLabel(CMLabelMenuId_AccountEdit, 2);
		snprintf(pass2, strlen(pass) + 1, pass);

		SecureZeroMemory(username, strlen(username));
		SecureZeroMemory(email, strlen(email));
		SecureZeroMemory(pass, strlen(pass));
	}

	c_account_list_menu::UpdateAccountingActiveHandle(false);

	hThreadCreate = 0;
	return 0;
}

static void AccountCreateSetupButtonLabels() {
	char* bufferUsername = H2CustomLanguageGetLabel(CMLabelMenuId_AccountCreate, 1);
	if (strlen(bufferUsername) <= 0) {
		char* bufferUsername2 = H2CustomLanguageGetLabel(CMLabelMenuId_AccountCreate, 0xFFFFFFF2);
		strcpy_s(bufferUsername, XUSER_NAME_SIZE, bufferUsername2);
	}
	char* bufferEmail = H2CustomLanguageGetLabel(CMLabelMenuId_AccountCreate, 2);
	if (strlen(bufferEmail) <= 0) {
		char* bufferEmail2 = H2CustomLanguageGetLabel(CMLabelMenuId_AccountCreate, 0xFFFFFFF3);
		strcpy_s(bufferEmail, 256, bufferEmail2);
	}
	char* bufferPassword = H2CustomLanguageGetLabel(CMLabelMenuId_AccountCreate, 3);
	if (strlen(bufferPassword) <= 0) {
		char* bufferPassword2 = H2CustomLanguageGetLabel(CMLabelMenuId_AccountCreate, 0xFFFFFFF4);
		strcpy_s(bufferPassword, 256, bufferPassword2);
	}
}

void* __cdecl c_account_create_menu::open(s_new_ui_screen_parameters* parameters)
{
	c_account_create_menu* account_list_menu = nullptr;
	BYTE* ui_buffer = (BYTE*)ui_memory_pool_allocate(sizeof(c_account_create_menu), 0);

	c_account_list_menu::accountingGoBackToList = true;
	c_account_list_menu::UpdateAccountingActiveHandle(true);
	AccountCreateSetupButtonLabels();

	if (ui_buffer) {
		account_list_menu = new (ui_buffer) c_account_create_menu(parameters->ui_channel, parameters->field_8, HIWORD(parameters->flags)); // manually call the constructor
	}

	account_list_menu->field_6C = true;
	user_interface_register_screen_to_channel(account_list_menu, parameters);
	return account_list_menu;
}

c_account_create_menu::c_account_create_menu(int _ui_channel, int a4, int _flags) :
	c_screen_with_menu(BRIGHTNESS_MENU_ID, _ui_channel, a4, _flags, &this->account_edit_list, true),
	account_edit_list(_flags)
{
}

c_account_create_list::c_account_create_list(int _flags) :
	c_list_widget(_flags, true)
{
	auto p_sub_2113C6 = Memory::GetAddress<int(__thiscall*)(void*)>(0x2113C6);
	auto p_sub_2113D3 = Memory::GetAddress<void* (__thiscall*)(void*, void*)>(0x2113D3);
	auto c_list_item_widget_ctor = Memory::GetAddress<void* (__thiscall*)(void*)>(0x21FFC9);
	auto c_list_item_widget_dctor = Memory::GetAddress<void(__thiscall*)(void*)>(0xA551);
	auto p_vector_constructor = Memory::GetAddress<void(__stdcall*)(void*, unsigned int, int, void* (__thiscall*)(void*), void(__thiscall*)(void*))>(0x28870B);

	// this constructs the buttons list widgets drawn inside the window
	// brightness menu can display 4 widget list buttons at once
	p_vector_constructor(this->item_list, 132, 4, c_list_item_widget_ctor, c_list_item_widget_dctor);

	this->field_2C0 = 0;
	p_sub_2113C6(&this->slot_2_unk.field_8);

	//this->slot_2_unk.c_slot_vtbl = &off_1229700;
	// this->slot_2_unk.c_slot_vtbl = NULL;
	// not sure why this exists, it just calls the function to the button handler
	// TODO implement virtual interface for slot_1, slot_2
	this->slot_2_unk.c_slot_vtbl = Memory::GetAddressRelative<void*>(0x7D9700);
	this->slot_2_unk.ui_screen_edit_list = this;
	this->slot_2_unk.button_handler_cb = &c_account_create_list::button_handler;

	int button_count = 4;
	s_data_array* account_create_list_data = allocate_list_data("account create edit list", button_count, 4);
	this->list_data_array = account_create_list_data;

	s_data_array::data_make_valid(this->list_data_array);

	for (int32 i = 0; i < this->list_data_array->datum_max_elements; i++) {
		s_data_array::datum_new_in_range(this->list_data_array);
	}

	// no fucking clue what's this, maybe related to this->slot_2_unk.c_slot_vtbl data offset
	// because this->slot_2_unk.c_slot_vtbl is at offset 708
	if ((void*)this == (void*)-(offsetof(c_account_create_list, slot_2_unk.c_slot_vtbl))) {
		p_sub_2113D3(&this->gap_70[56], nullptr);
	}
	else {
		p_sub_2113D3(&this->gap_70[56], &this->slot_2_unk.field_8);
	}
}

void c_account_create_list::button_handler(int* a2, int* a3)
{
	int button_id = *(int*)a3 & 0xFFFF;

	int parent_screen_idx = this->get_top_most_parent_widget_index();
	int parent_screen_ui_channel = this->get_top_most_parent_widget_ui_channel();

	bool close_parent_screen = false;

	if (button_id == 0) {
		wchar_t* textBuffer = (wchar_t*)H2CustomLanguageGetLabel(CMLabelMenuId_AccountCreate, 1); // account name
		CustomMenuCall_VKeyboard_Inner(textBuffer, XUSER_NAME_SIZE /* Wide string buffer size */, 0, CMLabelMenuId_AccountCreate, 0xFFFFF002, CMLabelMenuId_AccountCreate, 0xFFFFF003);
	}
	else if (button_id == 1) {
		wchar_t* textBuffer = (wchar_t*)H2CustomLanguageGetLabel(CMLabelMenuId_AccountCreate, 2); // password
		CustomMenuCall_VKeyboard_Inner(textBuffer, 256, 16, CMLabelMenuId_AccountCreate, 0xFFFFF004, CMLabelMenuId_AccountCreate, 0xFFFFF005);
	}
	else if (button_id == 2) {
		wchar_t* textBuffer = (wchar_t*)H2CustomLanguageGetLabel(CMLabelMenuId_AccountCreate, 3); // email
		CustomMenuCall_VKeyboard_Inner(textBuffer, 256, 16, CMLabelMenuId_AccountCreate, 0xFFFFF006, CMLabelMenuId_AccountCreate, 0xFFFFF007);
	}
	else if (button_id == 3) {
		if (!hThreadCreate) {
			c_account_list_menu::accountingGoBackToList = false;
			c_account_list_menu::UpdateAccountingActiveHandle(true);
			CustomMenuCall_Error_Inner(CMLabelMenuId_Error, 0xFFFFF02C, 0xFFFFF02D);
			hThreadCreate = CreateThread(NULL, 0, AccountCreateThread, (LPVOID)0, 0, NULL);
			close_parent_screen = true;
		}
	}

	if (close_parent_screen)
		user_interface_back_out_from_channel(parent_screen_ui_channel, parent_screen_idx);

	return;
}