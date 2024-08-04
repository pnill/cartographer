#include "stdafx.h"
#include "pc_file_system.h"

void file_seek_and_read(FILE* file_handle, uint32 file_offset, uint32 read_size, uint32 read_count, void* out_buffer)
{
	ASSERT(fseek(file_handle, file_offset, SEEK_SET) == 0);
	ASSERT(fread(out_buffer, read_size, read_count, file_handle));
}
