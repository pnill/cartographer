#pragma once
//use this for all base real math related structs and implementations
#include "math.h"

#define _USE_MATH_DEFINES
#include <math.h>

#define k_real_math_epsilon 0.000099999997f

#define DEGREES_TO_RADIANS(angle) ((real32)((angle) * M_PI / 180.0))

union real_point2d
{
	real32 v[2];
	struct { real32 x, y; };
};
CHECK_STRUCT_SIZE(real_point2d, sizeof(real32) * 2);

struct angle
{
	float rad = 0.0f;
};
CHECK_STRUCT_SIZE(angle, sizeof(float));

union real_euler_angles2d
{
	real32 v[2];
	struct { real32 yaw, pitch; };
};
CHECK_STRUCT_SIZE(real_euler_angles2d, sizeof(angle) * 2);

union real_euler_angles3d
{
	real32 v[3];
	struct { real32 yaw, pitch, roll; };
};
CHECK_STRUCT_SIZE(real_euler_angles3d, sizeof(angle) * 3);

union real_vector3d
{
	real32 v[3];
	struct { real32 i, j, k; };
	struct { real32 x, y, z; };
};
CHECK_STRUCT_SIZE(real_vector3d, sizeof(real32) * 3);

typedef real_vector3d real_point3d;

union real_vector2d
{
	real32 v[2];
	struct { real32 i, j; };
};
CHECK_STRUCT_SIZE(real_vector2d, sizeof(real32) * 2);

struct real_plane2d
{
	real_vector2d normal;
	real32 distance;
};
CHECK_STRUCT_SIZE(real_plane2d, sizeof(real_vector2d) + 4);

struct real_plane3d
{
	real_vector3d normal;
	real32 distance;
};
CHECK_STRUCT_SIZE(real_plane3d, sizeof(real_vector3d) + sizeof(real32));

union real_quaternion
{
	real32 v[4];
	struct { real32 i, j, k, w; };
};
CHECK_STRUCT_SIZE(real_quaternion, sizeof(real32) * 4);

union real_bounds
{
	real32 v[2];
	struct { real32 lower, upper; };
};
CHECK_STRUCT_SIZE(real_bounds, sizeof(real32) * 2);

struct angle_bounds
{
	angle lower;
	angle upper;
};
CHECK_STRUCT_SIZE(angle_bounds, sizeof(angle) * 2);

class c_quantized_orientation
{
public:
	int16 rotation_x;
	int16 rotation_y;
	int16 rotation_z;
	int16 rotation_w;
	real_point3d default_translation;
	real32 default_scale;
};
CHECK_STRUCT_SIZE(c_quantized_orientation, 24);

struct real_orientation
{
	real_quaternion quaternion;
	real_point3d position;
	real32 scale;
};
CHECK_STRUCT_SIZE(real_orientation, 32);

void __cdecl fast_quaternion_interpolate_and_normalize(const real_quaternion* previous, const real_quaternion* current, real32 fractional_ticks, real_quaternion* out_quaternion);

real32 square_root(real32 f);

real32 magnitude3d_squared(const real_vector3d* v1);

real32 magnitude3d(const real_vector3d* v1);

real32 dot_product3d(const real_vector3d* v1, const real_vector3d* v2);

void add_vectors3d(const real_vector3d* v1, const real_vector3d* v2, real_vector3d* out);

void multiply_vectors3d(const real_vector3d* v1, const real_vector3d* v2, real_vector3d* out);

void subtract_vector3d(const real_vector3d* v1, const real_vector3d* v2, real_vector3d* out);

void scale_vector3d(const real_vector3d* v1, real32 scale, real_vector3d* out);

void vector_from_points3d(const real_point3d* p1, const real_point3d* p2, real_vector3d* out);

real32 distance_squared3d(const real_point3d* p1, const real_point3d* p2);

real32 distance3d(const real_point3d* p1, const real_point3d* p2);

void point_from_line3d(const real_point3d* p1, const real_vector3d* direction, real32 length, real_point3d* out);

real32 normalize3d(real_vector3d* v1);

bool limit3d(real_vector3d* v, real32 limit);

void points_interpolate(const real_vector3d* previous_point, const real_point3d* target_point, real32 fractional_tick, real_point3d* out);

void scale_interpolate(real32 previous_scale, real32 current_scale, real32 fractional_tick, real32* out_scale);

static const real_vector3d global_zero_vector3d = { 0.0f, 0.0f, 0.0f };