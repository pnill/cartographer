#include "stdafx.h"
#include "async_helpers.h"

bool async_helper_write_buffer_to_file(void* callback_function, int32* buffer_details, uint32 flags, int32 priority, void* result)
{
	return INVOKE(0x8EE0A, 0, async_helper_write_buffer_to_file, callback_function, buffer_details, flags, priority, result);
}
