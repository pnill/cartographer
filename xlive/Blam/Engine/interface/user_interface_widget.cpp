#include "stdafx.h"

#include "user_interface_widget.h"

c_user_interface_widget::c_user_interface_widget(e_user_interface_widget_type widget_type, int16 user_flags)
{
	//INVOKE_TYPE(0x211D81, 0x0, void(__thiscall*)(c_user_interface_widget*, e_user_interface_widget_type, __int16), this, widget_type, user_flags);


	this->m_widget_type = widget_type;
	this->m_controllers_mask = user_flags;
	//this->interface_widget_vtable = &c_user_interface_widget::`vftable';
	this->m_block_index = NONE;
	this->m_hierarchy_order = NONE;
	this->parent_widget = nullptr;
	this->child_widget = nullptr;
	this->next_widget = nullptr;
	this->previous_widget = nullptr;
	this->m_animation_index = NONE;
	this->m_render_dept_bias = 0;
	this->m_allocated = false;
	this->field_6D = true;
	this->m_visible = true;
	this->m_can_handle_events = true;


	csmemset(&this->m_bounds, 0, sizeof(rectangle2d));
	this->m_widget_color.blue = 1.0;
	this->m_widget_color.green = 1.0;
	this->m_widget_color.red = 1.0;


	s_animation_transform animation;
	csmemset(&animation, 0, sizeof(animation));
	animation.field_0 = NONE;
	animation.direction = 1;
	animation.looping_stlye = 0;
	this->initialize_animation(&animation);
}


void c_user_interface_widget::destroy_recursive()
{
	INVOKE_TYPE(0x211E65, 0x0, void(__thiscall*)(c_user_interface_widget*), this);
}
void c_user_interface_widget::initialize_animation(s_animation_transform* animation)
{
	INVOKE_TYPE(0x2115FE, 0x0, void(__thiscall*)(c_user_interface_widget*, s_animation_transform*), this, animation);
}


/* public methods */

e_user_interface_widget_type c_user_interface_widget::get_type()
{
	return this->m_widget_type;
}

c_user_interface_widget* c_user_interface_widget::get_next()
{
	return this->next_widget;
}

c_user_interface_widget* c_user_interface_widget::get_previous()
{
	return this->previous_widget;
}

c_user_interface_widget* c_user_interface_widget::get_parent()
{
	return this->parent_widget;
}

c_user_interface_widget* c_user_interface_widget::get_children()
{
	return this->child_widget;
}

c_user_interface_widget* c_user_interface_widget::try_find_child(e_user_interface_widget_type type, uint32 idx, bool recursive_search)
{
	return INVOKE_TYPE(0x211909, 0x0, c_user_interface_widget * (__thiscall*)(c_user_interface_widget*, e_user_interface_widget_type, uint32, bool), this, type, idx, recursive_search);
}

c_text_widget* c_user_interface_widget::try_find_text_widget(uint32 idx)
{
	return (c_text_widget*)try_find_child(_widget_type_text, idx, false);
}

c_hud_widget* c_user_interface_widget::try_find_hud_widget(uint32 idx)
{
	return (c_hud_widget*)try_find_child(_widget_type_hud, idx, false);
}

c_bitmap_widget* c_user_interface_widget::try_find_bitmap_widget(uint32 idx)
{
	return (c_bitmap_widget*)try_find_child(_widget_type_bitmap, idx, false);
}

c_player_widget* c_user_interface_widget::try_find_player_widget(uint32 idx)
{
	return (c_player_widget*)try_find_child(_widget_type_player, idx, false);
}

c_model_widget* c_user_interface_widget::try_find_model_widget(uint32 idx)
{
	return (c_model_widget*)try_find_child(_widget_type_model, idx, false);
}

c_screen_widget* c_user_interface_widget::get_parent_screen()
{
	return INVOKE_TYPE(0x211BA1, 0x0, c_screen_widget*(__thiscall*)(c_user_interface_widget*), this);
}

void c_user_interface_widget::set_visible(bool visible)
{
	this->m_visible = visible;
}

void c_user_interface_widget::set_child_visible(e_user_interface_widget_type type, uint32 idx, bool visible)
{
	c_user_interface_widget* widget = this->try_find_child(type, idx, false);
	if (widget)
	{
		widget->set_visible(visible);
	}
}

void c_user_interface_widget::set_allocated(bool allocated)
{
	this->m_allocated = allocated;
}

void c_user_interface_widget::add_new_child(c_user_interface_widget* child)
{
	INVOKE_TYPE(0x21208E, 0x0, void(__thiscall*)(c_user_interface_widget*, c_user_interface_widget*), this, child);
}

void c_user_interface_widget::set_bounds(rectangle2d* bounds)
{
	//return INVOKE_TYPE(0x2116D2, 0x0, void(__thiscall*)(c_user_interface_widget*, rectangle2d*), this, bounds);
	this->m_bounds.top = bounds->top;
	this->m_bounds.left = bounds->left;
	this->m_bounds.bottom = bounds->bottom;
	this->m_bounds.right = bounds->right;
}

void c_user_interface_widget::set_controller_mask(uint32 user_mask)
{
	INVOKE_TYPE(0x211B37, 0x0, void(__thiscall*)(c_user_interface_widget*, uint32), this, user_mask);
}




// c_user_interface_widget virtual functions

c_user_interface_widget::~c_user_interface_widget()
{
	//return INVOKE_TYPE(0x212734, 0x0, c_user_interface_widget*(__thiscall*)(c_user_interface_widget*, char), lpMem, a2);
	this->destroy_recursive();
}


int32 c_user_interface_widget::setup_children()
{
	return INVOKE_TYPE(0x211E23, 0x0, int32(__thiscall*)(c_user_interface_widget*), this);
}

void c_user_interface_widget::on_screen_leave()
{
	INVOKE_TYPE(0x211488, 0x0, void(__thiscall*)(c_user_interface_widget*), this);
}

void c_user_interface_widget::update()
{
	INVOKE_TYPE(0x212CD8, 0x0, void(__thiscall*)(c_user_interface_widget*), this);
}

void c_user_interface_widget::render_widget(rectangle2d* viewport_bounds)
{
	INVOKE_TYPE(0x2114DD, 0x0, void(__thiscall*)(c_user_interface_widget*, rectangle2d*), this, viewport_bounds);
}

void* c_user_interface_widget::get_mouse_region(rectangle2d* mouse_region_out)
{
	return INVOKE_TYPE(0x2114E0, 0x0, void* (__thiscall*)(c_user_interface_widget*, rectangle2d*), this, mouse_region_out);
}

int32 c_user_interface_widget::initialize_child_animations(s_animation_transform* a2)
{
	return INVOKE_TYPE(0x211692, 0x0, int32(__thiscall*)(c_user_interface_widget*, s_animation_transform*), this, a2);
}

int32 c_user_interface_widget::get_intro_delay()
{
	return INVOKE_TYPE(0x2116BD, 0x0, int32(__thiscall*)(c_user_interface_widget*), this);
}

void* c_user_interface_widget::sub_611703(rectangle2d* unprojected_bounds)
{
	return INVOKE_TYPE(0x211703, 0x0, void* (__thiscall*)(c_user_interface_widget*, rectangle2d*), this, unprojected_bounds);
}

void c_user_interface_widget::sub_612A7C(c_user_interface_widget* a2)
{
	INVOKE_TYPE(0x212A7C, 0x0, void(__thiscall*)(c_user_interface_widget*, c_user_interface_widget*), this, a2);
}

c_user_interface_widget* c_user_interface_widget::sub_612ABC()
{
	return INVOKE_TYPE(0x212ABC, 0x0, c_user_interface_widget * (__thiscall*)(c_user_interface_widget*), this);
}

c_user_interface_widget* c_user_interface_widget::sub_612BCA()
{
	return INVOKE_TYPE(0x212BCA, 0x0, c_user_interface_widget * (__thiscall*)(c_user_interface_widget*), this);
}

uint8 c_user_interface_widget::handle_event(s_event_record* a2)
{
	return INVOKE_TYPE(0x2118F0, 0x0, uint8(__thiscall*)(c_user_interface_widget*, s_event_record*), this, a2);
}

e_user_interface_channel_type c_user_interface_widget::get_parent_channel()
{
	return INVOKE_TYPE(0x2120F8, 0x0, e_user_interface_channel_type(__thiscall*)(c_user_interface_widget*), this);
}

e_user_interface_render_window c_user_interface_widget::get_parent_render_window()
{
	return INVOKE_TYPE(0x212120, 0x0, e_user_interface_render_window(__thiscall*)(c_user_interface_widget*), this);
}

void c_user_interface_widget::construct_animation_on_region_enter(int32 a1)
{
	// does nothing for base class
	//INVOKE_TYPE(0x211903, 0x0, void(__thiscall*)(c_user_interface_widget*, int32), this, a1);
}

void c_user_interface_widget::construct_animation_on_region_leave(int32 a1)
{
	// does nothing for base class
	//INVOKE_TYPE(0x211906, 0x0, void(__thiscall*)(c_user_interface_widget*, int32), this, a1);
}

c_user_interface_widget* c_user_interface_widget::sub_6121F6(rectangle2d* point)
{
	return INVOKE_TYPE(0x2121F6, 0x0, c_user_interface_widget * (__thiscall*)(c_user_interface_widget*, rectangle2d*), this, point);
}

bool c_user_interface_widget::can_interact()
{
	//return INVOKE_TYPE(0xAD4D, 0x0, bool(__thiscall*)(c_user_interface_widget*), this);
	return 0;
}

bool c_user_interface_widget::sub_6114B9()
{
	return INVOKE_TYPE(0x2114B9, 0x0, bool(__thiscall*)(c_user_interface_widget*), this);
}
