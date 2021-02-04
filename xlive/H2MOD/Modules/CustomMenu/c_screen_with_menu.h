#pragma once
#include "CustomMenuGlobals.h"
#include <stdio.h>

extern DWORD H2BaseAddr;

class c_screen_with_menu;
int __fastcall c_screen_with_menu_title_desc(int a1, DWORD _EDX, char a2);
int __fastcall c_screen_with_menu_keyhandler(c_screen_with_menu* a1, DWORD _EDX, int a2);
int __fastcall c_screen_with_menu_buttonpress(c_screen_with_menu* a1, DWORD _EDX);

class c_screen_with_menu
{
public:
	void* deconstructor; //0x0000 
	char pad_0x0004[0x2C]; //0x0004
	void* keyhandler; //0x0030 
	char pad_0x0034[0x24]; //0x0034
	void* title_desc; //0x0058 
	void* button_press_selection; //0x005C 
	char pad_0x0060[0x38]; //0x0060
	void* FuncPtrHelper; //0x0098 
	char pad_0x009C[0x8]; //0x009C

	int menu_id = 0;
	int label_id_description = 0;
	int label_id_title = 0;

	c_screen_with_menu::c_screen_with_menu(int menu_id,int label_id_description,int label_id_title)
	{

		memcpy(&this->deconstructor, (BYTE*)(H2BaseAddr + 0x3D7ABC), 0xA4); //hax copy the original class into this one.

		this->menu_id = menu_id;
		this->label_id_description = label_id_description;
		this->label_id_title = label_id_title;

		title_desc = &c_screen_with_menu_title_desc;
		keyhandler = &c_screen_with_menu_keyhandler;
		button_press_selection = &c_screen_with_menu_buttonpress;

	};

	static void applyPatches();
	void __thiscall build_player_list(void* a1, int player_count);
}; static_assert(sizeof(c_screen_with_menu) == 0xB0, "Warning size of c_screen_with_menu != 0xA4");