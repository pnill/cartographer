#include "stdafx.h"
#include "collisions.h"

uint16 collision_compute_object_cull_flags(datum object_index)
{
	return INVOKE(0xCBE87, 0x8C1A7, collision_compute_object_cull_flags, object_index);
}
