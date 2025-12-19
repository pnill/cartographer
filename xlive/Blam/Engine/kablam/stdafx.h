#pragma once

/* windows */

#ifdef _WIN32

#pragma comment(lib, "Rpcrt4.lib")

// Windows 2000 is the current min requirement
#define WINVER 0x0500
#define _WIN32_WINNT 0x0500

#include <assert.h>
#include <Windows.h>

#endif

/* c runtime */

#include <stdio.h>

/* general includes */

#include "cseries.h"

/* resources */

#include "resource.h"
