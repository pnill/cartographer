#pragma once
#include "real_math.h"

union real_matrix3x3
{
	struct { real_vector3d forward, left, up; };
	struct { real32 n[9]; } v;						// Array of values
	real32 matrix[3][3];							// Matrix of values
};
ASSERT_STRUCT_SIZE(real_matrix3x3, 36);

struct real_matrix4x3
{
	real32 scale;
	real_matrix3x3 vectors;
	real_point3d position;
};
ASSERT_STRUCT_SIZE(real_matrix4x3, 52);

const real_matrix4x3 global_identity4x3 = { 1.0f, { global_forward3d, global_left3d, global_up3d }, {0.f, 0.f, 0.f} };

real_matrix3x3* matrix3x3_from_forward_and_up(real_matrix3x3* matrix, const real_vector3d* forward, const real_vector3d* up);

real_matrix3x3* matrix3x3_rotation_from_quaternion(real_matrix3x3* matrix, const real_quaternion* quaternion);

real_quaternion* matrix3x3_rotation_to_quaternion(const real_matrix3x3* matrix, real_quaternion* quaternion);

void matrix4x3_from_orientation(real_matrix4x3* matrix, const real_orientation* orientation);

void matrix4x3_rotation_from_quaternion(real_matrix4x3* matrix, const real_quaternion* quaternion);

real_quaternion* matrix4x3_rotation_to_quaternion(const real_matrix4x3* matrix, real_quaternion* out);

void matrix4x3_from_point_and_vectors(real_matrix4x3* matrix, const real_point3d* position, const real_vector3d* forward, const real_vector3d* up);

void matrix4x3_inverse(const real_matrix4x3* input, real_matrix4x3* output);

void __fastcall matrix4x3_multiply(const real_matrix4x3* matrix1, const real_matrix4x3* matrix2, real_matrix4x3* out);

void matrix4x3_translation(real_matrix4x3* matrix, const real_point3d* position);

real_matrix4x3* matrix4x3_identity(real_matrix4x3* matrix);

void matrix4x3_from_point_and_quaternion(real_matrix4x3* matrix, const real_point3d* point, const real_quaternion* quaternion);

void matrix4x3_to_point_and_vectors(real_matrix4x3* matrix, real_point3d* position, real_vector3d* forward, real_vector3d* up);

real_point3d* matrix4x3_transform_point(const real_matrix4x3* matrix, const real_point3d* point, real_point3d* result);

real_vector3d* matrix4x3_transform_vector(const real_matrix4x3* matrix, const real_vector3d* vector, real_vector3d* result);

real_matrix4x3* matrix4x3_rotation_from_angles(real_matrix4x3* matrix, real32 i, real32 j, real32 k);

void __cdecl matrix4x3_rotation_from_axis_and_angle(real_matrix4x3* matrix, real_vector3d* vector, real32 axis, real32 angle);

void matrix4x3_interpolate(const real_matrix4x3* previous, const real_matrix4x3* target, real32 fractional_ticks, real_matrix4x3* out_mat);

void __cdecl matrix3x3_multiply(real_matrix3x3* input_matrix_1, real_matrix3x3* input_matrix_2, real_matrix3x3* out_matrix);

void __cdecl matrix3x3_transform_vector(const real_matrix3x3* input_matrix, real_vector3d* input_vector, real_vector3d* out_vector);

real_matrix3x3* matrix3x3_from_angles(real_matrix3x3* matrix, real32 i, real32 j, real32 k);
