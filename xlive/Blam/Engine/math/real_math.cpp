#include "stdafx.h"
#include "real_math.h"

void __cdecl real_math_reset_precision(void)
{
	INVOKE(0x32D88, 0x26327, real_math_reset_precision);
	return;
}

void __cdecl fast_quaternion_interpolate_and_normalize(const real_quaternion* previous, const real_quaternion* current, real32 fractional_ticks, real_quaternion* out_quaternion)
{
	INVOKE(0x34F6F, 0x285EC, fast_quaternion_interpolate_and_normalize, previous, current, fractional_ticks, out_quaternion);
	return;
}

real32 square_root(real32 f)
{
	return sqrt(f);
}

real32 reciprocal_square_root(real32 f)
{
	return 1.0 / sqrt(f);
}

real32 absolute_value(real32 f)
{
	return abs(f);
}

real32 magnitude3d_squared(const real_vector3d* v1)
{
	return v1->i * v1->i + v1->j * v1->j + v1->k * v1->k;
}

real32 magnitude3d(const real_vector3d* v1)
{
	real32 magnitude_squared = magnitude3d_squared(v1);
	return square_root(magnitude_squared);
}

real32 dot_product3d(const real_vector3d* v1, const real_vector3d* v2)
{
	return v1->i * v2->i + v1->j * v2->j + v1->k * v2->k;
}

void add_vectors3d(const real_vector3d* v1, const real_vector3d* v2, real_vector3d* out)
{
	out->i = v1->i + v2->i;
	out->j = v1->j + v2->j;
	out->k = v1->k + v2->k;
	return;
}

void multiply_vectors3d(const real_vector3d* v1, const real_vector3d* v2, real_vector3d* out)
{
	out->i = v1->i * v2->i;
	out->j = v1->j * v2->j;
	out->k = v1->k * v2->k;
	return;
}

void subtract_vector3d(const real_vector3d* v1, const real_vector3d* v2, real_vector3d* out)
{
	out->i = v1->i - v2->i;
	out->j = v1->j - v2->j;
	out->k = v1->k - v2->k;
	return;
}

void scale_vector3d(const real_vector3d* v1, real32 scale, real_vector3d* out)
{
	out->i = v1->i * scale;
	out->j = v1->j * scale;
	out->k = v1->k * scale;
	return;
}

void vector_from_points3d(const real_point3d* p1, const real_point3d* p2, real_vector3d* out)
{
	subtract_vector3d(p1, p2, out);
	return;
}

real32 distance_squared3d(const real_point3d* p1, const real_point3d* p2)
{
	real_vector3d t1;
	vector_from_points3d(p1, p2, &t1);
	return magnitude3d_squared(&t1);
}

real32 distance3d(const real_point3d* p1, const real_point3d* p2)
{
	real32 distance_squared = distance_squared3d(p1, p2);
	return square_root(distance_squared);
}

void point_from_line3d(const real_point3d* p1, const real_vector3d* direction, real32 length, real_point3d* out)
{
	real_vector3d t1;
	scale_vector3d(direction, length, &t1);
	add_vectors3d(p1, &t1, out);
	return;
}

real32 normalize3d(real_vector3d* v1)
{
	real32 length = magnitude3d(v1);

	if (absolute_value(length) < k_real_math_epsilon)
		return 0.0f; // vector already normal

	scale_vector3d(v1, 1.0f / length, v1);

	return length;

	// return Memory::GetAddressRelative<real32(__cdecl*)(real_vector3d*)>(0x429359, 0x4273B0)(v1);
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

void points_interpolate(const real_vector3d* previous_point, const real_point3d* target_point, real32 fractional_tick, real_point3d* out)
{
	real_point3d tp1, tp2;
	scale_vector3d(previous_point, 1.0f - fractional_tick, &tp1);
	scale_vector3d(target_point, fractional_tick, &tp2);
	add_vectors3d(&tp1, &tp2, out);
	return;
}

void scale_interpolate(real32 previous_scale, real32 current_scale, real32 fractional_tick, real32* out_scale)
{
	*out_scale = previous_scale * (1.0f - fractional_tick) + (current_scale * fractional_tick);
}

