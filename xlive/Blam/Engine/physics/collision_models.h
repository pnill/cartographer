#pragma once
#include "collision_bsp.h"

struct collision_model_test_vector_result
{
	int32 field_0;
	int16 matrix_index;
	int16 field_6;
	collision_bsp_test_vector_result bsp_result;
};
CHECK_STRUCT_SIZE(collision_model_test_vector_result, 32);
