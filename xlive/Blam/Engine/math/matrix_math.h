#pragma once
#include "real_math.h"

/* prototypes */

real_matrix3x3* matrix3x3_from_forward_and_up(real_matrix3x3* matrix, real_vector3d const* forward, real_vector3d const* up);

real_matrix3x3* matrix3x3_rotation_from_quaternion(real_matrix3x3* __restrict matrix, real_quaternion const* __restrict quaternion);

real_quaternion* matrix3x3_rotation_to_quaternion(real_matrix3x3 const* __restrict matrix, real_quaternion* __restrict quaternion);

void matrix4x3_from_orientation(real_matrix4x3* __restrict matrix, real_orientation const* __restrict orientation);

void matrix4x3_rotation_from_quaternion(real_matrix4x3* matrix, real_quaternion const* quaternion);

void matrix4x3_rotation_to_quaternion(real_matrix4x3 const* matrix, real_quaternion* quaternion);

void matrix4x3_rotation_from_vectors(real_matrix4x3* matrix, real_vector3d const* forward, real_vector3d const* up);

void matrix4x3_from_point_and_vectors(real_matrix4x3* matrix, real_point3d const* point, real_vector3d const* forward, real_vector3d const* up);

void matrix4x3_inverse(real_matrix4x3 const* matrix, real_matrix4x3* result);

real_vector3d* __cdecl matrix4x3_inverse_transform_normal(real_matrix4x3 const* matrix, real_vector3d const* normal, real_vector3d* result);

void __fastcall matrix4x3_multiply(real_matrix4x3 const* a, real_matrix4x3 const* b, real_matrix4x3* result);

void matrix4x3_translation(real_matrix4x3* matrix, real_point3d const* point);

void matrix4x3_identity(real_matrix4x3* matrix);

void matrix4x3_from_point_and_quaternion(real_matrix4x3* matrix, real_point3d const* point, real_quaternion const* quaternion);

void matrix4x3_to_point_and_vectors(real_matrix4x3 const* matrix, real_point3d* point, real_vector3d* forward, real_vector3d* up);

real_point3d* matrix4x3_transform_point(real_matrix4x3 const* matrix, real_point3d const* point, real_point3d* result);

real_vector3d* matrix4x3_transform_vector(real_matrix4x3 const* matrix, real_vector3d const* vector, real_vector3d* result);

void matrix4x3_rotation_from_angles(real_matrix4x3* matrix, real32 yaw, real32 pitch, real32 roll);

void __cdecl matrix4x3_rotation_from_axis_and_angle(real_matrix4x3* matrix, real_vector3d const* axis, real32 sine, real32 cosine);

void matrix4x3_interpolate(real_matrix4x3 const* previous, real_matrix4x3 const* target, real32 fractional_ticks, real_matrix4x3* out_mat);

real_matrix3x3* __cdecl matrix3x3_multiply(real_matrix3x3 const* a, real_matrix3x3 const* b, real_matrix3x3* result);

real_vector3d* __cdecl matrix3x3_transform_vector(real_matrix3x3 const* matrix, real_vector3d const* vector, real_vector3d* result);

real_matrix3x3* matrix3x3_from_angles(real_matrix3x3* matrix, real32 yaw, real32 pitch, real32 roll);

real_vector3d* matrix4x3_transform_normal(real_matrix4x3 const* matrix, real_vector3d const* normal, real_vector3d* result);

real_plane3d* matrix4x3_transform_plane(real_matrix4x3 const* matrix, real_plane3d const* plane, real_plane3d* result);
