#include "stdafx.h"
#include "matrix_math.h"

/* public code */

real_matrix3x3* matrix3x3_from_forward_and_up(
	real_matrix3x3* matrix,
	real_vector3d const* forward,
	real_vector3d const* up)
{
	matrix->forward = *forward;
	cross_product3d(up, forward, &matrix->left);
	matrix->up = *up;
	return matrix;
}

real_matrix3x3* matrix3x3_rotation_from_quaternion(
	real_matrix3x3* __restrict matrix,
	real_quaternion const* __restrict quaternion)
{
	
	real32 n = quaternion->v.i * quaternion->v.i + quaternion->v.j * quaternion->v.j + quaternion->v.k * quaternion->v.k + quaternion->w * quaternion->w;
	real32 scale = (n > k_real_epsilon ? 2.0f / n : 0.0f);

	real32 xs = quaternion->v.i * scale;
	real32 ys = quaternion->v.j * scale;
	real32 zs = quaternion->v.k * scale;

	real32 wx = quaternion->w * xs;
	real32 wy = quaternion->w * ys;
	real32 wz = quaternion->w * zs;

	real32 xx = quaternion->v.i * xs;
	real32 xy = quaternion->v.i * ys;
	real32 xz = quaternion->v.i * zs;

	real32 yy = quaternion->v.j * ys;
	real32 yz = quaternion->v.j * zs;

	real32 zz = quaternion->v.k * zs;

	matrix->forward.i = 1.f - (zz + yy);
	matrix->left.i = xy - wz;
	matrix->up.i = xz + wy;

	matrix->forward.j = wz + xy;
	matrix->left.j = 1.f - (zz + xx);
	matrix->up.j = yz - wx;

	matrix->forward.k = xz - wy;
	matrix->left.k = wx + yz;
	matrix->up.k = 1.f - (xx + yy);
	
	return matrix;
}

real_quaternion* matrix3x3_rotation_to_quaternion(
	real_matrix3x3 const* __restrict matrix,
	real_quaternion* __restrict quaternion)
{
	real32 trace = matrix->forward.i + matrix->left.j + matrix->up.k;
	
	if (trace <= 0.f)
	{
		// Largest component
		int32 i = matrix->forward.i < matrix->left.j;
		
		if (matrix->v.n[4 * i] < matrix->up.k)
		{
			i = 2;
		}

		int32 j = (i + 1) % 3;
		int32 k = (i + 2) % 3;
		
		real32 s = matrix->v.n[4 * i] - matrix->v.n[4 * j] - matrix->v.n[4 * k];

		quaternion->v.n[i] = square_root(s + 1.0f) / 2.f;

		// Make sure value set is greater than epsilon
		ASSERT(quaternion->v.n[i] > k_real_epsilon);

		real32 scalar = 0.25f / quaternion->v.n[i];

		quaternion->w = (matrix->matrix[j][k] - matrix->matrix[k][j]) * scalar;
		quaternion->v.n[j] = (matrix->matrix[i][j] + matrix->matrix[j][i]) * scalar;
		quaternion->v.n[k] = (matrix->matrix[i][k] + matrix->matrix[k][i]) * scalar;
	}
	else
	{
		quaternion->w = square_root(trace + 1.f) / 2.f;
		
		// Make sure w is greater than epsilon
		ASSERT(quaternion->w > k_real_epsilon);

		real32 scalar = 0.25f / quaternion->w;
		quaternion->v.i = (matrix->left.k - matrix->up.j) * scalar;
		quaternion->v.j = (matrix->up.i - matrix->forward.k) * scalar;
		quaternion->v.k = (matrix->forward.j - matrix->left.i) * scalar;
	}

	if (quaternion->w < 0.0f)
	{
		quaternion->v.i = -quaternion->v.i;
		quaternion->v.j = -quaternion->v.j;
		quaternion->v.k = -quaternion->v.k;
		quaternion->w = -quaternion->w;
	}

	return quaternion;
}


void matrix4x3_from_orientation(
	real_matrix4x3* __restrict matrix,
	real_orientation const* __restrict orientation)
{
	matrix4x3_rotation_from_quaternion(matrix, &orientation->rotation);
	matrix->position = orientation->translation;
	matrix->scale = orientation->scale;
	return;
}

void matrix4x3_rotation_from_quaternion(
	real_matrix4x3* matrix, 
	real_quaternion const* quaternion)
{
	matrix3x3_rotation_from_quaternion(&matrix->vectors, quaternion);
	set_real_point3d(&matrix->position, 0.f, 0.f, 0.f);
	matrix->scale = 1.f;
	return;
}

void matrix4x3_rotation_to_quaternion(
	real_matrix4x3 const* matrix,
	real_quaternion* quaternion)
{
	matrix3x3_rotation_to_quaternion(&matrix->vectors, quaternion);
	return;
}

void matrix4x3_rotation_from_vectors(
	real_matrix4x3* matrix,
	real_vector3d const* forward,
	real_vector3d const* up)
{
	matrix->scale = 1.0f;
	matrix->vectors.forward = *forward;
	cross_product3d(up, forward, &matrix->vectors.left);
	matrix->vectors.up = *up;
	set_real_point3d(&matrix->position, 0.f, 0.f, 0.f);
	return;
}

void matrix4x3_from_point_and_vectors(
	real_matrix4x3* matrix,
	real_point3d const* point,
	real_vector3d const* forward, 
	real_vector3d const* up)
{
	matrix4x3_rotation_from_vectors(matrix, forward, up);
	matrix->position = *point;
	return;
}

void matrix4x3_inverse(
	real_matrix4x3 const* matrix,
	real_matrix4x3* result)
{
	if (matrix->scale!=0.f)
	{
		real32 swap;

		real32 x = -matrix->position.x;
		real32 y = -matrix->position.y;
		real32 z = -matrix->position.z;

		if (matrix->scale!=1.f)
		{
			result->scale = 1.f / matrix->scale;
			x *= result->scale;
			y *= result->scale;
			z *= result->scale;
		}
		else
		{
			result->scale = 1.f;
		}

		result->n[0][0] = matrix->n[0][0];
		result->n[1][1] = matrix->n[1][1];
		result->n[2][2] = matrix->n[2][2];

		swap = matrix->n[1][0];
		result->n[1][0] = matrix->n[0][1];
		result->n[0][1] = swap;

		swap = matrix->n[2][0];
		result->n[2][0] = matrix->n[0][2];
		result->n[0][2] = swap;

		swap = matrix->n[2][1];
		result->n[2][1] = matrix->n[1][2];
		result->n[1][2] = swap;

		result->n[3][0] = (x * result->n[0][0]) + y * result->n[1][0] + (z * result->n[2][0]);
		result->n[3][1] = (x * result->n[0][1]) + y * result->n[1][1] + (z * result->n[2][1]);
		result->n[3][2] = (x * result->n[0][2]) + y * result->n[1][2] + (z * result->n[2][2]);
	}
	else
	{
		csmemset(result, 0, sizeof(real_matrix4x3));
	}

	return;
}

real_vector3d* __cdecl matrix4x3_inverse_transform_normal(
	real_matrix4x3 const* matrix,
	real_vector3d const* normal,
	real_vector3d* result)
{
	return INVOKE(0x77D35, 0, matrix4x3_inverse_transform_normal, matrix, normal, result);
}

void __fastcall matrix4x3_multiply(
	real_matrix4x3 const* a, 
	real_matrix4x3 const* b,
	real_matrix4x3* result)
{
	INVOKE(0x78209, 0x75DFF, matrix4x3_multiply, a, b, result);
	return;
}

void matrix4x3_translation(
	real_matrix4x3* matrix,
	real_point3d const* point)
{
	matrix->scale = 1.f;
	matrix->vectors.forward = *global_forward3d;
	matrix->vectors.left = *global_left3d;
	matrix->vectors.up = *global_up3d;
	matrix->position = *point;
	return;
}

void matrix4x3_identity(
	real_matrix4x3* matrix)
{
	matrix->scale = 1.0f;
	matrix->vectors.forward = *global_forward3d;
	matrix->vectors.left = *global_left3d;
	matrix->vectors.up = *global_up3d;
	set_real_point3d(&matrix->position, 0.0f, 0.0f, 0.0f);
	return;
}

void matrix4x3_from_point_and_quaternion(
	real_matrix4x3* matrix,
	real_point3d const* point,
	real_quaternion const* quaternion)
{
	matrix3x3_rotation_from_quaternion(&matrix->vectors, quaternion);
	matrix->scale = 1.0f;
	matrix->position = *point;
	return;
}

void matrix4x3_to_point_and_vectors(
	real_matrix4x3 const* matrix,
	real_point3d* point,
	real_vector3d* forward,
	real_vector3d* up)
{
	*forward = matrix->vectors.forward;
	*up = matrix->vectors.up;
	*point = matrix->position;
	return;
}

real_point3d* matrix4x3_transform_point(
	real_matrix4x3 const* matrix,
	real_point3d const* point,
	real_point3d* result)
{
	real32 x;
	real32 y;
	real32 z;

	ASSERT(matrix && point && result);

	x = point->x;
	y = point->y;
	z = point->z;

	if (matrix->scale != 1.f)
	{
		x *= matrix->scale;
		y *= matrix->scale;
		z *= matrix->scale;
	}

	result->x = (((matrix->vectors.up.i * z) + (matrix->vectors.left.i * y)) + (matrix->vectors.forward.i * x)) + matrix->position.x;
	result->y = (((matrix->vectors.up.j * z) + (matrix->vectors.left.j * y)) + (matrix->vectors.forward.j * x)) + matrix->position.y;
	result->z = (((matrix->vectors.up.k * z) + (matrix->vectors.left.k * y)) + (matrix->vectors.forward.k * x)) + matrix->position.z;

	return result;
}

real_vector3d* matrix4x3_transform_vector(
	real_matrix4x3 const* matrix,
	real_vector3d const *vector,
	real_vector3d* result)
{
	real32 i = vector->i;
	real32 j = vector->j;
	real32 k = vector->k;

	if (matrix->scale != 1.f)
	{
		i *= matrix->scale;
		j *= matrix->scale;
		k *= matrix->scale;
	}

	result->i = (((matrix->vectors.up.i * k) + (matrix->vectors.left.i * j)) + (matrix->vectors.forward.i * i));
	result->j = (((matrix->vectors.up.j * k) + (matrix->vectors.left.j * j)) + (matrix->vectors.forward.j * i));
	result->k = (((matrix->vectors.up.k * k) + (matrix->vectors.left.k * j)) + (matrix->vectors.forward.k * i));

	return result;
}

void matrix4x3_rotation_from_angles(
	real_matrix4x3* matrix,
	real32 yaw,
	real32 pitch, 
	real32 roll)
{
	real32 a = cosine(roll);
	real32 b = sine(roll);
	real32 c = cosine(pitch);
	real32 d = sine(pitch);
	real32 e = cosine(yaw);
	real32 f = sine(yaw);
	
	real32 ad = a * d;
	real32 bd = b * d;

	matrix->scale = 1.f;
	matrix->vectors.forward.i = c * e;
	matrix->vectors.forward.j = a * f - e * bd;
	matrix->vectors.forward.k = e * ad + b * f;
	
	matrix->vectors.left.i = -c * f;
	matrix->vectors.left.j = f * bd + a * e;
	matrix->vectors.left.k = b * e - f * ad;
	
	matrix->vectors.up.i = -d;
	matrix->vectors.up.j = -b * c;
	matrix->vectors.up.k = c * a;

	matrix->position.x = 0.f;
	matrix->position.y = 0.f;
	matrix->position.z = 0.f;

	return;
}

void __cdecl matrix4x3_rotation_from_axis_and_angle(
	real_matrix4x3* matrix,
	real_vector3d const* axis, 
	real32 sine,
	real32 cosine)
{
	return INVOKE(0x775C1, 0x751B7, matrix4x3_rotation_from_axis_and_angle, matrix, axis, sine, cosine);
}

void matrix4x3_interpolate(
	real_matrix4x3 const* previous,
	real_matrix4x3 const* target,
	real32 fractional_ticks,
	real_matrix4x3* out_mat)
{
	real_quaternion q1_previous;
	real_quaternion q2_target;
	real_quaternion q3_interpolated;

	// first we convert matrix to quaternion rotation
	matrix4x3_rotation_to_quaternion(previous, &q1_previous);
	matrix4x3_rotation_to_quaternion(target, &q2_target);

	// interpolate the rotation quaternion
	fast_quaternion_interpolate_and_normalize(&q1_previous, &q2_target, fractional_ticks, &q3_interpolated);
	
	// and convert back to matrix format, since that's what the game uses
	matrix4x3_rotation_from_quaternion(out_mat, &q3_interpolated);
	
	// interpolate scale and the position of the node
	scalars_interpolate(previous->scale, target->scale, fractional_ticks, &out_mat->scale);
	points_interpolate(&previous->position, &target->position, fractional_ticks, &out_mat->position);
	
	return;
}

real_matrix3x3* __cdecl matrix3x3_multiply(
	real_matrix3x3 const* a,
	real_matrix3x3 const* b,
	real_matrix3x3* result)
{
	return INVOKE(0x78645, 0x7623B, matrix3x3_multiply, a, b, result);
}

real_vector3d* __cdecl matrix3x3_transform_vector(
	real_matrix3x3 const* matrix,
	real_vector3d const* vector,
	real_vector3d* result)
{
	return INVOKE(0x78800, 0x763F6, matrix3x3_transform_vector, matrix, vector, result);
}

real_matrix3x3* matrix3x3_from_angles(
	real_matrix3x3* matrix,
	real32 yaw,
	real32 pitch, 
	real32 roll)
{
	real32 a = cosine(roll);
	real32 b = sine(roll);
	real32 c = cosine(pitch);
	real32 d = sine(pitch);
	real32 e = cosine(yaw);
	real32 f = sine(yaw);

	real32 ae = a * e;
	real32 be = b * e;
	real32 af = a * f;
	real32 bf = b * f;

	matrix->forward.i = e * c;
	matrix->forward.j = f * c;
	matrix->forward.k = d;
	
	matrix->left.i = -(be * d) - af;
	matrix->left.j = ae - (bf * d);
	matrix->left.k = b * c;

	matrix->up.i = bf - (ae * d);
	matrix->up.j = (d * -af) - be;
	matrix->up.k = c * a;

	return matrix;
}

real_vector3d* matrix4x3_transform_normal(
	real_matrix4x3 const* matrix,
	real_vector3d const* normal,
	real_vector3d* result)
{
	real32 i = normal->i;
	real32 j = normal->j;
	real32 k = normal->k;
	
	result->i = ((i * matrix->vectors.forward.i) + (j * matrix->vectors.left.i)) + (k * matrix->vectors.up.i);
	result->j = ((i * matrix->vectors.forward.j) + (j * matrix->vectors.left.j)) + (k * matrix->vectors.up.j);
	result->k = ((i * matrix->vectors.forward.k) + (j * matrix->vectors.left.k)) + (k * matrix->vectors.up.k);

	return result;
}

real_plane3d* matrix4x3_transform_plane(
	real_matrix4x3 const* matrix,
	real_plane3d const* plane,
	real_plane3d* result)
{
	matrix4x3_transform_normal(matrix, &plane->n, &result->n);
	result->d = matrix->scale * plane->d + dot_product3d((real_vector3d*)&matrix->position, &result->n);
	return result;
}
