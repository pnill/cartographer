#pragma once
#include "Blam/Math/real_math.h"

namespace matrix_math
{
	void matrix4x3_to_quaternion_rotation(const real_matrix4x3* m, real_quaternion* out_quat);
	void quaternion_rotation_to_matrix4x3(const real_quaternion* quat_rot, real_matrix4x3* out_m);
	void quaternion_interpolate_and_normalize(const real_quaternion* previous, const real_quaternion* current, float fractional_ticks, real_quaternion* out_quaternion);
	void matrix4x3_interpolate(const real_matrix4x3* previous, const real_matrix4x3* target, float fractional_ticks, real_matrix4x3* out_mat);
}
