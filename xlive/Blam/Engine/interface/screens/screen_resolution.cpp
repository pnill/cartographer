#include "stdafx.h"
#include "screen_resolution.h"
#include "rasterizer/rasterizer_settings.h"

void* __cdecl c_screen_resolution_menu::load(c_screen_parameters* parameters)
{
	return INVOKE(0x249592, 0x0, c_screen_resolution_menu::load, parameters);
}

void* __cdecl c_screen_resolution_menu::load_mp(c_screen_parameters* parameters)
{
	return INVOKE(0x258B78, 0x0, c_screen_resolution_menu::load_mp, parameters);
}

static int32 rasterizer_settings_get_display_mode_hook()
{
	// return rasterizer_settings_get()->display_mode;
	// the caller compares the result with 1 so we can patch this and make it also return 1 for borderless

	int32 result = rasterizer_settings_get()->display_mode;

	if (result == _rasterizer_window_mode_borderless)
	{
		result = _rasterizer_window_mode_windowed;
	}

	return result;
}

void __cdecl c_screen_resolution_menu::apply_instance_patches()
{
	// disable refresh rate selection menu in borderless mode
	PatchCall(Memory::GetAddress(0x25023A), rasterizer_settings_get_display_mode_hook);
}
