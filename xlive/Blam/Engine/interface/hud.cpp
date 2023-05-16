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


// Used to grab the default crosshair size before we modify it
typedef void(__cdecl* update_hud_elements_display_settings_t)(int new_hud_size, int new_safe_area);
update_hud_elements_display_settings_t p_update_hud_elements_display_settings;

void __cdecl update_hud_elements_display_settings_hook(int new_hud_size, int new_safe_area)
{
	p_update_hud_elements_display_settings(new_hud_size, new_safe_area);
	original_hud_scale = *Memory::GetAddress<float*>(0x46402C);
	original_crosshair_text_scale = *Memory::GetAddress<float*>(0x464028);

	set_hud_size(1.0f);
	set_crosshair_size(H2Config_crosshair_scale);
}

void set_hud_size(float size)
{
	*Memory::GetAddress<float*>(0x46402C, 0x0) = original_hud_scale * size *  (1 / k_hud_upscale_size);
}

void set_crosshair_and_text_size(float size)
{
	*Memory::GetAddress<float*>(0x464028, 0x0) = original_crosshair_text_scale * size * (1 / k_hud_upscale_size);
}

float* get_crosshair_and_text_size()
{
	return Memory::GetAddress<float*>(0x464028);
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