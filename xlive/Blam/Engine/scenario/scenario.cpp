#include "stdafx.h"
#include "scenario.h"

scenario* get_global_scenario(void) 
{
	return *Memory::GetAddress<scenario**>(0x479E74, 0x4A6430);
}

s_scenario_structure_bsp_group_definition::s_collision_bsp_block* global_collision_bsp_get(void)
{
    return *Memory::GetAddress<s_scenario_structure_bsp_group_definition::s_collision_bsp_block**>(0x479E64, 0x4A6420);
}

uint32 scenario_netgame_equipment_size(void)
{
	return get_global_scenario()->netgame_equipment.count;
}

void location_invalidate(s_location* object_location)
{
    object_location->leaf_index = NONE;
    object_location->cluster_index = NONE;
    object_location->bsp_index = get_global_structure_bsp_index();
    return;
}

void __cdecl scenario_location_from_point(s_location* location, real_point3d* point)
{
    INVOKE(0x281EE, 0x30CB1, scenario_location_from_point, location, point);
    return;
}

bool __cdecl scenario_location_underwater(s_location* location, real_point3d* point, int16* global_material_index)
{
    return INVOKE(0x27A03, 0x304C6, scenario_location_underwater, location, point, global_material_index);
}

void __cdecl scenario_location_from_leaf(s_location* location, int32 leaf_index)
{
    INVOKE(0x2819D, 0x30C60, scenario_location_from_leaf, location, leaf_index);
    return;
}