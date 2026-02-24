#pragma once
#include "CartographerDllConf.h"

/* c runtime */

#include <immintrin.h>

#include <stdio.h>
#include <type_traits>

/* windows */

#ifdef _WIN32

#include <WinSDKVer.h>

#define WINVER 0x0601
#define _WIN32_WINNT 0x0601

#include <sdkddkver.h>

#define _USE_MATH_DEFINES
#include <math.h>

#define WIN32_LEAN_AND_MEAN     // Exclude rarely-used stuff from Windows headers

#include <Windows.h>
#include <initguid.h>			// initialize GUIDs locally

#endif

/* 3rd party */

/* game includes */

#include "cseries/cseries.h"
#include "cseries/cseries_errors.h"
#include "cseries/cseries_system_memory.h"

#include "math/real_math.h"
#include "memory/static_arrays.h"

#include "tag_files/tag_groups.h"

// use this macro to define _time and _clock namespaces
#define STD_CHRONO_DEFINE_TIME_AND_CLOCK(_time_name, _clock_name) \
	namespace _time_name = std::chrono; \
	using _clock_name = std::chrono::steady_clock; \
	using namespace std::chrono_literals;


static_assert(EXECUTABLE_TYPE >= 0 && EXECUTABLE_TYPE <= 7, "EXECUTABLE_TYPE VALUE BELOW 0 OR EXCEEDS 7");
static_assert(EXECUTABLE_VERSION > 0 && EXECUTABLE_VERSION < 65535, "EXECUTABLE_VERSION VALUE EXCEEDS 65534");
static_assert(COMPATIBLE_VERSION > 0 && COMPATIBLE_VERSION < 65535, "COMPATIBLE_VERSION VALUE EXCEEDS 65534");
