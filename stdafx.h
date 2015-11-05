// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//
#pragma once


#define _BIND_TO_CURRENT_CRT_VERSION 1

#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#define STRICT

//#include <WinSDKVer.h>

#define _WIN32_WINNT _WIN32_WINNT_WINXP

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

#ifndef _UNICODE
#define _UNICODE
#endif

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

// TODO: include this only if trac
#include <stdio.h>
#include <stdlib.h>
#include <Winsock2.h>
//#include <Objbase.h>
//#include <Shlwapi.h>

// #define XLIVELESS_EXPORTS
#include "xliveless.h"
#include "xlivedefs.h"
