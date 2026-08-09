#pragma once
#include "interface/user_interface.h"

class c_screen_resolution_menu
{
public:
	static void* __cdecl load(c_screen_parameters* parameters);
	static void* __cdecl load_mp(c_screen_parameters* parameters);

	static void __cdecl apply_instance_patches();
};