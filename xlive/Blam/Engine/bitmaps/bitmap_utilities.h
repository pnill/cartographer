#pragma once
#include "bitmaps/bitmap_group.h"

/* prototypes */

bitmap_data* __cdecl bitmap_2d_height_map(
    bitmap_data* source_bitmap,
    real32 source_offset_x,
    real32 source_offset_y,
    real32 source_size_x,
    real32 source_size_y,
    int16 destination_size_x,
    int16 destination_size_y);

void __cdecl bitmap_copy(const bitmap_data* source_bitmap, bitmap_data* destination_bitmap);
