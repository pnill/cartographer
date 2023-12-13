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

void __cdecl matrix3x3_from_forward_and_up(real_matrix3x3* out_matrix, real_vector3d* forward_vector, real_vector3d* up_vector);

void __cdecl matrix3x3_multiply(real_matrix3x3* input_matrix_1, real_matrix3x3* input_matrix_2, real_matrix3x3* out_matrix);

void __cdecl matrix3x3_transform_vector(real_matrix3x3* input_matrix, real_vector3d* input_vector, real_vector3d* out_vector);