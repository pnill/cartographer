#include "stdafx.h"
#include "c_account_list_menu.h"

#include "Blam\Engine\DataArray\DataArray.h"

#include "H2MOD\Modules\Accounts\Accounts.h"
#include "H2MOD\Modules\Accounts\AccountLogin.h"
#include "H2MOD\Modules\UI\XboxLiveTaskProgress.h"
#include "H2MOD\Modules\OnScreenDebug\OnscreenDebug.h"

#include "..\CustomLanguage.h"

#include "H2MOD\Modules\Shell\Config.h"

#include "H2MOD\Utils\Utils.h"

static bool accountRemoveMode;
bool c_account_list_menu::accountingGoBackToList = false;
int c_account_list_menu::accountingActiveHandleCount = 0;

bool c_account_list_menu::IsAccountingActiveHandle() {
	return accountingActiveHandleCount > 0;
}

void c_account_list_menu::UpdateAccountingActiveHandle(bool active) {
	accountingActiveHandleCount += active ? 1 : -1;
	addDebugText("Accounting active: %d", accountingActiveHandleCount);
	if (accountingActiveHandleCount <= 0) {
		SaveH2Accounts();
	}
}

static void AccountListSetupButtons() {
	for (int i = 0; i < H2AccountCount; i++) {
		add_cartographer_label(CMLabelMenuId_AccountList, 1 + i, H2AccountArrayUsername[i] ? H2AccountArrayUsername[i] : H2CustomLanguageGetLabel(CMLabelMenuId_AccountList, 0xFFFF0005), true);
	}

	add_cartographer_label(CMLabelMenuId_AccountList, 1 + H2AccountCount, H2CustomLanguageGetLabel(CMLabelMenuId_AccountList, 0xFFFF0004), true);
	add_cartographer_label(CMLabelMenuId_AccountList, 1 + H2AccountCount + 1, H2CustomLanguageGetLabel(CMLabelMenuId_AccountList, 0xFFFF0003), true);
	add_cartographer_label(CMLabelMenuId_AccountList, 1 + H2AccountCount + 2, H2CustomLanguageGetLabel(CMLabelMenuId_AccountList, 0xFFFF0000), true);
	add_cartographer_label(CMLabelMenuId_AccountList, 1 + H2AccountCount + 3, H2CustomLanguageGetLabel(CMLabelMenuId_AccountList, 0xFFFF0001 + (accountRemoveMode ? 1 : 0)), true);
}

void* __cdecl c_account_list_menu::open(s_new_ui_screen_parameters* parameters)
{
	c_account_list_menu* account_list_menu = nullptr;
	BYTE* ui_buffer = (BYTE*)ui_memory_pool_allocate(sizeof(c_account_list_menu), 0);

	AccountListSetupButtons();

	if (ui_buffer) {
		c_account_list_menu::UpdateAccountingActiveHandle(true);
		account_list_menu = new (ui_buffer) c_account_list_menu(parameters->ui_channel, parameters->field_8, HIWORD(parameters->flags), accountRemoveMode); // manually call the constructor
	}

	account_list_menu->field_6C = true;
	user_interface_register_screen_to_channel(account_list_menu, parameters);
	return account_list_menu;
}

c_account_list_menu::c_account_list_menu(int _ui_channel, int a4, __int16 _flags, bool _account_removal_mode) :
	c_screen_with_menu(BRIGHTNESS_MENU_ID, _ui_channel, a4, _flags, &this->account_edit_list, true),
	account_edit_list(_flags, H2AccountCount, H2AccountLastUsed, _account_removal_mode)
{
}

c_account_edit_list::c_account_edit_list(int _flags, int _account_count, int _default_selected_button, bool _account_removal_mode) :
	c_list_widget(_flags, true)
{
	auto sub_2113C6 = Memory::GetAddress<int(__thiscall*)(void*)>(0x2113C6);
	auto sub_2113D3 = Memory::GetAddress<void*(__thiscall*)(void*, void*)>(0x2113D3);
	auto c_list_item_widget_ctor = Memory::GetAddress<void*(__thiscall*)(void*)>(0x21FFC9);
	auto c_list_item_widget_dctor = Memory::GetAddress<void(__thiscall*)(void*)>(0xA551);
	auto p_vector_constructor = Memory::GetAddress<void(__stdcall*)(void*, unsigned int, int, void*(__thiscall*)(void*), void(__thiscall*)(void*))>(0x28870B);

	// this constructs the buttons list widgets drawn inside the window
	// brightness menu can display 4 widget list buttons at once
	p_vector_constructor(this->item_list, 132, 4, c_list_item_widget_ctor, c_list_item_widget_dctor);

	this->field_2C0 = 0;
	sub_2113C6(&this->slot_2_unk.field_8);

	//this->slot_2_unk.c_slot_vtbl = &off_1229700;
	// this->slot_2_unk.c_slot_vtbl = NULL;
	// not sure why this exists, it just calls the function to the button handler
	// TODO implement virtual interface for slot_1, slot_2
	this->slot_2_unk.c_slot_vtbl = Memory::GetAddressRelative<void*>(0x7D9700);
	this->slot_2_unk.ui_screen_edit_list = this;
	this->slot_2_unk.button_handler_cb = &c_account_edit_list::button_handler;
	this->default_selected_button = _default_selected_button;
	this->account_removal_mode = _account_removal_mode;

	int button_count = 4 + _account_count + (_account_count <= 0 ? -1 : 0);
	s_data_array* account_list_data = allocate_list_data("account list edit list", button_count, 4);
	this->list_data_array = account_list_data;

	s_data_array::data_make_valid(account_list_data);

	for (unsigned int i = 0; i < this->list_data_array->datum_max_elements; i++) {
		s_data_array::datum_new_in_range(this->list_data_array);
	}

	// no fucking clue what's this, maybe related to this->slot_2_unk.c_slot_vtbl data offset
	// because this->slot_2_unk.c_slot_vtbl is at offset 708
	if ((void*)this == (void*)-708) {
		sub_2113D3(&this->gap_70[56], nullptr);
	}
	else {
		sub_2113D3(&this->gap_70[56], &this->slot_2_unk.field_8);
	}
}

extern void GSCustomMenuCall_AccountEdit();
extern void GSCustomMenuCall_AccountCreate();
extern void xbox_live_task_progress_callback(DWORD a1);
extern DWORD WINAPI ThreadLogin(LPVOID lParam);

extern HANDLE hThreadLogin;

void c_account_edit_list::button_handler(int* a2, int* a3)
{
	int button_id = *(int*)a3 & 0xFFFF;

	int parent_screen_idx = this->get_top_most_parent_widget_index();
	int parent_screen_ui_channel = this->get_top_most_parent_widget_ui_channel();

	bool close_parent_screen = false;

	s_new_ui_screen_parameters new_ui_account_list_screen;
	new_ui_account_list_screen.data_new(0, 1 << *(int*)(*a2 + 4), parent_screen_ui_channel, 4, c_account_list_menu::open);

	if (button_id == H2AccountCount + 1) {
		if (!this->account_removal_mode) {
			GSCustomMenuCall_AccountCreate();
		}
	}
	else if (button_id == H2AccountCount + 2) {
		if (!this->account_removal_mode) {
			GSCustomMenuCall_AccountEdit();
		}
	}
	else if (button_id == H2AccountCount + 3) { // account removal mode switch
		//remove next selected account.
		accountRemoveMode = !this->account_removal_mode;
		add_cartographer_label(CMLabelMenuId_AccountList, 1 + H2AccountCount + 3, H2CustomLanguageGetLabel(CMLabelMenuId_AccountList, 0xFFFF0001 + (accountRemoveMode ? 1 : 0)), true);
		new_ui_account_list_screen.ui_screen_load_proc_exec();
		close_parent_screen = true;
	}
	else if (H2AccountCount > 0 && button_id >= 0 && button_id < H2AccountCount) { // account selection (either for deletion or login)
		if (this->account_removal_mode) {
			H2AccountAccountRemove(button_id);
			// update button list
			AccountListSetupButtons();
			// then re-open the menu
			accountRemoveMode = false; // reset global accountRemove state
			new_ui_account_list_screen.ui_screen_load_proc_exec();
			H2AccountLastUsed = 0;
			close_parent_screen = true;
		}
		else if (hThreadLogin == INVALID_HANDLE_VALUE) {
			c_account_list_menu::accountingGoBackToList = false;
			c_account_list_menu::UpdateAccountingActiveHandle(true);
			hThreadLogin = CreateThread(NULL, 0, ThreadLogin, (LPVOID)button_id, 0, NULL);
			c_xbox_live_task_progress_menu::Open(xbox_live_task_progress_callback);
			close_parent_screen = true;
		}
	}
	else if (button_id == H2AccountCount) { // play offline button
		BYTE abEnet[6];
		BYTE abOnline[20];
		XNetRandom(abEnet, sizeof(abEnet));
		XNetRandom(abOnline, sizeof(abOnline));
		accountRemoveMode = false;
		if (ConfigureUserDetails("[Username]", "12345678901234567890123456789012", rand(), 0, H2Config_ip_lan, ByteToHexStr(abEnet, 6).c_str(), ByteToHexStr(abOnline, 20).c_str(), false)) {
			//show select profile gui
			extern int notify_xlive_ui;
			notify_xlive_ui = 0;
			H2Config_master_ip = inet_addr("127.0.0.1");
			H2Config_master_port_relay = 2001;
			close_parent_screen = true;
			XUserSignInSetStatusChanged(0);
		}
	}

	if (close_parent_screen)
		user_interface_back_out_from_channel(parent_screen_ui_channel, parent_screen_idx);
	return;
}



