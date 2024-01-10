#include "stdafx.h"
#include "matrix_math.h"

real_matrix3x3* matrix3x3_from_forward_and_up(real_matrix3x3* matrix, const real_vector3d* forward, const real_vector3d* up)
{
	matrix->forward = *forward;
	cross_product3d(up, forward, &matrix->left);
	matrix->up = *up;
	return matrix;
}

real_matrix3x3* matrix3x3_rotation_from_quaternion(real_matrix3x3* matrix, const real_quaternion* quaternion)
{
	real32 dot_product = dot_product4d_quaternion(quaternion, quaternion);

	real32 scalar = (dot_product > k_real_math_epsilon ? 2.0f / dot_product : 0.0f);

	real_vector3d scaled_vector;
	scale_vector3d(&quaternion->vector, scalar, &scaled_vector);

	real_vector3d w_scaled_vector;
	scale_vector3d(&scaled_vector, quaternion->w, &w_scaled_vector);

	real_vector3d i_scaled_vector;
	scale_vector3d(&scaled_vector, quaternion->i, &i_scaled_vector);

	real32 j_scaled_vector_j = quaternion->j * scaled_vector.j;
	real32 j_scaled_vector_k = quaternion->j * scaled_vector.k;
	real32 k_scaled_vector_k = quaternion->k * scaled_vector.k;

	matrix->forward.i = 1.0f - (k_scaled_vector_k + j_scaled_vector_j);
	matrix->left.i = i_scaled_vector.j - w_scaled_vector.k;
	matrix->up.i = i_scaled_vector.k + w_scaled_vector.j;
	matrix->forward.j = i_scaled_vector.j + w_scaled_vector.k;
	matrix->left.j = 1.0f - (k_scaled_vector_k + i_scaled_vector.i);
	matrix->up.j = j_scaled_vector_k - w_scaled_vector.i;
	matrix->forward.k = i_scaled_vector.k - w_scaled_vector.j;
	matrix->left.k = j_scaled_vector_k + w_scaled_vector.i;
	matrix->up.k = 1.0f - (j_scaled_vector_j + i_scaled_vector.i);
	return matrix;
}

real_quaternion* matrix3x3_rotation_to_quaternion(const real_matrix3x3* matrix, real_quaternion* quaternion)
{
	real32 v1 = matrix->forward.i + matrix->left.j + matrix->up.k;
	if (v1 <= 0.0f)
	{
		uint32 i = matrix->forward.i < matrix->left.j;
		if (matrix->forward.v[i] < matrix->up.k)
		{
			i = 2;
		}

		int32 i_1 = (i + 1) % 3;
		int32 i_2 = (i + 2) % 3;
		real32 forward_result = matrix->v.n[4 * i] - matrix->v.n[4 * i_1] - matrix->v.n[4 * i_2] + 1.0f;
		real32 forward_result_sqroot = square_root(forward_result);

		quaternion->v.n[i] = forward_result_sqroot * 0.5f;
		real32 scalar = 0.25f / quaternion->v.n[i];

		quaternion->w = (matrix->matrix[i_1][i_2] - matrix->matrix[i_2][i_1]) * scalar;
		quaternion->v.n[i_1] = (matrix->matrix[i][i_1] + matrix->matrix[i_1][i]) * scalar;
		quaternion->v.n[i_2] = scalar * (matrix->matrix[i][i_2] + matrix->matrix[i_2][i]);
	}
	else
	{
		real32 v1_root = square_root(v1 + 1.0f);
		quaternion->w = v1_root * 0.5f;

		real32 scalar = 0.25f / quaternion->w;
		quaternion->i = (matrix->left.k - matrix->up.j) * scalar;
		quaternion->j = (matrix->up.i - matrix->forward.k) * scalar;
		quaternion->k = (matrix->forward.j - matrix->left.i) * scalar;
	}

	if (quaternion->w < 0.0f)
	{
		quaternion->i = -quaternion->i;
		quaternion->j = -quaternion->j;
		quaternion->k = -quaternion->k;
		quaternion->w = -quaternion->w;
	}
	return quaternion;
}


void matrix4x3_from_orientation(real_matrix4x3* matrix, const real_orientation* orientation)
{
	matrix4x3_rotation_from_quaternion(matrix, &orientation->quaternion);
	matrix->position = orientation->position;
	matrix->scale = orientation->scale;
	return;
}

void matrix4x3_rotation_from_quaternion(real_matrix4x3* matrix, const real_quaternion* quaternion)
{
	matrix3x3_rotation_from_quaternion(&matrix->vectors, quaternion);
	matrix->position.x = 0.0f;
	matrix->position.y = 0.0f;
	matrix->position.z = 0.0f;
	matrix->scale = 1.0f;
	return;
}

real_quaternion* matrix4x3_rotation_to_quaternion(const real_matrix4x3* matrix, real_quaternion* out)
{
	return matrix3x3_rotation_to_quaternion(&matrix->vectors, out);
	
}

void matrix4x3_rotation_from_vectors(real_matrix4x3* matrix, const real_vector3d* forward, const real_vector3d* up)
{
	matrix->scale = 1.0f;
	matrix->vectors.forward = *forward;
	cross_product3d(up, forward, &matrix->vectors.left);
	matrix->vectors.up = *up;
	set_real_point3d(&matrix->position, 0.0f, 0.0f, 0.0f);
	return;
}

void matrix4x3_from_point_and_vectors(real_matrix4x3* matrix, const real_point3d* position, const real_vector3d* forward, const real_vector3d* up)
{
	matrix4x3_rotation_from_vectors(matrix, forward, up);
	matrix->position = *position;
	return;
}

void matrix4x3_inverse(const real_matrix4x3* input, real_matrix4x3* output)
{
	if (input->scale == 0.0f)
	{
		csmemset(output, 0, sizeof(real_matrix4x3));
	}
	else
	{
		real_point3d inverse_pos;
		inverse_pos.x = -input->position.x;
		inverse_pos.y = -input->position.y;
		inverse_pos.z = -input->position.z;
		if (input->scale == 1.0f)
		{
			output->scale = 1.0f;
		}
		else
		{
			output->scale = 1.0 / input->scale;
			inverse_pos.x *= output->scale;
			inverse_pos.y *= output->scale;
			inverse_pos.z *= output->scale;
		}
		output->vectors.forward.i = input->vectors.forward.i;
		output->vectors.left.j = input->vectors.left.j;
		output->vectors.up.k = input->vectors.up.k;

		real32 temp_value = input->vectors.left.i;
		output->vectors.left.i = input->vectors.forward.j;
		output->vectors.forward.j = temp_value;
		
		temp_value = input->vectors.up.i;
		output->vectors.up.i = input->vectors.forward.k;
		output->vectors.forward.k = temp_value;

		temp_value = input->vectors.up.j;
		output->vectors.up.j = input->vectors.left.k;
		output->vectors.left.k = temp_value;

		output->position.x = output->vectors.left.i * inverse_pos.y		+ output->vectors.forward.i * inverse_pos.x + output->vectors.up.i * inverse_pos.z;
		output->position.y = output->vectors.forward.j * inverse_pos.x	+ output->vectors.left.j * inverse_pos.x	+ output->vectors.up.j * inverse_pos.z;
		output->position.z = output->vectors.forward.k * inverse_pos.x	+ output->vectors.left.k * inverse_pos.y	+ output->vectors.up.k * inverse_pos.z;
	}

	return;
}

void __fastcall matrix4x3_multiply(const real_matrix4x3* matrix1, const real_matrix4x3* matrix2, real_matrix4x3* out)
{
	INVOKE(0x78209, 0x75DFF, matrix4x3_multiply, matrix1, matrix2, out);
	return;
}

void matrix4x3_translation(real_matrix4x3* matrix, const real_point3d* position)
{
	matrix->scale = 1.0f;
	matrix->vectors.forward.i = 1.0f;
	matrix->vectors.forward.j = 0.0f;
	matrix->vectors.forward.k = 0.0f;
	matrix->vectors.left.i = 0.0f;
	matrix->vectors.left.j = 1.0f;
	matrix->vectors.left.k = 0.0f;
	matrix->vectors.up.i = 0.0f;
	matrix->vectors.up.j = 0.0f;
	matrix->vectors.up.k = 1.0f;
	matrix->position = *position;
	return;
}

real_matrix4x3* matrix4x3_identity(real_matrix4x3* matrix)
{
	matrix->scale = 1.0f;
	matrix->vectors.forward.i = 1.0f;
	matrix->vectors.forward.j = 0.0f;
	matrix->vectors.forward.k = 0.0f;
	matrix->vectors.left.i = 0.0f;
	matrix->vectors.left.j = 1.0f;
	matrix->vectors.left.k = 0.0f;
	matrix->vectors.up.i = 0.0f;
	matrix->vectors.up.j = 0.0f;
	matrix->vectors.up.k = 1.0f;
	matrix->position.x = 0.0f;
	matrix->position.y = 0.0f;
	matrix->position.z = 0.0f;
	return matrix;
}

void matrix4x3_from_point_and_quaternion(real_matrix4x3* matrix, const real_point3d* point, const real_quaternion* quaternion)
{
	matrix3x3_rotation_from_quaternion(&matrix->vectors, quaternion);
	matrix->position.x = 0.0f;
	matrix->position.y = 0.0f;
	matrix->position.z = 0.0f;
	matrix->scale = 1.0f;
	matrix->position = *point;
	return;
}

void matrix4x3_to_point_and_vectors(real_matrix4x3* matrix, real_point3d* position, real_vector3d* forward, real_vector3d* up)
{
	*forward = matrix->vectors.forward;
	*up = matrix->vectors.up;
	*position = matrix->position;
	return;
}

real_point3d* matrix4x3_transform_point(const real_matrix4x3* matrix, const real_point3d* in, real_point3d* out)
{
	real_point3d in_copy = *in; 
	if (matrix->scale != 1.0f)
	{
		in_copy.x *= matrix->scale;
		in_copy.y *= matrix->scale;
		in_copy.z *= matrix->scale;
	}

	out->x = (((matrix->vectors.up.i * in_copy.z) + (matrix->vectors.left.i * in_copy.y)) + (matrix->vectors.forward.i * in_copy.x)) + matrix->position.x;
	out->y = (((matrix->vectors.up.j * in_copy.z) + (matrix->vectors.left.j * in_copy.y)) + (matrix->vectors.forward.j * in_copy.x)) + matrix->position.y;
	out->z = (((matrix->vectors.up.k * in_copy.z) + (matrix->vectors.left.k * in_copy.y)) + (matrix->vectors.forward.k * in_copy.x)) + matrix->position.z;
	return out;
}

real_vector3d* matrix4x3_transform_vector(const real_matrix4x3* matrix, const real_vector3d *in, real_vector3d* out)
{
	real_point3d in_copy = *in;
	if (matrix->scale != 1.0f)
	{
		scale_vector3d(&in_copy, matrix->scale, &in_copy);
	}

	out->x = (((matrix->vectors.up.i * in_copy.z) + (matrix->vectors.left.i * in_copy.y)) + (matrix->vectors.forward.i * in_copy.x)) + matrix->position.x;
	out->y = (((matrix->vectors.up.j * in_copy.z) + (matrix->vectors.left.j * in_copy.y)) + (matrix->vectors.forward.j * in_copy.x)) + matrix->position.y;
	out->z = (((matrix->vectors.up.k * in_copy.z) + (matrix->vectors.left.k * in_copy.y)) + (matrix->vectors.forward.k * in_copy.x)) + matrix->position.z;
	return out;
}

real_matrix4x3* matrix4x3_rotation_from_angles(real_matrix4x3* matrix, real32 i, real32 j, real32 k)
{
	const real_vector3d cosine_vector { cos(i), cos(j), cos(k) };
	const real_vector3d sine_vector{ sin(i), sin(j), sin(k) };

	const real32 j_cos_k = sine_vector.j * cosine_vector.k;
	const real32 sin_jk = sine_vector.j * sine_vector.k;

	matrix->scale = 1.0f;
	matrix->vectors.forward.i = cosine_vector.i * cosine_vector.j;
	matrix->vectors.forward.j = sine_vector.i * cosine_vector.k - sin_jk * cosine_vector.i;
	matrix->vectors.forward.k = j_cos_k * cosine_vector.i + sine_vector.i * sine_vector.k;
	
	matrix->vectors.left.i = -cosine_vector.j * sine_vector.i;
	matrix->vectors.left.j = sin_jk * sine_vector.i + cosine_vector.i * cosine_vector.k;
	matrix->vectors.left.k = cosine_vector.i * sine_vector.k - sine_vector.i * j_cos_k;
	
	matrix->vectors.up.i = -sine_vector.j;
	matrix->vectors.up.j = -sine_vector.k * cosine_vector.j;
	matrix->vectors.up.k = cosine_vector.j * cosine_vector.k;
	
	matrix->position.x = 0.0f;
	matrix->position.y = 0.0f;
	matrix->position.z = 0.0f;
	return matrix;
}

void __cdecl matrix4x3_rotation_from_axis_and_angle(real_matrix4x3* matrix, real_vector3d* vector, real32 axis, real32 angle)
{
	return INVOKE(0x775C1, 0x751B7, matrix4x3_rotation_from_axis_and_angle, matrix, vector, axis, angle);
}

void matrix4x3_interpolate(const real_matrix4x3* previous, const real_matrix4x3* target, real32 fractional_ticks, real_matrix4x3* out_mat)
{
	real_quaternion q1_previous, q2_target, q3_interpolated;
	// first we convert matrix to quaternion rotation
	matrix4x3_rotation_to_quaternion(previous, &q1_previous);
	matrix4x3_rotation_to_quaternion(target, &q2_target);
	// interpolate the rotation quaternion
	fast_quaternion_interpolate_and_normalize(&q1_previous, &q2_target, fractional_ticks, &q3_interpolated);
	// and convert back to matrix format, since that's what the game uses
	matrix4x3_rotation_from_quaternion(out_mat, &q3_interpolated);
	// interpolate scale and the position of the node
	scale_interpolate(previous->scale, target->scale, fractional_ticks, &out_mat->scale);
	points_interpolate(&previous->position, &target->position, fractional_ticks, &out_mat->position);
	return;
}

void __cdecl matrix3x3_multiply(real_matrix3x3* input_matrix_1, real_matrix3x3* input_matrix_2, real_matrix3x3* out_matrix)
{
	INVOKE(0x78645, 0x7623B, matrix3x3_multiply, input_matrix_1, input_matrix_2, out_matrix);
}

void matrix3x3_transform_vector(real_matrix3x3* input_matrix, real_vector3d* input_vector, real_vector3d* out_vector)
{
	INVOKE(0x78800, 0x763F6, matrix3x3_transform_vector, input_matrix, input_vector, out_vector);
}
