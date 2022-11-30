#include "stdafx.h"

#include "matrix_math.h"
#include "Blam/Engine/math/real_math.h"

namespace matrix_math
{
	void matrix4x3_to_quaternion_rotation(const real_matrix4x3* m, real_quaternion* out_quat)
	{
		typedef void(__cdecl* matrix4x3_to_quaternion_rotation_t)(const real_matrix4x3*, real_quaternion*);
		auto p_matrix4x3_to_quaternion_rotation = Memory::GetAddressRelative<matrix4x3_to_quaternion_rotation_t>(0x4791A2);
		return p_matrix4x3_to_quaternion_rotation(m, out_quat);
	}

	void quaternion_rotation_to_matrix4x3(const real_quaternion* quat_rot, real_matrix4x3* out_m)
	{
		typedef void(__cdecl* quaternion_rotation_to_matrix4x3)(real_matrix4x3*, const real_quaternion*);
		auto p_quaternion_to_matrix4x3 = Memory::GetAddressRelative<quaternion_rotation_to_matrix4x3>(0x4778CB);
		return p_quaternion_to_matrix4x3(out_m, quat_rot);
	}

	void quaternion_interpolate_and_normalize(const real_quaternion* previous, const real_quaternion* current, float fractional_ticks, real_quaternion* out_quaternion)
	{
		typedef void(__cdecl* quaternion_interpolate_and_normalize_t)(const real_quaternion*, const real_quaternion*, float, real_quaternion*);
		auto p_quaternion_interpolate_and_normalize = Memory::GetAddressRelative<quaternion_interpolate_and_normalize_t>(0x434F6F);
		return p_quaternion_interpolate_and_normalize(previous, current, fractional_ticks, out_quaternion);
	}

	void matrix4x3_interpolate(const real_matrix4x3* previous, const real_matrix4x3* target, float fractional_ticks, real_matrix4x3* out_mat)
	{
		real_quaternion q1_previous, q2_target, q3_interpolated;
		// first we convert matrix to quaternion rotation
		matrix4x3_to_quaternion_rotation(previous, &q1_previous);
		matrix4x3_to_quaternion_rotation(target, &q2_target);
		// interpolate the rotation quaternion
		quaternion_interpolate_and_normalize(&q1_previous, &q2_target, fractional_ticks, &q3_interpolated);
		// and convert back to matrix format, since that's what the game uses
		quaternion_rotation_to_matrix4x3(&q3_interpolated, out_mat);
		// interpolate scale and the position of the node
		real_math::scale_interpolate(previous->scale, target->scale, fractional_ticks, &out_mat->scale);
		real_math::points_interpolate(&previous->position, &target->position, fractional_ticks, &out_mat->position);
	}
}