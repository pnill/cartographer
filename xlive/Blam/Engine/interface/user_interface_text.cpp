#include "stdafx.h"
#include "user_interface_text.h"

#include "interface/hud.h"


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
