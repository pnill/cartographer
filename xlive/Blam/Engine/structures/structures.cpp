#include "stdafx.h"
#include "structures.h"

s_structure_globals* structure_globals_get(void)
{
	return Memory::GetAddress<s_structure_globals*>(0x4D12A1, 0x4F5159);
}

bool __cdecl structure_get_sky(int32 cluster_index, int32* visible_sky_index, real_rgb_color* clear_color, bool* clear_color_active)
{
    return INVOKE(0xB3822, 0x0, structure_get_sky, cluster_index, visible_sky_index, clear_color, clear_color_active);
}
