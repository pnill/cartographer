#pragma once

#define MAXIMUM_DECALS_PER_STRUCTURE 6144

// max: MAXIMUM_DECALS_PER_STRUCTURE
struct structure_runtime_decal
{
	real_point3d position;
	uint16 decal_type;
	int8 yaw;
	int8 pitch;
};