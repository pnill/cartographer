#pragma once
#include "scenario_definitions.h"

struct s_location
{
	DWORD leaf_index;
	short cluster;
	WORD bsp_index;
};
CHECK_STRUCT_SIZE(s_location, 8);

scenario* get_global_scenario();
