#include "stdafx.h"
#include "rasterizer_dx9_submit.h"

/* public code */

void __cdecl rasterizer_dx9_set_shader_code(int32 shader_index)
{
	INVOKE(0x26F3CE, 0x0, rasterizer_dx9_set_shader_code, shader_index);
	return;
}

void __cdecl rasterizer_dx9_draw_rect(real_rectangle2d* quad, real32 depth, pixel32 color)
{
	INVOKE(0x26BE0D, 0x0, rasterizer_dx9_draw_rect, quad, depth, color);
	return;
}