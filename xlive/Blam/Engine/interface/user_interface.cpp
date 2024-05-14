#include "stdafx.h"
#include "user_interface.h"

float* get_ui_scale_factor()
{
	return Memory::GetAddress<float*>(0xA3E424);
}

void __cdecl error_message_menu_open(int32 a1, int32 ui_error_index, int32 a3, int16 a4, void* a5, void* a6)
{
	INVOKE(0x20E1D8, 0x0, error_message_menu_open, a1, ui_error_index, a3, a4, a5, a6);
	return;
}

bool __cdecl user_interface_controller_pick_profile_dialog_wrapper(int32 controller_index)
{
	return INVOKE(0x212F56, 0x0, user_interface_controller_pick_profile_dialog_wrapper, controller_index);
}

int32 __cdecl user_interface_local_player_count(void)
{
	return INVOKE(0x215BBD, 0x0, user_interface_local_player_count);
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