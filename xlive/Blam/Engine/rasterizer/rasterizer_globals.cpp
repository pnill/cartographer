#include "stdafx.h"
#include "rasterizer_globals.h"

void __cdecl rasterizer_get_screen_bounds(rectangle2d* screen_bounds)
{
	INVOKE(0x264ACD, 0, rasterizer_get_screen_bounds, screen_bounds);
}

void __cdecl rasterizer_get_frame_bounds(rectangle2d* frame_bounds)
{
	INVOKE(0x264AE3, 0, rasterizer_get_frame_bounds, frame_bounds);
}
