#pragma once
#include "CartographerDllConf.h"

/* c runtime */

#include <immintrin.h>

#include <atomic>
#include <string>
#include <thread>
#include <type_traits>
#include <unordered_map>

/* windows */

#ifdef _WIN32

#define _USE_MATH_DEFINES
#include <math.h>

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <inaddr.h>
#include <wincrypt.h>
#include <shellapi.h>
#include <assert.h>
#include <timeapi.h>

// Use Xinput 9.1.0 and Dinput 0x0800
#define XINPUT_USE_9_1_0
#define DIRECTINPUT_VERSION 0x0800

#include <xinput.h>
#include <dinput.h>

#endif

/* 3rd party */

#include <contrib/minizip/zip.h>

#include <Xlive/xlivedefs.h>

/* game includes */

#include "cseries/cseries.h"
#include "cseries/cseries_errors.h"
#include "cseries/cseries_system_memory.h"

#ifdef _WIN32
#include "cseries/cseries_windows.h"
#endif

#include "math/crypto_windows.h"
#include "math/integer_math.h"
#include "math/real_math.h"
#include "memory/static_arrays.h"
#include "networking/network_game_definitions.h"
#include "tag_files/tag_groups.h"

#include "Util/Hooks/Hook.h"
#include "Util/Memory.h"
