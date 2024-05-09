#include "stdafx.h"
#include "rasterizer_globals.h"

/* public code */

s_game_globals_rasterizer_data* rasterizer_globals_get_data(void)
{
	return *Memory::GetAddress<s_game_globals_rasterizer_data**>(0xA3DA3C);
}

void __cdecl rasterizer_get_screen_bounds(rectangle2d* screen_bounds)
{
	INVOKE(0x264ACD, 0, rasterizer_get_screen_bounds, screen_bounds);
}

void __cdecl rasterizer_get_frame_bounds(rectangle2d* frame_bounds)
{
	INVOKE(0x264AE3, 0, rasterizer_get_frame_bounds, frame_bounds);
}
