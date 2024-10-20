#include "stdafx.h"
#include "tiff_file.h"

/* public code */

const char* __cdecl tiff_export_begin_tiled(s_file_reference* file_reference, int32 a2, int32 a3, int32 a4, int32 a5, void** a6)
{
	return INVOKE(0x28731E, 0x0, tiff_export_begin_tiled, file_reference, a2, a3, a4, a5, a6);
}

int32 __cdecl tiff_export_raw(s_file_reference* file, bitmap_data* bitmap)
{
	return INVOKE(0x2871FA, 0x0, tiff_export_raw, file, bitmap);
}

int32 __cdecl tiff_export_add_tile_from_surface(void* handle, const bitmap_data* bitmap, int32 left, int32 top)
{
	return INVOKE(0x2871AF, 0x0, tiff_export_add_tile_from_surface, handle, bitmap, left, top);
}

int32 __cdecl tiff_export_end_tiled(void* data)
{
	return INVOKE(0x287138, 0x0, tiff_export_end_tiled, data);
}
