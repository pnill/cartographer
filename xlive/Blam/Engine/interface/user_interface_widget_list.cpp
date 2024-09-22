#include "stdafx.h"
#include "user_interface_widget_list.h"
#include "user_interface_widget_list_item.h"
#include "user_interface_widget_text.h"

c_list_widget::c_list_widget(uint16 user_flags)
	: c_user_interface_widget(_widget_type_list, user_flags)
{
	// INVOKE_TYPE(0x213B1C, 0x0, c_list_widget(*__thiscall*)(c_list_widget*, __int16), this, user_flags);

	this->m_list_data = 0;
	this->m_up_arrow_transition_time = 0;
	this->m_down_arrow_transition_time = 0;
	this->m_intro_delay_milliseconds = 0;
	this->field_A2 = 0;
	this->m_list_wraps = 0;
	this->m_list_interactive = 0;
	this->m_list_has_hidden_items = 0;
	this->field_A6 = 0;
	this->field_A7 = 1;
}

c_list_item_widget* c_list_widget::try_find_item_widget(uint32 idx)
{
	return INVOKE_TYPE(0x213772, 0x0, c_list_item_widget * (__thiscall*)(c_list_widget*, int), this, idx);
}

datum c_list_widget::get_old_data_index()
{
	return INVOKE_TYPE(0x21383B, 0x0, datum(__thiscall*)(c_list_widget*), this);
}

void c_list_widget::update_list_items_from_mapping(c_list_item_widget* item, int32 skin_index, int32 text_widget_idx, s_item_text_mapping* mapping, int32 total_mappings)
{
	//INVOKE_TYPE(0x2139F8, 0x0, void(__thiscall*)(c_list_widget*, c_list_item_widget*, int32, int32, s_item_text_mapping*, int32), this, item, skin_index, text_widget_idx, mapping, total_mappings);

	if (item == nullptr)
		return;

	c_text_widget* item_text = item->try_find_text_widget(text_widget_idx);
	if (item_text)
	{
		s_list_item_datum* item_datum = (s_list_item_datum*)datum_try_and_get(this->m_list_data, item->get_last_data_index());
		if (item_datum && total_mappings > 0)
		{
			int16 mapping_idx = 0;
			while (mapping[mapping_idx].item_id != item_datum->item_id)
			{
				if (++mapping_idx > total_mappings)
					return;
			}
			item_text->set_text_from_string_id(mapping[mapping_idx].item_text);
		}
	}
}
void c_list_widget::set_focused_item_index(datum item_index)
{
	INVOKE_TYPE(0x213F50, 0x0, void(__thiscall*)(c_list_widget*, __int16), this, DATUM_INDEX_TO_ABSOLUTE_INDEX(item_index));
}

void c_list_widget::remove_focused_item_datum_from_data_array()
{
	INVOKE_TYPE(0x214C32, 0x0, void(__thiscall*)(c_list_widget*), this);
}

void c_list_widget::remove_item_from_list(c_list_item_widget* item)
{
	INVOKE_TYPE(0x2144A4, 0x0, void(__thiscall*)(c_list_widget*, c_list_item_widget*), this, item);
}


// c_list_widget virtual functions

c_list_widget::~c_list_widget()
{
}

int32 c_list_widget::setup_children()
{
	return INVOKE_TYPE(0x213FE5, 0x0, int32(__thiscall*)(c_list_widget*), this);
}

void c_list_widget::on_screen_leave()
{
	INVOKE_TYPE(0x2136A3, 0x0, void(__thiscall*)(c_list_widget*), this);
}

void c_list_widget::update()
{
	INVOKE_TYPE(0x213CAC, 0x0, void(__thiscall*)(c_list_widget*), this);
}

void c_list_widget::render_widget(rectangle2d* viewport_bounds)
{
	INVOKE_TYPE(0x21401F, 0x0, void(__thiscall*)(c_list_widget*, rectangle2d*), this, viewport_bounds);
}

int32 c_list_widget::get_intro_delay()
{
	//return INVOKE_TYPE(0xB69B, 0x0, int32(__thiscall*)(c_list_widget*), this);
	return this->m_intro_delay_milliseconds;
}

bool c_list_widget::handle_event(s_event_record* event)
{
	return INVOKE_TYPE(0x214CA9, 0x0, bool(__thiscall*)(c_list_widget*, s_event_record*), this, event);
}

void c_list_widget::construct_animation_on_region_enter(int32 a1)
{
	INVOKE_TYPE(0x21447E, 0x0, void(__thiscall*)(c_list_widget*, int32), this, a1);
}

c_user_interface_text* c_list_widget::get_interface()
{
	//return INVOKE_TYPE(0xB698, 0x0, c_user_interface_text*(__thiscall*)(c_list_widget*), this);
	return nullptr;
}

int32 c_list_widget::link_item_widgets()
{
	return INVOKE_TYPE(0x213BDD, 0x0, int32(__thiscall*)(c_list_widget*), this);
}

bool c_list_widget::verify_item_in_focus(c_list_item_widget* item)
{
	return INVOKE_TYPE(0x21379C, 0x0, bool(__thiscall*)(c_list_widget*, c_list_item_widget*), this, item);
}
