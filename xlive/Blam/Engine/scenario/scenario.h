#pragma once
#include "Blam/Engine/physics/bsp3d.h"
#include "Blam/Engine/Objects/ObjectPlacementData.h"
#include "Blam/Math/real_math.h"

void __cdecl scenario_location_from_point(s_location* bsp_point, const real_point3d* point);
void __cdecl scenario_location_from_leaf(s_location* location, DWORD leaf_index);
