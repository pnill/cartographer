#include "stdafx.h"

#include "user_interface_widget_window.h"
#include "rasterizer/rasterizer_globals.h"

c_screen_widget::c_screen_widget(e_screen_id menu_id, e_user_interface_channel_type channel_type, e_user_interface_render_window window_index, int16 user_flags) :
	c_user_interface_widget(_widget_type_screen, user_flags),
	m_header_text((int16)0),
	m_screen_button_key_text((int16)0),
	m_screen_slot(this, &c_screen_widget::switch_panes)

{
	//INVOKE_TYPE(0x2106A2, 0x0, void(__thiscall*)(c_screen_widget*, e_user_interface_screen_id, e_user_interface_channel_type, e_user_interface_render_window, int16),
	//	this, menu_id, channel_type, window_index, user_flags);


	this->m_channel_type = channel_type;
	//this->interface_widget_vtable = &c_screen_widget::`vftable';
	this->m_screen_id = menu_id;
	this->m_window_index = window_index;
	this->m_child_count = NONE;

	this->m_pane_index = NONE;
	this->field_9FA = false;
	this->field_9FB = false;
	this->m_disable_overlay_effect = false;
	this->field_9FD = false;

	//h2v writes screen_id to some global variable ,
	//probably a leftover stuff from gfwl
	WriteValue<e_screen_id>(Memory::GetAddress(0x9758D8), menu_id);

	this->m_widget_type = _widget_type_screen;
	this->field_6D = true;
	this->m_hierarchy_order = ++this->m_child_count;

	rectangle2d frame_bounds;
	rasterizer_get_frame_bounds(&frame_bounds);
	this->set_bounds(&frame_bounds);

	csmemset(this->m_special_widgets, 0, sizeof(this->m_special_widgets));
	this->field_9FE = false;
	this->field_9FF = false;
	this->field_A00 = false;
	this->field_A01 = false;
}

void c_screen_widget::destroy()
{
	INVOKE_TYPE(0x20F6FF, 0x0, void(__thiscall*)(c_screen_widget*), this);
}

char c_screen_widget::switch_panes(long* pane_index_ptr)
{
	return INVOKE_TYPE(0x210227, 0x0, char(__thiscall*)(c_screen_widget*, long*), this, pane_index_ptr);
}

void c_screen_widget::verify_and_load_from_layout(datum widget_tag, s_interface_expected_screen_layout* expected_layout)
{
	INVOKE_TYPE(0x210A44, 0x0, void(__thiscall*)(c_screen_widget*, datum, s_interface_expected_screen_layout*), this, widget_tag, expected_layout);
}
void c_screen_widget::apply_new_representations_to_players(c_player_widget_representation* representations, int32 player_count)
{
	INVOKE_TYPE(0x211CD0, 0x0, void(__thiscall*)(c_screen_widget*, c_player_widget_representation*, int32), this, representations, player_count);
}
void* c_screen_widget::get_screen_definition()
{
	return INVOKE_TYPE(0x20E8A6, 0x0, void*(__thiscall*)(c_screen_widget*), this);
}

void c_screen_widget::animate_screen(int32 type)
{
	INVOKE_TYPE(0x212604, 0x0, void(__thiscall*)(c_screen_widget*, int32), this, type);
}


// c_screen_widget virtual functions

c_screen_widget::~c_screen_widget()
{
	// todo : is this necessary?
	this->destroy();
}


char c_screen_widget::handle_event(s_event_record* event)
{
	return INVOKE_TYPE(0x20EB2B, 0x0, char(__thiscall*)(c_screen_widget*, s_event_record*), this, event);
}

c_user_interface_text* c_screen_widget::get_interface()
{
	//return INVOKE_TYPE(0xAD50, 0x0, c_user_interface_text * (__thiscall*)(c_screen_widget*), this);
	return nullptr;
}

bool c_screen_widget::sub_6114B9()
{
	return INVOKE_TYPE(0x20E885, 0x0, bool(__thiscall*)(c_screen_widget*), this);
}

// c_screen_widget includes

void c_screen_widget::sub_60E884()
{
	//INVOKE_TYPE(0x20E884, 0x0, void(__thiscall*)(c_screen_widget*), this);
	return;
}

void c_screen_widget::cache_local_bitmaps_and_cursor()
{
	INVOKE_TYPE(0x20F790, 0x0, void(__thiscall*)(c_screen_widget*), this);
}

void c_screen_widget::setup_special_widgets()
{
	INVOKE_TYPE(0x210B4E, 0x0, void(__thiscall*)(c_screen_widget*), this);
}

c_user_interface_widget* c_screen_widget::sub_6102C5()
{
	return INVOKE_TYPE(0x2102C5, 0x0, c_user_interface_widget * (__thiscall*)(c_screen_widget*), this);
}

char c_screen_widget::sub_6103D6()
{
	return INVOKE_TYPE(0x2103D6, 0x0, char(__thiscall*)(c_screen_widget*), this);
}

int c_screen_widget::sub_60F1F4(s_event_record* a2)
{
	return INVOKE_TYPE(0x20F1F4, 0x0, int(__thiscall*)(c_screen_widget*, s_event_record*), this, a2);
}

char c_screen_widget::sub_60EFC1(s_event_record* event)
{
	return INVOKE_TYPE(0x20EFC1, 0x0, char(__thiscall*)(c_screen_widget*, s_event_record*), this, event);
}

int c_screen_widget::sub_60F081(s_event_record* a2)
{
	return INVOKE_TYPE(0x20F081, 0x0, int(__thiscall*)(c_screen_widget*, s_event_record*), this, a2);
}

int c_screen_widget::sub_60F151(int a2)
{
	return INVOKE_TYPE(0x20F151, 0x0, int(__thiscall*)(c_screen_widget*, int), this, a2);
}

char c_screen_widget::sub_40AD53()
{
	return INVOKE_TYPE(0xAD53, 0x0, char(__thiscall*)(c_screen_widget*), this);
}

e_user_interface_channel_type c_screen_widget::get_channel()
{
	//return INVOKE_TYPE(0x20EB8A, 0x0, e_user_interface_channel_type(__thiscall*)(c_screen_widget*), this);
	return this->m_channel_type;
}

e_user_interface_render_window c_screen_widget::get_render_window()
{
	//return INVOKE_TYPE(0x20EB8E, 0x0, e_user_interface_render_window(__thiscall*)(c_screen_widget*), this);
	return this->m_window_index;
}

int c_screen_widget::sub_60EB92(int a2)
{
	return INVOKE_TYPE(0x20EB92, 0x0, int(__thiscall*)(c_screen_widget*, int), this, a2);
}

void c_screen_widget::sub_60EBC2(int a1)
{
	//INVOKE_TYPE(0x20EBC2, 0x0, void(__thiscall*)(c_screen_widget*, int), this, a1);
	return;
}

void* c_screen_widget::sub_60EC5C(s_screen_state* state)
{
	return INVOKE_TYPE(0x20EC5C, 0x0, void* (__thiscall*)(c_screen_widget*, s_screen_state*), this, state);
}

void c_screen_widget::sub_60ECC9(s_screen_state* state)
{
	INVOKE_TYPE(0x20ECC9, 0x0, void(__thiscall*)(c_screen_widget*, s_screen_state*), this, state);
}

bool c_screen_widget::overlay_effect_is_disabled()
{
	//return INVOKE_TYPE(0xAD58, 0x0, bool(__thiscall*)(c_screen_widget*), this);
	return this->m_disable_overlay_effect;
}

void c_screen_widget::sub_60F2A4(char bitmap_index)
{
	return INVOKE_TYPE(0x20F2A4, 0x0, void(__thiscall*)(c_screen_widget*, char), this, bitmap_index);
}


void user_interface_register_screen_to_channel(c_screen_widget* new_screen, s_screen_parameters* parameters)
{
	return INVOKE_TYPE(0x20B8C3, 0x0, void(__cdecl*)(c_screen_widget*, s_screen_parameters*), new_screen, parameters);
}
