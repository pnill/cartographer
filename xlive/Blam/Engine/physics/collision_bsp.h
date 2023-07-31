#pragma once
#define MAXIMUM_EDGES_PER_COLLISION_BSP 262144
#define MAXIMUM_SURFACES_PER_COLLISION_BSP 131072

// max count: ((((MAXIMUM_SURFACES_PER_COLLISION_BSP)+(LONG_BITS-1))>>LONG_BITS_BITS)) = 4096
struct long_surface_flags
{
	int flags;
};
