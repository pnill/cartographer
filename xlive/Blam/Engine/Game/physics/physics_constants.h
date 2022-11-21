#pragma once
#include "Blam/Engine/Game/math/real_math.h"

#pragma pack(push,1)
struct physics_constants
{
	float gravity;
	float dword4;
	float dword8;
	real_point3d zero_vector;
	unsigned long dword18;
	static physics_constants* get();
	static float get_default_gravity();
};
CHECK_STRUCT_SIZE(physics_constants, 0x1C);
#pragma pack(pop)
