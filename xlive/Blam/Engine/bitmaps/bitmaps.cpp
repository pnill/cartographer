#include "stdafx.h"
#include "bitmaps.h"

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


int16 bitmap_format_get_bits_per_pixel(e_bitmap_data_format format)
{
	return (format != NONE ? g_bits_per_pixel[format] : 0);
}

uint8* __cdecl bitmap_get_base_address(bitmap_data* bitmap, int16 column, int16 row, uint16 mipmap_index)
{
	return INVOKE(0x2742F4, 0x0, bitmap_get_base_address, bitmap, column, row, mipmap_index);
}
