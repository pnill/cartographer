#pragma once
#include "c_list_widget.h"
#include "c_screen_with_menu.h"

struct __declspec(align(4)) s_menu_input_unk_data
{
	DWORD flags;
	DWORD field_4;
	int field_8;
	DWORD field_C;
	DWORD field_10;
	DWORD field_14;
	DWORD field_18;
	DWORD field_1C;
};

extern DWORD H2BaseAddr;
const int CreditsMenu_ID = 0xFF000006;

BYTE* ui_memory_pool_allocate(int size, int a2);
int __cdecl unk_used_after_constructor(void* ui_memory, s_menu_input_unk_data* a2);

void __stdcall sub_21bf85_CMLTD_(int thisptr, int label_id, int label_menu_id);
int __stdcall sub_2111ab_CMLTD_(int thisptr, int a2, int label_menu_id, int label_id_title, int label_id_description);
int __stdcall sub_20F790_CM_(int thisptr, __int16 selected_button_id);
int __cdecl CustomMenu_CallHead_(int a1, DWORD* menu_vftable_1, DWORD* menu_vftable_2, DWORD menu_button_handler, int number_of_buttons, int menu_wgit_type);
void CallWgit_(int WgitScreenfunctionPtr);
void CallWgit_(int WgitScreenfunctionPtr, int open_method);
void CallWgit_(int WgitScreenfunctionPtr, int open_method, int menu_wgit_type);