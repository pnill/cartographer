#pragma once

/* prototypes */

void* system_heap_alloc(size_t size);

#ifndef NDEBUG
#define CSERIES_MALLOC(size) system_heap_alloc(size)	// ### TODO: implement debug_malloc
#else
#define CSERIES_MALLOC(size) system_heap_alloc(size)
#endif


BOOL system_heap_free(void* block);

#ifndef NDEBUG
#define CSERIES_FREE(size) system_heap_free(size)	// ### TODO: implement debug_free
#else
#define CSERIES_FREE(size) system_heap_free(size)
#endif
