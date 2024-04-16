#pragma once

#include "math/function_definitions.h"
#include "tag_files/string_id.h"

#define k_maximum_interpolators_per_scenario 16

// max count: 1
struct s_scenario_interpolator
{
	string_id name;
	string_id accelerator_name_interpolator;
	string_id multiplier_name_interpolator;
	c_function_definition function;
	short pad[2];
};
ASSERT_STRUCT_SIZE(s_scenario_interpolator, 24);
