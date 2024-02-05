#include "stdafx.h"
#include "hud.h"
#include "new_hud_definitions.h"


#include "Blam/Engine/game/game_globals.h"

#include "H2MOD/Modules/Shell/Config.h"
#include "H2MOD/Tags/MetaLoader/tag_loader.h"
#include "H2MOD/Utils/Utils.h"


#define k_redraw_map_name "ui_redraw"

static float original_primary_hud_scale;
static float original_secondary_hud_scale;

// Used to grab the default crosshair size before we modify it
typedef void(__cdecl* update_hud_elements_display_settings_t)(int new_hud_size, int new_safe_area);
update_hud_elements_display_settings_t p_update_hud_elements_display_settings;

void __cdecl update_hud_elements_display_settings_hook(int new_hud_size, int new_safe_area)
{
	p_update_hud_elements_display_settings(new_hud_size, new_safe_area);
	original_primary_hud_scale = *Memory::GetAddress<float*>(0x46402C);
	original_secondary_hud_scale = *Memory::GetAddress<float*>(0x464028);

	set_primary_hud_scale(1.0f);
	set_secondary_hud_scale(1.0f);
}

void set_primary_hud_scale(float scale)
{
	*get_primary_hud_scale() = original_primary_hud_scale * scale * k_primary_upscale_size;
}

void set_secondary_hud_scale(float scale)
{
	*get_secondary_hud_scale() = original_secondary_hud_scale * scale * k_primary_upscale_size;
}

float* get_ui_scale()
{
	return Memory::GetAddress<float*>(0xA3E424);
}

float* get_primary_hud_scale()
{
	return Memory::GetAddress<float*>(0x46402C);
}

float* get_secondary_hud_scale()
{
	return Memory::GetAddress<float*>(0x464028);
}

void set_crosshair_offset(float offset)
{
	if (!FloatIsNaN(offset))
	{
		scenario_get_game_globals()->player_control[0]->crosshair_location.y = offset;
	}
}

void hud_patches_on_map_load()
{
	if (Memory::IsDedicatedServer()) { return; }

	set_crosshair_offset(H2Config_crosshair_offset);
	set_primary_hud_scale(1.0f);
	set_secondary_hud_scale(1.0f);
}

void hud_apply_pre_winmain_patches()
{
	if (Memory::IsDedicatedServer()) { return; }

	p_update_hud_elements_display_settings = Memory::GetAddress<update_hud_elements_display_settings_t>(0x264A18);

	// Replace all calls to update_hud_elements_display_settings with our hook
	PatchCall(Memory::GetAddress(0x25E1FC), update_hud_elements_display_settings_hook);
	PatchCall(Memory::GetAddress(0x264058), update_hud_elements_display_settings_hook);
	PatchCall(Memory::GetAddress(0x26406F), update_hud_elements_display_settings_hook);
}