#pragma once

#include "H2MOD/Modules/Accounts/Accounts.h"
#include "c_brightness_menu.h"
#include "CustomMenuGlobals.h"

class c_account_edit_list : public c_list_widget
{
public:
	char account_list_items[132 * 4];
	int field_2C0;
	c_slot2 slot_2_unk;
	// button handler callback
	void* context;
	void(c_account_edit_list::*button_handler_cb)(long a2, struct s_event_record* a3);

	c_account_edit_list::c_account_edit_list(int a2, int account_count);

	virtual char* IUnkFunc23() override
	{
		return account_list_items; // returns pointer to edit list
	}

	virtual int IUnkFun24() override
	{
		return 4; // returns edit list count
	}

	void get_label(int a1, int a2) override
	{
		auto p_sub_211909 = Memory::GetAddress<int(__thiscall*)(int, int, int, int)>(0x211909);
		auto p_sub_21bf85 = Memory::GetAddress<void(__thiscall*)(int, int)>(0x21bf85);

		__int16 button_id = *(WORD*)(a1 + 112);
		int v3 = p_sub_211909(a1, 6, 0, 0);
		if (v3)
		{
			set_widget_label_from_string_id_reimpl(v3, button_id + 1, 0xFF000009);
		}
	}

	// button handler
	void button_handler(long a2, struct s_event_record* a3);
};
static_assert(offsetof(c_account_edit_list, slot_2_unk.field_8) == 712);
static_assert(offsetof(c_account_edit_list, list_data_array) == 112);
static_assert(offsetof(c_account_edit_list, gap_70[56]) == 172);

class c_account_list_menu : protected c_screen_with_menu
{
public:
	
	static bool mode_remove_account;
	static bool accountingGoBackToList;
	static int accountingActiveHandleCount;
	static bool isAccountingActiveHandle();
	static void updateAccountingActiveHandle(bool active);

	static void* __cdecl open(s_menu_input_unk_data* a1);

	c_account_list_menu::c_account_list_menu(int a3, int a4, int a5);

	// interface
	virtual int custom_deconstructor(BYTE flags) override
	{
		updateAccountingActiveHandle(false);
		return c_screen_with_menu::custom_deconstructor(flags);
	};

	// c_screen_with_menu specific interface
	virtual int IUnkFunc23(int a2) override
	{
		// int __stdcall sub_2111ab_CMLTD(int thisptr, int a2, int label_menu_id, int label_id_title, int label_id_description)
		return sub_2111ab_CMLTD((int)this, a2, 0xFF000009, 0xFFFFFFF0, 0xFFFFFFF1);
		// return c_screen_with_menu::IUnkFunc23(a2);
	}

	virtual int IUnkFunc24() override
	{
		account_edit_list.set_selected_list_button_idx(H2AccountLastUsed);
		return c_screen_with_menu::IUnkFunc24();
	}

	virtual void* get_open_menu_cb() override 
	{
		return c_account_list_menu::open;
	}

	c_account_edit_list account_edit_list;
private:
};
static_assert(sizeof(c_account_list_menu) == 3388);