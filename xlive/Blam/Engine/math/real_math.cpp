#include "stdafx.h"
#include "real_math.h"

#include "matrix_math.h"

void __cdecl real_math_initialize(void)
{
	INVOKE(0x340D7, 0x27754, real_math_initialize);
	return;
}

void __cdecl real_math_reset_precision(void)
{
	INVOKE(0x32D88, 0x26327, real_math_reset_precision);
	return;
}

real32 distance_squared2d(const real_point2d* a, const real_point2d* b)
{
	return (a->x - b->x) * (a->x - b->x) + (a->y - b->y) * (a->y - b->y);
}

real32 distance2d(const real_point2d* a, const real_point2d* b)
{
	return square_root(distance_squared2d(a, b));
}

real_quaternion* quaternion_normalize(real_quaternion* q)
{
	real32 dot_product = dot_product4d_quaternion(q, q);
	if (dot_product <= 0.0f)
	{
		q->v.i = 0.0f;
		q->v.j = 0.0f;
		q->v.k = 0.0f;
		q->w = 1.0f;
	}
	else
	{
		real32 rsqrt = reciprocal_square_root(dot_product);
		q->v.i *= rsqrt;
		q->v.j *= rsqrt;
		q->v.k *= rsqrt;
		q->w *= rsqrt;
	}
	return q;
}

real_quaternion* fast_quaternion_interpolate_and_normalize(const real_quaternion* previous, const real_quaternion* current, real32 fractional_ticks, real_quaternion* quaternion)
{	
	real32 dot_product = dot_product4d_quaternion(current, previous);
	real32 v1 = (dot_product >= 0.f ? fractional_ticks : -fractional_ticks);
	real32 v2 = 1.0f - fractional_ticks;

	quaternion->v.i = (previous->v.i * v2) + (v1 * current->v.i);
	quaternion->v.j = (current->v.j * v1) + (v2 * previous->v.j);
	quaternion->v.k = (current->v.k * v1) + (v2 * previous->v.k);
	quaternion->w = (previous->w * v2) + (v1 * current->w);
	return quaternion_normalize(quaternion);
}

real32 normalize2d(real_vector2d* v)
{
	real32 result = square_root(dot_product2d(v, v));

	if (fabs(result) < k_real_math_epsilon)
	{
		result = 0.0f;
	}
	else
	{
		v->i *= (1.0f / result);
		v->j *= (1.0f / result);
	}
	return result;
}

real_vector2d* perpendicular2d(const real_vector2d* a, real_vector2d* result)
{
	result->i = -a->j;
	result->j = a->i;
	return result;
}

real32 normalize3d_with_default(real_vector3d* a, const real_vector3d* b)
{
	real32 magnitude = magnitude3d(a);
	if (fabs(magnitude) < k_real_math_epsilon)
	{
		magnitude = 0.0f;
		*a = *b;
	}
	else
	{
		scale_vector3d(a, 1.0f / magnitude, a);
	}
	return magnitude;
}

bool valid_real_vector3d_axes2(real_vector3d* forward, real_vector3d* up)
{
	bool result = false;
	if (valid_real_normal3d(forward) && valid_real_normal3d(up))
	{
		real32 product = dot_product3d(forward, up);
		if (valid_realcmp(product, 0.0f))
		{
			result = true;
		}
	}
	return result;
}

real32 distance_squared3d(const real_point3d* p1, const real_point3d* p2)
{
	real_vector3d t1;
	vector_from_points3d(p2, p1, &t1);
	return magnitude_squared3d(&t1);
}

real32 distance3d(const real_point3d* p1, const real_point3d* p2)
{
	real32 distance_squared = distance_squared3d(p1, p2);
	return square_root(distance_squared);
}

bool limit3d(real_vector3d* v, real32 limit)
{
	real32 dot_product = dot_product3d(v, v);

	if (dot_product <= pow(limit, 2.0f))
		return false;
	
	real32 scale = reciprocal_square_root(dot_product)* limit;
	scale_vector3d(v, scale, v);
	return true;
}

real_point3d* points_interpolate(const real_point3d* a, const real_point3d* b, real32 t, real_point3d* result)
{
	result->x = (a->x * (1.f - t)) + (b->x * t);
	result->y = (a->y * (1.f - t)) + (b->y * t);
	result->z = (a->z * (1.f - t)) + (b->z * t);
	return result;
}

real32 scale_interpolate(real32 previous_scale, real32 current_scale, real32 fractional_tick, real32* out_scale)
{
	*out_scale = previous_scale * (1.0f - fractional_tick) + (current_scale * fractional_tick);
	return *out_scale;
}

real_vector3d* __cdecl perpendicular3d(const real_vector3d* in, real_vector3d* out)
{
	return INVOKE(0x344c9, 0x27B46, perpendicular3d, in, out);
}

real_vector3d* __cdecl generate_up_vector3d(const real_vector3d* forward, real_vector3d* up)
{
	return INVOKE(0x346E2, 0x27D5F, generate_up_vector3d, forward, up);
}

void vectors3d_from_euler_angles3d(real_vector3d* forward, real_vector3d* up, const real_euler_angles3d* angles)
{
	ASSERT(forward);
	ASSERT(up);
	ASSERT(angles);

	real_point3d point;
	real_matrix4x3 matrix;
	matrix4x3_rotation_from_angles(&matrix, angles->yaw, angles->pitch, angles->roll);
	matrix4x3_to_point_and_vectors(&matrix, &point, forward, up);
	return;
}

void vector3d_from_euler_angles2d(real_vector3d* forward, const real_euler_angles2d* angles)
{
	ASSERT(forward);
	ASSERT(angles);

	const real32 horizontal_projection = cos(angles->pitch);

	forward->i = cos(angles->yaw) * horizontal_projection;
	forward->j = sin(angles->yaw) * horizontal_projection;
	forward->k = sin(angles->pitch);
}
