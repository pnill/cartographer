#include "stdafx.h"

#include "Util/Hooks/Hook.h"

#include "c_brightness_menu.h"

/*
	Re-implementation of default brightness menu
	Now with the abillity to override base class functions
*/

c_brightness_menu::c_brightness_menu(int a3, int a4, int a5) :
	c_screen_with_menu(BRIGHTNESS_MENU_ID, a3, a4, a5, &this->list_widgets),
	list_widgets(a5)
{
}

void* __cdecl c_brightness_menu::open_brightness_menu(s_new_ui_window_parameters* a1)
{
	c_brightness_menu* brightness_menu = nullptr;
	c_brightness_menu* ui = (c_brightness_menu*)ui_memory_pool_allocate(sizeof(c_brightness_menu), 0);

	if (ui) {
		brightness_menu = new (ui) c_brightness_menu(a1->ui_channel, a1->field_8, HIWORD(a1->flags)); // manually call the constructor
	}

	brightness_menu->field_6C = true;
	ui_new_window(brightness_menu, a1);
	return brightness_menu;
}

void replace_brightness_menu()
{
	// WritePointer((DWORD)Memory::GetAddressRelative(0x6492DF) + 1, (void*)c_brightness_menu::open_brightness_menu);
	// WritePointer((DWORD)Memory::GetAddressRelative(0x64966C) + 1, (void*)c_brightness_menu::open_brightness_menu);
}