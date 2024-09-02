#include "stdafx.h"
#include "data.h"

void* datum_get(const s_data_array* data_array, datum datum_index)
{
	return &data_array->data[data_array->datum_element_size * DATUM_INDEX_TO_ABSOLUTE_INDEX(datum_index)];
}

void* datum_try_and_get(const s_data_array* data_array, datum datum_index)
{
	return INVOKE(0x6639B, 0x32087, datum_try_and_get, data_array, datum_index);
}

datum __cdecl datum_new(s_data_array* data_array)
{
	return INVOKE(0x667A0, 0x3248C, datum_new, data_array);
}

void __cdecl datum_delete(s_data_array* data_array, datum datum_index)
{
	INVOKE(0x6693E, 0x3262A, datum_delete, data_array, datum_index);
	return;
}

void data_make_valid(s_data_array* data_array)
{
	// not entirely sure what this actually does
	INVOKE(0x66B33, 0x3281F, data_make_valid, data_array);
}


uint32 __cdecl datum_header_allocate(uint32 total_size, uint32 alignment_bits)
{
	return INVOKE(0x37E69, 0x2B4E6, datum_header_allocate, total_size, alignment_bits);
}

bool __cdecl datum_header_deallocate(void* object)
{
	// todo: server offset
	return INVOKE(0x37EC3, 0, datum_header_deallocate, object);
}