#include "stdafx.h"
#include "rasterizer_dx9_dynamic_geometry.h"

/* public code */

void rasterizer_dx9_primitive_builder(uint8* data, uint32 size, const void* old_data)
{
	csmemcpy(data, old_data, 80u);
	return;
}
