#include "stdafx.h"
#include "structures.h"

#include "H2MOD/Tags/TagInterface.h"
#include "Blam/Math/real_vector3d.h"
#include "Blam/Engine/math/real_math.h"
#include "Blam/Engine/math/convex_hulls.h"

__int16 get_global_structure_bsp_index()
{
	return *Memory::GetAddress<__int16*>(0x4119A4, 0x3B5290);
}

s_scenario_structure_bsp_group_definition* get_global_structure_bsp()
{
	return *Memory::GetAddress<s_scenario_structure_bsp_group_definition**>(0x479E6C);
}

s_scenario_structure_bsp_group_definition::s_collision_bsp_block* get_global_collision_bsp()
{
    return *Memory::GetAddress<s_scenario_structure_bsp_group_definition::s_collision_bsp_block**>(0x479E60);
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
    int cluster_size = get_global_structure_bsp()->clusters.size;

    for (int i = 0; i < cluster_size; ++i)
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

// TODO fix code so it isn't dereferencing a null pointer
bool sphere_intersects_cluster_portal(const s_scenario_structure_bsp_group_definition* sbsp, const short cluster_portal_index, const real_point3d* position, const float radius)
{
    const s_scenario_structure_bsp_group_definition::s_cluster_portals_block* cluster_portal = nullptr;
    if (sbsp->cluster_portals.data != DATUM_INDEX_NONE)
    {
        cluster_portal = sbsp->cluster_portals[cluster_portal_index];
    }

    const s_scenario_structure_bsp_group_definition::s_collision_bsp_block* collision_bsp = nullptr;
    if (sbsp->collision_bsp.data != DATUM_INDEX_NONE)
    {
        collision_bsp = sbsp->collision_bsp[0];                         // there can only be 1 collision bsp per bsp so we just grab 0th index
    }

    const real_plane3d* plane = nullptr;
    if (collision_bsp->planes.data != DATUM_INDEX_NONE)
    {
        plane = &collision_bsp->planes[cluster_portal->plane_index]->plane;
    }

    float distance = plane3d_distance_to_point(plane, position);
    if (float bounding_radius = cluster_portal->bounding_radius + radius;
        radius <= fabs(distance) || (bounding_radius * bounding_radius) <= distance_squared3d(position , &cluster_portal->centroid))
    {
        return false;
    }

    collision_bsp = get_global_collision_bsp();
    if (collision_bsp->planes.data != DATUM_INDEX_NONE)
    {
        plane = &collision_bsp->planes[cluster_portal->plane_index]->plane;
    }

    short projection = projection_from_vector3d(&plane->normal);
    bool unk_bool = projection_sign_from_vector3d(&plane->normal, projection);

    real_point3d p3d;
    real_point2d p2d;
    real_point2d p2d_array[128];

    point_from_line3d(position, &plane->normal, -distance, &p3d);
    project_point3d(&p3d, projection, unk_bool, &p2d);
    for (size_t i = 0; i < cluster_portal->vertices.size; ++i)
    {
        const real_point3d* point = nullptr;
        if (cluster_portal->vertices.data != DATUM_INDEX_NONE)
        {
            point = &cluster_portal->vertices[i]->point;
        }
        project_point3d(point, projection, unk_bool, &p2d_array[i]);
    }
    float new_radius = sqrt(radius * radius - distance * distance);
    return convex_hull2d_test_circle(cluster_portal->vertices.size, p2d_array, &p2d, new_radius);
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
    auto p_sphere_intersects_cluster_portal = Memory::GetAddress<sphere_intersects_cluster_portal_t>(0xB26E6);

    const s_scenario_structure_bsp_group_definition* sbsp = get_global_structure_bsp();
    s_structure_globals* structure_globals = s_structure_globals::get();

    const s_scenario_structure_bsp_group_definition::s_clusters_block* cluster_definition;
    if (sbsp->clusters.data != DATUM_INDEX_NONE)
    {
        cluster_definition = sbsp->clusters[cluster_reference];
    }

    short* next_indicie = intersected_indices;
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
            short result = structure_clusters_in_sphere_recursive(cluster_index, position, radius, cluster_count_minus_one, next_indicie);
            clusters_in_sphere += result;
            cluster_count_minus_one -= result;
            next_indicie += result;
        }
    }
    return clusters_in_sphere;
}

short structure_clusters_in_sphere(
    const short cluster_reference, 
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