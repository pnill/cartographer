#include "stdafx.h"

#include "real_vector3d.h"

#if (!defined(_M_FP_FAST)) || !_M_FP_FAST
#pragma fenv_access (on)
#endif

 //weird function but matches game's code
long double square_root(float f)
{
	return sqrt((long double)f);
}

// double square_root(float f)
// {
//	  return square_root((double)f);
// }

float magnitude3d_squared(const real_vector3d* v1)
{
	return v1->i * v1->i + v1->j * v1->j + v1->k * v1->k;
}

float magnitude3d(const real_vector3d* v1)
{
	float magnitude_squared = magnitude3d_squared(v1);
	return square_root(magnitude_squared);
}

float dot_product3d(const real_vector3d* v1, const real_vector3d* v2)
{
	return v1->i * v2->i + v1->j * v2->j + v1->k * v2->k;
}

void add_vectors3d(const real_vector3d* v1, const real_vector3d* v2, real_vector3d* out)
{
	out->i = v1->i + v2->i;
	out->j = v1->j + v2->j;
	out->k = v1->k + v2->k;
}

void multiply_vectors3d(const real_vector3d* v1, const real_vector3d* v2, real_vector3d* out)
{
	out->i = v1->i * v2->i;
	out->j = v1->j * v2->j;
	out->k = v1->k * v2->k;
}

void subtract_vector3d(const real_vector3d* v1, const real_vector3d* v2, real_vector3d* out)
{
	out->i = v1->i - v2->i;
	out->j = v1->j - v2->j;
	out->k = v1->k - v2->k;
}

void scale_vector3d(const real_vector3d* v1, const float scale, real_vector3d* out)
{
	out->i = v1->i * scale;
	out->j = v1->j * scale;
	out->k = v1->k * scale;
}

void vector_from_points3d(const real_point3d* p1, const real_point3d* p2, real_vector3d* out)
{
	subtract_vector3d(p1, p2, out);
}

float distance_squared3d(const real_point3d* p1, const real_point3d* p2)
{
	real_vector3d t1;
	vector_from_points3d(p1, p2, &t1);
	return magnitude3d_squared(&t1);
}

float distance3d(const real_point3d* p1, const real_point3d* p2)
{
	float distance_squared = distance_squared3d(p1, p2);
	return square_root(distance_squared);
}

void point_from_line3d(const real_point3d* p1, const real_vector3d* direction, float length, real_point3d* out)
{
	real_vector3d t1;
	scale_vector3d(direction, length, &t1);
	add_vectors3d(p1, &t1, out);
}

float normalize3d(real_vector3d* v1)
{
	float length = magnitude3d(v1);

	if (abs(length) < k_real_math_epsilon)
		return 0.0f; // vector already normal

	scale_vector3d(v1, 1.0f / length, v1);

	return length;

	// return Memory::GetAddressRelative<float(__cdecl*)(real_vector3d*)>(0x429359, 0x4273B0)(v1);
}

bool limit3d(real_vector3d* v, float limit)
{
	float dot_product = dot_product3d(v, v);

	if (dot_product <= pow(limit, 2.0f))
		return false;

	*v = *v * ((1.0f / square_root(dot_product)) * limit);
	return true;
}

void points_interpolate(const real_vector3d* previous_point, const real_point3d* target_point, float fractional_tick, real_point3d* out)
{
	real_point3d tp1, tp2;
	scale_vector3d(previous_point, 1.0f - fractional_tick, &tp1);
	scale_vector3d(target_point, fractional_tick, &tp2);
	add_vectors3d(&tp1, &tp2, out);
}