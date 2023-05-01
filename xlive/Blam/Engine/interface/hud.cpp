#include "stdafx.h"
#include "hud.h"

#include "Blam/Cache/TagGroups/globals_definition.hpp"
#include "H2MOD/Modules/Shell/Config.h"
#include "H2MOD/Tags/TagInterface.h"
#include "H2MOD/Utils/Utils.h"
#include "Util/Hooks/Hook.h"

static float original_crosshair_text_scale;
static float original_hud_text_scale;

// This controls the scale for hud when we implement the redrawn hud
// ex. a value of 0.25 would allow us to use hud bitmaps that are 4x larger (resolution wise) than the old ones
static float hud_upscale_size = 1.0f;

// Used to grab the default crosshair size before we modify it
typedef void(__cdecl* update_hud_elements_display_settings_t)(int new_hud_size, int new_safe_area);
update_hud_elements_display_settings_t p_update_hud_elements_display_settings;

void __cdecl update_hud_elements_display_settings_hook(const int new_hud_size, const int new_safe_area)
{
	p_update_hud_elements_display_settings(new_hud_size, new_safe_area);
	original_hud_text_scale = *Memory::GetAddress<float*>(0x46402C);
	original_crosshair_text_scale = *Memory::GetAddress<float*>(0x464028);
	
	set_crosshair_size(H2Config_crosshair_scale);
}

void set_hud_size(const float size)
{
	WriteValue(Memory::GetAddress(0x46402C, 0x0), original_hud_text_scale * size * hud_upscale_size);
}

void set_crosshair_size(const float size)
{
	WriteValue(Memory::GetAddress(0x464028, 0x0), original_crosshair_text_scale * size * hud_upscale_size);
}

void set_crosshair_offset(const float offset)
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
}

void hud_apply_patches()
{
	if (Memory::IsDedicatedServer()) { return; }

	original_hud_text_scale = *Memory::GetAddress<float*>(0x46402C);
	original_crosshair_text_scale = *Memory::GetAddress<float*>(0x464028);
	DETOUR_ATTACH(p_update_hud_elements_display_settings, Memory::GetAddress<update_hud_elements_display_settings_t>(0x264A18, 0x0), update_hud_elements_display_settings_hook);
}