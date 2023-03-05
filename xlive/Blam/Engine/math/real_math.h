#pragma once
//use this for all base real math related structs and implementations

#include "Blam\Cache\DataTypes\BlamPrimitiveType.h"

#define _USE_MATH_DEFINES
#include <math.h>

#define k_real_math_epsilon 0.000099999997f

#define degreesToRadians(angleDegrees) ((float)((angleDegrees) * M_PI / 180.0))

#ifndef _DEBUG
#define BLAM_MATH_INL __forceinline
#else
#define BLAM_MATH_INL 
#endif

struct real_point2d
{
	float x, y;
};
CHECK_STRUCT_SIZE(real_point2d, sizeof(float) * 2);

struct angle
{
	float rad = 0.0f;

	angle() {};

	angle(float _rad) :
		rad(_rad)
	{}

	bool operator==(const angle& other) const
	{
		return other.rad == rad;
	}

	bool operator!=(const angle& other) const
	{
		return !operator==(other);
	}


	double as_degree() const
	{
		return rad * (180.0 / 3.14159265358979323846);
	}

	double as_rad() const
	{
		return rad;
	}
};
CHECK_STRUCT_SIZE(angle, sizeof(float));

struct real_euler_angles2d
{
	angle yaw;
	angle pitch;
};
CHECK_STRUCT_SIZE(real_euler_angles2d, sizeof(angle) * 2);

struct real_euler_angles3d
{
	angle yaw;
	angle pitch;
	angle roll;
};
CHECK_STRUCT_SIZE(real_euler_angles3d, sizeof(angle) * 3);

union real_vector3d
{
	float v[3];
	struct { float i, j, k; };
	struct { float x, y, z; };

	BLAM_MATH_INL real_vector3d(const float _i, const float _j, const float _k) :
		i(_i),
		j(_j),
		k(_k)
	{
	}

	BLAM_MATH_INL real_vector3d() : real_vector3d(0.0f, 0.0f, 0.0f)
	{
	}

	BLAM_MATH_INL real_euler_angles3d get_angle() const
	{
		real_euler_angles3d angle;
		angle.yaw = acos(i);
		angle.pitch = acos(j);
		angle.roll = acos(k);
		return angle;
	}

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

struct real_vector2d
{
	float i, j;
};
CHECK_STRUCT_SIZE(real_vector2d, sizeof(float) * 2);

struct real_plane2d
{
	real_vector2d normal;
	float distance;
};
CHECK_STRUCT_SIZE(real_plane2d, sizeof(real_vector2d) + 4);

struct real_plane3d
{
	real_vector3d normal;
	float distance;
};
CHECK_STRUCT_SIZE(real_plane3d, sizeof(real_vector3d) + sizeof(float));

float BLAM_MATH_INL plane3d_distance_to_point(const real_plane3d* plane, const real_point3d* point);

struct real_quaternion
{
	union
	{
		float v[4];
		float i, j, k, w;
	};

	inline float get_square_length() const
	{
		return i * i + j * j + k * k + w * w;
	}
};
CHECK_STRUCT_SIZE(real_quaternion, sizeof(float) * 4);

struct real_bounds
{
	float lower;
	float upper;
};
CHECK_STRUCT_SIZE(real_bounds, sizeof(float) * 2);

struct angle_bounds
{
	angle lower;
	angle upper;
};
CHECK_STRUCT_SIZE(angle_bounds, sizeof(angle) * 2);

struct real_matrix4x3
{
	float scale = 1.0f;
	real_vector3d forward = {};
	real_vector3d left = {};
	real_vector3d up = {};
	real_point3d position = {};

	real_matrix4x3() = default;

	real_matrix4x3(const real_quaternion& rotation)
	{
		set_rotation(rotation);
	}

	real_matrix4x3(const real_quaternion& _rotation, const real_point3d& _position) :
		position(_position)
	{
		set_rotation(_rotation);
	}

	BLAM_MATH_INL void inverse_rotation()
	{
		std::swap(forward.j, left.i);
		std::swap(forward.k, up.i);
		std::swap(left.k, up.j);
	}

	BLAM_MATH_INL void inverse()
	{
		assert(scale != 0.0f);
		scale = 1.0f / scale;

		inverse_rotation();

		float inverse_pos_x = -position.x * scale;
		float inverse_pos_y = -position.y * scale;
		float inverse_pos_z = -position.z * scale;

		position.x = (inverse_pos_x * forward.i) + (inverse_pos_y * left.i) + (inverse_pos_z * up.i);
		position.y = (inverse_pos_x * forward.j) + (inverse_pos_y * left.j) + (inverse_pos_z * up.j);
		position.z = (inverse_pos_x * forward.k) + (inverse_pos_y * left.k) + (inverse_pos_z * up.k);
	};

	void set_rotation(const real_quaternion& rotation)
	{
		float square_len = rotation.get_square_length();
		assert(square_len != 0.0f);
		float s = 2.0f / square_len;

		auto is = rotation.i * s;
		auto js = rotation.j * s;
		auto ks = rotation.k * s;

		auto iw = rotation.w * is;
		auto jw = rotation.w * js;
		auto kw = rotation.w * ks;

		auto ii = rotation.i * is, jj = rotation.j * js, kk = rotation.k * ks;
		auto ij = rotation.i * js, ik = rotation.i * ks, jk = rotation.j * ks;

		forward = { 1.0f - (jj + kk),  ij - kw,            ik + jw };
		left = { ij + kw,           1.0f - (ii + kk),   jk - iw };
		up = { ik - jw,           jk + iw,            1.0f - (ii + jj) };
	}
};
CHECK_STRUCT_SIZE(real_matrix4x3, 52);

/* channel intensity is represented on a 0 to 1 scale */
struct real_color_argb
{
	float alpha = 1.0f;
	float red = 1.0f;
	float green = 1.0f;
	float blue = 1.0f;

	real_color_argb() {}

	real_color_argb(float _alpha, float _red, float _green, float _blue) :
		alpha(_alpha),
		red(_red),
		green(_green),
		blue(_blue)
	{}
};
CHECK_STRUCT_SIZE(real_color_argb, sizeof(float) * 4);

struct real_color_rgb
{
	float red = 1.0f;
	float green = 1.0f;
	float blue = 1.0f;

	real_color_rgb() {}

	real_color_rgb(float _red, float _green, float _blue) :
		red(_red),
		green(_green),
		blue(_blue)
	{}

	real_color_rgb(const real_color_argb& colour) :
		red(colour.red),
		green(colour.green),
		blue(colour.blue)
	{}

	real_color_argb as_rgba(float _alpha = 1.0f)
	{
		real_color_argb converted;
		converted.alpha = _alpha;
		converted.red = red;
		converted.green = green;
		converted.blue = blue;
		return converted;
	}
};
CHECK_STRUCT_SIZE(real_color_rgb, sizeof(float) * 3);

static void scale_interpolate(float previous_scale, float current_scale, float fractional_tick, float* out_scale)
{
	*out_scale = previous_scale * (1.0f - fractional_tick) + (current_scale * fractional_tick);
}

static const real_vector3d global_zero_vector3d = { 0.0f, 0.0f, 0.0f };
