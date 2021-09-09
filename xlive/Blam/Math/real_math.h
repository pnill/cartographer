#pragma once
//use this for all base real math related structs and implementations

#include "Blam\Cache\DataTypes\BlamPrimitiveType.h"

#define _USE_MATH_DEFINES
#include <math.h>

#define degreesToRadians(angleDegrees) ((float)((angleDegrees) * M_PI / 180.0))

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

struct real_vector2d
{
	float i, j;
};
CHECK_STRUCT_SIZE(real_vector2d, sizeof(float) * 2);

union real_vector3d
{
	struct { float i, j, k; };
	struct { float x, y, z; };

	real_euler_angles3d get_angle() const
	{
		real_euler_angles3d angle;
		angle.yaw = acos(i);
		angle.pitch = acos(j);
		angle.roll = acos(k);
		return angle;
	}

	// vector multiplication
	real_vector3d operator*(const real_vector3d& other) const
	{
		real_vector3d v;
		v.i = this->i * other.i;
		v.j = this->j * other.j;
		v.k = this->k * other.k;
		return v;
	}

	// scalar multiplication
	real_vector3d operator*(const float& scalar) const
	{
		real_vector3d v;
		v.i = this->i * scalar;
		v.j = this->j * scalar;
		v.k = this->k * scalar;
		return v;
	}

	real_vector3d operator/(const float& scalar) const
	{
		real_vector3d v;
		v.i = this->i / scalar;
		v.j = this->j / scalar;
		v.k = this->k / scalar;
		return v;
	}

	// vector addition
	real_vector3d operator+(const real_vector3d& other) const
	{
		real_vector3d v;
		v.i = this->i + other.i;
		v.j = this->j + other.j;
		v.k = this->k + other.k;
		return v;
	}

	// vector subtraction
	real_vector3d operator-(const real_vector3d& other) const
	{
		real_vector3d v;
		v.i = this->i - other.i;
		v.j = this->j - other.j;
		v.k = this->k - other.k;
		return v;
	}

	// dot product
	float dot_product(const real_vector3d& other) const
	{
		return this->i * other.i + this->j * other.j + this->k * other.k;
	}

	// dot product by itself
	float dot_product() const
	{
		return this->i * this->i + this->j * this->j + this->k * this->k;
	}

	// magnitude
	float magnitude() const
	{
		return sqrt(i * i + j * j + k * k);
	}
};
CHECK_STRUCT_SIZE(real_vector3d, sizeof(float) * 3);

typedef real_vector3d real_point3d;

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

struct real_quaternion
{
	float i, j, k, w;

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
	real_point3d translation = {};

	real_matrix4x3() = default;

	real_matrix4x3(const real_quaternion& rotation)
	{
		set_rotation(rotation);
	}

	real_matrix4x3(const real_quaternion& _rotation, const real_point3d& _translation) :
		translation(_translation)
	{
		set_rotation(_rotation);
	}

	inline void inverse_rotation()
	{
		std::swap(forward.j, left.i);
		std::swap(forward.k, up.i);
		std::swap(left.k, up.j);
	}

	inline void inverse()
	{
		assert(scale != 0.0f);
		scale = 1.0f / scale;

		inverse_rotation();

		float inverse_pos_x = -translation.x * scale;
		float inverse_pos_y = -translation.y * scale;
		float inverse_pos_z = -translation.z * scale;

		translation.x = (inverse_pos_x * forward.i) + (inverse_pos_y * left.i) + (inverse_pos_z * up.i);
		translation.y = (inverse_pos_x * forward.j) + (inverse_pos_y * left.j) + (inverse_pos_z * up.j);
		translation.z = (inverse_pos_x * forward.k) + (inverse_pos_y * left.k) + (inverse_pos_z * up.k);
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

	real_color_rgb(const real_color_argb &colour) :
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

static float normalize3d(real_vector3d *a1)
{
	return Memory::GetAddressRelative<float(__cdecl*)(real_vector3d*)>(0x429359, 0x4273B0)(a1);
}

static bool limit3d(void* a1, float limit)
{
	real_vector3d* v1 = (real_vector3d*)a1;

	float dot_product = v1->dot_product(*v1);

	if (dot_product <= pow(limit, 2.0f))
		return false;

	*v1 = *v1 * ((1.0f / sqrt(dot_product)) * limit);
	return true;
}

static const real_vector3d global_zero_vector3d = { 0.0f, 0.0f, 0.0f };