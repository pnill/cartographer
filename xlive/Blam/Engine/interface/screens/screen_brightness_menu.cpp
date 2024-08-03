#include "stdafx.h"

#include "screen_brightness_menu.h"

/*
	Re-implementation of default brightness menu
	Now with the abillity to override base class functions
*/

c_brightness_menu::c_brightness_menu(e_user_interface_channel_type _ui_channel, e_user_interface_render_window _window_index, uint16 _flags) :
	c_screen_with_menu(_screen_brightness_level, _ui_channel, _window_index, _flags, &this->m_list_widgets),
	m_list_widgets(_flags)
{
}

void* __cdecl c_brightness_menu::open_brightness_menu(s_screen_parameters* parameters)
{
	c_brightness_menu* brightness_menu = nullptr;
	c_brightness_menu* ui_buffer = (c_brightness_menu*)ui_pool_allocate_space(sizeof(c_brightness_menu), 0);

	if (ui_buffer) {
		brightness_menu = new (ui_buffer) c_brightness_menu(parameters->m_channel_type, parameters->m_window_index, parameters->user_flags); // manually call the constructor
		brightness_menu->m_allocated = true;
	}

	user_interface_register_screen_to_channel(brightness_menu, parameters);
	return brightness_menu;
}

void replace_brightness_menu()
{
	WritePointer(Memory::GetAddressRelative(0x6492DF) + 1, (void*)c_brightness_menu::open_brightness_menu);
	WritePointer(Memory::GetAddressRelative(0x64966C) + 1, (void*)c_brightness_menu::open_brightness_menu);
}