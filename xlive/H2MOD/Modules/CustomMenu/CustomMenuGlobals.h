#pragma once
#include "c_list_widget.h"
#include "c_screen_with_menu.h"

typedef void(__thiscall* button_handler_cb_t)(void* context, int a1, int* a2);

enum e_ui_channel
{
	hardware_error = 0,
	game_error = 1,
	virtual_keyboard = 2,
	gameshell_dialog = 3,
	gameshell_screen = 5,
	gameshell_background = 6,
};

struct __declspec(align(4)) s_new_ui_window_parameters
{
	void data_new(__int16 flags, int ui_channel, int a4, void* (__cdecl* ui_window_load_cb)(s_new_ui_window_parameters*))
	{
		this->flags = (DWORD)(flags << 16);
		this->ui_channel = ui_channel;
		this->field_8 = a4;
		this->field_10 = -1;
		this->field_14 = -1;
		this->field_18 = -1;
		this->proc_ui_window_load_cb = ui_window_load_cb;
	}

	void* ui_window_load_proc_exec()
	{
		return proc_ui_window_load_cb(this);
	}

	DWORD flags;
	DWORD ui_channel;
	int field_8;
	DWORD context; // this might be wrong, but it looks like it holds window context data
	DWORD field_10;
	DWORD field_14;
	DWORD field_18;
	void*(__cdecl* proc_ui_window_load_cb)(s_new_ui_window_parameters*);
};
static_assert(sizeof(s_new_ui_window_parameters) == 0x20);

template<typename T>
struct c_slot2
{
	void* c_slot_vtbl;
	int field_8[3]; // c_slot data?
	void* context;
	void(T::* button_handler_cb)(int* a2, int* a3);
};

extern DWORD H2BaseAddr;
const int CreditsMenu_ID = 0xFF000006;

BYTE* ui_memory_pool_allocate(int size, int a2);
int __cdecl ui_new_window(void* ui_memory, s_new_ui_window_parameters* a2);
int __cdecl ui_window_back_out(int ui_channel, int window_idx);

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