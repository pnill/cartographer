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

void draw_string_set_player_color(real_argb_color const* color)
{
	INVOKE(0x9884B, 0, draw_string_set_player_color, color);
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

bool __cdecl draw_string_set_string(wchar_t* string)
{
	return INVOKE(0x99D59, 0, draw_string_set_string, string);
}

void __cdecl draw_string_set_position(int16 x, int16 y)
{
	INVOKE(0x98833, 0, draw_string_set_position, x, y);
}

void __cdecl draw_string_calculate_bounds(rectangle2d* bounds, wchar_t* string, rectangle2d* out_draw_string_bounds, int32* out_unk_2,
	real32 scale)
{
	INVOKE(0x99D97, 0, draw_string_calculate_bounds, bounds, string, out_draw_string_bounds, out_unk_2, scale);
}

void draw_string_draw(rectangle2d* bounds, wchar_t* string, real32 scale)
{
	INVOKE(0x271b74, 0, draw_string_draw, bounds, string, scale);
}

void __cdecl draw_string_reset()
{
	INVOKE(0x98CDC, 0, draw_string_reset);
}
