#include "stdafx.h"
#include "CustomMenuGlobals.h"

#include "game/game_globals.h"
#include "H2MOD/Tags/TagInterface.h"

std::chrono::time_point<std::chrono::high_resolution_clock> lastOuterMenuUse;
void* lastOuterMenuFuncPtr = 0;

void* ui_custom_cartographer_load_menu(proc_ui_screen_load_cb_t p_ui_screen_proc_cb) {
	return ui_custom_cartographer_load_menu(p_ui_screen_proc_cb, 1, 0);
}

void* ui_custom_cartographer_load_menu(proc_ui_screen_load_cb_t p_ui_screen_proc_cb, int open_method2) {
	return ui_custom_cartographer_load_menu(p_ui_screen_proc_cb, open_method2, 0);
}

int prevOpenMethod = 3;
void* ui_custom_cartographer_load_menu(proc_ui_screen_load_cb_t p_ui_screen_proc_cb, int open_method2, int menu_wgit_type) {
	//int(__thiscall*WgitInitialize)(void*) = (int(__thiscall*)(void*))((char*)Memory::GetAddress() + 0x20B0BC);
	//0x0020C258 is another one.
	//void*(__thiscall*WgitFinalize)(void*) = (void*(__thiscall*)(void*))((char*)Memory::GetAddress() + 0x20B11E);

	int open_method = open_method2;
	if (open_method == 1) {
		open_method = prevOpenMethod;
	}
	else if (open_method == 2) {
		int CurrentWgitID = *(int*)((BYTE*)Memory::GetAddress() + 0x9758D8);
		if (menu_wgit_type == 0) {
			open_method = 3;
		}
		else if (lastOuterMenuFuncPtr > 0 && lastOuterMenuFuncPtr == p_ui_screen_proc_cb) {
			if (CurrentWgitID != menu_wgit_type) {
				std::chrono::milliseconds difference = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - lastOuterMenuUse);
				long long timeDiff = difference.count();
				if (timeDiff < 1500) {
					open_method = 3;
				}
			}
		}
	}
	lastOuterMenuFuncPtr = p_ui_screen_proc_cb;
	lastOuterMenuUse = std::chrono::high_resolution_clock::now();
	prevOpenMethod = open_method;

	s_screen_parameters new_screen_params;
	switch (open_method) {
	case 3:
		new_screen_params.data_new(0, FLAG(0), _user_interface_channel_type_dialog, _window_4, p_ui_screen_proc_cb);
		break;
	case 0:
	default:
		new_screen_params.data_new(0, FLAG(0), _user_interface_channel_type_gameshell_screen, _window_4, p_ui_screen_proc_cb);
		break;
	}

	return new_screen_params.ui_screen_load_proc_exec();
}