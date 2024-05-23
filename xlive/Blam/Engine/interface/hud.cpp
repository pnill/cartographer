#include "stdafx.h"
#include "hud.h"

#include "hud_draw.h"
#include "hud_messaging.h"
#include "hud_nav_points.h"
#include "new_hud.h"
#include "new_hud_definitions.h"
#include "new_hud_draw.h"

#include "camera/director.h"
#include "cutscene/cinematics.h"
#include "game/game.h"
#include "game/game_engine_util.h"
#include "game/game_globals.h"
#include "main/main_screenshot.h"
#include "render/render.h"

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
void __cdecl hud_play_unit_sounds(int32 user_index);
void hud_render_player_indicators(datum player_index);

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

void __cdecl hud_draw_screen(void)
{
	// INVOKE(0x2278AA, 0x0, draw_hud);

	int32 g_user_render_index = *global_user_render_index_get();
	if (g_user_render_index != NONE && !new_hud_dont_draw())
	{
		datum player_index = player_index_from_user_index(g_user_render_index);
		if (player_index != NONE && 
			sub_4701B6(player_index) &&
			!cubemap_screenshot_in_progress())
		{
			const s_hud_scripted_globals* g_hud_scripted_globals = get_hud_scripted_globals();
			const s_player* player = (s_player*)datum_get(s_player::get_data(), player_index);
			int32 perspective = director_get_perspective(g_user_render_index);

			if (g_hud_scripted_globals->field_0)
			{
				hud_play_unit_sounds(g_user_render_index);
				bool player_unit_valid = (perspective != 3 && perspective != 2 && player->unit_index != NONE ? true : false);
				if (player_unit_valid)
				{
					hud_render_player_indicators(player_index);
					hud_render_nav_points(g_user_render_index);
				}
			}
			if (g_hud_scripted_globals->show_hud_messages)
			{
				hud_messaging_update(g_user_render_index);
			}
		}
		draw_hud_layer();
	}

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

void __cdecl hud_play_unit_sounds(int32 user_index)
{
	INVOKE(0x220FC2, 0x0, hud_play_unit_sounds, user_index);
	return;
}

void hud_render_player_indicators(datum player_index)
{
	s_data_array* player_data_array = s_player::get_data();
	const s_player* player = (s_player*)datum_get(player_data_array, player_index);
	if ( game_is_campaign() && !cinematic_in_progress())
	{
		s_data_iterator<s_player> player_iterator(player_data_array);
		for (s_player* current_player = player_iterator.get_next_datum(); current_player; current_player = player_iterator.get_next_datum())
		{
			/* Original code still checks game engine even though the indicators are never used in multiplayer
			* 
			if (game_is_campaign())
				is_enemy = current_player->properties[0].team_index != player->properties[0].team_index;
			else
				is_enemy = game_engine_team_is_enemy((e_game_team)current_player->properties[0].team_index, (e_game_team)player->properties[0].team_index);
			*/

			bool is_enemy = player->properties[0].team_index != player->properties[0].team_index;
			datum current_index = player_iterator.get_current_datum_index();

			if (current_index != player_index && !is_enemy && current_player->unit_index != NONE)
			{
				hud_render_player_indicator(current_index);
			}
		}
	}
	return;
}