#pragma once
#include "math/color_math.h"

/* classes */

class c_user_interface_text
{
protected:
	//void* __vtable;
	int32 m_custom_font_type;
	real_rgb_color m_text_color;
	int16 m_text_is_pulsating;
	int16 field_16;
	datum field_18;
	int32 m_text_justification;
	real32 field_20;
	datum field_24;
	uint8 gap_28[16];
	int32 field_38;
	int16 field_3C;
	int16 text_length;
	int32 m_ui_start_time;

public:
	c_user_interface_text();

	const real_rgb_color* get_color();
	const int32 get_font();

	void set_font(int32 font_type);
	void set_pulsating(bool pulsating);
	void set_color(real_rgb_color* color);
	void set_color(const real_rgb_color* color);


	// c_user_interface_text virtual functions

	virtual ~c_user_interface_text();
	virtual void  initial_raw_string(const wchar_t* raw_text) = 0;
	virtual void  update_raw_string(const wchar_t* Source) = 0;
	virtual const wchar_t* get_raw_string() = 0;
};
ASSERT_STRUCT_SIZE(c_user_interface_text, 0x44);


/* public methods */

float get_ui_text_label_scale();
void set_ui_text_label_scale(float scale);

void user_interface_text_apply_hooks();