#include "stdafx.h"
#include "data.h"

void* datum_get(s_data_array* data, datum object_index)
{
	return data->data + (data->datum_element_size * DATUM_INDEX_TO_ABSOLUTE_INDEX(object_index));
}

void __cdecl datum_delete(s_data_array* data_array, datum object_index)
{
	INVOKE(0x6693E, 0x3262A, datum_delete, data_array, object_index);
	return;
}