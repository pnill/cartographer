#include "stdafx.h"
#include "structures.h"

#include "H2MOD/Tags/TagInterface.h"

__int16 get_global_structure_bsp_index()
{
	return *Memory::GetAddress<__int16*>(0x4119A4, 0x3B5290);
}

s_scenario_structure_bsp_group_definition* get_global_structure_bsp()
{
	return *Memory::GetAddress<s_scenario_structure_bsp_group_definition**>(0x479E6C);
}

void structure_cluster_marker_begin()
{
    s_structure_globals* structure_globals = s_structure_globals::get();
    structure_globals->cluster_marker_initialized = true;
    ++structure_globals->cluster_index;
}

void structure_cluster_marker_end()
{
    s_structure_globals::get()->cluster_marker_initialized = false;
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
                intersected_indices[result++] = (short)i;
            }
            ++*out_actual_cluster_count;
        }
    }
    return result;
}

__int16 structure_clusters_in_sphere_recursive(
    const short cluster_reference,
    const real_point3d* position,
    const float radius,
    const short cluster_count,
    short* intersected_indices)
{
    typedef bool(__cdecl* sphere_intersects_cluster_portal_t)(
        const s_scenario_structure_bsp_group_definition* sbsp,
        const short cluster_portal_index,
        const real_point3d* position,
        const float radius);
    auto p_sphere_intersects_cluster_portal = Memory::GetAddress<sphere_intersects_cluster_portal_t>(0xB2954);

    const s_scenario_structure_bsp_group_definition* sbsp = get_global_structure_bsp();
    s_structure_globals* structure_globals = s_structure_globals::get();

    const s_scenario_structure_bsp_group_definition::s_clusters_block* cluster_definition;
    if (sbsp->clusters.data != DATUM_INDEX_NONE)
    {
        //clusters_block = sbsp->clusters;
        cluster_definition = sbsp->clusters[cluster_reference];
    }

    __int16* next_indicie = intersected_indices;
    short cluster_count_minus_one = cluster_count - 1;

    if (cluster_count > 0)
    {
        *intersected_indices = cluster_reference;
        next_indicie = intersected_indices + 1;
    }
    if (structure_globals->cluster_instances[cluster_reference] != structure_globals->cluster_index)
    {
        structure_globals->cluster_instances[cluster_reference] = structure_globals->cluster_index;
    }

    // If there are no portals then there's 1 cluster anyways
    if (cluster_definition->portals.size <= 0) { return 1; }

    __int16 clusters_in_sphere = 1;
    for (size_t i = 0; i < cluster_definition->portals.size; i++)
    {
        __int16 cluster_index;
        __int16 portal_index = 0;

        if (cluster_definition->portals.data != DATUM_INDEX_NONE)
        {
            portal_index = cluster_definition->portals[i]->portal_index;
        }

        s_scenario_structure_bsp_group_definition::s_cluster_portals_block* cluster_portal = nullptr;
        if (sbsp->cluster_portals.data != DATUM_INDEX_NONE)
        {
            cluster_portal = sbsp->cluster_portals[portal_index];
        }

        if (cluster_portal->back_cluster == cluster_reference)
        {
            cluster_index = cluster_portal->front_cluster;
        }
        else
        {
            cluster_index = cluster_portal->back_cluster;
        }

        if (structure_globals->cluster_instances[cluster_index] != structure_globals->cluster_index && p_sphere_intersects_cluster_portal(sbsp, portal_index, position, radius))
        {
            clusters_in_sphere = structure_clusters_in_sphere_recursive(cluster_index, position, radius, cluster_count_minus_one, next_indicie);
            clusters_in_sphere += clusters_in_sphere;
            cluster_count_minus_one -= clusters_in_sphere;
            next_indicie += clusters_in_sphere;
        }
    }
    return clusters_in_sphere;
}

short structure_clusters_in_sphere(__int16 cluster_reference, 
    const real_point3d* position, 
    const float radius, 
    const short cluster_count, 
    short* intersected_indices, 
    short* intersected_references)
{
    __int16 clusters_in_sphere;

    clusters_in_sphere = 0;
    if (cluster_reference != -1)
    {
        clusters_in_sphere = 1;
        if (radius <= 0.0)
        {
            if (cluster_count > 0)
            {
                *intersected_indices = cluster_reference;
            }
        }
        else
        {
            structure_cluster_marker_begin();
            clusters_in_sphere = structure_clusters_in_sphere_recursive(cluster_reference, position, radius, cluster_count, intersected_indices);
            structure_cluster_marker_end();
        }
    }
    if (intersected_references)
    {
        *intersected_references = clusters_in_sphere;
    }

    if (clusters_in_sphere > cluster_count) { return cluster_count; }
    return clusters_in_sphere;
}