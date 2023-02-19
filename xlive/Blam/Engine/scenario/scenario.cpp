#include "stdafx.h"
#include "scenario.h"

#include "Blam/Cache/TagGroups/scenario_structure_bsp_definition.hpp"
#include "Blam/Engine/physics/bsp3d.h"
#include "H2MOD/Tags/TagInterface.h"

void location_invalidate(s_location* object_location)
{
    short global_structure_bsp_index = *Memory::GetAddress<short*>(0x4119A4);
    object_location->leaf_index = -1;
    object_location->cluster = -1;
    object_location->bsp_index = global_structure_bsp_index;
}

// Figure this out later
void scenario_location_from_point(s_location* location, const real_point3d* point)
{
    typedef int(__cdecl* scenario_location_from_point_t)(s_location* bsp_point, real_point3d* point);
    scenario_location_from_point_t p_scenario_location_from_point = Memory::GetAddress<scenario_location_from_point_t>(0x281EE);

    /*
    typedef int(__cdecl* bsp3d_test_point_t)(const s_scenario_structure_bsp_group_definition::s_collision_bsp_block* collision_bsp, int starting_index, const real_point3d* point);
    auto p_bsp3d_test_point = Memory::GetAddress<bsp3d_test_point_t>(0x76EFF);

    short global_structure_bsp_index = *Memory::GetAddress<short*>(0x4119A4);
    const s_scenario_structure_bsp_group_definition::s_collision_bsp_block* global_collision_bsp = Memory::GetAddress<s_scenario_structure_bsp_group_definition::s_collision_bsp_block*>(0x479E60);
    const s_scenario_structure_bsp_group_definition* global_structure_bsp = Memory::GetAddress<s_scenario_structure_bsp_group_definition*>(0x479E6C);

    if (global_structure_bsp_index == -1)
    {
        location->leaf_index = -1;
        location->cluster = -1;
    }
    else
    {
        int leaf_index = bsp3d_test_point(global_collision_bsp, 0, point);
        location->leaf_index = leaf_index;
        if (leaf_index == -1)
        {
            location->cluster = -1;
        }
        else
        {
            tag_block<s_scenario_structure_bsp_group_definition::s_leaves_block> leaves_block = global_structure_bsp->leaves;
            location->cluster = leaves_block[leaf_index]->cluster;
        }
    }
    location->bsp_index = global_structure_bsp_index;*/
}

void __cdecl scenario_location_from_leaf(s_location* location, DWORD leaf_index)
{
    const short global_structure_bsp_index = *Memory::GetAddress<short*>(0x4119A4);
    const s_scenario_structure_bsp_group_definition* global_structure_bsp = Memory::GetAddress<s_scenario_structure_bsp_group_definition*>(0x479E6C);

    location->leaf_index = leaf_index;
    if (leaf_index == -1)
    {
        location->cluster = -1;
        location->bsp_index = global_structure_bsp_index;
    }
    else
    {
        tag_block<s_scenario_structure_bsp_group_definition::s_leaves_block> leaves_block = global_structure_bsp->leaves;
        location->cluster = leaves_block[leaf_index]->cluster;
        location->bsp_index = global_structure_bsp_index;
    }
}