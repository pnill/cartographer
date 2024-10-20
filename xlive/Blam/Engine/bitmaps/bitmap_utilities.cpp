#include "stdafx.h"
#include "bitmap_utilities.h"

/* public code */

bitmap_data* __cdecl bitmap_2d_height_map(
    bitmap_data* source_bitmap,
    real32 source_offset_x,
    real32 source_offset_y,
    real32 source_size_x,
    real32 source_size_y,
    int16 destination_size_x,
    int16 destination_size_y)
{
    return INVOKE(0x286B0B, 0x0, bitmap_2d_height_map,
        source_bitmap,
        source_offset_x,
        source_offset_y,
        source_size_x,
        source_size_y,
        destination_size_x,
        destination_size_y);
}

void __cdecl bitmap_copy(const bitmap_data* source_bitmap, bitmap_data* destination_bitmap)
{
    INVOKE(0x2869D7, 0x0, bitmap_copy, source_bitmap, destination_bitmap);
    return;
}
