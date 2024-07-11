#include "stdafx.h"

#include "user_interface_memory.h"


s_data_array* ui_list_data_new(const char* name, uint32 count, uint32 size)
{
	return INVOKE(0x20D1FD, 0x0, ui_list_data_new, name, count, size);
}

uint8* ui_pool_allocate_space(uint32 pool_size, int32 a2)
{
	return INVOKE(0x20D2D8, 0x0, ui_pool_allocate_space, pool_size, a2);
}

void ui_pool_dellocate(uint8* object)
{
	return INVOKE(0x20D2EA, 0x0, ui_pool_dellocate, object);
}
