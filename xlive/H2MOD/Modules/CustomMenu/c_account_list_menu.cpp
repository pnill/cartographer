#include "stdafx.h"
#include "c_account_list_menu.h"

#include "Blam/Engine/DataArray/DataArray.h"

#include "H2MOD/Modules/Accounts/Accounts.h"
#include "H2MOD/Modules/Accounts/AccountLogin.h"
#include "H2MOD\Modules\UI\XboxLiveTaskProgress.h"
#include "H2MOD\Modules\OnScreenDebug\OnscreenDebug.h"

#include "CustomLanguage.h"

#include "H2MOD/Modules/Config/Config.h"

#include "H2MOD/Modules/Utils/Utils.h"

bool c_account_list_menu::mode_remove_account = false;
bool c_account_list_menu::accountingGoBackToList = false;
int c_account_list_menu::accountingActiveHandleCount = 0;

bool c_account_list_menu::isAccountingActiveHandle() {
	return accountingActiveHandleCount > 0;
}

void c_account_list_menu::updateAccountingActiveHandle(bool active) {
	accountingActiveHandleCount += active ? 1 : -1;
	addDebugText("Accounting active: %d", accountingActiveHandleCount);
	if (accountingActiveHandleCount <= 0) {
		SaveH2Accounts();
	}
}

void* __cdecl c_account_list_menu::open(s_menu_input_unk_data* a1)
{
	c_account_list_menu* account_list_menu = nullptr;
	BYTE* ui_buffer = (BYTE*)ui_memory_pool_allocate(sizeof(c_account_list_menu), 0);

	if (ui_buffer) {
		c_account_list_menu::updateAccountingActiveHandle(true);
		account_list_menu = new (ui_buffer) c_account_list_menu(a1->field_4, a1->field_8, HIWORD(a1->flags)); // manually call the constructor
	}

	account_list_menu->field_6C = true;
	unk_used_after_constructor(account_list_menu, a1);
	return account_list_menu;
}

c_account_list_menu::c_account_list_menu(int a3, int a4, int a5) :
	c_screen_with_menu(BRIGHTNESS_MENU_ID, a3, a4, a5, &this->account_edit_list),
	account_edit_list(a5, H2AccountCount)
{
}

c_account_edit_list::c_account_edit_list(int a2, int account_count) :
	c_list_widget(a2)
{
	auto p_vector_constructor = (void(__stdcall*)(void*, int, int, void(__thiscall*)(DWORD), int))((char*)H2BaseAddr + 0x28870B);
	auto sub_2113C6 = (int(__thiscall*)(void*))((char*)H2BaseAddr + 0x2113C6);
	auto sub_20D1FD = (int(__cdecl*)(char*, int, int))((char*)H2BaseAddr + 0x20D1FD);
	auto sub_66B33  = (int(__cdecl*)(int))((char*)H2BaseAddr + 0x66B33);
	auto sub_667A0  = (int(__cdecl*)(int))((char*)H2BaseAddr + 0x667A0);
	auto sub_2113D3 = (void* (__thiscall*)(void*, void*))((char*)H2BaseAddr + 0x2113D3);
	auto c_list_item_widget_ctor = (void* (__thiscall*)(void*))((char*)H2BaseAddr + 0x21FFC9);
	auto c_list_item_widget_dctor = (int(__thiscall*)(void*))((char*)H2BaseAddr + 0xA551);

	// this constructs the buttons list widgets drawn inside the window
	// brightness menu can display 4 widget list buttons at once
	p_vector_constructor(this->account_list_items, 132, 4, (void(__thiscall*)(DWORD))c_list_item_widget_ctor, (int)c_list_item_widget_dctor);

	this->field_2C0 = 0;
	sub_2113C6(&this->slot_2_unk.field_8);

	//this->slot_2_unk.c_slot_vtbl = &off_1229700;
	// this->slot_2_unk.c_slot_vtbl = NULL;
	// not sure why this exists, it just calls the function to the button handler
	this->slot_2_unk.c_slot_vtbl = Memory::GetAddressRelative<void*>(0x7D9700);
	this->context = this;
	this->button_handler_cb = &c_account_edit_list::button_handler;

	int button_count = 4 + account_count + (account_count <= 0 ? -1 : 0);
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

void c_account_edit_list::button_handler(long a2, struct s_event_record* a3)
{
	int button_id = *(int*)a3 & 0xFFFF;

	if (button_id == H2AccountCount + 1) {
		if (!c_account_list_menu::mode_remove_account) {
			GSCustomMenuCall_AccountCreate();
		}
	}
	else if (button_id == H2AccountCount + 2) {
		if (!c_account_list_menu::mode_remove_account) {
			GSCustomMenuCall_AccountEdit();
		}
	}
	else if (button_id == H2AccountCount + 3) {
		//remove next selected account.
		c_account_list_menu::mode_remove_account = !c_account_list_menu::mode_remove_account;
		add_cartographer_label(0xFF000009, 1 + H2AccountCount + 3, H2CustomLanguageGetLabel(0xFF000009, 0xFFFF0001 + (c_account_list_menu::mode_remove_account ? 1 : 0)), true);
	}
	else if (H2AccountCount > 0 && button_id >= 0 && button_id < H2AccountCount) {
		if (c_account_list_menu::mode_remove_account) {
			H2AccountAccountRemove(button_id);
			// account list

			H2AccountLastUsed = 0;
		}
		else if (hThreadLogin == INVALID_HANDLE_VALUE) {
			c_account_list_menu::accountingGoBackToList = false;
			c_account_list_menu::updateAccountingActiveHandle(true);
			hThreadLogin = CreateThread(NULL, 0, ThreadLogin, (LPVOID)button_id, 0, NULL);
			c_xbox_live_task_progress_menu::Open(xbox_live_task_progress_callback);
			return;
		}
	}
	else if (button_id == H2AccountCount) {
		if (!c_account_list_menu::mode_remove_account) {
			//play offline
			BYTE abEnet[6];
			BYTE abOnline[20];
			XNetRandom(abEnet, 6);
			XNetRandom(abOnline, 20);
			if (ConfigureUserDetails("[Username]", "12345678901234567890123456789012", rand(), 0, H2Config_ip_lan, ByteToHexStr(abEnet, 6).c_str(), ByteToHexStr(abOnline, 20).c_str(), false)) {
				//show select profile gui
				extern int notify_xlive_ui;
				notify_xlive_ui = 0;
				H2Config_master_ip = inet_addr("127.0.0.1");
				H2Config_master_port_relay = 2001;
			}
		}
	}

	return;
}



