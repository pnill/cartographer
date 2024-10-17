#include "stdafx.h"
#include "new_hud.h"

#include "bitmaps/bitmap_group.h"
#include "camera/camera.h"
#include "game/game.h"
#include "interface/hud.h"
#include "interface/new_hud_definitions.h"
#include "main/main_screenshot.h"
#include "networking/logic/life_cycle_manager.h"


#include "H2MOD/Modules/Input/KeyboardInput.h"
#include "H2MOD/Modules/Shell/Config.h"

/* globals */

bool g_should_draw_hud_override = true;
std::vector<datum> crosshair_bitmap_datums;				// Store all the crosshair bitmap datums
std::vector<point2d> crosshair_original_bitmap_sizes;	// We use point2d struct to store the original resolutions (x as width and y as height)

/* prototypes */

bool __cdecl render_ingame_chat_check(void);

// Hook for ui_get_hud_elements for modifying the hud anchor for text
real_point2d* __cdecl ui_get_hud_element_position_hook(e_hud_anchor anchor, real_point2d* point);

// Stores the bitmap width and height in crosshair_original_bitmap_sizes for use when scaling the crosshair bitmaps
void initialize_crosshair_bitmap_data(void);

// Checks if the vector contains the datum provided so we don't have duplicate datums in the vector
bool crosshair_bitmap_vector_contains_datum(datum tag_index);

// Gets all bitmap tags that are referenced as a crosshair from the new_hud_definition tags
// Intended to grab all bitmaps referenced as crosshairs rather than just the original ones
void get_crosshair_bitmap_datums(void);

void initialize_crosshair_scale(bool game_mode_ui_shell);


/* public code */

void new_hud_apply_patches(void)
{
	KeyboardInput::RegisterHotkey(&H2Config_hotkeyIdToggleHideIngameChat,
		[]() {
			H2Config_hide_ingame_chat = !H2Config_hide_ingame_chat;
		}
	);

	// Redirects the is_campaign call that the in-game chat renderer makes so we can show/hide it as we like.
	PatchCall(Memory::GetAddress(0x22667B), render_ingame_chat_check);
	PatchCall(Memory::GetAddress(0x226628), render_ingame_chat_check);

	// Hook ui_get_hud_elements for modifying the hud anchor for text
	PatchCall(Memory::GetAddress(0x22D25A), ui_get_hud_element_position_hook);
	return;
}

void should_draw_hud_override_set(bool flag)
{
	g_should_draw_hud_override = flag;
	return;
}

s_new_hud_engine_globals* get_new_hud_engine_globals(void)
{
	return *Memory::GetAddress<s_new_hud_engine_globals**>(0x9770F4, 0x99E93C);
}

s_new_hud_globals_player_info* new_hud_engine_globals_get_player_data(uint32 local_player_index)
{
	return INVOKE(0x2237ED, 0, new_hud_engine_globals_get_player_data, local_player_index);
}

void new_hud_engine_globals_set_drawing_player_index(datum player_datum)
{
	INVOKE(0x224B53, 0, new_hud_engine_globals_set_drawing_player_index, player_datum);
}

s_hud_scripted_globals* get_hud_scripted_globals(void)
{
	return *Memory::GetAddress<s_hud_scripted_globals**>(0x9765CC, 0x99FBB4);
}

s_new_hud_temporary_user_state* get_new_hud_temporary_user_state(int32 local_user_index)
{
	return &Memory::GetAddress<s_new_hud_temporary_user_state*>(0x9766D0, 0)[local_user_index];
}

bool new_hud_dont_draw(void)
{
	s_screenshot_globals* globals = get_screenshot_globals();

	// Added check for get_new_hud_engine_globals()->show_hud since it will still render other parts of the hud if show_hud is set to false
	// This does not match legacy behaviour, however the text for picking up weapons is a part of the hud so I assume this is a mistake from bungie
	bool dont_draw_hud = !g_should_draw_hud_override;


	bool original_check = globals->taking_screenshot && globals->resolution_multiplier > 1;

	// If original check or show_hud check are true we don't draw the hud
	// Otherwise return false so we show the hud
	return (original_check || dont_draw_hud ? true : false);
}

/* private code */

bool __cdecl render_ingame_chat_check(void) 
{
	if (H2Config_hide_ingame_chat)
	{
		datum local_player_datum_index = player_index_from_user_index(0);
		if (local_player_datum_index != NONE && s_player::get(local_player_datum_index)->is_chatting == 2)
		{
			hotkeyFuncToggleHideIngameChat();
		}
		return true;
	}

	else if (!game_is_ui_shell() && get_game_life_cycle() == _life_cycle_in_game)
	{
		//Enable chat in engine mode and game state mp.
		return false;
	}
	else {
		//original test - if is campaign
		return true;
	}
}

real_point2d* __cdecl ui_get_hud_element_position_hook(e_hud_anchor anchor, real_point2d* point)
{
	real32 safe_area = *Memory::GetAddress<real32*>(0x9770F0);
	s_camera* global_camera = get_global_camera();

	real32 scale_factor = *get_secondary_hud_scale();

	int32 render_view_player_index = *Memory::GetAddress<int32*>(0x4E6800);

	switch (anchor)
	{
	case _hud_anchor_weapon_hud:
		point->x = (real32)global_camera->window_bounds.left + safe_area;
		point->y = (real32)global_camera->window_bounds.top + (safe_area / scale_factor); // (100.f * scale_factor) - 100.f;
		break;
	default:
		INVOKE(0x223969, 0x0, ui_get_hud_element_position_hook, anchor, point);
		break;
	}
	return point;
}
