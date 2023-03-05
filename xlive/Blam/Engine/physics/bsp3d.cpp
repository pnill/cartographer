#include "stdafx.h"
#include "bsp3d.h"

#include "Blam/Cache/DataTypes/TagBlock.h"
#include "Blam/Engine/math/real_math.h"
#include "H2MOD/Tags/TagInterface.h"

int bsp3d_test_point(const s_scenario_structure_bsp_group_definition::s_collision_bsp_block* collision_bsp, int starting_index, const real_point3d* point)
{
    do
    {
        const s_scenario_structure_bsp_group_definition::s_collision_bsp_block::s_bsp_3d_nodes_block* node = collision_bsp->bsp_3d_nodes[starting_index & 0x7FFFFF];
        const real_plane3d* plane = nullptr;

        if (collision_bsp->planes.data != DATUM_INDEX_NONE)
        {
            plane = &collision_bsp->planes[node->plane]->plane;
        }

        if (plane3d_distance_to_point(plane, point) < 0.0f)
        {
            starting_index = node->get_child_index(false);
        }
        else
        {
            starting_index = node->get_child_index(true);
        }
    } while ((starting_index & 0x800000) == 0);

    if (starting_index == -1) { return -1; }
    else
    {
        return starting_index & 0x7FFFFF;
    }
}