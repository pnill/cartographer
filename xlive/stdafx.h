// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//
#pragma once

#define CARTOGRAPHER_HEAP_DEBUG 0

#if CARTOGRAPHER_HEAP_DEBUG
	#define _CRTDBG_MAP_ALLOC
#endif

#ifdef _DEBUG
#define D3D_DEBUG_INFO
#endif

#include <WinSDKVer.h>

#define WINVER 0x0601
#define _WIN32_WINNT 0x0601

#include <sdkddkver.h>

#define _WINSOCK_DEPRECATED_NO_WARNINGS
// sets the dinput/xinput version, Halo 2 uses the older 9.1.0 version and DirectInput 0x0800
#define XINPUT_USE_9_1_0
#define DIRECTINPUT_VERSION 0x0800

#define TEST_N_DEF(TEST)

#define _USE_MATH_DEFINES
#include <math.h>
#include <immintrin.h>

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <wincrypt.h>
#include <mmsystem.h>
#include <windef.h>
#include <ShlObj_core.h>
#include <psapi.h>
#include <TlHelp32.h>
#include <iphlpapi.h>
#include <errno.h>

// initialize GUIDs locally
#include <initguid.h>

// game input
#include <xinput.h>
#include <dinput.h>

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <mutex>
#include <string>
#include <vector>
#include <random>
#include <numeric>
#include <unordered_map>
#include <chrono>

#include <d3d9.h>
#include <d3dx9.h>
#include <d3d12.h>

#include <dxgi1_4.h>

#include "xliveless.h"
#include "xlivedefs.h"

// cartographer includes

#include "H2MOD/Modules/h2log/h2log.h"

#include "version.h"

#include "CartographerDllConf.h"

#include "Util/curl-interface.h"

// blam includes

#include "cseries/cseries.h"
#include "cseries/cseries_errors.h"
#include "cseries/cseries_system_memory.h"
#include "cseries/cseries_windows.h"

#include "math/crypto_windows.h"
#include "math/integer_math.h"
#include "math/real_math.h"
#include "memory/static_arrays.h"

#include "tag_files/tag_groups.h"

/* hooking */

#include "Util/Hooks/Hook.h"
#include "Util/Memory.h"

#include <contrib/minizip/zip.h>

#pragma comment(lib, "IPHLPAPI.lib")

static_assert(EXECUTABLE_TYPE >= 0 && EXECUTABLE_TYPE <= 7, "EXECUTABLE_TYPE VALUE BELOW 0 OR EXCEEDS 7");
static_assert(EXECUTABLE_VERSION > 0 && EXECUTABLE_VERSION < 65535, "EXECUTABLE_VERSION VALUE EXCEEDS 65534");
static_assert(COMPATIBLE_VERSION > 0 && COMPATIBLE_VERSION < 65535, "COMPATIBLE_VERSION VALUE EXCEEDS 65534");

extern std::random_device rd;

// use this macro to define _time and _clock namespaces
#define STD_CHRONO_DEFINE_TIME_AND_CLOCK(_time_name, _clock_name) \
	namespace _time_name = std::chrono; \
	using _clock_name = std::chrono::steady_clock; \
	using namespace std::chrono_literals;
