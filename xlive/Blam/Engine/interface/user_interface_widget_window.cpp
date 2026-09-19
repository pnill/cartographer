#include "stdafx.h"
#include "user_interface_widget_window.h"

#include "rasterizer/rasterizer_globals.h"

/* public code */

c_screen_widget::c_screen_widget(e_user_interface_screen_id menu_id, e_user_interface_channel_type channel_type, e_user_interface_render_window window_index, uint16 user_flags) :
	c_user_interface_widget(_widget_type_screen, user_flags),
	m_header_text((int16)0),
	m_screen_button_key_text((int16)0),
	m_screen_slot(this, &c_screen_widget::switch_panes)
{
	/*
	//INVOKE_TYPE(0x2106A2, 0x0, void(__thiscall*)(c_screen_widget*, e_user_interface_screen_id, e_user_interface_channel_type, e_user_interface_render_window, int16),
		this,
		menu_id,
		channel_type,
		window_index,
		user_flags);
	*/

	m_channel_type = channel_type;
	m_screen_id = menu_id;
	m_window_index = window_index;
	m_child_count = NONE;

	m_pane_index = NONE;
	field_9FA = false;
	field_9FB = false;
	m_disable_overlay_effect = false;
	field_9FD = false;

	// h2v writes screen_id to some global variable,
	// probably some leftover stuff from gfwl
	*Memory::GetAddress<e_user_interface_screen_id*>(0x9758D8) = menu_id;

	m_widget_type = _widget_type_screen;
	field_6D = true;
	m_hierarchy_order = ++m_child_count;

	rectangle2d frame_bounds;
	rasterizer_get_frame_bounds(&frame_bounds);
	set_bounds(&frame_bounds);

	csmemset(m_special_widgets, 0, sizeof(m_special_widgets));
	field_9FE = false;
	field_9FF = false;
	field_A00 = false;
	field_A01 = false;

	return;
}

e_user_interface_screen_id c_screen_widget::get_id(void) const
{
	return m_screen_id;
}

c_text_widget* c_screen_widget::get_screen_header_text(void)
{
	return try_find_text_widget(k_header_text_block_index);
}

c_text_widget* c_screen_widget::get_screen_button_key_text(void)
{
	return try_find_text_widget(k_button_key_text_block_index);
}

// only use this when you are sure the screen texts start from 2
// that means 0 and 1 are filled up by screen_header and button_key
// dont use this when there is a subheader / or in c_screen_with_menus
c_text_widget* c_screen_widget::try_find_screen_text(
	uint32 idx)
{
	return try_find_text_widget(TEXT_BLOCK_INDEX_TO_WIDGET_INDEX(idx));
}

void c_screen_widget::destroy(void)
{
	INVOKE_TYPE(0x20F6FF, 0x0, void(__thiscall*)(c_screen_widget*), this);
	return;
}

void c_screen_widget::switch_panes(
	int32* pane_index_ptr)
{
	INVOKE_TYPE(0x210227, 0x0, bool(__thiscall*)(c_screen_widget*, int32*), this, pane_index_ptr);
	return;
}

void c_screen_widget::verify_and_load_from_layout(
	datum widget_tag,
	s_interface_expected_screen_layout* expected_layout)
{
	INVOKE_TYPE(0x210A44, 0x0, void(__thiscall*)(c_screen_widget*, datum, s_interface_expected_screen_layout*), this, widget_tag, expected_layout);
	return;
}

void c_screen_widget::apply_new_representations_to_players(
	c_player_widget_representation* representations,
	int32 player_count)
{
	INVOKE_TYPE(0x211CD0, 0x0, void(__thiscall*)(c_screen_widget*, c_player_widget_representation*, int32), this, representations, player_count);
	return;
}
void c_screen_widget::initialize_button_keys_text(
	bool add_new_child)
{
	return INVOKE_TYPE(0x20FF73, 0x0, void(__thiscall*)(c_screen_widget*, bool), this, add_new_child);
}

void c_screen_widget::set_list_arrows_widget(
	c_bitmap_widget* up_arrow,
	c_bitmap_widget* down_arrow)
{
	//INVOKE_TYPE(0x20EF47, 0x0, void(__thiscall*)(c_screen_widget*, c_bitmap_widget*, c_bitmap_widget*), this, up_arrow, down_arrow);
	
	m_special_widgets[_special_widget_type_0] = (c_user_interface_widget*)up_arrow;
	m_special_widgets[_special_widget_type_1] = (c_user_interface_widget*)down_arrow;
	return;
}

void* c_screen_widget::get_screen_definition(void)
{
	return INVOKE_TYPE(0x20E8A6, 0x0, void*(__thiscall*)(c_screen_widget*), this);
}

void c_screen_widget::get_string_by_string_id(
	string_id sid,
	c_maximum_interface_text* out_string)
{
	INVOKE_TYPE(0x2101A4, 0x0, void(__thiscall*)(c_screen_widget*, string_id, c_maximum_interface_text*), this, sid, out_string);
	return;
}


// c_screen_widget virtual functions

bool c_screen_widget::handle_event(
	s_event_record* event)
{
	return INVOKE_TYPE(0x20EB2B, 0x0, bool(__thiscall*)(c_screen_widget*, s_event_record*), this, event);
}

c_user_interface_text* c_screen_widget::get_interface(void)
{
	//return INVOKE_TYPE(0xAD50, 0x0, c_user_interface_text * (__thiscall*)(c_screen_widget*), this);
	return nullptr;
}

bool c_screen_widget::sub_6114B9(void)
{
	return INVOKE_TYPE(0x20E885, 0x0, bool(__thiscall*)(c_screen_widget*), this);
}

// c_screen_widget includes

void c_screen_widget::sub_60E884(void)
{
	//INVOKE_TYPE(0x20E884, 0x0, void(__thiscall*)(c_screen_widget*), this);
	return;
}

void c_screen_widget::post_initialize(void)
{
	INVOKE_TYPE(0x20F790, 0x0, void(__thiscall*)(c_screen_widget*), this);
	return;
}

void c_screen_widget::post_initialize_button_keys(void)
{
	INVOKE_TYPE(0x210B4E, 0x0, void(__thiscall*)(c_screen_widget*), this);
	return;
}

c_user_interface_widget* c_screen_widget::sub_6102C5(void)
{
	return INVOKE_TYPE(0x2102C5, 0x0, c_user_interface_widget * (__thiscall*)(c_screen_widget*), this);
}

uint8 c_screen_widget::sub_6103D6(void)
{
	return INVOKE_TYPE(0x2103D6, 0x0, uint8(__thiscall*)(c_screen_widget*), this);
}

int32 c_screen_widget::sub_60F1F4(
	s_event_record* a2)
{
	return INVOKE_TYPE(0x20F1F4, 0x0, int32(__thiscall*)(c_screen_widget*, s_event_record*), this, a2);
}

uint8 c_screen_widget::sub_60EFC1(
	s_event_record* event)
{
	return INVOKE_TYPE(0x20EFC1, 0x0, uint8(__thiscall*)(c_screen_widget*, s_event_record*), this, event);
}

int32 c_screen_widget::sub_60F081(
	s_event_record* a2)
{
	return INVOKE_TYPE(0x20F081, 0x0, int32(__thiscall*)(c_screen_widget*, s_event_record*), this, a2);
}

e_user_interface_controller_component c_screen_widget::get_component_from_button_key(
	int32 special_widget_index)
{
	//converts private_use_characters to controller_component
	// anything other than a,b,x,y defaults to a
	//override this if you want to add support for say d-pad up or triggers
	return INVOKE_TYPE(0x20F151, 0x0, e_user_interface_controller_component(__thiscall*)(c_screen_widget*, int32), this, special_widget_index);
}

bool c_screen_widget::sub_40AD53(
	int32 a2)
{
	return INVOKE_TYPE(0xAD53, 0x0, bool(__thiscall*)(c_screen_widget*, int32), this, a2);
}

e_user_interface_channel_type c_screen_widget::get_channel(void)
{
	//return INVOKE_TYPE(0x20EB8A, 0x0, e_user_interface_channel_type(__thiscall*)(c_screen_widget*), this);
	return m_channel_type;
}

e_user_interface_render_window c_screen_widget::get_render_window(void)
{
	//return INVOKE_TYPE(0x20EB8E, 0x0, e_user_interface_render_window(__thiscall*)(c_screen_widget*), this);
	return m_window_index;
}

int32 c_screen_widget::sub_60EB92(
	int32 a2)
{
	return INVOKE_TYPE(0x20EB92, 0x0, int32(__thiscall*)(c_screen_widget*, int32), this, a2);
}

void c_screen_widget::sub_60EBC2(
	int32 a1)
{
	//INVOKE_TYPE(0x20EBC2, 0x0, void(__thiscall*)(c_screen_widget*, int), this, a1);
	return;
}

void* c_screen_widget::sub_60EC5C(
	s_screen_state* state)
{
	return INVOKE_TYPE(0x20EC5C, 0x0, void* (__thiscall*)(c_screen_widget*, s_screen_state*), this, state);
}

void c_screen_widget::sub_60ECC9(
	s_screen_state* state)
{
	INVOKE_TYPE(0x20ECC9, 0x0, void(__thiscall*)(c_screen_widget*, s_screen_state*), this, state);
}

bool c_screen_widget::overlay_effect_is_disabled(void)
{
	//return INVOKE_TYPE(0xAD58, 0x0, bool(__thiscall*)(c_screen_widget*), this);
	return m_disable_overlay_effect;
}

void c_screen_widget::set_favourites_bitmap_visible(
	bool show_icon)
{
	return INVOKE_TYPE(0x20F2A4, 0x0, void(__thiscall*)(c_screen_widget*, bool), this, show_icon);
}

void user_interface_register_screen_to_channel(
	c_screen_widget* new_screen,
	c_screen_parameters const* parameters)
{
	return INVOKE_TYPE(0x20B8C3, 0x0, void(__cdecl*)(c_screen_widget*, c_screen_parameters const*), new_screen, parameters);
}



c_screen_with_menu::c_screen_with_menu(
	e_user_interface_screen_id menu_id,
	e_user_interface_channel_type channel_type,
	e_user_interface_render_window window_index,
	uint16 user_flags,
	c_list_widget* list) :
	c_screen_widget(menu_id,channel_type,window_index,user_flags)
{
	m_child_list = list;
	return;
}

// c_screen_with_menus generally have header,buttonkeys and subheader indices fixed
// verify before using this , not all widgets have a subheader
c_text_widget* c_screen_with_menu::get_screen_subheader_text(void)
{
	return try_find_text_widget(k_sub_header_text_block_index);
}


bool c_screen_with_menu::handle_event(
	s_event_record* event)
{
	return INVOKE_TYPE(0x20EEBE, 0x0, bool(__thiscall*)(c_screen_with_menu*, s_event_record*), this, event);
}

c_user_interface_widget* c_screen_with_menu::sub_6121F6(
	rectangle2d* point)
{
	return INVOKE_TYPE(0x20EF08, 0x0, c_user_interface_widget * (__thiscall*)(c_screen_with_menu*, rectangle2d*), this, point);
}

void c_screen_with_menu::initialize(
	c_screen_parameters const* parameters)
{
	INVOKE_TYPE(0x2111AB, 0x0, int(__thiscall*)(c_screen_with_menu*, c_screen_parameters const*), this, parameters);
	return;
}
