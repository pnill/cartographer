#pragma once
//use this for all base real math related structs and implementations
#include "math.h"

#define _USE_MATH_DEFINES
#include <math.h>

#define k_real_math_epsilon 0.000099999997f

#define degreesToRadians(angleDegrees) ((float)((angleDegrees) * M_PI / 180.0))

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

	BLAM_MATH_INL bool operator==(const real_vector3d& other) const
	{
		return i == other.i && j == other.j && k == other.k;
	}

	BLAM_MATH_INL bool operator!=(const real_vector3d& other) const
	{
		return !(*this == other);
	}

	// vector multiplication
	BLAM_MATH_INL real_vector3d operator*(const real_vector3d& other) const
	{
		real_vector3d v;
		v.i = this->i * other.i;
		v.j = this->j * other.j;
		v.k = this->k * other.k;
		return v;
	}

	// scalar multiplication
	BLAM_MATH_INL real_vector3d operator*(const float scalar) const
	{
		real_vector3d v;
		v.i = this->i * scalar;
		v.j = this->j * scalar;
		v.k = this->k * scalar;
		return v;
	}

	BLAM_MATH_INL real_vector3d operator/(const float scalar) const
	{
		real_vector3d v;
		v.i = this->i / scalar;
		v.j = this->j / scalar;
		v.k = this->k / scalar;
		return v;
	}

	// vector addition
	BLAM_MATH_INL real_vector3d operator+(const real_vector3d& other) const
	{
		real_vector3d v;
		v.i = this->i + other.i;
		v.j = this->j + other.j;
		v.k = this->k + other.k;
		return v;
	}

	// vector subtraction
	BLAM_MATH_INL real_vector3d operator-(const real_vector3d& other) const
	{
		real_vector3d v;
		v.i = this->i - other.i;
		v.j = this->j - other.j;
		v.k = this->k - other.k;
		return v;
	}
};
CHECK_STRUCT_SIZE(real_vector3d, sizeof(float) * 3);

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

struct real_matrix4x3
{
	real32 scale;
	real_vector3d forward;
	real_vector3d left;
	real_vector3d up;
	real_point3d position;
};
CHECK_STRUCT_SIZE(real_matrix4x3, 52);

/* channel intensity is represented on a 0 to 1 scale */
union real_color_argb
{
	real32 v[4];
	struct { real32 alpha, red, green, blue; };
};
CHECK_STRUCT_SIZE(real_color_argb, sizeof(real32) * 4);

union real_color_rgb
{
	real32 v[3];
	struct { real32 red, green, blue; };
};
CHECK_STRUCT_SIZE(real_color_rgb, sizeof(real32) * 3);

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

static void scale_interpolate(float previous_scale, float current_scale, float fractional_tick, float* out_scale)
{
	*out_scale = previous_scale * (1.0f - fractional_tick) + (current_scale * fractional_tick);
}

static const real_vector3d global_zero_vector3d = { 0.0f, 0.0f, 0.0f };