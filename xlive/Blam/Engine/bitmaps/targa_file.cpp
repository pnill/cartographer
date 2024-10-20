#include "stdafx.h"
#include "targa_file.h"

/* public code */

void __cdecl targa_export(s_file_reference* file, const bitmap_data* bitmap)
{
	INVOKE(0x286F96, 0x0, targa_export, file, bitmap);
	return;
}
