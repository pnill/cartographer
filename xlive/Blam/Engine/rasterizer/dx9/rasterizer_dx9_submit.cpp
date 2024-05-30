#include "stdafx.h"
#include "rasterizer_dx9_submit.h"

/* public code */

void __cdecl rasterizer_dx9_set_vertex_shader_permutation(int32 shader_index)
{
	INVOKE(0x26F3CE, 0x0, rasterizer_dx9_set_vertex_shader_permutation, shader_index);
	return;
}

void __cdecl rasterizer_dx9_draw_rect(real_rectangle2d* quad, real32 depth, pixel32 color)
{
	INVOKE(0x26BE0D, 0x0, rasterizer_dx9_draw_rect, quad, depth, color);
	return;
}

bool __cdecl rasterizer_dx9_submit_vertex_declaration(uint32 vertex_declaration_index)
{
	return INVOKE(0x26F45E, 0x0, rasterizer_dx9_submit_vertex_declaration, vertex_declaration_index);
}

void __cdecl rasterizer_dx9_submit_resolve(void)
{
	INVOKE(0x26F4F9, 0x0, rasterizer_dx9_submit_resolve);
	return;
}
