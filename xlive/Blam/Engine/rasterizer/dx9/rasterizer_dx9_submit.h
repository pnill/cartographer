#pragma once

#include "math/color_math.h"

/* public code */

void __cdecl rasterizer_dx9_set_shader_code(int32 shader_index);

void __cdecl rasterizer_dx9_draw_rect(real_rectangle2d* quad, real32 depth, pixel32 color);
