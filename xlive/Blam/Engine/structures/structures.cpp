#include "stdafx.h"
#include "structures.h"

__int16 get_global_structure_bsp_index()
{
	return *Memory::GetAddress<__int16*>(0x4119A4, 0x3B5290);
}

s_scenario_structure_bsp_group_definition* get_global_structure_bsp()
{
	return *Memory::GetAddress<s_scenario_structure_bsp_group_definition**>(0x479E6C);
}

__int16 structure_clusters_in_bitvector(const s_game_cluster_bit_vectors* cluster_bitvector, short maximum_clusters, short* intersected_indices, short* out_actual_cluster_count)
{
    __int16 result = 0;
    *out_actual_cluster_count = 0;

    for (int i = 0; i < get_global_structure_bsp()->clusters.size; ++i)
    {
        if ( ((1 << (i & 31)) & cluster_bitvector[i >> 5].cluster_bitvector) != 0 )
        {
            if (result < maximum_clusters)
            {
                intersected_indices[result++] = i;
            }
            ++*out_actual_cluster_count;
        }
    }
    return result;
}