// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//
#pragma once


#define _BIND_TO_CURRENT_CRT_VERSION 1

#define VC_EXTRALEAN

//#include <WinSDKVer.h>

#define _WIN32_WINNT _WIN32_WINNT_VISTA

//#include <SDKDDKVer.h>

#ifdef _DEBUG
#define _SECURE_SCL 1
#include <crtdbg.h>
#endif

#ifdef NDEBUG
#define _SECURE_SCL 0
#endif

#ifndef UNICODE
#define UNICODE
#endif

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

extern std::random_device rd;

#define COMPILE_WITH_VOICE 0

#undef small
