#include "stdafx.h"
#include "collisions.h"

uint16 collision_compute_object_cull_flags(datum object_index)
{
	return INVOKE(0xCBE87, 0x8C1A7, collision_compute_object_cull_flags, object_index);
}

bool __cdecl collision_test_line(e_collision_test_flags flags,
    const real_point3d* placement,
    const real_point3d* position,
    datum first_ignore_object_index,
    datum second_ignore_object_index, 
    collision_result* collision)
{
    return INVOKE(0xCAC08, 0x8AF26, collision_test_line, flags, placement, position, first_ignore_object_index, second_ignore_object_index, collision);
}

bool __cdecl collision_test_vector(e_collision_test_flags flags,
    const real_point3d* placement,
    const real_vector3d* direction,
    datum first_ignore_object_index,
    datum second_ignore_object_index,
    collision_result* collision)
{
    return INVOKE(0xCA3ED, 0x8A70B, collision_test_vector, flags, placement, direction, first_ignore_object_index, second_ignore_object_index, collision);
}
