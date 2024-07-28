#include "stdafx.h"
#include "user_interface.h"

void __cdecl screen_error_ok_dialog_show(int32 a1, e_ui_error_types ui_error_index, int32 a3, int16 a4, void* a5, void* a6)
{
	INVOKE(0x20E1D8, 0x0, screen_error_ok_dialog_show, a1, ui_error_index, a3, a4, a5, a6);
	return;
}

bool __cdecl user_interface_channel_is_busy(e_user_interface_channel_type channel_type)
{
	return INVOKE(0x20A540, 0x0, user_interface_channel_is_busy, channel_type);
}

void __cdecl user_interface_error_display_ok_cancle_dialog_with_ok_callback(e_user_interface_channel_type channel_type, e_user_interface_render_window window_index, uint16 user_flags, void* ok_callback_handle, e_ui_error_types error_type)
{
	INVOKE(0x20E3BB, 0x0, user_interface_error_display_ok_cancle_dialog_with_ok_callback, channel_type, window_index, user_flags, ok_callback_handle, error_type);
}

bool __cdecl user_interface_back_out_from_channel_by_id(e_user_interface_channel_type channel_type, e_user_interface_render_window window_index, e_user_interface_screen_id id)
{
	return INVOKE(0x209850, 0x0, user_interface_back_out_from_channel_by_id, channel_type, window_index, id);
}

void __cdecl user_interface_enter_game_shell(int32 context)
{
	INVOKE(0x20CE70, 0x0, user_interface_enter_game_shell, context);
}

bool __cdecl user_interface_in_screen(e_user_interface_channel_type channel_type, e_user_interface_render_window window_index, e_user_interface_screen_id screen_id)
{
	return INVOKE(0x20BAAA, 0x0, user_interface_in_screen, channel_type, window_index, screen_id);
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