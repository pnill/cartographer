#include "stdafx.h"

#include "Util/Hooks/Hook.h"

#include "c_brightness_menu_boilerplate.h"

c_brightness_menu::c_brightness_menu(int a3, int a4, int a5) :
	c_screen_with_menu(BRIGHTNESS_MENU_ID, a3, a4, a5, &this->list_widgets),
	list_widgets(a5)
{
}

void* __cdecl c_brightness_menu::open_brightness_menu(s_new_ui_screen_parameters* parameters)
{
	c_brightness_menu* brightness_menu = nullptr;
	c_brightness_menu* ui_buffer = (c_brightness_menu*)ui_memory_pool_allocate(sizeof(c_brightness_menu), 0);

	// ** here you can place some code you need to execute before opening the menu

	if (ui_buffer) {
		brightness_menu = new (ui_buffer) c_brightness_menu(parameters->ui_channel, parameters->field_8, HIWORD(parameters->flags)); // manually call the constructor
	}

	brightness_menu->field_6C = true;
	user_interface_register_screen_to_channel(brightness_menu, parameters);
	return brightness_menu;
}

c_account_edit_list::c_account_edit_list(int _flags) :
	c_list_widget(_flags)
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
	this->slot_2_unk.button_handler_cb = &c_account_edit_list::button_handler;

	// ** here place how many buttons you will have
	int button_count = 0;
	s_data_array* account_list_data = allocate_list_data("brightness edit list", button_count, 4);
	this->list_data_array = account_list_data;

	s_data_array::data_make_valid(account_list_data);

	for (unsigned int i = 0; i < this->list_data_array->datum_max_elements; i++) {
		s_data_array::datum_new_in_range(this->list_data_array);
	}

	// no fucking clue what's this, maybe related to this->slot_2_unk.c_slot_vtbl data offset
	// because this->slot_2_unk.c_slot_vtbl is at offset 708
	if ((void*)this == (void*)-708) {
		sub_2113D3(&this->gap_70[56], nullptr);
	}
	else {
		sub_2113D3(&this->gap_70[56], &this->slot_2_unk.field_8);
	}
}

void c_account_edit_list::button_handler(int* a2, int* a3)
{
	int button_id = *(int*)a3 & 0xFFFF;

	int parent_screen_idx = this->get_top_most_parent_widget_index();
	int parent_screen_ui_channel = this->get_top_most_parent_widget_ui_channel();

	bool close_parent_screen = true;

	// ** you can open new screens if needed
	// s_new_ui_screen_parameters new_ui_;
	// new_ui_.data_new(0, 1 << *(int*)(*a2 + 4), parent_screen_ui_channel, 4, c_account_list_menu::open);

	// here place the button 

	if (close_parent_screen)
		user_interface_back_out_from_channel(parent_screen_ui_channel, parent_screen_idx);
	return;
}
