#pragma once

/* macros */

#define CARTOGRAPHER_HEAP_DEBUG 0

#if CARTOGRAPHER_HEAP_DEBUG
#define _CRTDBG_MAP_ALLOC
#endif

/* includes */

#include "version.h"
