#include "stdafx.h"

#include "user_interface_bitmap_block.h"

const real_vector2d* c_bitmap_widget::get_render_scale()
{
	return &m_render_scale;
}

bitmap_data* c_bitmap_widget::get_current_bitmap_data()
{
	return INVOKE_TYPE(0x21C941, 0x0, bitmap_data * (__thiscall*)(c_bitmap_widget*), this);
}

void c_bitmap_widget::verify_and_update_bitmap_index(int16 index)
{
	INVOKE_TYPE(0x21C300, 0x0, void(__thiscall*)(c_bitmap_widget*, int16), this, index);
}
void c_bitmap_widget::assign_new_bitmap_block(bitmap_data* block)
{
	INVOKE_TYPE(0x21CF73, 0x0, void(__thiscall*)(c_bitmap_widget*, bitmap_data*), this, block);
}

void c_bitmap_widget::set_local_bitmap(int16 local_bitmap_block_index, int16 bitmap_block_index)
{
	INVOKE_TYPE(0x21CFED, 0x0, int(__thiscall*)(c_bitmap_widget*, int16, int16), this, local_bitmap_block_index, bitmap_block_index);
}


void c_bitmap_widget::set_render_scale(const real_vector2d* scale)
{
	if (scale)
	{
		this->m_render_scale = *scale;
	}
}


// c_bitmap_widget virtual functions

c_user_interface_widget* c_bitmap_widget::destructor(uint32 flags)
{
	this->~c_bitmap_widget();
	if (TEST_BIT(flags, 0))
	{
	}

	return this;
}

void c_bitmap_widget::update()
{
	return INVOKE_TYPE(0x21C9AC, 0x0, void(__thiscall*)(c_bitmap_widget*), this);
}

void c_bitmap_widget::render_widget(rectangle2d* viewport_bounds)
{
	return INVOKE_TYPE(0x21D046, 0x0, void(__thiscall*)(void*,rectangle2d*), this, viewport_bounds);
}

int32 c_bitmap_widget::get_intro_delay()
{
	return INVOKE_TYPE(0x21C2EF, 0x0, int32(__thiscall*)(c_bitmap_widget*), this);
}

void* c_bitmap_widget::get_unprojected_bounds(rectangle2d* unprojected_bounds)
{
	return INVOKE_TYPE(0x21C364, 0x0, void*(__thiscall*)(c_bitmap_widget*, rectangle2d*), this, unprojected_bounds);
}

c_user_interface_text* c_bitmap_widget::get_interface()
{
	//return INVOKE_TYPE(0x21C5DC, 0x0, c_user_interface_text*(__thiscall*)(c_bitmap_widget*), this);
	return nullptr;
}
