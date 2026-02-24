#pragma once
#include "CartographerDllConf.h"

/* c runtime */

#include <immintrin.h>
#include <stdio.h>

#include <type_traits>

/* windows */

#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN

#define _USE_MATH_DEFINES
#include <math.h>

#include <Windows.h>
#include <inaddr.h>

#endif

/* 3rd party libs */

#include <xlivedefs.h>

/* game includes */

#include "cseries/cseries.h"
#include "cseries/cseries_errors.h"
#include "cseries/cseries_system_memory.h"

#include "math/crypto_windows.h"
#include "math/integer_math.h"
#include "math/real_math.h"
#include "memory/static_arrays.h"

#include "tag_files/tag_groups.h"

#include "Util/Hooks/Hook.h"
#include "Util/Memory.h"
