#pragma once
#include "bitmap_group.h"

/* prototypes */

int16 bitmap_format_get_bits_per_pixel(e_bitmap_data_format format);

uint32* __cdecl bitmap_2d_address(const bitmap_data* bitmap, int16 x, int16 y, uint16 mipmap_index);

bitmap_data* __cdecl bitmap_2d_new(int16 width, int16 height, int16 mipmap_count, e_bitmap_data_format format, e_bitmap_data_flags flags);

void __cdecl bitmap_clear_data(bitmap_data* bitmap, int32 value);

void __cdecl bitmap_data_free(bitmap_data* bitmap);
