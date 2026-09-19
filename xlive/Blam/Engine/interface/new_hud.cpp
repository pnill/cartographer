#include "stdafx.h"
#include "new_hud.h"

#include "game/game.h"
#include "game/players.h"
#include "interface/hud.h"
#include "main/main_screenshot.h"
#include "networking/logic/network_life_cycle.h"
#include "render/render.h"

#include "H2MOD/Modules/Input/KeyboardInput.h"
#include "H2MOD/Modules/Shell/Config.h"


/* structures */

struct s_new_hud_engine_globals
{
	s_new_hud_globals_player_info player_data[k_number_of_users];
	int32 field_200;
	int32 field_204;
	datum betraying_player_datum_index;
	int32 gap_20C;
	s_player_appearance default_profile_traits;
	int16 unk_220;
	bool show_hud;
	bool flag_20D;			// initialized to 1 but unused?
	bool flag_20E;			// initialized to 1 but unused?
	bool connected_to_live;
	int8 gap_222[2];
	real32 hud_opacity;
	real32 unk_22C;
	real32 unk_230;
};
ASSERT_STRUCT_SIZE(s_new_hud_engine_globals, 564);

/* prototypes */

static s_new_hud_engine_globals* get_new_hud_engine_globals(void);

bool __cdecl render_ingame_chat_check(void);

// Hook for ui_get_hud_elements for modifying the hud anchor for text
real_point2d* __cdecl ui_get_hud_element_position_hook(e_hud_anchor anchor, real_point2d* point);

/* globals */

bool g_should_draw_hud_override = true;

/* public code */

void new_hud_apply_patches(void)
{
	// Redirects the is_campaign call that the in-game chat renderer makes so we can show/hide it as we like.
	PatchCall(Memory::GetAddress(0x22667B), render_ingame_chat_check);
	PatchCall(Memory::GetAddress(0x226628), render_ingame_chat_check);

	// Hook ui_get_hud_elements for modifying the hud anchor for text
	PatchCall(Memory::GetAddress(0x22D25A), ui_get_hud_element_position_hook);
	return;
}

e_screen_split_type new_hud_get_screen_split_type(
	int32 render_user_index)
{
	e_screen_split_type result;

	switch (get_global_render_window_count())
	{
	case 2:
		result = _screen_split_type_half;
		break;
	case 3:
		result = render_user_index == 0 ? _screen_split_type_half : _screen_split_type_quarter;
		break;
	case 4:
		result = _screen_split_type_quarter;
		break;
	default:
		result = _screen_split_type_full;
		break;
	}

	return result;
}

void should_draw_hud_override_set(bool flag)
{
	g_should_draw_hud_override = flag;
	return;
}

s_new_hud_globals_player_info* __cdecl new_hud_engine_globals_get_player_data(int32 local_player_index)
{
	return INVOKE(0x2237ED, 0, new_hud_engine_globals_get_player_data, local_player_index);
}

void __cdecl new_hud_engine_globals_set_drawing_player_index(datum player_datum)
{
	INVOKE(0x224B53, 0, new_hud_engine_globals_set_drawing_player_index, player_datum);
	return;
}

s_hud_scripted_globals* get_hud_scripted_globals(void)
{
	return *Memory::GetAddress<s_hud_scripted_globals**>(0x9765CC, 0x99FBB4);
}

s_new_hud_temporary_user_state* get_new_hud_temporary_user_state(int32 local_user_index)
{
	return &Memory::GetAddress<s_new_hud_temporary_user_state*>(0x9766D0, 0)[local_user_index];
}

void __cdecl new_hud_widget_anchor_calculate_point(int32 anchor, real_point2d* out_point)
{
	INVOKE(0x223969, 0, new_hud_widget_anchor_calculate_point, anchor, out_point);
	return;
}

uint32 __cdecl new_hud_text_get_split_screen_font_type(e_hud_anchor anchor)
{
	return INVOKE(0x224030, 0, new_hud_text_get_split_screen_font_type, anchor);
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

static s_new_hud_engine_globals* get_new_hud_engine_globals(void)
{
	return *Memory::GetAddress<s_new_hud_engine_globals**>(0x9770F4, 0x99E93C);
}

bool __cdecl render_ingame_chat_check(void) 
{
	if (H2Config_hide_ingame_chat)
	{
		return true;
	}

	else if (!game_is_ui_shell() && network_life_cycle_get_state() == _life_cycle_state_in_game)
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
	s_render* render = render_get();

	real32 scale_factor = *get_secondary_hud_scale();

	//int32 render_view_player_index = *Memory::GetAddress<int32*>(0x4E6800);

	switch (anchor)
	{
	case _hud_anchor_weapon_hud:
		point->x = (real32)render->camera.window_bounds.left + safe_area;
		point->y = (real32)render->camera.window_bounds.top + (safe_area / scale_factor); // (100.f * scale_factor) - 100.f;
		break;
	default:
		INVOKE(0x223969, 0x0, ui_get_hud_element_position_hook, anchor, point);
		break;
	}
	return point;
}
