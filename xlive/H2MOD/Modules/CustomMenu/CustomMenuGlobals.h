#pragma once
#include "c_list_widget.h"
#include "c_screen_with_menu.h"

typedef void(__thiscall* button_handler_cb_t)(void* context, int a1, int* a2);

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

struct c_slot2
{
	void* c_slot_vtbl;
	int field_8[3]; // c_slot data?
};

extern DWORD H2BaseAddr;
const int CreditsMenu_ID = 0xFF000006;

BYTE* ui_memory_pool_allocate(int size, int a2);
int __cdecl unk_used_after_constructor(void* ui_memory, s_menu_input_unk_data* a2);

#define VIRTUAL_KEYBOARD_MENU_TYPE_DEFAULT_MAX 17

// we just re-use the first seventeen keyboard id's in default code for now
#define VIRTUAL_KEYBOARD_MENU_TYPE_MAX_NEW (VIRTUAL_KEYBOARD_MENU_TYPE_DEFAULT_MAX * 2)

void __stdcall set_widget_label_from_string_id_reimpl(int thisptr, int label_id, int label_menu_id);

int __stdcall sub_2111ab_CMLTD(int thisptr, int a2, int label_menu_id, int label_id_title, int label_id_description);
void __stdcall sub_2101a4_CMLTD(int thisptr, int label_id, wchar_t* rtn_label, int label_menu_id);
char __stdcall sub_21bb0b_CMLTD(void* thisptr, __int16 a2, int* aa3, int label_menu_id, int label_id_description);
void __cdecl sub_3e3ac_CMLTD(int a1, int label_id, wchar_t* rtn_label, int label_menu_id);
int __cdecl CustomMenu_CallHead_(int a1, DWORD* menu_vftable_1, DWORD* menu_vftable_2, DWORD menu_button_handler, int number_of_buttons, int menu_wgit_type);
void CallWgit_(int WgitScreenfunctionPtr);
void CallWgit_(int WgitScreenfunctionPtr, int open_method);
void CallWgit_(int WgitScreenfunctionPtr, int open_method, int menu_wgit_type);