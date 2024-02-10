#include "stdafx.h"

#include "real_quantization.h"

int32 quantize_real(real32 val, real32 min, real32 max, int8 max_bits, bool exact_midpoint)
{
	int32 max_value = (1 << max_bits) - 1;
	if (exact_midpoint)
	{
		max_value -= max_value % 2;
	}

	return (int32)((val - min) / ((max - min) / (real32)max_value));
}

real32 dequantize_real(int32 quantized_value, real32 min, real32 max, int8 max_bits, bool exact_midpoint)
{
	int32 max_value = (1 << max_bits) - 1;
	real32 result;
	if (exact_midpoint)
	{
		max_value -= max_value % 2;
	}

	if (!quantized_value)
	{
		result = min;
	}
	else if (quantized_value < max_value)
	{
		result = (real32)((((real32)(max_value - quantized_value) * min) + ((real32)quantized_value * max)) / (real32)max_value);
	}
	else
	{
		result = max;
	}

	return result;
}

int32 quantize_unit_vector(const real_vector3d* p)
{
	int32 type;
	real32 f1, f2;
	real_vector3d v = { abs(p->i), abs(p->j), abs(p->k) };

	if (v.j >= v.i)
	{
		if (v.j > v.k)
		{
			type = p->j <= 0.0f ? 4 : 1;
			f1 = p->i / v.j;
			f2 = p->k / v.j;
		}
	}
	else if (v.i > v.k)
	{
		type = p->i <= 0.0f ? 3 : 0;
		f1 = p->j / v.i;
		f2 = p->k / v.i;
	}
	else
	{
		type = p->k <= 0.0f ? 5 : 2;
		f1 = p->i / v.k;
		f2 = p->j / v.k;
	}

	int32 t1 = quantize_real(f1, -1.0f, 1.0f, 8, true);
	return type | ((t1 | (quantize_real(f2, -1.0f, 1.0f, 8, true) << 8)) << 3);
}

void dequantize_unit_vector(int32 quantized_unit_vector, real_vector3d* out_unit_vector)
{
	real32 dequantized_f1 = dequantize_real((int8)(quantized_unit_vector >> 3), -1.0f, 1.0f, 8, true);
	real32 dequantized_f2 = dequantize_real((int8)(quantized_unit_vector >> 11), -1.0f, 1.0f, 8, true);

	int32 type = quantized_unit_vector & ((1 << 3) - 1);
	switch (type)
	{
	case 0:
		*out_unit_vector = { 1.0f, dequantized_f1, dequantized_f2 };
		break;
	case 1:
		*out_unit_vector = { dequantized_f1, 1.0f, dequantized_f2, };
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