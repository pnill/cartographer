#pragma once
#include "Blam/Engine/physics/bsp3d.h"
#include "Blam/Engine/objects/object_placement.h"
#include "Blam/Math/real_math.h"

void __cdecl location_invalidate(s_location* object_location);
void __cdecl scenario_location_from_point(s_location* bsp_point, const real_point3d* point);
void __cdecl scenario_location_from_leaf(s_location* location, DWORD leaf_index);
