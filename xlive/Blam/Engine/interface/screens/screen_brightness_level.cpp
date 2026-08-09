#include "stdafx.h"
#include "screen_brightness_level.h"

void* __cdecl c_screen_brightness_level_menu::load(c_screen_parameters* parameters)
{
	return INVOKE(0x24925C, 0x0, c_screen_brightness_level_menu::load, parameters);
}

void* __cdecl c_screen_brightness_level_menu::load_mp(c_screen_parameters* parameters)
{
	return INVOKE(0x258C8C, 0x0, c_screen_brightness_level_menu::load_mp, parameters);
}
