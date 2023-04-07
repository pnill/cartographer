#pragma once
#include "Blam/Math/real_math.h"

struct alignas(4) s_physics_constants
{
	float gravity;
	float dword4;
	float dword8;
	real_point3d zero_vector;
	DWORD dword18;
	static s_physics_constants* get();
	static float get_default_gravity();
};
