#pragma once
#include "CartographerDllConf.h"

/* c runtime */

#include <immintrin.h>

#include <atomic>
#include <memory>
#include <string>
#include <type_traits>
#include <unordered_map>

/* windows */

#ifdef _WIN32

#define _USE_MATH_DEFINES
#include <math.h>

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <WinSock2.h>
#include <iphlpapi.h>

#pragma comment(lib, "IPHLPAPI.lib")

#endif

/* 3rd party */

#include <xlivedefs.h>

/* game includes */

#include "cseries/cseries.h"
#include "cseries/cseries_errors.h"
#include "cseries/cseries_system_memory.h"


#include "math/crypto_windows.h"
#include "math/integer_math.h"
#include "math/real_math.h"
#include "memory/static_arrays.h"
#include "networking/network_game_definitions.h"
#include "tag_files/tag_groups.h"

#include "Util/Hooks/Hook.h"
#include "Util/Memory.h"
