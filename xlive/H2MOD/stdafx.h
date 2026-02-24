#pragma once
#include "CartographerDllConf.h"

/* c runtime */

#include <immintrin.h>

#include <atomic>
#include <mutex>
#include <random>
#include <stdexcept>
#include <string>
#include <thread>
#include <type_traits>
#include <unordered_map>
#include <vector>

/* windows */

#ifdef _WIN32

#define _USE_MATH_DEFINES
#include <math.h>

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <bcrypt.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <mmsystem.h>
#include <assert.h>

// Use Xinput 9.1.0 and Dinput 0x0800
#define XINPUT_USE_9_1_0
#define DIRECTINPUT_VERSION 0x0800

#include <xinput.h>
#include <dinput.h>

#endif

/* 3rd party libs */

#include <curl/curl.h>
#include <contrib/minizip/zip.h>

#include <xlivedefs.h>

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

#include "tag_files/tag_groups.h"

#include "H2MOD/Utils/curl-interface.h"
#include "H2MOD/Utils/curl_constants.h"

#include "Util/Hooks/Hook.h"
#include "Util/Memory.h"

extern std::random_device rd;

// use this macro to define _time and _clock namespaces
#define STD_CHRONO_DEFINE_TIME_AND_CLOCK(_time_name, _clock_name) \
	namespace _time_name = std::chrono; \
	using _clock_name = std::chrono::steady_clock; \
	using namespace std::chrono_literals;