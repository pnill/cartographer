#include "stdafx.h"
#include "draw_string.h"

void __cdecl draw_string_get_color(real_argb_color* color)
{
	INVOKE(0x98955, 0, draw_string_get_color, color);
}

void __cdecl draw_string_set_color(real_argb_color const* color)
{
	INVOKE(0x98A00, 0, draw_string_set_color, color);
}

void __cdecl draw_string_set_shadow_color(real_argb_color const* color)
{
	INVOKE(0x98A38, 0, draw_string_set_shadow_color, color);
}

void __cdecl draw_string_set_font(int32 font_index)
{
	INVOKE(0x98A87, 0, draw_string_set_font, font_index);
}

void __cdecl draw_string_set_format(int32 style, int32 justification, uint32 flags, bool wrap_horizontally)
{
	INVOKE(0x98A9D, 0, draw_string_set_format, style, justification, flags, wrap_horizontally);
}

void __cdecl draw_string_set_draw_mode(int32 font, int32 style, int32 justification, uint32 flags, real_argb_color const* color, real_argb_color const* shadow_color, bool wrap_horizontally)
{
	INVOKE(0x98AC4, 0, draw_string_set_draw_mode, font, style, justification, flags, color, shadow_color, wrap_horizontally);
}

void __cdecl draw_string_reset()
{
	INVOKE(0x98CDC, 0, draw_string_reset);
}
