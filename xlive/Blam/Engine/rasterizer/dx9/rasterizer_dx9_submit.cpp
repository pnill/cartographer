#include "stdafx.h"
#include "rasterizer_dx9_submit.h"

/* public code */

void __cdecl rasterizer_dx9_gpu_frontend_initialize(void)
{
	INVOKE(0x26F29B, 0x0, rasterizer_dx9_submit_resolve);
	return;
}

void __cdecl rasterizer_dx9_set_vertex_shader_permutation(int32 shader_index)
{
	INVOKE_TYPE(0x26F3CE, 0x0, void(__cdecl*)(int32), shader_index);
	return;
}

void __cdecl rasterizer_dx9_set_vertex_shader_permutation(int32 shader_index, int32 a2, int32 max_nodes_per_vertex)
{
	INVOKE_TYPE(0x26F364, 0x0, void(__cdecl*)(int32, int32, int32), shader_index, a2, max_nodes_per_vertex);
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


