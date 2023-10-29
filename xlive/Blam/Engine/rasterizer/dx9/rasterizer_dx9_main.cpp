#include "stdafx.h"
#include "rasterizer_dx9_main.h"

#include "Blam/Engine/bitmaps/bitmap_group.h"

#include "Util/Hooks/Hook.h"

datum last_bitmap_tag_index = 0;

datum last_bitmap_tag_index_get(void)
{
    return last_bitmap_tag_index;
}

typedef void(__cdecl* sub_65F600_t)(int16, datum, int16, real32);
sub_65F600_t p_sub_65F600;
void __cdecl sub_65F600(int16 stage, datum bitmap_tag_index, int16 bitmap_data_index, real32 a4)
{
    last_bitmap_tag_index = bitmap_tag_index;
    p_sub_65F600(stage, bitmap_tag_index, bitmap_data_index, a4);
    return;
}

void rasterizer_dx9_main_apply_patches(void)
{
    DETOUR_ATTACH(p_sub_65F600, Memory::GetAddress<sub_65F600_t>(0x25F600, 0x0), sub_65F600);
    return;
}
