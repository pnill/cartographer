#include "stdafx.h"
#include "bsp3d.h"

#include "Blam/Cache/DataTypes/TagBlock.h"
#include "H2MOD/Tags/TagInterface.h"

double BLAM_MATH_INL plane3d_distance_to_point(const real_plane3d* plane, const real_point3d* point)
{
    return ((((point->x * plane->normal.i) + (point->y * plane->normal.j)) + (point->z * plane->normal.k)) - plane->distance);
}

int __cdecl bsp3d_test_point(const s_scenario_structure_bsp_group_definition::s_collision_bsp_block* collision_bsp, int starting_index, const real_point3d* point)
{
    tag_block<s_scenario_structure_bsp_group_definition::s_collision_bsp_block::s_bsp_3d_nodes_block> node_block = collision_bsp->bsp_3d_nodes;
    tag_block<s_scenario_structure_bsp_group_definition::s_collision_bsp_block::s_planes_block> plane_block = collision_bsp->planes;
    do
    {
        const s_scenario_structure_bsp_group_definition::s_collision_bsp_block::s_bsp_3d_nodes_block* node = node_block[0x7FFFFF & starting_index];
        const real_plane3d plane = plane_block[node->plane]->plane;

        starting_index = node->front_child_lower;
        if (plane3d_distance_to_point(&plane, point) < 0.0f)
        {
            starting_index = node->back_child_lower;
        }
    } while ((starting_index & 0x800000) == 0);

    if (starting_index == -1)
    {
        return -1;
    }
    else
    {
        return starting_index & 0x7FFFFF;
    }
}