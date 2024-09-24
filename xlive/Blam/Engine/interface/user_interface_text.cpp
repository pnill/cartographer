#include "stdafx.h"
#include "user_interface_text.h"

#include "Blam/Engine/interface/hud.h"

c_user_interface_text::c_user_interface_text()
{
	this->m_custom_font_type = 0;
	this->m_text_is_pulsating = 0;
	this->field_16 = 1;
	this->field_18 = NONE;
	this->m_text_justification = 2;
	this->field_20 = 1.0;
	this->field_24 = NONE;
	this->field_38 = 0;
	this->field_3C = NONE;
	this->text_length = 0;
	this->m_ui_start_time = 0;
	this->m_text_color.blue = 1.0;
	this->m_text_color.green = 1.0;
	this->m_text_color.red = 1.0;
}

const real_rgb_color* c_user_interface_text::get_color()
{
	return &this->m_text_color;
}

const int32 c_user_interface_text::get_font()
{
	return this->m_custom_font_type;
}

void c_user_interface_text::set_font(int32 font_type)
{
	this->m_custom_font_type = font_type;
}

void c_user_interface_text::set_pulsating(bool pulsating)
{
	this->m_text_is_pulsating = pulsating;
}

void c_user_interface_text::set_color(real_rgb_color* color)
{
	this->m_text_color = *color;
}

void c_user_interface_text::set_color(const real_rgb_color* color)
{
	this->m_text_color = *color;
}

bool c_user_interface_text::is_private_use_character(wchar_t character)
{
	if (IN_RANGE(character, K_PRIVATE_USE_CHARACTER_SPACE_START, K_PRIVATE_USE_CHARACTER_SPACE_END))
		return true;
	return false;
}

// Seperated scale for the text labels (carto addition)
float ui_text_label_scaling = 0.0f;

float get_ui_text_label_scale()
{
	return ui_text_label_scaling;
}

void set_ui_text_label_scale(float scale)
{
	ui_text_label_scaling = scale;
}

bool __cdecl user_interface_parse_string(wchar_t* string, size_t max_length, char a3)
{
	return INVOKE(0x22F712, 0x0, user_interface_parse_string, string, max_length, a3);
}

// this seems to compute the required space to display the text?
void __cdecl ui_get_text_bounds_and_position_hook(int a1, wchar_t* string, int a3, int a4, float scale)
{
	typedef void(__cdecl* ui_get_text_size_t)(int, wchar_t*, int, int, float);
	auto p_ui_get_text_size = Memory::GetAddress<ui_get_text_size_t>(0x99D97);

	p_ui_get_text_size(a1, string, a3, a4, *get_secondary_hud_scale());
}

void user_interface_text_apply_hooks()
{
	// Replace the ui_scale_factor with our own scaling for text labels
	WritePointer(Memory::GetAddress(0x2305AC) + 4, &ui_text_label_scaling);
	WritePointer(Memory::GetAddress(0x23066A) + 4, &ui_text_label_scaling);

	PatchCall(Memory::GetAddress(0x22CFFD), ui_get_text_bounds_and_position_hook);
}
