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

void __cdecl matrix3x3_rotation_from_quaternion(real_matrix3x3* matrix, const real_quaternion* quaternion);

void __cdecl matrix4x3_rotation_to_quaternion(const real_matrix4x3* matrix, real_quaternion* out);

void __cdecl matrix4x3_from_point_and_vectors(real_matrix4x3* matrix, const real_point3d* position, const real_vector3d* forward, const real_vector3d* up);

void __cdecl matrix4x3_inverse(const real_matrix4x3* input, real_matrix4x3* output);

void __fastcall matrix4x3_multiply(const real_matrix4x3* matrix1, const real_matrix4x3* matrix2, real_matrix4x3* out);

void matrix4x3_translation(real_matrix4x3* matrix, const real_point3d* position);

void matrix4x3_from_point_and_quaternion(real_matrix4x3* matrix, const real_point3d* point, const real_quaternion* quaternion);

void matrix4x3_interpolate(const real_matrix4x3* previous, const real_matrix4x3* target, float fractional_ticks, real_matrix4x3* out_mat);
