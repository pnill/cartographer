// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//
#pragma once

#define CARTOGRAPHER_HEAP_DEBUG 0

#define WINVER _WIN32_WINNT_VISTA
#define _WIN32_WINNT _WIN32_WINNT_VISTA
#define _WINSOCK_DEPRECATED_NO_WARNINGS
// sets the dinput/xinput version, Halo 2 uses the older 9.1.0 version and DirectInput 0x0800
#define XINPUT_USE_9_1_0
#define DIRECTINPUT_VERSION 0x0800

#include "version.h"

#define TEST_N_DEF(TEST)

#define _USE_MATH_DEFINES
#include <math.h>

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <WinSock2.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <wincrypt.h>
#include <mmsystem.h>
#include <windef.h>
#include <codecvt>
#include <DbgHelp.h>
#include <ShlObj.h>
#include <psapi.h>
#include <TlHelp32.h>

// game input
#include <xinput.h>
#include <dinput.h>

// TODO: include this only if trac
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <float.h>
//#include <Objbase.h>
//#include <Shlwapi.h>
#include <set>
#include <map>
#include <mutex>
#include <queue>
#include <string>
#include <vector>
#include <random>
#include <numeric>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <filesystem>

#include <d3d9.h>
#include <d3dx9.h>

// Cartographer includes
#include "cseries/cseries.h"
#include "math/math.h"
#include "math/integer_math.h"
#include "math/real_math.h"

#include "Util/curl-interface.h"
#include "Util/Hooks/Hook.h"
#include "Util/log.h"
#include "Util/Memory.h"
#include "Util/remove_all_ptr.h"

#include "xliveless.h"
#include "xlivedefs.h"


#include <contrib/minizip/zip.h>

#if defined __has_include
#  if __has_include ("CartographerDllConf.h")
#    include "CartographerDllConf.h"
#  endif
#endif

extern std::random_device rd;

#if ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || (defined(__cplusplus) && (__cplusplus >= 201703L)))
#define CONSTEXPR constexpr
#else
#define CONSTEXPR
#endif

#define COMPILE_WITH_VOICE 0

#pragma region Warnings as errors
#pragma warning(error: 4700)
#pragma endregion

#if ((!defined(_M_FP_FAST)) || !_M_FP_FAST)
#define FLOATING_POINT_ENV_ACCESS() _Pragma("fenv_access (on)")
#else
#define FLOATING_POINT_ENV_ACCESS()
#endif

// use this macro to define _time and _clock namespaces
#define CHRONO_DEFINE_TIME_AND_CLOCK() \
	namespace _time = std::chrono; \
	using _clock = std::chrono::steady_clock; \
	using namespace std::chrono_literals;

#undef small
