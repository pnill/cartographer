#include "stdafx.h"
#include "rasterizer_globals.h"

/* public code */

s_rasterizer_globals* rasterizer_globals_get(void)
{
	return Memory::GetAddress<s_rasterizer_globals*>(0xA3E400);
}

uint32 rasterizer_get_width(void)
{
	return rasterizer_globals_get()->resolution_x;
}

uint32 rasterizer_get_height(void)
{
	return rasterizer_globals_get()->resolution_y;
}

void rasterizer_get_screen_bounds(rectangle2d* screen_bounds)
{
	*screen_bounds = rasterizer_globals_get()->screen_bounds;
	return;
}

void rasterizer_get_frame_bounds(rectangle2d* frame_bounds)
{
	*frame_bounds = rasterizer_globals_get()->frame_bounds;
	return;
}

void rasterizer_get_screen_and_frame_bounds(rectangle2d* screen_bounds, rectangle2d* frame_bounds)
{
	rasterizer_get_screen_bounds(screen_bounds);
	rasterizer_get_frame_bounds(frame_bounds);
	return;
}

void rasterizer_get_z_planes(real32* z_near, real32* z_far)
{
	s_rasterizer_globals* rasterizer_globals = rasterizer_globals_get();

	ASSERT(z_near && z_far);

	*z_near = rasterizer_globals->clipping_parameters.z_near;
	*z_far = rasterizer_globals->clipping_parameters.z_far;
	return;
}
