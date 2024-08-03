#pragma once

#include "interface/user_interface_headers.h"

extern DWORD H2BaseAddr;
const int CreditsMenu_ID = 0xFF000006;

void __stdcall set_widget_label_from_string_id_reimpl(int thisptr, int label_id, int label_menu_id);

void __stdcall sub_2101a4_CMLTD(int thisptr, int label_id, wchar_t* rtn_label, int label_menu_id);
char __stdcall sub_21bb0b_CMLTD(void* thisptr, __int16 a2, int* aa3, int label_menu_id, int label_id_description);
void __cdecl sub_3e3ac_CMLTD(int a1, int label_id, wchar_t* rtn_label, int label_menu_id);

// vkeyboard related
int __stdcall sub_23ae3c_CMLTD(void* thisptr, int label_menu_id, int label_id_title, int label_id_description);
char __stdcall sub_210a44_CMLTD(int thisptr, int a2, int* a3, int label_menu_id, int label_id_title, int label_id_description);

void* ui_custom_cartographer_load_menu(proc_ui_screen_load_cb_t WgitScreenfunctionPtr);
void* ui_custom_cartographer_load_menu(proc_ui_screen_load_cb_t WgitScreenfunctionPtr, int open_method);
void* ui_custom_cartographer_load_menu(proc_ui_screen_load_cb_t WgitScreenfunctionPtr, int open_method, int menu_wgit_type);
