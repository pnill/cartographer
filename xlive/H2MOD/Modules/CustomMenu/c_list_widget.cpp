
#include "c_list_widget.h"

void __fastcall c_list_widget_label_buttons(int thisptr, DWORD _EDX, int a1, int a2)
{

	c_list_widget *c_list_ptr = (c_list_widget*)((BYTE*)_EDX - 0x30);

	int(__thiscall* sub_211909)(int, int, int, int) = (int(__thiscall*)(int, int, int, int))((char*)H2BaseAddr + 0x211909);
	void(__thiscall* sub_21bf85)(int, int label_id) = (void(__thiscall*)(int, int))((char*)H2BaseAddr + 0x21bf85);

	__int16 button_id = *(WORD*)(a1 + 112);
	int v3 = sub_211909((int)a1, 6, 0, 0);
	if (v3)
	{
		sub_21bf85_CMLTD_(v3, button_id + 1, c_list_ptr->menu_id);
	}
}
