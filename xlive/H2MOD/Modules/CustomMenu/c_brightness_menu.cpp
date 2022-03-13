#include "stdafx.h"

#include "Util/Hooks/Hook.h"

#include "c_brightness_menu.h"

/*
	Re-implementation of default brightness menu
	Now with the abillity to override base class functions
*/

c_brightness_menu::c_brightness_menu(int a3, int a4, int a5)
	: c_screen_with_menu(BRIGHTNESS_MENU_ID, a3, a4, a5, (BYTE*)this->data + 2652)
{
	// initialize button list

	auto p_initialize_button_list = Memory::GetAddressRelative<void(__thiscall*)(void*, int)>(0x650E22); // c_brightness_level_edit_list
	p_initialize_button_list((BYTE*)this->data + 2652, a5);
}

void* __cdecl c_brightness_menu::open_brightness_menu(s_menu_input_unk_data* a1)
{
	c_brightness_menu* brightness_menu = nullptr;
	c_brightness_menu* ui = (c_brightness_menu*)ui_memory_pool_allocate(3388, 0);

	if (ui) {
		brightness_menu = new (ui) c_brightness_menu(a1->field_4, a1->field_8, HIWORD(a1->flags)); // manually call the constructor
	}

	*(BYTE*)((int)brightness_menu + 108) = 1;
	unk_used_after_constructor(brightness_menu, a1);
	return brightness_menu;
}

void replace_brightness_menu()
{
	WritePointer((DWORD)Memory::GetAddressRelative(0x6492DF) + 1, (void*)c_brightness_menu::open_brightness_menu);
	WritePointer((DWORD)Memory::GetAddressRelative(0x64966C) + 1, (void*)c_brightness_menu::open_brightness_menu);
}