#pragma once

void __cdecl error_message_menu_open(int32 a1, int32 ui_error_index, int32 a3, int16 a4, void* a5, void* a6);

bool __cdecl user_interface_controller_pick_profile_dialog_wrapper(int32 controller_index);

int32 __cdecl user_interface_local_player_count(void);

void render_menu_user_interface_to_usercall(int32 window_index, int32 controller_index, int32 player_count, rectangle2d* rect2d);
