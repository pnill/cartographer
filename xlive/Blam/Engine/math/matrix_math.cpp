#include "stdafx.h"
#include "matrix_math.h"

void __cdecl matrix3x3_rotation_from_quaternion(real_matrix3x3* matrix, const real_quaternion* quaternion)
{
	INVOKE(0x776F3, 0x752E9, matrix3x3_rotation_from_quaternion, matrix, quaternion);
	return;
}

void __cdecl matrix4x3_from_orientation(real_matrix4x3* matrix, const real_orientation* orientation)
{
	INVOKE(0x778CB, 0x754C1, matrix4x3_from_orientation, matrix, orientation);
	return;
}

void __cdecl matrix4x3_rotation_from_quaternion(real_matrix4x3* matrix, const real_quaternion* quaternion)
{
	INVOKE(0x7784E, 0x75444, matrix4x3_rotation_from_quaternion, matrix, quaternion);
	return;
}

void __cdecl matrix4x3_rotation_to_quaternion(const real_matrix4x3* matrix, real_quaternion* out)
{
	INVOKE(0x791A2, 0x76CF3, matrix4x3_rotation_to_quaternion, matrix, out);
	return;
}

void __cdecl matrix4x3_from_point_and_vectors(real_matrix4x3* matrix, const real_point3d* position, const real_vector3d* forward, const real_vector3d* up)
{
	INVOKE(0x791B2, 0x76D03, matrix4x3_from_point_and_vectors, matrix, position, forward, up);
	return;
}

void __cdecl matrix4x3_inverse(const real_matrix4x3* input, real_matrix4x3* output)
{
	INVOKE(0x773DA, 0x74FD0, matrix4x3_inverse, input, output);
	return;
}

void __fastcall matrix4x3_multiply(const real_matrix4x3* matrix1, const real_matrix4x3* matrix2, real_matrix4x3* out)
{
	INVOKE(0x78209, 0x75DFF, matrix4x3_multiply, matrix1, matrix2, out);
	return;
}

void matrix4x3_translation(real_matrix4x3* matrix, const real_point3d* position)
{
	matrix->scale = 1.0f;
	matrix->vectors.forward.i = 1.0f;
	matrix->vectors.forward.j = 0.0f;
	matrix->vectors.forward.k = 0.0f;
	matrix->vectors.left.i = 0.0f;
	matrix->vectors.left.j = 1.0f;
	matrix->vectors.left.k = 0.0f;
	matrix->vectors.up.i = 0.0f;
	matrix->vectors.up.j = 0.0f;
	matrix->vectors.up.k = 1.0f;
	matrix->position = *position;
	return;
}

void matrix4x3_from_point_and_quaternion(real_matrix4x3* matrix, const real_point3d* point, const real_quaternion* quaternion)
{
	matrix3x3_rotation_from_quaternion(&matrix->vectors, quaternion);
	matrix->position.x = 0.0f;
	matrix->position.y = 0.0f;
	matrix->position.z = 0.0f;
	matrix->scale = 1.0f;
	matrix->position = *point;
	return;
}

void matrix4x3_interpolate(const real_matrix4x3* previous, const real_matrix4x3* target, real32 fractional_ticks, real_matrix4x3* out_mat)
{
	real_quaternion q1_previous, q2_target, q3_interpolated;
	// first we convert matrix to quaternion rotation
	matrix4x3_rotation_to_quaternion(previous, &q1_previous);
	matrix4x3_rotation_to_quaternion(target, &q2_target);
	// interpolate the rotation quaternion
	fast_quaternion_interpolate_and_normalize(&q1_previous, &q2_target, fractional_ticks, &q3_interpolated);
	// and convert back to matrix format, since that's what the game uses
	matrix4x3_rotation_from_quaternion(out_mat, &q3_interpolated);
	// interpolate scale and the position of the node
	scale_interpolate(previous->scale, target->scale, fractional_ticks, &out_mat->scale);
	points_interpolate(&previous->position, &target->position, fractional_ticks, &out_mat->position);
	return;
}
