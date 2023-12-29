#pragma once
#include "Blam/Engine/math/color_math.h"
#include "Blam/Engine/math/integer_math.h"

void __cdecl draw_string_get_color(real_argb_color* color);
void __cdecl draw_string_set_color(real_argb_color const* color);
void __cdecl draw_string_set_shadow_color(real_argb_color const* color);
void __cdecl draw_string_set_font(int32 font_index);
void __cdecl draw_string_set_options(int32 style, int32 justification, uint32 flags, bool wrap_horizontally);
void __cdecl draw_string_set_font_and_options(int32 font, int32 style, int32 justification, uint32 flags, real_argb_color const* color, real_argb_color const* shadow_color, bool wrap_horizontally);
void __cdecl draw_string_reset();
void __cdecl draw_string_render(rectangle2d const* bounds, wchar_t const* string);
