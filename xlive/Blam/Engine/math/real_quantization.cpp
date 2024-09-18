#include "stdafx.h"

#include "real_quantization.h"

int32 quantize_real(real32 value, real32 min_value, real32 max_value, int32 size_in_bits, bool exact_midpoint)
{
	int32 step_count = (1 << size_in_bits) - 1;
	if (exact_midpoint)
	{
		step_count -= step_count % 2;
	}

	return (int32)((value - min_value) / ((max_value - min_value) / (real32)step_count));
}

real32 dequantize_real(int32 quantized_value, real32 min_value, real32 max_value, int32 size_in_bits, bool exact_midpoint)
{
	int32 step_count = (1 << size_in_bits) - 1;
	real32 result;
	if (exact_midpoint)
	{
		step_count -= step_count % 2;
	}

	if (!quantized_value)
	{
		result = min_value;
	}
	else if (quantized_value < step_count)
	{
		result = (real32)((((real32)(step_count - quantized_value) * min_value) + ((real32)quantized_value * max_value)) / (real32)step_count);
	}
	else
	{
		result = max_value;
	}

	return result;
}

int32 quantize_unit_vector(const real_vector3d* u)
{
	int32 type;
	real32 f1, f2;
	real_vector3d v = { abs(u->i), abs(u->j), abs(u->k) };

	if (v.i > v.j
		&& v.i > v.k)
	{
		type = u->i > 0.0f ? 0 : 3;
		f1 = u->j / v.i;
		f2 = u->k / v.i;
	}
	else if (v.j > v.k)
	{
		type = u->j > 0.0f ? 1 : 4;
		f1 = u->i / v.j;
		f2 = u->k / v.j;
	}
	else
	{
		type = u->k > 0.0f ? 2 : 5;
		f1 = u->i / v.k;
		f2 = u->j / v.k;
	}

	int32 quantized_f1 = quantize_real(f1, -1.0f, 1.0f, 8, true);
	return type | ((quantized_f1 | (quantize_real(f2, -1.0f, 1.0f, 8, true) << 8)) << 3);
}

void dequantize_unit_vector(int32 quantized_unit_vector, real_vector3d* out_unit_vector)
{
	real32 dequantized_f1 = dequantize_real((uint8)(quantized_unit_vector >>  3), -1.0f, 1.0f, 8, true);
	real32 dequantized_f2 = dequantize_real((uint8)(quantized_unit_vector >> 11), -1.0f, 1.0f, 8, true);

	int32 type = quantized_unit_vector & ((1 << 3) - 1);
	switch (type)
	{
	case 0:
		*out_unit_vector = { 1.0f, dequantized_f1, dequantized_f2 };
		break;
	case 1:
		*out_unit_vector = { dequantized_f1, 1.0f, dequantized_f2 };
		break;
	case 2:
		*out_unit_vector = { dequantized_f1, dequantized_f2, 1.0f };
		break;
	case 3:
		*out_unit_vector = { -1.0f, dequantized_f1, dequantized_f2 };
		break;
	case 4:
		*out_unit_vector = { dequantized_f1, -1.0f, dequantized_f2 };
		break;
	case 5:
		*out_unit_vector = { dequantized_f1, dequantized_f2, -1.0f };
		break;
	default:
		*out_unit_vector = global_up3d;
		break;
	}

	normalize3d(out_unit_vector);
}