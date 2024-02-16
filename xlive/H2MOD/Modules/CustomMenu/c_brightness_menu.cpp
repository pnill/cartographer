#include "stdafx.h"



#include "c_brightness_menu.h"

/*
	Re-implementation of default brightness menu
	Now with the abillity to override base class functions
*/

c_brightness_menu::c_brightness_menu(int _ui_channel, int a4, __int16 _flags) :
	c_screen_with_menu(BRIGHTNESS_MENU_ID, _ui_channel, a4, _flags, &this->list_widgets, true),
	list_widgets(_flags)
{
}

void* __cdecl c_brightness_menu::open_brightness_menu(s_new_ui_screen_parameters* parameters)
{
	c_brightness_menu* brightness_menu = nullptr;
	c_brightness_menu* ui_buffer = (c_brightness_menu*)ui_memory_pool_allocate(sizeof(c_brightness_menu), 0);

	if (ui_buffer) {
		brightness_menu = new (ui_buffer) c_brightness_menu(parameters->ui_channel, parameters->field_8, HIWORD(parameters->flags)); // manually call the constructor
	}

	brightness_menu->field_6C = true;
	user_interface_register_screen_to_channel(brightness_menu, parameters);
	return brightness_menu;
}

void replace_brightness_menu()
{
	WritePointer((DWORD)Memory::GetAddressRelative(0x6492DF) + 1, (void*)c_brightness_menu::open_brightness_menu);
	WritePointer((DWORD)Memory::GetAddressRelative(0x64966C) + 1, (void*)c_brightness_menu::open_brightness_menu);
}