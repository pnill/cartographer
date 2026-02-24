#pragma once
#include "CartographerDllConf.h"

/* c runtime */

#include <stdio.h>
#include <stdlib.h>

#include <atomic>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>

/* windows */

#ifdef _WIN32

#include <WinSDKVer.h>

#define WINVER 0x0601
#define _WIN32_WINNT 0x0601

#include <sdkddkver.h>

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#define _USE_MATH_DEFINES
#include <math.h>

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <Ws2tcpip.h>
#include <shlwapi.h>
#include <iphlpapi.h>
#include <mmeapi.h>
#include <timeapi.h>

#include <d3d9.h>

#pragma comment(lib, "IPHLPAPI.lib")

#endif

/* 3rd party */

#include <curl/curl.h>

#include <xliveless.h>
#include <xlivedefs.h>

/* game includes */

#include "H2MOD/Modules/h2log/h2log.h"
#include "H2MOD/Utils/curl-interface.h"
#include "H2MOD/Utils/curl_constants.h"

// TODO: remove these from here
#include "cseries/cseries.h"
#include "memory/static_arrays.h"

// use this macro to define _time and _clock namespaces
#define STD_CHRONO_DEFINE_TIME_AND_CLOCK(_time_name, _clock_name) \
	namespace _time_name = std::chrono; \
	using _clock_name = std::chrono::steady_clock; \
	using namespace std::chrono_literals;
