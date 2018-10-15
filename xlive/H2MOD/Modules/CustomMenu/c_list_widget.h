#pragma once
#include <Windows.h>
#include <stdio.h>
#include "CustomMenuGlobals.h"

class c_list_widget;
void __fastcall c_list_widget_label_buttons(int thisptr, DWORD _EDX, int a1, int a2);

extern DWORD H2BaseAddr;
class c_list_widget
{
public:
	char pad_0x0000[0x8]; //0x0000
	char ClassName[36]; //0xAB5980 
	char pad_0x002C[0x64]; //0x002C
	void *LabelButton_Ptr; //0x0090
	char pad_0x0094[0x4]; //0x0094

	int menu_id = 0; // store menu_id in our copy for later use

	c_list_widget::c_list_widget(int menu_button_id)
	{
		memcpy(&this->pad_0x0000, (BYTE*)(H2BaseAddr + 0x3d96fc), 0x98); //hax copy the original class into this one.
		this->menu_id = menu_button_id; // set the menu_id
		LabelButton_Ptr = &c_list_widget_label_buttons; // overwrite pointer to LabelButtons function with ours.
	};
}; //Size=0x0098
static_assert(sizeof(c_list_widget) == 0x9C, "Warning sizeof(c_edit_list) != 0x98");