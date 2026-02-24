#pragma once
#include "CartographerDllConf.h"

/* c runtime */

#include <immintrin.h>

#include <type_traits>

/* windows */

#ifdef _WIN32

#define _USE_MATH_DEFINES
#include <math.h>

#define WIN32_LEAN_AND_MEAN

#endif

/* game includes */

#include "cseries/cseries.h"
#include "cseries/cseries_errors.h"
#include "cseries/cseries_system_memory.h"

#include "math/real_math.h"
#include "memory/static_arrays.h"

#include "tag_files/tag_groups.h"

#include "Util/Memory.h"
