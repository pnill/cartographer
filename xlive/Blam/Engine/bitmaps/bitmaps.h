#pragma once
#include "bitmap_group.h"

int16 bitmap_format_get_bits_per_pixel(e_bitmap_data_format format);

uint8* __cdecl bitmap_get_base_address(bitmap_data* bitmap, int16 column, int16 row, uint16 mipmap_index);
