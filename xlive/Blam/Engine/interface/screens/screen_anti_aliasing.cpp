#include "stdafx.h"
#include "screen_anti_aliasing.h"

void* __cdecl c_screen_anti_aliasing_menu::load(c_screen_parameters* parameters)
{
	return INVOKE(0x24936E, 0x0, c_screen_anti_aliasing_menu::load, parameters);
}

void* __cdecl c_screen_anti_aliasing_menu::load_mp(c_screen_parameters* parameters)
{
	return INVOKE(0x258DA0, 0x0, c_screen_anti_aliasing_menu::load_mp, parameters);
}
