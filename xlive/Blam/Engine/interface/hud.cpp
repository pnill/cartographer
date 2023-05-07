#include "stdafx.h"
#include "hud.h"

#include "Blam/Cache/TagGroups/globals_definition.hpp"

#include "H2MOD/Modules/Shell/Config.h"
#include "H2MOD/Tags/TagInterface.h"
#include "H2MOD/Utils/Utils.h"
#include "Util/Hooks/Hook.h"

static float original_crosshair_text_scale;
static float original_hud_scale;

// Seperated value for the scale of the crosshair
static float crosshair_size = 1.0f;

// This controls the scale for hud when we implement the redrawn hud
// ex. a value of 4.0 would allow us to use hud bitmaps that are 4x larger (resolution wise) than the old ones
#define k_hud_upscale_size 1.0f

// you'd think we could use 1200 since that's the games normal max resolution, but seems 1200 still hides some text :(
#define max_crosshair_text_scale 1080.0f * 0.0010416667f 
// >= 1.25 will make text disappear
#define max_ui_scale 1.049f 


// Used to grab the default crosshair size before we modify it
typedef void(__cdecl* update_hud_elements_display_settings_t)(int new_hud_size, int new_safe_area);
update_hud_elements_display_settings_t p_update_hud_elements_display_settings;

void __cdecl update_hud_elements_display_settings_hook(int new_hud_size, int new_safe_area)
{
	p_update_hud_elements_display_settings(new_hud_size, new_safe_area);
	original_hud_scale = *Memory::GetAddress<float*>(0x46402C);
	original_crosshair_text_scale = *Memory::GetAddress<float*>(0x464028);
	
	if (H2Config_hiresfix)
	{
		high_res_fix();
	}

	set_hud_size(1.0f);
	set_crosshair_size(H2Config_crosshair_scale);
}

void high_res_fix()
{
	if (original_crosshair_text_scale > max_crosshair_text_scale)
	{
		// textScale = resolution_height * 0.0010416667
		// but if it's too large the game won't render it some reason (max seems to be around 1.4, 1.3 when sword icon is in the text?)
		// lets just set it to the largest known good value for now, until the cause of large text sizes being hidden is figured out
		set_crosshair_and_text_size(max_crosshair_text_scale);
	}

	// UI_Scale was updated just before we were called, so lets fix it real quick...
	float* g_ui_scale = Memory::GetAddress<float*>(0xA3E424);
	if (*g_ui_scale > max_ui_scale)
	{
		// uiScale = resolution_height * 0.00083333335f
		// at higher resolutions text starts being cut off (pretty much any UI_Scale above 1 will result in text cut-off)
		// the render_text_wrapper_hook below fixes that by scaling the width of the element by UI_Scale (which the game doesn't do for some reason...)
		// however fonts will stop being rendered if the UI_Scale is too large (>= ~1.25)
		// so this'll make sure UI_Scale doesn't go above 1.249, but will result in the UI being drawn smaller
		*g_ui_scale = max_ui_scale;
	}
}

typedef int(__cdecl* render_text_wrapper_t)(rect2d* a1, rect2d* a2, int a3, int a4, int a5, float a6, int a7, int a8);
render_text_wrapper_t p_render_text_wrapper;

int __cdecl render_text_wrapper_hook(rect2d* a1, rect2d* a2, int a3, int a4, int a5, float a6, int a7, int a8)
{
	float g_ui_scale = *Memory::GetAddress<float*>(0xA3E424);
	if (a2 && a2 != a1) // if a2 == a1 then this is a chapter name which scaling seems to break, so skip those ones
	{
		short width = a2->right - a2->left;
		a2->right = a2->left + (short)((float)width * g_ui_scale); // scale width by UI_Scale, stops text elements from being cut-off when UI_Scale > 1
	}
	return p_render_text_wrapper(a1, a2, a3, a4, a5, a6, a7, a8);
}

void set_hud_size(float size)
{
	*Memory::GetAddress<float*>(0x46402C, 0x0) = original_hud_scale * size *  (1 / k_hud_upscale_size);
}

void set_crosshair_and_text_size(float size)
{
	*Memory::GetAddress<float*>(0x464028, 0x0) = original_crosshair_text_scale * size * (1 / k_hud_upscale_size);
}

void set_crosshair_size(float size)
{
	crosshair_size = size * original_crosshair_text_scale;
}

void set_crosshair_offset(float offset)
{
	if (!FloatIsNaN(offset))
	{
		s_globals_group_definition* globals = tags::get_matg_globals_ptr();
		globals->player_control[0]->crosshair_location.y = offset;
	}
}

void hud_patches_on_map_load()
{
	if (Memory::IsDedicatedServer()) { return; }

	set_crosshair_size(H2Config_crosshair_scale);
	set_crosshair_offset(H2Config_crosshair_offset);
	set_hud_size(1.0f);
}

void hud_apply_patches()
{
	if (Memory::IsDedicatedServer()) { return; }

	if (H2Config_hiresfix) 
	{
		// menu text fix for higher resolutions
		DETOUR_ATTACH(p_render_text_wrapper, Memory::GetAddress<render_text_wrapper_t>(0x271B02), render_text_wrapper_hook);
	}
}

void hud_apply_pre_winmain_patches()
{
	if (Memory::IsDedicatedServer()) { return; }

	p_update_hud_elements_display_settings = Memory::GetAddress<update_hud_elements_display_settings_t>(0x264A18, 0x0);

	// Replace all calls to update_hud_elements_display_settings with our hook
	PatchCall(Memory::GetAddress(0x25E1FC), update_hud_elements_display_settings_hook);
	PatchCall(Memory::GetAddress(0x264058), update_hud_elements_display_settings_hook);
	PatchCall(Memory::GetAddress(0x26406F), update_hud_elements_display_settings_hook);

	// Replace the crosshair and text scale global with our own just for the crosshair
	WritePointer(Memory::GetAddress(0x222F9F + 4), &crosshair_size);
}