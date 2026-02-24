#pragma once
#include "CartographerDllConf.h"

/* c runtime */

#include <immintrin.h>

#include <string>
#include <type_traits>

/* windows */

#ifdef _WIN32

#define _USE_MATH_DEFINES
#include <math.h>

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <Ws2tcpip.h>
#include <iphlpapi.h>
#include <shellapi.h>

// Use Xinput 9.1.0 and Dinput 0x0800
#define XINPUT_USE_9_1_0
#define DIRECTINPUT_VERSION 0x0800

#include <xinput.h>
#include <dinput.h>

#pragma comment(lib, "IPHLPAPI.lib")

#endif

/* 3rd party */

#include <xlivedefs.h>

/* game includes */

#include "cseries/cseries.h"
#include "cseries/cseries_errors.h"
#include "cseries/cseries_system_memory.h"

#include "math/integer_math.h"
#include "math/real_math.h"
#include "memory/static_arrays.h"
#include "tag_files/tag_groups.h"

#include "H2MOD/Utils/curl_constants.h"
#include "Util/Hooks/Hook.h"
#include "Util/Memory.h"

#include "interface/user_interface_widget_window.h"
