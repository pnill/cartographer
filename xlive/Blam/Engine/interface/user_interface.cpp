#include "stdafx.h"
#include "user_interface.h"

float* get_ui_scale_factor()
{
	return Memory::GetAddress<float*>(0xA3E424);
}

void render_menu_user_interface_to_usercall(int32 window_index, int32 controller_index, int32 player_count, rectangle2d* rect2d)
{
	static void* render_menu_user_interface = (void*)Memory::GetAddress(0x20B697);
	__asm {
		push rect2d
		push player_count
		push controller_index
		mov esi, window_index
		call render_menu_user_interface
		add esp, 12
	}
	return;
}