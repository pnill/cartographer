#include "stdafx.h"
#include "hud.h"

#include "new_hud.h"
#include "new_hud_definitions.h"

#include "game/game_globals.h"

#include "H2MOD/Modules/Shell/Config.h"
#include "H2MOD/Tags/MetaLoader/tag_loader.h"
#include "H2MOD/Utils/Utils.h"

/* constants */

#define k_redraw_map_name "ui_redraw"

/* globals */

real32 g_original_primary_hud_scale;
real32 g_original_secondary_hud_scale;

/* typedefs */

// Used to grab the default crosshair size before we modify it
typedef void(__cdecl* update_hud_elements_display_settings_t)(int32 new_hud_size, int32 new_safe_area);
update_hud_elements_display_settings_t p_update_hud_elements_display_settings;

/* prototypes */

void __cdecl update_hud_elements_display_settings_hook(int32 new_hud_size, int32 new_safe_area);

/* public code */

void hud_patches_on_map_load(void)
{
	set_crosshair_offset(H2Config_crosshair_offset);
	set_primary_hud_scale(1.f);
	set_secondary_hud_scale(1.f);
	return;
}

void hud_apply_pre_winmain_patches(void)
{
	p_update_hud_elements_display_settings = Memory::GetAddress<update_hud_elements_display_settings_t>(0x264A18);

	// Replace all calls to update_hud_elements_display_settings with our hook
	PatchCall(Memory::GetAddress(0x25E1FC), update_hud_elements_display_settings_hook);
	PatchCall(Memory::GetAddress(0x264058), update_hud_elements_display_settings_hook);
	PatchCall(Memory::GetAddress(0x26406F), update_hud_elements_display_settings_hook);
	return;
}

real32* get_ui_scale(void)
{
	return Memory::GetAddress<real32*>(0xA3E424);
}

real32* get_primary_hud_scale(void)
{
	return Memory::GetAddress<real32*>(0x46402C);
}

real32* get_secondary_hud_scale(void)
{
	return Memory::GetAddress<real32*>(0x464028);
}

void set_primary_hud_scale(real32 scale)
{
	*get_primary_hud_scale() = g_original_primary_hud_scale * scale * k_primary_upscale_size;
}

void set_secondary_hud_scale(real32 scale)
{
	*get_secondary_hud_scale() = g_original_secondary_hud_scale * scale * k_primary_upscale_size;
}

void set_crosshair_offset(real32 offset)
{
	if (!FloatIsNaN(offset))
	{
		scenario_get_game_globals()->player_control[0]->crosshair_location.y = offset;
	}
}

void __cdecl draw_hud(void)
{
	INVOKE(0x2278AA, 0x0, draw_hud);
	return;
}

/* private code */

void __cdecl update_hud_elements_display_settings_hook(int32 new_hud_size, int32 new_safe_area)
{
	p_update_hud_elements_display_settings(new_hud_size, new_safe_area);
	g_original_primary_hud_scale = *Memory::GetAddress<float*>(0x46402C);
	g_original_secondary_hud_scale = *Memory::GetAddress<float*>(0x464028);

	set_primary_hud_scale(1.f);
	set_secondary_hud_scale(1.f);
	return;
}
