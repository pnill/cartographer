#pragma once
#include "Blam/Engine/DataArray/DataArray.h"

bool next_index_range_checks(const s_data_array* data, datum* new_next_index);
datum __cdecl datum_new(s_data_array* data_array);
void __cdecl datum_delete(s_data_array* data_array, datum index);
