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

#include <Windows.h>
#include <Winsock2.h>
#include <Shlwapi.h>

#include <d3d9.h>
#include <d3dx9.h>

#include <d3d9on12.h>
#include <d3d12.h>

#include <dxgi1_4.h>

#endif

/* 3rd party */

#include <xlivedefs.h>
#include <xliveless.h>

/* game includes */

#include "cseries/cseries.h"
#include "cseries/cseries_errors.h"
#include "cseries/cseries_system_memory.h"

#include "math/integer_math.h"
#include "math/real_math.h"
#include "memory/static_arrays.h"

#include "tag_files/tag_groups.h"

#include "Util/Hooks/Hook.h"
#include "Util/Memory.h"
