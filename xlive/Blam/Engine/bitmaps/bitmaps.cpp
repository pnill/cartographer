#include "stdafx.h"
#include "bitmaps.h"

/* globals */

const int16 g_bits_per_pixel[k_bitmap_format_count] =
{
  8,
  8,
  8,
  16,
  4,
  4,
  16,
  0,
  16,
  16,
  32,
  32,
  1,
  0,
  4,
  8,
  8,
  8,
  8,
  128,
  96,
  48,
  16,
  16,
};

/* public code */

int16 bitmap_format_get_bits_per_pixel(e_bitmap_data_format format)
{
	return (format != NONE ? g_bits_per_pixel[format] : 0);
}

uint32* __cdecl bitmap_2d_address(const bitmap_data* bitmap, int16 x, int16 y, uint16 mipmap_index)
{
	return INVOKE(0x2742F4, 0x0, bitmap_2d_address, bitmap, x, y, mipmap_index);
}

bitmap_data* __cdecl bitmap_2d_new(int16 width, int16 height, int16 mipmap_count, e_bitmap_data_format format, e_bitmap_data_flags flags)
{
	return INVOKE(0x27548E, 0x0, bitmap_2d_new, width, height, mipmap_count, format, flags);
}

void __cdecl bitmap_clear_data(bitmap_data* bitmap, int32 value)
{
	INVOKE(0x2869AB, 0x0, bitmap_clear_data, bitmap, value);
	return;
}

void __cdecl bitmap_data_free(bitmap_data* bitmap)
{
	INVOKE(0x2742BF, 0x0, bitmap_data_free, bitmap);
	return;
}
