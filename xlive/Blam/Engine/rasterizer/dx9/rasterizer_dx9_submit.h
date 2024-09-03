#pragma once

#include "math/color_math.h"

/* public code */

void __cdecl rasterizer_dx9_set_vertex_shader_permutation(int32 shader_index);

void __cdecl rasterizer_dx9_set_vertex_shader_permutation(int32 shader_index, int32 a2, int32 max_nodes_per_vertex);

void __cdecl rasterizer_dx9_draw_rect(real_rectangle2d* quad, real32 depth, pixel32 color);

bool __cdecl rasterizer_dx9_submit_vertex_declaration(uint32 vertex_declaration_index);

void __cdecl rasterizer_dx9_submit_resolve(void);
