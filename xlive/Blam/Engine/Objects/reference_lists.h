#pragma once
#include "Blam/Cache/DataTypes/BlamPrimitiveType.h"
#include "Blam/Engine/DataArray/DataArray.h"

struct data_reference
{
	DWORD unk;
	datum next_datum;
	DWORD first_reference_index;
};

void reference_list_add(s_data_array* array, int* first_reference_index, const datum next_datum, const int payload_size, const void* payload);