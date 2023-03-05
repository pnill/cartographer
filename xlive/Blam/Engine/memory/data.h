#pragma once
#include "Blam/Engine/DataArray/DataArray.h"

datum __cdecl datum_new(s_data_array* data_array);
bool next_index_range_checks(const s_data_array* data, datum* new_next_index);
void datum_delete(s_data_array* data_array, unsigned __int16 index);
