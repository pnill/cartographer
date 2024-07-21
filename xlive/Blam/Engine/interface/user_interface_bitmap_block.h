#pragma once
#include "user_interface_widget.h"
#include "user_interface_screen_widget_definition.h"
#include "bitmaps/bitmap_group.h"

/* classes */

class c_bitmap_widget : public c_user_interface_widget
{
protected:
	s_button_widget_reference* m_tag_block;
	int32 delay_milliseconds;
	int32 last_frame_milliseconds;
	uint32 field_7C;
	uint32 field_80;
	real_vector2d m_render_scale;
	real32 field_8C;
	int16 m_bitmap_block_index;
	uint8 gap_92[2];
	bitmap_data* m_bitmap_data;

public:
	const real_vector2d* get_render_scale();
	void verify_and_update_bitmap_index(int16 index);
	void assign_new_bitmap_block(bitmap_data* block);
	void set_local_bitmap(int16 local_bitmap_block_index, int16 bitmap_block_index);
	void set_render_scale(const real_vector2d* scale);


	// c_bitmap_widget virtual functions

	virtual ~c_bitmap_widget();
	virtual void update() override;
	virtual void render_widget(rectangle2d* viewport_bounds) override;
	virtual int32 get_intro_delay() override;
	virtual void* get_unprojected_bounds(rectangle2d* unprojected_bounds) override;
	virtual c_user_interface_text* get_interface() override;
};
ASSERT_STRUCT_SIZE(c_bitmap_widget, 0x98);