#include "stdafx.h"
#include "main_render.h"

#include "main_screenshot.h"

#include "cutscene/cinematics.h"
#include "game/player_mapping.h"
#include "game/players.h"
#include "rasterizer/dx9/rasterizer_dx9_main.h"
#include "rasterizer/rasterizer_main.h"
#include "render/render_cartographer_ingame_ui.h"

#include "H2MOD/Modules/Shell/Config.h"

/* globals */

bool debug_render_freeze = false;
bool debug_render_horizontal_splitscreen = false;
bool debug_force_all_player_views_to_default_player = false;

// TODO: figure out why having this global instead of window_bound_get breaks bloom
//window_bound g_window_bounds[6];

/*  prototypes */

static window_bound* window_bound_get(void);

static void main_render_hook(void);

static void compute_window_bounds_to_usercall(
	window_bound* window,
	int32 single_view,
	int32 user_index,
	int32 window_bound_index,
	int32 window_count,
	e_display_split_type display_split_type,
	s_observer_result* observer_result);

/* public code */

void main_render_apply_patches(void)
{
	// this is replacing a nullsub
	PatchCall(Memory::GetAddress(0x19228E), main_render_hook);

	PatchCall(Memory::GetAddress(0x27009A), main_render_player_view);
	PatchCall(Memory::GetAddress(0x2700A5), main_render_previous_backbuffer);
	return;
}

void __cdecl main_render(void)
{
	INVOKE(0x27002A, 0x0, main_render);
	return;
}

void __cdecl main_render_player_view(void)
{
	int32 player_window_count;
	if (cinematic_in_progress())
	{
		player_window_count = 1;
	}
	else if (local_player_count() >= 1)
	{
		player_window_count = MIN(local_player_count(), MAXIMUM_PLAYER_WINDOWS);
	}
	else
	{
		player_window_count = 1;
	}

	const int32 window_count = player_window_count + 1;

	ASSERT(player_window_count <= MAXIMUM_PLAYER_WINDOWS);
	ASSERT(window_count <= MAXIMUM_RENDERED_WINDOWS);

	e_display_split_type display_split_type;
	if (player_window_count <= 1)
	{
		display_split_type = _display_split_type_none;
	}
	else
	{	
		// treating _display_split_type_none as automatic 
		// we also want to enforce horitzontal split if g_debug_render_horizontal_splitscreen is enabled
		if (H2Config_split_mode != _display_split_type_none) 
		{
			display_split_type = H2Config_split_mode;
		}
		else
		{
			display_split_type = rasterizer_get_display_type() == _display_type_4_by_3 ? _display_split_type_horizontal : _display_split_type_vertical;
		}

		display_split_type = debug_render_horizontal_splitscreen ? _display_split_type_horizontal : display_split_type;
	}

	window_bound* g_window_bounds = window_bound_get();

	int32 user_index = NONE;
	window_bound* player_windows = &g_window_bounds[1];
	for (int32 window_num = 0; window_num < player_window_count; ++window_num)
	{
		s_observer_result* observer_result = NULL;
		if (debug_force_all_player_views_to_default_player)
		{
			user_index = player_mapping_first_active_output_user();
		}
		// TODO: another debug global condition here:
		else
		{
			++user_index;
			while (user_index < k_number_of_users && !players_user_is_active(user_index))
			{
				++user_index;
			}

			if (user_index >= k_number_of_users)
			{
				user_index = NONE;
			}
		}
		
		if (user_index != NONE)
		{
			observer_result = observer_get_camera(user_index);
			ASSERT(observer_result != NULL);
		}

		compute_window_bounds_to_usercall(&player_windows[window_num], 0, user_index, window_num, player_window_count, display_split_type, observer_result);
	}

	window_bound* window = &g_window_bounds[window_count];
	window->window_bound_index = NONE;
	window->user_index = NONE;
	window->single_view = 1;
	rasterizer_get_screen_and_frame_bounds(
		&window->rasterizer_camera.viewport_bounds,
		&window->rasterizer_camera.window_bounds);
	render_camera_build(&window->rasterizer_camera, NULL, NULL, NULL);

	if (!debug_render_freeze || window->single_view)
	{
		window->render_camera = window->rasterizer_camera;
	}

	if (screenshot_render(&g_window_bounds[1]))
	{
		render_frame(3, window_count, player_window_count, display_split_type, &g_window_bounds[1]);
	}

	return;
}


void __cdecl main_render_previous_backbuffer(int32 a1, int32 a2)
{
	window_bound* g_window_bounds = window_bound_get();

	g_window_bounds[0].single_view = 1;
	g_window_bounds[0].window_bound_index = NONE;
	g_window_bounds[0].user_index = NONE;
	rasterizer_get_screen_and_frame_bounds(
		&g_window_bounds[0].rasterizer_camera.viewport_bounds,
		&g_window_bounds[0].rasterizer_camera.window_bounds);

	render_camera_build(&g_window_bounds[0].rasterizer_camera, NULL, NULL, NULL);
	
	if (!debug_render_freeze || g_window_bounds[0].single_view)
	{
		g_window_bounds[0].render_camera = g_window_bounds[0].rasterizer_camera;
	}

	render_nonplayer_frame(&g_window_bounds[0]);


	s_rasterizer_dx9_main_globals* rasterizer_dx9_globals = rasterizer_dx9_main_globals_get();
	IDirect3DSurface9* backbuffer;
	if (screenshot_in_progress())
	{
		backbuffer = rasterizer_dx9_globals->global_d3d_surface_screenshot;
		rasterizer_dx9_globals->global_d3d_surface_screenshot->AddRef();
	}
	else
	{
		rasterizer_dx9_globals->global_d3d_device->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backbuffer);
	}

	rasterizer_dx9_globals->global_d3d_device->StretchRect(
		rasterizer_dx9_globals->global_d3d_surface_render_primary,
		NULL,
		backbuffer,
		NULL,
		D3DTEXF_NONE);

	if (backbuffer)
	{
		backbuffer->Release();
	}

	return;
}

/* private code */

static window_bound* window_bound_get(void)
{
	return Memory::GetAddress<window_bound*>(0xA4B738);
}

static void main_render_hook(void)
{
	if (!cubemap_screenshot_in_progress())
	{
		render_cartographer_ingame_ui();
	}

	return;
}

static void compute_window_bounds_to_usercall(
	window_bound* window,
	int32 single_view,
	int32 user_index,
	int32 window_bound_index,
	int32 window_count,
	e_display_split_type display_split_type,
	s_observer_result* observer_result)
{
	void* compute_window_bounds = (void*)Memory::GetAddress(0x26FA84);
	__asm
	{
		push observer_result
		push display_split_type
		push window_count
		push window_bound_index
		push window
		mov ecx, user_index
		mov eax, single_view
		call compute_window_bounds
		add esp, 20
	}
	return;
}