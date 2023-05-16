#include "stdafx.h"
#include "rasterizer_settings.h"

#include "Blam/Engine/interface/user_interface_text.h"
#include "Util/Hooks/Hook.h"

typedef void(__cdecl* update_screen_settings_t)(int, int, short, short, short, short, float, float);
update_screen_settings_t p_update_screen_settings;

void __cdecl update_screen_settings(
	int width, 
	int height, 
	short left,
	short top,
	short a5, 
	short a6,
	float ui_scale,
	float window_scale)
{
	p_update_screen_settings(width, height, left, top, a5, a6, ui_scale, window_scale);

	// Change the text label scale if the global ui scale goes above 1 
	// (text fix for resolutions above 1080 pixels vertically)
	if (ui_scale > 1.0f)
		set_ui_text_label_scale(1.f / ui_scale);
	else
		set_ui_text_label_scale(ui_scale);
}

void rasterizer_settings_apply_hooks()
{
	DETOUR_ATTACH(p_update_screen_settings, Memory::GetAddress<update_screen_settings_t>(0x264979), update_screen_settings);
}