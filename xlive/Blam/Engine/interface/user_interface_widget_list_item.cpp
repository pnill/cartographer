#include "stdafx.h"
#include "user_interface_widget_list_item.h"

c_list_item_widget::c_list_item_widget() :
	c_group_widget(_widget_type_list_item, NULL)
{
	//return INVOKE_TYPE(0x21FFC9, 0x0, c_list_item_widget*(__thiscall*)(c_list_item_widget*), this);

	this->m_last_data_index = NONE;
	this->m_intro_delay_milliseconds = 0;
	this->field_78 = 1;
	//signal_slot_ptr_reset((_slot*)&this->signal1);
	//signal_slot_ptr_reset((_slot*)&this->signal2);
	this->signal1 = nullptr;
	this->signal2 = nullptr;
	this->m_widget_type = _widget_type_list_item;
	this->field_6D = 1;

}

datum c_list_item_widget::get_last_data_index()
{
	return this->m_last_data_index;
}


// c_list_item_widget virtual functions

c_list_item_widget::~c_list_item_widget()
{
	//return INVOKE_TYPE(0x220057, 0x0, void*(__thiscall*)(c_list_item_widget*, char), lpMem,a2);
}

void c_list_item_widget::update()
{
	INVOKE_TYPE(0x220075, 0x0, void(__thiscall*)(c_list_item_widget*), this);
}

int c_list_item_widget::get_intro_delay()
{
	//return INVOKE_TYPE(0x220053, 0x0, int(__thiscall*)(c_list_item_widget*), this);
	return this->m_intro_delay_milliseconds;
}

char c_list_item_widget::handle_event(s_event_record* event)
{
	return INVOKE_TYPE(0x22009E, 0x0, char(__thiscall*)(c_list_item_widget*, s_event_record*), this, event);
}

void c_list_item_widget::construct_animation_on_region_enter(int a1)
{
	INVOKE_TYPE(0x2201EF, 0x0, void(__thiscall*)(c_list_item_widget*, int), this, a1);
}

void c_list_item_widget::construct_animation_on_region_leave(int a1)
{
	INVOKE_TYPE(0x2202FA, 0x0, void(__thiscall*)(c_list_item_widget*, int), this, a1);
}

bool c_list_item_widget::can_interact()
{
	return INVOKE_TYPE(0x21FF30, 0x0, bool(__thiscall*)(c_list_item_widget*), this);
}

bool c_list_item_widget::sub_6114B9()
{
	return INVOKE_TYPE(0x21FF05, 0x0, bool(__thiscall*)(c_list_item_widget*), this);
}

bool c_list_item_widget::last_data_index_valid()
{
	//return INVOKE_TYPE(0x21FF26, 0x0, bool(__thiscall*)(c_list_item_widget*), this);
	return !DATUM_IS_NONE(m_last_data_index);
}
