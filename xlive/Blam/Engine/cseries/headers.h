#pragma once

// Build with minimum required components
#ifdef RELEASE_MIN_CONFIGURATION

#define SPDLOG_DISABLED 1
#define IMGUI_DISABLE 1

#endif

// Default build:
#ifdef RELEASE_CONFIGURATION

#define TERMINAL_ENABLED 1				// Not originally enabled (We want this in cartographer's default build)
#define ERRORS_ENABLED 1				// Not originally enabled (We want this in cartographer's default build)
#define EVENTS_ENABLED 1				// Not originally enabled (We want this in cartographer's default build)
#define OBJECT_OVERRIDE_ENABLED 1		// Not originally enabled (We want this in cartographer's default build)
#define OBJECT_DEBUG 1					// Not originally enabled (We want this in cartographer's default build)
#define UI_DEBUG 1						// Not originally enabled (We want this in cartographer's default build)
#define HS_COMPILER_ENABLED 1			// Not originally enabled (We want this in cartographer's default build)

#endif

// Release with profiling code
#ifdef PROFILE_CONFIGURATION

#define PROFILE_ENABLED 1
#define RASTERIZER_PROFILE_ENABLED 1

#define SPDLOG_DISABLED 1

#endif

// Debug features but with optimization
#ifdef PLAY_CONFIGURATION

#define TERMINAL_ENABLED 1
#define ERRORS_ENABLED 1
#define EVENTS_ENABLED 1
#define OBJECT_OVERRIDE_ENABLED 1
#define OBJECT_DEBUG 1
#define UI_DEBUG 1
#define HS_COMPILER_ENABLED 1
#define ASSERTS_ENABLED 1
#define VALIDATE_REAL_ENABLED 1
#define DEBUG_MENU_ENABLED 1
#define PROFILE_ENABLED 1

#endif

// Debug but with inlined functions and some missing debug features
#ifdef DEBUG_CONFIGURATION

#define TERMINAL_ENABLED 1
#define ASSERTS_ENABLED 1
#define EVENTS_ENABLED 1
#define ERRORS_ENABLED 1
#define DEBUG_MEMORY_ENABLED 1
#define VALIDATE_REAL_ENABLED 1
#define OBJECT_OVERRIDE_ENABLED 1
#define OBJECT_DEBUG 1
#define UI_DEBUG 1
#define DEBUG_MENU_ENABLED 1
#define HS_COMPILER_ENABLED 1
#define PROFILE_ENABLED 1
#define RASTERIZER_PROFILE_ENABLED 1

#endif

// Full debug build
#ifdef SYMBOLS_CONFIGURATION

#define TERMINAL_ENABLED 1
#define ASSERTS_ENABLED 1
#define EVENTS_ENABLED 1
#define ERRORS_ENABLED 1
#define DEBUG_MEMORY_ENABLED 1
#define VALIDATE_REAL_ENABLED 1
#define OBJECT_OVERRIDE_ENABLED 1
#define OBJECT_DEBUG 1
#define UI_DEBUG 1
#define DEBUG_MENU_ENABLED 1
#define HS_COMPILER_ENABLED 1
#define PROFILE_ENABLED 1
#define RASTERIZER_PROFILE_ENABLED 1

#endif

#include "cseries/cseries.h"
#include "cseries/cseries_errors.h"
#include "cseries/cseries_system_memory.h"
#include "cseries/cseries_windows.h"

#include "math/color_math.h"
#include "math/crypto_windows.h"
#include "math/integer_math.h"
#include "math/real_math.h"
#include "memory/static_arrays.h"

#include "tag_files/files_windows.h"
#include "tag_files/tag_groups.h"
#include "tag_files/tag_group_access.h"
