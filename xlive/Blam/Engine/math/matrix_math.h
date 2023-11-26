#pragma once
#include "real_math.h"

struct real_matrix3x3
{
	real_vector3d forward;
	real_vector3d left;
	real_vector3d up;
};

struct real_matrix4x3
{
	real32 scale;
	real_matrix3x3 vectors;
	real_point3d position;
};
CHECK_STRUCT_SIZE(real_matrix4x3, 52);

void __cdecl matrix4x3_rotation_to_quaternion(const real_matrix4x3* matrix, real_quaternion* out);

void matrix4x3_interpolate(const real_matrix4x3* previous, const real_matrix4x3* target, float fractional_ticks, real_matrix4x3* out_mat);
