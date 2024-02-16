#pragma once

int32 quantize_unit_vector(const real_vector3d* p);
void dequantize_unit_vector(int32 quantized_unit_vector, real_vector3d* out_unit_vector);
