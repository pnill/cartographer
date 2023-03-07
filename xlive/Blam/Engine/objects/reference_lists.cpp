#include "stdafx.h"
#include "reference_lists.h"

void reference_list_add(s_data_array* array, int* first_reference_index, datum next_datum, const int payload_size, const void* payload)
{
    datum object_datum;
    data_reference* current_data_ref = nullptr;

    object_datum = s_data_array::datum_new_in_range(array);
    if (object_datum != -1)
    {
        current_data_ref = (data_reference*)&array->data[array->single_element_size * (unsigned __int16)object_datum];
        current_data_ref->next_datum = next_datum;
        current_data_ref->first_reference_index = *first_reference_index;
        *first_reference_index = object_datum;

        if (payload)
        {
            memcpy(current_data_ref + sizeof(data_reference), payload, payload_size);
        }
    }
}