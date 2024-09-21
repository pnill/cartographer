#pragma once
#include "memory/data.h"

s_data_array* ui_list_data_new(const char* name, uint32 count, uint32 size);

uint8* ui_pool_allocate_space(uint32 pool_size, int32 a2);
void ui_pool_deallocate(uint8* object);
