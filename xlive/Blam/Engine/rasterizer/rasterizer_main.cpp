#include "stdafx.h"
#include "rasterizer_main.h"

#include "game/game.h"
#include "rasterizer/dx9/rasterizer_dx9_main.h"

/* prototypes */

// Used to limit framerate using our implementation
static void __cdecl rasterizer_present_hook(bitmap_data* bitmap);

/* public code */

void rasterizer_main_apply_patches(void)
{
	// present hooks for the frame limiter
	PatchCall(Memory::GetAddress(0x19073C), rasterizer_present_hook);
	rasterizer_dx9_main_apply_patches();
	return;
}

void rasterizer_sapien_apply_patches(void)
{
	rasterizer_dx9_sapien_apply_patches();
	return;
}

s_rasterizer_debug_options* rasterizer_debug_options_get(void)
{
	return Memory::GetAddress<s_rasterizer_debug_options*>(0x46816C);
}

void rasterizer_present_frame_screenshot_wrapper(bitmap_data* bitmap)
{
	rasterizer_dx9_present(bitmap, true);
	return;
}

e_display_type rasterizer_get_display_type(void)
{
	const bool ui_shell = game_is_active() && game_is_ui_shell();
	return ui_shell ? _display_type_4_by_3 : rasterizer_globals_get()->display_parameters.display_type;
}


uint32 rasterizer_get_adapter_count(void)
{
	return rasterizer_dx9_main_globals_get()->global_d3d_interface->GetAdapterCount();
}

bool rasterizer_is_initialized(void)
{
	return rasterizer_dx9_main_globals_get()->global_d3d_device != NULL;
}

bool rasterizer_initialize(void)
{
	return rasterizer_dx9_initialize();
}

void rasterizer_reset(bool create_window)
{
	rasterizer_dx9_reset(create_window);
	return;
}

bool rasterizer_initialize_screenshot_render_target(uint32 screen_width, uint32 screen_height)
{
	return rasterizer_dx9_initialize_screenshot_render_target(screen_width, screen_height);;
}

void rasterizer_cleanup_screenshot_render_target(void)
{
	rasterizer_dx9_cleanup_screenshot_render_target();
	return;
}

bool rasterizer_device_is_lost(void)
{
	return rasterizer_dx9_device_is_lost();
}

void rasterizer_main_render_pregame(void)
{
	rasterizer_dx9_main_render_pregame();
	return;
}

/* private code */

static void __cdecl rasterizer_present_hook(bitmap_data* bitmap)
{
	rasterizer_dx9_present(bitmap, false);
	return;
}

