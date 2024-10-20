#pragma once
#include "bitmap_group.h"
#include "tag_files/files_windows.h"

/* prototypes */

const char* __cdecl tiff_export_begin_tiled(s_file_reference* file_reference, int32 a2, int32 a3, int32 a4, int32 a5, void** a6);

int32 __cdecl tiff_export_raw(s_file_reference* file, bitmap_data* bitmap);

int32 __cdecl tiff_export_add_tile_from_surface(void* handle, const bitmap_data* bitmap, int32 left, int32 top);

int32 __cdecl tiff_export_end_tiled(void* data);
