#pragma once
#include "Blam/Engine/DataArray/DataArray.h"

struct cluster_partition
{
	int* cluster_references;
	s_data_array* array0;
	s_data_array* array1;
};
