#include "stdafx.h"
#include "screen_safe_area_setting.h"

void* __cdecl c_screen_safe_area_menu::load(c_screen_parameters* parameters)
{
	return INVOKE(0x2493F7, 0x0, c_screen_safe_area_menu::load, parameters);
}

void* __cdecl c_screen_safe_area_menu::load_mp(c_screen_parameters* parameters)
{
	return INVOKE(0x258E2A, 0x0, c_screen_safe_area_menu::load_mp, parameters);
}
