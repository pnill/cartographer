#pragma once

#include "real_math.h"

float magnitude3d(const real_vector3d* v1);
void add_vectors3d(const real_vector3d* v1, const real_vector3d* v2, real_vector3d* out);

float normalize3d(real_vector3d* v1);
void point_from_line3d(const real_point3d* p1, const real_vector3d* direction, float length, real_point3d* out);
float dot_product3d(const real_vector3d* v1, const real_vector3d* v2);
void vector_from_points3d(const real_point3d* p1, const real_point3d* p2, real_vector3d* out);
void scale_vector3d(const real_vector3d* v1, const float scale, real_vector3d* out);
bool limit3d(void* a1, float limit);
void points_interpolate(const real_vector3d* previous_point, const real_point3d* target_point, float fractional_tick, real_point3d* out);