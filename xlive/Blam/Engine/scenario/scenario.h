#pragma once
#include "scenario_definitions.h"
#include "Blam/Engine/objects/object_placement.h"

scenario* get_global_scenario(void);

s_scenario_structure_bsp_group_definition::s_collision_bsp_block* global_collision_bsp_get(void);

uint32 scenario_netgame_equipment_size(void);

void location_invalidate(s_location* object_location);

void __cdecl scenario_location_from_point(s_location* location, real_point3d* point);

void __cdecl scenario_location_from_leaf(s_location* location, int32 leaf_index);
