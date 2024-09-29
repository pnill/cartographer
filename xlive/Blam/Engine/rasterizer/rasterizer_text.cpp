#include "stdafx.h"
#include "rasterizer_text.h"

/* public code */

void __cdecl rasterizer_draw_unicode_string(rectangle2d const* bounds, wchar_t const* string)
{
	INVOKE(0x271BC8, 0, rasterizer_draw_unicode_string, bounds, string);
}

bool __cdecl rasterizer_text_cache_initialize(void)
{
	return INVOKE(0x271254, 0x0, rasterizer_text_cache_initialize);
}
