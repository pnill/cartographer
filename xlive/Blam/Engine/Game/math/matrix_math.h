#pragma once
#include "Blam/Engine/Game/math/real_math.h"

struct real_matrix4x3
{
	float scale = 1.0f;
	real_vector3d forward = {};
	real_vector3d left = {};
	real_vector3d up = {};
	real_point3d position = {};

	real_matrix4x3() = default;

	explicit real_matrix4x3(const real_quaternion& rotation)
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

		forward =	{ 1.0f - (jj + kk),  ij - kw,            ik + jw };
		left =		{ ij + kw,           1.0f - (ii + kk),   jk - iw };
		up =		{ ik - jw,           jk + iw,            1.0f - (ii + jj) };
	}
};
CHECK_STRUCT_SIZE(real_matrix4x3, 52);


namespace matrix_math
{
	void matrix4x3_to_quaternion_rotation(const real_matrix4x3* m, real_quaternion* out_quat);
	void quaternion_rotation_to_matrix4x3(const real_quaternion* quat_rot, real_matrix4x3* out_m);
	void quaternion_interpolate_and_normalize(const real_quaternion* previous, const real_quaternion* current, float fractional_ticks, real_quaternion* out_quaternion);
	void matrix4x3_interpolate(const real_matrix4x3* previous, const real_matrix4x3* target, float fractional_ticks, real_matrix4x3* out_mat);
}
