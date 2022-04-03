#include "stdafx.h"

#include "c_error_menu.h"

#include "CustomLanguage.h"

#pragma region CM_Error_Inner

void* __cdecl CustomMenu_Error_Inner(s_new_ui_screen_parameters* parameters) {
	return c_error_menu::open(parameters);
}

void CustomMenuCall_Error_Inner() {
	if (!Memory::IsDedicatedServer()) {
		int WgitScreenfunctionPtr = (int)(CustomMenu_Error_Inner);
		CallWgit(WgitScreenfunctionPtr, 1);
	}

	/*char* lblTitle = H2CustomLanguageGetLabel(CMLabelMenuId_Error, 0xFFFFFFF0);
	char* lblDesc = H2CustomLanguageGetLabel(CMLabelMenuId_Error, 0xFFFFFFF1);
	if (!lblTitle || !lblDesc) {
		if (lblTitle)
			addDebugText(lblTitle);
		if (lblDesc)
			addDebugText(lblDesc);
	}
	else {
		addDebugText("%s - %s", lblTitle, lblDesc);
	}*/
}

void CustomMenuCall_Error_Inner(char* title, char* description) {
	add_cartographer_label(CMLabelMenuId_Error, 0xFFFFFFF0, title, true);
	add_cartographer_label(CMLabelMenuId_Error, 0xFFFFFFF1, description, true);
	CustomMenuCall_Error_Inner();
}

void CustomMenuCall_Error_Inner(int menuIdTitle, int title, int menuIdDesc, int description) {
	char* lblTitle = H2CustomLanguageGetLabel(menuIdTitle, title);
	char* lblDesc = H2CustomLanguageGetLabel(menuIdDesc, description);
	CustomMenuCall_Error_Inner(lblTitle, lblDesc);
}

void CustomMenuCall_Error_Inner(int menuId, int title, int description) {
	CustomMenuCall_Error_Inner(menuId, title, menuId, description);
}

#pragma endregion

void* __cdecl c_error_menu::open(s_new_ui_screen_parameters* parameters)
{
	c_error_menu* error_menu = nullptr;
	BYTE* ui_buffer = ui_memory_pool_allocate(sizeof(c_error_menu), 0);
	if (ui_buffer) {
		error_menu = new (ui_buffer) c_error_menu(parameters->ui_channel, parameters->field_8, HIWORD(parameters->flags));
	}
	error_menu->field_6C = true;
	user_interface_register_screen_to_channel(error_menu, parameters);
	return error_menu;
}

void c_error_edit_list::button_handler(int* a2, int* a3)
{
	int button_id = *(int*)a3 & 0xFFFF;

	int parent_screen_idx = this->get_top_most_parent_widget_index();
	int parent_screen_ui_channel = this->get_top_most_parent_widget_ui_channel();

	bool close_parent_screen = false;

	// ** you can open new screens if needed
	// s_new_ui_screen_parameters new_ui_;
	// new_ui_.data_new(0, 1 << *(int*)(*a2 + 4), parent_screen_ui_channel, 4, c_account_list_menu::open);

	// here place the button 

	if (close_parent_screen)
		user_interface_back_out_from_channel(parent_screen_ui_channel, parent_screen_idx);
	return;
}

c_error_edit_list::c_error_edit_list(int _flags) :
	c_list_widget(_flags, true)
{
	auto sub_2113C6 = Memory::GetAddress<int(__thiscall*)(void*)>(0x2113C6);
	auto sub_2113D3 = Memory::GetAddress<void* (__thiscall*)(void*, void*)>(0x2113D3);
	auto c_list_item_widget_ctor = Memory::GetAddress<void* (__thiscall*)(void*)>(0x21FFC9);
	auto c_list_item_widget_dctor = Memory::GetAddress<void(__thiscall*)(void*)>(0xA551);
	auto p_vector_constructor = Memory::GetAddress<void(__stdcall*)(void*, unsigned int, int, void* (__thiscall*)(void*), void(__thiscall*)(void*))>(0x28870B);

	// this constructs the buttons list widgets drawn inside the window
	// brightness menu can display 4 widget list buttons at once
	p_vector_constructor(this->item_list, 132, 4, c_list_item_widget_ctor, c_list_item_widget_dctor);

	this->field_2C0 = 0;
	sub_2113C6(&this->slot_2_unk.field_8);

	//this->slot_2_unk.c_slot_vtbl = &off_1229700;
	// this->slot_2_unk.c_slot_vtbl = NULL;
	// not sure why this exists, it just calls the function to the button handler
	// TODO implement virtual interface for slot_1, slot_2
	this->slot_2_unk.c_slot_vtbl = Memory::GetAddressRelative<void*>(0x7D9700);
	this->slot_2_unk.ui_screen_edit_list = this;
	this->slot_2_unk.button_handler_cb = &c_error_edit_list::button_handler;

	// ** here place how many buttons you will have
	int button_count = 0;
	//s_data_array* account_list_data = allocate_list_data("brightness edit list", button_count, 4);
	//this->list_data_array = account_list_data;

	// s_data_array::data_make_valid(account_list_data);

	/*for (unsigned int i = 0; i < this->list_data_array->datum_max_elements; i++) {
		s_data_array::datum_new_in_range(this->list_data_array);
	}*/

	// no fucking clue what's this, maybe related to this->slot_2_unk.c_slot_vtbl data offset
	// because this->slot_2_unk.c_slot_vtbl is at offset 708
	if ((void*)this == (void*)-708) {
		sub_2113D3(&this->gap_70[56], nullptr);
	}
	else {
		sub_2113D3(&this->gap_70[56], &this->slot_2_unk.field_8);
	}
}

c_error_menu::c_error_menu(int _ui_channel, int a4, int _flags) :
	c_screen_with_menu(BRIGHTNESS_MENU_ID, _ui_channel, a4, _flags, &error_edit_list, true),
	error_edit_list(_flags)
{
}

