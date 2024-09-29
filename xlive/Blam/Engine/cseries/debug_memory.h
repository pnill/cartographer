#pragma once

/* prototypes */

void system_debug_memory_internal(const char* string, const char* file, int32 line);

/* macros */

#ifdef DEBUG_MEMORY_ENABLED
#define SYSTEM_DEBUG_MEMORY(STATEMENT)	\
system_debug_memory_internal("after "#STATEMENT, __FILE__, __LINE__)
#else
#define SYSTEM_DEBUG_MEMORY(STATEMENT) (void)(#STATEMENT)
#endif
