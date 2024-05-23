#include "stdafx.h"
#include "bitmap_group.h"

bitmap_data* __cdecl bitmap_group_get_bitmap_from_sequence(datum tag_index, int16 sequence_index, int16 sprite_index)
{
	return INVOKE(0x37D571, 0x0, bitmap_group_get_bitmap_from_sequence, tag_index, sequence_index, sprite_index);
}
