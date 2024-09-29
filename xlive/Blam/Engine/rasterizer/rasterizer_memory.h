#pragma once

/* prototypes */

uint32 rasterizer_pool_allocate(int32 type, int32 element_count, int32 element_size);

void* rasterizer_pool_get_from_offset(uint32 offset);

void __cdecl rasterizer_memory_initialize(void);
