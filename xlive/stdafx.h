// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//
#pragma once

#define HEAP_DEBUG 0

#define _BIND_TO_CURRENT_CRT_VERSION 1

#define VC_EXTRALEAN

//#include <WinSDKVer.h>

#define WINVER _WIN32_WINNT_VISTA
#define _WIN32_WINNT _WIN32_WINNT_VISTA
#define _WINDSOCK_DEPRECATED_NO_WARNINGS

//#include <SDKDDKVer.h>

#ifdef _DEBUG
#define _SECURE_SCL 1
#include <crtdbg.h>
#endif

#ifdef NDEBUG
#define _SECURE_SCL 0
#endif

#define TEST_N_DEF(TEST)

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

// TODO: include this only if trac
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
//#include <Objbase.h>
//#include <Shlwapi.h>
#include <set>
#include <map>
#include <mutex>
#include <queue>
#include <tuple>
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

#include "Util/log.h"
#include "xliveless.h"
#include "xlivedefs.h"

#include "Util/Memory.h"
#include "Util/curl-interface.h"

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

#define STRINGIFY(x) #x

// use this macro to define _time and _clock namespaces
#define CHRONO_DEFINE_TIME_AND_CLOCK() \
	namespace _time = std::chrono; \
	using _clock = std::chrono::steady_clock; \
	using namespace std::chrono_literals; \

#undef small
